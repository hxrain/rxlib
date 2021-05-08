#ifndef _UT_NET_TCP_H_
#define _UT_NET_TCP_H_

#include "../rx_net_tcp_listener.h"
#include "../rx_net_tcp_client.h"
#include "../rx_os_thread.h"
#include "../rx_tdd.h"

namespace rx
{
	//-----------------------------------------------------
	//��װһ���򵥵����ڲ��Ե�tcp����������(�����ٵĴ���,�޶�̬�ڴ����,ʵ�ֿͻ��˷�ʲô�ͻ�Ӧʲô,ͬʱҲ����ʾ���socket���ܵ�ʹ��)
	class tcp_echo_svr_t
	{
		typedef array_ft<tcp_session_t, 8> session_array_t; //�޶���󲢷��Ự����

		tcp_listener_t      m_svr;                          //���ڽ��������ӵ�socket����˹���
		session_array_t     m_sessions;                     //��ǰ��Ự����

		//-------------------------------------------------
		//���������ĵ�һ���ղ�λ����;-1δ�ҵ�.
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
		//�������˿ڽ��м���
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
		//���echo������
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
		//����˵��̹߳�����������,��������ӽ���,�����շ�Ӧ��.
		//����ֵ:��Ч����������(�����ӽ���/�շ��������ӶϿ�/�շ����)
		uint32_t step(uint32_t nc_timeout_us = 10, uint32_t wr_timeout_us = sec2us(3))
		{
			uint32_t ac = 0;

			uint32_t idx = m_find_first_empty();
			if (idx != (uint32_t)-1)
			{//���пղ�λ���Գ����»Ự,���Խ���������
				socket_t new_sock = bad_socket;
				sock_addr_t peer_addr;
				//����svrsock,���������ӽ����Ĵ���
				tcp_listener_t::listener_t *ss = m_svr.step(new_sock, &peer_addr, nc_timeout_us);
				if (ss)
				{//�����ӵ���
					++ac;
					char addrstr[80];
					sock::addr_infos(new_sock, addrstr);
					m_svr.logger().debug("%s :: tip<accept new tcp server session.>", addrstr);
					//��ʼ�����µĻỰ
					tcp_session_bind(m_sessions[idx], new_sock);
				}
			}

			uint8_t wr_buff[1024 * 64];						//�շ���ʱʹ�õĻ�����

			for (uint32_t i = 0; i < m_sessions.capacity(); ++i)
			{//�������ӻỰ�����շ�����
				tcp_session_t &ss = m_sessions[i];
				if (!ss.connected())
					continue;

				//0��ʱ�ȴ�,���ٽ��н���̽��
				uint32_t rs = ss.readx(wr_buff, sizeof(wr_buff), 0);
				if (rs == 0)
				{
					if (!ss.connected())
					{//�����ж���
						++ac;
						continue;
					}
				}
				else
				{//�յ�������,ԭ�����͸��Է�
					++ac;
					if (ss.write(wr_buff, rs, wr_timeout_us) != ec_ok)
					{//���ʹ���,�����ж���
						continue;
					}
				}
			}
			return ac;
		}
	};

	//-----------------------------------------------------
	//��echo���в��ԵĿͻ���
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
	//��tcp echo����˽��в��Եļ򵥺���
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
	//��tcp echo����˽��в��Ե��߳�
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
	//��tcp echo�����������������Ե���������
	inline void ut_tcp_echo_thd(rx_tdd_t& rt, uint32_t loop = 3, uint32_t count = 3)
	{
		ut_tcpsvr_thd_t thd;
		rt.tdd_assert(thd.start(&rt));                      //�������̲߳�����
		rx_sleep(5);										//�������,��һ��,�÷����̱߳�ִ��
		for (uint32_t i = 0; i < count; ++i)
			ut_tcp_echo_clt(rt, loop);                       //���ÿͻ��˲���ѭ��
		thd.stop(true);                                     //�����߳�
	}
}

rx_tdd(test_net_tcp_base)
{
	for (uint32_t i = 0; i < 5; ++i)
		rx::ut_tcp_echo_thd(*this);
}

#endif // _UT_MEM_POOL_H_
