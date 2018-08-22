#ifndef _UT_DTL_RINGBUFF_H_
#define _UT_DTL_RINGBUFF_H_

#include "../rx_cc_macro.h"
#include "../rx_tdd.h"
#include "../rx_dtl_ringbuff.h"

inline void test_dtl_ringbuff_base_1(rx_tdd_base &rt)
{
    uint8_t tmp[5];
    rx::ringbuff_fixed<17> rb;

    rt.tdd_assert(rb.empty());
    rt.tdd_assert(rb.capacity()==17);
    rt.tdd_assert(rb.remain()==17);
    rt.tdd_assert(rb.size()==0);

    rt.tdd_assert(rb.push(tmp,5));
    rt.tdd_assert(!rb.empty());
    rt.tdd_assert(rb.remain()==12);
    rt.tdd_assert(rb.size()==5);

    rt.tdd_assert(rb.push(tmp,5));
    rt.tdd_assert(!rb.empty());
    rt.tdd_assert(rb.remain()==7);
    rt.tdd_assert(rb.size()==10);

    rt.tdd_assert(rb.pop(0,true)!=NULL);
    rt.tdd_assert(!rb.empty());
    rt.tdd_assert(rb.remain()==7);
    rt.tdd_assert(rb.size()==10);

    rt.tdd_assert(rb.pop(3)!=NULL);
    rt.tdd_assert(!rb.empty());
    rt.tdd_assert(rb.remain()==7);
    rt.tdd_assert(rb.size()==7);

    rt.tdd_assert(rb.pop(6)!=NULL);
    rt.tdd_assert(!rb.empty());
    rt.tdd_assert(rb.remain()==9);
    rt.tdd_assert(rb.size()==1);

    rt.tdd_assert(rb.push(tmp,5));
    rt.tdd_assert(!rb.empty());
    rt.tdd_assert(rb.remain()==4);
    rt.tdd_assert(rb.size()==1);
    rt.tdd_assert(rb.size(true)==6);

    rt.tdd_assert(rb.pop(6)==NULL);
    rt.tdd_assert(rb.pop()!=NULL);
    rt.tdd_assert(!rb.empty());
    rt.tdd_assert(rb.remain()==12);
    rt.tdd_assert(rb.size()==5);

    rt.tdd_assert(rb.pop()!=NULL);
    rt.tdd_assert(rb.empty());
    rt.tdd_assert(rb.remain()==17);
    rt.tdd_assert(rb.size()==0);
}

rx_tdd(rx_dtl_ringbuff)
{
    test_dtl_ringbuff_base_1(*this);
}


#endif
