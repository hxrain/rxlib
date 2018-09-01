#ifndef _RX_UT_DTL_UTIL_H_
#define _RX_UT_DTL_UTIL_H_

#include "../rx_str_util.h"
#include "../rx_str_tiny.h"
#include "../rx_tdd.h"

//---------------------------------------------------------
inline void str_util_base_1(rx_tdd_t &rt)
{
    uint8_t tmp1[5],tmp2[6],tmp3[7];
    rt.tdd_assert(rx::make_tiny_string(tmp1,sizeof(tmp1),"123")==0);
    rt.tdd_assert(rx::make_tiny_string(tmp2, sizeof(tmp2), "123") == 1);
    rt.tdd_assert(rx::make_tiny_string(tmp3, sizeof(tmp3), "123") == 2);
    rx::tiny_string_head_ct &a=*(rx::tiny_string_head_ct *)tmp1;
    rx::tiny_string_head_ct &b=*(rx::tiny_string_head_ct *)tmp2;
    rx::tiny_string_head_ct &c=*(rx::tiny_string_head_ct *)tmp3;
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
    rx::tiny_string_head_ct s;
    tmp_tiny_str_t(uint16_t c,const char* s):s(c,s){}
}tmp_tiny_str_t;
#pragma pack(pop)

inline void str_util_base_2(rx_tdd_t &rt)
{
    uint8_t buff[16];
    uint16_t cap=uint16_t(sizeof(buff)-sizeof(tmp_tiny_str_t));
    rx::ct::OC<tmp_tiny_str_t>((tmp_tiny_str_t*)buff,cap,"haha");

#if RX_CC == RX_CC_GCC||RX_CC_MINGW
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif
    const tmp_tiny_str_t &t=reinterpret_cast<tmp_tiny_str_t&>(buff);
#if RX_CC == RX_CC_GCC||RX_CC_MINGW
    #pragma GCC diagnostic pop
#endif

    rt.tdd_assert(t.s=="haha");
    rt.tdd_assert(strcmp((char*)buff+8,"haha")==0);
}
//---------------------------------------------------------
namespace rx
{
    template<class CT>
    inline void str_util_base_3(rx_tdd_t &rt)
    {
        CT str1[128];
        CT str2[128];
        CT strA[128];
        CT strB[128];
        CT* rs=st::strcpy(str1,sc<CT>::hex_upr());
        rt.tdd_assert(st::strcmp(rs,sc<CT>::hex_upr())==0);
        rt.tdd_assert(rs==str1);
        rt.tdd_assert(st::strlen(rs)==16);

        rt.tdd_assert(st::strcpy(str2,sizeof(str2),str1)==16);
        rt.tdd_assert(st::strcmp(str1,str2)==0);

        rt.tdd_assert(st::strcpy(str2,sizeof(str2),str1,str1,sc<CT>::space())==33);
        rt.tdd_assert(str2[16]==sc<CT>::space());
        rt.tdd_assert(st::strncmp(str2,sc<CT>::hex_upr(),16)==0);
        rt.tdd_assert(st::strncmp(str2+17,sc<CT>::hex_upr(),16)==0);
        rt.tdd_assert(st::strncmpi(str2,sc<CT>::hex_lwr(),16)==0);
        rt.tdd_assert(st::stricmp(str2+17,sc<CT>::hex_lwr())==0);

        str2[0]=0;
        rt.tdd_assert(st::strcat(str2,sizeof(str2),sc<CT>::hex_upr(),sc<CT>::hex_upr())==32);

        strA[0]=sc<CT>::A();strA[1]=0;
        strB[0]=sc<CT>::F();strB[1]=0;
        rt.tdd_assert(st::sub(str2,strA,strB,str1,sizeof(str1))==4);

        st::hex2(0x12,strB);strB[2]=0;
        rt.tdd_assert(strB[0]==sc<CT>::hex_upr(1));
        rt.tdd_assert(strB[1]==sc<CT>::hex_upr(2));

        rt.tdd_assert(st::byte((const CT*)strB)==0x12);

        rt.tdd_assert(st::strcpy(str2,sizeof(str2),sc<CT>::hex_upr(),sc<CT>::hex_upr(),sc<CT>::space())==33);
        rt.tdd_assert(st::replace(str2,sc<CT>::hex_upr(),strA,str1,sizeof(str1))!=NULL);
        rt.tdd_assert(str1[0]==sc<CT>::A());
        rt.tdd_assert(str1[1]==sc<CT>::space());
        rt.tdd_assert(str1[2]==sc<CT>::A());
        rt.tdd_assert(str1[3]==0);

        rt.tdd_assert(st::isnumber_str(sc<CT>::hex_upr(),0,true));
        rt.tdd_assert(st::isnumber_str(sc<CT>::hex_lwr(),0,true));
    }
}
//---------------------------------------------------------
rx_tdd(str_util_base)
{
    str_util_base_1(*this);
    str_util_base_2(*this);
    rx::str_util_base_3<char>(*this);
    rx::str_util_base_3<wchar_t>(*this);
}



#endif // _RX_UT_CTOBJ_H_
