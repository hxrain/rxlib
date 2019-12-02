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
inline void ut_timer_tw_base1(rx_tdd_t &rt,uint32_t cycle)
{
    //定义事件处理器对象
    ut_timer_tw_result tr;
    //定义tw定时器管理器
    rx::timing_wheel_t<wheels> w;

    //定义初始时间点并初始化定时器管理器
    uint64_t curr_time = 0;
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
inline void ut_timer_tw_base2(rx_tdd_t &rt)
{
    ut_timer_tw_result tr;
    rx::timing_wheel_t<wheels> w;
    uint64_t curr_time = 0;
    size_t h;
    rt.tdd_assert(w.wheels_init(++curr_time));
    rt.tdd_assert(tr.hits == 0);

    h = w.timer_insert(evt_obj(tr), 255, 0, 1);
    rt.tdd_assert(h > 0);
    curr_time += 253;
    rt.tdd_assert(w.wheels_step(++curr_time) == 0);
    rt.tdd_assert(tr.hits == 0);
    rt.tdd_assert(w.wheels_step(++curr_time) == 1);
    rt.tdd_assert(tr.hits == 1);
    rt.tdd_assert(w.wheels_step(++curr_time) == 0);
    rt.tdd_assert(tr.hits == 1);

    h = w.timer_insert(evt_obj(tr), 256, 0, 1);
    rt.tdd_assert(h > 0);
    curr_time += 254;
    rt.tdd_assert(w.wheels_step(++curr_time) == 0);
    rt.tdd_assert(tr.hits == 1);
    rt.tdd_assert(w.wheels_step(++curr_time) == 1);
    rt.tdd_assert(tr.hits == 2);
    rt.tdd_assert(w.wheels_step(++curr_time) == 0);
    rt.tdd_assert(tr.hits == 2);

    h = w.timer_insert(evt_obj(tr), 257, 0, 1);
    rt.tdd_assert(h > 0);
    curr_time += 255;
    rt.tdd_assert(w.wheels_step(++curr_time) == 0);
    rt.tdd_assert(tr.hits == 2);
    rt.tdd_assert(w.wheels_step(++curr_time) == 1);
    rt.tdd_assert(tr.hits == 3);
    rt.tdd_assert(w.wheels_step(++curr_time) == 0);
    rt.tdd_assert(tr.hits == 3);
}


template<uint32_t wheels>
inline void ut_timer_tw_base3(rx_tdd_t &rt)
{
    ut_timer_tw_result tr;
    rx::timing_wheel_t<wheels> w;
    uint64_t curr_time = 0;
    size_t h;
    rt.tdd_assert(w.wheels_init(++curr_time));
    rt.tdd_assert(w.wheels_step(++curr_time) == 0);
    rt.tdd_assert(tr.hits == 0);
    h = w.timer_insert(evt_obj(tr), 1, 0, 1);
    rt.tdd_assert(h > 0);
    rt.tdd_assert(w.wheels_step(++curr_time) == 1);
    rt.tdd_assert(tr.hits==1);
    rt.tdd_assert(w.wheels_step(++curr_time) == 0);
    rt.tdd_assert(tr.hits == 1);

    h = w.timer_insert(evt_obj(tr), 1, 0, 1);
    rt.tdd_assert(h > 0);
    rt.tdd_assert(w.timer_update(h));
    rt.tdd_assert(w.timer_remove(h));
    rt.tdd_assert(w.wheels_step(++curr_time) == 0);
    rt.tdd_assert(tr.hits == 1);

    h = w.timer_insert(evt_obj(tr), 2, 0, 1);
    rt.tdd_assert(h > 0);
    rt.tdd_assert(w.wheels_step(++curr_time) == 0);
    rt.tdd_assert(tr.hits == 1);
    rt.tdd_assert(w.wheels_step(++curr_time) == 1);
    rt.tdd_assert(tr.hits == 2);

    h = w.timer_insert(evt_obj(tr), 255, 0, 1);
    rt.tdd_assert(h > 0);
    curr_time += 254;
    rt.tdd_assert(w.wheels_step(++curr_time) == 1);
    rt.tdd_assert(tr.hits == 3);
    rt.tdd_assert(w.wheels_step(++curr_time) == 0);
    rt.tdd_assert(tr.hits == 3);

    h = w.timer_insert(evt_obj(tr), 257, 0, 1);
    rt.tdd_assert(h > 0);
    curr_time += 256;
    rt.tdd_assert(w.wheels_step(++curr_time) == 1);
    rt.tdd_assert(tr.hits == 4);
    rt.tdd_assert(w.wheels_step(++curr_time) == 0);
    rt.tdd_assert(tr.hits == 4);

    h = w.timer_insert(evt_obj(tr), 2, 0, 1);
    rt.tdd_assert(h > 0);
    rt.tdd_assert(w.wheels_step(++curr_time) == 0);
    rt.tdd_assert(tr.hits == 4);
    rt.tdd_assert(w.timer_update(h));
    rt.tdd_assert(w.wheels_step(++curr_time) == 0);
    rt.tdd_assert(tr.hits == 4);
    rt.tdd_assert(w.wheels_step(++curr_time) == 1);
    rt.tdd_assert(tr.hits == 5);


    h = w.timer_insert(evt_obj(tr), 256, 0, 1);
    rt.tdd_assert(h > 0);
    curr_time += 254;
    rt.tdd_assert(w.wheels_step(++curr_time) == 0);
    rt.tdd_assert(tr.hits == 5);
    rt.tdd_assert(w.wheels_step(++curr_time) == 1);
    rt.tdd_assert(tr.hits == 6);
    rt.tdd_assert(w.wheels_step(++curr_time) == 0);
    rt.tdd_assert(tr.hits == 6);
}

template<uint32_t wheels>
inline void ut_timer_tw_base4(rx_tdd_t &rt,uint32_t inv_limit,uint32_t hit_limit,uint32_t timer_count)
{
    ut_timer_tw_result tr;
    rx::timing_wheel_t<wheels> w;
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

rx_tdd(ut_timer_base)
{
    ut_timer_tw_base1<3>(*this, 256 * 256 + 256+1);
    ut_timer_tw_base1<3>(*this, 256 * 256 + 424);

    ut_timer_tw_base2<1>(*this);

    ut_timer_tw_base1<1>(*this,2);
    ut_timer_tw_base1<2>(*this,2);
    ut_timer_tw_base1<3>(*this,2);
    ut_timer_tw_base1<4>(*this,2);

    ut_timer_tw_base1<1>(*this,257);
    ut_timer_tw_base1<2>(*this,257);
    ut_timer_tw_base1<3>(*this,257);
    ut_timer_tw_base1<4>(*this,257);

    ut_timer_tw_base1<1>(*this,256);
    ut_timer_tw_base1<2>(*this,256);
    ut_timer_tw_base1<3>(*this,256);
    ut_timer_tw_base1<4>(*this,256);

    ut_timer_tw_base1<1>(*this,255);
    ut_timer_tw_base1<2>(*this,255);
    ut_timer_tw_base1<3>(*this,255);
    ut_timer_tw_base1<4>(*this,255);

    ut_timer_tw_base1<1>(*this,511);
    ut_timer_tw_base1<2>(*this,511);
    ut_timer_tw_base1<3>(*this,511);
    ut_timer_tw_base1<4>(*this,511);

    ut_timer_tw_base1<1>(*this,512);
    ut_timer_tw_base1<2>(*this,512);
    ut_timer_tw_base1<3>(*this,512);
    ut_timer_tw_base1<4>(*this,512);

    ut_timer_tw_base1<1>(*this,513);
    ut_timer_tw_base1<2>(*this,513);
    ut_timer_tw_base1<3>(*this,513);
    ut_timer_tw_base1<4>(*this,513);

    ut_timer_tw_base1<1>(*this,256*256-1);
    ut_timer_tw_base1<2>(*this,256*256-1);
    ut_timer_tw_base1<3>(*this,256*256-1);
    ut_timer_tw_base1<4>(*this,256*256-1);

    ut_timer_tw_base1<1>(*this,256*256);
    ut_timer_tw_base1<2>(*this,256*256);
    ut_timer_tw_base1<3>(*this,256*256);
    ut_timer_tw_base1<4>(*this,256*256);

    ut_timer_tw_base1<1>(*this,256*256+1);
    ut_timer_tw_base1<2>(*this,256*256+1);
    ut_timer_tw_base1<3>(*this,256*256+1);
    ut_timer_tw_base1<4>(*this,256*256+1);

    ut_timer_tw_base2<1>(*this);
    ut_timer_tw_base2<2>(*this);
    ut_timer_tw_base2<3>(*this);
    ut_timer_tw_base2<4>(*this);

    ut_timer_tw_base3<1>(*this);
    ut_timer_tw_base3<2>(*this);
    ut_timer_tw_base3<3>(*this);
    ut_timer_tw_base3<4>(*this);

    ut_timer_tw_base4<1>(*this,1000*30,100,100);
    ut_timer_tw_base4<1>(*this,1000*30,100,1000);
    ut_timer_tw_base4<1>(*this,1000*30,100,10000);

    ut_timer_tw_base4<2>(*this,1000*30,100,100);
    ut_timer_tw_base4<2>(*this,1000*30,100,1000);
    ut_timer_tw_base4<2>(*this,1000*30,100,10000);

    ut_timer_tw_base4<3>(*this,1000*30,100,100);
    ut_timer_tw_base4<3>(*this,1000*30,100,1000);
    ut_timer_tw_base4<3>(*this,1000*30,100,10000);

    ut_timer_tw_base4<4>(*this,1000*30,100,100);
    ut_timer_tw_base4<4>(*this,1000*30,100,1000);
    ut_timer_tw_base4<4>(*this,1000*30,100,10000);
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