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


//本文件进行tiny_hashtbl/tiny_skiplist/std::map的性能对比
//分为三个方面:初始构造;查询;销毁.
namespace rx
{
    //-----------------------------------------------------
    //定长hash表的性能测试
    template<uint32_t MaxSize, uint32_t LoopCount>
    inline void ut_tiny_hashtbl_loop_1(rx_tdd_t &rt)
    {
        typedef tiny_hashtbl_uint32_t<uint32_t(MaxSize*1.3)> cntr_t;
        tdd_tt(tt," tiny_hashtbl","int/int");
        for (uint32_t tl = 0; tl < LoopCount; ++tl)
        {
            cntr_t cntr;
            for(uint32_t mi=0;mi<MaxSize;++mi)
                cntr.insert(mi,mi);
        }
        tdd_tt_hit(tt,"MAKE/CLEAN(LoopCount=%u,MaxSize=%u)",LoopCount,MaxSize);

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
        tdd_tt_hit(tt,"FIND/LOOP(LoopCount=%u,MaxSize=%u)",LoopCount,MaxSize);

        for(uint32_t li=0;li<LoopCount;++li)
        {
            for(typename cntr_t::iterator I=cntr.begin();I!=cntr.end();++I)
                rx_check(I!=cntr.end());
        }
        tdd_tt_hit(tt,"FOR/LOOP(LoopCount=%u,MaxSize=%u)",LoopCount,MaxSize);
    }

    //-----------------------------------------------------
    //变长跳表的性能测试
    template<uint32_t MaxSize, uint32_t LoopCount>
    inline void ut_tiny_skiplist_loop_1(rx_tdd_t &rt)
    {
        typedef tiny_skiplist_t<uint32_t,uint32_t> cntr_t;
        tdd_tt(tt,"tiny_skiplist","int/int");
        for (uint32_t tl = 0; tl < LoopCount; ++tl)
        {
            cntr_t cntr;
            for(uint32_t mi=0;mi<MaxSize;++mi)
                cntr.insert(mi,mi);
        }
        tdd_tt_hit(tt,"MAKE/CLEAN(LoopCount=%u,MaxSize=%u)",LoopCount,MaxSize);

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
        tdd_tt_hit(tt,"FIND/LOOP(LoopCount=%u,MaxSize=%u)",LoopCount,MaxSize);

        for(uint32_t li=0;li<LoopCount;++li)
        {
            for(typename cntr_t::iterator I=cntr.begin();I!=cntr.end();++I)
                rx_check(I!=cntr.end());
        }
        tdd_tt_hit(tt,"FOR/LOOP(LoopCount=%u,MaxSize=%u)",LoopCount,MaxSize);
    }

    //-----------------------------------------------------
    //标准std::map的性能测试,用于对比
    template<uint32_t MaxSize, uint32_t LoopCount>
    inline void ut_tiny_map_loop_1(rx_tdd_t &rt)
    {
        typedef std::map<uint32_t,uint32_t> cntr_t;
        tdd_tt(tt,"     std::map","int/int");
        for (uint32_t tl = 0; tl < LoopCount; ++tl)
        {
            cntr_t cntr;
            for(uint32_t mi=0;mi<MaxSize;++mi)
                cntr[mi]=mi;
        }
        tdd_tt_hit(tt,"MAKE/CLEAN(LoopCount=%u,MaxSize=%u)",LoopCount,MaxSize);

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
        tdd_tt_hit(tt,"FIND/LOOP(LoopCount=%u,MaxSize=%u)",LoopCount,MaxSize);

        for(uint32_t li=0;li<LoopCount;++li)
        {
            for(typename cntr_t::iterator I=cntr.begin();I!=cntr.end();++I)
                rx_check(I!=cntr.end());
        }
        tdd_tt_hit(tt,"FOR/LOOP(LoopCount=%u,MaxSize=%u)",LoopCount,MaxSize);
    }

