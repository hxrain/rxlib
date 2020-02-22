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
    //tcp���ӻ�Ͽ��¼���ί������
    typedef delegate2_t<socket_t,void*,void> tcp_evt_conn_t;

    //-----------------------------------------------------
    //tcp�Ựʹ�õ����ò���,��ȡ����������,��ʡ�ڴ�.
    class tcp_sesncfg_t
    {
    public:
        uint32_t                timeout_us_rd;              //���ճ�ʱus
        uint32_t                timeout_us_wr;              //���ͳ�ʱus
        uint32_t                timeout_us_conn;            //���ӳ�ʱus
        logger_i                logger;                     //������־��¼��
        tcp_evt_conn_t          on_connect;                 //�¼�ί��:tcp�Ự���ӳɹ�
        tcp_evt_conn_t          on_disconnect;              //�¼�ί��:tcp�Ự���ӶϿ�
        sock::event_rw_t        on_recv;                    //�¼�ί��:tcp�Ự���յ�����
        sock::event_rw_t        on_send;                    //�¼�ί��:tcp�Ự����������
        tcp_sesncfg_t():timeout_us_rd(1000*500),timeout_us_wr(1000*500),timeout_us_conn(1000*3000){}
    };

    //-----------------------------------------------------
    //ͬ��net������TCP�շ��Ự����
    class tcp_session_t
    {
    protected:
        socket_t                m_sock;                     //ͨ��ʹ�õ�Socket
        tcp_sesncfg_t          &m_cfg;                      //ͨ��ʹ�õ����ò���
        void*                   m_usrdata;                  //�뵱ǰ�Ự�󶨵��ⲿ�û�����
        //-------------------------------------------------
        //�����շ�������־,���Ͽ�����
        void m_err_disconn(const char* tip)
        {
            os_errmsg_t osmsg;
            const char* msg=osmsg.msg(tip);                 //�õ���ʽ�����ϵͳ������Ϣ������tip

            ip_str_t ip_l,ip_r;
            uint16_t port_l,port_r;
            sock::addr_infos(m_sock,ip_l,port_l,ip_r,port_r);//�õ�ͨ��˫����ַ��Ϣ

            //�����־
            m_cfg.logger.warn("%s ->LOC<%s:%u>RMT<%s:%u>",msg,ip_l,port_l,ip_r,port_r);
            //�Ͽ�����,�ͷ�socket
            disconnect(true);
        }

    public:
        //-------------------------------------------------
        tcp_session_t(tcp_sesncfg_t& cfg,void* usrdata=NULL):m_sock(bad_socket),m_cfg(cfg),m_usrdata(usrdata){}
        //-------------------------------------------------
        virtual ~tcp_session_t(){disconnect();}
        socket_t socket() const {return m_sock;}
        //-------------------------------------------------
        //�жϵ�ǰ�Ƿ���������,socket�Ƿ���Ч.
        bool connected(){return m_sock!=bad_socket;}
        //-------------------------------------------------
        //�Ͽ�����,�ͷ�socket
        void disconnect(bool NoWait=true)
        {
            if (m_cfg.on_disconnect.is_valid())
                m_cfg.on_disconnect(m_sock,m_usrdata);
            sock::close(m_sock,NoWait);
        }
        //-------------------------------------------------
        //��ָ������(size)������(data)���͸��Զ�.���Զ���ָ�����ͳ�ʱ�ȴ�ʱ��.
        //����ֵ:ec_ok���;��������,����δ���,�ر�����:ec_uninit���ӶϿ���δ��ʼ��;ec_net_writeЩ����;ec_net_timeout���ͳ�ʱ;
        error_t write(const void *data,uint32_t size,uint32_t timeout_us=0)
        {
            if (m_sock==bad_socket)
                return ec_uninit;

            if (!timeout_us)
                timeout_us=m_cfg.timeout_us_wr;

            //����ȷ�������¼���ί��
            sock::event_rw_t *evt=m_cfg.on_send.is_valid()?&m_cfg.on_send:NULL;

            //����������ѭ������
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
        //Ҫ������ȡlen���ֽڵ����ݵ�buff��
        //����ֵ:ec_ok���;��������,����δ���,�ر�����:ec_uninit���ӶϿ���δ��ʼ��;ec_net_read����;ec_net_disconn�Է��Ѿ��Ͽ�;ec_net_timeout���ճ�ʱ;
        error_t read(void* buff,uint32_t len,uint32_t timeout_us=0)
        {
            if (m_sock==bad_socket)
                return ec_uninit;

            if (!timeout_us)
                timeout_us=m_cfg.timeout_us_rd;

            //����ȷ�������¼���ί��
            sock::event_rw_t *evt=m_cfg.on_recv.is_valid()?&m_cfg.on_recv:NULL;

            //����������ѭ������
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
        //Ҫ���ȡ���len���ֽڵ����ݵ�buff��,ֱ����ʱ��Ͽ�
        //����ֵ:ʵ�ʽ��ճ���(����ʱ���ӻ�Ͽ�,����δ�Ͽ���Ϊ��ʱ)
        uint32_t try_read(void* buff,uint32_t len,uint32_t timeout_us=0)
        {
            if (m_sock==bad_socket)
                return 0;

            if (!timeout_us)
                timeout_us=m_cfg.timeout_us_rd;

            //����ȷ�������¼���ί��
            sock::event_rw_t *evt=m_cfg.on_recv.is_valid()?&m_cfg.on_recv:NULL;

            //����������ѭ������
            sock::recv_buff_i ri((uint8_t*)buff,len,false);
            int r=sock::read_loop(m_sock,ri,timeout_us,evt,m_usrdata);
            if (r==0)
                m_err_disconn("net peer closed.");
            else if (r<0)
                m_err_disconn("net recv error.");
            return ri.size();
        }
        //-------------------------------------------------
        //Ҫ���ȡ���len���ֽڵ����ݵ�buff��,ֱ���յ�tag,��ʱ��Ͽ�;���tagsize��֪tag�ĳ���,���θ�֪tag��buff�еĿ�ʼλ������,-1������;
        //����ֵ:ʵ�ʽ��ճ���(����ʱ���ӻ�Ͽ�,����δ�Ͽ���Ϊ��ʱ);
        uint32_t try_read(void* buff,uint32_t len,void* tag,uint32_t &tagsize,uint32_t timeout_us=0)
        {
            if (m_sock==bad_socket)
                return 0;

            if (!timeout_us)
                timeout_us=m_cfg.timeout_us_rd;

            //����ȷ�������¼���ί��
            sock::event_rw_t *evt=m_cfg.on_recv.is_valid()?&m_cfg.on_recv:NULL;

            //����������ѭ������
            sock::recv_tag_i ri((uint8_t*)buff,len,false);
            ri.tag=(uint8_t*)tag;
            ri.tagsize=tagsize;
            int r=sock::read_loop(m_sock,ri,timeout_us,evt,m_usrdata);
            if (r==0)
                m_err_disconn("net peer closed.");
            else if (r<0)
                m_err_disconn("net recv error.");
            //��¼tag���ֵ�λ��
            tagsize=ri.tagpos;
            return ri.size();
        }
    };
}

#endif
