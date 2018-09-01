#ifndef _UT_RX_DTL_HASHTBL_TINY_H_
#define _UT_RX_DTL_HASHTBL_TINY_H_

#include "../rx_cc_macro.h"
#include "../rx_tdd.h"
#include "../rx_raw_hashtbl_tiny.h"
#include "../rx_dtl_hashtbl_tiny.h"

namespace rx
{
    inline void tmp_hashtbl_tiny_base_1(rx_tdd_t &rt)
    {
        const uint32_t ms = 7;
        uint32_set_t<ms> s;
        rt.tdd_assert(s.capacity() == ms);
        rt.tdd_assert(s.size() == 0);
        rt.tdd_assert(s.collision() == 0);

        rt.tdd_assert(s.insert(1));
        rt.tdd_assert(s.size() == 1);
        rt.tdd_assert(s.collision() == 0);

        rt.tdd_assert(s.insert(1));
        rt.tdd_assert(s.size() == 1);
        rt.tdd_assert(s.collision() == 0);

        rt.tdd_assert(s.insert(2));
        rt.tdd_assert(s.size() == 2);
        rt.tdd_assert(s.collision() == 0);

        rt.tdd_assert(s.insert(3));
        rt.tdd_assert(s.size() == 3);
        rt.tdd_assert(s.collision() == 0);

        rt.tdd_assert(s.insert(4));
        rt.tdd_assert(s.size() == 4);
        rt.tdd_assert(s.collision() == 0);

    }
}

rx_tdd(hashtbl_tiny_base)
{
    rx::tmp_hashtbl_tiny_base_1(*this);
}


#endif
