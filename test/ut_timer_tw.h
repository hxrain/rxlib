#ifndef _RX_UT_TIMER_TW_H_
#define _RX_UT_TIMER_TW_H_

#include "../rx_tdd.h"
#include "../rx_timer_tw.h"
#include "../rx_hash_rand.h"
#include "../rx_tdd_tick.h"

//---------------------------------------------------------
void ut_timewheel_base_0(rx_tdd_t &rt)
{
    //for (int l2 = 0; l2 < 256; ++l2)
    //{
    //    for (int l1 = 0; l1 < 256; ++l1)
    //    {
    //        for (int l0 = 0; l0 < 256; ++l0)
    //        {
    //            uint32_t t = l2 * 256 * 256 + l1 * 256 + l0;
    //            printf("tick=%6d L2=%3d L1=%3d L0=%3d\n",t,l2,l1,l0);
    //        }
    //    }
    //}

    rx::tw::wheel_group_t<4> wa;
    uint8_t wi, si;

    wa.set_abs_pos(1);
    rt.tdd_assert(wa.slot_idx(0)==1);
    rt.tdd_assert(wa.calc_curr_tick(true) == 1);
    wa.calc_rel_pos(1, wi, si);
    rt.tdd_assert(wi == 0 && si == 2);
    wa.calc_rel_pos(100, wi, si);
    rt.tdd_assert(wi == 0 && si == 101);
    wa.calc_rel_pos(255, wi, si);
    rt.tdd_assert(wi == 0 && si == 0);
    wa.calc_rel_pos(256, wi, si);
    rt.tdd_assert(wi == 1 && si == 1);
    wa.calc_rel_pos(257, wi, si);
    rt.tdd_assert(wi == 1 && si == 1);
    wa.calc_rel_pos(511, wi, si);
    rt.tdd_assert(wi == 1 && si == 2);
    wa.calc_rel_pos(512, wi, si);
    rt.tdd_assert(wi == 1 && si == 2);
    wa.calc_rel_pos(513, wi, si);
    rt.tdd_assert(wi == 1 && si == 2);

    wa.set_abs_pos(2);
    rt.tdd_assert(wa.slot_idx(0) == 2);
    rt.tdd_assert(wa.calc_curr_tick(true) == 2);
    wa.calc_rel_pos(1, wi, si);
    rt.tdd_assert(wi == 0 && si == 3);
    wa.calc_rel_pos(100, wi, si);
    rt.tdd_assert(wi == 0 && si == 102);
    wa.calc_rel_pos(255, wi, si);
    rt.tdd_assert(wi == 0 && si == 1);

    wa.set_abs_pos(255);
    rt.tdd_assert(wa.slot_idx(0) == 255);
    rt.tdd_assert(wa.calc_curr_tick(true) == 255);
    wa.calc_rel_pos(1, wi, si);
    rt.tdd_assert(wi == 0 && si == 0);
    wa.calc_rel_pos(100, wi, si);
    rt.tdd_assert(wi == 0 && si == 99);
    wa.calc_rel_pos(255, wi, si);
    rt.tdd_assert(wi == 0 && si == 254);

    wa.set_abs_pos(256);
    rt.tdd_assert(wa.slot_idx(1) == 1 && wa.slot_idx(0) == 0);
    rt.tdd_assert(wa.calc_curr_tick(true) == 256);
    wa.calc_rel_pos(1, wi, si);
    rt.tdd_assert(wi == 0 && si == 1);
    wa.calc_rel_pos(100, wi, si);
    rt.tdd_assert(wi == 0 && si == 100);
    wa.calc_rel_pos(255, wi, si);
    rt.tdd_assert(wi == 0 && si == 255);

    wa.set_abs_pos(257);
    rt.tdd_assert(wa.slot_idx(1) == 1 && wa.slot_idx(0) == 1);
    rt.tdd_assert(wa.calc_curr_tick(true) == 257);
    wa.calc_rel_pos(256, wi, si);
    rt.tdd_assert(wi == 1 && si == 2);
    wa.calc_rel_pos(257, wi, si);
    rt.tdd_assert(wi == 1 && si == 2);
    wa.calc_rel_pos(511, wi, si);
    rt.tdd_assert(wi == 1 && si == 3);
    wa.calc_rel_pos(512, wi, si);
    rt.tdd_assert(wi == 1 && si == 3);
    wa.calc_rel_pos(513, wi, si);
    rt.tdd_assert(wi == 1 && si == 3);


    wa.set_abs_pos(511);
    rt.tdd_assert(wa.slot_idx(1) == 1 && wa.slot_idx(0) == 255);
    rt.tdd_assert(wa.calc_curr_tick(true) == 511);
    wa.calc_rel_pos(256, wi, si);
    rt.tdd_assert(wi == 1 && si == 2);
    wa.calc_rel_pos(257, wi, si);
    rt.tdd_assert(wi == 1 && si == 3);

    wa.set_abs_pos(512);
    rt.tdd_assert(wa.slot_idx(1) == 2 && wa.slot_idx(0) == 0);
    rt.tdd_assert(wa.calc_curr_tick(true) == 512);

    wa.set_abs_pos(513);
    rt.tdd_assert(wa.slot_idx(1) == 2 && wa.slot_idx(0) == 1);
    rt.tdd_assert(wa.calc_curr_tick(true) == 513);

    wa.set_abs_pos(767);
    rt.tdd_assert(wa.slot_idx(1) == 2 && wa.slot_idx(0) == 255);
    rt.tdd_assert(wa.calc_curr_tick(true) == 767);

    wa.set_abs_pos(768);
    rt.tdd_assert(wa.slot_idx(1) == 3 && wa.slot_idx(0) == 0);
    rt.tdd_assert(wa.calc_curr_tick(true) == 768);

    wa.set_abs_pos(1023);
    rt.tdd_assert(wa.slot_idx(1) == 3 && wa.slot_idx(0) == 255);
    rt.tdd_assert(wa.calc_curr_tick(true) == 1023);

    wa.set_abs_pos(1024);
    rt.tdd_assert(wa.slot_idx(1) == 4 && wa.slot_idx(0) == 0);
    rt.tdd_assert(wa.calc_curr_tick(true) == 1024);

    wa.set_abs_pos(1025);
    rt.tdd_assert(wa.slot_idx(1) == 4 && wa.slot_idx(0) == 1);
    rt.tdd_assert(wa.calc_curr_tick(true) == 1025);

    wa.set_abs_pos(65535);
    rt.tdd_assert(wa.slot_idx(1) == 255 && wa.slot_idx(0) == 255);
    rt.tdd_assert(wa.calc_curr_tick(true) == 65535);

    wa.set_abs_pos(65536);
    rt.tdd_assert(wa.slot_idx(2) == 1 && wa.slot_idx(1) == 0 && wa.slot_idx(0) == 0);
    rt.tdd_assert(wa.calc_curr_tick(true) == 65536);

    wa.set_abs_pos(65536 + 255);
    rt.tdd_assert(wa.slot_idx(2) == 1 && wa.slot_idx(1) == 0 && wa.slot_idx(0) == 255);
    rt.tdd_assert(wa.calc_curr_tick(true) == 65536 + 255);

    wa.set_abs_pos(65536 + 65535);
    rt.tdd_assert(wa.slot_idx(2) == 1 && wa.slot_idx(1) == 255 && wa.slot_idx(0) == 255);
    rt.tdd_assert(wa.calc_curr_tick(true) == 65536 + 65535);

    wa.set_abs_pos(65536 + 65536);
    rt.tdd_assert(wa.slot_idx(2) == 2 && wa.slot_idx(1) == 0 && wa.slot_idx(0) == 0);
    rt.tdd_assert(wa.calc_curr_tick(true) == 65536 + 65536);

    wa.set_abs_pos(16711679);
    rt.tdd_assert(wa.slot_idx(2) == 254 && wa.slot_idx(1) == 255 && wa.slot_idx(0) == 255);
    rt.tdd_assert(wa.calc_curr_tick(true) == 16711679);

    wa.set_abs_pos(16711680);
    rt.tdd_assert(wa.slot_idx(2) == 255 && wa.slot_idx(1) == 0 && wa.slot_idx(0) == 0);
    rt.tdd_assert(wa.calc_curr_tick(true) == 16711680);

    wa.set_abs_pos(16711681);
    rt.tdd_assert(wa.slot_idx(2) == 255 && wa.slot_idx(1) == 0 && wa.slot_idx(0) == 1);
    rt.tdd_assert(wa.calc_curr_tick(true) == 16711681);
}
//---------------------------------------------------------
//tw定时器事件处理器
class ut_tw_handler_t
{
public:
    uint64_t    hits;
    ut_tw_handler_t():hits(0){}
    void on_timer(size_t handle, uint32_t event_code, uint32_t repeat)
    {
        ++hits;
    }
};
#define evt_obj(obj) obj,cf_ptr(ut_tw_handler_t,on_timer)
//---------------------------------------------------------
template<uint32_t wheels>
inline void ut_timer_tw_base1(rx_tdd_t &rt,uint32_t cycle,uint32_t rep=1)
{
    tdd_tt(ut_timer_tw_base1, "timewheels","base1 : wheels(%d)cycle(%d)", wheels, cycle);
    //定义事件处理器对象
    ut_tw_handler_t tr;
    //定义tw定时器管理器
    rx::tw_timer_mgr_t<wheels> w;

    //初始化定时器管理器
    rt.tdd_assert(w.wheels_init());
    rt.tdd_assert(tr.hits == 0);

    //创建一个定时器,使用tr对象进行事件触发的处理,定时周期是cycle,事件码0,重复次数rep
    size_t h = w.timer_insert(evt_obj(tr), cycle, 0, rep);
    rt.tdd_assert(h > 0);

    //模拟当前时间即将到达触发时间之前的时间点
    uint32_t lc = cycle*rep-2;
    rt.tdd_assert(w.wheels_step(lc) == 0);
    rt.tdd_assert(tr.hits == 0);
    rt.tdd_assert(w.wheels_step() == 0);
    rt.tdd_assert(tr.hits == 0);
    //时间流逝到达触发点,应被触发
    rt.tdd_assert(w.wheels_step() == 1);
    rt.tdd_assert(tr.hits == 1);
    //时间流逝超过了触发点,不应该被触发
    rt.tdd_assert(w.wheels_step() == 0);
    rt.tdd_assert(tr.hits == 1);
}
//---------------------------------------------------------
template<uint32_t wheels>
inline void ut_timer_tw_base2(rx_tdd_t &rt, uint32_t cycle, uint32_t rep = 1)
{
    tdd_tt(ut_timer_tw_base1, "timewheels", "base2 : wheels(%d)cycle(%d),rep(%d)", wheels, cycle, rep);
    //定义事件处理器对象
    ut_tw_handler_t tr;
    //定义tw定时器管理器
    rx::tw_timer_mgr_t<wheels> w;

    //初始化定时器管理器
    rt.tdd_assert(w.wheels_init());
    rt.tdd_assert(tr.hits == 0);

    //创建一个定时器,使用tr对象进行事件触发的处理,定时周期是cycle,事件码0,重复次数rep
    size_t h = w.timer_insert(evt_obj(tr), cycle, 0, rep);
    rt.tdd_assert(h > 0);

    uint32_t tol = cycle*rep;
    w.wheels_step(tol);
    rt.tdd_assert(tr.hits == rep);
}
//---------------------------------------------------------
template<uint32_t wheels>
inline void ut_timer_tw_base4(rx_tdd_t &rt, uint32_t cycle_limit, uint32_t hit_limit, uint32_t timer_count)
{
    tdd_tt(ut_timer_tw_base4, "timewheels", "base test4:wheels(%d)cycle(%d),hits(%d),timers(%d)", wheels, cycle_limit, hit_limit, timer_count);
    ut_tw_handler_t tr;
    rx::tw_timer_mgr_t<wheels> w;
    rx::rand_skeeto_bsa_t rnd;
    uint32_t need_hits = 0;

    size_t h;
    rt.tdd_assert(w.wheels_init());
    uint32_t err_count = 0;

    uint32_t inv = rnd.get(cycle_limit);
    uint32_t hc = rnd.get(hit_limit);

    need_hits += hc;
    h = w.timer_insert(evt_obj(tr), inv, 0, hc);
    if (h == 0)
        ++err_count;

    uint32_t hits = 0;
    uint32_t lc = 0;
    uint32_t mkc = timer_count;
    while (hits < need_hits)
    {
        ++lc;
        if (mkc)
        {
            --mkc;
            inv = rnd.get(cycle_limit, 1);
            hc = rnd.get(hit_limit, 1);

            need_hits += hc;
            h = w.timer_insert(evt_obj(tr), inv, 0, hc);
            if (h == 0)
                ++err_count;
        }
        uint32_t r = w.wheels_step();
        if (r)
            hits += r;

        rx_assert_if(w.timer_count() == 0, hits == need_hits);
    }
    rt.tdd_assert(need_hits == tr.hits);
    tdd_tt_msg(ut_timer_tw_base4, "total_hits=%d,used_time=%d,err=%d", need_hits, lc, err_count);
}

