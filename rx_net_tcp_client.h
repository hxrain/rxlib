#ifndef RX_NET_TCP_CLIENT_H
#define RX_NET_TCP_CLIENT_H

#include "rx_net_sock_std.h"
#include "rx_ct_error.h"
#include "rx_logger_std.h"
#include "rx_ct_delegate.h"
#include "rx_os_misc.h"

namespace rx
{
    //-----------------------------------------------------
    class tcp_session_t;
    //tcp连接事件的委托类型:tcp会话对象;会话是否为tcp客户端;事件没有返回值
    typedef delegate2_t<tcp_session_t&,bool,void> tcp_evt_conn_t;
    //tcp断开事件的委托类型:tcp会话对象;是否由错误引发会话断开;事件没有返回值
    typedef delegate2_t<tcp_session_t&,bool,void> tcp_evt_disconn_t;

    //-----------------------------------------------------
    //tcp会话使用的配置参量,抽取出公共对象,节省内存.
    typedef struct tcp_sesncfg_t
    {
        uint32_t                timeout_us_rd;              //接收超时us
        uint32_t                timeout_us_wr;              //发送超时us
        uint32_t                timeout_us_conn;            //连接超时us
        logger_i               &logger;                     //错误日志记录器
        tcp_evt_conn_t          on_connect;                 //事件委托:tcp会话连接成功
        tcp_evt_disconn_t       on_disconnect;              //事件委托:tcp会话连接断开
        sock::event_rw_t        on_recv;                    //事件委托:tcp会话接收到数据
        sock::event_rw_t        on_send;                    //事件委托:tcp会话发送了数据
        tcp_sesncfg_t(logger_i &log):timeout_us_rd(ms2us(500)),timeout_us_wr(ms2us(500)),timeout_us_conn(sec2us(3)),logger(log){}
    }
    tcp_sesncfg_t;

    //-----------------------------------------------------
    //访问tcp会话配置,可以给出全局默认值,便于使用
    inline tcp_sesncfg_t& get_tcp_sesncfg(tcp_sesncfg_t* ptr=NULL)
    {
        static tcp_sesncfg_t cfg(make_logger_con());
        if (ptr==NULL)
            return cfg;
        return *ptr;
    }

    //-----------------------------------------------------
    //同步net操作的TCP收发会话功能(可用于客户端与服务端)
    class tcp_session_t
    {
    protected:
        friend void tcp_session_bind(tcp_session_t &sesn,socket_t sock,void *usrdata);

