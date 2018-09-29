#ifndef _RX_UT_CTOBJ_H_
#define _RX_UT_CTOBJ_H_

#include "../rx_ct_util.h"
#include "../rx_ct_delegate.h"
#include "../rx_tdd.h"

//---------------------------------------------------------
namespace rx_ut
{
    //-----------------------------------------------------
    class cb_test1_class_t
    {
    public:
        uint32_t member_method(uint32_t &p1) { printf("TEST DELEGATE :: call cb_test1_class_t::member_method()\n"); return 10; }
    };
    uint32_t std_cb_func1_t(uint32_t &p1, void *usrdat) { printf("TEST DELEGATE :: call std_cb_func1_t()\n"); return 11; }
    //-----------------------------------------------------
    void test_delegate_base_1(rx_tdd_t &rt)
    {
        //进行标准函数的委托绑定与调用测试
        typedef rx::delegate1_t<uint32_t&> dt;
        uint32_t p;
        dt dgf(std_cb_func1_t);
        rt.tdd_assert(dgf.is_valid());
        rt.tdd_assert(dgf(p) == 11);

        //进行成员函数的委托绑定与调用测试
        cb_test1_class_t cb;
        dt dgc(cb, &cb_test1_class_t::member_method);
        rt.tdd_assert(dgc.is_valid());
        rt.tdd_assert(dgc(p) == 10);
    }

    //-----------------------------------------------------
    class cb_test2_class_t
    {
    public:
        uint32_t member_method(uint32_t &p1, uint32_t &p2) { printf("TEST DELEGATE :: call cb_test2_class_t::member_method()\n"); return 20; }
    };
    uint32_t std_cb_func2_t(uint32_t &p1, uint32_t &p2, void *usrdat) { printf("TEST DELEGATE :: call std_cb_func2_t()\n"); return 21; }
    //-----------------------------------------------------
    void test_delegate_base_2(rx_tdd_t &rt)
    {
        //进行标准函数的委托绑定与调用测试
        typedef rx::delegate2_t<uint32_t&, uint32_t&> dt;
        uint32_t p;
        dt dgf(std_cb_func2_t);
        rt.tdd_assert(dgf.is_valid());
        rt.tdd_assert(dgf(p, p) == 21);

        //进行成员函数的委托绑定与调用测试
        cb_test2_class_t cb;
        dt dgc(cb, &cb_test2_class_t::member_method);
        rt.tdd_assert(dgc.is_valid());
        rt.tdd_assert(dgc(p, p) == 20);
    }

    //-----------------------------------------------------
    class cb_test3_class_t
    {
    public:
        uint32_t member_method(uint32_t &p1, uint32_t &p2, uint32_t &p3) { printf("TEST DELEGATE :: call cb_test3_class_t::member_method()\n"); return 30; }
    };
    uint32_t std_cb_func3_t(uint32_t &p1, uint32_t &p2, uint32_t &p3, void *usrdat) { printf("TEST DELEGATE :: call std_cb_func3_t()\n"); return 31; }
    //-----------------------------------------------------
    void test_delegate_base_3(rx_tdd_t &rt)
    {
        //进行标准函数的委托绑定与调用测试
        typedef rx::delegate3_t<uint32_t&, uint32_t&, uint32_t&> dt;
        uint32_t p;
        dt dgf(std_cb_func3_t);
        rt.tdd_assert(dgf.is_valid());
        rt.tdd_assert(dgf(p, p, p) == 31);

        //进行成员函数的委托绑定与调用测试
        cb_test3_class_t cb;
        dt dgc(cb, &cb_test3_class_t::member_method);
        rt.tdd_assert(dgc.is_valid());
        rt.tdd_assert(dgc(p, p, p) == 30);
    }

    //-----------------------------------------------------
    class cb_test4_class_t
    {
    public:
        uint32_t member_method(uint32_t &p1, uint32_t &p2, uint32_t &p3, uint32_t &p4) { printf("TEST DELEGATE :: call cb_test4_class_t::member_method()\n"); return 40; }
    };
    uint32_t std_cb_func4_t(uint32_t &p1, uint32_t &p2, uint32_t &p3, uint32_t &p4, void *usrdat) { printf("TEST DELEGATE :: call std_cb_func4_t()\n"); return 41; }
    //-----------------------------------------------------
    void test_delegate_base_4(rx_tdd_t &rt)
    {
        //进行标准函数的委托绑定与调用测试
        typedef rx::delegate4_t<uint32_t&, uint32_t&, uint32_t&, uint32_t&> dt;
        uint32_t p;
        dt dgf(std_cb_func4_t);
        rt.tdd_assert(dgf.is_valid());
        rt.tdd_assert(dgf(p,p,p,p) == 41);

        //进行成员函数的委托绑定与调用测试
        cb_test4_class_t cb;
        dt dgc(cb, &cb_test4_class_t::member_method);
        rt.tdd_assert(dgc.is_valid());
        rt.tdd_assert(dgc(p, p, p, p) == 40);
    }
}

//---------------------------------------------------------
class tmp_tst_obj_1
{
public:
    int tmp_v1;
    tmp_tst_obj_1(int v):tmp_v1(v){}
};
//---------------------------------------------------------
class tmp_tst_obj_2:public tmp_tst_obj_1
{
public:
    tmp_tst_obj_2(int v):tmp_tst_obj_1(v){}
};

//---------------------------------------------------------
rx_tdd(tmp_obj)
{
    rx_ut::test_delegate_base_1(*this);
    rx_ut::test_delegate_base_2(*this);
    rx_ut::test_delegate_base_3(*this);
    rx_ut::test_delegate_base_4(*this);

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
