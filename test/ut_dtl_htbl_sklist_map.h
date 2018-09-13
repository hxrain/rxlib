#ifndef _UT_DTL_HASHTBL_SKIPLIST_MAP_H_
#define _UT_DTL_HASHTBL_SKIPLIST_MAP_H_

#include "../rx_cc_macro.h"
#include "../rx_tdd.h"
#include "../rx_raw_skiplist.h"
#include "../rx_dtl_skiplist.h"
#include "../rx_raw_hashtbl.h"
#include "../rx_dtl_hashtbl_tiny.h"
#include "../rx_tdd_tick.h"
#include <map>
#include <string>
#include <list>


//本文件进行tiny_hashtbl/tiny_skiplist/std::map的性能对比
//分为三个方面:初始构造;查询;销毁.
namespace rx
{
    //-----------------------------------------------------
    //关联容器的性能对比测试用例(key/val都为uint32_t)
    template<uint32_t MaxSize, uint32_t LoopCount,class cntr_t,class cmp_t>
    inline void ut_tiny_map_cntr_loop_1(rx_tdd_t &rt,const char* msga,const char* msgb)
    {
        //测试1:容器构造并填充,之后销毁.
        tdd_tt(tt, msga, msgb);
        for (uint32_t tl = 0; tl < LoopCount; ++tl)
        {
            cntr_t cntr;
            for (uint32_t mi = 0; mi<MaxSize; ++mi)
                cntr.insert(mi, mi);
        }
        tdd_tt_hit(tt, "MAKE(LoopCount=%u,MaxSize=%u)", LoopCount, MaxSize);

        //测试2:单次填充
        cntr_t cntr;
        for (uint32_t mi = 0; mi<MaxSize; ++mi)
            cntr.insert(rx_hash_skeeto_triple(mi), mi);

        rt.tdd_assert(cntr.size()==MaxSize);
        tdd_tt_hit(tt, "BEGIN(LoopCount=%u,MaxSize=%u)", LoopCount, MaxSize);

        //测试3:对容器进行查找
        uint64_t rc = 0;
        uint64_t ec = 0;
        for (uint32_t li = 0; li<LoopCount; ++li)
        {
            for (uint32_t mi = 0; mi<MaxSize; ++mi)
            {
                typename cntr_t::iterator I = cntr.find(rx_hash_skeeto_triple(mi));
                if (I != cntr.end() && cmp_t::equ(I,mi))
                    ++rc;
                else
                    ++ec;
            }
        }
        rt.tdd_assert(rc == LoopCount*MaxSize);
        tdd_tt_hit(tt, "FIND(LoopCount=%u,MaxSize=%u)", LoopCount, MaxSize);

        //测试4:对容器进行遍历
        rc = 0;
        for (uint32_t li = 0; li<LoopCount; ++li)
        {
            for (typename cntr_t::iterator I = cntr.begin(); I != cntr.end(); ++I)
                if (I != cntr.end())
                    ++rc;
        }
        rt.tdd_assert(rc == LoopCount*MaxSize);
        tdd_tt_hit(tt, "FOR(LoopCount=%u,MaxSize=%u)", LoopCount, MaxSize);

        //测试5:单次遍历
        rc = 0;
        for (typename cntr_t::iterator I = cntr.begin(); I != cntr.end(); ++I)
            if (I != cntr.end())
                ++rc;
        rt.tdd_assert(rc == MaxSize);
        tdd_tt_hit(tt,"FOR(MaxSize=%u)",MaxSize);
    }
    //-----------------------------------------------------
    //关联容器的性能对比测试用例(key为字符串)
    template<uint32_t MaxSize, uint32_t LoopCount,class cntr_t,class cmp_t>
    inline void ut_tiny_map_cntr_loop_2(rx_tdd_t &rt,const char* msga,const char* msgb)
    {
        //测试1:容器构造并填充,之后销毁.
        n2s ns;
        tdd_tt(tt,msga,msgb);
        for (uint32_t tl = 0; tl < LoopCount; ++tl)
        {
            cntr_t cntr;
            for(uint32_t mi=0;mi<MaxSize;++mi)
                cntr.insert(ns(rx_hash_skeeto_triple(mi)),mi);
        }
        tdd_tt_hit(tt,"MAKE(LoopCount=%u,MaxSize=%u)",LoopCount,MaxSize);

        //测试2:单次填充
        cntr_t cntr;
        for(uint32_t mi=0;mi<MaxSize;++mi)
            cntr.insert(ns(rx_hash_skeeto_triple(mi)),mi);

        rt.tdd_assert(cntr.size()==MaxSize);
        tdd_tt_hit(tt, "BEGIN(LoopCount=%u,MaxSize=%u)", LoopCount, MaxSize);

        //测试3:对容器进行查找
        rand_skeeto_triple_t rnd;
        uint64_t rc = 0;
        for(uint32_t li=0;li<LoopCount;++li)
        {
            for(uint32_t mi=0;mi<MaxSize;++mi)
            {
                uint32_t i=rnd.get(MaxSize-1);
                typename cntr_t::iterator I=cntr.find(ns(rx_hash_skeeto_triple(i)));
                if (I != cntr.end() && cmp_t::equ(I,i))
                    rc++;
            }
        }
        rt.tdd_assert(rc== LoopCount*MaxSize);
        tdd_tt_hit(tt,"FIND(LoopCount=%u,MaxSize=%u)",LoopCount,MaxSize);

        //测试4:对容器进行遍历
        rc = 0;
        for(uint32_t li=0;li<LoopCount;++li)
        {
            for (typename cntr_t::iterator I = cntr.begin(); I != cntr.end(); ++I)
                if (I != cntr.end())
                    ++rc;
        }
        rt.tdd_assert(rc == LoopCount*MaxSize);
        tdd_tt_hit(tt,"FOR(LoopCount=%u,MaxSize=%u)",LoopCount,MaxSize);

        //测试5:单次遍历
        rc = 0;
            for (typename cntr_t::iterator I = cntr.begin(); I != cntr.end(); ++I)
                if (I != cntr.end())
                    ++rc;
        rt.tdd_assert(rc == MaxSize);
        tdd_tt_hit(tt,"FOR(MaxSize=%u)",MaxSize);
    }
}


