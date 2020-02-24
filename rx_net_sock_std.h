#ifndef _RX_NET_SOCK_H_
#define _RX_NET_SOCK_H_

#include "rx_cc_macro.h"
#include "rx_os_misc.h"
#include "rx_str_util_ext.h"

#if RX_IS_OS_WIN
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <mstcpip.h>
    typedef SOCKET socket_t;
    const uint32_t bad_socket=bad_socket;
    #define RX_CHECK_WINSOCK2() WSAHtonl(0,0,0)   //��䱾��������,����֪��Ҫ����ws2_32.lib,ʹ��winsock2��API.

    namespace rx
    {
        //����ȫ��socket����������.��win��ʹ��ʱ�������socket�����ĳ�ʼ��.
        class socket_env_t
        {
            bool m_Inited;
        public:
            socket_env_t(bool AutoInit=true):m_Inited(false)
            {
                RX_CHECK_WINSOCK2();
                if (AutoInit&&!init())
                    rx_alert(os_errmsg_t().msg("socket_t env init fail."));
            }
            bool init()
            {
                if (m_Inited) return true;
                WORD Ver=MAKEWORD((2),(2));
                WSADATA wsaData;
                if (WSAStartup( Ver, &wsaData ))
                    return false;
                m_Inited=true;
                return true;
            }
            void uninit()
            {
                if (!m_Inited) return;
                m_Inited=false;
                WSACleanup();
            }
            virtual~socket_env_t(){uninit();}
        };
    }
#else
    #include <sys/socket.h>
    #include <sys/ioctl.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <netdb.h>
    typedef int socket_t;
    const int32_t bad_socket=-1;
    #define RX_CHECK_WINSOCK2()

    namespace rx
    {
        class socket_env_t
        {
        public:
            socket_env_t(bool AutoInit=true){}
            bool init(){return true;}
            void uninit(){}
            virtual~socket_env_t(){uninit();}
        };
    }

#endif

namespace rx
{
    //-----------------------------------------------------
    //����򵥵�IP��ַ���洢�ռ�
    typedef struct ip_str_t
    {
        char addr[16];                                      //��ʱ�Ƚ�����ipv4��ַ�ռ�
        ip_str_t(){addr[0]=0;}
        operator char* ()const{return (char*)addr;}
    }ip_str_t;