template<uint32_t wheels>
void tw_timer_test_B1(rx_tdd_t &rt)
{
    ut_timer_tw_base1<wheels>(rt, 3);

    ut_timer_tw_base1<wheels>(rt, 255);

    ut_timer_tw_base1<wheels>(rt, 256);

    ut_timer_tw_base1<wheels>(rt, 257);

    ut_timer_tw_base1<wheels>(rt, 511);

    ut_timer_tw_base1<wheels>(rt, 512);

    ut_timer_tw_base1<wheels>(rt, 513);

    ut_timer_tw_base1<wheels>(rt, 256 * 256 - 1);

    ut_timer_tw_base1<wheels>(rt, 256 * 256);

    ut_timer_tw_base1<wheels>(rt, 256 * 256 + 1);

    ut_timer_tw_base1<wheels>(rt, 256 * 256 + 256+1);

    ut_timer_tw_base1<wheels>(rt, 256 * 256 + 256+168);
}

template<uint32_t wheels>
void tw_timer_test_B4(rx_tdd_t &rt)
{
    ut_timer_tw_base4<wheels>(rt, 3, 10, 20);
    ut_timer_tw_base4<wheels>(rt, 10, 10, 198);
    ut_timer_tw_base4<wheels>(rt, 20, 10, 1000);
}