        socket_t                m_sock;                     //通信使用的Socket
        //-------------------------------------------------
        //处理收发错误日志,并断开连接
        bool m_err_disconn(const char* tip,bool is_error=true)
        {
            os_errmsg_t osmsg(tip);                         //得到格式化后的系统错误信息描述与tip
            char addrstr[80];
            sock::addr_infos(m_sock,addrstr);               //得到通信双方地址信息
            tcp_sesncfg_t &sc=get_tcp_sesncfg(sesncfg);     //获取会话配置信息
            //输出日志
            if (is_error)
                sc.logger.warn("%s :: %s",addrstr,(const char*)osmsg);
            else
                sc.logger.debug("%s :: %s",addrstr,(const char*)osmsg);
            //断开连接,释放socket,并告知是由于错误而引起的中断
            m_disconnect(true,is_error);
            return false;
        }
        //-------------------------------------------------
        //处理会话连接的断开事件
        void m_disconnect(bool NoWait,bool HaveErr)
        {
            if (m_sock==bad_socket)
                return;
            tcp_sesncfg_t &sc=get_tcp_sesncfg(sesncfg);     //获取会话配置信息
            if (sc.on_disconnect.is_valid())
                sc.on_disconnect(*this,HaveErr);
            sock::close(m_sock,NoWait);
        }
    public:
        void*           usrdata;                            //session绑定的外部用户数据
        tcp_sesncfg_t*  sesncfg;                            //session统一使用的配置参数
        //-------------------------------------------------
        tcp_session_t():m_sock(bad_socket),usrdata(NULL),sesncfg(NULL){}
        //-------------------------------------------------
        virtual ~tcp_session_t(){disconnect();}
        socket_t socket() const {return m_sock;}
        logger_i& logger(){return get_tcp_sesncfg(sesncfg).logger;}
        //-------------------------------------------------
        //判断当前是否处于连接中,socket是否有效.
        bool connected(){return m_sock!=bad_socket;}
        //-------------------------------------------------
        //断开连接,释放socket
        void disconnect(bool NoWait=true){m_disconnect(NoWait,false);}
        //-------------------------------------------------
        //将指定长度(size)的数据(data)发送给对端.可以额外指定发送超时等待时间.
        //返回值:ec_ok完成;其他错误,发送未完成,关闭连接:ec_uninit连接断开或未初始化;ec_net_write些错误;ec_net_timeout发送超时;
        error_t write(const void *data,uint32_t size,uint32_t timeout_us=(uint32_t)-1)
        {
            if (m_sock==bad_socket)
                return ec_uninit;

            tcp_sesncfg_t &sc=get_tcp_sesncfg(sesncfg);     //获取会话配置信息
            if (timeout_us==(uint32_t)-1)
                timeout_us=sc.timeout_us_wr;

            //尝试确定发送事件的委托
            sock::event_rw_t *evt=sc.on_send.is_valid()?&sc.on_send:NULL;
            //进行真正的循环发送
            int32_t rc=sock::write_loop(m_sock,data,size,timeout_us,evt,this);
            if (rc>0)
                return ec_ok;

            if (rc==0)
            {
                m_err_disconn("tcp_session_t::write() net send timeout.");
                return ec_net_timeout;
            }

            m_err_disconn("tcp_session_t::write() net send error.");
            return ec_net_write;
        }
        //-------------------------------------------------
        //要求必须读取len个字节的数据到buff中(默认超时使用会话配置)
        //返回值:ec_ok完成;其他错误,发送未完成,关闭连接:ec_uninit连接断开或未初始化;ec_net_read错误;ec_net_disconn对方已经断开;ec_net_timeout接收超时;
        error_t read(void* buff,uint32_t len,uint32_t timeout_us=(uint32_t)-1)
        {
            if (m_sock==bad_socket)
                return ec_uninit;

            tcp_sesncfg_t &sc=get_tcp_sesncfg(sesncfg);     //获取会话配置信息
            if (timeout_us==(uint32_t)-1)
                timeout_us=sc.timeout_us_rd;

            //尝试确定接收事件的委托
            sock::event_rw_t *evt=sc.on_recv.is_valid()?&sc.on_recv:NULL;

            //进行真正的循环接收
            uint32_t recved=len;
            int32_t rc=sock::read_loop(m_sock,(uint8_t*)buff,recved,true,timeout_us,evt,this);
            if (rc<0)
            {//出错了
                m_err_disconn("tcp_session_t::read() net recv error.");
                return ec_net_read;
            }
            if (rc==1)
            {//超时了
                if ((uint32_t)rc==len)
                    return ec_ok;
                m_err_disconn("tcp_session_t::read() net recv timeout.");
                return ec_net_timeout;
            }
            if (rc==0)
            {//连接断开了
                m_err_disconn("tcp_session_t::read() net peer closed.",false);
                return ec_net_disconn;
            }
            if (recved<len)
            {//没有接收完整
                m_err_disconn("tcp_session_t::read() net recv fail.");
                return ec_net_read;
            }
            return ec_ok;
        }
        //-------------------------------------------------
        //要求读取最多len个字节的数据到buff中,直到超时或断开(默认超时使用会话配置)
        //返回值:实际接收长度(连接断开则出错了,连接未断开则为超时)
        uint32_t readx(void* buff,uint32_t len,uint32_t timeout_us=(uint32_t)-1)
        {
            if (m_sock==bad_socket)
                return 0;

            tcp_sesncfg_t &sc=get_tcp_sesncfg(sesncfg);     //获取会话配置信息
            if (timeout_us==(uint32_t)-1)
                timeout_us=sc.timeout_us_rd;

            //尝试确定接收事件的委托
            sock::event_rw_t *evt=sc.on_recv.is_valid()?&sc.on_recv:NULL;

            //进行真正的循环接收
            sock::recv_buff_i ri((uint8_t*)buff,len,false);
            int r=sock::read_loop(m_sock,ri,timeout_us,evt,this);
            if (r==0)
                m_err_disconn("tcp_session_t::readx() net peer closed.",false);
            else if (r<0)
                m_err_disconn("tcp_session_t::readx() net recv error.");
            return ri.size();
        }
        //-------------------------------------------------
        //要求读取最多len个字节的数据到buff中,直到收到tag,或超时或断开(默认超时使用会话配置);入参tagsize告知tag的长度,出参告知tag在buff中的开始位置索引,-1不存在;
        //返回值:实际接收长度(连接断开则出错了,连接未断开则为超时);
        uint32_t readx(void* buff,uint32_t len,void* tag,uint32_t &tagsize,uint32_t timeout_us=0)
        {
            if (m_sock==bad_socket)
                return 0;

            tcp_sesncfg_t &sc=get_tcp_sesncfg(sesncfg);     //获取会话配置信息
            if (!timeout_us)
                timeout_us=sc.timeout_us_rd;

            //尝试确定接收事件的委托
            sock::event_rw_t *evt=sc.on_recv.is_valid()?&sc.on_recv:NULL;

            //进行真正的循环接收
            sock::recv_tag_i ri((uint8_t*)buff,len,false);
            ri.tag=(uint8_t*)tag;
            ri.tagsize=tagsize;
            int r=sock::read_loop(m_sock,ri,timeout_us,evt,this);
            if (r==0)
                m_err_disconn("tcp_session_t::readx(tag) net peer closed.",false);
            else if (r<0)
                m_err_disconn("tcp_session_t::readx(tag) net recv error.");
            //记录tag出现的位置
            tagsize=ri.tagpos;
            return ri.size();
        }
    };
    //绑定sock到指定的session对象,便于服务端初始化会话使用.
    inline void tcp_session_bind(tcp_session_t &sesn,socket_t sock,void *usrdata=NULL)
    {
        rx_assert(sesn.m_sock==bad_socket);
        rx_assert(sesn.usrdata==NULL);
        sesn.m_sock=sock;
        sesn.usrdata=usrdata;
        tcp_sesncfg_t &sc=get_tcp_sesncfg(sesn.sesncfg);    //获取会话配置信息
        if (sc.on_connect.is_valid())
            sc.on_connect(sesn,false);                      //连接事件,发生在server的会话上,不是client
    }

