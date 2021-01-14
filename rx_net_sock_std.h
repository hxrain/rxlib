#ifndef _RX_NET_SOCK_H_
#define _RX_NET_SOCK_H_

#include "rx_cc_macro.h"
#include "rx_os_misc.h"
#include "rx_str_util_ext.h"
#include "rx_ct_delegate.h"
#include <memory.h>

#if RX_IS_OS_WIN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mstcpip.h>
typedef SOCKET socket_t;
const uint32_t bad_socket = bad_socket;
#define RX_CHECK_WINSOCK2() WSAHtonl(0,0,0)   //��䱾��������,����֪��Ҫ����ws2_32.lib,ʹ��winsock2��API.

namespace rx
{
	//����ȫ��socket����������.��win��ʹ��ʱ�������socket�����ĳ�ʼ��.
	class socket_env_t
	{
		bool m_Inited;
	public:
		socket_env_t(bool AutoInit = true) :m_Inited(false)
		{
			RX_CHECK_WINSOCK2();
			if (AutoInit && !init())
				rx_alert(os_errmsg_t().msg("socket_t env init fail."));
		}
		bool init()
		{
			if (m_Inited) return true;
			WORD Ver = MAKEWORD((2), (2));
			WSADATA wsaData;
			if (WSAStartup(Ver, &wsaData))
				return false;
			m_Inited = true;
			return true;
		}
		void uninit()
		{
			if (!m_Inited) return;
			m_Inited = false;
			WSACleanup();
		}
		virtual~socket_env_t() { uninit(); }
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
const int32_t bad_socket = -1;
#define RX_CHECK_WINSOCK2()

namespace rx
{
	class socket_env_t
	{
	public:
		socket_env_t(bool AutoInit = true) {}
		bool init() { return true; }
		void uninit() {}
		virtual~socket_env_t() { uninit(); }
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
		ip_str_t() { addr[0] = 0; }
		operator char* ()const { return (char*)addr; }
		const char* c_str()const { return (char*)addr; }
	}ip_str_t;

