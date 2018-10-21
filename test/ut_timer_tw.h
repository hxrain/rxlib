#ifndef _RX_UT_TIMER_TW_H_
#define _RX_UT_TIMER_TW_H_

#include "../rx_tdd.h"
#include "../rx_timer_tw.h"

class ut_timer_tw_result
{
public:
    uint64_t    hits;
    ut_timer_tw_result():hits(0){}
    void on_timer(size_t handle, uint32_t event_code, uint16_t repeat)
    {
        ++hits;
    }
};

template<uint32_t wheels>
inline void ut_timer_tw_base0(rx_tdd_t &rt,uint32_t cycle)
{
    ut_timer_tw_result tr;
    rx::timing_wheel_t<wheels> w;
    size_t h;
    uint64_t curr_time = 128;
    rt.tdd_assert(w.wheels_init(curr_time));
    rt.tdd_assert(tr.hits == 0);

    h = w.timer_insert(mr_obj(ut_timer_tw_result, tr, on_timer), cycle, 0, 1);
    rt.tdd_assert(h > 0);
    curr_time += cycle-2;
    rt.tdd_assert(w.wheels_step(++curr_time) == 0);
    rt.tdd_assert(tr.hits == 0);
    rt.tdd_assert(w.wheels_step(++curr_time) == 1);
    rt.tdd_assert(tr.hits == 1);
    rt.tdd_assert(w.wheels_step(++curr_time) == 0);
    rt.tdd_assert(tr.hits == 1);
}

template<uint32_t wheels>
inline void ut_timer_tw_base1(rx_tdd_t &rt)
{
    ut_timer_tw_result tr;
    rx::timing_wheel_t<wheels> w;
    uint64_t curr_time = 0;
    size_t h;
    rt.tdd_assert(w.wheels_init(++curr_time));
    rt.tdd_assert(tr.hits == 0);

    h = w.timer_insert(mr_obj(ut_timer_tw_result, tr, on_timer), 255, 0, 1);
    rt.tdd_assert(h > 0);
    curr_time += 253;
    rt.tdd_assert(w.wheels_step(++curr_time) == 0);
    rt.tdd_assert(tr.hits == 0);
    rt.tdd_assert(w.wheels_step(++curr_time) == 1);
    rt.tdd_assert(tr.hits == 1);
    rt.tdd_assert(w.wheels_step(++curr_time) == 0);
    rt.tdd_assert(tr.hits == 1);

    h = w.timer_insert(mr_obj(ut_timer_tw_result, tr, on_timer), 256, 0, 1);
    rt.tdd_assert(h > 0);
    curr_time += 254;
    rt.tdd_assert(w.wheels_step(++curr_time) == 0);
    rt.tdd_assert(tr.hits == 1);
    rt.tdd_assert(w.wheels_step(++curr_time) == 1);
    rt.tdd_assert(tr.hits == 2);
    rt.tdd_assert(w.wheels_step(++curr_time) == 0);
    rt.tdd_assert(tr.hits == 2);

    h = w.timer_insert(mr_obj(ut_timer_tw_result, tr, on_timer), 257, 0, 1);
    rt.tdd_assert(h > 0);
    curr_time += 255;
    rt.tdd_assert(w.wheels_step(++curr_time) == 0);
    rt.tdd_assert(tr.hits == 3);
    rt.tdd_assert(w.wheels_step(++curr_time) == 1);
    rt.tdd_assert(tr.hits == 4);
    rt.tdd_assert(w.wheels_step(++curr_time) == 0);
    rt.tdd_assert(tr.hits == 4);
}


