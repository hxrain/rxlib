#ifndef _UT_OS_LOCK_H_
#define _UT_OS_LOCK_H_

#include "../rx_cc_macro.h"
#include "../rx_tdd.h"
#include "../rx_dtl_skiplist.h"

namespace rx
{
    inline void test_skiplist_base_1(rx_tdd_t &rt)
    {
        const uint32_t vals_size=10;
        uint32_t vals[vals_size];
        for(uint32_t i=0;i<vals_size;++i)
            vals[i]=i;

        raw_skiplist_t sl;
        sl.insert(3,vals[3]);
        sl.insert(8,vals[8]);
        sl.insert(5,vals[5]);
        sl.insert(7,vals[7]);
        sl.insert(2,vals[2]);

        sl_node_t *n=sl.find(3);
        rt.tdd_assert(n&&n->key==3&&n->obj==vals[3]);

        n=sl.find(8);
        rt.tdd_assert(n&&n->key==8&&n->obj==vals[8]);

        sl.print();

        n=sl.find(5);
        rt.tdd_assert(n&&n->key==5&&n->obj==vals[5]);
        sl.remove(5);
        n=sl.find(5);
        rt.tdd_assert(n==NULL);
        sl.remove(1);
        sl.uninit();
    }
}

inline void test_skiplist_base_2(rx_tdd_t &rt)
{

}

rx_tdd(skiplist_base)
{
    rx::test_skiplist_base_1(*this);
    test_skiplist_base_2(*this);
}


#endif
