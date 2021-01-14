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
#define RX_CHECK_WINSOCK2() WSAHtonl(0,0,0)   //语句本身无意义,仅告知需要链接ws2_32.lib,使用winsock2的API.

namespace rx
{
	//定义全局socket环境管理器.在win上使用时必须进行socket环境的初始化.
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
	//定义简单的IP地址串存储空间
	typedef struct ip_str_t
	{
		char addr[16];                                      //暂时先仅考虑ipv4地址空间
		ip_str_t() { addr[0] = 0; }
		operator char* ()const { return (char*)addr; }
		const char* c_str()const { return (char*)addr; }
	}ip_str_t;

	//-----------------------------------------------------
	//封装socket地址相关功能
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
		sock_addr_t(uint32_t Host, int Port)                //Host是网络字节序的IP地址
		{
			clear();
			m_sa.sin_port = htons(Port);
			m_sa.sin_addr.s_addr = Host;
		}
		sock_addr_t(const struct sockaddr_in& Addr) :m_sa(Addr) {}
		//-------------------------------------------------
		//清理地址信息
		void clear()
		{
			memset(&m_sa, 0, sizeof(struct sockaddr_in));
			m_sa.sin_family = AF_INET;						//IP地址族
			m_sa.sin_port = htons(0);						//struct sockaddr_in.sin_port需要网络字节序的端口
			m_sa.sin_addr.s_addr = INADDR_ANY;				//struct sockaddr_in.sin_addr.s_addr需要的是网络字节序的IP地址
		}
		//-------------------------------------------------
		//判断两个地址信息是否相同
		bool operator==(const sock_addr_t& addr)
		{
			return m_sa.sin_port == addr.m_sa.sin_port &&
				m_sa.sin_addr.s_addr == addr.m_sa.sin_addr.s_addr;
		}
		//-------------------------------------------------
		//判断当前内部地址是否有效
		bool is_valid() { return m_sa.sin_addr.s_addr != INADDR_NONE&&m_sa.sin_port != 0; }
		//-------------------------------------------------
		//设置主机字节序的端口号
		bool set_port(uint16_t P) { m_sa.sin_port = htons(P); return true; }
		//-------------------------------------------------
		//得到主机字节序的端口号
		const uint16_t port() const { return ntohs(m_sa.sin_port); }
		//-------------------------------------------------
		//使用IP串或域名来设置地址;顺带可以设置端口
		//返回值:设置的IP串或域名是否合法
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
		//得到网络字节序的IP地址
		uint32_t ip_addr() { return m_sa.sin_addr.s_addr; }
		//-------------------------------------------------
		//得到IP地址串:缓冲区;缓冲区尺寸.返回值:NULL失败,其他为缓冲区指针
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
		//转换网络字节序IP地址到字符串.返回值:串长度;0缓冲区不足.
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
		//根据socket地址得到IP地址串.返回值:串长度;0缓冲区不足.
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
		//根据IP串或域名得到对应的网络字节序的IP地址(gethostbyname存在多线程阻塞的问题,可以使用其他DNS客户端或缓存方案)
		//返回值:INADDR_NONE失败;其他成功
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
		//将IP串转换为IP值
		//返回值:INADDR_NONE失败;其他成功
		static uint32_t to_ip(const char* str)
		{
			uint32_t addr;
			if (inet_pton(AF_INET, str, &addr) > 0)
				return addr;
			return INADDR_NONE;
		}
	};

	//-----------------------------------------------------
	//对socket集合进行简单管理,便于获取nfds
	class sock_sets
	{
		fd_set          m_sets;
		uint32_t        m_size;
		socket_t        m_nfds;
	public:
		//-------------------------------------------------
		sock_sets() { reset(); }
		//-------------------------------------------------
		//复位,准备重新填充
		void reset() { FD_ZERO(&m_sets); m_size = 0; m_nfds = 0; }
		//-------------------------------------------------
		//获取集合的最大容量与当前元素数量
		static uint32_t capacity() { return FD_SETSIZE; }
		uint32_t size() { return m_size; }
		//-------------------------------------------------
		//放入一个元素
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
		//放入集合的socket最大值
		socket_t nfds() { return m_nfds + 1; }
		//-------------------------------------------------
		operator fd_set* () { return &m_sets; }
		operator fd_set& () { return m_sets; }
		//-------------------------------------------------
		//在进行了select之后,仅有此方法可用,判断给定的sock是否还在集合中
		bool contain(socket_t sock) { return FD_ISSET(sock, &m_sets) != 0; }
	};

	//对常用socket相关功能函数进行封装
	namespace sock
	{
		//-------------------------------------------------
		//创建一个socket句柄:是否创建为TCP套接字;否则就是创建UDP套接字.
		//返回值:bad_socket失败;其他为可用套接字
		inline socket_t create(bool tcp = true)
		{
			if (tcp)
				return socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			else
				return socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
		}
		//-------------------------------------------------
		//使用明确的参数创建指定的socket
		inline socket_t create_raw(int proto = IPPROTO_ICMP, int type = SOCK_RAW, int af = AF_INET)
		{
			return socket(af, type, proto);
		}
		//-------------------------------------------------
		//将这个socket与一个地址绑定起来
		inline bool bind(socket_t sock, const sock_addr_t& sa)
		{
			RX_CHECK_WINSOCK2();
			return 0 == bind(sock, (struct sockaddr *)&sa.addr(), sizeof(sa.addr()));
		}
		//-------------------------------------------------
		//将这个socket与一个地址和端口绑定起来(默认与本机的全部接口ip的指定端口绑定)
		inline bool bind(socket_t sock, uint16_t port, const char* host = NULL)
		{
			if (is_empty(host))
				host = "0.0.0.0";
			sock_addr_t Addr(host, port);
			return bind(sock, Addr);
		}
		//-------------------------------------------------
		//在指定的socket上启动监听
		inline bool listen(socket_t sock, int backlog = SOMAXCONN)
		{
			return 0 == ::listen(sock, backlog);
		}
		//-------------------------------------------------
		//在指定的socket上进行接受连接操作,同时得到客户端的地址信息
		//返回值:新连接socket,或bad_socket
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
		//在这个socket上进行接受连接操作:同时得到客户端的地址信息
		inline socket_t accept(socket_t sock)
		{
			sock_addr_t sa;
			return accept(sock, sa);
		}
		//-------------------------------------------------
		//在指定的sock集合上等待事件,io多路复用:set_r读集;set_w写集;set_e错误集;超时时间微秒(-1完全阻塞);nfds告知全部集合中的最大描述符+1
		//返回值:<0错误;0超时;>0有事件发生
		inline int32_t select(fd_set* set_r, fd_set* set_w, fd_set *set_e, uint32_t timeout_us, int nfds)
		{
			struct timeval tm;
			tm.tv_sec = timeout_us / sec2us(1);
			tm.tv_usec = timeout_us%sec2us(1);
			return select(nfds, set_r, set_w, set_e, (timeout_us != (uint32_t)-1 ? &tm : NULL));
		}
		//-------------------------------------------------
		//等待socket集有可读写元素
		//返回值:<0错误;0超时;>0有事件发生
		inline int32_t select(fd_set& set, uint32_t timeout_us, int nfds, bool is_read = true)
		{
			if (is_read)
				return select(&set, NULL, NULL, timeout_us, nfds);
			else
				return select(NULL, &set, NULL, timeout_us, nfds);
		}
		//等待socket扩展集有可读写元素
		//返回值:<0错误;0超时;>0有事件发生
		inline int32_t select(sock_sets& set, uint32_t timeout_us, bool is_read = true)
		{
			if (is_read)
				return select((fd_set*)set, NULL, NULL, timeout_us, (int)set.nfds());
			else
				return select(NULL, (fd_set*)set, NULL, timeout_us, (int)set.nfds());
		}
		//-------------------------------------------------
		//等待指定的socket可读或可写.
		//返回值:<0错误;0超时;1可读写
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
		//对这个socket进行控制操作
		inline bool ioctrl(socket_t sock, uint32_t cmd, uint32_t& args)
		{
			#if RX_IS_OS_WIN
			return 0 == ioctlsocket(sock, cmd, (u_long*)&args);
			#else
			return 0 == ::ioctl(sock, cmd, &args);
			#endif
	}
		//-------------------------------------------------
		//在这个socket上设置选项:选项等级(SOL_SOCKET或IPPROTO_TCP);选项编码;选项值,值长度
		inline bool opt_set(socket_t sock, int OptionLevel, int Option, const void* OptionValue, int OptionValueLen)
		{
			return 0 == setsockopt(sock, OptionLevel, Option, (const char*)OptionValue, OptionValueLen);
		}
		//-------------------------------------------------
		//得到这个socket上制定选项的值:选项等级(SOL_SOCKET或IPPROTO_TCP);选项编码;选项值缓冲区,缓冲区长度/值长度
		inline bool opt_get(socket_t sock, int OptionLevel, int Option, void* OptionValue, int &OptionValueLen)
		{
			return 0 == getsockopt(sock, OptionLevel, Option, (char*)OptionValue, (socklen_t*)&OptionValueLen);
		}
		//-------------------------------------------------
		//查询指定socket上是否有错误
		inline bool opt_get_err(socket_t sock, int &err)
		{
			err = 0;
			int s = sizeof(int);
			return opt_get(sock, SOL_SOCKET, SO_ERROR, &err, s);
		}
		//-------------------------------------------------
		//设置socket为阻塞或非阻塞模式
		inline bool block_mode(socket_t sock, bool is_block)
		{
			uint32_t v = is_block ? 0 : 1;
			return ioctrl(sock, FIONBIO, v);
		}
		//-------------------------------------------------
		//查询接收缓冲区内可读取数据长度
		//返回值:<0错误;0没有数据;>0是可读取的数据长度
		inline int32_t readable_size(socket_t sock)
		{
			uint32_t N = 0;
			if (ioctrl(sock, FIONREAD, N))
				return (int32_t)N;
			return -1;
		}
		//-------------------------------------------------
		//增强的连接操作,可超时等待:连接的目标地址;需要等待的时间(-1使用系统默认行为)
		//返回值:1成功;0连接超时;<0错误:-1非阻塞模式设置错误;-2阻塞模式恢复错误;-3连接错误;-4select操作失败
		inline int32_t connect(socket_t sock, const sock_addr_t& sa, uint32_t timeout_us)
		{
			if (timeout_us == (uint32_t)-1)
			{//不需要进行连接超时的控制,使用系统内默认的链接超时等待时间
				if (0 == connect(sock, (struct sockaddr *)&sa.addr(), sizeof(sa.addr())))
					return 1;
				else return -3;                             //连接失败
			}

			//需要进行连接超时的控制
			if (!block_mode(sock, false))
				return -1;                                  //将socket设置为非阻塞模式

			int32_t cr = 0;
			if (0 != ::connect(sock, (struct sockaddr *)&sa.addr(), sizeof(sa.addr())))
				cr = wait(sock, timeout_us, false);             //进行非阻塞连接并等待可写(连接成功)

			#if !RX_IS_OS_WIN
			if (cr > 0)
			{//在linux上,需要再次判断sock内部是否有错误
				int err;
				opt_get_err(sock, err);
				if (err != 0)
				{//修正错误的原因
					errno = err;
					cr = -3;
		}
}
			#endif

			if (!block_mode(sock, true))                     //将socket恢复为阻塞模式
				return -2;
			if (cr < 0)
				return -4;                                  //判断select状态,作为连接结果
			return cr;
		}
		//将socket连接到指定的主机地址和端口
		inline int32_t connect(socket_t sock, const char* host, uint16_t port, uint32_t timeout_us)
		{
			sock_addr_t sa(host, port);
			return connect(sock, sa, timeout_us);
		}
		//-------------------------------------------------
		//关闭指定的socket.可以指定强制立即关闭,不进入time_wait状态
		inline bool close(socket_t &sock, bool no_wait = false)
		{
			if (sock == (socket_t)bad_socket) return true;

			if (no_wait)
			{//不需要进行关闭等待,那么就直接关闭
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
				shutdown(sock, SD_SEND);                     //关闭时等待发送完成
				#else
				shutdown(sock, SHUT_WR);                     //关闭时等待发送完成
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
		//在socket上查看数据:缓冲区;缓冲区长度;
		//返回值:<发生了错误;0说明连接断开;>0为数据长度
		inline int32_t peek(socket_t sock, uint8_t* Buf, uint32_t BufLen)
		{
			return ::recv(sock, (char*)Buf, BufLen, MSG_PEEK);
		}
		//-------------------------------------------------
		//在这个socket上接收数据:缓冲区;缓冲区长度;操作标记(MSG_PEEK查看模式);
		//返回值:<0发生了错误;0说明连接断开;>0为收到的数据长度
		inline int32_t recv(socket_t sock, uint8_t* Buf, uint32_t BufLen, int Flag = 0)
		{
			return ::recv(sock, (char*)Buf, BufLen, Flag);
		}
		//-------------------------------------------------
		//期待发送:缓冲区;待发送的数据长度;发送操作标记
		//返回值:<0有错误;>=0告知实际发送的长度(因为底层缓冲区的剩余空间可能不够大);
		inline int32_t send(socket_t sock, const void* Buf, uint32_t BufLen, int Flag = 0)
		{
			return ::send(sock, (char*)Buf, BufLen, Flag);
		}
		//-------------------------------------------------
		//在这个socket上接收数据:缓冲区;缓冲区长度;该数据来源地址;接收到的数据长度;操作标记;
		//返回值:<0发生了错误;>=0告知接收长度.
		inline int32_t recvfrom(socket_t sock, uint8_t* Buf, uint32_t BufLen, sock_addr_t& sa, int Flag = 0)
		{
			int AddrLen = sizeof(sa.addr());
			return ::recvfrom(sock, (char*)Buf, BufLen, Flag, (struct sockaddr *)&sa.addr(), (socklen_t*)&AddrLen);
		}
		//-------------------------------------------------
		//期待发送:缓冲区;待发送的数据长度;发送到目标地址;发送操作标记
		//返回值:<0有错误;>=0告知实际发送的长度(因为底层缓冲区的剩余空间可能不够大);
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
		//获取指定socket的本端地址
		inline bool local_addr(socket_t sock, sock_addr_t& sa)
		{
			socklen_t AddrSize = sizeof(sa.addr());
			return 0 == getsockname(sock, (struct sockaddr*)&sa.addr(), &AddrSize);
		}
		//获取指定socket的本地端口
		inline uint32_t local_port(socket_t sock)
		{
			sock_addr_t Addr;
			if (!local_addr(sock, Addr))
				return 0;
			return Addr.port();
		}
		//-------------------------------------------------
		//得到当前连接上的本机端IP
		inline char* local_ip(socket_t sock, ip_str_t &str)
		{
			sock_addr_t sa;
			if (!local_addr(sock, sa))
				return NULL;
			return sa.ip_str(str.addr, sizeof(str));
		}
		//-------------------------------------------------
		//得到指定socket上对方地址
		inline bool peer_addr(socket_t sock, sock_addr_t& sa)
		{
			socklen_t AddrSize = sizeof(sa.addr());
			return 0 == getpeername(sock, (struct sockaddr*)&sa.addr(), &AddrSize);
		}
		//-------------------------------------------------
		//得到指定socket上对方的IP地址串
		inline char* peer_ip(socket_t sock, ip_str_t &str)
		{
			sock_addr_t sa;
			if (!peer_addr(sock, sa))
				return NULL;
			return sa.ip_str(str.addr, sizeof(str));
		}
		//-------------------------------------------------
		//获取该socket上对应的对方端口
		//返回值:0失败;其他为端口号
		inline uint16_t peer_port(socket_t sock)
		{
			sock_addr_t sa;
			if (!peer_addr(sock, sa))
				return 0;
			return sa.port();
		}
		//-------------------------------------------------
		//设置socket默认的读写超时等待时长
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
		//获取当前socket的系统收发缓冲区大小
		//返回值:-1出错;其他为结果
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
		//设置当前socket的系统收发缓冲区大小
		inline bool opt_buffsize(socket_t sock, uint32_t Size, bool is_read)
		{
			int o = is_read ? SO_RCVBUF : SO_SNDBUF;
			int t = sizeof(Size);
			return opt_set(sock, SOL_SOCKET, o, &Size, t);
		}
		//-------------------------------------------------
		//设置是否可以重复使用端口地址
		inline bool opt_reuse_addr(socket_t sock, bool reuse)
		{
			int option = reuse ? 1 : 0;
			return opt_set(sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
		}
		//-------------------------------------------------
		//是否开启链路维持保持连接不要中断.但此模式使用默认参数,间隔探测时间太久了,意义不大.
		inline bool opt_keepalive(socket_t sock, bool Enable)
		{
			int option = Enable ? 1 : 0;
			return opt_set(sock, SOL_SOCKET, SO_KEEPALIVE, &option, sizeof(option));
		}
		//开启链路维持的高级模式,可以指定空闲间隔时间和探测包发送间隔时间
		inline bool opt_keepalive_ex(socket_t sock, bool Enable, uint32_t idle_ms, uint32_t reinterval_ms = 1000)
		{
			#if RX_IS_OS_WIN
			struct tcp_keepalive keepin;
			struct tcp_keepalive keepout;

			keepin.keepaliveinterval = reinterval_ms;       //探测包发送间隔时间
			keepin.keepalivetime = idle_ms;                 //链路上超过idle_ms没有传输数据，就自动发送探测包
			keepin.onoff = Enable ? 1 : 0;
			uint32_t keepout_len;
			return 0 == WSAIoctl(sock, SIO_KEEPALIVE_VALS, &keepin, sizeof(keepin), &keepout, sizeof(keepout), (DWORD*)&keepout_len, NULL, NULL);
			#else
			uint32_t keepAlive = Enable ? 1 : 0;
			if (!opt_set(sock, SOL_SOCKET, SO_KEEPALIVE, (void*)&keepAlive, sizeof(keepAlive)))
				return false;

			if (!Enable) return false;

			uint32_t keepIdle = idle_ms / 1000;             //如果在keepIdle秒内没有任何数据交互,则进行探测. 缺省值:7200(s)
			if (keepIdle == 0) keepIdle = 1;
			if (!opt_set(sock, IPPROTO_TCP, TCP_KEEPIDLE, (void*)&keepIdle, sizeof(keepIdle)))
				return false;

			uint32_t keepInterval = reinterval_ms / 1000;   //探测时发探测包的时间间隔为keepInterval秒. 缺省值:75(s)
			if (reinterval_ms == 0) reinterval_ms = 1;
			if (!opt_set(sock, IPPROTO_TCP, TCP_KEEPINTVL, (void*)&keepInterval, sizeof(keepInterval)))
				return false;

			uint32_t keepCount = 3;                         //探测重试的次数. 全部超时则认定连接失效..缺省值:9(次)
			if (!opt_set(sock, IPPROTO_TCP, TCP_KEEPCNT, (void*)&keepCount, sizeof(keepCount)))
				return false;
			return true;
			#endif
		}
		//-------------------------------------------------
		//开启socket的广播模式
		inline bool opt_broadcast(socket_t sock, bool Enable = true)
		{
			int flag = Enable ? 1 : 0;
			return setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&flag, sizeof(flag)) == 0;
		}
		//-------------------------------------------------
		//组播回环控制(是否接收本机发来的组播消息)
		inline bool group_loopback(socket_t sock, bool V)
		{
			uint8_t LV = V ? 1 : 0;
			return setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP, (char*)&LV, sizeof(LV)) == 0;
		}
		//-------------------------------------------------
		//处理socket的组播模式:根据IP,选定组播使用的网卡
		inline bool group_interface(socket_t sock, const char* LocalIP)
		{
			struct in_addr addr;
			addr.s_addr = sock_addr_t::lookup(LocalIP);
			return 0 == setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, (char*)&addr, sizeof(addr));
		}
		//-------------------------------------------------
		//处理socket的组播模式:加入一个组
		inline bool group_add(socket_t sock, const char* GroupAddr, const char* LocalIP = NULL)
		{
			struct ip_mreq M;
			M.imr_multiaddr.s_addr = sock_addr_t::lookup(GroupAddr);
			M.imr_interface.s_addr = INADDR_ANY;
			if (LocalIP)
				M.imr_interface.s_addr = sock_addr_t::lookup(LocalIP);
			return setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&M, sizeof(M)) == 0;
		}
		//处理socket的组播模式:离开一个组
		inline bool group_del(socket_t sock, const char* GroupAddr, const char* LocalIP = NULL)
		{
			struct ip_mreq M;
			M.imr_multiaddr.s_addr = sock_addr_t::lookup(GroupAddr);
			M.imr_interface.s_addr = INADDR_ANY;
			if (LocalIP)
				M.imr_interface.s_addr = sock_addr_t::lookup(LocalIP);
			return setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&M, sizeof(M)) == 0;
		}
		//从指定的socket上获取本地ip端口以及远程ip端口信息
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
		//定义收发数据事件的委托类型:sock,sock用户数据,本次数据缓冲区,本次数据长度,总长度,返回值告知是否继续
		typedef delegate5_t<socket_t, void*, uint8_t*, uint32_t, uint32_t, bool> event_rw_t;

		//持续化接收数据接口
		class recv_data_i
		{
		public:
			//申请接收缓冲区:期待的最大缓冲区长度;实际可用缓冲区长度;返回值:NULL失败;其他成功
			virtual uint8_t* get(uint32_t maxsize, uint32_t &realsize) = 0;
			//告知缓冲区实际使用长度;返回值:是否继续接收
			virtual bool put(uint32_t datasize) = 0;
			//已经接收的数据尺寸
			virtual uint32_t size() = 0;
		};

		//-------------------------------------------------
		//持续接收数据的缓冲器
		class recv_buff_i :public recv_data_i
		{
		protected:
			uint8_t     *m_buff;                            //接收缓冲区
			uint32_t    m_maxsize;                          //最大接收长度
			bool        m_must;                             //是否必须接满最大长度
			uint32_t    m_size;                             //已经接收的长度
		public:
			//绑定接收缓冲区:接收缓冲区,最大接收长度,是否必须接满.
			recv_buff_i(uint8_t* buff, uint32_t maxsize, bool mustsize = false) :m_buff(buff), m_maxsize(maxsize), m_must(mustsize), m_size(0) {}
			//申请接收缓冲区:期待的最大缓冲区长度;实际可用缓冲区长度;返回值:NULL失败;其他成功
			uint8_t* get(uint32_t maxsize, uint32_t &realsize)
			{
				realsize = min(m_maxsize - m_size, maxsize);
				return m_buff + m_size;
			}
			//接收完成,告知实际接收数据长度;返回值:是否继续接收
			bool put(uint32_t datasize)
			{
				m_size += datasize;
				rx_assert(m_size <= m_maxsize);
				if (m_must&&m_size == m_maxsize)
					return false;
				return true;
			}
			//已经接收的数据尺寸
			uint32_t size() { return m_size; }
			//缓冲器复位,准备重新接收使用
			void reset() { m_size = 0; }
		};

		//-------------------------------------------------
		//尝试等待接收到tag就结束
		class recv_tag_i :public recv_buff_i
		{
		public:
			uint8_t     *tag;
			uint32_t    tagsize;
			uint32_t    tagpos;
			recv_tag_i(uint8_t* buff, uint32_t maxsize, bool mustsize = false) :recv_buff_i(buff, maxsize, mustsize), tag(NULL), tagsize(0), tagpos(-1) {}
			//接收完成,告知实际接收数据长度;返回值:是否继续接收
			bool put(uint32_t datasize)
			{
				bool ic = recv_buff_i::put(datasize);
				if (recv_buff_i::m_size > tagsize)
				{//缓冲区足够长了,判断是否含有tag
					rx_assert_if(tagsize, tag != NULL);
					const uint8_t* pos = st::memmemx(recv_buff_i::m_buff, recv_buff_i::m_size, tag, tagsize);
					if (pos)
					{//接收缓冲区中确实含有tag了,可以结束接收了.
						tagpos = uint32_t(pos - recv_buff_i::m_buff); //tagpos记录tag出现在buff中的位置
						return false;
					}
				}
				if (!ic)
					return false;                           //缓冲区满了,不能再继续接收了
				return true;
			}
		};

		//-------------------------------------------------
		//可控制循环数量和单次最大等待时间的持续读取功能:读取缓冲器;单次等待超时us;接收观察事件和事件参数;最大循环次数(0不限);读取标记.
		//返回值:告知循环结束原因,<0错误了;0连接断开;1等待超时;2事件要求停止;3缓冲器要求停止;4循环到达上限.
		inline int32_t read_loop(socket_t sock, recv_data_i& ri, uint32_t timeout_us = ms2us(500),
			event_rw_t* evt = NULL, void* evt_param = NULL, uint32_t loops = 0, uint32_t flag = 0)
		{
			uint32_t block_size = opt_buffsize(sock, true); //查询socket的底层缓冲区尺寸
			if (block_size == (uint32_t)-1) return -1;

			uint32_t lps = 0;
			int r;
			while (loops == 0 || ++lps <= loops)            //进行循环读取
			{
				uint32_t buff_size = 0;
				uint8_t *buff = ri.get(block_size, buff_size); //从缓冲器获取接收缓冲区
				if (buff == NULL) return 3;

				if (timeout_us != (uint32_t)-1)
				{//尝试进行接收前的超时等待,避免阻塞模式进入死等状态
					r = wait(sock, timeout_us);
					if (r < 0)
						return -2;
					else if (r == 0)
						return 1;
				}

				r = recv(sock, buff, buff_size, flag);		//执行真正的接收动作
				if (r < 0)
					return -3;
				else if (r == 0)
					return 0;                               //接收动作告知,连接已经断开

				bool is_break = !ri.put(r);                 //告知缓冲器本次接收长度,获知是否可以中断循环
				if (evt&&evt->is_valid())                   //再次尝试通知事件回调:本次接收的数据和数据长度,已接收长度
					is_break = !(*evt)(sock, evt_param, buff, r, ri.size()) || is_break;  //是否需要中断循环的判断需要叠加
				if (is_break)
					return 2;                               //事件或缓冲器要求停止接收
			}
			return 4;                                       //循环次数到达上限
		}
		//可控制循环数量和单次最大等待时间的持续读取功能:缓冲区和缓冲长度(返回时记录实际接收长度),是否必须接满缓冲区;单次等待超时us;接收观察事件和事件参数;最大循环次数(0不限);读取标记.
		//返回值:告知循环结束原因,<0错误了;0连接断开;1等待超时;2事件要求停止;3缓冲器要求停止;4循环到达上限.
		inline int32_t read_loop(socket_t sock, uint8_t* buff, uint32_t &maxsize, bool mustsize = false,
			uint32_t timeout_us = ms2us(500), event_rw_t* evt = NULL, void* evt_param = NULL, uint32_t loops = 0, uint32_t flag = 0)
		{
			recv_buff_i ri(buff, maxsize, mustsize);
			int r = read_loop(sock, ri, timeout_us, evt, evt_param, loops, flag);
			maxsize = ri.size();
			return r;
		}
		//-------------------------------------------------
		//统一的写接口:数据缓冲区;数据长度;写等待超时
		//返回值:<0错误;0超时;>0成功
		inline int32_t write_loop(socket_t sock, const void* datas, uint32_t data_size,
			uint32_t timeout_us = ms2us(500), event_rw_t* evt = NULL, void* evt_param = NULL, uint32_t flag = 0)
		{
			uint32_t block_size = opt_buffsize(sock, false);  //查询socket的底层缓冲区尺寸
			if (block_size == (uint32_t)-1) return -1;

			uint32_t sended = 0;                              //已经写入过的数据长度
			while (sended < data_size)                        //循环写入全部的数据
			{
				uint32_t dl = min(block_size, data_size - sended);//本次需要写入的数据长度
				if (timeout_us != (uint32_t)-1)
				{//尝试进行发送前的超时等待,避免底层缓冲区不够用
					int r = wait(sock, timeout_us, false);
					if (r < 0) return -2;
					else if (r == 0) return 0;
				}
				uint8_t *buff = (uint8_t*)datas + sended;     //待发送缓冲区指针
				int32_t rl = send(sock, buff, dl, flag);      //真正的发送动作
				if (rl < 0) return -3;
				if (evt&&evt->is_valid())
					(*evt)(sock, evt_param, buff, rl, sended);//通知事件回调:本次发送的数据和数据长度,已发送数据的总长度

				sended += rl;                                 //调整本次真实写入过的数据长度
			}
			return sended;
		}
		}

	//-----------------------------------------------------
	//定义一个socket的轻量级托管类,主要的目的是为了利用析构函数关闭socket
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
	//根据通信的目标地址,尝试获取对应的本地IP(仅依据本地路由表,不判断目标是否可以连接,UDP模式)
	//入口:DestIP目标地址
	//返回值:INADDR_NONE查询失败;其他为本地ip
	inline uint32_t localip_by_dest(uint32_t DestIP)
	{
		sock_t s = sock::create(false);                       //创建udp socket并进行托管
		if (s == bad_socket)
			return INADDR_NONE;

		sock_addr_t dst_addr(DestIP, 1);                     //构造目标地址,不关注目标端口的问题
		if (sock::connect(s, dst_addr, 0) <= 0)                 //关键,让socket底层查询本机路由表,进行目标关联
			return INADDR_NONE;

		if (!sock::local_addr(s, dst_addr))                  //现在可以获取本socket的本机地址了
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
	//判断给定的地址是否为本机地址
	inline bool is_local_ip(const char* Host)
	{
		uint32_t DstIP = sock_addr_t::lookup(Host);
		if (DstIP == INADDR_NONE)
			return false;
		return DstIP == localip_by_dest(DstIP);
	}
	//-------------------------------------------------
	//TCP端口连接测试
	inline bool tcp_conn_test(const char* dest, uint32_t port, uint32_t timeout_us = ms2us(500))
	{
		sock_t s = sock::create();                            //创建tcp socket并托管
		if (s == bad_socket)
			return false;
		return sock::connect(s, dest, port, timeout_us) > 0;     //尝试连接目标
	}
	//-------------------------------------------------
	//使用tcp连接目标后,提取本地对应的ip地址串
	inline char* localip_by_dest(const char* dest, uint32_t port, ip_str_t &lip, uint32_t timeout_us = ms2us(500))
	{
		sock_t s = sock::create();                            //创建tcp socket并托管
		if (s == bad_socket)
			return NULL;
		if (sock::connect(s, dest, port, timeout_us) <= 0)       //尝试连接目标
			return NULL;
		return sock::local_ip(s, lip);
	}
		}

#endif
