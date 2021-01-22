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
#define RX_SPIN_ROUND_THRESHOLD 8							// 自旋轮数阀值,进行休眠策略的调整
#endif

//---------------------------------------------------------
//自旋锁高级休眠策略:rounds记录第几轮休眠;pauses记录低轮数模式中的空转次数(建议使用2的n次方);THRESHOLD告知高低轮数切换临界点.
inline void rx_spin_loop_pause(uint32_t &rounds, uint32_t &pauses, const uint32_t THRESHOLD = RX_SPIN_ROUND_THRESHOLD)
{
	if (rounds < THRESHOLD)
	{
		//先进行CPU级的短休眠尝试
		for (int32_t i = pauses; i > 0; --i)
			rx_mm_pause();									// 这是为支持超线程的 CPU 准备的切换提示
		pauses = pauses << 1;								// 短自旋次数指数翻倍
	}
	else
	{
		//进行OS调度器休眠与切换尝试,且根据轮数进行周期性调整
		uint32_t yield_cnt = rounds - THRESHOLD;
		if ((yield_cnt & 63) == 63)
			rx_thread_yield_us(1);							// 真正的休眠,并让出线程
		else if ((yield_cnt & 3) == 3)
			rx_thread_yield_us(0);
		else if (!rx_thread_yield())						// 尝试让步给该线程所在的CPU核心上的别的线程
			rx_thread_yield_us(0);
	}
	++rounds;
}

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
			mem_barrier();                                  // 编译器读写屏障

			// 先尝试原子锁定.
			if (rx_atomic_swap(m_lock, 1) != 0)				// 旧值为0时(空锁)则直接成功
			{
				// 原子锁定进行循环尝试,中间夹杂必要的调度休眠
				uint32_t rounds = 0;
				uint32_t pauses = 4;
				do
					rx_spin_loop_pause(rounds, pauses, RX_SPIN_ROUND_THRESHOLD);// 进行休眠
				while (!trylock());							// 再次尝试原子锁定
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

	//------------------------------------------------------
	// 基于原子变量自旋锁模式的多线程栅栏(起跑线前,运动员需要等发令枪,裁判员需要等运动员都准备好.)
	class spin_barrier_t
	{
		uint8_t     padding1[CPU_CACHELINE_SIZE / 2];
		int32_t     m_lock;									// 原子计数,归零的时候就是发令枪响了.
		uint8_t     padding2[CPU_CACHELINE_SIZE / 2 - sizeof(long)];
	public:
		spin_barrier_t() :m_lock(0) {}
		//--------------------------------------------------
		//等待,发令枪响或超时(默认死等).返回值:是否等待成功(否则就是超时)
		bool wait(uint32_t timeout_rounds = 0)
		{
			mem_barrier();                                  // 编译器读写屏障
			rx_atomic_add(m_lock, 1);						// 计数增加

			uint32_t rounds = 0;
			uint32_t pauses = 4;
			do {
				rx_spin_loop_pause(rounds, pauses, RX_SPIN_ROUND_THRESHOLD);// 进行休眠
				if (timeout_rounds && rounds > timeout_rounds)
					return false;							// 轮数超时
			} while (!rx_atomic_load(m_lock));				// 判断发令枪是否触发

			return true;
		}
		//--------------------------------------------------
		//放行,如果期待的expected人都到齐了
		bool pass(uint32_t expected, uint32_t timeout_rounds = 0)
		{
			mem_barrier();                                  // 编译器读写屏障

			uint32_t rounds = 0;
			uint32_t pauses = 4;
			do {
				rx_spin_loop_pause(rounds, pauses, RX_SPIN_ROUND_THRESHOLD);// 进行休眠
				if (timeout_rounds && rounds > timeout_rounds)
					return false;							// 轮数超时
			} while (rx_atomic_load(m_lock) != expected);	// 等待人员都到齐

			rx_atomic_store(m_lock, 0);                     // 发令枪响了
			return true;
		}
	};
	rx_static_assert(sizeof(spin_barrier_t) == CPU_CACHELINE_SIZE);
}
#if RX_CC==RX_CC_CLANG
#pragma GCC diagnostic pop
#endif

#endif