template<uint32_t wheels>
inline void ut_timer_tw_base(rx_tdd_t &rt)
{
    ut_timer_tw_result tr;
    rx::timing_wheel_t<wheels> w;
    uint64_t curr_time = 0;
    size_t h;
    rt.tdd_assert(w.wheels_init(++curr_time));
    rt.tdd_assert(w.wheels_step(++curr_time) == 0);
    rt.tdd_assert(tr.hits == 0);
    h = w.timer_insert(mr_obj(ut_timer_tw_result, tr, on_timer), 1, 0, 1);
    rt.tdd_assert(h > 0);
    rt.tdd_assert(w.wheels_step(++curr_time) == 1);
    rt.tdd_assert(tr.hits==1);
    rt.tdd_assert(w.wheels_step(++curr_time) == 0);
    rt.tdd_assert(tr.hits == 1);

    h = w.timer_insert(mr_obj(ut_timer_tw_result, tr, on_timer), 1, 0, 1);
    rt.tdd_assert(h > 0);
    rt.tdd_assert(w.timer_update(h));
    rt.tdd_assert(w.timer_remove(h));
    rt.tdd_assert(w.wheels_step(++curr_time) == 0);
    rt.tdd_assert(tr.hits == 1);

    h = w.timer_insert(mr_obj(ut_timer_tw_result, tr, on_timer), 2, 0, 1);
    rt.tdd_assert(h > 0);
    rt.tdd_assert(w.wheels_step(++curr_time) == 0);
    rt.tdd_assert(tr.hits == 1);
    rt.tdd_assert(w.wheels_step(++curr_time) == 1);
    rt.tdd_assert(tr.hits == 2);

    h = w.timer_insert(mr_obj(ut_timer_tw_result, tr, on_timer), 255, 0, 1);
    rt.tdd_assert(h > 0);
    curr_time += 254;
    rt.tdd_assert(w.wheels_step(++curr_time) == 1);
    rt.tdd_assert(tr.hits == 3);
    rt.tdd_assert(w.wheels_step(++curr_time) == 0);
    rt.tdd_assert(tr.hits == 3);

    h = w.timer_insert(mr_obj(ut_timer_tw_result, tr, on_timer), 257, 0, 1);
    rt.tdd_assert(h > 0);
    curr_time += 256;
    rt.tdd_assert(w.wheels_step(++curr_time) == 1);
    rt.tdd_assert(tr.hits == 4);
    rt.tdd_assert(w.wheels_step(++curr_time) == 0);
    rt.tdd_assert(tr.hits == 4);

    h = w.timer_insert(mr_obj(ut_timer_tw_result, tr, on_timer), 2, 0, 1);
    rt.tdd_assert(h > 0);
    rt.tdd_assert(w.wheels_step(++curr_time) == 0);
    rt.tdd_assert(tr.hits == 4);
    rt.tdd_assert(w.timer_update(h));
    rt.tdd_assert(w.wheels_step(++curr_time) == 0);
    rt.tdd_assert(tr.hits == 4);
    rt.tdd_assert(w.wheels_step(++curr_time) == 1);
    rt.tdd_assert(tr.hits == 5);


    h = w.timer_insert(mr_obj(ut_timer_tw_result, tr, on_timer), 256, 0, 1);
    rt.tdd_assert(h > 0);
    curr_time += 254;
    rt.tdd_assert(w.wheels_step(++curr_time) == 0);
    rt.tdd_assert(tr.hits == 5);
    rt.tdd_assert(w.wheels_step(++curr_time) == 1);
    rt.tdd_assert(tr.hits == 6);
    rt.tdd_assert(w.wheels_step(++curr_time) == 0);
    rt.tdd_assert(tr.hits == 6);
}

rx_tdd(ut_timer_base)
{
    ut_timer_tw_base0<1>(*this,2);
    ut_timer_tw_base0<2>(*this,2);
    ut_timer_tw_base0<3>(*this,2);
    ut_timer_tw_base0<4>(*this,2);

    ut_timer_tw_base0<1>(*this,257);
    ut_timer_tw_base0<2>(*this,257);
    ut_timer_tw_base0<3>(*this,257);
    ut_timer_tw_base0<4>(*this,257);

    ut_timer_tw_base0<1>(*this,256);
    ut_timer_tw_base0<2>(*this,256);
    ut_timer_tw_base0<3>(*this,256);
    ut_timer_tw_base0<4>(*this,256);

    ut_timer_tw_base0<1>(*this,255);
    ut_timer_tw_base0<2>(*this,255);
    ut_timer_tw_base0<3>(*this,255);
    ut_timer_tw_base0<4>(*this,255);

    ut_timer_tw_base0<1>(*this,511);
    ut_timer_tw_base0<2>(*this,511);
    ut_timer_tw_base0<3>(*this,511);
    ut_timer_tw_base0<4>(*this,511);

    ut_timer_tw_base0<1>(*this,512);
    ut_timer_tw_base0<2>(*this,512);
    ut_timer_tw_base0<3>(*this,512);
    ut_timer_tw_base0<4>(*this,512);

    ut_timer_tw_base0<1>(*this,513);
    ut_timer_tw_base0<2>(*this,513);
    ut_timer_tw_base0<3>(*this,513);
    ut_timer_tw_base0<4>(*this,513);

    for(uint32_t i=2;i<256*256+1024;++i)
    {
        ut_timer_tw_base0<1>(*this,i);
        ut_timer_tw_base0<2>(*this,i);
        ut_timer_tw_base0<3>(*this,i);
        ut_timer_tw_base0<4>(*this,i);
    }
    //ut_timer_tw_base<1>(*this);
    //ut_timer_tw_base<2>(*this);
    //ut_timer_tw_base<3>(*this);
    //ut_timer_tw_base<4>(*this);
}

#endif