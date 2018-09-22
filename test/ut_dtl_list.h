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
//使用模板特化进行容器push动作的区分
template<class cntr_t,bool asc>
struct tst_list_push{};
template<class cntr_t>
struct tst_list_push<cntr_t,true>
{
    template<class DT>
    static typename cntr_t::iterator push(cntr_t &s,const DT& val){return s.push_back(val);}
    enum {is_back=true};
};
template<class cntr_t>
struct tst_list_push<cntr_t,false>
{
    template<class DT>
    static typename cntr_t::iterator push(cntr_t &s,const DT& val){return s.push_front(val);}
    enum {is_back=false};
};
//---------------------------------------------------------
//使用模板特化进行容器pop动作的区分
template<class cntr_t,bool front>
struct tst_list_pop{};
template<class cntr_t>
struct tst_list_pop<cntr_t,true>
{
    static bool pop(cntr_t &s){return s.pop_front();}
    enum {is_front=true};
};
template<class cntr_t>
struct tst_list_pop<cntr_t,false>
{
    static bool pop(cntr_t &s){return s.pop_back();}
    enum {is_front=false};
};
//---------------------------------------------------------
//使用模板特化进行容器last动作的区分
template<class cntr_t,bool front>
struct tst_list_last{};
template<class cntr_t>
struct tst_list_last<cntr_t,true>
{
    static typename cntr_t::iterator last(cntr_t &s){return s.begin();}
    enum {is_front=true};
};
template<class cntr_t>
struct tst_list_last<cntr_t,false>
{
    static typename cntr_t::iterator last(cntr_t &s){return s.rbegin();}
    enum {is_front=false};
};
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
//对容器进行基础操作的测试,整数类型
template<class ct,class dv,bool pushback,bool popfront,bool isback>
inline void test_dtl_list_base_1(rx_tdd_t &rt)
{
    typedef tst_list_push<ct,pushback> pt;
    typedef tst_list_pop<ct,popfront> ot;
    typedef tst_list_last<ct,isback> lt;

    typename dv::val_t tst_arr[10];
    uint32_t count=0;
    ct s;
    rt.tdd_assert(s.size()==0);
    rt.tdd_assert(s.begin()==s.end());

    tst_arr[count]=dv::val(1);
    typename ct::iterator I;
    I=pt::push(s,tst_arr[count++]);
    rt.tdd_assert(I==lt::last(s));
    rt.tdd_assert(s.size()==count);
    rt.tdd_assert(s.begin()!=s.end());
    test_dtl_list_loop_test_1(rt,s,tst_arr,count,pt::is_back);

    tst_arr[count]=dv::val(2);
    I=pt::push(s,tst_arr[count++]);
    rt.tdd_assert(I==lt::last(s));
    rt.tdd_assert(s.size()==count);
    rt.tdd_assert(s.begin()!=s.end());
    test_dtl_list_loop_test_1(rt,s,tst_arr,count,pt::is_back);

    tst_arr[count]=dv::val(3);
    I=pt::push(s,tst_arr[count++]);
    rt.tdd_assert(I==lt::last(s));
    rt.tdd_assert(s.size()==count);
    rt.tdd_assert(s.begin()!=s.end());
    test_dtl_list_loop_test_1(rt,s,tst_arr,count,pt::is_back);
    /*
    --count;
    rt.tdd_assert(*lt::last(s)==tst_arr[count]);
    rt.tdd_assert(ot::pop(s));
    rt.tdd_assert(s.size()==count);
    rt.tdd_assert(s.begin()!=s.end());
    test_dtl_list_loop_test_1(rt,s,tst_arr,count,!ot::is_front);

    --count;
    rt.tdd_assert(*lt::last(s)==tst_arr[count]);
    rt.tdd_assert(ot::pop(s));
    rt.tdd_assert(s.size()==count);
    rt.tdd_assert(s.begin()!=s.end());
    test_dtl_list_loop_test_1(rt,s,tst_arr,count,!ot::is_front);
    */
    s.clear();
    rt.tdd_assert(s.size()==0);
    rt.tdd_assert(s.begin()==s.end());
}

rx_tdd(dtl_list_base)
{
    test_dtl_list_base_1<rx::stack_int32_t,tst_list_data<int>  ,false,true,true >(*this);//LIFO
    test_dtl_list_base_1<rx::stack_cstr_t, tst_list_data<const char*>,false,true,true >(*this);//LIFO

    test_dtl_list_base_1<rx::queue_int32_t,tst_list_data<int>  ,false,true,true >(*this);//LIFO
    test_dtl_list_base_1<rx::queue_cstr_t, tst_list_data<const char*>,false,true,true >(*this);//LIFO

    test_dtl_list_base_1<rx::queue_int32_t,tst_list_data<int>  ,true,true,false >(*this);//FIFO
    test_dtl_list_base_1<rx::queue_cstr_t,tst_list_data<const char*> ,true,true,false >(*this);//FIFO

}

#endif
