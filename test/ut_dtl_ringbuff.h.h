#ifndef _UT_DTL_RINGBUFF_H_
#define _UT_DTL_RINGBUFF_H_

#include "../rx_cc_macro.h"
#include "../rx_tdd.h"
#include "../rx_dtl_ringbuff.h"

inline void test_dtl_ringbuff_base_1(rx_tdd_base &rt)
{
}

rx_tdd(rx_dtl_ringbuff)
{
    test_dtl_ringbuff_base_1(*this);
}


#endif