	//-----------------------------------------------------
	//��װsocket��ַ��ع���
	class sock_addr_t
	{
		struct sockaddr_in m_sa;
	public:
		//-------------------------------------------------
		sock_addr_t() { clear(); }
		sock_addr_t(const char* Host, int Port)
		{
			clear();
			set_addr(Host, Port);
		}
		sock_addr_t(uint32_t Host, int Port)                //Host�������ֽ����IP��ַ
		{
			clear();
			m_sa.sin_port = htons(Port);
			m_sa.sin_addr.s_addr = Host;
		}
		sock_addr_t(const struct sockaddr_in& Addr) :m_sa(Addr) {}
		//-------------------------------------------------
		//�����ַ��Ϣ
		void clear()
		{
			memset(&m_sa, 0, sizeof(struct sockaddr_in));
			m_sa.sin_family = AF_INET;						//IP��ַ��
			m_sa.sin_port = htons(0);						//struct sockaddr_in.sin_port��Ҫ�����ֽ���Ķ˿�
			m_sa.sin_addr.s_addr = INADDR_ANY;				//struct sockaddr_in.sin_addr.s_addr��Ҫ���������ֽ����IP��ַ
		}
		//-------------------------------------------------
		//�ж�������ַ��Ϣ�Ƿ���ͬ
		bool operator==(const sock_addr_t& addr)
		{
			return m_sa.sin_port == addr.m_sa.sin_port &&
				m_sa.sin_addr.s_addr == addr.m_sa.sin_addr.s_addr;
		}
		//-------------------------------------------------
		//�жϵ�ǰ�ڲ���ַ�Ƿ���Ч
		bool is_valid() { return m_sa.sin_addr.s_addr != INADDR_NONE&&m_sa.sin_port != 0; }
		//-------------------------------------------------
		//���������ֽ���Ķ˿ں�
		bool set_port(uint16_t P) { m_sa.sin_port = htons(P); return true; }
		//-------------------------------------------------
		//�õ������ֽ���Ķ˿ں�
		const uint16_t port() const { return ntohs(m_sa.sin_port); }
		//-------------------------------------------------
		//ʹ��IP�������������õ�ַ;˳���������ö˿�
		//����ֵ:���õ�IP���������Ƿ�Ϸ�
		bool set_addr(const char* host, uint16_t port = 0)
		{
			if (port) set_port(port);
			m_sa.sin_addr.s_addr = lookup(host);
			if (m_sa.sin_addr.s_addr != INADDR_NONE) return true;
			return false;
		}
		struct sockaddr_in& addr() { return m_sa; }
		const struct sockaddr_in& addr()const { return m_sa; }
		//-------------------------------------------------
		//�õ������ֽ����IP��ַ
		uint32_t ip_addr() { return m_sa.sin_addr.s_addr; }
		//-------------------------------------------------
		//�õ�IP��ַ��:������;�������ߴ�.����ֵ:NULLʧ��,����Ϊ������ָ��
		char* ip_str(char* Buf, int Size) const
		{
			if (to_str(m_sa, Buf, Size))
				return Buf;
			return NULL;
		}
		char* ip_str(ip_str_t& str) const
		{
			if (to_str(m_sa, str))
				return str.addr;
			return NULL;
		}
		//-------------------------------------------------
		//ת�������ֽ���IP��ַ���ַ���.����ֵ:������;0����������.
		static uint32_t to_str(uint32_t ip, char *buff, uint32_t size)
		{
			struct sockaddr_in sa;
			sa.sin_addr.s_addr = ip;
			return to_str(sa, buff, size);
		}
		static uint32_t to_str(uint32_t ip, ip_str_t& str)
		{
			struct sockaddr_in sa;
			sa.sin_addr.s_addr = ip;
			return to_str(sa, str);
		}
		//-------------------------------------------------
		//����socket��ַ�õ�IP��ַ��.����ֵ:������;0����������.
		static uint32_t to_str(const struct sockaddr_in &sa, char *buff, uint32_t size)
		{
			sncat<16> cat;
			uint8_t *bp = (uint8_t*)&sa.sin_addr.s_addr;
			cat("%u.", bp[0])("%u.", bp[1])("%u.", bp[2])("%u", bp[3]);
			return st::strcpy(buff, size, cat.str, cat.size);
		}
		static uint32_t to_str(const struct sockaddr_in &sa, ip_str_t& str)
		{
			sncat<0> cat(str.addr);
			uint8_t *bp = (uint8_t*)&sa.sin_addr.s_addr;
			cat("%u.", bp[0])("%u.", bp[1])("%u.", bp[2])("%u", bp[3]);
			return cat.size;
		}
		//-------------------------------------------------
		//����IP���������õ���Ӧ�������ֽ����IP��ַ(gethostbyname���ڶ��߳�����������,����ʹ������DNS�ͻ��˻򻺴淽��)
		//����ֵ:INADDR_NONEʧ��;�����ɹ�
		static uint32_t lookup(const char* host)
		{
			if (is_empty(host)) return INADDR_ANY;
			struct addrinfo hints, *res = NULL;
			fillzero(hints);

			hints.ai_family = AF_INET;						// Allow IPv4
			hints.ai_flags = AI_PASSIVE;					// For wildcard IP address
			hints.ai_protocol = 0;							// Any protocol 
			hints.ai_socktype = SOCK_STREAM;

			if (getaddrinfo(host, NULL, &hints, &res) != 0)
				return INADDR_NONE;

			rx_assert(res != NULL && res->ai_addr != NULL);
			uint32_t addr = *(uint32_t*)(res->ai_addr->sa_data+2);
			freeaddrinfo(res);
			return addr;
		}
		//-------------------------------------------------
		//��IP��ת��ΪIPֵ
		//����ֵ:INADDR_NONEʧ��;�����ɹ�
		static uint32_t to_ip(const char* str)
		{
			uint32_t addr;
			if (inet_pton(AF_INET, str, &addr) > 0)
				return addr;
			return INADDR_NONE;
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
		sock_sets() { reset(); }
		//-------------------------------------------------
		//��λ,׼���������
		void reset() { FD_ZERO(&m_sets); m_size = 0; m_nfds = 0; }
		//-------------------------------------------------
		//��ȡ���ϵ���������뵱ǰԪ������
		static uint32_t capacity() { return FD_SETSIZE; }
		uint32_t size() { return m_size; }
		//-------------------------------------------------
		//����һ��Ԫ��
		bool push(socket_t sock)
		{
			if (m_size == capacity())
				return false;
			FD_SET(sock, &m_sets);
			m_nfds = max(m_nfds, sock);
			++m_size;
			return true;
		}
		//-------------------------------------------------
		//���뼯�ϵ�socket���ֵ
		socket_t nfds() { return m_nfds + 1; }
		//-------------------------------------------------
		operator fd_set* () { return &m_sets; }
		operator fd_set& () { return m_sets; }
		//-------------------------------------------------
		//�ڽ�����select֮��,���д˷�������,�жϸ�����sock�Ƿ��ڼ�����
		bool contain(socket_t sock) { return FD_ISSET(sock, &m_sets) != 0; }
	};

