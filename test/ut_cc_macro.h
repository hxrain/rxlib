#ifndef _RX_UT_CC_MACRO_H_
#define _RX_UT_CC_MACRO_H_

#include "../rx_tdd.h"





//�������кŲ�Ҫ�ı�,������Բ���ͨ��.
void rx_test_CT_LINE(rx_tdd_base &rt)
{
    int RX_CT_LINE = __LINE__;
    int RX_CT_LINE = __LINE__;
    rt.assert(RX_CT_LINE_EX(13)!=RX_CT_LINE_EX(14));
    rt.assert(RX_CT_LINE_EX(13)==13);
    rt.assert(RX_CT_LINE_EX(14)==14);
}

rx_tdd(test_CC_MACRO_base)
{
    rx_test_CT_LINE(*this);
}
#endif // _RX_UT_CC_MACRO_H_
