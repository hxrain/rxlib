#ifndef _UT_DTL_HASHTBL_SKIPLIST_MAP_H_
#define _UT_DTL_HASHTBL_SKIPLIST_MAP_H_

#include "../rx_cc_macro.h"
#include "../rx_tdd.h"
#include "../rx_dtl_skiplist_raw.h"
#include "../rx_dtl_skiplist.h"
#include "../rx_dtl_hashtbl_raw.h"
#include "../rx_dtl_hashtbl_tiny.h"
#include "../rx_tdd_tick.h"
#include <map>
#include <string>
#include <list>


//本文件进行hashtbl/skiplist/std::map的性能对比
//分为三个方面:初始构造;查询;销毁.

//-----------------------------------------------------
//关联容器的性能对比测试用例(key/val都为uint32_t)
template<uint32_t MaxSize, uint32_t LoopCount,class cntr_t,class cmp_t,class key_op>
inline void ut_map_cntr_loop_1(rx_tdd_t &rt,const char* msga,const char* msgb)
{
    //测试1:容器构造并填充,之后销毁.
    tdd_tt(tt, msga, msgb);
    for (uint32_t tl = 0; tl < LoopCount; ++tl)
    {
        cntr_t cntr;
        for (uint32_t mi = 0; mi<MaxSize; ++mi)
            cntr.insert(key_op::key(mi), mi);
    }
    tdd_tt_hit(tt, "MAKE(LoopCount=%u,MaxSize=%u)", LoopCount, MaxSize);

    //测试2:单次填充
    cntr_t cntr;
    for (uint32_t mi = 0; mi<MaxSize; ++mi)
        cntr.insert(key_op::key(mi), mi);

    rt.tdd_assert(cntr.size()==MaxSize);

    //测试3:对容器进行查找
    uint64_t rc = 0;
    uint64_t ec = 0;
    for (uint32_t li = 0; li<LoopCount; ++li)
    {
        for (uint32_t mi = 0; mi<MaxSize; ++mi)
        {
            typename cntr_t::iterator I = cntr.find(key_op::key(mi));
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
}


//---------------------------------------------------------
template<uint32_t MaxSize, uint32_t LoopCount,class cntr_t,class key_op>
inline void ut_list_cntr_loop_1(rx_tdd_t &rt,const char* msga,const char* msgb)
{
    tdd_tt(tt, msga, msgb);
    //测试1:单次填充
    cntr_t cntr;
    for (uint32_t mi = 0; mi<MaxSize; ++mi)
        cntr.push_back(key_op::key(mi));

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

//---------------------------------------------------------

class tmp_cmp
{
public:
    //用于抹平不同容器迭代器对于val访问的差异而临时使用的val比较器
    template<class it,class vt>
    static bool equ(const it& I,const vt& v){return *I==v;}
    template<class vt>
    static bool equ(const std::map<uint32_t,uint32_t>::iterator& I,const vt& v){return I->second==v;}
    template<class vt>
    static bool equ(const std::map<std::string,uint32_t>::iterator& I,const vt& v){return I->second==v;}

    //用于抹平不同容器key类型的差异而临时使用的key转换器
    class key_op_uint
    {
    public:
        static uint32_t key(uint32_t i){return i;}
    };
    class key_op_uint_hash
    {
    public:
        static uint32_t key(uint32_t i){return rx_hash_skeeto_triple(i);}
    };
    class key_op_uint_hash_str
    {
    public:
        static const char* key(uint32_t i){static rx::n2s ns;return ns(rx_hash_skeeto_triple(i));}
    };
};

//---------------------------------------------------------
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
//最终进行对比测试的功能封装
template<uint32_t LoopCount,uint32_t MaxSize,bool test_hashtbl=true>
class map_cnrt_test
{
public:
    //执行对比测试
    static void test(rx_tdd_t &rt)
    {
        ut_list_cntr_loop_1<MaxSize, LoopCount,std::list<uint32_t>,tmp_cmp::key_op_uint >(rt,"    std::list","int");
        ut_list_cntr_loop_1<MaxSize, LoopCount,std::list<std::string>,tmp_cmp::key_op_uint_hash_str >(rt,"    std::list","string");

        printf("\n");
        if (test_hashtbl)
        {
            ut_map_cntr_loop_1 <MaxSize, LoopCount, rx::tiny_hashtbl_t<uint32_t(MaxSize*2.3)>, tmp_cmp, tmp_cmp::key_op_uint>(rt, " tiny_hashtbl", "int/int");
            ut_map_cntr_loop_1 <MaxSize, LoopCount, rx::tiny_hashlist_t<uint32_t(MaxSize*2.3)>, tmp_cmp, tmp_cmp::key_op_uint>(rt, "tiny_hashlist", "int/int");
        }
        ut_map_cntr_loop_1 <MaxSize, LoopCount, rx::skiplist_t<uint32_t, uint32_t>,tmp_cmp,tmp_cmp::key_op_uint>(rt, "     skiplist", "int/int");
        ut_map_cntr_loop_1 <MaxSize, LoopCount, stdmap<uint32_t>,tmp_cmp,tmp_cmp::key_op_uint >(rt,"     std::map","int/int");

        printf("\n");
        if (test_hashtbl)
        {
            ut_map_cntr_loop_1 <MaxSize, LoopCount, rx::tiny_hashtbl_t<uint32_t(MaxSize*1.3),uint32_t>, tmp_cmp, tmp_cmp::key_op_uint_hash>(rt, " tiny_hashtbl", "hash(int)/int");
            ut_map_cntr_loop_1 <MaxSize, LoopCount, rx::tiny_hashlist_t<uint32_t(MaxSize*1.3),uint32_t>, tmp_cmp, tmp_cmp::key_op_uint_hash>(rt, "tiny_hashlist","hash(int)/int");
        }
        ut_map_cntr_loop_1 <MaxSize, LoopCount, rx::skiplist_t<uint32_t, uint32_t>,tmp_cmp,tmp_cmp::key_op_uint_hash>(rt, "     skiplist", "hash(int)/int");
        ut_map_cntr_loop_1 <MaxSize, LoopCount, stdmap<uint32_t>,tmp_cmp,tmp_cmp::key_op_uint_hash >(rt,"     std::map","hash(int)/int");
        
        printf("\n");
        ut_map_cntr_loop_1 <MaxSize, LoopCount, stdmap<std::string>,tmp_cmp,tmp_cmp::key_op_uint_hash_str>(rt,"     std::map","std::string(hash(int)/int");
        ut_map_cntr_loop_1 <MaxSize, LoopCount, rx::skiplist_t<const char*,uint32_t>,tmp_cmp,tmp_cmp::key_op_uint_hash_str>(rt,"     skiplist", "tinystr(hash(int))/int");
        ut_map_cntr_loop_1 <MaxSize, LoopCount, rx::skiplist_t<std::string,uint32_t>,tmp_cmp,tmp_cmp::key_op_uint_hash_str>(rt,"     skiplist", "std::string(hash(int))/int");
        if (test_hashtbl)
        {
            ut_map_cntr_loop_1 <MaxSize, LoopCount, rx::tiny_hashtbl_st<uint32_t(MaxSize*2.3)>, tmp_cmp, tmp_cmp::key_op_uint_hash_str>(rt, " tiny_hashtbl", "tinystr/int");
            ut_map_cntr_loop_1 <MaxSize, LoopCount, rx::tiny_hashlist_st<uint32_t(MaxSize*2.3)>, tmp_cmp, tmp_cmp::key_op_uint_hash_str>(rt, "tiny_hashlist", "tinystr/int");
        }
    }
};

//---------------------------------------------------------
rx_tdd(htbl_sklist_map_loop)
{
    map_cnrt_test<1000,2000>::test(*this);
}

rx_tdd_rtl(htbl_sklist_map_loop,tdd_level_slow)
{
    map_cnrt_test<500,100000,false>::test(*this);
}

#endif