	//�Գ���socket��ع��ܺ������з�װ
	namespace sock
	{
		//-------------------------------------------------
		//����һ��socket���:�Ƿ񴴽�ΪTCP�׽���;������Ǵ���UDP�׽���.
		//����ֵ:bad_socketʧ��;����Ϊ�����׽���
		inline socket_t create(bool tcp = true)
		{
			if (tcp)
				return socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			else
				return socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
		}
		//-------------------------------------------------
		//ʹ����ȷ�Ĳ�������ָ����socket
		inline socket_t create_raw(int proto = IPPROTO_ICMP, int type = SOCK_RAW, int af = AF_INET)
		{
			return socket(af, type, proto);
		}
		//-------------------------------------------------
		//�����socket��һ����ַ������
		inline bool bind(socket_t sock, const sock_addr_t& sa)
		{
			RX_CHECK_WINSOCK2();
			return 0 == bind(sock, (struct sockaddr *)&sa.addr(), sizeof(sa.addr()));
		}
		//-------------------------------------------------
		//�����socket��һ����ַ�Ͷ˿ڰ�����(Ĭ���뱾����ȫ���ӿ�ip��ָ���˿ڰ�)
		inline bool bind(socket_t sock, uint16_t port, const char* host = NULL)
		{
			if (is_empty(host))
				host = "0.0.0.0";
			sock_addr_t Addr(host, port);
			return bind(sock, Addr);
		}
		//-------------------------------------------------
		//��ָ����socket����������
		inline bool listen(socket_t sock, int backlog = SOMAXCONN)
		{
			return 0 == ::listen(sock, backlog);
		}
		//-------------------------------------------------
		//��ָ����socket�Ͻ��н������Ӳ���,ͬʱ�õ��ͻ��˵ĵ�ַ��Ϣ
		//����ֵ:������socket,��bad_socket
		inline socket_t accept(socket_t listen_sock, sock_addr_t& sa)
		{
			socklen_t OutLen = sizeof(sa.addr());
			socket_t Ret = bad_socket;
			#if RX_IS_OS_WIN
			try {
				Ret = WSAAccept(listen_sock, (struct sockaddr *)&sa.addr(), &OutLen, NULL, 0);
			}
			catch (...) {}
			#else
			Ret = accept(listen_sock, (struct sockaddr *)&sa.addr(), &OutLen);
			#endif
			if (OutLen != (int)sizeof(sa.addr()))
				return bad_socket;
			return Ret;
		}
		//-------------------------------------------------
		//�����socket�Ͻ��н������Ӳ���:ͬʱ�õ��ͻ��˵ĵ�ַ��Ϣ
		inline socket_t accept(socket_t sock)
		{
			sock_addr_t sa;
			return accept(sock, sa);
		}
		//-------------------------------------------------
		//��ָ����sock�����ϵȴ��¼�,io��·����:set_r����;set_wд��;set_e����;��ʱʱ��΢��(-1��ȫ����);nfds��֪ȫ�������е����������+1
		//����ֵ:<0����;0��ʱ;>0���¼�����
		inline int32_t select(fd_set* set_r, fd_set* set_w, fd_set *set_e, uint32_t timeout_us, int nfds)
		{
			struct timeval tm;
			tm.tv_sec = timeout_us / sec2us(1);
			tm.tv_usec = timeout_us%sec2us(1);
			return select(nfds, set_r, set_w, set_e, (timeout_us != (uint32_t)-1 ? &tm : NULL));
		}
		//-------------------------------------------------
		//�ȴ�socket���пɶ�дԪ��
		//����ֵ:<0����;0��ʱ;>0���¼�����
		inline int32_t select(fd_set& set, uint32_t timeout_us, int nfds, bool is_read = true)
		{
			if (is_read)
				return select(&set, NULL, NULL, timeout_us, nfds);
			else
				return select(NULL, &set, NULL, timeout_us, nfds);
		}
		//�ȴ�socket��չ���пɶ�дԪ��
		//����ֵ:<0����;0��ʱ;>0���¼�����
		inline int32_t select(sock_sets& set, uint32_t timeout_us, bool is_read = true)
		{
			if (is_read)
				return select((fd_set*)set, NULL, NULL, timeout_us, (int)set.nfds());
			else
				return select(NULL, (fd_set*)set, NULL, timeout_us, (int)set.nfds());
		}
		//-------------------------------------------------
		//�ȴ�ָ����socket�ɶ����д.
		//����ֵ:<0����;0��ʱ;1�ɶ�д
		inline int32_t wait(socket_t sock, uint32_t timeout_us, bool is_read = true)
		{
			fd_set set;
			FD_ZERO(&set);
			FD_SET(sock, &set);
			int32_t r = select(set, timeout_us, int(sock + 1), is_read);
			if (r <= 0)
				return r;
			return (FD_ISSET(sock, &set) ? 1 : 0);
		}
		//-------------------------------------------------
		//�����socket���п��Ʋ���
		inline bool ioctrl(socket_t sock, uint32_t cmd, uint32_t& args)
		{
			#if RX_IS_OS_WIN
			return 0 == ioctlsocket(sock, cmd, (u_long*)&args);
			#else
			return 0 == ::ioctl(sock, cmd, &args);
			#endif
	}
		//-------------------------------------------------
		//�����socket������ѡ��:ѡ��ȼ�(SOL_SOCKET��IPPROTO_TCP);ѡ�����;ѡ��ֵ,ֵ����
		inline bool opt_set(socket_t sock, int OptionLevel, int Option, const void* OptionValue, int OptionValueLen)
		{
			return 0 == setsockopt(sock, OptionLevel, Option, (const char*)OptionValue, OptionValueLen);
		}
		//-------------------------------------------------
		//�õ����socket���ƶ�ѡ���ֵ:ѡ��ȼ�(SOL_SOCKET��IPPROTO_TCP);ѡ�����;ѡ��ֵ������,����������/ֵ����
		inline bool opt_get(socket_t sock, int OptionLevel, int Option, void* OptionValue, int &OptionValueLen)
		{
			return 0 == getsockopt(sock, OptionLevel, Option, (char*)OptionValue, (socklen_t*)&OptionValueLen);
		}
		//-------------------------------------------------
		//��ѯָ��socket���Ƿ��д���
		inline bool opt_get_err(socket_t sock, int &err)
		{
			err = 0;
			int s = sizeof(int);
			return opt_get(sock, SOL_SOCKET, SO_ERROR, &err, s);
		}
		//-------------------------------------------------
		//����socketΪ�����������ģʽ
		inline bool block_mode(socket_t sock, bool is_block)
		{
			uint32_t v = is_block ? 0 : 1;
			return ioctrl(sock, FIONBIO, v);
		}
		//-------------------------------------------------
		//��ѯ���ջ������ڿɶ�ȡ���ݳ���
		//����ֵ:<0����;0û������;>0�ǿɶ�ȡ�����ݳ���
		inline int32_t readable_size(socket_t sock)
		{
			uint32_t N = 0;
			if (ioctrl(sock, FIONREAD, N))
				return (int32_t)N;
			return -1;
		}
		//-------------------------------------------------
		//��ǿ�����Ӳ���,�ɳ�ʱ�ȴ�:���ӵ�Ŀ���ַ;��Ҫ�ȴ���ʱ��(-1ʹ��ϵͳĬ����Ϊ)
		//����ֵ:1�ɹ�;0���ӳ�ʱ;<0����:-1������ģʽ���ô���;-2����ģʽ�ָ�����;-3���Ӵ���;-4select����ʧ��
		inline int32_t connect(socket_t sock, const sock_addr_t& sa, uint32_t timeout_us)
		{
			if (timeout_us == (uint32_t)-1)
			{//����Ҫ�������ӳ�ʱ�Ŀ���,ʹ��ϵͳ��Ĭ�ϵ����ӳ�ʱ�ȴ�ʱ��
				if (0 == connect(sock, (struct sockaddr *)&sa.addr(), sizeof(sa.addr())))
					return 1;
				else return -3;                             //����ʧ��
			}

			//��Ҫ�������ӳ�ʱ�Ŀ���
			if (!block_mode(sock, false))
				return -1;                                  //��socket����Ϊ������ģʽ

			int32_t cr = 0;
			if (0 != ::connect(sock, (struct sockaddr *)&sa.addr(), sizeof(sa.addr())))
				cr = wait(sock, timeout_us, false);             //���з��������Ӳ��ȴ���д(���ӳɹ�)

			#if !RX_IS_OS_WIN
			if (cr > 0)
			{//��linux��,��Ҫ�ٴ��ж�sock�ڲ��Ƿ��д���
				int err;
				opt_get_err(sock, err);
				if (err != 0)
				{//���������ԭ��
					errno = err;
					cr = -3;
		}
}
			#endif

			if (!block_mode(sock, true))                     //��socket�ָ�Ϊ����ģʽ
				return -2;
			if (cr < 0)
				return -4;                                  //�ж�select״̬,��Ϊ���ӽ��
			return cr;
		}
		//��socket���ӵ�ָ����������ַ�Ͷ˿�
		inline int32_t connect(socket_t sock, const char* host, uint16_t port, uint32_t timeout_us)
		{
			sock_addr_t sa(host, port);
			return connect(sock, sa, timeout_us);
		}
		//-------------------------------------------------
		//�ر�ָ����socket.����ָ��ǿ�������ر�,������time_wait״̬
		inline bool close(socket_t &sock, bool no_wait = false)
		{
			if (sock == (socket_t)bad_socket) return true;

			if (no_wait)
			{//����Ҫ���йرյȴ�,��ô��ֱ�ӹر�
				#if 0
				int value = 0;
				setsockopt(sock, SOL_SOCKET, SO_DONTLINGER, (const char*)&value, sizeof(value));
				#else
				struct linger value;
				value.l_onoff = 1;
				value.l_linger = 0;
				setsockopt(sock, SOL_SOCKET, SO_LINGER, (const char*)&value, sizeof(value));
				#endif
		}
			else
			{
				#if RX_IS_OS_WIN
				shutdown(sock, SD_SEND);                     //�ر�ʱ�ȴ��������
				#else
				shutdown(sock, SHUT_WR);                     //�ر�ʱ�ȴ��������
				#endif
			}

			bool Ret;
			#if RX_IS_OS_WIN
			Ret = (0 == closesocket(sock));
			#else
			Ret = (0 == ::close(sock));
			#endif
			sock = bad_socket;
			return Ret;
			}
		//-------------------------------------------------
		//��socket�ϲ鿴����:������;����������;
		//����ֵ:<�����˴���;0˵�����ӶϿ�;>0Ϊ���ݳ���
		inline int32_t peek(socket_t sock, uint8_t* Buf, uint32_t BufLen)
		{
			return ::recv(sock, (char*)Buf, BufLen, MSG_PEEK);
		}
		//-------------------------------------------------
		//�����socket�Ͻ�������:������;����������;�������(MSG_PEEK�鿴ģʽ);
		//����ֵ:<0�����˴���;0˵�����ӶϿ�;>0Ϊ�յ������ݳ���
		inline int32_t recv(socket_t sock, uint8_t* Buf, uint32_t BufLen, int Flag = 0)
		{
			return ::recv(sock, (char*)Buf, BufLen, Flag);
		}
		//-------------------------------------------------
		//�ڴ�����:������;�����͵����ݳ���;���Ͳ������
		//����ֵ:<0�д���;>=0��֪ʵ�ʷ��͵ĳ���(��Ϊ�ײ㻺������ʣ��ռ���ܲ�����);
		inline int32_t send(socket_t sock, const void* Buf, uint32_t BufLen, int Flag = 0)
		{
			return ::send(sock, (char*)Buf, BufLen, Flag);
		}
		//-------------------------------------------------
		//�����socket�Ͻ�������:������;����������;��������Դ��ַ;���յ������ݳ���;�������;
		//����ֵ:<0�����˴���;>=0��֪���ճ���.
		inline int32_t recvfrom(socket_t sock, uint8_t* Buf, uint32_t BufLen, sock_addr_t& sa, int Flag = 0)
		{
			int AddrLen = sizeof(sa.addr());
			return ::recvfrom(sock, (char*)Buf, BufLen, Flag, (struct sockaddr *)&sa.addr(), (socklen_t*)&AddrLen);
		}
		//-------------------------------------------------
		//�ڴ�����:������;�����͵����ݳ���;���͵�Ŀ���ַ;���Ͳ������
		//����ֵ:<0�д���;>=0��֪ʵ�ʷ��͵ĳ���(��Ϊ�ײ㻺������ʣ��ռ���ܲ�����);
		inline int32_t  sendto(socket_t sock, const void* Buf, uint32_t BufLen, const sock_addr_t& sa, int Flag = 0)
		{
			return ::sendto(sock, (char*)Buf, BufLen, Flag, (struct sockaddr *)&sa.addr(), sizeof(sa.addr()));
		}
		inline int32_t  sendto(socket_t sock, const void* Buf, uint32_t BufLen, const char* host, uint16_t port, int Flag = 0)
		{
			sock_addr_t sa(host, port);
			return sendto(sock, Buf, BufLen, sa, Flag);
		}
		//-------------------------------------------------
		//��ȡָ��socket�ı��˵�ַ
		inline bool local_addr(socket_t sock, sock_addr_t& sa)
		{
			socklen_t AddrSize = sizeof(sa.addr());
			return 0 == getsockname(sock, (struct sockaddr*)&sa.addr(), &AddrSize);
		}
		//��ȡָ��socket�ı��ض˿�
		inline uint32_t local_port(socket_t sock)
		{
			sock_addr_t Addr;
			if (!local_addr(sock, Addr))
				return 0;
			return Addr.port();
		}
		//-------------------------------------------------
		//�õ���ǰ�����ϵı�����IP
		inline char* local_ip(socket_t sock, ip_str_t &str)
		{
			sock_addr_t sa;
			if (!local_addr(sock, sa))
				return NULL;
			return sa.ip_str(str.addr, sizeof(str));
		}
		//-------------------------------------------------
		//�õ�ָ��socket�϶Է���ַ
		inline bool peer_addr(socket_t sock, sock_addr_t& sa)
		{
			socklen_t AddrSize = sizeof(sa.addr());
			return 0 == getpeername(sock, (struct sockaddr*)&sa.addr(), &AddrSize);
		}
		//-------------------------------------------------
		//�õ�ָ��socket�϶Է���IP��ַ��
		inline char* peer_ip(socket_t sock, ip_str_t &str)
		{
			sock_addr_t sa;
			if (!peer_addr(sock, sa))
				return NULL;
			return sa.ip_str(str.addr, sizeof(str));
		}
		//-------------------------------------------------
		//��ȡ��socket�϶�Ӧ�ĶԷ��˿�
		//����ֵ:0ʧ��;����Ϊ�˿ں�
		inline uint16_t peer_port(socket_t sock)
		{
			sock_addr_t sa;
			if (!peer_addr(sock, sa))
				return 0;
			return sa.port();
		}
		//-------------------------------------------------
		//����socketĬ�ϵĶ�д��ʱ�ȴ�ʱ��
		inline bool opt_timeout(socket_t sock, uint32_t timeout_us, bool is_read)
		{
			int opt = is_read ? SO_RCVTIMEO : SO_SNDTIMEO;
			#if RX_IS_OS_WIN
			uint32_t ms = (timeout_us + 999) / 1000;
			return opt_set(sock, SOL_SOCKET, opt, &ms, sizeof(ms));
			#else
			struct timeval tm;
			tm.tv_sec = timeout_us / sec2us(1);
			tm.tv_usec = timeout_us%sec2us(1);
			return opt_set(sock, SOL_SOCKET, opt, &tm, sizeof(tm));
			#endif
		}
		//-------------------------------------------------
		//��ȡ��ǰsocket��ϵͳ�շ���������С
		//����ֵ:-1����;����Ϊ���
		inline int opt_buffsize(socket_t sock, bool is_read)
		{
			int o = is_read ? SO_RCVBUF : SO_SNDBUF;
			int s = 0;
			int t = sizeof(s);
			if (!opt_get(sock, SOL_SOCKET, o, &s, t))
				return -1;
			return s;
		}
		//-------------------------------------------------
		//���õ�ǰsocket��ϵͳ�շ���������С
		inline bool opt_buffsize(socket_t sock, uint32_t Size, bool is_read)
		{
			int o = is_read ? SO_RCVBUF : SO_SNDBUF;
			int t = sizeof(Size);
			return opt_set(sock, SOL_SOCKET, o, &Size, t);
		}
		//-------------------------------------------------
		//�����Ƿ�����ظ�ʹ�ö˿ڵ�ַ
		inline bool opt_reuse_addr(socket_t sock, bool reuse)
		{
			int option = reuse ? 1 : 0;
			return opt_set(sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
		}
		//-------------------------------------------------
		//�Ƿ�����·ά�ֱ������Ӳ�Ҫ�ж�.����ģʽʹ��Ĭ�ϲ���,���̽��ʱ��̫����,���岻��.
		inline bool opt_keepalive(socket_t sock, bool Enable)
		{
			int option = Enable ? 1 : 0;
			return opt_set(sock, SOL_SOCKET, SO_KEEPALIVE, &option, sizeof(option));
		}
		//������·ά�ֵĸ߼�ģʽ,����ָ�����м��ʱ���̽������ͼ��ʱ��
		inline bool opt_keepalive_ex(socket_t sock, bool Enable, uint32_t idle_ms, uint32_t reinterval_ms = 1000)
		{
			#if RX_IS_OS_WIN
			struct tcp_keepalive keepin;
			struct tcp_keepalive keepout;

			keepin.keepaliveinterval = reinterval_ms;       //̽������ͼ��ʱ��
			keepin.keepalivetime = idle_ms;                 //��·�ϳ���idle_msû�д������ݣ����Զ�����̽���
			keepin.onoff = Enable ? 1 : 0;
			uint32_t keepout_len;
			return 0 == WSAIoctl(sock, SIO_KEEPALIVE_VALS, &keepin, sizeof(keepin), &keepout, sizeof(keepout), (DWORD*)&keepout_len, NULL, NULL);
			#else
			uint32_t keepAlive = Enable ? 1 : 0;
			if (!opt_set(sock, SOL_SOCKET, SO_KEEPALIVE, (void*)&keepAlive, sizeof(keepAlive)))
				return false;

			if (!Enable) return false;

			uint32_t keepIdle = idle_ms / 1000;             //�����keepIdle����û���κ����ݽ���,�����̽��. ȱʡֵ:7200(s)
			if (keepIdle == 0) keepIdle = 1;
			if (!opt_set(sock, IPPROTO_TCP, TCP_KEEPIDLE, (void*)&keepIdle, sizeof(keepIdle)))
				return false;

			uint32_t keepInterval = reinterval_ms / 1000;   //̽��ʱ��̽�����ʱ����ΪkeepInterval��. ȱʡֵ:75(s)
			if (reinterval_ms == 0) reinterval_ms = 1;
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
		inline bool opt_broadcast(socket_t sock, bool Enable = true)
		{
			int flag = Enable ? 1 : 0;
			return setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&flag, sizeof(flag)) == 0;
		}
		//-------------------------------------------------
		//�鲥�ػ�����(�Ƿ���ձ����������鲥��Ϣ)
		inline bool group_loopback(socket_t sock, bool V)
		{
			uint8_t LV = V ? 1 : 0;
			return setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP, (char*)&LV, sizeof(LV)) == 0;
		}
		//-------------------------------------------------
		//����socket���鲥ģʽ:����IP,ѡ���鲥ʹ�õ�����
		inline bool group_interface(socket_t sock, const char* LocalIP)
		{
			struct in_addr addr;
			addr.s_addr = sock_addr_t::lookup(LocalIP);
			return 0 == setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, (char*)&addr, sizeof(addr));
		}
		//-------------------------------------------------
		//����socket���鲥ģʽ:����һ����
		inline bool group_add(socket_t sock, const char* GroupAddr, const char* LocalIP = NULL)
		{
			struct ip_mreq M;
			M.imr_multiaddr.s_addr = sock_addr_t::lookup(GroupAddr);
			M.imr_interface.s_addr = INADDR_ANY;
			if (LocalIP)
				M.imr_interface.s_addr = sock_addr_t::lookup(LocalIP);
			return setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&M, sizeof(M)) == 0;
		}
		//����socket���鲥ģʽ:�뿪һ����
		inline bool group_del(socket_t sock, const char* GroupAddr, const char* LocalIP = NULL)
		{
			struct ip_mreq M;
			M.imr_multiaddr.s_addr = sock_addr_t::lookup(GroupAddr);
			M.imr_interface.s_addr = INADDR_ANY;
			if (LocalIP)
				M.imr_interface.s_addr = sock_addr_t::lookup(LocalIP);
			return setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&M, sizeof(M)) == 0;
		}
		//��ָ����socket�ϻ�ȡ����ip�˿��Լ�Զ��ip�˿���Ϣ
		inline void addr_infos(socket_t sock, ip_str_t &ip_l, uint16_t &port_l, ip_str_t &ip_r, uint16_t &port_r)
		{
			sock_addr_t addr_l, addr_r;
			local_addr(sock, addr_l);
			local_addr(sock, addr_r);

			addr_l.ip_str(ip_l);
			addr_r.ip_str(ip_r);
			port_l = addr_l.port();
			port_r = addr_r.port();
		}
		inline char* addr_infos(socket_t sock, char buff[80])
		{
			sncat<0> cat(buff);
			ip_str_t ipstr;
			sock_addr_t addr;

			cat("socket(%u)", sock);
			local_addr(sock, addr);
			cat("local(%s:%u)", addr.ip_str(ipstr), addr.port());
			peer_addr(sock, addr);
			cat("peer(%s:%u)", addr.ip_str(ipstr), addr.port());
			return buff;
		}

