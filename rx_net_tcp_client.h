#ifndef RX_NET_TCP_CLIENT_H
#define RX_NET_TCP_CLIENT_H

#include "rx_net_tcp_session.h"

namespace rx
{
    //-----------------------------------------------------
    //同步net模式的tcp客户端功能封装
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
            get_tcp_sesncfg(super_t::sesncfg).logger.warn("%sDST<%s:%u>",(const char*)osmsg,ip_r,m_dst.port());
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

            //进行真正的连接动作
            int r=sock::connect(super_t::m_sock,m_dst,timeout_us);
            if (r==1)
            {//连接成功了,给出外部回调通知
                if (get_tcp_sesncfg(super_t::sesncfg).on_connect.is_valid())
                    get_tcp_sesncfg(super_t::sesncfg).on_connect(super_t::m_sock,*this);
                return true;
            }
            if (r==0)
                return m_err_close("socket connect timeout.");

            return m_err_close("socket connect error.");
        }
    };
}

#endif