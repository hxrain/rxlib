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
    //tcp�����¼���ί������:tcp�Ự����;�Ự�Ƿ�Ϊtcp�ͻ���;�¼�û�з���ֵ
    typedef delegate2_t<tcp_session_t&,bool,void> tcp_evt_conn_t;
    //tcp�Ͽ��¼���ί������:tcp�Ự����;�Ƿ��ɴ��������Ự�Ͽ�;�¼�û�з���ֵ
    typedef delegate2_t<tcp_session_t&,bool,void> tcp_evt_disconn_t;

    //-----------------------------------------------------
    //tcp�Ựʹ�õ����ò���,��ȡ����������,��ʡ�ڴ�.
    typedef struct tcp_sesncfg_t
    {
        uint32_t                timeout_us_rd;              //���ճ�ʱus
        uint32_t                timeout_us_wr;              //���ͳ�ʱus
        uint32_t                timeout_us_conn;            //���ӳ�ʱus
        logger_i               &logger;                     //������־��¼��
        tcp_evt_conn_t          on_connect;                 //�¼�ί��:tcp�Ự���ӳɹ�
        tcp_evt_disconn_t       on_disconnect;              //�¼�ί��:tcp�Ự���ӶϿ�
        sock::event_rw_t        on_recv;                    //�¼�ί��:tcp�Ự���յ�����
        sock::event_rw_t        on_send;                    //�¼�ί��:tcp�Ự����������
        tcp_sesncfg_t(logger_i &log):timeout_us_rd(ms2us(500)),timeout_us_wr(ms2us(500)),timeout_us_conn(sec2us(3)),logger(log){}
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
    //ͬ��net������TCP�շ��Ự����(�����ڿͻ���������)
    class tcp_session_t
    {
    protected:
        friend void tcp_session_bind(tcp_session_t &sesn,socket_t sock,void *usrdata);

        socket_t                m_sock;                     //ͨ��ʹ�õ�Socket
        //-------------------------------------------------
        //�����շ�������־,���Ͽ�����
        bool m_err_disconn(const char* tip,bool is_error=true)
        {
            os_errmsg_t osmsg(tip);                         //�õ���ʽ�����ϵͳ������Ϣ������tip
            char addrstr[80];
            sock::addr_infos(m_sock,addrstr);               //�õ�ͨ��˫����ַ��Ϣ
            tcp_sesncfg_t &sc=get_tcp_sesncfg(sesncfg);     //��ȡ�Ự������Ϣ
            //�����־
            if (is_error)
                sc.logger.warn("%s :: %s",addrstr,(const char*)osmsg);
            else
                sc.logger.debug("%s :: %s",addrstr,(const char*)osmsg);
            //�Ͽ�����,�ͷ�socket,����֪�����ڴ����������ж�
            m_disconnect(true,is_error);
            return false;
        }
        //-------------------------------------------------
        //����Ự���ӵĶϿ��¼�
        void m_disconnect(bool NoWait,bool HaveErr)
        {
            if (m_sock==bad_socket)
                return;
            tcp_sesncfg_t &sc=get_tcp_sesncfg(sesncfg);     //��ȡ�Ự������Ϣ
            if (sc.on_disconnect.is_valid())
                sc.on_disconnect(*this,HaveErr);
            sock::close(m_sock,NoWait);
        }
    public:
        void*           usrdata;                            //session�󶨵��ⲿ�û�����
        tcp_sesncfg_t*  sesncfg;                            //sessionͳһʹ�õ����ò���
        //-------------------------------------------------
        tcp_session_t():m_sock(bad_socket),usrdata(NULL),sesncfg(NULL){}
        //-------------------------------------------------
        virtual ~tcp_session_t(){disconnect();}
        socket_t socket() const {return m_sock;}
        logger_i& logger(){return get_tcp_sesncfg(sesncfg).logger;}
        //-------------------------------------------------
        //�жϵ�ǰ�Ƿ���������,socket�Ƿ���Ч.
        bool connected(){return m_sock!=bad_socket;}
        //-------------------------------------------------
        //�Ͽ�����,�ͷ�socket
        void disconnect(bool NoWait=true){m_disconnect(NoWait,false);}
        //-------------------------------------------------
        //��ָ������(size)������(data)���͸��Զ�.���Զ���ָ�����ͳ�ʱ�ȴ�ʱ��.
        //����ֵ:ec_ok���;��������,����δ���,�ر�����:ec_uninit���ӶϿ���δ��ʼ��;ec_net_writeЩ����;ec_net_timeout���ͳ�ʱ;
        error_t write(const void *data,uint32_t size,uint32_t timeout_us=(uint32_t)-1)
        {
            if (m_sock==bad_socket)
                return ec_uninit;

            tcp_sesncfg_t &sc=get_tcp_sesncfg(sesncfg);     //��ȡ�Ự������Ϣ
            if (timeout_us==(uint32_t)-1)
                timeout_us=sc.timeout_us_wr;

            //����ȷ�������¼���ί��
            sock::event_rw_t *evt=sc.on_send.is_valid()?&sc.on_send:NULL;
            //����������ѭ������
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
        //Ҫ������ȡlen���ֽڵ����ݵ�buff��(Ĭ�ϳ�ʱʹ�ûỰ����)
        //����ֵ:ec_ok���;��������,����δ���,�ر�����:ec_uninit���ӶϿ���δ��ʼ��;ec_net_read����;ec_net_disconn�Է��Ѿ��Ͽ�;ec_net_timeout���ճ�ʱ;
        error_t read(void* buff,uint32_t len,uint32_t timeout_us=(uint32_t)-1)
        {
            if (m_sock==bad_socket)
                return ec_uninit;

            tcp_sesncfg_t &sc=get_tcp_sesncfg(sesncfg);     //��ȡ�Ự������Ϣ
            if (timeout_us==(uint32_t)-1)
                timeout_us=sc.timeout_us_rd;

            //����ȷ�������¼���ί��
            sock::event_rw_t *evt=sc.on_recv.is_valid()?&sc.on_recv:NULL;

            //����������ѭ������
            uint32_t recved=len;
            int32_t rc=sock::read_loop(m_sock,(uint8_t*)buff,recved,true,timeout_us,evt,this);
            if (rc<0)
            {//������
                m_err_disconn("tcp_session_t::read() net recv error.");
                return ec_net_read;
            }
            if (rc==1)
            {//��ʱ��
                if ((uint32_t)rc==len)
                    return ec_ok;
                m_err_disconn("tcp_session_t::read() net recv timeout.");
                return ec_net_timeout;
            }
            if (rc==0)
            {//���ӶϿ���
                m_err_disconn("tcp_session_t::read() net peer closed.",false);
                return ec_net_disconn;
            }
            if (recved<len)
            {//û�н�������
                m_err_disconn("tcp_session_t::read() net recv fail.");
                return ec_net_read;
            }
            return ec_ok;
        }
        //-------------------------------------------------
        //Ҫ���ȡ���len���ֽڵ����ݵ�buff��,ֱ����ʱ��Ͽ�(Ĭ�ϳ�ʱʹ�ûỰ����)
        //����ֵ:ʵ�ʽ��ճ���(���ӶϿ��������,����δ�Ͽ���Ϊ��ʱ)
        uint32_t readx(void* buff,uint32_t len,uint32_t timeout_us=(uint32_t)-1)
        {
            if (m_sock==bad_socket)
                return 0;

            tcp_sesncfg_t &sc=get_tcp_sesncfg(sesncfg);     //��ȡ�Ự������Ϣ
            if (timeout_us==(uint32_t)-1)
                timeout_us=sc.timeout_us_rd;

            //����ȷ�������¼���ί��
            sock::event_rw_t *evt=sc.on_recv.is_valid()?&sc.on_recv:NULL;

            //����������ѭ������
            sock::recv_buff_i ri((uint8_t*)buff,len,false);
            int r=sock::read_loop(m_sock,ri,timeout_us,evt,this);
            if (r==0)
                m_err_disconn("tcp_session_t::readx() net peer closed.",false);
            else if (r<0)
                m_err_disconn("tcp_session_t::readx() net recv error.");
            return ri.size();
        }
        //-------------------------------------------------
        //Ҫ���ȡ���len���ֽڵ����ݵ�buff��,ֱ���յ�tag,��ʱ��Ͽ�(Ĭ�ϳ�ʱʹ�ûỰ����);���tagsize��֪tag�ĳ���,���θ�֪tag��buff�еĿ�ʼλ������,-1������;
        //����ֵ:ʵ�ʽ��ճ���(���ӶϿ��������,����δ�Ͽ���Ϊ��ʱ);
        uint32_t readx(void* buff,uint32_t len,void* tag,uint32_t &tagsize,uint32_t timeout_us=0)
        {
            if (m_sock==bad_socket)
                return 0;

            tcp_sesncfg_t &sc=get_tcp_sesncfg(sesncfg);     //��ȡ�Ự������Ϣ
            if (!timeout_us)
                timeout_us=sc.timeout_us_rd;

            //����ȷ�������¼���ί��
            sock::event_rw_t *evt=sc.on_recv.is_valid()?&sc.on_recv:NULL;

            //����������ѭ������
            sock::recv_tag_i ri((uint8_t*)buff,len,false);
            ri.tag=(uint8_t*)tag;
            ri.tagsize=tagsize;
            int r=sock::read_loop(m_sock,ri,timeout_us,evt,this);
            if (r==0)
                m_err_disconn("tcp_session_t::readx(tag) net peer closed.",false);
            else if (r<0)
                m_err_disconn("tcp_session_t::readx(tag) net recv error.");
            //��¼tag���ֵ�λ��
            tagsize=ri.tagpos;
            return ri.size();
        }
    };
    //��sock��ָ����session����,���ڷ���˳�ʼ���Ựʹ��.
    inline void tcp_session_bind(tcp_session_t &sesn,socket_t sock,void *usrdata=NULL)
    {
        rx_assert(sesn.m_sock==bad_socket);
        rx_assert(sesn.usrdata==NULL);
        sesn.m_sock=sock;
        sesn.usrdata=usrdata;
        tcp_sesncfg_t &sc=get_tcp_sesncfg(sesn.sesncfg);    //��ȡ�Ự������Ϣ
        if (sc.on_connect.is_valid())
            sc.on_connect(sesn,false);                      //�����¼�,������server�ĻỰ��,����client
    }

    //-----------------------------------------------------
    //ͬ��ģʽnet���ܵ�tcp�ͻ��˹��ܷ�װ
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
            get_tcp_sesncfg(super_t::sesncfg).logger.warn("socket(%u)dst<%s:%u> :: %s",super_t::m_sock,(const char*)ip_r,m_dst.port(),(const char*)osmsg);
            sock::close(super_t::m_sock,true);
            return false;
        }
    public:
        //-------------------------------------------------
        //���ô����ӵ�Ŀ���ַ
        bool dst_addr(const char* host,uint16_t port)
        {
            return m_dst.set_addr(host,port);
        }
        //-------------------------------------------------
        //����socket������Ŀ���ַ,ʹ�ñ��ض˿�����,�ɿ������ӳ�ʱus
        bool connect(uint16_t LocalPort=0,uint32_t timeout_us=sec2us(3))
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

            tcp_sesncfg_t &sc=get_tcp_sesncfg(sesncfg);     //��ȡ�Ự������Ϣ
            //�������������Ӷ���
            int r=sock::connect(super_t::m_sock,m_dst,timeout_us);
            if (r==1)
            {//���ӳɹ���,�����ⲿ�ص�֪ͨ
                if (sc.on_connect.is_valid())
                    sc.on_connect(*this,true);              //���������¼�,������tcp�ͻ���
                return true;
            }
            if (r==0)
                return m_err_close("socket connect timeout.");

            return m_err_close("socket connect error.");
        }
    };
}

#endif
