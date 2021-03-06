#ifndef _RX_UT_DTL_RINGQUEUE_H_
#define _RX_UT_DTL_RINGQUEUE_H_

#include "../rx_cc_macro.h"
#include "../rx_tdd.h"
#include "../rx_dtl_ringqueue.h"
#include "../rx_lock_spin.h"
#include <time.h>
#include <stdlib.h>

namespace rx
{
    //---------------------------------------------------------
    inline void test_dtl_ringqueue_base_1(rx_tdd_t &rt)
    {
        typedef ringqueue_ft<uint32_t, 7, spin_lock_t,uint8_t> ringqueue_t;
        ringqueue_t rq;

        rt.tdd_assert(rq.capacity() == 128);
        rt.tdd_assert(rq.pop_front() == NULL);
        rt.tdd_assert(rq.push_back(1));
        rt.tdd_assert(rq.size() == 1);
        rt.tdd_assert(rq.pop_front() != NULL);
        rt.tdd_assert(rq.size() == 0);
        rt.tdd_assert(rq.pop_front() == NULL);

        for (int i = 1; i <= 127; ++i)
        {
            rt.tdd_assert(rq.push_back(i));
            rt.tdd_assert(rq.size() == i);
        }

        rt.tdd_assert(rq.push_back(255));
        rt.tdd_assert(rq.size() == 128);

		rt.tdd_assert(!rq.push_back(0));
		rt.tdd_assert(rq.size() == 128);

        rt.tdd_assert(rq.pop_front() != NULL);
        rt.tdd_assert(rq.size() == 127);

        rt.tdd_assert(rq.push_back(255));
        rt.tdd_assert(rq.size() == 128);

        rt.tdd_assert(!rq.push_back(255));
        rt.tdd_assert(rq.size() == 128);

        for (int i = 1; i <= 128; ++i)
        {
            rt.tdd_assert(rq.pop_front()!=NULL);
            rt.tdd_assert(rq.size() == 128-i);
        }

        rt.tdd_assert(rq.size() == 0);
        rt.tdd_assert(rq.pop_front() == NULL);

        for (int i = 1; i <= 125; ++i)
        {
            rt.tdd_assert(rq.push_back(i));
            rt.tdd_assert(rq.size() == i);
        }

        rt.tdd_assert(rq.push_back(126));
        rt.tdd_assert(rq.size() == 126);

        rt.tdd_assert(rq.push_back(127));
        rt.tdd_assert(rq.size() == 127);

        rt.tdd_assert(rq.push_back(255));
        rt.tdd_assert(rq.size() == 128);

        rt.tdd_assert(!rq.push_back(255));
        rt.tdd_assert(rq.size() == 128);

        rt.tdd_assert(rq.pop_front() != NULL);
        rt.tdd_assert(rq.size() == 127);

        rt.tdd_assert(rq.push_back(255));
        rt.tdd_assert(rq.size() == 128);

        rt.tdd_assert(!rq.push_back(255));
        rt.tdd_assert(rq.size() == 128);

        for (int i = 1; i <= 128; ++i)
        {
            rt.tdd_assert(rq.pop_front() != NULL);
            rt.tdd_assert(rq.size() == 128 - i);
        }

        rt.tdd_assert(rq.size() == 0);
        rt.tdd_assert(rq.pop_front() == NULL);

        for (int i = 1; i <= 128; ++i)
        {
            rt.tdd_assert(rq.push_back(i));
            rt.tdd_assert(rq.size() == i);
        }

        rt.tdd_assert(!rq.push_back(255));
        rt.tdd_assert(rq.size() == 128);

        for (int i = 1; i <= 128; ++i)
        {
            rt.tdd_assert(rq.pop_front() != NULL);
            rt.tdd_assert(rq.size() == 128 - i);
        }
    }

