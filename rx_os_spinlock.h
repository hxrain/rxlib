#ifndef _RS_OS_SPINLOCK_H_
#define _RS_OS_SPINLOCK_H_

#include "rx_cc_macro.h"
#include "rx_cc_atomic.h"
#include "rx_os_lock.h"

#if RX_CC==RX_CC_CLANG
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-private-field"
#endif

namespace rx
{
	enum { CPU_CACHELINE_SIZE = 64 };                       // ����CACH-LINE���ٻ�����Ż����
	//------------------------------------------------------
	//�ο�:  https://github.com/shines77/RingQueue/blob/master/include/RingQueue/RingQueue.h
	//����ԭ�ӱ���ʵ�ֵ�������,��������
	class spin_lock_t :public lock_i
	{
		uint8_t     padding1[CPU_CACHELINE_SIZE / 2];
		int32_t     m_lock;
		uint8_t     padding2[CPU_CACHELINE_SIZE / 2 - sizeof(long)];
	public:
		spin_lock_t() :m_lock(0) {}
		//--------------------------------------------------
		//����
		virtual bool lock(bool is_wr_lock = true)
		{
			static const uint32_t YIELD_THRESHOLD = 8;      // ����������ֵ

			mem_barrier();                                  // ��������д����

			//�ȳ���ԭ������.
			if (rx_atomic_swap(m_lock, 1) != 0)				//��ֵΪ0ʱ(����)��ֱ�ӳɹ�
			{
				//ԭ����������ѭ������,�м���ӱ�Ҫ�ĵ�������
				uint32_t rounds = 0;
				uint32_t pauses = 1;
				do
				{
					if (rounds < YIELD_THRESHOLD)
					{
						//�Ƚ���CPU���Ķ����߳���
						for (int32_t i = pauses; i > 0; --i)
							rx_mm_pause();                  // ����Ϊ֧�ֳ��̵߳� CPU ׼�����л���ʾ
						pauses = pauses << 1;					// ����������ָ������
					}
					else
					{
						//����һ�������Եĵ����������л�
						uint32_t yield_cnt = rounds - YIELD_THRESHOLD;
						if ((yield_cnt & 63) == 63)
							rx_thread_yield_us(1);          // ����������, ת���ں�̬
						else if ((yield_cnt & 3) == 3)
							rx_thread_yield_us(0);          // �л������ȼ����Լ�һ������ߵ��߳�, ���Ի������CPU������
						else if (!rx_thread_yield())        // �ò������߳����ڵ�CPU�����ϵı���߳�,
							rx_thread_yield_us(0);          // ���ͬ������û�п��л����߳�,
					}
					++rounds;
				} while (!trylock());						//�ٴγ���ԭ������
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
}
#if RX_CC==RX_CC_CLANG
#pragma GCC diagnostic pop
#endif

#endif
