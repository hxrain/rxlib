#ifndef RX_NET_TCP_SERVER_H
#define RX_NET_TCP_SERVER_H

#include "rx_net_tcp_session.h"
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
    protected:
        typedef array_ft<listener_t,RX_SVRSOCK_MAX_LISTEN_PORTS>   sock_array_t;

        sock_array_t    m_socks;                            //socket监听数组
        logger_i        &m_logger;                          //日志输出接口

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
            for(uint32_t i=0;i<m_socks.capacity();++i)
            {
                if (m_socks[i].sock==bad_socket)
                    return i;
            }
            return -1;
        }
        //-------------------------------------------------
        //根据地址信息查找对应的socket信息的索引.
        //返回值:-1不存在;其他为数组索引
        uint32_t m_find_by_addr(const sock_addr_t& addr)
        {
            for(uint32_t i=0;i<m_socks.capacity();++i)
            {
                if (m_socks[i].addr==addr)
                    return i;
            }
            return -1;
        }
    public:
        //-------------------------------------------------
        tcp_svrsocks_t(logger_i& logger):m_logger(logger){}
        tcp_svrsocks_t():m_logger(make_logger_con()){}      //默认可以使用控制台作为日志记录器
        virtual ~tcp_svrsocks_t(){close();}
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

            m_socks[idx]=ss;                                //记录当前监听socket到指定的槽位
            return true;
        }
        //-------------------------------------------------
        //关闭指定的监听端口,或全部的端口
        bool close(uint16_t port=0,const char* host=NULL)
        {
            if (port==0)
            {//关闭全部的监听socket
                for(uint32_t i=0;i<m_socks.capacity();++i)
                {
                    listener_t &ss=m_socks[i];
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

            listener_t &ss=m_socks[idx];
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
            return NULL;
        }
    };

    //-----------------------------------------------------
    //多线程常规功能的tcp服务器,可完成通用的多线程服务器.
    class tcp_server_t
    {
    };
}

#endif
