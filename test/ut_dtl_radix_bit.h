#ifndef _RX_UT_DTL_RADIX_BIT_H_
#define _RX_UT_DTL_RADIX_BIT_H_

#include "../rx_tdd.h"
#include "../rx_dtl_radix_bit.h"

namespace rx_ut
{
    //-----------------------------------------------------
    inline void ut_dtl_radix_bit_base_1(rx_tdd_t &rt)
    {
        rx::radix_bit_t rax;
        rx::rax_init(&rax);

        rx::leaf_t leaf1, leaf2, leaf3, leaf4, leaf5;
        rx::rax_insert(&rax, &leaf1, 1);
        rx::rax_insert(&rax, &leaf2, 2);
        rx::rax_insert(&rax, &leaf3, 0x123123);
        rx::rax_insert(&rax, &leaf4, 0x124);
        rx::rax_insert(&rax, &leaf5, 0x15);

        rx::leaf_t *r = rx::rax_find(&rax, 0x15);
        rt.tdd_assert(r->key==0x15);

        r = rx::rax_find(&rax, 0x2);
        rt.tdd_assert(r->key == 0x2);

        r = rx::rax_remove(&rax, 1);
        rt.tdd_assert(r->key == 1);
    }
}

rx_tdd(radix_bit_base)
{
    rx_ut::ut_dtl_radix_bit_base_1(*this);
}

#endif