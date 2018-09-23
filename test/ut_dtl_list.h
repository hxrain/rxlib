#ifndef _RX_UT_DTL_LIST_H_
#define _RX_UT_DTL_LIST_H_

#include "../rx_tdd.h"
#include "../rx_dtl_list.h"

//---------------------------------------------------------
//进行容器的遍历,同时对比给定的数组元素内容
template<class ct,class vt>
inline void test_dtl_list_loop_test_1(rx_tdd_t &rt,const ct &s,vt* arr,uint32_t count,bool asc=true,uint32_t asc_offset=0)
{
    rt.tdd_assert(s.size()==count);
    typename ct::iterator I=s.begin();
    if (asc)
    {
        for(uint32_t i=asc_offset;I!=s.end();++i,++I)
            rt.tdd_assert(*I==arr[i]);
    }
    else
    {
        for(uint32_t i=0;I!=s.end();++i,++I)
            rt.tdd_assert(*I==arr[count-i-1]);
    }
}

//---------------------------------------------------------
//使用模板特化进行容器测试数据类型的区分
template<class dt>
struct tst_list_data{};
template<>
struct tst_list_data<int>
{
    static int val(int v){return v;}
    typedef int val_t;
};
template<>
struct tst_list_data<const char*>
{
    static const char* val(uint32_t v)
    {
        switch(v)
        {
        case 0:return "0";
        case 1:return "1";
        case 2:return "2";
        case 3:return "3";
        case 4:return "4";
        }
        return "BAD";
    }
    typedef const char* val_t;
};

//---------------------------------------------------------
//对容器进行栈操作的测试
template<class ct,class dv>
inline void test_dtl_stack_base_1(rx_tdd_t &rt)
{
    typename dv::val_t tst_arr[10];
    uint32_t count=0;
    ct s;
    rt.tdd_assert(s.size()==0);
    rt.tdd_assert(s.begin()==s.end());

    tst_arr[count]=dv::val(1);
    typename ct::iterator I;
    I=s.push_front(tst_arr[count++]);
    rt.tdd_assert(I==s.begin());
    rt.tdd_assert(s.size()==count);
    rt.tdd_assert(s.begin()!=s.end());
    test_dtl_list_loop_test_1(rt,s,tst_arr,count,false);

    tst_arr[count]=dv::val(2);
    I=s.push_front(tst_arr[count++]);
    rt.tdd_assert(I==s.begin());
    rt.tdd_assert(s.size()==count);
    rt.tdd_assert(s.begin()!=s.end());
    test_dtl_list_loop_test_1(rt,s,tst_arr,count,false);

    tst_arr[count]=dv::val(3);
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

//---------------------------------------------------------
//对容器进行栈操作的测试
template<class ct,class dv>
inline void test_dtl_queue_base_1(rx_tdd_t &rt)
{
    typename dv::val_t tst_arr[10];
    uint32_t count=0;
    ct s;
    rt.tdd_assert(s.size()==0);
    rt.tdd_assert(s.begin()==s.end());

    tst_arr[count]=dv::val(1);
    typename ct::iterator I;
    I=s.push_back(tst_arr[count++]);
    rt.tdd_assert(I==s.rbegin());
    rt.tdd_assert(s.size()==count);
    rt.tdd_assert(s.begin()!=s.end());
    test_dtl_list_loop_test_1(rt,s,tst_arr,count,true);

    tst_arr[count]=dv::val(2);
    I=s.push_back(tst_arr[count++]);
    rt.tdd_assert(I==s.rbegin());
    rt.tdd_assert(s.size()==count);
    rt.tdd_assert(s.begin()!=s.end());
    test_dtl_list_loop_test_1(rt,s,tst_arr,count,true);

    tst_arr[count]=dv::val(3);
    I=s.push_back(tst_arr[count++]);
    rt.tdd_assert(I==s.rbegin());
    rt.tdd_assert(s.size()==count);
    rt.tdd_assert(s.begin()!=s.end());
    test_dtl_list_loop_test_1(rt,s,tst_arr,count,true);

    --count;
    rt.tdd_assert(*s.begin()==tst_arr[0]);
    rt.tdd_assert(s.pop_front());
    rt.tdd_assert(s.size()==count);
    rt.tdd_assert(s.begin()!=s.end());
    test_dtl_list_loop_test_1(rt,s,tst_arr,count,true,1);

    --count;
    rt.tdd_assert(*s.begin()==tst_arr[1]);
    rt.tdd_assert(s.pop_front());
    rt.tdd_assert(s.size()==count);
    rt.tdd_assert(s.begin()!=s.end());
    test_dtl_list_loop_test_1(rt,s,tst_arr,count,true,2);

    s.clear();
    rt.tdd_assert(s.size()==0);
    rt.tdd_assert(s.begin()==s.end());
}

//---------------------------------------------------------
rx_tdd(dtl_list_base)
{
    //测试stack_t的栈操作
    test_dtl_stack_base_1<rx::stack_int32_t,tst_list_data<int> >(*this);
    test_dtl_stack_base_1<rx::stack_cstr_t, tst_list_data<const char*> >(*this);

    //测试queue_t的栈操作
    test_dtl_stack_base_1<rx::queue_int32_t,tst_list_data<int> >(*this);
    test_dtl_stack_base_1<rx::queue_cstr_t, tst_list_data<const char*> >(*this);

    //测试list_t的栈操作
    test_dtl_stack_base_1<rx::list_int32_t,tst_list_data<int> >(*this);
    test_dtl_stack_base_1<rx::list_cstr_t, tst_list_data<const char*> >(*this);

    //测试queue_t的队列操作
    test_dtl_queue_base_1<rx::queue_int32_t,tst_list_data<int> >(*this);
    test_dtl_queue_base_1<rx::queue_cstr_t,tst_list_data<const char*> >(*this);

    //测试list_t的队列操作
    test_dtl_queue_base_1<rx::list_int32_t,tst_list_data<int> >(*this);
    test_dtl_queue_base_1<rx::list_cstr_t,tst_list_data<const char*> >(*this);
}

#endif
