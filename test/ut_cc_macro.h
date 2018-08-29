#ifndef _RX_UT_CC_MACRO_H_
#define _RX_UT_CC_MACRO_H_

#include "../rx_tdd.h"
#include "../rx_cc_macro.h"




//函数的行号与内部代码行位置不要改变,否则测试不能通过.
void rx_test_CT_LINE(rx_tdd_t &rt)
{
    int RX_CT_LINE = __LINE__;
    int RX_CT_LINE = __LINE__;
    rt.assert(RX_CT_LINE_EX(13)!=RX_CT_LINE_EX(14));
    rt.assert(RX_CT_LINE_EX(13)==13);
    rt.assert(RX_CT_LINE_EX(14)==14);

    int RX_CT_SYM(int_) = __LINE__;
    rt.assert(RX_CT_SYM_EX(int_, 19) == 19);
}

void rx_test_CT_BASE(rx_tdd_t &rt)
{
    rt.assert((!0)==1);
    rt.assert((!!0)==0);

    rt.assert((!2)==0);
    rt.assert((!!2)==1);
}

rx_tdd(test_CC_MACRO_base)
{
    rx_test_CT_LINE(*this);
    rx_test_CT_BASE(*this);
}
#endif // _RX_UT_CC_MACRO_H_
