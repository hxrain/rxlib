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
    //-----------------------------------------------------
    inline void ut_dtl_radix_bit_base_2(rx_tdd_t &rt)
    {
        typedef rx::raw_raxbit_t<uint32_t, rx::raxbit_op_int<uint32_t> > rax_t;
        rax_t rax;
        rax_t::leaf_t *leaf1, *leaf2, *leaf3, *leaf4, *leaf5;
        leaf1 = rax.insert(1); leaf1->key = 1;
        rt.tdd_assert(rax.limbs() == 0);

        leaf2 = rax.insert(2); leaf2->key = 2;
        rt.tdd_assert(rax.limbs() == 9);

        leaf3 = rax.insert(0x123123); leaf3->key = 0x123123;
        rt.tdd_assert(rax.limbs() == 9);

        leaf4 = rax.insert(0x124); leaf4->key = 0x124;
        rt.tdd_assert(rax.limbs() == 9);

        leaf5 = rax.insert(0x15); leaf5->key = 0x15;
        rt.tdd_assert(rax.limbs() == 9);

        rax_t::leaf_t *r = rax.find(0x15);
        rt.tdd_assert(r->key == 0x15);

        r = rax.find(0x2);
        rt.tdd_assert(r->key == 0x2);

        rt.tdd_assert(rax.remove(1));

        r = rax.find(0x2);
        rt.tdd_assert(r->key == 0x2);

        rt.tdd_assert(rax.remove(2));
        rt.tdd_assert(rax.limbs() == 7);
    }
    inline void ut_dtl_radix_bit_base_2_1(rx_tdd_t &rt)
    {
        typedef rx::raw_raxbit_t<uint32_t, rx::raxbit_op_int<uint32_t> > rax_t;
        rax_t rax;
        rax_t::leaf_t *leaf1, *leaf2, *leaf3, *leaf4, *leaf5;
        leaf1 = rax.insert(1); leaf1->key = 1;
        rt.tdd_assert(rax.limbs() == 0);

        leaf2 = rax.insert(2); leaf2->key = 2;
        rt.tdd_assert(rax.limbs() == 9);

        leaf3 = rax.insert(0x123123); leaf3->key = 0x123123;
        rt.tdd_assert(rax.limbs() == 9);

        leaf4 = rax.insert(0x124); leaf4->key = 0x124;
        rt.tdd_assert(rax.limbs() == 9);

        leaf5 = rax.insert(0x15); leaf5->key = 0x15;
        rt.tdd_assert(rax.limbs() == 9);

        rax_t::leaf_t *r = rax.find(0x15);
        rt.tdd_assert(r->key == 0x15);

        r = rax.find(0x2);
        rt.tdd_assert(r->key == 0x2);

        rax_t::back_path_t path;
        rt.tdd_assert(rax.left(path)->key == 1);
        rt.tdd_assert(rax.next(path)->key == 2);
        rt.tdd_assert(rax.next(path)->key == 0x15);
        rt.tdd_assert(rax.next(path)->key == 0x124);
        rt.tdd_assert(rax.next(path)->key == 0x123123);
        rt.tdd_assert(rax.next(path) == NULL);

        rt.tdd_assert(rax.right(path)->key== 0x123123);
        rt.tdd_assert(rax.prev(path)->key == 0x124);
        rt.tdd_assert(rax.prev(path)->key == 0x15);
        rt.tdd_assert(rax.prev(path)->key == 2);
        rt.tdd_assert(rax.prev(path)->key == 1);
        rt.tdd_assert(rax.prev(path) == NULL);

        rax.clear();
    }
    //-----------------------------------------------------
    inline void ut_dtl_radix_bit_base_3(rx_tdd_t &rt)
    {
        typedef rx::raw_raxbit_t<uint8_t, rx::raxbit_op_int<uint8_t> > rax_t;
        rax_t rax;
        rax_t::leaf_t *leaf1, *leaf2, *leaf3, *leaf4;
        leaf1 = rax.insert(1); leaf1->key = 1;
        rt.tdd_assert(rax.limbs() == 0);

        leaf2 = rax.insert(2); leaf2->key = 2;
        rt.tdd_assert(rax.limbs() == 1);

        leaf3 = rax.insert(0x12); leaf3->key = 0x12;
        rt.tdd_assert(rax.limbs() == 1);

        leaf4 = rax.insert(0x15); leaf4->key = 0x15;
        rt.tdd_assert(rax.limbs() == 2);
        rt.tdd_assert(rax.size() == 4);

        rax_t::leaf_t *r = rax.find(0x15);
        rt.tdd_assert(r->key == 0x15);

        r = rax.find(0x2);
        rt.tdd_assert(r->key == 0x2);

        rt.tdd_assert(rax.remove(1));
        rt.tdd_assert(rax.limbs() == 1);

        r = rax.find(0x2);
        rt.tdd_assert(r->key == 0x2);

        rt.tdd_assert(rax.remove(2));
        rt.tdd_assert(rax.limbs() == 1);

        rt.tdd_assert(rax.remove(0x15));
        rt.tdd_assert(rax.limbs() == 0);

        rt.tdd_assert(rax.remove(0x12));
        rt.tdd_assert(rax.limbs() == 0);

        rt.tdd_assert(!rax.remove(0x12));
        rt.tdd_assert(rax.limbs() == 0);
        rt.tdd_assert(rax.size() == 0);
    }
}

rx_tdd(radix_bit_base)
{
    rx_ut::ut_dtl_radix_bit_base_2_1(*this);
    rx_ut::ut_dtl_radix_bit_base_3(*this);
    rx_ut::ut_dtl_radix_bit_base_1(*this);
    rx_ut::ut_dtl_radix_bit_base_2(*this);
}

#endif