template<uint32_t MaxSize, uint32_t LoopCount,class cntr_t>
inline void ut_list_cntr_loop_1(rx_tdd_t &rt,const char* msga,const char* msgb)
{
    tdd_tt(tt, msga, msgb);
    //测试1:单次填充
    cntr_t cntr;
    for (uint32_t mi = 0; mi<MaxSize; ++mi)
        cntr.push_back(rx_hash_skeeto_triple(mi));

    rt.tdd_assert(cntr.size()==MaxSize);
    tdd_tt_hit(tt, "BEGIN(LoopCount=%u,MaxSize=%u)", LoopCount, MaxSize);

    //测试2:对容器进行遍历
    uint64_t rc = 0;
    for(uint32_t li=0;li<LoopCount;++li)
    {
        for (typename cntr_t::iterator I = cntr.begin(); I != cntr.end(); ++I)
            if (I != cntr.end())
                ++rc;
    }
    rt.tdd_assert(rc == LoopCount*MaxSize);
    tdd_tt_hit(tt,"FOR(LoopCount=%u,MaxSize=%u)",LoopCount,MaxSize);
}

template<uint32_t MaxSize, uint32_t LoopCount,class cntr_t>
inline void ut_list_cntr_loop_2(rx_tdd_t &rt,const char* msga,const char* msgb)
{
    tdd_tt(tt, msga, msgb);
    rx::n2s ns;
    //测试1:单次填充
    cntr_t cntr;
    for (uint32_t mi = 0; mi<MaxSize; ++mi)
        cntr.push_back(ns(rx_hash_skeeto_triple(mi)));

    rt.tdd_assert(cntr.size()==MaxSize);
    tdd_tt_hit(tt, "BEGIN(LoopCount=%u,MaxSize=%u)", LoopCount, MaxSize);

    //测试2:对容器进行遍历
    uint64_t rc = 0;
    for(uint32_t li=0;li<LoopCount;++li)
    {
        for (typename cntr_t::iterator I = cntr.begin(); I != cntr.end(); ++I)
            if (I != cntr.end())
                ++rc;
    }
    rt.tdd_assert(rc == LoopCount*MaxSize);
    tdd_tt_hit(tt,"FOR(LoopCount=%u,MaxSize=%u)",LoopCount,MaxSize);
}

