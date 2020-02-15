#ifndef _RX_UT_localtime_H_
#define _RX_UT_localtime_H_

#include "../rx_os_thread.h"
#include "../rx_datetime_ex.h"
#include "../rx_datetime.h"
#include "../rx_tdd.h"


inline void test_localtime(uint64_t dt, struct tm &tp, rx_tdd_t &rt)
{
    char t1[20],t2[20];
    rx_localtime(dt, tp);
    rx_iso_datetime(tp,t1);

    struct tm *p = localtime((time_t*)&dt);
    if (p==NULL)
    {
        //rx_alert("localtime fail.");
        return;
    }
    struct tm dp =*p;
    rx_iso_datetime(dp,t2);

    rt.msg_assert(tp.tm_year == dp.tm_year,"%llu -> %s -> %s",dt,t1,t2);
    rt.msg_assert(tp.tm_mon  == dp.tm_mon,"%llu -> %s -> %s",dt,t1,t2);
    rt.msg_assert(tp.tm_mday == dp.tm_mday,"%llu -> %s -> %s",dt,t1,t2);
    rt.msg_assert(tp.tm_hour == dp.tm_hour,"%llu -> %s -> %s",dt,t1,t2);
    rt.msg_assert(tp.tm_min  == dp.tm_min,"%llu -> %s -> %s",dt,t1,t2);
    rt.msg_assert(tp.tm_sec  == dp.tm_sec,"%llu -> %s -> %s",dt,t1,t2);
    rt.msg_assert(tp.tm_wday == dp.tm_wday,"%llu -> %s -> %s",dt,t1,t2);
    rt.msg_assert(tp.tm_yday == dp.tm_yday,"%llu -> %s -> %s",dt,t1,t2);

    rt.msg_assert(rx_make_utc(tp) == dt,"%llu -> %s -> %s",dt,t1,t2);
}

inline void test_localtime_loop(rx_tdd_t &rt)
{
    int tag = -1;
    struct tm tp;
    char tmp[20];
    time_t ct=time(NULL);
    printf("rx_time_zone=%d ct=%u\n",rx_time_zone(ct),(uint32_t)ct);
    printf("rx_time_zone=%d ct=%u\n",rx_time_zone(575),575);
    printf("rx_time_zone=%d ct=%u\n",rx_time_zone(57599),57599);
    printf("rx_time_zone=%d ct=%u\n",rx_time_zone(57600),57600);
    printf("rx_time_zone=%d ct=%u\n",rx_time_zone(57601),57601);

    uint64_t dtloop = rx_make_utc(2118, 5, 22, 10, 45, 15);
#if RX_CC_MINGW
    dtloop=57600;   //mingw的时区数据有问题
#endif
    for (uint64_t dt = 0; dt < dtloop; ++dt)
    {
        test_localtime(dt, tp, rt);
        if (tp.tm_mday != tag)
        {
            tag = tp.tm_mday;
            rx_iso_datetime(tp,tmp);
            printf("%s\r\n", tmp);
        }
    }
}

inline void test_tick_us(rx_tdd_t &rt)
{
    uint64_t bt=rx_get_tick_us();
    char tmp[1024*128];
    memset(tmp,0,sizeof(tmp));
    uint32_t dt=uint32_t(rx_get_tick_us()-bt);
    rt.msg_assert(dt<500,"memset 1k byte use time:<%7u> us",dt);
}

inline void test_tick_us2(rx_tdd_t &rt)
{
    uint64_t bt=rx_get_tick_us();
    uint32_t dt=uint32_t(rx_get_tick_us()-bt);
    rt.msg_assert(dt<=100,"rx_tick_us() use time:<%7u> us",dt);
}

inline void test_localtime_base_1(rx_tdd_t &rt)
{
    rt.enable_error_wait();

    uint64_t dt = rx_make_utc(2018,5,22,10,45,15);
    struct tm tp;

    test_localtime(dt,tp,rt);

    test_localtime(94579200, tp, rt);
    test_localtime(157651200, tp, rt);
    test_localtime(3422361600ul, tp, rt);

    rt.tdd_assert(!rx_leap_year(2018));

    struct tm *dp = localtime((time_t*)&dt);

    rx_localtime(dt, tp);
    rt.tdd_assert(tp.tm_year==2018-1900);
    rt.tdd_assert(tp.tm_mon==5-1);
    rt.tdd_assert(tp.tm_mday==22);
    rt.tdd_assert(tp.tm_hour==10);
    rt.tdd_assert(tp.tm_min==45);
    rt.tdd_assert(tp.tm_sec==15);
    rt.tdd_assert(tp.tm_wday==dp->tm_wday);
    rt.tdd_assert(tp.tm_yday==dp->tm_yday);

    dt = rx_time_zone();

    for(int i=0;i<1000;++i)
        test_tick_us2(rt);

    for(int i=0;i<1000;++i)
        test_tick_us(rt);
}

inline void test_tick_base_1(rx_tdd_t &rt)
{
    rx::tick_us_t tu;
    rx::tick_us_t::TickType BT=tu.update();
    rx::tick_us_t::TickType ET=tu.update();
    rt.tdd_assert(ET-BT<100);

    rx::meter_us_t tmu(1);
    rx_thread_yield_us(10);
    rt.tdd_assert(tmu.is_timing());

    rx::speeder_us_t smu;
    smu.set(10);
    rt.tdd_assert(!smu.hit(100));
    rt.tdd_assert(smu.count()==1);
    rt.tdd_assert(smu.total()==100);
    rt.tdd_assert(smu.value()==0);

    rx_thread_yield_us(10);
    rt.tdd_assert(smu.hit(10));
    rt.tdd_assert(smu.count()==2);
    rt.tdd_assert(smu.total()==110);
    rt.tdd_assert(smu.value()==110);
}

rx_tdd(localtime_base)
{
    test_tick_base_1(*this);
}

rx_tdd_rtl(localtime_loop,tdd_level_slow)
{
#if defined(RX_IS_OS_WIN)
    test_localtime_loop(*this);
#endif
    test_localtime_base_1(*this);
}

#endif
