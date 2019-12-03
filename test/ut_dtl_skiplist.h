#ifndef _UT_OS_LOCK_H_
#define _UT_OS_LOCK_H_

#define RX_UT_TEST_SKIPSET      1
#define RX_UT_TEST_SKIPLIST     1

#include "../rx_cc_macro.h"
#include "../rx_tdd.h"
#include "../rx_tdd_tick.h"

#if RX_UT_TEST_SKIPLIST
    #include "../rx_dtl_skiplist.h"
#endif

#if RX_UT_TEST_SKIPSET
    #include "../rx_dtl_skipset.h"
#endif
namespace rx_ut
{
#if RX_UT_TEST_SKIPLIST
    //-----------------------------------------------------
    inline void test_skiplist_base_1(rx_tdd_t &rt)
    {
        const uint32_t vals_size=10;
        uint32_t vals[vals_size];
        for(uint32_t i=0;i<vals_size;++i)
            vals[i]=i;

        rx::skiplist_ut sl;
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

        rx::skiplist_ut::iterator I = sl.find((uint32_t)3);
        rt.tdd_assert(I != sl.end() && I() == 3 && *I == vals[3]);

        I = sl.find((uint32_t)8);
        rt.tdd_assert(I != sl.end() && I() == 8 && *I == vals[8]);

        //-------------------------------------------------

        I=sl.find((uint32_t)5);
        rt.tdd_assert(I != sl.end() && I() == 5 && *I == vals[5]);
        rt.tdd_assert(sl.erase((uint32_t)5));
        rt.tdd_assert(*sl.begin()==2);
        rt.tdd_assert(sl.begin()()==2);
        rt.tdd_assert(*sl.rbegin()==8);
        rt.tdd_assert(sl.rbegin()()==8);
        I=sl.find((uint32_t)5);
        rt.tdd_assert(I==sl.end());
        rt.tdd_assert(!sl.erase((uint32_t)1));
        bool dup = false;
        rt.tdd_assert(sl.insert((uint32_t)3, vals[4], &dup) != sl.end() && dup);

        I=sl.find((uint32_t)3);
        rt.tdd_assert(I != sl.end() && I() == 3 && *I == vals[3]);
        I=sl.find((uint32_t)2);
        rt.tdd_assert(I != sl.end() && I() == 2 && *I == vals[2]);

        sl.clear();
    }

    //-----------------------------------------------------
    inline void test_skiplist_base_1c(rx_tdd_t &rt)
    {
        const uint32_t vals_size=10;
        uint32_t vals[vals_size];
        for(uint32_t i=0;i<vals_size;++i)
            vals[i]=i;

        rx::skiplist_ct sl;
        rt.tdd_assert(sl.begin()==sl.end());
        rt.tdd_assert(sl.rbegin()==sl.end());

        rt.tdd_assert(sl.insert("3",vals[3])!=sl.end());
        rt.tdd_assert(*sl.begin()==3);
        rt.tdd_assert(sl.begin()()=="3");
        rt.tdd_assert(*sl.rbegin()==3);
        rt.tdd_assert(sl.rbegin()()=="3");

        rt.tdd_assert(sl.insert("8",vals[8])!=sl.end());
        rt.tdd_assert(*sl.begin()==3);
        rt.tdd_assert(sl.begin()()=="3");
        rt.tdd_assert(*sl.rbegin()==8);
        rt.tdd_assert(sl.rbegin()()=="8");
        rt.tdd_assert(sl.size()==2);

        rt.tdd_assert(sl.insert("5",vals[5]) != sl.end());
        rt.tdd_assert(*sl.begin()==3);
        rt.tdd_assert(sl.begin()()=="3");
        rt.tdd_assert(*sl.rbegin()==8);
        rt.tdd_assert(sl.rbegin()()=="8");

        rt.tdd_assert(sl.insert("7",vals[7]) != sl.end());
        rt.tdd_assert(*sl.begin()==3);
        rt.tdd_assert(sl.begin()()=="3");
        rt.tdd_assert(*sl.rbegin()==8);
        rt.tdd_assert(sl.rbegin()()=="8");

        rt.tdd_assert(sl.insert("2",vals[2]) != sl.end());
        rt.tdd_assert(*sl.begin()==2);
        rt.tdd_assert(sl.begin()()=="2");
        rt.tdd_assert(*sl.rbegin()==8);
        rt.tdd_assert(sl.rbegin()()=="8");
        rt.tdd_assert(sl.size()==5);

        rx::skiplist_ct::iterator I=sl.find("3");
        rt.tdd_assert(I != sl.end() && I() == "3" && *I == vals[3]);

        I=sl.find("8");
        rt.tdd_assert(I != sl.end() && I() == "8" && *I == vals[8]);

        I=sl.find("5");
        rt.tdd_assert(I != sl.end() && I() == "5" && *I == vals[5]);
        rt.tdd_assert(sl.erase("5"));
        rt.tdd_assert(*sl.begin()==2);
        rt.tdd_assert(sl.begin()()=="2");
        rt.tdd_assert(*sl.rbegin()==8);
        rt.tdd_assert(sl.rbegin()()=="8");

        I=sl.find("5");
        rt.tdd_assert(I==NULL&&I==sl.end());
        rt.tdd_assert(!sl.erase("1"));
        bool dup = false;
        rt.tdd_assert(sl.insert("3",vals[4],&dup)!=sl.end()&&dup);

        I=sl.find("3");
        rt.tdd_assert(I != sl.end() && I() == "3" && *I == vals[3]);
        I=sl.find("2");
        rt.tdd_assert(I != sl.end() && I() == "2" && *I == vals[2]);

        sl.clear();
    }

