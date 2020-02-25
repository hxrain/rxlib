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
    //tcp���ӻ�Ͽ��¼���ί������
    typedef delegate2_t<socket_t,tcp_session_t&,void> tcp_evt_conn_t;

    //-----------------------------------------------------
    //tcp�Ựʹ�õ����ò���,��ȡ����������,��ʡ�ڴ�.
    typedef struct tcp_sesncfg_t
    {
        uint32_t                timeout_us_rd;              //���ճ�ʱus
        uint32_t                timeout_us_wr;              //���ͳ�ʱus
        uint32_t                timeout_us_conn;            //���ӳ�ʱus
        logger_i               &logger;                     //������־��¼��
        tcp_evt_conn_t          on_connect;                 //�¼�ί��:tcp�Ự���ӳɹ�
        tcp_evt_conn_t          on_disconnect;              //�¼�ί��:tcp�Ự���ӶϿ�
        sock::event_rw_t        on_recv;                    //�¼�ί��:tcp�Ự���յ�����
        sock::event_rw_t        on_send;                    //�¼�ί��:tcp�Ự����������
        tcp_sesncfg_t(logger_i &log):timeout_us_rd(1000*500),timeout_us_wr(1000*500),timeout_us_conn(1000*3000),logger(log){}
    }
    tcp_sesncfg_t;

    //-----------------------------------------------------
    //����tcp�Ự����,���Ը���ȫ��Ĭ��ֵ,����ʹ��
    inline tcp_sesncfg_t& get_tcp_sesncfg(tcp_sesncfg_t* ptr=NULL)
    {
        static tcp_sesncfg_t cfg(make_logger_con());
        if (ptr==NULL)
            return cfg;
        return *ptr;
    }

    //-----------------------------------------------------
    //ͬ��net������TCP�շ��Ự����
    class tcp_session_t
    {
    protected:
        friend void tcp_session_bind(tcp_session_t &sesn,socket_t sock,void *usrdata);

        socket_t                m_sock;                     //ͨ��ʹ�õ�Socket
        //-------------------------------------------------
        //�����շ�������־,���Ͽ�����
        bool m_err_disconn(const char* tip)
        {
            os_errmsg_t osmsg(tip);                         //�õ���ʽ�����ϵͳ������Ϣ������tip
            char addrstr[53];
            sock::addr_infos(m_sock,addrstr);               //�õ�ͨ��˫����ַ��Ϣ
            //�����־
            get_tcp_sesncfg(sesncfg).logger.warn("%s -> %s",(const char*)osmsg,addrstr);
            //�Ͽ�����,�ͷ�socket
            disconnect(true);
            return false;
        }

    public:
        void*           usrdata;                            //session�󶨵��ⲿ�û�����
        tcp_sesncfg_t*  sesncfg;                            //sessionͳһʹ�õ����ò���
        //-------------------------------------------------
        tcp_session_t():m_sock(bad_socket),usrdata(NULL),sesncfg(NULL){}
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
            if (m_sock==bad_socket)
                return;

            if (get_tcp_sesncfg(sesncfg).on_disconnect.is_valid())
                get_tcp_sesncfg(sesncfg).on_disconnect(m_sock,*this);
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
                timeout_us=get_tcp_sesncfg(sesncfg).timeout_us_wr;

            //����ȷ�������¼���ί��
            sock::event_rw_t *evt=get_tcp_sesncfg(sesncfg).on_send.is_valid()?&get_tcp_sesncfg(sesncfg).on_send:NULL;

            //����������ѭ������
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
        //Ҫ������ȡlen���ֽڵ����ݵ�buff��
        //����ֵ:ec_ok���;��������,����δ���,�ر�����:ec_uninit���ӶϿ���δ��ʼ��;ec_net_read����;ec_net_disconn�Է��Ѿ��Ͽ�;ec_net_timeout���ճ�ʱ;
        error_t read(void* buff,uint32_t len,uint32_t timeout_us=0)
        {
            if (m_sock==bad_socket)
                return ec_uninit;

            if (!timeout_us)
                timeout_us=get_tcp_sesncfg(sesncfg).timeout_us_rd;

            //����ȷ�������¼���ί��
            sock::event_rw_t *evt=get_tcp_sesncfg(sesncfg).on_recv.is_valid()?&get_tcp_sesncfg(sesncfg).on_recv:NULL;

            //����������ѭ������
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
        //Ҫ���ȡ���len���ֽڵ����ݵ�buff��,ֱ����ʱ��Ͽ�
        //����ֵ:ʵ�ʽ��ճ���(����ʱ���ӻ�Ͽ�,����δ�Ͽ���Ϊ��ʱ)
        uint32_t try_read(void* buff,uint32_t len,uint32_t timeout_us=0)
        {
            if (m_sock==bad_socket)
                return 0;

            if (!timeout_us)
                timeout_us=get_tcp_sesncfg(sesncfg).timeout_us_rd;

            //����ȷ�������¼���ί��
            sock::event_rw_t *evt=get_tcp_sesncfg(sesncfg).on_recv.is_valid()?&get_tcp_sesncfg(sesncfg).on_recv:NULL;

            //����������ѭ������
            sock::recv_buff_i ri((uint8_t*)buff,len,false);
            int r=sock::read_loop(m_sock,ri,timeout_us,evt,this);
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
                timeout_us=get_tcp_sesncfg(sesncfg).timeout_us_rd;

            //����ȷ�������¼���ί��
            sock::event_rw_t *evt=get_tcp_sesncfg(sesncfg).on_recv.is_valid()?&get_tcp_sesncfg(sesncfg).on_recv:NULL;

            //����������ѭ������
            sock::recv_tag_i ri((uint8_t*)buff,len,false);
            ri.tag=(uint8_t*)tag;
            ri.tagsize=tagsize;
            int r=sock::read_loop(m_sock,ri,timeout_us,evt,this);
            if (r==0)
                m_err_disconn("net peer closed.");
            else if (r<0)
                m_err_disconn("net recv error.");
            //��¼tag���ֵ�λ��
            tagsize=ri.tagpos;
            return ri.size();
        }
    };
    //��sock��ָ����session����
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