		//-------------------------------------------------
		//�����շ������¼���ί������:sock,sock�û�����,�������ݻ�����,�������ݳ���,�ܳ���,����ֵ��֪�Ƿ����
		typedef delegate5_t<socket_t, void*, uint8_t*, uint32_t, uint32_t, bool> event_rw_t;

		//�������������ݽӿ�
		class recv_data_i
		{
		public:
			//������ջ�����:�ڴ�����󻺳�������;ʵ�ʿ��û���������;����ֵ:NULLʧ��;�����ɹ�
			virtual uint8_t* get(uint32_t maxsize, uint32_t &realsize) = 0;
			//��֪������ʵ��ʹ�ó���;����ֵ:�Ƿ��������
			virtual bool put(uint32_t datasize) = 0;
			//�Ѿ����յ����ݳߴ�
			virtual uint32_t size() = 0;
		};

		//-------------------------------------------------
		//�����������ݵĻ�����
		class recv_buff_i :public recv_data_i
		{
		protected:
			uint8_t     *m_buff;                            //���ջ�����
			uint32_t    m_maxsize;                          //�����ճ���
			bool        m_must;                             //�Ƿ���������󳤶�
			uint32_t    m_size;                             //�Ѿ����յĳ���
		public:
			//�󶨽��ջ�����:���ջ�����,�����ճ���,�Ƿ�������.
			recv_buff_i(uint8_t* buff, uint32_t maxsize, bool mustsize = false) :m_buff(buff), m_maxsize(maxsize), m_must(mustsize), m_size(0) {}
			//������ջ�����:�ڴ�����󻺳�������;ʵ�ʿ��û���������;����ֵ:NULLʧ��;�����ɹ�
			uint8_t* get(uint32_t maxsize, uint32_t &realsize)
			{
				realsize = min(m_maxsize - m_size, maxsize);
				return m_buff + m_size;
			}
			//�������,��֪ʵ�ʽ������ݳ���;����ֵ:�Ƿ��������
			bool put(uint32_t datasize)
			{
				m_size += datasize;
				rx_assert(m_size <= m_maxsize);
				if (m_must&&m_size == m_maxsize)
					return false;
				return true;
			}
			//�Ѿ����յ����ݳߴ�
			uint32_t size() { return m_size; }
			//��������λ,׼�����½���ʹ��
			void reset() { m_size = 0; }
		};

