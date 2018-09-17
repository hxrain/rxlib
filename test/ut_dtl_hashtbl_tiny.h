#ifndef _UT_RX_DTL_HASHTBL_TINY_H_
#define _UT_RX_DTL_HASHTBL_TINY_H_

#include "../rx_cc_macro.h"
#include "../rx_tdd.h"
#include "../rx_dtl_hashtbl_raw.h"
#include "../rx_dtl_hashtbl_tiny.h"

namespace rx
{
    //-----------------------------------------------------
    const uint32_t tmp_hashtbl_tiny_msize=7;

    //-----------------------------------------------------
    //¹şÏ£¼¯ºÏ»ù´¡²âÊÔ
    template<class cntr_t>
    inline void raw_tinyhashset_base_loop1(rx_tdd_t &rt,const cntr_t& s)
    {
        for(typename cntr_t::iterator i= s.begin();i!=s.end();++i)
            rt.tdd_assert(*i!=0);
    }
    inline void raw_tinyhashset_base_1(rx_tdd_t &rt)
    {
        typedef tiny_hashset_t<tmp_hashtbl_tiny_msize,int32_t> cntr_t;
        cntr_t s;
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

        raw_tinyhashset_base_loop1(rt,s);

        rt.tdd_assert(s.find(4));
        rt.tdd_assert(s.find(2));
        rt.tdd_assert(s.erase(2));
        rt.tdd_assert(s.size() == 3);
        rt.tdd_assert(s.collision() == 0);
        rt.tdd_assert(!s.find(2));
    }

    //-----------------------------------------------------
    //¹şÏ£±í»ù´¡²âÊÔ
    template<class cntr_t>
    inline void raw_tinyhashtbl_base_loop1(rx_tdd_t &rt,const cntr_t& s)
    {
        for(typename cntr_t::iterator i= s.begin();i!=s.end();++i)
            rt.tdd_assert(*i!=0&&(i()+1)==*i);
    }

    inline void raw_tinyhashtbl_base_1(rx_tdd_t &rt)
    {
        typedef tiny_hashtbl_t<tmp_hashtbl_tiny_msize,int,int> cntr_t;
        cntr_t s;
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

        raw_tinyhashtbl_base_loop1(rt,s);

        rt.tdd_assert(s.find(4)!=s.end());
        rt.tdd_assert(s.find(2)!=s.end())
            ;
        rt.tdd_assert(s.erase(2));
        rt.tdd_assert(s.size() == 3);
        rt.tdd_assert(s.collision() == 0);
        rt.tdd_assert(s.find(2)==s.end());
    }

    //-----------------------------------------------------
    //¹şÏ£Á´±í»ù´¡²âÊÔ
    inline void raw_tinyhashlist_base_1(rx_tdd_t &rt)
    {
        typedef tiny_hashlist_t<tmp_hashtbl_tiny_msize,int,int> cntr_t;
        cntr_t s;

        rt.tdd_assert(s.capacity() == tmp_hashtbl_tiny_msize);
        rt.tdd_assert(s.size() == 0);
        rt.tdd_assert(s.collision() == 0);
        rt.tdd_assert(s.begin()==s.end());

        rt.tdd_assert(s.insert(1,2));
        rt.tdd_assert(s.size() == 1);
        rt.tdd_assert(s.collision() == 0);
        rt.tdd_assert(s.begin()()==1);
        rt.tdd_assert(s.rbegin()()==1);
        rt.tdd_assert(s.begin()!=s.end());

        rt.tdd_assert(s.insert(1,2));
        rt.tdd_assert(s.size() == 1);
        rt.tdd_assert(s.collision() == 0);

        rt.tdd_assert(s.insert(2,3));
        rt.tdd_assert(s.size() == 2);
        rt.tdd_assert(s.collision() == 0);
        rt.tdd_assert(s.begin()()==1);
        rt.tdd_assert(s.rbegin()()==2);
        rt.tdd_assert(s.begin()!=s.end());

        rt.tdd_assert(s.insert(3,4));
        rt.tdd_assert(s.size() == 3);
        rt.tdd_assert(s.collision() == 0);
        rt.tdd_assert(s.begin()()==1);
        rt.tdd_assert(s.rbegin()()==3);
        rt.tdd_assert(s.begin()!=s.end());

        rt.tdd_assert(s.insert(4,5));
        rt.tdd_assert(s.size() == 4);
        rt.tdd_assert(s.collision() == 0);
        rt.tdd_assert(s.begin()()==1);
        rt.tdd_assert(s.rbegin()()==4);
        rt.tdd_assert(s.begin()!=s.end());

        raw_tinyhashtbl_base_loop1(rt,s);

        rt.tdd_assert(s.find(4)!=s.end());
        rt.tdd_assert(s.find(2)!=s.end())
            ;
        rt.tdd_assert(s.erase(2));
        rt.tdd_assert(s.begin()()==1);
        rt.tdd_assert(s.rbegin()()==4);
        rt.tdd_assert(s.begin()!=s.end());

        rt.tdd_assert(s.size() == 3);
        rt.tdd_assert(s.collision() == 0);
        rt.tdd_assert(s.find(2)==s.end());

        rt.tdd_assert(s.erase(4));
        rt.tdd_assert(s.begin()()==1);
        rt.tdd_assert(s.rbegin()()==3);
        rt.tdd_assert(s.begin()!=s.end());

        rt.tdd_assert(s.size() == 2);
        rt.tdd_assert(s.collision() == 0);
        rt.tdd_assert(s.find(4)==s.end());

        rt.tdd_assert(s.erase(1));
        rt.tdd_assert(s.begin()()==3);
        rt.tdd_assert(s.rbegin()()==3);
        rt.tdd_assert(s.begin()!=s.end());

        rt.tdd_assert(s.size() == 1);
        rt.tdd_assert(s.collision() == 0);
        rt.tdd_assert(s.find(1)==s.end());

        rt.tdd_assert(s.erase(3));
        rt.tdd_assert(s.begin()==s.end());

        rt.tdd_assert(s.size() == 0);
        rt.tdd_assert(s.collision() == 0);
        rt.tdd_assert(s.find(3)==s.end());
    }
}

rx_tdd(hashtbl_tiny_base)
{
    rx::raw_tinyhashset_base_1(*this);
    rx::raw_tinyhashtbl_base_1(*this);
    rx::raw_tinyhashlist_base_1(*this);
}


#endif
