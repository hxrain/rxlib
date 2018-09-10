#ifndef _UT_OS_LOCK_H_
#define _UT_OS_LOCK_H_

#include "../rx_cc_macro.h"
#include "../rx_tdd.h"
#include "../rx_dtl_skiplist.h"

namespace rx
{
    inline void test_skiplist_base_1(rx_tdd_t &rt)
    {
        //tiny_skiplist_rnd_level<> rnd;
        //for(int i=0;i<100;++i)
        //    printf("%d  ",rnd.make());

        const uint32_t vals_size=10;
        uint32_t vals[vals_size];
        for(uint32_t i=0;i<vals_size;++i)
            vals[i]=i;

        tiny_skiplist_uint32_t sl;
        rt.tdd_assert(sl.begin()==sl.end());
        rt.tdd_assert(sl.rbegin()==sl.end());
        sl.insert(3,vals[3]);
        rt.tdd_assert(*sl.begin()==3);
        rt.tdd_assert(sl.begin()()==3);
        rt.tdd_assert(*sl.rbegin()==3);
        rt.tdd_assert(sl.rbegin()()==3);
        sl.insert(8,vals[8]);
        rt.tdd_assert(*sl.begin()==3);
        rt.tdd_assert(sl.begin()()==3);
        rt.tdd_assert(*sl.rbegin()==8);
        rt.tdd_assert(sl.rbegin()()==8);
        rt.tdd_assert(sl.size()==2);
        sl.insert(5,vals[5]);
        rt.tdd_assert(*sl.begin()==3);
        rt.tdd_assert(sl.begin()()==3);
        rt.tdd_assert(*sl.rbegin()==8);
        rt.tdd_assert(sl.rbegin()()==8);
        sl.insert(7,vals[7]);
        rt.tdd_assert(*sl.begin()==3);
        rt.tdd_assert(sl.begin()()==3);
        rt.tdd_assert(*sl.rbegin()==8);
        rt.tdd_assert(sl.rbegin()()==8);
        sl.insert(2,vals[2]);
        rt.tdd_assert(*sl.begin()==2);
        rt.tdd_assert(sl.begin()()==2);
        rt.tdd_assert(*sl.rbegin()==8);
        rt.tdd_assert(sl.rbegin()()==8);
        rt.tdd_assert(sl.size()==5);

        tiny_skiplist_uint32_t::node_t *n=sl.find(3);
        rt.tdd_assert(n&&n->key==3&&n->val==vals[3]);

        n=sl.find(8);
        rt.tdd_assert(n&&n->key==8&&n->val==vals[8]);

        //-------------------------------------------------
        #if RX_RAW_SKIPLIST_DEBUG_PRINT
        sl.print();
        #endif
        //-------------------------------------------------

        n=sl.find(5);
        rt.tdd_assert(n&&n->key==5&&n->val==vals[5]);
        rt.tdd_assert(sl.erase(5));
        rt.tdd_assert(*sl.begin()==2);
        rt.tdd_assert(sl.begin()()==2);
        rt.tdd_assert(*sl.rbegin()==8);
        rt.tdd_assert(sl.rbegin()()==8);
        n=sl.find(5);
        rt.tdd_assert(n==NULL);
        rt.tdd_assert(!sl.erase(1));
        rt.tdd_assert(!sl.insert(3,vals[4]));

        n=sl.find(3);
        rt.tdd_assert(n&&n->key==3&&n->val==vals[3]);
        n=sl.find(2);
        rt.tdd_assert(n&&n->key==2&&n->val==vals[2]);

        sl.clear();

    }

    inline void test_skipset_base_1(rx_tdd_t &rt)
    {
        tiny_skipset_uint32_t sl;
        rt.tdd_assert(sl.begin()==sl.end());
        rt.tdd_assert(sl.rbegin()==sl.end());
        sl.insert(3);
        rt.tdd_assert(*sl.begin()==3);
        rt.tdd_assert(*sl.rbegin()==3);
        sl.insert(8);
        rt.tdd_assert(*sl.begin()==3);
        rt.tdd_assert(*sl.rbegin()==8);
        rt.tdd_assert(sl.size()==2);
        sl.insert(5);
        rt.tdd_assert(*sl.begin()==3);
        rt.tdd_assert(*sl.rbegin()==8);
        sl.insert(7);
        rt.tdd_assert(*sl.begin()==3);
        rt.tdd_assert(*sl.rbegin()==8);
        sl.insert(2);
        rt.tdd_assert(*sl.begin()==2);
        rt.tdd_assert(*sl.rbegin()==8);
        rt.tdd_assert(sl.size()==5);


        rt.tdd_assert(sl.find(3));
        rt.tdd_assert(sl.find(8));

        //-------------------------------------------------
        #if RX_RAW_SKIPLIST_DEBUG_PRINT
        sl.print();
        #endif
        //-------------------------------------------------

        rt.tdd_assert(sl.find(5));
        rt.tdd_assert(sl.erase(5));
        rt.tdd_assert(*sl.begin()==2);
        rt.tdd_assert(*sl.rbegin()==8);
        rt.tdd_assert(!sl.find(5));
        rt.tdd_assert(!sl.erase(1));
        rt.tdd_assert(!sl.insert(3));

        //-------------------------------------------------

        rt.tdd_assert(sl.find(3));
        rt.tdd_assert(sl.find(2));

        sl.clear();
    }
}



rx_tdd(skiplist_base)
{
    rx::test_skiplist_base_1(*this);
    rx::test_skipset_base_1(*this);
}


#endif