		//-------------------------------------------------
		//���Եȴ����յ�tag�ͽ���
		class recv_tag_i :public recv_buff_i
		{
		public:
			uint8_t     *tag;
			uint32_t    tagsize;
			uint32_t    tagpos;
			recv_tag_i(uint8_t* buff, uint32_t maxsize, bool mustsize = false) :recv_buff_i(buff, maxsize, mustsize), tag(NULL), tagsize(0), tagpos(-1) {}
			//�������,��֪ʵ�ʽ������ݳ���;����ֵ:�Ƿ��������
			bool put(uint32_t datasize)
			{
				bool ic = recv_buff_i::put(datasize);
				if (recv_buff_i::m_size > tagsize)
				{//�������㹻����,�ж��Ƿ���tag
					rx_assert_if(tagsize, tag != NULL);
					const uint8_t* pos = st::memmemx(recv_buff_i::m_buff, recv_buff_i::m_size, tag, tagsize);
					if (pos)
					{//���ջ�������ȷʵ����tag��,���Խ���������.
						tagpos = uint32_t(pos - recv_buff_i::m_buff); //tagpos��¼tag������buff�е�λ��
						return false;
					}
				}
				if (!ic)
					return false;                           //����������,�����ټ���������
				return true;
			}
		};

		//-------------------------------------------------
		//�ɿ���ѭ�������͵������ȴ�ʱ��ĳ�����ȡ����:��ȡ������;���εȴ���ʱus;���չ۲��¼����¼�����;���ѭ������(0����);��ȡ���.
		//����ֵ:��֪ѭ������ԭ��,<0������;0���ӶϿ�;1�ȴ���ʱ;2�¼�Ҫ��ֹͣ;3������Ҫ��ֹͣ;4ѭ����������.
		inline int32_t read_loop(socket_t sock, recv_data_i& ri, uint32_t timeout_us = ms2us(500),
			event_rw_t* evt = NULL, void* evt_param = NULL, uint32_t loops = 0, uint32_t flag = 0)
		{
			uint32_t block_size = opt_buffsize(sock, true); //��ѯsocket�ĵײ㻺�����ߴ�
			if (block_size == (uint32_t)-1) return -1;

			uint32_t lps = 0;
			int r;
			while (loops == 0 || ++lps <= loops)            //����ѭ����ȡ
			{
				uint32_t buff_size = 0;
				uint8_t *buff = ri.get(block_size, buff_size); //�ӻ�������ȡ���ջ�����
				if (buff == NULL) return 3;

				if (timeout_us != (uint32_t)-1)
				{//���Խ��н���ǰ�ĳ�ʱ�ȴ�,��������ģʽ��������״̬
					r = wait(sock, timeout_us);
					if (r < 0)
						return -2;
					else if (r == 0)
						return 1;
				}

				r = recv(sock, buff, buff_size, flag);		//ִ�������Ľ��ն���
				if (r < 0)
					return -3;
				else if (r == 0)
					return 0;                               //���ն�����֪,�����Ѿ��Ͽ�

				bool is_break = !ri.put(r);                 //��֪���������ν��ճ���,��֪�Ƿ�����ж�ѭ��
				if (evt&&evt->is_valid())                   //�ٴγ���֪ͨ�¼��ص�:���ν��յ����ݺ����ݳ���,�ѽ��ճ���
					is_break = !(*evt)(sock, evt_param, buff, r, ri.size()) || is_break;  //�Ƿ���Ҫ�ж�ѭ�����ж���Ҫ����
				if (is_break)
					return 2;                               //�¼��򻺳���Ҫ��ֹͣ����
			}
			return 4;                                       //ѭ��������������
		}
		//�ɿ���ѭ�������͵������ȴ�ʱ��ĳ�����ȡ����:�������ͻ��峤��(����ʱ��¼ʵ�ʽ��ճ���),�Ƿ�������������;���εȴ���ʱus;���չ۲��¼����¼�����;���ѭ������(0����);��ȡ���.
		//����ֵ:��֪ѭ������ԭ��,<0������;0���ӶϿ�;1�ȴ���ʱ;2�¼�Ҫ��ֹͣ;3������Ҫ��ֹͣ;4ѭ����������.
		inline int32_t read_loop(socket_t sock, uint8_t* buff, uint32_t &maxsize, bool mustsize = false,
			uint32_t timeout_us = ms2us(500), event_rw_t* evt = NULL, void* evt_param = NULL, uint32_t loops = 0, uint32_t flag = 0)
		{
			recv_buff_i ri(buff, maxsize, mustsize);
			int r = read_loop(sock, ri, timeout_us, evt, evt_param, loops, flag);
			maxsize = ri.size();
			return r;
		}
		//-------------------------------------------------
		//ͳһ��д�ӿ�:���ݻ�����;���ݳ���;д�ȴ���ʱ
		//����ֵ:<0����;0��ʱ;>0�ɹ�
		inline int32_t write_loop(socket_t sock, const void* datas, uint32_t data_size,
			uint32_t timeout_us = ms2us(500), event_rw_t* evt = NULL, void* evt_param = NULL, uint32_t flag = 0)
		{
			uint32_t block_size = opt_buffsize(sock, false);  //��ѯsocket�ĵײ㻺�����ߴ�
			if (block_size == (uint32_t)-1) return -1;

			uint32_t sended = 0;                              //�Ѿ�д��������ݳ���
			while (sended < data_size)                        //ѭ��д��ȫ��������
			{
				uint32_t dl = min(block_size, data_size - sended);//������Ҫд������ݳ���
				if (timeout_us != (uint32_t)-1)
				{//���Խ��з���ǰ�ĳ�ʱ�ȴ�,����ײ㻺����������
					int r = wait(sock, timeout_us, false);
					if (r < 0) return -2;
					else if (r == 0) return 0;
				}
				uint8_t *buff = (uint8_t*)datas + sended;     //�����ͻ�����ָ��
				int32_t rl = send(sock, buff, dl, flag);      //�����ķ��Ͷ���
				if (rl < 0) return -3;
				if (evt&&evt->is_valid())
					(*evt)(sock, evt_param, buff, rl, sended);//֪ͨ�¼��ص�:���η��͵����ݺ����ݳ���,�ѷ������ݵ��ܳ���

				sended += rl;                                 //����������ʵд��������ݳ���
			}
			return sended;
		}
		}

