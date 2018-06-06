#ifndef _RX_OS_DATETIME_H_
#define _RX_OS_DATETIME_H_

    #include "rx_cc_macro.h"
	#include "rx_datetime.h"
	#include <time.h>
/*
	本单元进行系统时间相关函数的封装处理.
		rx_time_zone()										获取系统当前时区(秒)
		rx_time()											获取系统当前时间(UTC秒)
*/

    //---------------------------------------------------------------
    //利用系统时间函数(本地时间),获取系统本地的时区(秒值)
    //注意:此方法多线程使用时须注意
    inline int32_t rx_time_zone()
    {
        time_t dt = 0;
        struct tm dp;
    #if RX_CC==RX_CC_VC
        localtime_s(&dp, &dt);                              //win上据说是安全的
    #elif RX_OS==RX_OS_LINUX
        localtime_r(&dt, &dp);                              //linux上据说有多线程锁定性能的问题
    #else
        dp = *localtime((time_t*)&dt);                    //多线程不安全的标准函数
    #endif
        return (int32_t)rx_make_utc(dp, 0);
    }

    //---------------------------------------------------------------
	//获取当前系统的时间,UTC格式.
	inline uint64_t rx_time(){return time(NULL);}
    //---------------------------------------------------------------
    //获取当前系统开机后的滴答数(微秒)
#if RX_OS==RX_OS_LINUX
    inline uint64_t rx_tick_us()
    {
        struct timespec tp;
        clock_gettime(CLOCK_MONOTONIC_RAW, &tp);
        return (tp.tv_sec * 1000 * 1000 + tp.tv_nsec / 1000);
    }
#elif defined(RX_OS_WIN)
    inline uint64_t rx_tick_us()
    {
        LARGE_INTEGER t;
        static uint64_t m_timer_freq = 0;
        if (!m_timer_freq)
        {
            if (QueryPerformanceFrequency(&t))
                m_timer_freq = t.QuadPart;
        }

        if (!QueryPerformanceCounter(&t) || !m_timer_freq)
            return -1;
        return uint64_t((double(t.QuadPart) / m_timer_freq) * 1000 * 1000);
    }
#else
    inline uint64_t rx_tick_us()
    {
        rx_st_assert(false, "unsupport os.");
        return -1;
    }
#endif


#endif
