#ifndef _RS_OS_SPINLOCK_H_
#define _RS_OS_SPINLOCK_H_

#include "rx_cc_macro.h"
#include "rx_cc_atomic.h"
#include "rx_lock_os.h"

#if RX_CC==RX_CC_CLANG
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-private-field"
#endif

#if !defined(RX_SPIN_ROUND_THRESHOLD)
#define RX_SPIN_ROUND_THRESHOLD 8							// ����������ֵ,�������߲��Եĵ���
#endif

//---------------------------------------------------------
//�������߼����߲���:rounds��¼�ڼ�������;pauses��¼������ģʽ�еĿ�ת����(����ʹ��2��n�η�);THRESHOLD��֪�ߵ������л��ٽ��.
inline void rx_spin_loop_pause(uint32_t &rounds, uint32_t &pauses, const uint32_t THRESHOLD = RX_SPIN_ROUND_THRESHOLD)
{
	if (rounds < THRESHOLD)
	{
		//�Ƚ���CPU���Ķ����߳���
		for (int32_t i = pauses; i > 0; --i)
			rx_mm_pause();									// ����Ϊ֧�ֳ��̵߳� CPU ׼�����л���ʾ
		pauses = pauses << 1;								// ����������ָ������
	}
	else
	{
		//����OS�������������л�����,�Ҹ����������������Ե���
		uint32_t yield_cnt = rounds - THRESHOLD;
		if ((yield_cnt & 63) == 63)
			rx_thread_yield_us(1);							// ����������,���ó��߳�
		else if ((yield_cnt & 3) == 3)
			rx_thread_yield_us(0);
		else if (!rx_thread_yield())						// �����ò������߳����ڵ�CPU�����ϵı���߳�
			rx_thread_yield_us(0);
	}
	++rounds;
}

namespace rx
{
	//------------------------------------------------------
	//�ο�:  https://github.com/shines77/RingQueue/blob/master/include/RingQueue/RingQueue.h
	//����ԭ�ӱ���ʵ�ֵ�������,���ɵݹ�
	class spin_lock_t :public lock_i
	{
		uint8_t     padding1[CPU_CACHELINE_SIZE / 2 - sizeof(void*)];
		int32_t     m_lock;
		uint8_t     padding2[CPU_CACHELINE_SIZE / 2 - sizeof(long)];
	public:
		spin_lock_t() :m_lock(0) {}
		//--------------------------------------------------
		//����
		virtual bool lock(bool is_wr_lock = true)
		{
			mem_barrier();                                  // ��������д����

			// �ȳ���ԭ������.
			if (rx_atomic_swap(m_lock, 1) != 0)				// ��ֵΪ0ʱ(����)��ֱ�ӳɹ�
			{
				// ԭ����������ѭ������,�м���ӱ�Ҫ�ĵ�������
				uint32_t rounds = 0;
				uint32_t pauses = 4;
				do
					rx_spin_loop_pause(rounds, pauses, RX_SPIN_ROUND_THRESHOLD);// ��������
				while (!trylock());							// �ٴγ���ԭ������
			}
			return true;
		}
		//--------------------------------------------------
		//����
		virtual bool unlock()
		{
			mem_barrier();                                  // ��������д����
			rx_atomic_store(m_lock, 0);                     // �ͷ���
			return true;
		}
		//--------------------------------------------------
		//��������
		virtual bool trylock(bool is_wr_lock = true)
		{
			mem_barrier();
			return rx_atomic_cas(m_lock, 0, 1);
		}
	};
	rx_static_assert(sizeof(spin_lock_t) == CPU_CACHELINE_SIZE);

	//------------------------------------------------------
	// ����ԭ�ӱ���������ģʽ�Ķ��߳�դ��(������ǰ,�˶�Ա��Ҫ�ȷ���ǹ,����Ա��Ҫ���˶�Ա��׼����.)
	class spin_barrier_t
	{
		uint8_t     padding1[CPU_CACHELINE_SIZE / 2];
		int32_t     m_lock;									// ԭ�Ӽ���,�����ʱ����Ƿ���ǹ����.
		uint8_t     padding2[CPU_CACHELINE_SIZE / 2 - sizeof(long)];
	public:
		spin_barrier_t() :m_lock(0) {}
		//--------------------------------------------------
		//�ȴ�,����ǹ���ʱ(Ĭ������).����ֵ:�Ƿ�ȴ��ɹ�(������ǳ�ʱ)
		bool wait(uint32_t timeout_rounds = 0)
		{
			mem_barrier();                                  // ��������д����
			rx_atomic_add(m_lock, 1);						// ��������

			uint32_t rounds = 0;
			uint32_t pauses = 4;
			do {
				rx_spin_loop_pause(rounds, pauses, RX_SPIN_ROUND_THRESHOLD);// ��������
				if (timeout_rounds && rounds > timeout_rounds)
					return false;							// ������ʱ
			} while (!rx_atomic_load(m_lock));				// �жϷ���ǹ�Ƿ񴥷�

			return true;
		}
		//--------------------------------------------------
		//����,����ڴ���expected�˶�������
		bool pass(uint32_t expected, uint32_t timeout_rounds = 0)
		{
			mem_barrier();                                  // ��������д����

			uint32_t rounds = 0;
			uint32_t pauses = 4;
			do {
				rx_spin_loop_pause(rounds, pauses, RX_SPIN_ROUND_THRESHOLD);// ��������
				if (timeout_rounds && rounds > timeout_rounds)
					return false;							// ������ʱ
			} while (rx_atomic_load(m_lock) != expected);	// �ȴ���Ա������

			rx_atomic_store(m_lock, 0);                     // ����ǹ����
			return true;
		}
	};
	rx_static_assert(sizeof(spin_barrier_t) == CPU_CACHELINE_SIZE);
}
#if RX_CC==RX_CC_CLANG
#pragma GCC diagnostic pop
#endif

#endif
