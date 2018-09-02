#ifndef _RS_OS_SPINLOCK_H_
#define _RS_OS_SPINLOCK_H_

#include "rx_cc_macro.h"
#include "rx_cc_atomic.h"
#include "rx_os_lock.h"

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
        spin_lock_t():m_lock(0) {}
        //--------------------------------------------------
        //����
        virtual bool lock(bool is_wr_lock = true)
        {
            static const uint32_t YIELD_THRESHOLD = 8;      // ����������ֵ

            mem_barrier();                                  // ��������д����

            //�ȳ��Ի�ȡһ��������.
            if (rx_atomic_swap(&m_lock, 1) != 0)
            {
                //�״γ���û�гɹ������ѭ������
                uint32_t loop_count = 0;
                uint32_t spin_count = 1;
                do
                {
                    if (loop_count < YIELD_THRESHOLD)
                    {
                        //�Ƚ���CPU���Ķ����߳���
                        for (int32_t pause_cnt = spin_count; pause_cnt > 0; --pause_cnt)
                            rx_mm_pause();                  // ����Ϊ֧�ֳ��̵߳� CPU ׼�����л���ʾ
                        spin_count = spin_count<<1;         // ����������ָ������
                    }
                    else
                    {
                        //����һ�������Եĵ����������л�
                        uint32_t yield_cnt = loop_count - YIELD_THRESHOLD;
                        if ((yield_cnt & 63) == 63)
                            rx_thread_yield_us(1);          // ����������, ת���ں�̬
                        else if ((yield_cnt & 3) == 3)
                            rx_thread_yield_us(0);          // �л������ȼ����Լ�һ������ߵ��߳�, ���Ի������CPU������
                        else if (!rx_thread_yield())        // �ò������߳����ڵ�CPU�����ϵı���߳�,
                            rx_thread_yield_us(0);      // ���ͬ������û�п��л����߳�,
                    }
                    ++loop_count;
                }
                while (!rx_atomic_cas(&m_lock, 0, 1));
            }
            return true;
        }
        //--------------------------------------------------
        //����
        virtual bool unlock()
        {
            mem_barrier();                                  // ��������д����
            rx_atomic_store(&m_lock,0);                     // �ͷ���
            return true;
        }
        //--------------------------------------------------
        //��������
        virtual bool trylock(bool is_wr_lock = true)
        {
            mem_barrier();
            return rx_atomic_swap(&m_lock, 1) == 0;
        }
    };
}

#endif