    //-----------------------------------------------------
    //��װsocket��ַ��ع���
    class sock_addr_t
    {
        struct sockaddr_in m_sa;
    public:
        //-------------------------------------------------
        sock_addr_t(){clear();}
        sock_addr_t(const char* Host,int Port)
        {
            clear();
            set_addr(Host,Port);
        }
        sock_addr_t(uint32_t Host,int Port)                 //Host�������ֽ����IP��ַ
        {
            clear();
            m_sa.sin_port         = htons(Port);
            m_sa.sin_addr.s_addr  = Host;
        }
        sock_addr_t(const struct sockaddr_in& Addr):m_sa(Addr){}
        //-------------------------------------------------
        //�����ַ��Ϣ
        void clear()
        {
            memset(&m_sa,0,sizeof(struct sockaddr_in));
            m_sa.sin_family       = AF_INET;                //IP��ַ��
            m_sa.sin_port         = htons(0);               //struct sockaddr_in.sin_port��Ҫ�����ֽ���Ķ˿�
            m_sa.sin_addr.s_addr  = INADDR_ANY;             //struct sockaddr_in.sin_addr.s_addr��Ҫ���������ֽ����IP��ַ
        }
        //-------------------------------------------------
        //�ж�������ַ��Ϣ�Ƿ���ͬ
        bool operator==(const sock_addr_t& addr)
        {
            return m_sa.sin_port==addr.m_sa.sin_port&&m_sa.sin_addr.s_addr==addr.m_sa.sin_addr.s_addr;
        }
        //-------------------------------------------------
        //�жϵ�ǰ�ڲ���ַ�Ƿ���Ч
        bool is_valid(){return m_sa.sin_addr.s_addr!=INADDR_NONE&&m_sa.sin_port!=0;}
        //-------------------------------------------------
        //���������ֽ���Ķ˿ں�
        bool set_port(uint16_t P){m_sa.sin_port = htons(P);return true;}
        //-------------------------------------------------
        //�õ������ֽ���Ķ˿ں�
        const uint16_t port() const{return ntohs(m_sa.sin_port);}
        //-------------------------------------------------
        //ʹ��IP�������������õ�ַ;˳���������ö˿�
        //����ֵ:���õ�IP���������Ƿ�Ϸ�
        bool set_addr(const char* host,uint16_t port=0)
        {
            if (port) set_port(port);
            m_sa.sin_addr.s_addr=lookup(host);
            if (m_sa.sin_addr.s_addr!=INADDR_NONE) return true;
            return false;
        }
        struct sockaddr_in& addr(){return m_sa;}
        const struct sockaddr_in& addr()const{return m_sa;}
        //-------------------------------------------------
        //�õ������ֽ����IP��ַ
        uint32_t ip_addr(){return m_sa.sin_addr.s_addr;}
        //-------------------------------------------------
        //�õ�IP��ַ��:������;�������ߴ�.����ֵ:NULLʧ��,����Ϊ������ָ��
        char* ip_str(char* Buf,int Size) const
        {
            if (to_str(m_sa,Buf,Size))
                return Buf;
            return NULL;
        }
        char* ip_str(ip_str_t& str) const
        {
            if (to_str(m_sa,str))
                return str.addr;
            return NULL;
        }
        //-------------------------------------------------
        //ת�������ֽ���IP��ַ���ַ���.����ֵ:������;0����������.
        static uint32_t to_str(uint32_t ip,char *buff,uint32_t size)
        {
            struct sockaddr_in sa;
            sa.sin_addr.s_addr=ip;
            return to_str(sa,buff,size);
        }
        static uint32_t to_str(uint32_t ip,ip_str_t& str)
        {
            struct sockaddr_in sa;
            sa.sin_addr.s_addr=ip;
            return to_str(sa,str);
        }
        //-------------------------------------------------
        //����socket��ַ�õ�IP��ַ��.����ֵ:������;0����������.
        static uint32_t to_str(const struct sockaddr_in &sa,char *buff,uint32_t size)
        {
            sncat<16> cat;
            uint8_t *bp=(uint8_t*)&sa.sin_addr.s_addr;
            cat("%u.",bp[0])("%u.",bp[1])("%u.",bp[2])("%u",bp[3]);
            return st::strcpy(buff,size,cat.str,cat.size);
        }
        static uint32_t to_str(const struct sockaddr_in &sa,ip_str_t& str)
        {
            sncat<0> cat(str.addr);
            uint8_t *bp=(uint8_t*)&sa.sin_addr.s_addr;
            cat("%u.",bp[0])("%u.",bp[1])("%u.",bp[2])("%u",bp[3]);
            return cat.size;
        }
        //-------------------------------------------------
        //����IP���������õ���Ӧ�������ֽ����IP��ַ(gethostbyname���ڶ��߳�����������,����ʹ������DNS�ͻ��˻򻺴淽��)
        //����ֵ:INADDR_NONEʧ��;�����ɹ�
        static uint32_t lookup(const char* host)
        {
            uint32_t ret=inet_addr(host);                   //�ȳ��Խ���IP����ַ��ת��
            if (INADDR_NONE!=ret) return ret;               //ת���ɹ���ֱ�ӷ���

            struct hostent *Host=gethostbyname(host);       //�ٳ��Խ��������Ĳ���
            if (Host==NULL) return INADDR_NONE;             //����ʧ��ֱ�ӷ���
            return ((struct in_addr *)Host->h_addr)->s_addr;
        }
        //-------------------------------------------------
        //��IP��ת��ΪIPֵ
        //����ֵ:INADDR_NONEʧ��;�����ɹ�
        static uint32_t to_ip(const char* Str)
        {
            return inet_addr(Str);
        }
    };

    //-----------------------------------------------------
    //��socket���Ͻ��м򵥹���,���ڻ�ȡnfds
    class sock_sets
    {
        fd_set          m_sets;
        uint32_t        m_size;
        socket_t        m_nfds;
    public:
        //-------------------------------------------------
        sock_sets(){reset();}
        //-------------------------------------------------
        //��λ,׼���������
        void reset(){FD_ZERO(&m_sets);m_size=0;m_nfds=0;}
        //-------------------------------------------------
        //��ȡ���ϵ���������뵱ǰԪ������
        static uint32_t capacity(){return FD_SETSIZE;}
        uint32_t size(){return m_size;}
        //-------------------------------------------------
        //����һ��Ԫ��
        bool push(socket_t sock)
        {
            if (m_size==capacity())
                return false;
            FD_SET(sock,&m_sets);
            m_nfds=max(m_nfds,sock);
            ++m_size;
            return true;
        }
        //-------------------------------------------------
        //���뼯�ϵ�socket���ֵ
        socket_t nfds(){return m_nfds+1;}
        //-------------------------------------------------
        operator fd_set* (){return &m_sets;}
        operator fd_set& (){return m_sets;}
        //-------------------------------------------------
        //�ڽ�����select֮��,���д˷�������,�жϸ�����sock�Ƿ��ڼ�����
        bool contain(socket_t sock){return FD_ISSET(sock,&m_sets)!=0;}
    };

