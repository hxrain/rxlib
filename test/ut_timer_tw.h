#ifndef _RX_UT_TIMER_TW_H_
#define _RX_UT_TIMER_TW_H_

#include "../rx_tdd.h"
#include "../rx_timer_tw.h"
#include "../rx_hash_rand.h"

const bool ut_show_timer_tw_debug = false;

//tw定时器事件处理器
class ut_timer_tw_result
{
public:
    uint64_t    hits;
    ut_timer_tw_result():hits(0){}
    void on_timer(size_t handle, uint32_t event_code, uint32_t repeat)
    {
        ++hits;
    }
};
#define evt_obj(obj) obj,cf_ptr(ut_timer_tw_result,on_timer)

template<uint32_t wheels>
inline void ut_timer_tw_base1(rx_tdd_t &rt,uint32_t cycle, uint64_t curr_time = 0)
{
    //定义事件处理器对象
    ut_timer_tw_result tr;
    //定义tw定时器管理器
    rx::tw_timer_mgr_t<wheels> w;

    //初始化定时器管理器
    rt.tdd_assert(w.wheels_init(curr_time));
    rt.tdd_assert(tr.hits == 0);

    //创建一个定时器,使用tr对象进行事件触发的处理,定时周期是cycle,事件码0,重复次数1
    size_t h = w.timer_insert(evt_obj(tr), cycle, 0, 1);
    rt.tdd_assert(h > 0);

    //模拟当前时间即将到达触发时间之前的时间点
    curr_time += cycle-2;
    //给出时间流逝跨度,不应该被触发
    rt.tdd_assert(w.wheels_step(++curr_time) == 0);
    rt.tdd_assert(tr.hits == 0);
    //时间流逝到达触发点,应被触发
    rt.tdd_assert(w.wheels_step(++curr_time) == 1);
    rt.tdd_assert(tr.hits == 1);
    //时间流逝超过了触发点,不应该被触发
    rt.tdd_assert(w.wheels_step(++curr_time) == 0);
    rt.tdd_assert(tr.hits == 1);
}

template<uint32_t wheels>
inline void ut_timer_tw_base4(rx_tdd_t &rt,uint32_t inv_limit,uint32_t hit_limit,uint32_t timer_count)
{
    ut_timer_tw_result tr;
    rx::tw_timer_mgr_t<wheels> w;
    rx::rand_skeeto_bsa_t rnd;
    uint64_t curr_time = 0;
    uint32_t need_hits=0;

    tdd_print(ut_show_timer_tw_debug,"wheels=%d,inv_limit=%d,hit_limit=%d,timer_count=%d:", wheels, inv_limit, hit_limit, timer_count);

    size_t h;
    rt.tdd_assert(w.wheels_init(curr_time));
    uint32_t err_count=0;

    uint32_t inv=rnd.get(inv_limit);
    uint32_t hc=rnd.get(hit_limit);

    need_hits+=hc;
    h=w.timer_insert(evt_obj(tr), inv, 0, hc);
    if (h==0)
        ++err_count;

    uint32_t hits=0;
    uint32_t lc=0;
    uint32_t mkc=timer_count;
    while(hits<need_hits)
    {
        ++lc;
        if (mkc)
        {
            --mkc;
            inv=rnd.get(inv_limit,1);
            hc=rnd.get(hit_limit,1);

            need_hits+=hc;
            h=w.timer_insert(evt_obj(tr), inv, 0, hc);
            if (h==0)
                ++err_count;
        }
        uint32_t r = w.wheels_step(++curr_time);
        if (r)
            hits += r;

        rx_assert_if(w.timer_count() == 0, hits==need_hits);
    }
    rt.tdd_assert(need_hits==tr.hits);
    tdd_print(ut_show_timer_tw_debug, "total_hits=%d,loop_count=%d,err=%d\n", need_hits,lc,err_count);
}

template<uint32_t wheels>
void tw_timer_test_B1(rx_tdd_t &rt)
{
    ut_timer_tw_base1<wheels>(rt, 2);

    ut_timer_tw_base1<wheels>(rt, 255);

    ut_timer_tw_base1<wheels>(rt, 256);

    ut_timer_tw_base1<wheels>(rt, 257);

    ut_timer_tw_base1<wheels>(rt, 511);

    ut_timer_tw_base1<wheels>(rt, 512);

    ut_timer_tw_base1<wheels>(rt, 513);

    ut_timer_tw_base1<wheels>(rt, 256 * 256 - 1);

    ut_timer_tw_base1<wheels>(rt, 256 * 256);

    ut_timer_tw_base1<wheels>(rt, 256 * 256 + 1);
}
rx_tdd(ut_timer_base)
{
    tw_timer_test_B1<1>(*this);
    tw_timer_test_B1<2>(*this);
    tw_timer_test_B1<3>(*this);
    tw_timer_test_B1<4>(*this);
    //ut_timer_tw_base1<3>(*this, 256 * 256 + 256+1);
    //ut_timer_tw_base1<3>(*this, 256 * 256 + 256+168);
}

rx_tdd_rtl(ut_timer_base,tdd_level_slow)
{
    for (uint32_t i = 2; i<256 * 256; ++i)
    {
        ut_timer_tw_base1<1>(*this, i);
        ut_timer_tw_base1<2>(*this, i);
        ut_timer_tw_base1<3>(*this, i);
        ut_timer_tw_base1<4>(*this, i);
    }

    for (uint32_t i = 256 * 256; i< 256 * 256 + 1024; ++i)
    {
        ut_timer_tw_base1<1>(*this, i);
        ut_timer_tw_base1<2>(*this, i);
        ut_timer_tw_base1<3>(*this, i);
        ut_timer_tw_base1<4>(*this, i);
    }
}
#endif