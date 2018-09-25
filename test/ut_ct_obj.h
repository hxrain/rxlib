#ifndef _RX_UT_CTOBJ_H_
#define _RX_UT_CTOBJ_H_

#include "../rx_ct_util.h"
#include "../rx_tdd.h"

class tmp_tst_obj_1
{
public:
    int tmp_v1;
    tmp_tst_obj_1(int v):tmp_v1(v){}
};

class tmp_tst_obj_2:public tmp_tst_obj_1
{
public:
    tmp_tst_obj_2(int v):tmp_tst_obj_1(v){}
};

namespace rx_ut
{
    class cb_test_class_t
    {
    public:
        uint32_t member_method(void *obj, void *p1, void *p2, void *usrdat) { printf("TEST DELEGATE :: call cb_test_class_t::member_method()\n"); return 20; }
    };
    uint32_t std_cb_func_t(void *obj, void *p1, void *p2, void *usrdat) { printf("TEST DELEGATE :: call std_cb_func_t()\n"); return 10; }

    void test_delegate_base_1(rx_tdd_t &rt)
    {
        rx::delegate_def_t dg;

        //进行标准函数的委托绑定与调用测试
        dg.bind(std_cb_func_t);
        rt.tdd_assert(dg.is_valid());
        rt.tdd_assert(dg(NULL,0,0,0)==10);

        //进行成员函数的委托绑定与调用测试
        cb_test_class_t cb;
        dg.bind(cb,&cb_test_class_t::member_method);
        rt.tdd_assert(dg.is_valid());
        rt.tdd_assert(dg(NULL, 0, 0, 0) == 20);
    }

    void test_delegate_base_2(rx_tdd_t &rt)
    {
        //进行标准函数的委托绑定与调用测试
        rx::delegate_def_t dg(std_cb_func_t);
        rt.tdd_assert(dg.is_valid());
        rt.tdd_assert(dg(NULL, 0, 0, 0) == 10);

        //进行成员函数的委托绑定与调用测试
        cb_test_class_t cb;
        rx::delegate_def_t dgc(cb, &cb_test_class_t::member_method);
        rt.tdd_assert(dgc.is_valid());
        rt.tdd_assert(dgc(NULL, 0, 0, 0) == 20);
    }
}

rx_tdd(tmp_obj)
{
    rx_ut::test_delegate_base_1(*this);
    rx_ut::test_delegate_base_2(*this);

    uint8_t buff_obj[sizeof(tmp_tst_obj_2)*5];
    tmp_tst_obj_2 *o2=(tmp_tst_obj_2*)buff_obj;
    rx::ct::OC(o2,2);
    assert(o2->tmp_v1==2);
    rx::ct::OD(o2);

    rx::ct::AC(o2,5,3);
    assert(o2->tmp_v1==3);
    assert((++o2)->tmp_v1==3);
    rx::ct::AD(--o2,5);

    assert(rx::Min(5,8,3)==3);
    assert(rx::Max(5,8,3)==8);
    //assert(0);

    assert(rx::factorial(0) == 1);
    assert(rx::factorial(1) == 1);
    assert(rx::factorial(2) == 2);
    assert(rx::factorial(3) == 6);
    assert(rx::factorial(4) == 24);
    assert(rx::factorial(5) == 120);
}



#endif // _RX_UT_CTOBJ_H_
