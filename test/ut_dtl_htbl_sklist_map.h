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


//本文件进行tiny_hashtbl/tiny_skiplist/std::map的性能对比
//分为三个方面:初始构造;查询;销毁.
namespace rx
{
    //-----------------------------------------------------
    //定长hash表的性能测试
    template<uint32_t TestCount,uint32_t MaxSize, uint32_t LoopCount>
    inline void ut_tiny_hashtbl_loop_1(rx_tdd_t &rt)
    {
        typedef tiny_hashtbl_uint32_t<uint32_t(MaxSize*1.3)> cntr_t;
        tt(" tiny_hashtbl");
        for (uint32_t tl = 0; tl < TestCount; ++tl)
        {
            cntr_t cntr;
            for(uint32_t mi=0;mi<MaxSize;++mi)
                cntr.insert(mi,mi);
        }
        tdd_tt_hit(tt,"MAKE/CLEAN(TestCount=%u,MaxSize=%u,LoopCount=%u)",TestCount,MaxSize,LoopCount);

        cntr_t cntr;
        for(uint32_t mi=0;mi<MaxSize;++mi)
            cntr.insert(mi,mi);

        rand_skeeto_triple_t rnd;
        for(uint32_t li=0;li<LoopCount;++li)
        {
            for(uint32_t mi=0;mi<MaxSize;++mi)
            {
                uint32_t i=rnd.get(MaxSize-1);
                typename cntr_t::iterator I=cntr.find(i);
                rx_check(I!=cntr.end()&&*I==i);
            }
        }
        tdd_tt_hit(tt,"FIND/LOOP(TestCount=%u,MaxSize=%u,LoopCount=%u)",TestCount,MaxSize,LoopCount);

        for(uint32_t li=0;li<LoopCount;++li)
        {
            for(typename cntr_t::iterator I=cntr.begin();I!=cntr.end();++I)
                rx_check(I!=cntr.end());
        }
        tdd_tt_hit(tt,"FOR/LOOP(TestCount=%u,MaxSize=%u,LoopCount=%u)",TestCount,MaxSize,LoopCount);
    }

    //-----------------------------------------------------
    //变长跳表的性能测试
    template<uint32_t TestCount, uint32_t MaxSize, uint32_t LoopCount>
    inline void ut_tiny_skiplist_loop_1(rx_tdd_t &rt)
    {
        typedef tiny_skiplist_t<uint32_t,uint32_t,8> cntr_t;
        tt("tiny_skiplist");
        for (uint32_t tl = 0; tl < TestCount; ++tl)
        {
            cntr_t cntr;
            for(uint32_t mi=0;mi<MaxSize;++mi)
                cntr.insert(mi,mi);
        }
        tdd_tt_hit(tt,"MAKE/CLEAN(TestCount=%u,MaxSize=%u,LoopCount=%u)",TestCount,MaxSize,LoopCount);

        cntr_t cntr;
        for(uint32_t mi=0;mi<MaxSize;++mi)
            cntr.insert(mi,mi);

        #if RX_RAW_SKIPLIST_DEBUG_PRINT
        cntr.print();
        #endif

        rand_skeeto_triple_t rnd;
        for(uint32_t li=0;li<LoopCount;++li)
        {
            for(uint32_t mi=0;mi<MaxSize;++mi)
            {
                uint32_t i=rnd.get(MaxSize-1);
                cntr_t::iterator I=cntr.find(i);
                rx_check(I!=cntr.end()&&*I==i);
            }
        }
        tdd_tt_hit(tt,"FIND/LOOP(TestCount=%u,MaxSize=%u,LoopCount=%u)",TestCount,MaxSize,LoopCount);

        for(uint32_t li=0;li<LoopCount;++li)
        {
            for(typename cntr_t::iterator I=cntr.begin();I!=cntr.end();++I)
                rx_check(I!=cntr.end());
        }
        tdd_tt_hit(tt,"FOR/LOOP(TestCount=%u,MaxSize=%u,LoopCount=%u)",TestCount,MaxSize,LoopCount);
    }

    //-----------------------------------------------------
    //标准std::map的性能测试,用于对比
    template<uint32_t TestCount, uint32_t MaxSize, uint32_t LoopCount>
    inline void ut_tiny_map_loop_1(rx_tdd_t &rt)
    {
        typedef std::map<uint32_t,uint32_t> cntr_t;
        tt("     std::map");
        for (uint32_t tl = 0; tl < TestCount; ++tl)
        {
            cntr_t cntr;
            for(uint32_t mi=0;mi<MaxSize;++mi)
                cntr[mi]=mi;
        }
        tdd_tt_hit(tt,"MAKE/CLEAN(TestCount=%u,MaxSize=%u,LoopCount=%u)",TestCount,MaxSize,LoopCount);

        cntr_t cntr;
        for(uint32_t mi=0;mi<MaxSize;++mi)
            cntr[mi]=mi;

        rand_skeeto_triple_t rnd;
        for(uint32_t li=0;li<LoopCount;++li)
        {
            for(uint32_t mi=0;mi<MaxSize;++mi)
            {
                uint32_t i=rnd.get(MaxSize-1);
                cntr_t::iterator I=cntr.find(i);
                rx_check(I!=cntr.end()&&I->second==i);
            }
        }
        tdd_tt_hit(tt,"FIND/LOOP(TestCount=%u,MaxSize=%u,LoopCount=%u)",TestCount,MaxSize,LoopCount);

        for(uint32_t li=0;li<LoopCount;++li)
        {
            for(typename cntr_t::iterator I=cntr.begin();I!=cntr.end();++I)
                rx_check(I!=cntr.end());
        }
        tdd_tt_hit(tt,"FOR/LOOP(TestCount=%u,MaxSize=%u,LoopCount=%u)",TestCount,MaxSize,LoopCount);
    }
}

//---------------------------------------------------------
rx_tdd(htbl_sklist_map_loop)
{
    const uint32_t TestCount=500,LoopCount=500,MaxSize=1000;
    rx::ut_tiny_hashtbl_loop_1  <TestCount, MaxSize, LoopCount>(*this);
    rx::ut_tiny_skiplist_loop_1 <TestCount, MaxSize, LoopCount>(*this);
    rx::ut_tiny_map_loop_1      <TestCount, MaxSize, LoopCount>(*this);

    rx::ut_tiny_hashtbl_loop_1  <TestCount, MaxSize*5, LoopCount>(*this);
    rx::ut_tiny_skiplist_loop_1 <TestCount, MaxSize*5, LoopCount>(*this);
    rx::ut_tiny_map_loop_1      <TestCount, MaxSize*5, LoopCount>(*this);

}



#endif
