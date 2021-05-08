#ifndef _UT_NET_TCP_H_
#define _UT_NET_TCP_H_

#include "../rx_net_tcp_listener.h"
#include "../rx_net_tcp_client.h"
#include "../rx_os_thread.h"
#include "../rx_tdd.h"

namespace rx
{
	//-----------------------------------------------------
	//封装一个简单的用于测试的tcp回音服务器(用最少的代码,无动态内存分配,实现客户端发什么就回应什么,同时也是演示相关socket功能的使用)
	class tcp_echo_svr_t
	{
		typedef array_ft<tcp_session_t, 8> session_array_t; //限定最大并发会话数量

		tcp_listener_t      m_svr;                          //用于接受新连接的socket服务端功能
		session_array_t     m_sessions;                     //当前活动会话数组

		//-------------------------------------------------
		//查找遇到的第一个空槽位索引;-1未找到.
		uint32_t m_find_first_empty()
		{
			for (uint32_t i = 0; i < m_sessions.capacity(); ++i)
			{
				if (!m_sessions[i].connected())
					return i;
			}
			return -1;
		}
	public:
		//-------------------------------------------------
		tcp_echo_svr_t() {}
		~tcp_echo_svr_t() { uninit(); }
		logger_t& logger() { return m_svr.logger(); }
		//-------------------------------------------------
		//打开两个端口进行监听
		bool init(uint16_t port1 = 45601, uint16_t port2 = 45602)
		{
			uninit();
			if (!m_svr.open(port1, NULL, m_sessions.capacity() * 2) ||
				!m_svr.open(port2, NULL, m_sessions.capacity() * 2))
			{
				m_svr.close();
				return false;
			}
			return true;
		}
		//-------------------------------------------------
		//解除echo服务器
		void uninit()
		{
			m_svr.close();
			for (uint32_t i = 0; i < m_sessions.capacity(); ++i)
			{
				tcp_session_t &ss = m_sessions[i];
				if (ss.connected())
					ss.disconnect();
			}
		}
		//-------------------------------------------------
		//服务端单线程功能驱动方法,完成新连接建立,处理收发应答.
		//返回值:有效动作的数量(新连接建立/收发错误连接断开/收发完成)
		uint32_t step(uint32_t nc_timeout_us = 10, uint32_t wr_timeout_us = sec2us(3))
		{
			uint32_t ac = 0;

			uint32_t idx = m_find_first_empty();
			if (idx != (uint32_t)-1)
			{//还有空槽位可以承载新会话,则尝试接受新连接
				socket_t new_sock = bad_socket;
				sock_addr_t peer_addr;
				//驱动svrsock,进行新连接建立的处理
				tcp_listener_t::listener_t *ss = m_svr.step(new_sock, &peer_addr, nc_timeout_us);
				if (ss)
				{//新连接到达
					++ac;
					char addrstr[80];
					sock::addr_infos(new_sock, addrstr);
					m_svr.logger().debug("%s :: tip<accept new tcp server session.>", addrstr);
					//初始化绑定新的会话
					tcp_session_bind(m_sessions[idx], new_sock);
				}
			}

			uint8_t wr_buff[1024 * 64];						//收发临时使用的缓冲区

			for (uint32_t i = 0; i < m_sessions.capacity(); ++i)
			{//对已连接会话进行收发处理
				tcp_session_t &ss = m_sessions[i];
				if (!ss.connected())
					continue;

				//0超时等待,快速进行接收探察
				uint32_t rs = ss.readx(wr_buff, sizeof(wr_buff), 0);
				if (rs == 0)
				{
					if (!ss.connected())
					{//连接中断了
						++ac;
						continue;
					}
				}
				else
				{//收到数据了,原样发送给对方
					++ac;
					if (ss.write(wr_buff, rs, wr_timeout_us) != ec_ok)
					{//发送错误,连接中断了
						continue;
					}
				}
			}
			return ac;
		}
	};

	//-----------------------------------------------------
	//对echo进行测试的客户端
	inline void ut_tcp_echo_clt(rx_tdd_t& rt, uint32_t loop = 100)
	{
		tcp_client_t    clt;
		rt.tdd_assert(clt.dst_addr("127.0.0.1", 45601));
		if (!rt.tdd_assert(clt.connect()))
			return;

		uint8_t tmp[1024 * 4];
		uint8_t tmp2[sizeof(tmp)];
		for (uint32_t i = 0; i < loop; ++i)
		{
			error_t e = clt.write(tmp, sizeof(tmp), sec2us(5));
			if (!rt.tdd_assert(e == ec_ok))
				break;

			e = clt.read(tmp2, sizeof(tmp2), sec2us(5));
			if (!rt.tdd_assert(e == ec_ok))
				break;

			if (!rt.tdd_assert(0 == memcmp(tmp, tmp2, sizeof(tmp))))
				break;
		}
		clt.disconnect(false);
	}
	//-----------------------------------------------------
	//对tcp echo服务端进行测试的简单函数
	inline void ut_tcp_echo_svr(rx_tdd_t& rt, uint32_t loop = 100000)
	{
		tcp_echo_svr_t svr;
		svr.logger().bind(make_logger_con());
		if (!rt.tdd_assert(svr.init()))
			return;

		for (uint32_t i = 0; i < loop; ++i)
		{
			uint32_t r = svr.step(ms2us(1));
			if (r)
				svr.logger().debug("ut_tcp_echo_svr step %d/%d", i + 1, loop);
		}
	}
	//-----------------------------------------------------
	//对tcp echo服务端进行测试的线程
	class ut_tcpsvr_thd_t :public thread_ex_t
	{
		uint32_t on_run(void *param)
		{
			rx_tdd_t &rt = *(rx_tdd_t*)param;

			tcp_echo_svr_t svr;
			svr.logger().bind(make_logger_con());

			if (!rt.tdd_assert(svr.init()))
				return 1;

			while (!need_break())
				svr.step();

			return 0;
		}
	};
	//-----------------------------------------------------
	//对tcp echo服务器进行完整测试的用例函数
	inline void ut_tcp_echo_thd(rx_tdd_t& rt, uint32_t loop = 3, uint32_t count = 3)
	{
		ut_tcpsvr_thd_t thd;
		rt.tdd_assert(thd.start(&rt));                      //创建了线程并启动
		rx_sleep(5);										//保险起见,等一下,让服务线程被执行
		for (uint32_t i = 0; i < count; ++i)
			ut_tcp_echo_clt(rt, loop);                       //调用客户端测试循环
		thd.stop(true);                                     //结束线程
	}
}

rx_tdd(test_net_tcp_base)
{
	for (uint32_t i = 0; i < 5; ++i)
		rx::ut_tcp_echo_thd(*this);
}

#endif // _UT_MEM_POOL_H_