    //-----------------------------------------------------
    //同步模式net功能的tcp客户端功能封装
    class tcp_client_t:public tcp_session_t
    {
        typedef tcp_session_t super_t;
        sock_addr_t     m_dst;
        //-------------------------------------------------
        //在连接建立前发生了错误,关闭socket,不用给出连接断开的回调
        bool m_err_close(const char* tip)
        {
            ip_str_t ip_r;
            m_dst.ip_str(ip_r);
            os_errmsg_t osmsg(tip);
            get_tcp_sesncfg(super_t::sesncfg).logger.warn("socket(%u)dst<%s:%u> :: %s",super_t::m_sock,(const char*)ip_r,m_dst.port(),(const char*)osmsg);
            sock::close(super_t::m_sock,true);
            return false;
        }
    public:
        //-------------------------------------------------
        //设置待连接的目标地址
        bool dst_addr(const char* host,uint16_t port)
        {
            return m_dst.set_addr(host,port);
        }
        //-------------------------------------------------
        //创建socket并连接目标地址,使用本地端口任意,可控制连接超时us
        bool connect(uint16_t LocalPort=0,uint32_t timeout_us=sec2us(3))
        {//不管如何,先断开连接.之后初始化必要的变量,并进行连接
            super_t::disconnect(true);

            super_t::m_sock=sock::create(true);
            if (super_t::m_sock==bad_socket)
                return m_err_close("tcp socket create fail.");  //socket创建失败

            if (LocalPort)
            {//设置本地端口了,那么就需要与本地地址进行绑定操作
                if (!sock::opt_reuse_addr(super_t::m_sock,true))
                    return m_err_close("socket SO_REUSEADDR error.");

                if (!sock::bind(super_t::m_sock,LocalPort))
                    return m_err_close("socket bind local port error.");
            }

            tcp_sesncfg_t &sc=get_tcp_sesncfg(sesncfg);     //获取会话配置信息
            //进行真正的连接动作
            int r=sock::connect(super_t::m_sock,m_dst,timeout_us);
            if (r==1)
            {//连接成功了,给出外部回调通知
                if (sc.on_connect.is_valid())
                    sc.on_connect(*this,true);              //触发连接事件,发生在tcp客户端
                return true;
            }
            if (r==0)
                return m_err_close("socket connect timeout.");

            return m_err_close("socket connect error.");
        }
    };
}

#endif
