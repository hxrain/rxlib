#ifndef _RX_UT_localtime_H_
#define _RX_UT_localtime_H_

#include "../rx_os_datetime.h"
#include "../rx_datetime.h"
#include "../rx_tdd.h"

inline void test_localtime(uint64_t dt, struct tm &tp, rx_tdd_base &tdd)
{
    char t1[20],t2[20];
    rx_localtime(dt, tp);
    rx_iso_time(tp,t1);

    struct tm *dp = localtime((time_t*)&dt);
    rx_iso_time(tp,t2);

    tdd.assert(tp.tm_year == dp->tm_year,__LINE__,"%llu -> %s -> %s",dt,t1,t2);
    tdd.assert(tp.tm_mon  == dp->tm_mon,__LINE__,"%llu -> %s -> %s",dt,t1,t2);
    tdd.assert(tp.tm_mday == dp->tm_mday,__LINE__,"%llu -> %s -> %s",dt,t1,t2);
    tdd.assert(tp.tm_hour == dp->tm_hour,__LINE__,"%llu -> %s -> %s",dt,t1,t2);
    tdd.assert(tp.tm_min  == dp->tm_min,__LINE__,"%llu -> %s -> %s",dt,t1,t2);
    tdd.assert(tp.tm_sec  == dp->tm_sec,__LINE__,"%llu -> %s -> %s",dt,t1,t2);
    tdd.assert(tp.tm_wday == dp->tm_wday,__LINE__,"%llu -> %s -> %s",dt,t1,t2);
    tdd.assert(tp.tm_yday == dp->tm_yday,__LINE__,"%llu -> %s -> %s",dt,t1,t2);

    tdd.assert(rx_make_utc(tp) == dt,__LINE__,"%llu -> %s -> %s",dt,t1,t2);
}

inline void test_localtime_loop(rx_tdd_base &tdd)
{
    int tag = -1;
    struct tm tp;
    char tmp[20];
    uint64_t dtloop = rx_make_utc(2118, 5, 22, 10, 45, 15);
    for (uint64_t dt = 0; dt < dtloop; ++dt)
    {
        test_localtime(dt, tp, tdd);
        if (tp.tm_mday != tag)
        {
            tag = tp.tm_mday;
            rx_iso_time(tp,tmp);
            printf("%s\r\n", tmp);
        }
    }
}

rx_tdd(localtime_base)
{
    uint64_t dt = rx_make_utc(2018,5,22,10,45,15);
    struct tm tp;

    test_localtime(dt,tp,*this);

    test_localtime(94579200, tp, *this);
    test_localtime(157651200, tp, *this);
    test_localtime(3422361600, tp, *this);

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

    test_localtime_loop(*this);
}

#endif
