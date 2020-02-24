#ifndef RX_NET_TCP_CLIENT_H
#define RX_NET_TCP_CLIENT_H

#include "rx_net_tcp_session.h"

namespace rx
{
    //-----------------------------------------------------
    //ͬ��netģʽ��tcp�ͻ��˹��ܷ�װ
    class tcp_client_t:public tcp_session_t
    {
        typedef tcp_session_t super_t;
        sock_addr_t     m_dst;
        //-------------------------------------------------
        //�����ӽ���ǰ�����˴���,�ر�socket,���ø������ӶϿ��Ļص�
        bool m_err_close(const char* tip)
        {
            ip_str_t ip_r;
            m_dst.ip_str(ip_r);
            os_errmsg_t osmsg(tip);
            super_t::m_cfg.logger.warn("%s ->DST<%s:%u>",(const char*)osmsg,ip_r,m_dst.port());
            sock::close(super_t::m_sock,true);
            return false;
        }
    public:
        //-------------------------------------------------
        tcp_client_t(tcp_sesncfg_t& cfg,void* usrdata=NULL):tcp_session_t(cfg,usrdata){}
        //-------------------------------------------------
        //���ô����ӵ�Ŀ���ַ
        bool dst_addr(const char* host,uint16_t port)
        {
            return m_dst.set_addr(host,port);
        }
        //-------------------------------------------------
        //����socket������Ŀ���ַ,ʹ�ñ��ض˿�����,�ɿ������ӳ�ʱus
        bool connect(int LocalPort=0,int timeout_us=1000*1000*10)
        {//�������,�ȶϿ�����.֮���ʼ����Ҫ�ı���,����������
            super_t::disconnect(true);

            super_t::m_sock=sock::create(true);
            if (super_t::m_sock==bad_socket)
                return m_err_close("tcp socket create fail.");  //socket����ʧ��

            if (LocalPort)
            {//���ñ��ض˿���,��ô����Ҫ�뱾�ص�ַ���а󶨲���
                if (!sock::opt_reuse_addr(super_t::m_sock,true))
                    return m_err_close("socket SO_REUSEADDR error.");

                if (!sock::bind(super_t::m_sock,LocalPort))
                    return m_err_close("socket bind local port error.");
            }

            //�������������Ӷ���
            int r=sock::connect(super_t::m_sock,m_dst,timeout_us);
            if (r==1)
            {//���ӳɹ���,�����ⲿ�ص�֪ͨ
                if (m_cfg.on_connect.is_valid())
                    m_cfg.on_connect(super_t::m_sock,this);
                return true;
            }
            if (r==0)
                return m_err_close("socket connect timeout.");

            return m_err_close("socket connect error.");
        }
    };
}

#endif