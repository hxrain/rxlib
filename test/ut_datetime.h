#ifndef _RX_UT_localtime_H_
#define _RX_UT_localtime_H_

#include "../rx_os_datetime.h"
#include "../rx_datetime.h"
#include "../rx_tdd.h"

inline void test_localtime(uint64_t dt, struct tm &tp, rx_tdd_base &rt)
{
    char t1[20],t2[20];
    rx_localtime(dt, tp);
    rx_iso_time(tp,t1);

    struct tm dp = *localtime((time_t*)&dt);
    rx_iso_time(dp,t2);

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

inline void test_localtime_loop(rx_tdd_base &rt)
{
    int tag = -1;
    struct tm tp;
    char tmp[20];
    uint64_t dtloop = rx_make_utc(2118, 5, 22, 10, 45, 15);
    for (uint64_t dt = 0; dt < dtloop; ++dt)
    {
        test_localtime(dt, tp, rt);
        if (tp.tm_mday != tag)
        {
            tag = tp.tm_mday;
            rx_iso_time(tp,tmp);
            printf("%s\r\n", tmp);
        }
    }
}

inline void test_tick_us(rx_tdd_base &rt)
{
    uint64_t bt=rx_tick_us();
    char tmp[1024*128];
    memset(tmp,0,sizeof(tmp));
    uint32_t dt=uint32_t(rx_tick_us()-bt);
    rt.msg_assert(dt<500,"memset 1k byte use time:<%7u> us",dt);
}

inline void test_tick_us2(rx_tdd_base &rt)
{
    uint64_t bt=rx_tick_us();
    uint32_t dt=uint32_t(rx_tick_us()-bt);
    rt.msg_assert(dt<=100,"rx_tick_us() use time:<%7u> us",dt);
}


rx_tdd(localtime_base)
{
    enable_error_wait();

    uint64_t dt = rx_make_utc(2018,5,22,10,45,15);
    struct tm tp;

    test_localtime(dt,tp,*this);

    test_localtime(94579200, tp, *this);
    test_localtime(157651200, tp, *this);
    test_localtime(3422361600ul, tp, *this);

    tdd_assert(!rx_leap_year(2018));

    struct tm *dp = localtime((time_t*)&dt);

    rx_localtime(dt, tp);
    tdd_assert(tp.tm_year==2018-1900);
    tdd_assert(tp.tm_mon==5-1);
    tdd_assert(tp.tm_mday==22);
    tdd_assert(tp.tm_hour==10);
    tdd_assert(tp.tm_min==45);
    tdd_assert(tp.tm_sec==15);
    tdd_assert(tp.tm_wday==dp->tm_wday);
    tdd_assert(tp.tm_yday==dp->tm_yday);

    dt = rx_time_zone();

    for(int i=0;i<1000;++i)
        test_tick_us2(*this);

    for(int i=0;i<1000;++i)
        test_tick_us(*this);

    //test_localtime_loop(*this);
}

#endif
