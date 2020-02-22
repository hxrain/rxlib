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
    //tcp连接或断开事件的委托类型
    typedef delegate2_t<socket_t,void*,void> tcp_evt_conn_t;

    //-----------------------------------------------------
    //tcp会话使用的配置参量,抽取出公共对象,节省内存.
    class tcp_sesncfg_t
    {
    public:
        uint32_t                timeout_us_rd;              //接收超时us
        uint32_t                timeout_us_wr;              //发送超时us
        uint32_t                timeout_us_conn;            //连接超时us
        logger_i                logger;                     //错误日志记录器
        tcp_evt_conn_t          on_connect;                 //事件委托:tcp会话连接成功
        tcp_evt_conn_t          on_disconnect;              //事件委托:tcp会话连接断开
        sock::event_rw_t        on_recv;                    //事件委托:tcp会话接收到数据
        sock::event_rw_t        on_send;                    //事件委托:tcp会话发送了数据
        tcp_sesncfg_t():timeout_us_rd(1000*500),timeout_us_wr(1000*500),timeout_us_conn(1000*3000){}
    };

    //-----------------------------------------------------
    //同步net操作的TCP收发会话功能
    class tcp_session_t
    {
    protected:
        socket_t                m_sock;                     //通信使用的Socket
        tcp_sesncfg_t          &m_cfg;                      //通信使用的配置参数
        void*                   m_usrdata;                  //与当前会话绑定的外部用户数据
        //-------------------------------------------------
        //处理收发错误日志,并断开连接
        void m_err_disconn(const char* tip)
        {
            os_errmsg_t osmsg;
            const char* msg=osmsg.msg(tip);                 //得到格式化后的系统错误信息描述与tip

            ip_str_t ip_l,ip_r;
            uint16_t port_l,port_r;
            sock::addr_infos(m_sock,ip_l,port_l,ip_r,port_r);//得到通信双方地址信息

            //输出日志
            m_cfg.logger.warn("%s ->LOC<%s:%u>RMT<%s:%u>",msg,ip_l,port_l,ip_r,port_r);
            //断开连接,释放socket
            disconnect(true);
        }

    public:
        //-------------------------------------------------
        tcp_session_t(tcp_sesncfg_t& cfg,void* usrdata=NULL):m_sock(bad_socket),m_cfg(cfg),m_usrdata(usrdata){}
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
            if (m_cfg.on_disconnect.is_valid())
                m_cfg.on_disconnect(m_sock,m_usrdata);
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
                timeout_us=m_cfg.timeout_us_wr;

            //尝试确定发送事件的委托
            sock::event_rw_t *evt=m_cfg.on_send.is_valid()?&m_cfg.on_send:NULL;

            //进行真正的循环发送
            int32_t rc=sock::write_loop(m_sock,data,size,timeout_us,evt,m_usrdata);
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
                timeout_us=m_cfg.timeout_us_rd;

            //尝试确定接收事件的委托
            sock::event_rw_t *evt=m_cfg.on_recv.is_valid()?&m_cfg.on_recv:NULL;

            //进行真正的循环接收
            int32_t rc=sock::read_loop(m_sock,(uint8_t*)buff,len,true,timeout_us,evt,m_usrdata);
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
                timeout_us=m_cfg.timeout_us_rd;

            //尝试确定接收事件的委托
            sock::event_rw_t *evt=m_cfg.on_recv.is_valid()?&m_cfg.on_recv:NULL;

            //进行真正的循环接收
            sock::recv_buff_i ri((uint8_t*)buff,len,false);
            int r=sock::read_loop(m_sock,ri,timeout_us,evt,m_usrdata);
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
                timeout_us=m_cfg.timeout_us_rd;

            //尝试确定接收事件的委托
            sock::event_rw_t *evt=m_cfg.on_recv.is_valid()?&m_cfg.on_recv:NULL;

            //进行真正的循环接收
            sock::recv_tag_i ri((uint8_t*)buff,len,false);
            ri.tag=(uint8_t*)tag;
            ri.tagsize=tagsize;
            int r=sock::read_loop(m_sock,ri,timeout_us,evt,m_usrdata);
            if (r==0)
                m_err_disconn("net peer closed.");
            else if (r<0)
                m_err_disconn("net recv error.");
            //记录tag出现的位置
            tagsize=ri.tagpos;
            return ri.size();
        }
    };
}

#endif
