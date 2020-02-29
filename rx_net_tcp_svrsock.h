#ifndef RX_NET_TCP_SVRSOCK_H
#define RX_NET_TCP_SVRSOCK_H

#include "rx_net_tcp_client.h"
#include "rx_dtl_array.h"

namespace rx
{
    //tcp服务端可同时监听端口数的最大值
    #ifndef RX_SVRSOCK_MAX_LISTEN_PORTS
        #define RX_SVRSOCK_MAX_LISTEN_PORTS     8
    #endif

    //-----------------------------------------------------
    //封装最简单基础的tcp服务端底层功能,可以完成简单服务.
    class tcp_svrsocks_t
    {
    public:
        //-------------------------------------------------
        typedef struct listener_t                           //本地监听使用的socket与绑定的端口地址信息:监听者.
        {
            socket_t        sock;
            sock_addr_t     addr;
            listener_t():sock(bad_socket){}
        }listener_t;
        static const uint32_t max_listens=RX_SVRSOCK_MAX_LISTEN_PORTS<FD_SETSIZE?RX_SVRSOCK_MAX_LISTEN_PORTS:FD_SETSIZE;
    protected:
        typedef array_ft<listener_t,max_listens>        listener_array_t;
        typedef array_stack_ft<listener_t*,max_listens> listener_ptr_stack_t;

        listener_array_t        m_listeners;                //socket监听数组
        logger_i                &m_logger;                  //日志输出接口
        listener_ptr_stack_t    m_working;                  //存放工作中的监听者指针的栈,当作链表使用

