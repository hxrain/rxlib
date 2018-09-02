#ifndef _RS_OS_SPINLOCK_H_
#define _RS_OS_SPINLOCK_H_

#include "rx_cc_macro.h"
#include "rx_cc_atomic.h"
#include "rx_os_lock.h"

namespace rx
{
    enum { CPU_CACHELINE_SIZE = 64 };                       // 进行CACH-LINE高速缓存的优化填充
    //------------------------------------------------------
    //参考:  https://github.com/shines77/RingQueue/blob/master/include/RingQueue/RingQueue.h
    //基于原子变量实现的自旋锁,不可重入
    class spin_lock_t :public lock_i
    {
        uint8_t     padding1[CPU_CACHELINE_SIZE / 2];
        int32_t     m_lock;
        uint8_t     padding2[CPU_CACHELINE_SIZE / 2 - sizeof(long)];
    public:
        spin_lock_t():m_lock(0) {}
        //--------------------------------------------------
        //锁定
        virtual bool lock(bool is_wr_lock = true)
        {
            static const uint32_t YIELD_THRESHOLD = 8;      // 自旋次数阀值

            mem_barrier();                                  // 编译器读写屏障

            //先尝试获取一下自旋锁.
            if (rx_atomic_swap(&m_lock, 1) != 0)
            {
                //首次尝试没有成功则进行循环尝试
                uint32_t loop_count = 0;
                uint32_t spin_count = 1;
                do
                {
                    if (loop_count < YIELD_THRESHOLD)
                    {
                        //先进行CPU级的短休眠尝试
                        for (int32_t pause_cnt = spin_count; pause_cnt > 0; --pause_cnt)
                            rx_mm_pause();                  // 这是为支持超线程的 CPU 准备的切换提示
                        spin_count = spin_count<<1;         // 短自旋次数指数翻倍
                    }
                    else
                    {
                        //进行一定周期性的调度休眠与切换
                        uint32_t yield_cnt = loop_count - YIELD_THRESHOLD;
                        if ((yield_cnt & 63) == 63)
                            rx_thread_yield_us(1);          // 真正的休眠, 转入内核态
                        else if ((yield_cnt & 3) == 3)
                            rx_thread_yield_us(0);          // 切换到优先级跟自己一样或更高的线程, 可以换到别的CPU核心上
                        else if (!rx_thread_yield())        // 让步给该线程所在的CPU核心上的别的线程,
                            rx_thread_yield_us(0);      // 如果同核心上没有可切换的线程,
                    }
                    ++loop_count;
                }
                while (!rx_atomic_cas(&m_lock, 0, 1));
            }
            return true;
        }
        //--------------------------------------------------
        //解锁
        virtual bool unlock()
        {
            mem_barrier();                                  // 编译器读写屏障
            rx_atomic_store(&m_lock,0);                     // 释放锁
            return true;
        }
        //--------------------------------------------------
        //尝试锁定
        virtual bool trylock(bool is_wr_lock = true)
        {
            mem_barrier();
            return rx_atomic_swap(&m_lock, 1) == 0;
        }
    };
}

#endif
