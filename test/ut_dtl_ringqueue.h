#ifndef _RX_UT_DTL_RINGQUEUE_H_
#define _RX_UT_DTL_RINGQUEUE_H_


#include "../rx_tdd.h"
#include "../rx_dtl_ringqueue.h"
#include <time.h>
#include <stdlib.h>

namespace rx
{
    //---------------------------------------------------------
    inline void test_dtl_ringqueue_base_1(rx_tdd_base &rt)
    {
        typedef ringqueue_fixed<uint32_t, 7, spin_lock_t,uint8_t> ringqueue_t;
        ringqueue_t rq;

        rt.tdd_assert(rq.capacity() == 128);
        rt.tdd_assert(rq.pop() == NULL);
        rt.tdd_assert(rq.push(1));
        rt.tdd_assert(rq.size() == 1);
        rt.tdd_assert(rq.pop() != NULL);
        rt.tdd_assert(rq.size() == 0);
        rt.tdd_assert(rq.pop() == NULL);

        for (int i = 1; i <= 127; ++i)
        {
            rt.tdd_assert(rq.push(i));
            rt.tdd_assert(rq.size() == i);
        }

        rt.tdd_assert(rq.push(255));
        rt.tdd_assert(rq.size() == 128);

        rt.tdd_assert(rq.pop() != NULL);
        rt.tdd_assert(rq.size() == 127);

        rt.tdd_assert(rq.push(255));
        rt.tdd_assert(rq.size() == 128);

        rt.tdd_assert(!rq.push(255));
        rt.tdd_assert(rq.size() == 128);

        for (int i = 1; i <= 128; ++i)
        {
            rt.tdd_assert(rq.pop()!=NULL);
            rt.tdd_assert(rq.size() == 128-i);
        }

        rt.tdd_assert(rq.size() == 0);
        rt.tdd_assert(rq.pop() == NULL);

        for (int i = 1; i <= 125; ++i)
        {
            rt.tdd_assert(rq.push(i));
            rt.tdd_assert(rq.size() == i);
        }

        rt.tdd_assert(rq.push(126));
        rt.tdd_assert(rq.size() == 126);

        rt.tdd_assert(rq.push(127));
        rt.tdd_assert(rq.size() == 127);

        rt.tdd_assert(rq.push(255));
        rt.tdd_assert(rq.size() == 128);

        rt.tdd_assert(!rq.push(255));
        rt.tdd_assert(rq.size() == 128);

        rt.tdd_assert(rq.pop() != NULL);
        rt.tdd_assert(rq.size() == 127);

        rt.tdd_assert(rq.push(255));
        rt.tdd_assert(rq.size() == 128);

        rt.tdd_assert(!rq.push(255));
        rt.tdd_assert(rq.size() == 128);

        for (int i = 1; i <= 128; ++i)
        {
            rt.tdd_assert(rq.pop() != NULL);
            rt.tdd_assert(rq.size() == 128 - i);
        }

        rt.tdd_assert(rq.size() == 0);
        rt.tdd_assert(rq.pop() == NULL);

        for (int i = 1; i <= 128; ++i)
        {
            rt.tdd_assert(rq.push(i));
            rt.tdd_assert(rq.size() == i);
        }

        rt.tdd_assert(!rq.push(255));
        rt.tdd_assert(rq.size() == 128);

        for (int i = 1; i <= 128; ++i)
        {
            rt.tdd_assert(rq.pop() != NULL);
            rt.tdd_assert(rq.size() == 128 - i);
        }

    }
    //---------------------------------------------------------
    inline void test_dtl_ringqueue_base_2(rx_tdd_base &rt)
    {
        typedef ringqueue_fixed<uint32_t, 8, spin_lock_t> ringqueue_t;
        ringqueue_t rq;

        rt.tdd_assert(rq.capacity() == 256);
        rt.tdd_assert(rq.pop() == NULL);
        rt.tdd_assert(rq.push(1));
        rt.tdd_assert(rq.size()==1);
        rt.tdd_assert(rq.pop()!=NULL);
        rt.tdd_assert(rq.size() == 0);
        rt.tdd_assert(rq.pop() == NULL);

        for (int i = 1; i <= 255; ++i)
        {
            rt.tdd_assert(rq.push(i));
            rt.tdd_assert(rq.size() == i);
        }
        rt.tdd_assert(rq.push(255));
        rt.tdd_assert(rq.size() == 256);

        rt.tdd_assert(rq.pop() != NULL);
        rt.tdd_assert(rq.size() == 255);

        rt.tdd_assert(rq.push(255));
        rt.tdd_assert(rq.size() == 256);

        rt.tdd_assert(!rq.push(255));
        rt.tdd_assert(rq.size() == 256);
    }

    //---------------------------------------------------------
    template<class ST,uint32_t CP>
    inline void test_dtl_ringqueue_base_3(rx_tdd_base &rt)
    {
        typedef ringqueue_fixed<uint32_t, CP, null_lock_t, ST> ringqueue_t;
        ringqueue_t rq;
        srand((uint32_t)time(NULL));
        uint64_t tc = 0;
        const uint32_t tt = 100000;

        for (uint32_t lc = 0; lc < tt; ++lc)
        {
            uint32_t is_push = rand()&1;
            uint32_t op_count =3+ rand() % 30;
            tc += op_count;
            if (is_push)
            {
                for (uint32_t op = 0; op < op_count; ++op)
                {
                    if (rq.size() == rq.capacity())
                        rt.tdd_assert(!rq.push(op));
                    else
                        rt.tdd_assert(rq.push(op));
                }
            }
            else
            {
                for (uint32_t op = 0; op < op_count; ++op)
                {
                    if (rq.size() == 0)
                        rt.tdd_assert(rq.pop()==NULL);
                    else
                        rt.tdd_assert(rq.pop()!=NULL);
                }
            }

            if (lc % 1000==0)
                printf("[%8u/%8u] test op=%8llu\n",lc,tt,tc);
        }
        printf("CP=%u test=%8u  op=%8llu\n",CP, tt,tc);
    }
}
//---------------------------------------------------------
rx_tdd(dtl_ringqueue_base)
{
    rx::test_dtl_ringqueue_base_1(*this);
    rx::test_dtl_ringqueue_base_2(*this);

    rx::test_dtl_ringqueue_base_3<uint8_t, 7>(*this);
    rx::test_dtl_ringqueue_base_3<uint8_t, 6>(*this);
    rx::test_dtl_ringqueue_base_3<uint8_t, 5>(*this);
    rx::test_dtl_ringqueue_base_3<uint8_t, 4>(*this);

    rx::test_dtl_ringqueue_base_3<uint16_t, 15>(*this);
    rx::test_dtl_ringqueue_base_3<uint16_t, 14>(*this);
    rx::test_dtl_ringqueue_base_3<uint16_t, 13>(*this);
    rx::test_dtl_ringqueue_base_3<uint16_t, 12>(*this);

}








#endif
