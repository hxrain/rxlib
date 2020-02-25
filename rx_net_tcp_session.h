#ifndef RX_NET_TCP_SESSION_H
#define RX_NET_TCP_SESSION_H

#include "rx_net_sock_std.h"
#include "rx_net_sock_ext.h"
#include "rx_ct_error.h"
#include "rx_logger_std.h"
#include "rx_ct_delegate.h"
#include "rx_os_misc.h"

namespace rx
{
    //-----------------------------------------------------
    class tcp_session_t;
    //tcp连接或断开事件的委托类型
    typedef delegate2_t<socket_t,tcp_session_t&,void> tcp_evt_conn_t;

    //-----------------------------------------------------
    //tcp会话使用的配置参量,抽取出公共对象,节省内存.
    typedef struct tcp_sesncfg_t
    {
        uint32_t                timeout_us_rd;              //接收超时us
        uint32_t                timeout_us_wr;              //发送超时us
        uint32_t                timeout_us_conn;            //连接超时us
        logger_i               &logger;                     //错误日志记录器
        tcp_evt_conn_t          on_connect;                 //事件委托:tcp会话连接成功
        tcp_evt_conn_t          on_disconnect;              //事件委托:tcp会话连接断开
        sock::event_rw_t        on_recv;                    //事件委托:tcp会话接收到数据
        sock::event_rw_t        on_send;                    //事件委托:tcp会话发送了数据
        tcp_sesncfg_t(logger_i &log):timeout_us_rd(1000*500),timeout_us_wr(1000*500),timeout_us_conn(1000*3000),logger(log){}
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
    //同步net操作的TCP收发会话功能
    class tcp_session_t
    {
    protected:
        friend void tcp_session_bind(tcp_session_t &sesn,socket_t sock,void *usrdata);

        socket_t                m_sock;                     //通信使用的Socket
        //-------------------------------------------------
        //处理收发错误日志,并断开连接
        bool m_err_disconn(const char* tip)
        {
            os_errmsg_t osmsg(tip);                         //得到格式化后的系统错误信息描述与tip
            char addrstr[53];
            sock::addr_infos(m_sock,addrstr);               //得到通信双方地址信息
            //输出日志
            get_tcp_sesncfg(sesncfg).logger.warn("%s -> %s",(const char*)osmsg,addrstr);
            //断开连接,释放socket
            disconnect(true);
            return false;
        }

    public:
        void*           usrdata;                            //session绑定的外部用户数据
        tcp_sesncfg_t*  sesncfg;                            //session统一使用的配置参数
        //-------------------------------------------------
        tcp_session_t():m_sock(bad_socket),usrdata(NULL),sesncfg(NULL){}
        //-------------------------------------------------
        virtual ~tcp_session_t(){disconnect();}
        socket_t socket() const {return m_sock;}
        //-------------------------------------------------
        //判断当前是否处于连接中,socket是否有效.
        bool connected(){return m_sock!=bad_socket;}
        //-------------------------------------------------
        //断开连接,释放socket
        void disconnect(bool NoWait=true)
        {
            if (m_sock==bad_socket)
                return;

            if (get_tcp_sesncfg(sesncfg).on_disconnect.is_valid())
                get_tcp_sesncfg(sesncfg).on_disconnect(m_sock,*this);
            sock::close(m_sock,NoWait);
        }
        //-------------------------------------------------
        //将指定长度(size)的数据(data)发送给对端.可以额外指定发送超时等待时间.
        //返回值:ec_ok完成;其他错误,发送未完成,关闭连接:ec_uninit连接断开或未初始化;ec_net_write些错误;ec_net_timeout发送超时;
        error_t write(const void *data,uint32_t size,uint32_t timeout_us=0)
        {
            if (m_sock==bad_socket)
                return ec_uninit;

            if (!timeout_us)
                timeout_us=get_tcp_sesncfg(sesncfg).timeout_us_wr;

            //尝试确定发送事件的委托
            sock::event_rw_t *evt=get_tcp_sesncfg(sesncfg).on_send.is_valid()?&get_tcp_sesncfg(sesncfg).on_send:NULL;

            //进行真正的循环发送
            int32_t rc=sock::write_loop(m_sock,data,size,timeout_us,evt,this);
            if (rc>0)
                return ec_ok;

            if (rc==0)
            {
                m_err_disconn("net send timeout.");
                return ec_net_timeout;
            }

            m_err_disconn("net send error.");
            return ec_net_write;
        }
        //-------------------------------------------------
        //要求必须读取len个字节的数据到buff中
        //返回值:ec_ok完成;其他错误,发送未完成,关闭连接:ec_uninit连接断开或未初始化;ec_net_read错误;ec_net_disconn对方已经断开;ec_net_timeout接收超时;
        error_t read(void* buff,uint32_t len,uint32_t timeout_us=0)
        {
            if (m_sock==bad_socket)
                return ec_uninit;

            if (!timeout_us)
                timeout_us=get_tcp_sesncfg(sesncfg).timeout_us_rd;

            //尝试确定接收事件的委托
            sock::event_rw_t *evt=get_tcp_sesncfg(sesncfg).on_recv.is_valid()?&get_tcp_sesncfg(sesncfg).on_recv:NULL;

            //进行真正的循环接收
            int32_t rc=sock::read_loop(m_sock,(uint8_t*)buff,len,true,timeout_us,evt,this);
            if (rc>0)
            {
                if ((uint32_t)rc==len)
                    return ec_ok;
                m_err_disconn("net recv timeout.");
                return ec_net_timeout;
            }

            if (rc==0)
            {
                m_err_disconn("net peer closed.");
                return ec_net_disconn;
            }

            m_err_disconn("net recv error.");
            return ec_net_read;
        }
        //-------------------------------------------------
        //要求读取最多len个字节的数据到buff中,直到超时或断开
        //返回值:实际接收长度(出错时连接会断开,连接未断开则为超时)
        uint32_t try_read(void* buff,uint32_t len,uint32_t timeout_us=0)
        {
            if (m_sock==bad_socket)
                return 0;

            if (!timeout_us)
                timeout_us=get_tcp_sesncfg(sesncfg).timeout_us_rd;

            //尝试确定接收事件的委托
            sock::event_rw_t *evt=get_tcp_sesncfg(sesncfg).on_recv.is_valid()?&get_tcp_sesncfg(sesncfg).on_recv:NULL;

            //进行真正的循环接收
            sock::recv_buff_i ri((uint8_t*)buff,len,false);
            int r=sock::read_loop(m_sock,ri,timeout_us,evt,this);
            if (r==0)
                m_err_disconn("net peer closed.");
            else if (r<0)
                m_err_disconn("net recv error.");
            return ri.size();
        }
        //-------------------------------------------------
        //要求读取最多len个字节的数据到buff中,直到收到tag,或超时或断开;入参tagsize告知tag的长度,出参告知tag在buff中的开始位置索引,-1不存在;
        //返回值:实际接收长度(出错时连接会断开,连接未断开则为超时);
        uint32_t try_read(void* buff,uint32_t len,void* tag,uint32_t &tagsize,uint32_t timeout_us=0)
        {
            if (m_sock==bad_socket)
                return 0;

            if (!timeout_us)
                timeout_us=get_tcp_sesncfg(sesncfg).timeout_us_rd;

            //尝试确定接收事件的委托
            sock::event_rw_t *evt=get_tcp_sesncfg(sesncfg).on_recv.is_valid()?&get_tcp_sesncfg(sesncfg).on_recv:NULL;

            //进行真正的循环接收
            sock::recv_tag_i ri((uint8_t*)buff,len,false);
            ri.tag=(uint8_t*)tag;
            ri.tagsize=tagsize;
            int r=sock::read_loop(m_sock,ri,timeout_us,evt,this);
            if (r==0)
                m_err_disconn("net peer closed.");
            else if (r<0)
                m_err_disconn("net recv error.");
            //记录tag出现的位置
            tagsize=ri.tagpos;
            return ri.size();
        }
    };
    //绑定sock到指定的session对象
    inline void tcp_session_bind(tcp_session_t &sesn,socket_t sock,void *usrdata=NULL)
    {
        rx_assert(sesn.m_sock==bad_socket);
        rx_assert(sesn.usrdata==NULL);
        sesn.m_sock=sock;
        sesn.usrdata=usrdata;
        if (get_tcp_sesncfg(sesn.sesncfg).on_connect.is_valid())
            get_tcp_sesncfg(sesn.sesncfg).on_connect(sock,sesn);
    }
}

#endif