//用于抹平不同容器迭代器对于val访问的差异而临时使用的val比较器
class tmp_cmp
{
public:
    template<class it,class vt>
    static bool equ(const it& I,const vt& v){return *I==v;}
    template<class vt>
    static bool equ(const std::map<uint32_t,uint32_t>::iterator& I,const vt& v){return I->second==v;}
    template<class vt>
    static bool equ(const std::map<std::string,uint32_t>::iterator& I,const vt& v){return I->second==v;}
};

//用于抹平std::map的insert接口语义的差异而临时定义的map包装类
template<class key_t>
class stdmap:public std::map<key_t,uint32_t>
{
    typedef std::map<key_t,uint32_t> super;
public:
    int insert(const key_t& key,uint32_t val)
    {
        try{
            super::operator[](key)=val;
            return 1;
        }catch(...){return -1;}
    }

};

//---------------------------------------------------------
rx_tdd(htbl_sklist_map_loop)
{
    const uint32_t LoopCount=100,MaxSize=1000;

    ut_list_cntr_loop_1<MaxSize, LoopCount,std::list<uint32_t> >(*this,"    std::list","int");
    ut_list_cntr_loop_2<MaxSize, LoopCount,std::list<std::string> >(*this,"    std::list","string");

    rx::ut_tiny_map_cntr_loop_1 <MaxSize, LoopCount, rx::tiny_hashtbl_uint32_t<uint32_t(MaxSize*1.3)>,tmp_cmp>(*this, " tiny_hashtbl","int/int");
    rx::ut_tiny_map_cntr_loop_1 <MaxSize, LoopCount, rx::tiny_skiplist_t<uint32_t, uint32_t>,tmp_cmp>(*this, "tiny_skiplist", "int/int");
    rx::ut_tiny_map_cntr_loop_1 <MaxSize, LoopCount, stdmap<uint32_t>,tmp_cmp >(*this,"     std::map","int/int");

    rx::ut_tiny_map_cntr_loop_2 <MaxSize, LoopCount, rx::tiny_skiplist_t<const char*,uint32_t>,tmp_cmp>(*this,"tiny_skiplist", "const char*/int");
    rx::ut_tiny_map_cntr_loop_2 <MaxSize, LoopCount, rx::tiny_skiplist_t<std::string,uint32_t>,tmp_cmp>(*this,"tiny_skiplist", "std::string/int");
    rx::ut_tiny_map_cntr_loop_2 <MaxSize, LoopCount, stdmap<std::string>,tmp_cmp>(*this,"     std::map","std::string/int");
}

rx_tdd_rtl(htbl_sklist_map_loop,tdd_level_std)
{
    const uint32_t LoopCount=500,MaxSize=100000;
    ut_list_cntr_loop_1<MaxSize, LoopCount,std::list<uint32_t> >(*this,"    std::list","int");
    ut_list_cntr_loop_2<MaxSize, LoopCount,std::list<std::string> >(*this,"    std::list","string");

    rx::ut_tiny_map_cntr_loop_1 <MaxSize, LoopCount, rx::tiny_skiplist_t<uint32_t, uint32_t>,tmp_cmp>(*this, "tiny_skiplist", "int/int");
    rx::ut_tiny_map_cntr_loop_1 <MaxSize, LoopCount, stdmap<uint32_t>,tmp_cmp>(*this,"     std::map","int/int");

    rx::ut_tiny_map_cntr_loop_2 <MaxSize, LoopCount, rx::tiny_skiplist_t<const char*,uint32_t>,tmp_cmp>(*this,"tiny_skiplist", "const char*/int");
    rx::ut_tiny_map_cntr_loop_2 <MaxSize, LoopCount, rx::tiny_skiplist_t<std::string,uint32_t>,tmp_cmp>(*this,"tiny_skiplist", "std::string/int");
    rx::ut_tiny_map_cntr_loop_2 <MaxSize, LoopCount, stdmap<std::string>,tmp_cmp>(*this,"     std::map","std::string/int");
}

#endif
