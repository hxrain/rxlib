#ifndef _RX_UT_HASHMAP_H_
#define _RX_UT_HASHMAP_H_

#include "../rx_cc_macro.h"
#include "../rx_tdd.h"
#include "../rx_tdd_tick.h"
#include "../rx_dtl_skiplist.h"
#include "../rx_dtl_hashmap.h"
#include <map>

namespace rx_ut
{
    //-----------------------------------------------------
    inline void test_hashmap_base_1(rx_tdd_t &rt)
    {
        typedef rx::hashmap_t<10,int32_t,int32_t> hashmap_t;
        hashmap_t m;

        rt.tdd_assert(m.size() == 0);

        bool dup;
        rt.tdd_assert(m.insert(1, 0)!= m.end());
        rt.tdd_assert(m.size() == 1);
        rt.tdd_assert(m.insert(1, 2, &dup)!= m.end() && dup);
        rt.tdd_assert(m.size() == 1);

        hashmap_t::iterator I = m.find(1);
        rt.tdd_assert(I != m.end() && *I == 0 && I() == 1);

        rt.tdd_assert(m.insert(2, 1)!= m.end());
        rt.tdd_assert(m.size() == 2);

        I = m.find(2);
        rt.tdd_assert(I != m.end() && *I == 1 && I() == 2);

        rt.tdd_assert(m.insert(3, 1)!= m.end());
        rt.tdd_assert(m.size() == 3);

        I = m.find(3);
        rt.tdd_assert(I != m.end() && *I == 1 && I() == 3);

        uint32_t c = 0;
        for (I = m.begin(); I != m.end(); ++I)
            ++c;
        rt.tdd_assert(c==3);
    }


    //-----------------------------------------------------
    template<uint32_t total,class map_t>
    inline void test_hashmap_base_2rx(rx_tdd_t &rt,map_t &m,const char* msg)
    {
        tdd_tt(t,"hashmap vs stdmap",msg);
        m.clear();
        tdd_tt_hit(t,"clear");

        for (uint32_t i = 0; i < total; ++i)
            m.insert(rx_hash_skeeto_3s(i), i);
        rt.tdd_assert(m.size() == total);
        tdd_tt_hit(t,"insert");

        for (uint32_t i = 0; i < total; ++i)
            m.find(rx_hash_skeeto_3s(i));
        tdd_tt_hit(t,"loop");

        m.clear();
    }
    //-----------------------------------------------------
    template<uint32_t total,class map_t>
    inline void test_hashmap_base_2std(rx_tdd_t &rt,map_t &m,const char* msg)
    {
        tdd_tt(t,"hashmap vs stdmap",msg);
        m.clear();
        tdd_tt_hit(t,"clear");

        for (uint32_t i = 0; i < total; ++i)
            m.insert(std::make_pair(rx_hash_skeeto_3s(i), i));
        rt.tdd_assert(m.size() == total);
        tdd_tt_hit(t,"insert");

        for (uint32_t i = 0; i < total; ++i)
            m.find(rx_hash_skeeto_3s(i));
        tdd_tt_hit(t,"loop");

        m.clear();
    }
}

rx_tdd(skiplist_base)
{
    typedef rx::hashmap_t<100000, uint32_t, uint32_t> ut2_hashmap_t;
    typedef rx::skiplist_uint32_t ut2_skiplist_t;
    typedef std::map<uint32_t, uint32_t> ut2_stdmap_t;

    static ut2_hashmap_t   ut2_hashmap;
    static ut2_skiplist_t  ut2_skiplist;
    static ut2_stdmap_t    ut2_stdmap;

    rx_ut::test_hashmap_base_1(*this);
    rx_ut::test_hashmap_base_2rx  <10000*100>(*this,ut2_hashmap,"hashmap");
    rx_ut::test_hashmap_base_2rx  <10000*100>(*this,ut2_skiplist,"skiplist");
#if RX_USE_TDD_TICK
    rx_ut::test_hashmap_base_2std <10000*100>(*this,ut2_stdmap,"stdmap");
#endif
}

#endif