        //-------------------------------------------------
        //记录错误信息到日志
        bool m_err(const char* tip,...)
        {
            char tmp_tip[128];

            va_list ap;
            va_start(ap,tip);
            st::vsnprintf(tmp_tip,sizeof(tmp_tip),tip,ap);
            va_end(ap);

            os_errmsg_t errmsg(tmp_tip);
            m_logger.warn("%s",(const char*)errmsg);
            return false;
        }
        //-------------------------------------------------
        //查找空的socket信息的索引.
        //返回值:-1不存在;其他为数组索引
        uint32_t m_find_by_empty()
        {
            for(uint32_t i=0;i<m_listeners.capacity();++i)
            {
                if (m_listeners[i].sock==bad_socket)
                    return i;
            }
            return -1;
        }
        //-------------------------------------------------
        //根据地址信息查找对应的socket信息的索引.
        //返回值:-1不存在;其他为数组索引
        uint32_t m_find_by_addr(const sock_addr_t& addr)
        {
            for(uint32_t i=0;i<m_listeners.capacity();++i)
            {
                if (m_listeners[i].addr==addr)
                    return i;
            }
            return -1;
        }
    public:
        //-------------------------------------------------
        tcp_svrsocks_t(logger_i& logger):m_logger(logger){}
        tcp_svrsocks_t():m_logger(make_logger_con()){}      //默认可以使用控制台作为日志记录器
        virtual ~tcp_svrsocks_t(){close();}
        logger_i& logger(){return m_logger;}
        //-------------------------------------------------
        //尝试监听本地端口,允许只监听指定的本机地址;设定socket上可建立链接的最大数量.
        bool open(uint16_t port,const char* host=NULL,uint32_t backlogs=1024)
        {
            uint32_t idx=m_find_by_empty();                 //先查找可用槽位
            if (idx==uint32_t(-1))
                return m_err("tcp svrsocks is full.");

            listener_t ss;
            if (!ss.addr.set_addr(host,port))               //先转换网络地址
                return m_err("invalid local listen addr <%s:%u>.",not_empty(host),port);

            if (m_find_by_addr(ss.addr)!=(uint32_t)-1)      //查找网络地址是否已经被监听
                return m_err("local addr already exists. can't listen again.");

            ss.sock=sock::create(true);                     //创建socket
            if (ss.sock==bad_socket)
                return m_err("can't create tcp socket.");

            if (!sock::bind(ss.sock,ss.addr))               //绑定socket到指定的端口
            {
                m_err("can't bind local addr <%s:%u>.",not_empty(host),port);
                sock::close(ss.sock,true);
                return false;
            }

            if (!sock::listen(ss.sock,min((uint32_t)SOMAXCONN,backlogs)))//真正开启监听
            {
                m_err("can't listen local addr <%s:%u>.",not_empty(host),port);
                sock::close(ss.sock,true);
                return false;
            }

            m_listeners[idx]=ss;                            //记录当前监听socket到指定的槽位
            return true;
        }
        //-------------------------------------------------
        //关闭指定的监听端口,或全部的端口
        bool close(uint16_t port=0,const char* host=NULL)
        {
            if (port==0)
            {//关闭全部的监听socket
                for(uint32_t i=0;i<m_listeners.capacity();++i)
                {
                    listener_t &ss=m_listeners[i];
                    if (ss.sock!=bad_socket)
                    {
                        sock::close(ss.sock,true);
                        ss.addr.clear();
                    }
                }
                return true;
            }

            sock_addr_t sa;
            if (!sa.set_addr(host,port))                    //转换网络地址
                return m_err("invalid local listen addr <%s:%u>.",not_empty(host),port);

            uint32_t idx=m_find_by_addr(sa);                //查找网络地址对应的socket信息索引
            if (idx==(uint32_t)-1)
                return false;

            listener_t &ss=m_listeners[idx];
            if (ss.sock!=bad_socket)                        //关闭指定监听者
            {
                sock::close(ss.sock,true);
                ss.addr.clear();
                return true;
            }
            return false;
        }
        //-------------------------------------------------
        //进行接受链接的单步操作,可作为外部事件循环的底层基础.
        //出参:新连接的会话socket,新连接的对端地址指针.
        //返回值:NULL没有新连接建立;其他为建立新连接的监听者指针
        listener_t* step(socket_t &new_sock,sock_addr_t *peer_addr=NULL,uint32_t timeout_us=1000)
        {
            if (m_working.size()==0)
            {//不存在待处理的监听者,需要重新监听一轮了
                sock_sets sets;
                for(uint32_t i=0;i<m_listeners.capacity();++i)
                {//将监听socket放入socket集合
                    listener_t &ss=m_listeners[i];
                    if (ss.sock!=bad_socket)
                        sets.push(ss.sock);
                }
                if (sets.size())
                {//在socket集合上等待读取事件(判断是否有新连接到达)
                    int r=sock::select(sets,timeout_us);
                    if (r<0)
                    {//等待时出错了
                        m_err("tcp listen select() error.");
                        return NULL;
                    }
                    if (r==0)
                        return NULL;                        //等待超时,没有新连接

                    for(uint32_t i=0;i<m_listeners.capacity();++i)
                    {//现在有新连接到达了,需要分辨是发生哪个监听者上.
                        listener_t &ss=m_listeners[i];
                        if (ss.sock!=bad_socket&&sets.contain(ss.sock))
                            m_working.push_back(&ss);       //记录监听者
                    }
                }
            }

            if (m_working.size())
            {//有新连接到达了,需要处理监听者
                sock_addr_t tmp_addr;
                if (peer_addr==NULL)
                    peer_addr=&tmp_addr;

                listener_t *ss=*m_working.rbegin();         //取出当前待处理的监听者
                rx_assert(ss!=NULL);
                new_sock=sock::accept(ss->sock,*peer_addr); //在当前监听socket上进行新连接的接收
                m_working.pop_back();                       //重要:不管成败,当前监听者都要放弃了

                if (new_sock==bad_socket)
                    return NULL;
                return ss;                                  //新连接建立完成,告知是哪个监听者
            }

            return NULL;
        }
    };

