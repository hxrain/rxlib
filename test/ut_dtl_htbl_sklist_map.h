#ifndef _UT_DTL_HASHTBL_SKIPLIST_MAP_H_
#define _UT_DTL_HASHTBL_SKIPLIST_MAP_H_

#include "../rx_cc_macro.h"
#include "../rx_tdd.h"
#include "../rx_raw_skiplist.h"
#include "../rx_dtl_skiplist.h"
#include "../rx_raw_hashtbl.h"
#include "../rx_dtl_hashtbl_tiny.h"

//本文件进行tiny_hashtbl/tiny_skiplist/std::map的性能对比
//分为三个方面:初始构造;查询;销毁.
namespace rx
{
    //-----------------------------------------------------
    template<uint32_t MaxSize, uint32_t LoopCount>
    inline void ut_tiny_hashtbl_loop_1(rx_tdd_t &rt)
    {

    }

    //-----------------------------------------------------
    template<uint32_t MaxSize, uint32_t LoopCount>
    inline void ut_tiny_skiplist_loop_1(rx_tdd_t &rt)
    {

    }

    //-----------------------------------------------------
    template<uint32_t MaxSize, uint32_t LoopCount>
    inline void ut_tiny_map_loop_1(rx_tdd_t &rt)
    {

    }
}

rx_tdd(htbl_sklist_map_loop)
{
    rx::ut_tiny_hashtbl_loop_1  <100,50000>(*this);
    rx::ut_tiny_skiplist_loop_1 <100, 50000>(*this);
    rx::ut_tiny_map_loop_1      <100, 50000>(*this);

    rx::ut_tiny_hashtbl_loop_1  <1000, 50000>(*this);
    rx::ut_tiny_skiplist_loop_1 <1000, 50000>(*this);
    rx::ut_tiny_map_loop_1      <1000, 50000>(*this);

}

rx_tdd_rtl(htbl_sklist_map_loop, tdd_level_slow)
{
    rx::ut_tiny_hashtbl_loop_1  <10000, 5000000>(*this);
    rx::ut_tiny_skiplist_loop_1 <10000, 5000000>(*this);
    rx::ut_tiny_map_loop_1      <10000, 5000000>(*this);

    rx::ut_tiny_hashtbl_loop_1  <100000, 5000000>(*this);
    rx::ut_tiny_skiplist_loop_1 <100000, 5000000>(*this);
    rx::ut_tiny_map_loop_1      <100000, 5000000>(*this);

}



#endif