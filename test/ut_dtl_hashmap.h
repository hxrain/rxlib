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


    typedef rx::hashmap_t<1000000, int32_t, int32_t> ut2_hashmap_t;
    //typedef rx::skiplist_int32_t ut2_hashmap_t;
    //typedef std::map<int, int> ut2_hashmap_t;
    ut2_hashmap_t ut2_m;
    //-----------------------------------------------------
    inline void test_hashmap_base_2(rx_tdd_t &rt)
    {
        ut2_m.clear();

        const int32_t total = 10000 * 4000;
        for (int32_t i = 0; i < total; ++i)
        {
            //ut2_m.insert(std::make_pair(rx_hash_skeeto_3s((uint32_t)i), i));
            ut2_m.insert((int)rx_hash_skeeto_3s((uint32_t)i), i);
        }
        rt.tdd_assert(ut2_m.size() == total);

        getchar();
    }
}

rx_tdd(skiplist_base)
{
    //rx_ut::test_hashmap_base_1(*this);
    rx_ut::test_hashmap_base_2(*this);
    rx_ut::test_hashmap_base_2(*this);
}

#endif
