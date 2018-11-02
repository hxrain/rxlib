#ifndef _UT_OS_LOCK_H_
#define _UT_OS_LOCK_H_

#include "../rx_cc_macro.h"
#include "../rx_tdd.h"
#include "../rx_dtl_skiplist.h"

namespace rx_ut
{
    //-----------------------------------------------------
    inline void test_skiplist_base_1(rx_tdd_t &rt)
    {
        const uint32_t vals_size=10;
        uint32_t vals[vals_size];
        for(uint32_t i=0;i<vals_size;++i)
            vals[i]=i;

        rx::skiplist_uint32_t sl;
        rt.tdd_assert(sl.begin()==sl.end());
        rt.tdd_assert(sl.rbegin()==sl.end());
        sl.insert((uint32_t)3,vals[3]);
        rt.tdd_assert(*sl.begin()==3);
        rt.tdd_assert(sl.begin()()==3);
        rt.tdd_assert(*sl.rbegin()==3);
        rt.tdd_assert(sl.rbegin()()==3);
        sl.insert((uint32_t)8,vals[8]);
        rt.tdd_assert(*sl.begin()==3);
        rt.tdd_assert(sl.begin()()==3);
        rt.tdd_assert(*sl.rbegin()==8);
        rt.tdd_assert(sl.rbegin()()==8);
        rt.tdd_assert(sl.size()==2);
        sl.insert((uint32_t)5,vals[5]);
        rt.tdd_assert(*sl.begin()==3);
        rt.tdd_assert(sl.begin()()==3);
        rt.tdd_assert(*sl.rbegin()==8);
        rt.tdd_assert(sl.rbegin()()==8);
        sl.insert((uint32_t)7,vals[7]);
        rt.tdd_assert(*sl.begin()==3);
        rt.tdd_assert(sl.begin()()==3);
        rt.tdd_assert(*sl.rbegin()==8);
        rt.tdd_assert(sl.rbegin()()==8);
        sl.insert((uint32_t)2,vals[2]);
        rt.tdd_assert(*sl.begin()==2);
        rt.tdd_assert(sl.begin()()==2);
        rt.tdd_assert(*sl.rbegin()==8);
        rt.tdd_assert(sl.rbegin()()==8);
        rt.tdd_assert(sl.size()==5);

        rx::skiplist_uint32_t::node_t *n=sl.find((uint32_t)3);
        rt.tdd_assert(n&&n->key==3&&n->val==vals[3]);

        n=sl.find((uint32_t)8);
        rt.tdd_assert(n&&n->key==8&&n->val==vals[8]);

        //-------------------------------------------------
        #if RX_RAW_SKIPLIST_DEBUG_PRINT
        sl.print();
        #endif
        //-------------------------------------------------

        n=sl.find((uint32_t)5);
        rt.tdd_assert(n&&n->key==5&&n->val==vals[5]);
        rt.tdd_assert(sl.erase((uint32_t)5));
        rt.tdd_assert(*sl.begin()==2);
        rt.tdd_assert(sl.begin()()==2);
        rt.tdd_assert(*sl.rbegin()==8);
        rt.tdd_assert(sl.rbegin()()==8);
        n=sl.find((uint32_t)5);
        rt.tdd_assert(n==NULL);
        rt.tdd_assert(!sl.erase((uint32_t)1));
        rt.tdd_assert(!sl.insert((uint32_t)3,vals[4]));

        n=sl.find((uint32_t)3);
        rt.tdd_assert(n&&n->key==3&&n->val==vals[3]);
        n=sl.find((uint32_t)2);
        rt.tdd_assert(n&&n->key==2&&n->val==vals[2]);

        sl.clear();

    }
    //-----------------------------------------------------
    inline void test_skiplist_base_1c(rx_tdd_t &rt)
    {
        const uint32_t vals_size=10;
        uint32_t vals[vals_size];
        for(uint32_t i=0;i<vals_size;++i)
            vals[i]=i;

        rx::skiplist_cstr_uint32_t sl;
        rt.tdd_assert(sl.begin()==sl.end());
        rt.tdd_assert(sl.rbegin()==sl.end());

        rt.tdd_assert(sl.insert("3",vals[3])>0);
        rt.tdd_assert(*sl.begin()==3);
        rt.tdd_assert(sl.begin()()=="3");
        rt.tdd_assert(*sl.rbegin()==3);
        rt.tdd_assert(sl.rbegin()()=="3");

        rt.tdd_assert(sl.insert("8",vals[8])>0);
        rt.tdd_assert(*sl.begin()==3);
        rt.tdd_assert(sl.begin()()=="3");
        rt.tdd_assert(*sl.rbegin()==8);
        rt.tdd_assert(sl.rbegin()()=="8");
        rt.tdd_assert(sl.size()==2);

        rt.tdd_assert(sl.insert("5",vals[5])>0);
        rt.tdd_assert(*sl.begin()==3);
        rt.tdd_assert(sl.begin()()=="3");
        rt.tdd_assert(*sl.rbegin()==8);
        rt.tdd_assert(sl.rbegin()()=="8");

        rt.tdd_assert(sl.insert("7",vals[7])>0);
        rt.tdd_assert(*sl.begin()==3);
        rt.tdd_assert(sl.begin()()=="3");
        rt.tdd_assert(*sl.rbegin()==8);
        rt.tdd_assert(sl.rbegin()()=="8");

        rt.tdd_assert(sl.insert("2",vals[2])>0);
        rt.tdd_assert(*sl.begin()==2);
        rt.tdd_assert(sl.begin()()=="2");
        rt.tdd_assert(*sl.rbegin()==8);
        rt.tdd_assert(sl.rbegin()()=="8");
        rt.tdd_assert(sl.size()==5);

        rx::skiplist_cstr_uint32_t::node_t *n=sl.find("3");
        rt.tdd_assert(n&&n->key=="3"&&n->val==vals[3]);

        n=sl.find("8");
        rt.tdd_assert(n&&n->key=="8"&&n->val==vals[8]);

        n=sl.find("5");
        rt.tdd_assert(n&&n->key=="5"&&n->val==vals[5]);
        rt.tdd_assert(sl.erase("5"));
        rt.tdd_assert(*sl.begin()==2);
        rt.tdd_assert(sl.begin()()=="2");
        rt.tdd_assert(*sl.rbegin()==8);
        rt.tdd_assert(sl.rbegin()()=="8");

        n=sl.find("5");
        rt.tdd_assert(n==NULL);
        rt.tdd_assert(!sl.erase("1"));
        rt.tdd_assert(!sl.insert("3",vals[4]));

        n=sl.find("3");
        rt.tdd_assert(n&&n->key=="3"&&n->val==vals[3]);
        n=sl.find("2");
        rt.tdd_assert(n&&n->key=="2"&&n->val==vals[2]);

        sl.clear();

    }
    //-----------------------------------------------------
    inline void test_skiplist_base_1w(rx_tdd_t &rt)
    {
        const uint32_t vals_size=10;
        uint32_t vals[vals_size];
        for(uint32_t i=0;i<vals_size;++i)
            vals[i]=i;

        rx::skiplist_wstr_uint32_t sl;
        rt.tdd_assert(sl.begin()==sl.end());
        rt.tdd_assert(sl.rbegin()==sl.end());

        rt.tdd_assert(sl.insert(L"3",vals[3])>0);
        rt.tdd_assert(*sl.begin()==3);
        rt.tdd_assert(sl.begin()()==L"3");
        rt.tdd_assert(*sl.rbegin()==3);
        rt.tdd_assert(sl.rbegin()()==L"3");

        rt.tdd_assert(sl.insert(L"8",vals[8])>0);
        rt.tdd_assert(*sl.begin()==3);
        rt.tdd_assert(sl.begin()()==L"3");
        rt.tdd_assert(*sl.rbegin()==8);
        rt.tdd_assert(sl.rbegin()()==L"8");
        rt.tdd_assert(sl.size()==2);

        rt.tdd_assert(sl.insert(L"5",vals[5])>0);
        rt.tdd_assert(*sl.begin()==3);
        rt.tdd_assert(sl.begin()()==L"3");
        rt.tdd_assert(*sl.rbegin()==8);
        rt.tdd_assert(sl.rbegin()()==L"8");

        rt.tdd_assert(sl.insert(L"7",vals[7])>0);
        rt.tdd_assert(*sl.begin()==3);
        rt.tdd_assert(sl.begin()()==L"3");
        rt.tdd_assert(*sl.rbegin()==8);
        rt.tdd_assert(sl.rbegin()()==L"8");

        rt.tdd_assert(sl.insert(L"2",vals[2])>0);
        rt.tdd_assert(*sl.begin()==2);
        rt.tdd_assert(sl.begin()()==L"2");
        rt.tdd_assert(*sl.rbegin()==8);
        rt.tdd_assert(sl.rbegin()()==L"8");
        rt.tdd_assert(sl.size()==5);

        rx::skiplist_wstr_uint32_t::node_t *n=sl.find(L"3");
        rt.tdd_assert(n&&n->key==L"3"&&n->val==vals[3]);

        n=sl.find(L"8");
        rt.tdd_assert(n&&n->key==L"8"&&n->val==vals[8]);

        n=sl.find(L"5");
        rt.tdd_assert(n&&n->key==L"5"&&n->val==vals[5]);
        rt.tdd_assert(sl.erase(L"5"));
        rt.tdd_assert(*sl.begin()==2);
        rt.tdd_assert(sl.begin()()==L"2");
        rt.tdd_assert(*sl.rbegin()==8);
        rt.tdd_assert(sl.rbegin()()==L"8");

        n=sl.find(L"5");
        rt.tdd_assert(n==NULL);
        rt.tdd_assert(!sl.erase(L"1"));
        rt.tdd_assert(!sl.insert(L"3",vals[4]));

        n=sl.find(L"3");
        rt.tdd_assert(n&&n->key==L"3"&&n->val==vals[3]);
        n=sl.find(L"2");
        rt.tdd_assert(n&&n->key==L"2"&&n->val==vals[2]);

        sl.clear();

    }