    //-----------------------------------------------------
    //变长跳表的性能测试
    template<uint32_t MaxSize, uint32_t LoopCount>
    inline void ut_tiny_skiplist_loop_2(rx_tdd_t &rt)
    {
        typedef tiny_skiplist_t<const char*,uint32_t> cntr_t;
        n2s ns;
        tdd_tt(tt,"tiny_skiplist","string/int");
        for (uint32_t tl = 0; tl < LoopCount; ++tl)
        {
            cntr_t cntr;
            for(uint32_t mi=0;mi<MaxSize;++mi)
                cntr.insert(ns(mi),mi);
        }
        tdd_tt_hit(tt,"MAKE/CLEAN(LoopCount=%u,MaxSize=%u)",LoopCount,MaxSize);

        cntr_t cntr;
        for(uint32_t mi=0;mi<MaxSize;++mi)
            cntr.insert(ns(mi),mi);

        rand_skeeto_triple_t rnd;
        for(uint32_t li=0;li<LoopCount;++li)
        {
            for(uint32_t mi=0;mi<MaxSize;++mi)
            {
                uint32_t i=rnd.get(MaxSize-1);
                cntr_t::iterator I=cntr.find(ns(i));
                rx_check(I!=cntr.end()&&*I==i);
            }
        }
        tdd_tt_hit(tt,"FIND/LOOP(LoopCount=%u,MaxSize=%u)",LoopCount,MaxSize);

        for(uint32_t li=0;li<LoopCount;++li)
        {
            for(typename cntr_t::iterator I=cntr.begin();I!=cntr.end();++I)
                rx_check(I!=cntr.end());
        }
        tdd_tt_hit(tt,"FOR/LOOP(LoopCount=%u,MaxSize=%u)",LoopCount,MaxSize);
    }
    //-----------------------------------------------------
    //变长跳表的性能测试
    template<uint32_t MaxSize, uint32_t LoopCount>
    inline void ut_tiny_skiplist_loop_2s(rx_tdd_t &rt)
    {
        typedef tiny_skiplist_t<std::string,uint32_t> cntr_t;
        n2s ns;
        tdd_tt(tt,"tiny_skiplist/2s","string/int");
        for (uint32_t tl = 0; tl < LoopCount; ++tl)
        {
            cntr_t cntr;
            for(uint32_t mi=0;mi<MaxSize;++mi)
                cntr.insert(ns(mi),mi);
        }
        tdd_tt_hit(tt,"MAKE/CLEAN(LoopCount=%u,MaxSize=%u)",LoopCount,MaxSize);

        cntr_t cntr;
        for(uint32_t mi=0;mi<MaxSize;++mi)
            cntr.insert(ns(mi),mi);

        rand_skeeto_triple_t rnd;
        for(uint32_t li=0;li<LoopCount;++li)
        {
            for(uint32_t mi=0;mi<MaxSize;++mi)
            {
                uint32_t i=rnd.get(MaxSize-1);
                cntr_t::iterator I=cntr.find(ns(i));
                rx_check(I!=cntr.end()&&*I==i);
            }
        }
        tdd_tt_hit(tt,"FIND/LOOP(LoopCount=%u,MaxSize=%u)",LoopCount,MaxSize);

        for(uint32_t li=0;li<LoopCount;++li)
        {
            for(typename cntr_t::iterator I=cntr.begin();I!=cntr.end();++I)
                rx_check(I!=cntr.end());
        }
        tdd_tt_hit(tt,"FOR/LOOP(LoopCount=%u,MaxSize=%u)",LoopCount,MaxSize);
    }
    //-----------------------------------------------------
    //标准std::map的性能测试,用于对比
    template<uint32_t MaxSize, uint32_t LoopCount>
    inline void ut_tiny_map_loop_2(rx_tdd_t &rt)
    {
        typedef std::map<std::string,uint32_t> cntr_t;
        tdd_tt(tt,"     std::map","string/int");

        n2s ns;
        for (uint32_t tl = 0; tl < LoopCount; ++tl)
        {
            cntr_t cntr;
            for(uint32_t mi=0;mi<MaxSize;++mi)
                cntr[ns(mi)]=mi;
        }
        tdd_tt_hit(tt,"MAKE/CLEAN(LoopCount=%u,MaxSize=%u)",LoopCount,MaxSize);

        cntr_t cntr;
        for(uint32_t mi=0;mi<MaxSize;++mi)
            cntr[ns(mi)]=mi;

        rand_skeeto_triple_t rnd;
        for(uint32_t li=0;li<LoopCount;++li)
        {
            for(uint32_t mi=0;mi<MaxSize;++mi)
            {
                uint32_t i=rnd.get(MaxSize-1);
                cntr_t::iterator I=cntr.find(ns(i));
                rx_check(I!=cntr.end()&&I->second==i);
            }
        }
        tdd_tt_hit(tt,"FIND/LOOP(LoopCount=%u,MaxSize=%u)",LoopCount,MaxSize);

        for(uint32_t li=0;li<LoopCount;++li)
        {
            for(typename cntr_t::iterator I=cntr.begin();I!=cntr.end();++I)
                rx_check(I!=cntr.end());
        }
        tdd_tt_hit(tt,"FOR/LOOP(LoopCount=%u,MaxSize=%u)",LoopCount,MaxSize);
    }
}

//---------------------------------------------------------
rx_tdd(htbl_sklist_map_loop)
{
    const uint32_t LoopCount=100,MaxSize=1000;
    
    rx::ut_tiny_hashtbl_loop_1  <MaxSize, LoopCount>(*this);
    rx::ut_tiny_skiplist_loop_1 <MaxSize, LoopCount>(*this);
    rx::ut_tiny_map_loop_1      <MaxSize, LoopCount>(*this);
    
    rx::ut_tiny_skiplist_loop_2 <MaxSize, LoopCount>(*this);
    rx::ut_tiny_skiplist_loop_2s<MaxSize, LoopCount>(*this);
    rx::ut_tiny_map_loop_2      <MaxSize, LoopCount>(*this);
}

rx_tdd_rtl(htbl_sklist_map_loop,tdd_level_slow)
{
    const uint32_t LoopCount=500,MaxSize=100000;

    rx::ut_tiny_skiplist_loop_1 <MaxSize, LoopCount>(*this);
    rx::ut_tiny_map_loop_1      <MaxSize, LoopCount>(*this);

    rx::ut_tiny_skiplist_loop_2 <MaxSize, LoopCount>(*this);
    rx::ut_tiny_skiplist_loop_2s<MaxSize, LoopCount>(*this);
    rx::ut_tiny_map_loop_2      <MaxSize, LoopCount>(*this);

}

#endif