rx_tdd(ut_timer_base)
{
    tw_timer_test_B1<4>(*this);
    tw_timer_test_B1<3>(*this);
    tw_timer_test_B1<2>(*this);
    tw_timer_test_B1<1>(*this);
    ut_timer_tw_base2<2>(*this, 257, 2);
    ut_timer_tw_base2<2>(*this, 256, 2);
    ut_timer_tw_base2<2>(*this, 255, 2);
    ut_timer_tw_base2<2>(*this, 30, 2);
    ut_timer_tw_base2<1>(*this, 257, 2);
    ut_timer_tw_base2<1>(*this, 256, 2);
    ut_timer_tw_base2<1>(*this, 255, 2);
    ut_timer_tw_base2<1>(*this, 30, 2);
    ut_timer_tw_base1<2>(*this, 257);
    ut_timer_tw_base1<2>(*this, 256);
    ut_timer_tw_base1<2>(*this, 255);
    ut_timer_tw_base1<2>(*this, 30);
    ut_timer_tw_base1<2>(*this, 3);
    ut_timer_tw_base2<1>(*this, 3, 2);
    ut_timer_tw_base1<1>(*this, 257);
    ut_timer_tw_base1<1>(*this, 256);
    ut_timer_tw_base1<1>(*this, 255);
    ut_timer_tw_base1<1>(*this, 3);
    ut_timewheel_base_0(*this);
}

rx_tdd_rtl(ut_timer_base,tdd_level_slow)
{
    for (uint32_t i = 3; i<256 * 256 * 2 + 1024; i+=37)
    {
        ut_timer_tw_base1<1>(*this, i);
        ut_timer_tw_base1<2>(*this, i);
        ut_timer_tw_base1<3>(*this, i);
        ut_timer_tw_base1<4>(*this, i);
    }
    tw_timer_test_B4<1>(*this);
    tw_timer_test_B4<2>(*this);
    tw_timer_test_B4<3>(*this);
    tw_timer_test_B4<4>(*this);
}
#endif