    //-----------------------------------------------------
    inline void test_skipset_base_1(rx_tdd_t &rt)
    {
        rx::skipset_int32_t sl;
        rt.tdd_assert(sl.begin()==sl.end());
        rt.tdd_assert(sl.rbegin()==sl.end());

        rt.tdd_assert(sl.insert(3)>0);
        rt.tdd_assert(*sl.begin()==3);
        rt.tdd_assert(*sl.rbegin()==3);

        rt.tdd_assert(sl.insert(8)>0);
        rt.tdd_assert(*sl.begin()==3);
        rt.tdd_assert(*sl.rbegin()==8);
        rt.tdd_assert(sl.size()==2);

        rt.tdd_assert(sl.insert(5)>0);
        rt.tdd_assert(*sl.begin()==3);
        rt.tdd_assert(*sl.rbegin()==8);

        rt.tdd_assert(sl.insert(7)>0);
        rt.tdd_assert(*sl.begin()==3);
        rt.tdd_assert(*sl.rbegin()==8);

        rt.tdd_assert(sl.insert(2)>0);
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

        rt.tdd_assert(sl.find(3));
        rt.tdd_assert(sl.find(2));

        sl.clear();
    }

    //-----------------------------------------------------
    inline void test_skipset_base_1c(rx_tdd_t &rt)
    {
        rx::skipset_cstr_t sl;
        rt.tdd_assert(sl.begin()==sl.end());
        rt.tdd_assert(sl.rbegin()==sl.end());

        rt.tdd_assert(sl.insert("3")>0);
        rt.tdd_assert(*sl.begin()=="3");
        rt.tdd_assert(*sl.rbegin()=="3");

        rt.tdd_assert(sl.insert("8")>0);
        rt.tdd_assert(*sl.begin()=="3");
        rt.tdd_assert(*sl.rbegin()=="8");
        rt.tdd_assert(sl.size()==2);

        rt.tdd_assert(sl.insert("5")>0);
        rt.tdd_assert(*sl.begin()=="3");
        rt.tdd_assert(*sl.rbegin()=="8");

        rt.tdd_assert(sl.insert("7")>0);
        rt.tdd_assert(*sl.begin()=="3");
        rt.tdd_assert(*sl.rbegin()=="8");

        rt.tdd_assert(sl.insert("2")>0);
        rt.tdd_assert(*sl.begin()=="2");
        rt.tdd_assert(*sl.rbegin()=="8");
        rt.tdd_assert(sl.size()==5);

        rt.tdd_assert(sl.find("3"));
        rt.tdd_assert(sl.find("8"));

        rt.tdd_assert(sl.find("5"));
        rt.tdd_assert(sl.erase("5"));
        rt.tdd_assert(*sl.begin()=="2");
        rt.tdd_assert(*sl.rbegin()=="8");
        rt.tdd_assert(!sl.find("5"));
        rt.tdd_assert(!sl.erase("1"));
        rt.tdd_assert(!sl.insert("3"));

        //-------------------------------------------------

        rt.tdd_assert(sl.find("3"));
        rt.tdd_assert(sl.find("2"));

        sl.clear();
    }

