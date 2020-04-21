#ifndef _RX_UT_DTL_UTIL_H_
#define _RX_UT_DTL_UTIL_H_

#include "../rx_str_tiny.h"
#include "../rx_str_util_std.h"
#include "../rx_str_util_ext.h"
#include "../rx_str_util_bin.h"
#include "../rx_tdd.h"
#include "../rx_hash_rand.h"
#include "../rx_tdd_tick.h"

//---------------------------------------------------------
inline void ut_str_util_base_1(rx_tdd_t &rt)
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

inline void ut_str_util_base_2(rx_tdd_t &rt)
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
    inline void ut_str_util_base_3(rx_tdd_t &rt)
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

inline void ut_str_util_cat_1(rx_tdd_t &rt)
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

class ut_strstr
{
public:
    typedef const char* (*strstr_t)(const char *haystack, const char *needle);
    strstr_t    fun;
    char m_buff_main[1280];
    char m_buff_sub[340];
    ut_strstr(){rx_rnd_t32().seed(1);}
    void make_main()
    {
        for(uint32_t i=0;i<sizeof(m_buff_main);++i)
            m_buff_main[i]=rx_rnd_t32().get(10)+'0';
        m_buff_main[sizeof(m_buff_main)-1]=0;
    }
    void make_sub(uint32_t offset,uint32_t len)
    {
        rx_assert(offset+len<sizeof(m_buff_main));
        strncpy(m_buff_sub,m_buff_main+offset,len);
        m_buff_sub[len]=0;
    }
    uint32_t loop_by_size(uint32_t len)
    {
        uint32_t bad=0;
        for(uint32_t offset=0;offset<sizeof(m_buff_main)-1-len;++offset)
        {
            make_sub(offset,len);
            const char* pos=fun(m_buff_main,m_buff_sub);
            if (pos==NULL)
                ++bad;
        }
        return bad;
    }
    uint32_t loop()
    {
        uint32_t bad=0;
        make_main();
        for(uint32_t len=1;len<sizeof(m_buff_sub)-1;++len)
            bad+=loop_by_size(len);
        return bad;
    }
};
inline void ut_str_util_faststrstr_2(rx_tdd_t &rt)
{
    ut_strstr us;

    tdd_tt(t, "strstr", "");

    us.fun=(ut_strstr::strstr_t)strstr;
    uint32_t bad=us.loop();
    rt.assert(bad==0);
    tdd_tt_msg(t, "libc");

    us.fun=(ut_strstr::strstr_t)rx::st::strstrx<char>;
    bad=us.loop();
    rt.assert(bad==0);
    tdd_tt_msg(t, "rx");
}
//---------------------------------------------------------
inline void ut_str_util_faststrstr_1(rx_tdd_t &rt)
{
    rt.tdd_assert(*rx::st::strstrx("345246","46")=='4');
    rt.tdd_assert(*rx::st::strstrx("0123456789","234")=='2');
    rt.tdd_assert( rx::st::strstrx("0123456789","246")==NULL);
    rt.tdd_assert(*rx::st::strstrx("012345246789","46")=='4');
    rt.tdd_assert(*rx::st::strstrx("012345246789","467")=='4');
    rt.tdd_assert(*rx::st::strstrx("012345246789","4678")=='4');
    rt.tdd_assert(*rx::st::strstrx("012345246789","46789")=='4');
    rt.tdd_assert(*rx::st::strstrx(L"012345246789",L"246")==L'2');
    rt.tdd_assert(*rx::st::strstrx("012345246789","2467")=='2');
    rt.tdd_assert(*rx::st::strstrx("012345246789","24678")=='2');
    rt.tdd_assert(*rx::st::strstrx("012345246789","246789")=='2');
}
//---------------------------------------------------------
inline void ut_str_util_fastmemmem_1(rx_tdd_t &rt)
{
    rt.tdd_assert(*rx::st::memmemx("345246",6,"46",2)=='4');
    rt.tdd_assert(*rx::st::memmemx("0123456789",10,"234",3)=='2');
    rt.tdd_assert( rx::st::memmemx("0123456789",10,"246",3)==NULL);
    rt.tdd_assert(*rx::st::memmemx("012345246789",12,"46",2)=='4');
    rt.tdd_assert(*rx::st::memmemx("012345246789",12,"467",3)=='4');
    rt.tdd_assert(*rx::st::memmemx("012345246789",12,"4678",4)=='4');
    rt.tdd_assert(*rx::st::memmemx("012345246789",12,"46789",5)=='4');
    rt.tdd_assert(*rx::st::memmemx("012345246789",12,"246",3)=='2');
    rt.tdd_assert(*rx::st::memmemx("012345246789",12,"2467",4)=='2');
    rt.tdd_assert(*rx::st::memmemx("012345246789",12,"24678",5)=='2');
    rt.tdd_assert(*rx::st::memmemx("012345246789",12,"246789",6)=='2');
}
//---------------------------------------------------------
rx_tdd(ut_str_util_base)
{
    ut_str_util_fastmemmem_1(*this);
    ut_str_util_faststrstr_2(*this);
    ut_str_util_faststrstr_1(*this);
    ut_str_util_base_1(*this);
    ut_str_util_base_2(*this);
    rx::ut_str_util_base_3<char>(*this);
    rx::ut_str_util_base_3<wchar_t>(*this);
    ut_str_util_cat_1(*this);
}



#endif // _RX_UT_CTOBJ_H_
