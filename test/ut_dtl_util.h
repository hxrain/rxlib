#ifndef _RX_UT_DTL_UTIL_H_
#define _RX_UT_DTL_UTIL_H_

#include "../rx_dtl_util.h"
#include "../rx_tdd.h"

//---------------------------------------------------------
inline void dtl_util_base_1(rx_tdd_base &rt)
{
    uint8_t tmp1[5],tmp2[6],tmp3[7];
    rt.tdd_assert(rx::make_tiny_string(tmp1,sizeof(tmp1),"123")==0);
    rt.tdd_assert(rx::make_tiny_string(tmp2, sizeof(tmp2), "123") == 1);
    rt.tdd_assert(rx::make_tiny_string(tmp3, sizeof(tmp3), "123") == 2);
    rx::tiny_stringc &a=*(rx::tiny_stringc *)tmp1;
    rx::tiny_stringc &b=*(rx::tiny_stringc *)tmp2;
    rx::tiny_stringc &c=*(rx::tiny_stringc *)tmp3;
    rt.tdd_assert(a < b);
    rt.tdd_assert(b < c);
    rt.tdd_assert(b <= c);
    rt.tdd_assert(b > a);
    rt.tdd_assert(c > b);
    rt.tdd_assert(c >= b);
    rt.tdd_assert(c != b);

    rt.tdd_assert(strcmp(c,"12")==0);
}
//---------------------------------------------------------
#pragma pack(push,1)
typedef struct tmp_tiny_str_t
{
    int a;
    rx::tiny_stringc s;
    tmp_tiny_str_t(uint16_t c,const char* s):s(c,s){}
}tmp_tiny_str_t;
#pragma pack(pop)

inline void dtl_util_base_2(rx_tdd_base &rt)
{
    uint8_t buff[16];
    uint16_t cap=uint16_t(sizeof(buff)-sizeof(tmp_tiny_str_t));
    rx::ct::OC<tmp_tiny_str_t>((tmp_tiny_str_t*)buff,cap,"haha");
    tmp_tiny_str_t &t=*(tmp_tiny_str_t*)buff;
    rt.tdd_assert(t.s=="haha");
    rt.tdd_assert(strcmp(buff+8,"haha")==0);
}
//---------------------------------------------------------
rx_tdd(dtl_util_base)
{
    dtl_util_base_1(*this);
    dtl_util_base_2(*this);
}



#endif // _RX_UT_CTOBJ_H_