	//-----------------------------------------------------
	//����һ��socket���������й���,��Ҫ��Ŀ����Ϊ���������������ر�socket
	class sock_t
	{
		socket_t m_sock;
	public:
		sock_t() :m_sock(bad_socket) {}
		sock_t(socket_t s) :m_sock(s) {}
		operator socket_t() const
		{
			return m_sock;
		}
		sock_t& operator=(socket_t s)
		{
			m_sock = s;
			return *this;
		}
		virtual ~sock_t()
		{
			if (m_sock == bad_socket)
				return;
			sock::close(m_sock, true);
			m_sock = bad_socket;
		}
	};
	//-----------------------------------------------------
	//����ͨ�ŵ�Ŀ���ַ,���Ի�ȡ��Ӧ�ı���IP(�����ݱ���·�ɱ�,���ж�Ŀ���Ƿ��������,UDPģʽ)
	//���:DestIPĿ���ַ
	//����ֵ:INADDR_NONE��ѯʧ��;����Ϊ����ip
	inline uint32_t localip_by_dest(uint32_t DestIP)
	{
		sock_t s = sock::create(false);                       //����udp socket�������й�
		if (s == bad_socket)
			return INADDR_NONE;

		sock_addr_t dst_addr(DestIP, 1);                     //����Ŀ���ַ,����עĿ��˿ڵ�����
		if (sock::connect(s, dst_addr, 0) <= 0)                 //�ؼ�,��socket�ײ��ѯ����·�ɱ�,����Ŀ�����
			return INADDR_NONE;

		if (!sock::local_addr(s, dst_addr))                  //���ڿ��Ի�ȡ��socket�ı�����ַ��
			return INADDR_NONE;
		return dst_addr.ip_addr();
	}
	inline uint32_t localip_by_dest(const char* DestIP)
	{
		uint32_t DstIP = sock_addr_t::lookup(DestIP);
		if (DstIP == INADDR_NONE)
			return INADDR_NONE;
		return localip_by_dest(DstIP);
	}
	inline char* localip_by_dest(const char* DestIP, ip_str_t &lip)
	{
		uint32_t ip = localip_by_dest(DestIP);
		if (ip == INADDR_NONE)
			return NULL;
		sock_addr_t::to_str(ip, lip);
		return lip.addr;
	}
	//-------------------------------------------------
	//�жϸ����ĵ�ַ�Ƿ�Ϊ������ַ
	inline bool is_local_ip(const char* Host)
	{
		uint32_t DstIP = sock_addr_t::lookup(Host);
		if (DstIP == INADDR_NONE)
			return false;
		return DstIP == localip_by_dest(DstIP);
	}
	//-------------------------------------------------
	//TCP�˿����Ӳ���
	inline bool tcp_conn_test(const char* dest, uint32_t port, uint32_t timeout_us = ms2us(500))
	{
		sock_t s = sock::create();                            //����tcp socket���й�
		if (s == bad_socket)
			return false;
		return sock::connect(s, dest, port, timeout_us) > 0;     //��������Ŀ��
	}
	//-------------------------------------------------
	//ʹ��tcp����Ŀ���,��ȡ���ض�Ӧ��ip��ַ��
	inline char* localip_by_dest(const char* dest, uint32_t port, ip_str_t &lip, uint32_t timeout_us = ms2us(500))
	{
		sock_t s = sock::create();                            //����tcp socket���й�
		if (s == bad_socket)
			return NULL;
		if (sock::connect(s, dest, port, timeout_us) <= 0)       //��������Ŀ��
			return NULL;
		return sock::local_ip(s, lip);
	}
		}

#endif