    //-----------------------------------------------------
    //封装一个简单的用于测试的tcp回音服务器(用最少的代码,无动态内存分配,实现客户端发什么就回应什么,同时也是演示相关socket功能的使用)
    class tcp_echo_svr_t
    {
        typedef array_ft<tcp_session_t,8> session_array_t;  //限定最大并发会话数量

        tcp_svrsocks_t      m_svr;                          //用于接受新连接的socket服务端功能
        session_array_t     m_sessions;                     //当前活动会话数组
        uint32_t            m_actives;                      //活跃会话数量
        //-------------------------------------------------
        //查找遇到的第一个空槽位索引;-1未找到.
        uint32_t m_find_first_empty()
        {
            for(uint32_t i=0;i<m_sessions.capacity();++i)
            {
                if (!m_sessions[i].connected())
                    return i;
            }
            return -1;
        }
    public:
        //-------------------------------------------------
        tcp_echo_svr_t():m_actives(0){}
        tcp_echo_svr_t(logger_i& l):m_svr(l),m_actives(0){}
        ~tcp_echo_svr_t(){uninit();}
        logger_i& logger(){return m_svr.logger();}
        //-------------------------------------------------
        //打开两个端口进行监听
        bool init(uint16_t port1=45601,uint16_t port2=45602)
        {
            uninit();
            if (!m_svr.open(port1,NULL,m_sessions.capacity()*2)||
                !m_svr.open(port2,NULL,m_sessions.capacity()*2))
            {
                m_svr.close();
                return false;
            }
            return true;
        }
        //-------------------------------------------------
        //解除echo服务器
        void uninit()
        {
            m_svr.close();
            for(uint32_t i=0;i<m_actives;++i)
            {
                tcp_session_t &ss=m_sessions[i];
                if (ss.connected())
                    ss.disconnect();
            }
            m_actives=0;
        }
        //-------------------------------------------------
        //服务端单线程功能驱动方法,完成新连接建立,处理收发应答.
        //返回值:有效动作的数量(新连接建立/收发错误连接断开/收发完成)
        uint32_t step(uint32_t timeout_us=10,uint32_t wr_timeout_us=sec2us(3))
        {
            uint32_t ac=0;

            if (m_actives<m_sessions.capacity())
            {//还有空槽位可以承载新会话
                uint32_t idx=m_find_first_empty();
                rx_assert(idx!=(uint32_t)-1);

                socket_t new_sock=bad_socket;
                sock_addr_t peer_addr;
                //驱动svrsock,进行新连接建立的处理
                tcp_svrsocks_t::listener_t *ss=m_svr.step(new_sock,&peer_addr,timeout_us);
                if (ss)
                {//新连接到达
                    ++ac;
                    char addrstr[53];
                    sock::addr_infos(new_sock,addrstr);
                    m_svr.logger().debug("accept new tcp session: %s",addrstr);
                    //初始化绑定新的会话
                    tcp_session_bind(m_sessions[idx],new_sock);
                    ++m_actives;
                }
            }

            uint8_t wr_buff[1024*64];                               //收发临时使用的缓冲区

            for(uint32_t i=0;i<m_sessions.capacity();++i)
            {//对已连接会话进行收发处理
                tcp_session_t &ss=m_sessions[i];
                if (!ss.connected())
                    continue;
                uint32_t rs=ss.readx(wr_buff,sizeof(wr_buff),0);    //0超时等待,快速进行接收探察
                if (rs==0)
                {
                    if (!ss.connected())
                    {//连接中断了
                        ++ac;
                        --m_actives;
                        continue;
                    }
                }
                else
                {//收到数据了,原样发送给对方
                    ++ac;
                    if (ss.write(wr_buff,rs,wr_timeout_us)!=ec_ok)
                    {//发送错误,连接中断了
                        --m_actives;
                        continue;
                    }
                }
            }
            return ac;
        }
    };

    //-----------------------------------------------------
    //多线程常规功能的tcp服务器,可完成通用的多线程服务器.
    class tcp_server_t
    {
    };
}

#endif
