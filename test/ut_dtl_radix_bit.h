#ifndef _RX_UT_DTL_RADIX_BIT_H_
#define _RX_UT_DTL_RADIX_BIT_H_

#include "../rx_tdd.h"
#include "../rx_dtl_raxbit_raw.h"
#include "../rx_dtl_raxbit_set.h"

namespace rx_ut
{
    //-----------------------------------------------------
    inline void ut_dtl_radix_bit_base_2(rx_tdd_t &rt)
    {
        typedef rx::raw_raxbit_t<uint32_t, rx::raxbit_op_int<uint32_t> > rax_t;
        rax_t rax;
        rax_t::leaf_t *leaf1, *leaf2, *leaf3, *leaf4, *leaf5;
        leaf1 = rax.insert(1); leaf1->key = 1;
        rt.tdd_assert(rax.limbs() == 0);

        leaf2 = rax.insert(2); leaf2->key = 2;
        rt.tdd_assert(rax.limbs() == 9 || rax.limbs() == 7);//64bit is 9;32bit is 7

        leaf3 = rax.insert(0x123123); leaf3->key = 0x123123;
        rt.tdd_assert(rax.limbs() == 9 || rax.limbs() == 7);

        leaf4 = rax.insert(0x124); leaf4->key = 0x124;
        rt.tdd_assert(rax.limbs() == 9 || rax.limbs() == 7);

        leaf5 = rax.insert(0x15); leaf5->key = 0x15;
        rt.tdd_assert(rax.limbs() == 9 || rax.limbs() == 7);

        rax_t::leaf_t *r = rax.find(0x15);
        rt.tdd_assert(r->key == 0x15);

        r = rax.find(0x2);
        rt.tdd_assert(r->key == 0x2);

        rt.tdd_assert(rax.remove(1));

        r = rax.find(0x2);
        rt.tdd_assert(r->key == 0x2);

        rt.tdd_assert(rax.remove(2));
        rt.tdd_assert(rax.limbs() == 7 || rax.limbs() == 5);

        rax.clear();
        rt.tdd_assert(rax.limbs()==0);
        rt.tdd_assert(rax.size()==0);
    }
    //-----------------------------------------------------
    inline void ut_dtl_radix_bit_base_2_1(rx_tdd_t &rt)
    {
        typedef rx::raw_raxbit_t<uint32_t, rx::raxbit_op_int<uint32_t> > rax_t;
        rax_t rax;
        rax_t::leaf_t *leaf1, *leaf2, *leaf3, *leaf4, *leaf5;
        leaf1 = rax.insert(1); leaf1->key = 1;
        rt.tdd_assert(rax.limbs() == 0);

        leaf2 = rax.insert(2); leaf2->key = 2;
        rt.tdd_assert(rax.limbs() == 9 || rax.limbs() == 7);//64bit is 9;32bit is 7

        leaf3 = rax.insert(0x123123); leaf3->key = 0x123123;
        rt.tdd_assert(rax.limbs() == 9 || rax.limbs() == 7);

        leaf4 = rax.insert(0x124); leaf4->key = 0x124;
        rt.tdd_assert(rax.limbs() == 9 || rax.limbs() == 7);

        leaf5 = rax.insert(0x15); leaf5->key = 0x15;
        rt.tdd_assert(rax.limbs() == 9 || rax.limbs() == 7);

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

        rt.tdd_assert(rax.find(1,path)->key == 1);
        rt.tdd_assert(rax.next(path)->key == 2);
        rt.tdd_assert(rax.next(path)->key == 0x15);
        rt.tdd_assert(rax.next(path)->key == 0x124);
        rt.tdd_assert(rax.next(path)->key == 0x123123);
        rt.tdd_assert(rax.next(path) == NULL);

        rt.tdd_assert(rax.find(2, path)->key == 2);
        rt.tdd_assert(rax.prev(path)->key == 1);
        rt.tdd_assert(rax.next(path)->key == 2);
        rt.tdd_assert(rax.next(path)->key == 0x15);
        rt.tdd_assert(rax.next(path)->key == 0x124);
        rt.tdd_assert(rax.next(path)->key == 0x123123);
        rt.tdd_assert(rax.next(path) == NULL);

        rt.tdd_assert(rax.find(0x15, path)->key == 0x15);
        rt.tdd_assert(rax.next(path)->key == 0x124);
        rt.tdd_assert(rax.next(path)->key == 0x123123);
        rt.tdd_assert(rax.next(path) == NULL);

        rt.tdd_assert(rax.find(0x15, path)->key == 0x15);
        rt.tdd_assert(rax.prev(path)->key == 2);
        rt.tdd_assert(rax.prev(path)->key == 1);

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
    //-----------------------------------------------------
    inline void ut_dtl_radix_bit_base_4(rx_tdd_t &rt)
    {
        typedef rx::raxbit_set_t<> cntr_t;
        cntr_t cntr;
        rt.tdd_assert(cntr.begin()==NULL);
        rt.tdd_assert(cntr.rbegin()==NULL);

        rt.tdd_assert(cntr.insert(2));
        rt.tdd_assert(cntr.insert(3));
        rt.tdd_assert(cntr.insert(9));
        rt.tdd_assert(cntr.insert(200000));
        rt.tdd_assert(cntr.insert(100000));
        rt.tdd_assert(cntr.insert(20000));

        rt.tdd_assert(cntr.find(2));
        rt.tdd_assert(cntr.find(3));
        rt.tdd_assert(cntr.find(9));
        rt.tdd_assert(cntr.find(200000));
        rt.tdd_assert(cntr.find(100000));
        rt.tdd_assert(cntr.find(20000));

        rt.tdd_assert(cntr.size()==6);
        rt.tdd_assert(cntr.begin()->key==2);
        rt.tdd_assert(cntr.rbegin()->key==200000);

        cntr_t::looper_t looper(cntr);
        cntr_t::iterator I=cntr.begin(looper);
        rt.tdd_assert(*I==2);++I;
        rt.tdd_assert(*I==3);++I;
        rt.tdd_assert(*I==9);++I;
        rt.tdd_assert(*I==20000);++I;
        rt.tdd_assert(*I==100000);++I;
        rt.tdd_assert(*I==200000);++I;
        rt.tdd_assert(I==cntr.end());

        rt.tdd_assert(cntr.erase(3));
        rt.tdd_assert(!cntr.erase(3));

        I=cntr.find(3,looper);
        rt.tdd_assert(I==cntr.end());

        I=cntr.find(9,looper);
        rt.tdd_assert(I!=cntr.end());
        cntr.erase(I);
        rt.tdd_assert(*I==20000);

        cntr.clear();
    }
}

rx_tdd(radix_bit_base)
{
    rx_ut::ut_dtl_radix_bit_base_4(*this);
    rx_ut::ut_dtl_radix_bit_base_2_1(*this);
    rx_ut::ut_dtl_radix_bit_base_3(*this);
    rx_ut::ut_dtl_radix_bit_base_2(*this);
}

#endif