    //�Գ���socket��ع��ܺ������з�װ
    namespace sock
    {
        //-------------------------------------------------
        //����һ��socket���:�Ƿ񴴽�ΪTCP�׽���;������Ǵ���UDP�׽���.
        //����ֵ:bad_socketʧ��;����Ϊ�����׽���
        inline socket_t create(bool tcp=true)
        {
            if (tcp)
                return socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
            else
                return socket(AF_INET,SOCK_DGRAM,IPPROTO_IP);
        }
        //-------------------------------------------------
        //ʹ����ȷ�Ĳ�������ָ����socket
        inline socket_t create_raw(int proto=IPPROTO_ICMP,int type=SOCK_RAW,int af=AF_INET)
        {
            return socket(af,type,proto);
        }
        //-------------------------------------------------
        //�����socket��һ����ַ������
        inline bool bind(socket_t sock,const sock_addr_t& sa)
        {
            RX_CHECK_WINSOCK2();
            return 0==bind(sock,(struct sockaddr *)&sa.addr(),sizeof(sa.addr()));
        }
        //-------------------------------------------------
        //�����socket��һ����ַ�Ͷ˿ڰ�����(Ĭ���뱾����ȫ���ӿ�ip��ָ���˿ڰ�)
        inline bool bind(socket_t sock,uint16_t port,const char* host=NULL)
        {
            if (is_empty(host))
                host="0.0.0.0";
            sock_addr_t Addr(host,port);
            return bind(sock,Addr);
        }
        //-------------------------------------------------
        //��ָ����socket����������
        inline bool listen(socket_t sock,int backlog=SOMAXCONN)
        {
            return 0==::listen(sock,backlog);
        }
        //-------------------------------------------------
        //��ָ����socket�Ͻ��н������Ӳ���,ͬʱ�õ��ͻ��˵ĵ�ַ��Ϣ
        //����ֵ:������socket,��bad_socket
        inline socket_t accept(socket_t listen_sock,sock_addr_t& sa)
        {
            socklen_t OutLen=sizeof(sa.addr());
            socket_t Ret=bad_socket;
        #if RX_IS_OS_WIN
            try{
                Ret=WSAAccept(listen_sock,(struct sockaddr *)&sa.addr(),&OutLen,NULL,0);
            }catch(...){}
        #else
            Ret=accept(listen_sock,(struct sockaddr *)&sa.addr(),&OutLen);
        #endif
            if (OutLen!=(int)sizeof(sa.addr()))
                return bad_socket;
            return Ret;
        }
        //-------------------------------------------------
        //�����socket�Ͻ��н������Ӳ���:ͬʱ�õ��ͻ��˵ĵ�ַ��Ϣ
        inline socket_t accept(socket_t sock)
        {
            sock_addr_t sa;
            return accept(sock,sa);
        }
        //-------------------------------------------------
        //��ָ����sock�����ϵȴ��¼�,io��·����:set_r����;set_wд��;set_e����;��ʱʱ����΢��;nfds��֪ȫ�������е����������+1
        //����ֵ:<0����;0��ʱ;>0���¼�����
        inline int32_t select(fd_set* set_r,fd_set* set_w,fd_set *set_e,uint32_t timeout_us,int nfds)
        {
            const uint32_t sec_us=1000*1000;
            struct timeval tm;
            tm.tv_sec=timeout_us/sec_us;
            tm.tv_usec=timeout_us%sec_us;
            return select(nfds,set_r,set_w,set_e,(timeout_us!=(uint32_t)-1?&tm:NULL));
        }
        //-------------------------------------------------
        //�ȴ������п���socket
        //����ֵ:<0����;0��ʱ;>0���¼�����
        inline int32_t select_rd(fd_set& set,uint32_t timeout_us,int nfds)
        {
            return select(&set,NULL,NULL,timeout_us,nfds);
        }
        inline int32_t select_rd(sock_sets& set,uint32_t timeout_us)
        {
            return select_rd(set,timeout_us,(int)set.nfds());
        }
        //-------------------------------------------------
        //�ȴ�д���п���socket
        //����ֵ:<0����;0��ʱ;>0���¼�����
        inline int32_t select_wr(fd_set& set,uint32_t timeout_us,int nfds)
        {
            return select(NULL,&set,NULL,timeout_us,nfds);
        }
        inline int32_t select_wr(sock_sets& set,uint32_t timeout_us)
        {
            return select_wr(set,timeout_us,(int)set.nfds());
        }
        //-------------------------------------------------
        //�ȴ�ָ����socket�ɶ�.
        //����ֵ:<0����;0��ʱ;1�ɶ�
        inline int32_t wait_rd(socket_t sock,uint32_t timeout_us)
        {
            fd_set set;
            FD_ZERO(&set);
            FD_SET(sock,&set);
            int32_t r=select_rd(set,timeout_us,int(sock+1));
            if (r<=0)
                return r;
            return (FD_ISSET(sock,&set)?1:0);
        }
        //-------------------------------------------------
        //�ȴ�ָ����socket��д.
        //����ֵ:<0����;0��ʱ;1��д
        inline int32_t wait_wr(socket_t sock,uint32_t timeout_us)
        {
            fd_set set;
            FD_ZERO(&set);
            FD_SET(sock,&set);
            int32_t r=select_wr(set,timeout_us,int(sock+1));
            if (r<=0)
                return r;
            return (FD_ISSET(sock,&set)?1:0);
        }
        //-------------------------------------------------
        //�����socket���п��Ʋ���
        inline bool ioctrl(socket_t sock,uint32_t cmd,uint32_t& args)
        {
        #if RX_IS_OS_WIN
            return 0==ioctlsocket(sock, cmd, (u_long*)&args);
        #else
            return 0==::ioctl(sock, cmd, &args);
        #endif
        }
        //-------------------------------------------------
        //����socketΪ�����������ģʽ
        inline bool block_mode(socket_t sock,bool is_block)
        {
            uint32_t v=is_block?0:1;
            return ioctrl(sock,FIONBIO,v);
        }
        //-------------------------------------------------
        //��ѯ���ջ������ڿɶ�ȡ���ݳ���
        //����ֵ:<0����;0û������;>0�ǿɶ�ȡ�����ݳ���
        inline int32_t readable_size(socket_t sock)
        {
            uint32_t N=0;
            if (ioctrl(sock,FIONREAD,N))
                return (int32_t)N;
            return -1;
        }
        //-------------------------------------------------
        //��ǿ�����Ӳ���,�ɳ�ʱ�ȴ�:���ӵ�Ŀ���ַ;��Ҫ�ȴ���ʱ��
        //����ֵ:1�ɹ�;0���ӳ�ʱ;<0����:-1������ģʽ���ô���;-2����ģʽ�ָ�����;-3���Ӵ���;-4select����ʧ��
        inline int32_t connect(socket_t sock,const sock_addr_t& sa,uint32_t timeout_us)
        {
            if (!timeout_us)
            {//����Ҫ�������ӳ�ʱ�Ŀ���,ʹ��ϵͳ��Ĭ�ϵ����ӳ�ʱ�ȴ�ʱ��
                if (0==connect(sock,(struct sockaddr *)&sa.addr(),sizeof(sa.addr())))
                    return 1;
                else return -3;                             //����ʧ��
            }

            //��Ҫ�������ӳ�ʱ�Ŀ���
            if (!block_mode(sock,false))
                return -1;                                  //��socket����Ϊ������ģʽ

            int32_t cr=0;
            if (0!=connect(sock,(struct sockaddr *)&sa.addr(),sizeof(sa.addr())))
                cr=wait_wr(sock,timeout_us);                //���з��������Ӳ��ȴ���д(���ӳɹ�)

            if (!block_mode(sock,true))                     //��socket�ָ�Ϊ����ģʽ
                return -2;
            if (cr<0)
                return -4;                                  //�ж�select״̬,��Ϊ���ӽ��
            return cr;
        }
        //��socket���ӵ�ָ����������ַ�Ͷ˿�
        inline int32_t connect(socket_t sock,const char* host,uint16_t port,uint32_t timeout_us)
        {
            sock_addr_t sa(host,port);
            return connect(sock,sa,timeout_us);
        }
        //-------------------------------------------------
        //�ر�ָ����socket.����ָ��ǿ�������ر�,������time_wait״̬
        inline bool close(socket_t &sock,bool no_wait=false)
        {
            if (sock==(socket_t)bad_socket) return true;

            if (no_wait)
            {//����Ҫ���йرյȴ�,��ô��ֱ�ӹر�
        #if 0
                int value = 0;
                setsockopt(sock,SOL_SOCKET,SO_DONTLINGER,(const char*)&value,sizeof(value));
        #else
                struct linger value;
                value.l_onoff=1;
                value.l_linger=0;
                setsockopt(sock,SOL_SOCKET,SO_LINGER,(const char*)&value,sizeof(value));
        #endif
            }
            else
            {
        #if RX_IS_OS_WIN
                shutdown(sock,SD_SEND);                     //�ر�ʱ�ȴ��������
        #else
                shutdown(sock,SHUT_WR);                     //�ر�ʱ�ȴ��������
        #endif
            }

            bool Ret;
        #if RX_IS_OS_WIN
            Ret= 0==closesocket(sock);
        #else
            Ret= 0==close(sock);
        #endif
            sock=bad_socket;
            return Ret;
        }
        //-------------------------------------------------
        //��socket�ϲ鿴����:������;����������;
        //����ֵ:<�����˴���;0˵�����ӶϿ�;>0Ϊ���ݳ���
        inline int32_t peek(socket_t sock,uint8_t* Buf,uint32_t BufLen)
        {
            return ::recv(sock,(char*)Buf,BufLen,MSG_PEEK);
        }
        //-------------------------------------------------
        //�����socket�Ͻ�������:������;����������;�������(MSG_PEEK�鿴ģʽ);
        //����ֵ:<0�����˴���;0˵�����ӶϿ�;>0Ϊ�յ������ݳ���
        inline int32_t recv(socket_t sock,uint8_t* Buf,uint32_t BufLen,int Flag=0)
        {
            return ::recv(sock,(char*)Buf,BufLen,Flag);
        }
        //-------------------------------------------------
        //�ڴ�����:������;�����͵����ݳ���;���Ͳ������
        //����ֵ:<0�д���;>=0��֪ʵ�ʷ��͵ĳ���(��Ϊ�ײ㻺������ʣ��ռ���ܲ�����);
        inline int32_t send(socket_t sock,const void* Buf,uint32_t BufLen,int Flag=0)
        {
            return ::send(sock,(char*)Buf,BufLen,Flag);
        }
        //-------------------------------------------------
        //�����socket�Ͻ�������:������;����������;��������Դ��ַ;���յ������ݳ���;�������;
        //����ֵ:<0�����˴���;>=0��֪���ճ���.
        inline int32_t recvfrom(socket_t sock,uint8_t* Buf,uint32_t BufLen,sock_addr_t& sa,int Flag=0)
        {
            int AddrLen=sizeof(sa.addr());
            return ::recvfrom(sock,(char*)Buf,BufLen,Flag,(struct sockaddr *)&sa.addr(),(socklen_t*)&AddrLen);
        }
        //-------------------------------------------------
        //�ڴ�����:������;�����͵����ݳ���;���͵�Ŀ���ַ;���Ͳ������
        //����ֵ:<0�д���;>=0��֪ʵ�ʷ��͵ĳ���(��Ϊ�ײ㻺������ʣ��ռ���ܲ�����);
        inline int32_t  sendto(socket_t sock,const void* Buf,uint32_t BufLen,const sock_addr_t& sa,int Flag=0)
        {
            return ::sendto(sock,(char*)Buf,BufLen,Flag,(struct sockaddr *)&sa.addr(),sizeof(sa.addr()));
        }
        inline int32_t  sendto(socket_t sock,const void* Buf,uint32_t BufLen,const char* host,uint16_t port,int Flag=0)
        {
            sock_addr_t sa(host,port);
            return sendto(sock,Buf,BufLen,sa,Flag);
        }
        //-------------------------------------------------
        //��ȡָ��socket�ı��˵�ַ
        inline bool local_addr(socket_t sock,sock_addr_t& sa)
        {
            socklen_t AddrSize=sizeof(sa.addr());
            return 0==getsockname(sock,(struct sockaddr*)&sa.addr(),&AddrSize);
        }
        //��ȡָ��socket�ı��ض˿�
        inline uint32_t local_port(socket_t sock)
        {
            sock_addr_t Addr;
            if (!local_addr(sock,Addr))
                return 0;
            return Addr.port();
        }
        //-------------------------------------------------
        //�õ���ǰ�����ϵı�����IP
        inline char* local_ip(socket_t sock,ip_str_t &str)
        {
            sock_addr_t sa;
            if (!local_addr(sock,sa))
                return NULL;
            return sa.ip_str(str.addr,sizeof(str));
        }
        //-------------------------------------------------
        //�õ�ָ��socket�϶Է���ַ
        inline bool peer_addr(socket_t sock,sock_addr_t& sa)
        {
            socklen_t AddrSize=sizeof(sa.addr());
            return 0==getpeername(sock,(struct sockaddr*)&sa.addr(),&AddrSize);
        }
        //-------------------------------------------------
        //�õ�ָ��socket�϶Է���IP��ַ��
        inline char* peer_ip(socket_t sock,ip_str_t &str)
        {
            sock_addr_t sa;
            if (!peer_addr(sock,sa))
                return NULL;
            return sa.ip_str(str.addr,sizeof(str));
        }
        //-------------------------------------------------
        //��ȡ��socket�϶�Ӧ�ĶԷ��˿�
        //����ֵ:0ʧ��;����Ϊ�˿ں�
        inline uint16_t peer_port(socket_t sock)
        {
            sock_addr_t sa;
            if (!peer_addr(sock,sa))
                return 0;
            return sa.port();
        }
        //-------------------------------------------------
        //�����socket������ѡ��:ѡ��ȼ�(SOL_SOCKET��IPPROTO_TCP);ѡ�����;ѡ��ֵ,ֵ����
        inline bool opt_set(socket_t sock,int OptionLevel,int Option,const void* OptionValue,int OptionValueLen)
        {
            return 0==setsockopt(sock,OptionLevel,Option,(const char*)OptionValue,OptionValueLen);
        }
        //-------------------------------------------------
        //�õ����socket���ƶ�ѡ���ֵ:ѡ��ȼ�(SOL_SOCKET��IPPROTO_TCP);ѡ�����;ѡ��ֵ������,����������/ֵ����
        inline bool opt_get(socket_t sock,int OptionLevel,int Option,void* OptionValue,int &OptionValueLen)
        {
            return 0==getsockopt(sock,OptionLevel,Option,(char*)OptionValue,(socklen_t*)&OptionValueLen);
        }
        //-------------------------------------------------
        //����socketĬ�ϵĶ���ʱ�ȴ�ʱ��
        inline bool opt_timeout_rd(socket_t sock,uint32_t timeout_us)
        {
        #if RX_IS_OS_WIN
            uint32_t ms=timeout_us/1000;
            return opt_set(sock,SOL_SOCKET,SO_RCVTIMEO,&ms,sizeof(ms));
        #else
            const uint32_t sec_us=1000*1000;
            struct timeval tm;
            tm.tv_sec=timeout_us/sec_us;
            tm.tv_usec=timeout_us%sec_us;
            return opt_set(sock,SOL_SOCKET,SO_RCVTIMEO,&tm,sizeof(tm));
        #endif
        }
        //-------------------------------------------------
        //����socketĬ�ϵ�д��ʱ�ȴ�ʱ��
        inline bool opt_timeout_wr(socket_t sock,uint32_t timeout_us)
        {
        #if RX_IS_OS_WIN
            uint32_t ms=timeout_us/1000;
            return opt_set(sock,SOL_SOCKET,SO_SNDTIMEO,&ms,sizeof(ms));
        #else
            const uint32_t sec_us=1000*1000;
            struct timeval tm;
            tm.tv_sec=timeout_us/sec_us;
            tm.tv_usec=timeout_us%sec_us;
            return opt_set(sock,SOL_SOCKET,SO_SNDTIMEO,&tm,sizeof(tm));
        #endif
        }
        //-------------------------------------------------
        //��ȡ��ǰsocket��ϵͳ���ջ�������С
        //����ֵ:-1����;����Ϊ���
        inline int opt_buffsize_rd(socket_t sock)
        {
            int s=0;
            int t=sizeof(s);
            if (!opt_get(sock,SOL_SOCKET,SO_RCVBUF,&s,t))
                return -1;
            return s;
        }
        //-------------------------------------------------
        //���õ�ǰsocket��ϵͳ���ջ�������С
        inline bool opt_buffsize_rd(socket_t sock,uint32_t Size)
        {
            int t=sizeof(Size);
            return opt_set(sock,SOL_SOCKET,SO_RCVBUF,&Size,t);
        }
        //-------------------------------------------------
        //�õ���ǰsocket��ϵͳ���ͻ�������С
        //����ֵ:-1����;����Ϊ���
        inline int opt_buffsize_wr(socket_t sock)
        {
            int s=0;
            int t=sizeof(s);
            if (!opt_get(sock,SOL_SOCKET,SO_SNDBUF,&s,t))
                return -1;
            return s;
        }
        //-------------------------------------------------
        //���õ�ǰsocket��ϵͳ���ͻ�������С
        inline bool opt_buffsize_wr(socket_t sock,uint32_t Size)
        {
            int t=sizeof(Size);
            return opt_set(sock,SOL_SOCKET,SO_SNDBUF,&Size,t);
        }
        //-------------------------------------------------
        //�����Ƿ�����ظ�ʹ�ö˿ڵ�ַ
        inline bool opt_reuse_addr(socket_t sock,bool reuse)
        {
            int option = reuse?1:0;
            return opt_set(sock,SOL_SOCKET, SO_REUSEADDR,&option,sizeof(option));
        }
        //-------------------------------------------------
        //�Ƿ�����·ά�ֱ������Ӳ�Ҫ�ж�.����ģʽʹ��Ĭ�ϲ���,���̽��ʱ��̫����,���岻��.
        inline bool opt_keepalive(socket_t sock,bool Enable)
        {
            int option = Enable?1:0;
            return opt_set(sock,SOL_SOCKET, SO_KEEPALIVE,&option,sizeof(option));
        }
        //������·ά�ֵĸ߼�ģʽ,����ָ�����м��ʱ���̽������ͼ��ʱ��
        inline bool opt_keepalive_ex(socket_t sock,bool Enable,uint32_t idle_ms,uint32_t reinterval_ms=1000)
        {
        #if RX_IS_OS_WIN
            struct tcp_keepalive keepin;
            struct tcp_keepalive keepout;

            keepin.keepaliveinterval=reinterval_ms;         //̽������ͼ��ʱ��
            keepin.keepalivetime=idle_ms;                   //��·�ϳ���idle_msû�д������ݣ����Զ�����̽���
            keepin.onoff=Enable?1:0;
            uint32_t keepout_len;
            return 0==WSAIoctl(sock,SIO_KEEPALIVE_VALS,&keepin,sizeof(keepin),&keepout,sizeof(keepout),(DWORD*)&keepout_len,NULL,NULL);
        #else
            uint32_t keepAlive=Enable?1:0;
            if (!opt_set(sock, SOL_SOCKET, SO_KEEPALIVE, (void*)&keepAlive, sizeof(keepAlive)))
                return false;

            if (!Enable) return false;

            uint32_t keepIdle = idle_ms/1000;               //�����keepIdle����û���κ����ݽ���,�����̽��. ȱʡֵ:7200(s)
            if (keepIdle==0) keepIdle=1;
            if (!opt_set(sock, IPPROTO_TCP, TCP_KEEPIDLE, (void*)&keepIdle, sizeof(keepIdle)))
                return false;

            uint32_t keepInterval = reinterval_ms/1000;     //̽��ʱ��̽�����ʱ����ΪkeepInterval��. ȱʡֵ:75(s)
            if (reinterval_ms==0) reinterval_ms=1;
            if (!opt_set(sock, IPPROTO_TCP, TCP_KEEPINTVL, (void*)&keepInterval, sizeof(keepInterval)))
                return false;

            uint32_t keepCount = 3;                         //̽�����ԵĴ���. ȫ����ʱ���϶�����ʧЧ..ȱʡֵ:9(��)
            if (!opt_set(sock, IPPROTO_TCP, TCP_KEEPCNT, (void*)&keepCount, sizeof(keepCount)))
                return false;
            return true;
        #endif
        }
        //-------------------------------------------------
        //����socket�Ĺ㲥ģʽ
        inline bool opt_broadcast(socket_t sock,bool Enable=true)
        {
            int flag=Enable?1:0;
            return setsockopt(sock,SOL_SOCKET,SO_BROADCAST,(char*)&flag,sizeof(flag))==0;
        }
        //-------------------------------------------------
        //�鲥�ػ�����(�Ƿ���ձ����������鲥��Ϣ)
        inline bool group_loopback(socket_t sock,bool V)
        {
            uint8_t LV=V?1:0;
            return setsockopt(sock,IPPROTO_IP,IP_MULTICAST_LOOP,(char*)&LV,sizeof(LV))==0;
        }
        //-------------------------------------------------
        //����socket���鲥ģʽ:����IP,ѡ���鲥ʹ�õ�����
        inline bool group_interface(socket_t sock,const char* LocalIP)
        {
            struct in_addr addr;
            addr.s_addr=sock_addr_t::lookup(LocalIP);
            return 0==setsockopt(sock,IPPROTO_IP,IP_MULTICAST_IF,(char*)&addr,sizeof(addr));
        }
        //-------------------------------------------------
        //����socket���鲥ģʽ:����һ����
        inline bool group_add(socket_t sock,const char* GroupAddr,const char* LocalIP=NULL)
        {
            struct ip_mreq M;
            M.imr_multiaddr.s_addr=sock_addr_t::lookup(GroupAddr);
            M.imr_interface.s_addr=INADDR_ANY;
            if (LocalIP)
                M.imr_interface.s_addr=sock_addr_t::lookup(LocalIP);
            return setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&M,sizeof(M))==0;
        }
        //����socket���鲥ģʽ:�뿪һ����
        inline bool group_del(socket_t sock,const char* GroupAddr,const char* LocalIP=NULL)
        {
            struct ip_mreq M;
            M.imr_multiaddr.s_addr=sock_addr_t::lookup(GroupAddr);
            M.imr_interface.s_addr=INADDR_ANY;
            if (LocalIP)
                M.imr_interface.s_addr=sock_addr_t::lookup(LocalIP);
            return setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&M,sizeof(M))==0;
        }
        //��ָ����socket�ϻ�ȡ����ip�˿��Լ�Զ��ip�˿���Ϣ
        inline void addr_infos(socket_t sock,ip_str_t &ip_l,uint16_t &port_l,ip_str_t &ip_r,uint16_t &port_r)
        {
            sock_addr_t addr_l,addr_r;
            local_addr(sock,addr_l);
            local_addr(sock,addr_r);

            addr_l.ip_str(ip_l);
            addr_r.ip_str(ip_r);
            port_l=addr_l.port();
            port_r=addr_r.port();
        }
        inline char* addr_infos(socket_t sock,char buff[53])
        {
            sncat<0> cat(buff);
            ip_str_t ipstr;
            sock_addr_t addr;

            local_addr(sock,addr);
            cat("LOC<%s:%u>",addr.ip_str(ipstr),addr.port());
            peer_addr(sock,addr);
            cat("DST<%s:%u>",addr.ip_str(ipstr),addr.port());
            return buff;
        }
    }
    //-----------------------------------------------------
    //����һ��socket���������й���,��Ҫ��Ŀ����Ϊ���������������ر�socket
    class sock_t
    {
        socket_t m_sock;
    public:
        sock_t():m_sock(bad_socket){}
        sock_t(socket_t s):m_sock(s){}
        operator socket_t() const
        {
            return m_sock;
        }
        sock_t& operator=(socket_t s)
        {
            m_sock=s;
            return *this;
        }
        virtual ~sock_t()
        {
            if (m_sock==bad_socket)
                return;
            sock::close(m_sock,true);
            m_sock=bad_socket;
        }
    };
    //-----------------------------------------------------
    //����ͨ�ŵ�Ŀ���ַ,���Ի�ȡ��Ӧ�ı���IP(�����ݱ���·�ɱ�,���ж�Ŀ���Ƿ��������,UDPģʽ)
    //���:DestIPĿ���ַ
    //����ֵ:INADDR_NONE��ѯʧ��;����Ϊ����ip
    inline uint32_t localip_by_dest(uint32_t DestIP)
    {
        sock_t s=sock::create(false);                       //����udp socket�������й�
        if (s==bad_socket)
            return INADDR_NONE;

        sock_addr_t dst_addr(DestIP,1);                     //����Ŀ���ַ,����עĿ��˿ڵ�����
        if (sock::connect(s,dst_addr,0)<=0)                 //�ؼ�,��socket�ײ��ѯ����·�ɱ�,����Ŀ�����
            return INADDR_NONE;

        if (!sock::local_addr(s,dst_addr))                  //���ڿ��Ի�ȡ��socket�ı�����ַ��
            return INADDR_NONE;
        return dst_addr.ip_addr();
    }
    inline uint32_t localip_by_dest(const char* DestIP)
    {
        uint32_t DstIP=sock_addr_t::lookup(DestIP);
        if (DstIP==INADDR_NONE)
            return INADDR_NONE;
        return localip_by_dest(DstIP);
    }
    inline char* localip_by_dest(const char* DestIP,ip_str_t &lip)
    {
        uint32_t ip=localip_by_dest(DestIP);
        if (ip==INADDR_NONE)
            return NULL;
        sock_addr_t::to_str(ip,lip);
        return lip.addr;
    }
    //-------------------------------------------------
    //�жϸ����ĵ�ַ�Ƿ�Ϊ������ַ
    inline bool is_local_ip(const char* Host)
    {
        uint32_t DstIP=sock_addr_t::lookup(Host);
        if (DstIP==INADDR_NONE)
            return false;
        return DstIP==localip_by_dest(DstIP);
    }
    //-------------------------------------------------
    //TCP�˿����Ӳ���
    inline bool tcp_conn_test(const char* dest,uint32_t port,uint32_t timeout_ms=500)
    {
        sock_t s=sock::create();                            //����tcp socket���й�
        if (s==bad_socket)
            return false;
        return sock::connect(s,dest,port,timeout_ms*1000)>0;//��������Ŀ��
    }
    //-------------------------------------------------
    //ʹ��tcp����Ŀ���,��ȡ���ض�Ӧ��ip��ַ��
    inline char* localip_by_dest(const char* dest,uint32_t port,ip_str_t &lip,uint32_t timeout_ms=500)
    {
        sock_t s=sock::create();                            //����tcp socket���й�
        if (s==bad_socket)
            return NULL;
        if (sock::connect(s,dest,port,timeout_ms*1000)<=0)  //��������Ŀ��
            return NULL;
        return sock::local_ip(s,lip);
    }
}

#endif