    //-----------------------------------------------------
    inline void test_skiplist_base_1w(rx_tdd_t &rt)
    {
        const uint32_t vals_size=10;
        uint32_t vals[vals_size];
        for(uint32_t i=0;i<vals_size;++i)
            vals[i]=i;

        rx::skiplist_wt sl;
        rt.tdd_assert(sl.begin()==sl.end());
        rt.tdd_assert(sl.rbegin()==sl.end());

        rt.tdd_assert(sl.insert(L"3",vals[3])!=sl.end());
        rt.tdd_assert(*sl.begin()==3);
        rt.tdd_assert(sl.begin()()==L"3");
        rt.tdd_assert(*sl.rbegin()==3);
        rt.tdd_assert(sl.rbegin()()==L"3");

        rt.tdd_assert(sl.insert(L"8",vals[8]) != sl.end());
        rt.tdd_assert(*sl.begin()==3);
        rt.tdd_assert(sl.begin()()==L"3");
        rt.tdd_assert(*sl.rbegin()==8);
        rt.tdd_assert(sl.rbegin()()==L"8");
        rt.tdd_assert(sl.size()==2);

        rt.tdd_assert(sl.insert(L"5",vals[5]) != sl.end());
        rt.tdd_assert(*sl.begin()==3);
        rt.tdd_assert(sl.begin()()==L"3");
        rt.tdd_assert(*sl.rbegin()==8);
        rt.tdd_assert(sl.rbegin()()==L"8");

        rt.tdd_assert(sl.insert(L"7",vals[7]) != sl.end());
        rt.tdd_assert(*sl.begin()==3);
        rt.tdd_assert(sl.begin()()==L"3");
        rt.tdd_assert(*sl.rbegin()==8);
        rt.tdd_assert(sl.rbegin()()==L"8");

        rt.tdd_assert(sl.insert(L"2",vals[2]) != sl.end());
        rt.tdd_assert(*sl.begin()==2);
        rt.tdd_assert(sl.begin()()==L"2");
        rt.tdd_assert(*sl.rbegin()==8);
        rt.tdd_assert(sl.rbegin()()==L"8");
        rt.tdd_assert(sl.size()==5);

        rx::skiplist_wt::iterator I=sl.find(L"3");
        rt.tdd_assert(I != sl.end() && I() == L"3" && *I == vals[3]);

        I=sl.find(L"8");
        rt.tdd_assert(I != sl.end() && I() == L"8" && *I == vals[8]);

        I=sl.find(L"5");
        rt.tdd_assert(I != sl.end() && I() == L"5" && *I == vals[5]);
        rt.tdd_assert(sl.erase(L"5"));
        rt.tdd_assert(*sl.begin()==2);
        rt.tdd_assert(sl.begin()()==L"2");
        rt.tdd_assert(*sl.rbegin()==8);
        rt.tdd_assert(sl.rbegin()()==L"8");

        I=sl.find(L"5");
        rt.tdd_assert(I==NULL);
        rt.tdd_assert(!sl.erase(L"1"));
        bool dup = false;
        rt.tdd_assert(sl.insert(L"3",vals[4],&dup)!=sl.end()&&dup);

        I=sl.find(L"3");
        rt.tdd_assert(I != sl.end() && I() == L"3" && *I == vals[3]);
        I=sl.find(L"2");
        rt.tdd_assert(I != sl.end() && I() == L"2" && *I == vals[2]);

        sl.clear();
    }
    //-----------------------------------------------------
    inline void ut_skiplist_bd_base_make()
    {
        rx::skiplist_t<int, int> m;

        tdd_tt(t, "ut_stdmap_base", "make");

        for (int i = 0; i < 10000 * 4000; ++i)
            m.insert(i, i);

        tdd_tt_msg(t, "");
        getchar();
    }
#endif

#if RX_UT_TEST_SKIPSET
    //-----------------------------------------------------
    inline void test_skipset_base_1(rx_tdd_t &rt)
    {
        rx::skipset_it sl;
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
        rx::skipset_ct sl;
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
        rx::skipset_wt sl(seed);
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
        rx::skipset_wt sl;
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
#endif

}

rx_tdd(skiplist_base)
{
    //for(uint32_t i=0;i<10000000;++i)
    //    rx::test_skipset_base_1w(*this,i+1537538321);

    //rx_ut::ut_skiplist_bd_base_make();
#if RX_UT_TEST_SKIPSET
    rx_ut::test_skipset_base_1w(*this,1537538337);
    rx_ut::test_skipset_base_1w(*this,1537760438);

    rx_ut::test_skipset_base_2w(*this);

    rx_ut::test_skipset_base_1(*this);
    rx_ut::test_skipset_base_1c(*this);
    rx_ut::test_skipset_base_1w(*this,0);

#endif

#if RX_UT_TEST_SKIPLIST
    rx_ut::test_skiplist_base_1(*this);
    rx_ut::test_skiplist_base_1c(*this);
    rx_ut::test_skiplist_base_1w(*this);
#endif
}


#endif