    //---------------------------------------------------------
    inline void test_dtl_ringqueue_base_2(rx_tdd_t &rt)
    {
        typedef ringqueue_ft<uint32_t, 8, spin_lock_t> ringqueue_t;
        ringqueue_t rq;

        rt.tdd_assert(rq.capacity() == 256);
        rt.tdd_assert(rq.pop_front() == NULL);
        rt.tdd_assert(rq.push_back(1));
        rt.tdd_assert(rq.size()==1);
        rt.tdd_assert(rq.pop_front()!=NULL);
        rt.tdd_assert(rq.size() == 0);
        rt.tdd_assert(rq.pop_front() == NULL);

        for (uint32_t i = 1; i <= 255; ++i)
        {
            rt.tdd_assert(rq.push_back(i));
            rt.tdd_assert(rq.size() == i);
        }
        rt.tdd_assert(rq.push_back(255));
        rt.tdd_assert(rq.size() == 256);

        rt.tdd_assert(rq.pop_front() != NULL);
        rt.tdd_assert(rq.size() == 255);

        rt.tdd_assert(rq.push_back(255));
        rt.tdd_assert(rq.size() == 256);

        rt.tdd_assert(!rq.push_back(255));
        rt.tdd_assert(rq.size() == 256);
    }

    //---------------------------------------------------------
    template<class ST,uint32_t CP,class LT>
    inline void test_dtl_ringqueue_base_3(rx_tdd_t &rt,const uint32_t tt=1000)
    {
        typedef ringqueue_ft<uint32_t, CP, LT, ST> ringqueue_t;
        ringqueue_t rq;
        uint32_t tc = 0;

        chkbad_t bad;

        for (uint32_t lc = 0; lc < tt; ++lc)
        {
            uint32_t is_push_back = rand()&1;
            uint32_t op_count =3+ rand() % 80;
            tc += op_count;
            if (is_push_back)
            {
                for (uint32_t op = 0; op < op_count; ++op)
                {
                    if (rq.size() == rq.capacity())
                        bad.assert(!rq.push_back(op));
                    else
                        bad.assert(rq.push_back(op));
                }
            }
            else
            {
                for (uint32_t op = 0; op < op_count; ++op)
                {
                    if (rq.size() == 0)
                        bad.assert(rq.pop_front()==NULL);
                    else
                        bad.assert(rq.pop_front()!=NULL);
                }
            }

            //if (lc % 1000==0)
            //    printf("[%8u/%8u] test op=%8u\n",lc,tt,tc);
        }
        rt.tdd_assert(bad==0);
        if (tt>100)
            printf("CP=%4u test=%8u  op=%8u\n",CP, tt,tc);
    }

    //---------------------------------------------------------
    template<class LT>
    inline void test_dtl_ringqueue_base_3L(rx_tdd_t &rt,const uint32_t tt=1000)
    {
        test_dtl_ringqueue_base_3<uint8_t, 7, LT>(rt,tt);
        test_dtl_ringqueue_base_3<uint8_t, 6, LT>(rt,tt);
        test_dtl_ringqueue_base_3<uint8_t, 5, LT>(rt,tt);
        test_dtl_ringqueue_base_3<uint8_t, 4, LT>(rt,tt);

        test_dtl_ringqueue_base_3<uint16_t, 15, LT>(rt,tt);
        test_dtl_ringqueue_base_3<uint16_t, 14, LT>(rt,tt);
        test_dtl_ringqueue_base_3<uint16_t, 13, LT>(rt,tt);
        test_dtl_ringqueue_base_3<uint16_t, 12, LT>(rt,tt);

    }
}
//---------------------------------------------------------
rx_tdd(dtl_ringqueue_base)
{
    rx::test_dtl_ringqueue_base_1(*this);
    rx::test_dtl_ringqueue_base_2(*this);
    srand((uint32_t)time(NULL));
    rx::test_dtl_ringqueue_base_3L<rx::null_lock_t>(*this,10);
    rx::test_dtl_ringqueue_base_3L<rx::spin_lock_t>(*this,10);
}

rx_tdd_rtl(dtl_ringqueue_base_2,tdd_level_slow)
{
    srand((uint32_t)time(NULL));
    rx::test_dtl_ringqueue_base_3L<rx::null_lock_t>(*this,1000000);
    rx::test_dtl_ringqueue_base_3L<rx::spin_lock_t>(*this,1000000);

}






#endif