    //-----------------------------------------------------
    inline void test_skipset_base_1w(rx_tdd_t &rt,uint32_t seed)
    {
        rx::skipset_wstr_t sl(seed);
        rt.tdd_assert(sl.begin()==sl.end());
        rt.tdd_assert(sl.rbegin()==sl.end());

        rt.tdd_assert(sl.insert(L"3")>0);
        rt.tdd_assert(*sl.begin()==L"3");
        rt.tdd_assert(*sl.rbegin()==L"3");

        rt.tdd_assert(sl.insert(L"8")>0);
        rt.tdd_assert(*sl.begin()==L"3");
        rt.tdd_assert(*sl.rbegin()==L"8");
        rt.tdd_assert(sl.size()==2);

        rt.tdd_assert(sl.insert(L"5")>0);
        rt.tdd_assert(*sl.begin()==L"3");
        rt.tdd_assert(*sl.rbegin()==L"8");

        rt.tdd_assert(sl.insert(L"7")>0);
        rt.tdd_assert(*sl.begin()==L"3");
        rt.tdd_assert(*sl.rbegin()==L"8");

        rt.tdd_assert(sl.insert(L"2")>0);
        rt.tdd_assert(*sl.begin()==L"2");
        rt.tdd_assert(*sl.rbegin()==L"8");
        rt.tdd_assert(sl.size()==5);

        rt.tdd_assert(sl.find(L"3"));
        rt.tdd_assert(sl.find(L"8"));

        rt.tdd_assert(sl.find(L"5"));
        rt.tdd_assert(sl.erase(L"5"));
        rt.tdd_assert(*sl.begin()==L"2");
        rt.tdd_assert(*sl.rbegin()==L"8");
        rt.tdd_assert(!sl.find(L"5"));
        rt.tdd_assert(!sl.erase(L"1"));
        rt.tdd_assert(!sl.insert(L"3"));

        rt.tdd_assert(sl.find(L"3"));
        rt.tdd_assert(sl.find(L"2"));

        sl.clear();
    }
    inline void test_skipset_base_2w(rx_tdd_t &rt)
    {
        rx::skipset_wstr_t sl;
        rt.tdd_assert(sl.begin()==sl.end());
        rt.tdd_assert(sl.rbegin()==sl.end());

        rt.tdd_assert(sl.insert(L"3")>0);
        rt.tdd_assert(*sl.begin()==L"3");
        rt.tdd_assert(*sl.rbegin()==L"3");

        rt.tdd_assert(sl.insert(L"8")>0);
        rt.tdd_assert(*sl.begin()==L"3");
        rt.tdd_assert(*sl.rbegin()==L"8");
        rt.tdd_assert(sl.size()==2);

        rt.tdd_assert(sl.insert(L"5")>0);
        rt.tdd_assert(*sl.begin()==L"3");
        rt.tdd_assert(*sl.rbegin()==L"8");

        rt.tdd_assert(sl.erase(L"5"));
        rt.tdd_assert(*sl.begin()==L"3");
        rt.tdd_assert(*sl.rbegin()==L"8");
        rt.tdd_assert(!sl.find(L"5"));
        rt.tdd_assert(!sl.erase(L"1"));
        rt.tdd_assert(!sl.insert(L"3"));

        rt.tdd_assert(sl.find(L"3"));
        rt.tdd_assert(!sl.find(L"2"));

        sl.clear();
    }

    //-----------------------------------------------------
    inline void ut_skiplist_bd_base_make()
    {
        rx::skiplist_t<int, int> m;

        tdd_tt(t, "ut_stdmap_base", "make");

        for (int i = 0; i < 10000 * 4000; ++i)
            m.insert(i, i);

        tdd_tt_hit(t, "");
        getchar();
    }
}

rx_tdd(skiplist_base)
{
    //for(uint32_t i=0;i<10000000;++i)
    //    rx::test_skipset_base_1w(*this,i+1537538321);

    //rx_ut::ut_skiplist_bd_base_make();

    rx_ut::test_skipset_base_1w(*this,1537538337);
    rx_ut::test_skipset_base_1w(*this,1537760438);

    rx_ut::test_skipset_base_2w(*this);

    rx_ut::test_skipset_base_1(*this);
    rx_ut::test_skipset_base_1c(*this);
    rx_ut::test_skipset_base_1w(*this,0);

    rx_ut::test_skiplist_base_1(*this);
    rx_ut::test_skiplist_base_1c(*this);
    rx_ut::test_skiplist_base_1w(*this);
}


#endif
