#ifndef _UT_RX_DTL_HASHTBL_TINY_H_
#define _UT_RX_DTL_HASHTBL_TINY_H_

#include "../rx_cc_macro.h"
#include "../rx_tdd.h"
#include "../rx_raw_hashtbl.h"
#include "../rx_dtl_hashtbl_tiny.h"

namespace rx
{
    const uint32_t tmp_hashtbl_tiny_msize=7;
    inline void raw_hashtbl_tinyset_base_loop1(rx_tdd_t &rt,const tiny_hashset_uint32_t<tmp_hashtbl_tiny_msize>& s)
    {
        tiny_hashset_uint32_t<tmp_hashtbl_tiny_msize>::iterator i= s.begin();
        for(;i!=s.end();++i)
            rt.tdd_assert(*i!=0);
    }
    inline void raw_hashtbl_tinyset_base_1(rx_tdd_t &rt)
    {
        tiny_hashset_uint32_t<tmp_hashtbl_tiny_msize> s;
        rt.tdd_assert(s.capacity() == tmp_hashtbl_tiny_msize);
        rt.tdd_assert(s.size() == 0);
        rt.tdd_assert(s.collision() == 0);

        rt.tdd_assert(s.insert(1));
        rt.tdd_assert(s.size() == 1);
        rt.tdd_assert(s.collision() == 0);

        rt.tdd_assert(s.insert(1));
        rt.tdd_assert(s.size() == 1);
        rt.tdd_assert(s.collision() == 0);

        rt.tdd_assert(s.insert(2));
        rt.tdd_assert(s.size() == 2);
        rt.tdd_assert(s.collision() == 0);

        rt.tdd_assert(s.insert(3));
        rt.tdd_assert(s.size() == 3);
        rt.tdd_assert(s.collision() == 0);

        rt.tdd_assert(s.insert(4));
        rt.tdd_assert(s.size() == 4);
        rt.tdd_assert(s.collision() == 0);

        raw_hashtbl_tinyset_base_loop1(rt,s);

        rt.tdd_assert(s.find(4));
        rt.tdd_assert(s.find(2))
            ;
        rt.tdd_assert(s.erase(2));
        rt.tdd_assert(s.size() == 3);
        rt.tdd_assert(s.collision() == 0);
        rt.tdd_assert(!s.find(2));
    }

    inline void raw_hashtbl_tinytbl_base_loop1(rx_tdd_t &rt,const tiny_hashtbl_uint32_t<tmp_hashtbl_tiny_msize>& s)
    {
        tiny_hashtbl_uint32_t<tmp_hashtbl_tiny_msize>::iterator i= s.begin();
        for(;i!=s.end();++i)
            rt.tdd_assert(*i!=0&&i()+1==*i);
    }

    inline void raw_hashtbl_tinytbl_base_1(rx_tdd_t &rt)
    {
        tiny_hashtbl_uint32_t<tmp_hashtbl_tiny_msize> s;
        rt.tdd_assert(s.capacity() == tmp_hashtbl_tiny_msize);
        rt.tdd_assert(s.size() == 0);
        rt.tdd_assert(s.collision() == 0);

        rt.tdd_assert(s.insert(1,2));
        rt.tdd_assert(s.size() == 1);
        rt.tdd_assert(s.collision() == 0);

        rt.tdd_assert(s.insert(1,2));
        rt.tdd_assert(s.size() == 1);
        rt.tdd_assert(s.collision() == 0);

        rt.tdd_assert(s.insert(2,3));
        rt.tdd_assert(s.size() == 2);
        rt.tdd_assert(s.collision() == 0);

        rt.tdd_assert(s.insert(3,4));
        rt.tdd_assert(s.size() == 3);
        rt.tdd_assert(s.collision() == 0);

        rt.tdd_assert(s.insert(4,5));
        rt.tdd_assert(s.size() == 4);
        rt.tdd_assert(s.collision() == 0);

        raw_hashtbl_tinytbl_base_loop1(rt,s);

        rt.tdd_assert(s.find(4)!=s.end());
        rt.tdd_assert(s.find(2)!=s.end())
            ;
        rt.tdd_assert(s.erase(2));
        rt.tdd_assert(s.size() == 3);
        rt.tdd_assert(s.collision() == 0);
        rt.tdd_assert(s.find(2)==s.end());
    }
}

rx_tdd(hashtbl_tiny_base)
{
    rx::raw_hashtbl_tinyset_base_1(*this);
    rx::raw_hashtbl_tinytbl_base_1(*this);
}


#endif
