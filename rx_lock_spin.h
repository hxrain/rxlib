#ifndef _RS_OS_SPINLOCK_H_
#define _RS_OS_SPINLOCK_H_

#include "rx_cc_macro.h"
#include "rx_cc_atomic.h"
#include "rx_lock_os.h"

#if RX_CC==RX_CC_CLANG
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-private-field"
#endif

namespace rx
{
	//------------------------------------------------------
	//参考:  https://github.com/shines77/RingQueue/blob/master/include/RingQueue/RingQueue.h
	//基于原子变量实现的自旋锁,不可递归
	class spin_lock_t :public lock_i
	{
		uint8_t     padding1[CPU_CACHELINE_SIZE / 2 - sizeof(void*)];
		int32_t     m_lock;
		uint8_t     padding2[CPU_CACHELINE_SIZE / 2 - sizeof(long)];
	public:
		spin_lock_t() :m_lock(0) {}
		//--------------------------------------------------
		//锁定
		virtual bool lock(bool is_wr_lock = true)
		{
			static const uint32_t YIELD_THRESHOLD = 8;      // 自旋轮数阀值

			mem_barrier();                                  // 编译器读写屏障

			//先尝试原子锁定.
			if (rx_atomic_swap(m_lock, 1) != 0)				//旧值为0时(空锁)则直接成功
			{
				//原子锁定进行循环尝试,中间夹杂必要的调度休眠
				uint32_t rounds = 0;
				uint32_t pauses = 1;
				do
				{
					if (rounds < YIELD_THRESHOLD)
					{
						//先进行CPU级的短休眠尝试
						for (int32_t i = pauses; i > 0; --i)
							rx_mm_pause();                  // 这是为支持超线程的 CPU 准备的切换提示
						pauses = pauses << 1;				// 短自旋次数指数翻倍
					}
					else
					{
						//进行一定周期性的调度休眠与切换
						uint32_t yield_cnt = rounds - YIELD_THRESHOLD;
						if ((yield_cnt & 63) == 63)
							rx_thread_yield_us(1);          // 真正的休眠,并让出线程
						else if ((yield_cnt & 3) == 3)
							rx_thread_yield_us(0);          // 切换到其他线程(优先级跟自己一样或更高的线程, 可换到别的CPU核心)
						else if (!rx_thread_yield())        // 尝试让步给该线程所在的CPU核心上的别的线程
							rx_thread_yield_us(0);          // 切换到其他线程(优先级跟自己一样或更高的线程, 可换到别的CPU核心)
					}
					++rounds;
				} while (!trylock());						//再次尝试原子锁定
			}
			return true;
		}
		//--------------------------------------------------
		//解锁
		virtual bool unlock()
		{
			mem_barrier();                                  // 编译器读写屏障
			rx_atomic_store(m_lock, 0);                     // 释放锁
			return true;
		}
		//--------------------------------------------------
		//尝试锁定
		virtual bool trylock(bool is_wr_lock = true)
		{
			mem_barrier();
			return rx_atomic_cas(m_lock, 0, 1);
		}
	};
	rx_static_assert(sizeof(spin_lock_t) == CPU_CACHELINE_SIZE);
}
#if RX_CC==RX_CC_CLANG
#pragma GCC diagnostic pop
#endif

#endif
