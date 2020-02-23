#ifndef RX_NET_TCP_SERVER_H
#define RX_NET_TCP_SERVER_H

#include "rx_net_tcp_session.h"
#include "rx_dtl_array.h"

namespace rx
{
    //tcp����˿�ͬʱ�����˿��������ֵ
    #ifndef RX_SVRSOCK_MAX_LISTEN_PORTS
        #define RX_SVRSOCK_MAX_LISTEN_PORTS     8
    #endif

    //-----------------------------------------------------
    //��װ��򵥻�����tcp����˵ײ㹦��,������ɼ򵥷���.
    class tcp_svrsocks_t
    {
    public:
        //-------------------------------------------------
        typedef struct listener_t                           //���ؼ���ʹ�õ�socket��󶨵Ķ˿ڵ�ַ��Ϣ:������.
        {
            socket_t        sock;
            sock_addr_t     addr;
            listener_t():sock(bad_socket){}
        }listener_t;
    protected:
        typedef array_ft<listener_t,RX_SVRSOCK_MAX_LISTEN_PORTS>   sock_array_t;

        sock_array_t    m_socks;                            //socket��������
        logger_i        &m_logger;                          //��־����ӿ�

        //-------------------------------------------------
        //��¼������Ϣ����־
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
        //���ҿյ�socket��Ϣ������.
        //����ֵ:-1������;����Ϊ��������
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
        //���ݵ�ַ��Ϣ���Ҷ�Ӧ��socket��Ϣ������.
        //����ֵ:-1������;����Ϊ��������
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
        tcp_svrsocks_t():m_logger(make_logger_con()){}      //Ĭ�Ͽ���ʹ�ÿ���̨��Ϊ��־��¼��
        virtual ~tcp_svrsocks_t(){close();}
        //-------------------------------------------------
        //���Լ������ض˿�,����ֻ����ָ���ı�����ַ;�趨socket�Ͽɽ������ӵ��������.
        bool open(uint16_t port,const char* host=NULL,uint32_t backlogs=1024)
        {
            uint32_t idx=m_find_by_empty();                 //�Ȳ��ҿ��ò�λ
            if (idx==uint32_t(-1))
                return m_err("tcp svrsocks is full.");

            listener_t ss;
            if (!ss.addr.set_addr(host,port))               //��ת�������ַ
                return m_err("invalid local listen addr <%s:%u>.",not_empty(host),port);

            if (m_find_by_addr(ss.addr)!=(uint32_t)-1)      //���������ַ�Ƿ��Ѿ�������
                return m_err("local addr already exists. can't listen again.");

            ss.sock=sock::create(true);                     //����socket
            if (ss.sock==bad_socket)
                return m_err("can't create tcp socket.");

            if (!sock::bind(ss.sock,ss.addr))               //��socket��ָ���Ķ˿�
            {
                m_err("can't bind local addr <%s:%u>.",not_empty(host),port);
                sock::close(ss.sock,true);
                return false;
            }

            if (!sock::listen(ss.sock,min((uint32_t)SOMAXCONN,backlogs)))//������������
            {
                m_err("can't listen local addr <%s:%u>.",not_empty(host),port);
                sock::close(ss.sock,true);
                return false;
            }

            m_socks[idx]=ss;                                //��¼��ǰ����socket��ָ���Ĳ�λ
            return true;
        }
        //-------------------------------------------------
        //�ر�ָ���ļ����˿�,��ȫ���Ķ˿�
        bool close(uint16_t port=0,const char* host=NULL)
        {
            if (port==0)
            {//�ر�ȫ���ļ���socket
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
            if (!sa.set_addr(host,port))                    //ת�������ַ
                return m_err("invalid local listen addr <%s:%u>.",not_empty(host),port);

            uint32_t idx=m_find_by_addr(sa);                //���������ַ��Ӧ��socket��Ϣ����
            if (idx==(uint32_t)-1)
                return false;

            listener_t &ss=m_socks[idx];
            if (ss.sock!=bad_socket)                        //�ر�ָ��������
            {
                sock::close(ss.sock,true);
                ss.addr.clear();
                return true;
            }
            return false;
        }
        //-------------------------------------------------
        //���н������ӵĵ�������,����Ϊ�ⲿ�¼�ѭ���ĵײ����.
        //����:�����ӵĻỰsocket,�����ӵĶԶ˵�ַָ��.
        //����ֵ:NULLû�������ӽ���;����Ϊ���������ӵļ�����ָ��
        listener_t* step(socket_t &new_sock,sock_addr_t *peer_addr=NULL,uint32_t timeout_us=1000)
        {
            return NULL;
        }
    };

    //-----------------------------------------------------
    //���̳߳��湦�ܵ�tcp������,�����ͨ�õĶ��̷߳�����.
    class tcp_server_t
    {
    };
}

#endif
