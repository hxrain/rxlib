#ifndef _RX_UT_DTL_LIST_H_
#define _RX_UT_DTL_LIST_H_

#include "../rx_tdd.h"
#include "../../rx_dtl_list.h"

template<class ct,class vt>
inline void test_dtl_list_loop_test_1(rx_tdd_t &rt,const ct &s,vt* arr,uint32_t count,bool asc=true)
{
    rt.tdd_assert(s.size()==count);
    ct::iterator I=s.begin();
    if (asc)
    {
        for(uint32_t i=0;I!=s.end();++i,++I)
            rt.tdd_assert(*I==arr[i]);
    }
    else
    {
        for(uint32_t i=0;I!=s.end();++i,++I)
            rt.tdd_assert(*I==arr[count-i-1]);
    }
}

template<class ct>
inline void test_dtl_stack_base_1(rx_tdd_t &rt)
{
    int tst_arr[10];
    uint32_t count=0;
    ct s;
    rt.tdd_assert(s.size()==0);
    rt.tdd_assert(s.begin()==s.end());
    
    tst_arr[count]=1;
    ct::iterator I=s.push_front(tst_arr[count++]);
    rt.tdd_assert(I==s.begin());
    rt.tdd_assert(s.size()==count);
    rt.tdd_assert(s.begin()!=s.end());
    test_dtl_list_loop_test_1(rt,s,tst_arr,count,false);

    tst_arr[count]=2;
    I=s.push_front(tst_arr[count++]);
    rt.tdd_assert(I==s.begin());
    rt.tdd_assert(s.size()==count);
    rt.tdd_assert(s.begin()!=s.end());
    test_dtl_list_loop_test_1(rt,s,tst_arr,count,false);

    tst_arr[count]=3;
    I=s.push_front(tst_arr[count++]);
    rt.tdd_assert(I==s.begin());
    rt.tdd_assert(s.size()==count);
    rt.tdd_assert(s.begin()!=s.end());
    test_dtl_list_loop_test_1(rt,s,tst_arr,count,false);

    --count;
    rt.tdd_assert(tst_arr[count]==*s.begin());
    rt.tdd_assert(s.pop_front());
    rt.tdd_assert(s.size()==count);
    rt.tdd_assert(s.begin()!=s.end());
    test_dtl_list_loop_test_1(rt,s,tst_arr,count,false);

    --count;
    rt.tdd_assert(tst_arr[count]==*s.begin());
    rt.tdd_assert(s.pop_front());
    rt.tdd_assert(s.size()==count);
    rt.tdd_assert(s.begin()!=s.end());
    test_dtl_list_loop_test_1(rt,s,tst_arr,count,false);

    s.clear();
    rt.tdd_assert(s.size()==0);
    rt.tdd_assert(s.begin()==s.end());
}

template<class ct>
inline void test_dtl_stack_str_1(rx_tdd_t &rt)
{
    char* tst_arr[10];
    uint32_t count=0;
    ct s;
    rt.tdd_assert(s.size()==0);
    rt.tdd_assert(s.begin()==s.end());

    tst_arr[count]="1";
    ct::iterator I=s.push_front(tst_arr[count++]);
    rt.tdd_assert(I==s.begin());
    rt.tdd_assert(s.size()==count);
    rt.tdd_assert(s.begin()!=s.end());
    test_dtl_list_loop_test_1(rt,s,tst_arr,count,false);

    tst_arr[count]="2";
    I=s.push_front(tst_arr[count++]);
    rt.tdd_assert(I==s.begin());
    rt.tdd_assert(s.size()==count);
    rt.tdd_assert(s.begin()!=s.end());
    test_dtl_list_loop_test_1(rt,s,tst_arr,count,false);

    tst_arr[count]="3";
    I=s.push_front(tst_arr[count++]);
    rt.tdd_assert(I==s.begin());
    rt.tdd_assert(s.size()==count);
    rt.tdd_assert(s.begin()!=s.end());
    test_dtl_list_loop_test_1(rt,s,tst_arr,count,false);

    --count;
    rt.tdd_assert(*s.begin()==tst_arr[count]);
    rt.tdd_assert(s.pop_front());
    rt.tdd_assert(s.size()==count);
    rt.tdd_assert(s.begin()!=s.end());
    test_dtl_list_loop_test_1(rt,s,tst_arr,count,false);

    --count;
    rt.tdd_assert(*s.begin()==tst_arr[count]);
    rt.tdd_assert(s.pop_front());
    rt.tdd_assert(s.size()==count);
    rt.tdd_assert(s.begin()!=s.end());
    test_dtl_list_loop_test_1(rt,s,tst_arr,count,false);

    s.clear();
    rt.tdd_assert(s.size()==0);
    rt.tdd_assert(s.begin()==s.end());
}


rx_tdd(dtl_list_base)
{
    test_dtl_stack_base_1<rx::stack_int32_t>(*this);
    test_dtl_stack_str_1<rx::stack_cstr_t>(*this);
}

#endif