#ifndef _RX_UT_DTL_UTIL_H_
#define _RX_UT_DTL_UTIL_H_

#include "../rx_str_tiny.h"
#include "../rx_str_util_std.h"
#include "../rx_str_util_ext.h"
#include "../rx_str_util_bin.h"
#include "../rx_tdd.h"

//---------------------------------------------------------
inline void str_util_base_1(rx_tdd_t &rt)
{
    uint8_t tmp1[1+sizeof(rx::tiny_string_t<>)],tmp2[2+sizeof(rx::tiny_string_t<>)],tmp3[3+sizeof(rx::tiny_string_t<>)];
    rx::tiny_string_t<> &a=*rx::tiny_string_t<>::make(tmp1, sizeof(tmp1), "123");
    rx::tiny_string_t<> &b=*rx::tiny_string_t<>::make(tmp2, sizeof(tmp2), "123");
    rx::tiny_string_t<> &c=*rx::tiny_string_t<>::make(tmp3, sizeof(tmp3), "123");
    rt.tdd_assert(a.size() == 0);
    rt.tdd_assert(b.size() == 1);
    rt.tdd_assert(c.size() == 2);
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
    rx::tiny_string_t<> s;
    tmp_tiny_str_t(uint16_t c,char* s):s(c,s){}
}tmp_tiny_str_t;
#pragma pack(pop)

inline void str_util_base_2(rx_tdd_t &rt)
{
    uint8_t buff[8+sizeof(tmp_tiny_str_t)];
    uint16_t cap=uint16_t(sizeof(buff)-sizeof(tmp_tiny_str_t));
    char *strbuf=(char*)buff+sizeof(tmp_tiny_str_t);
    tmp_tiny_str_t &t=*rx::ct::OC<tmp_tiny_str_t>((tmp_tiny_str_t*)buff,cap,strbuf);

    t.s.assign("haha");

    rt.tdd_assert(t.s=="haha");
    rt.tdd_assert(strcmp((char*)buff+sizeof(tmp_tiny_str_t),"haha")==0);
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

        rt.tdd_assert(st::strcpy(str2,sizeof(str2),str1,str1,sc<CT>::space)==33);
        rt.tdd_assert(str2[16]==sc<CT>::space);
        rt.tdd_assert(st::strncmp(str2,sc<CT>::hex_upr(),16)==0);
        rt.tdd_assert(st::strncmp(str2+17,sc<CT>::hex_upr(),16)==0);
        rt.tdd_assert(st::strncmpi(str2,sc<CT>::hex_lwr(),16)==0);
        rt.tdd_assert(st::stricmp(str2+17,sc<CT>::hex_lwr())==0);

        str2[0]=0;
        rt.tdd_assert(st::strcat(str2,sizeof(str2),sc<CT>::hex_upr(),sc<CT>::hex_upr())==32);

        strA[0]=sc<CT>::A;strA[1]=0;
        strB[0]=sc<CT>::F;strB[1]=0;
        rt.tdd_assert(st::sub(str2,strA,strB,str1,sizeof(str1))==4);

        st::hex2(0x12,strB);strB[2]=0;
        rt.tdd_assert(strB[0]==sc<CT>::hex_upr(1));
        rt.tdd_assert(strB[1]==sc<CT>::hex_upr(2));

        rt.tdd_assert(st::byte((const CT*)strB)==0x12);

        rt.tdd_assert(st::strcpy(str2,sizeof(str2),sc<CT>::hex_upr(),sc<CT>::hex_upr(),sc<CT>::space)==33);
        rt.tdd_assert(st::replace(str2,sc<CT>::hex_upr(),strA,str1,sizeof(str1))!=NULL);
        rt.tdd_assert(str1[0]==sc<CT>::A);
        rt.tdd_assert(str1[1]==sc<CT>::space);
        rt.tdd_assert(str1[2]==sc<CT>::A);
        rt.tdd_assert(str1[3]==0);

        rt.tdd_assert(st::isnumber(sc<CT>::hex_upr(),0,true));
        rt.tdd_assert(st::isnumber(sc<CT>::hex_lwr(),0,true));
    }
}

inline void str_util_cat_1(rx_tdd_t &rt)
{//小串拼装器测试.
    char buff[15];
    rx::cat_t cat(sizeof(buff),buff);
    cat="cat test";
    rt.tdd_assert(rx::st::strcmp(cat.c_str(),"cat test")==0);
    rt.tdd_assert(cat.size()==8);
    cat.fmt("%s","cat test");
    rt.tdd_assert(rx::st::strcmp(cat.c_str(),"cat test")==0);
    rt.tdd_assert(cat.size()==8);

    cat<<'1'<<'2';
    rt.tdd_assert(rx::st::strcmp(cat.c_str(),"cat test12")==0);
    rt.tdd_assert(cat.size()==10);

    cat("3")("4")("%d",5);
    rt.tdd_assert(rx::st::strcmp(cat.c_str(),"cat test12345")==0);
    rt.tdd_assert(cat.size()==13);

    cat<<"6";
    rt.tdd_assert(rx::st::strcmp(cat.c_str(),"cat test123456")==0);
    rt.tdd_assert(cat.size()==14);

    cat<<"7";
    rt.tdd_assert(rx::st::strcmp(cat.c_str(),"cat test123456")==0);
    rt.tdd_assert(cat.size()==cat.capacity());//放不进去了,所以标记出现错误了

    cat="";
    rt.tdd_assert(cat.size()==0);

    cat="123456789012345";//赋值溢出的时候,保留最大长度的值
    rt.tdd_assert(cat.size()==14);
    rt.tdd_assert(rx::st::strcmp(cat.c_str(),"12345678901234")==0);

    cat="1234567890123";
    rt.tdd_assert(cat.size()==13);
    rt.tdd_assert(rx::st::strcmp(cat.c_str(),"1234567890123")==0);

    cat("%d",4);
    rt.tdd_assert(cat.size()==14);
    rt.tdd_assert(rx::st::strcmp(cat.c_str(),"12345678901234")==0);

    cat("%d",5);
    rt.tdd_assert(cat.size()==cat.capacity());
    rt.tdd_assert(rx::st::strcmp(cat.c_str(),"12345678901234")==0);
}
//---------------------------------------------------------
rx_tdd(str_util_base)
{
    str_util_base_1(*this);
    str_util_base_2(*this);
    rx::str_util_base_3<char>(*this);
    rx::str_util_base_3<wchar_t>(*this);
    str_util_cat_1(*this);
}



#endif // _RX_UT_CTOBJ_H_
