#ifndef _RX_UT_DTL_UTIL_H_
#define _RX_UT_DTL_UTIL_H_

#include "../rx_dtl_util.h"
#include "../rx_tdd.h"


inline void dtl_util_base_1(rx_tdd_base &rt)
{
    uint8_t tmp1[5],tmp2[6],tmp3[7];
    rt.tdd_assert(rx::make_tiny_string(tmp1,sizeof(tmp1),"123")==0);
    rt.tdd_assert(rx::make_tiny_string(tmp2, sizeof(tmp2), "123") == 1);
    rt.tdd_assert(rx::make_tiny_string(tmp3, sizeof(tmp3), "123") == 2);
    rx::tiny_stringc &a=*(rx::tiny_stringc *)tmp1, &b=*(rx::tiny_stringc *)tmp2, &c=*(rx::tiny_stringc *)tmp3;
    rt.tdd_assert(a < b);
    rt.tdd_assert(b < c);
    rt.tdd_assert(b <= c);
    rt.tdd_assert(b > a);
    rt.tdd_assert(c > b);
    rt.tdd_assert(c >= b);
    rt.tdd_assert(c != b);
}

rx_tdd(dtl_util_base)
{
    dtl_util_base_1(*this);
}



#endif // _RX_UT_CTOBJ_H_
