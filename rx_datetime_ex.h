#ifndef _RX_OS_DATETIME_H_
#define _RX_OS_DATETIME_H_

    #include "rx_cc_macro.h"
	#include "rx_datetime.h"
	#include <time.h>
/*
	本单元进行系统时间相关函数的封装处理.
		rx_time_zone()										获取系统当前时区(秒)
		rx_time()											获取系统当前时间(UTC秒)
        rx_tick_us()                                        获取系统开机后至今的滴答数(微秒)
        rx_tick_ms()                                        获取系统开机后至今的滴答数(毫秒)
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

#if RX_OS==RX_OS_LINUX
    //---------------------------------------------------------------
    //获取当前系统开机后的滴答数(微秒)
    inline uint64_t rx_tick_us()
    {
        struct timespec tp;
        clock_gettime(CLOCK_MONOTONIC_RAW, &tp);
        return (tp.tv_sec * 1000 * 1000 + tp.tv_nsec / 1000);
    }
    //---------------------------------------------------------
    //获取当前系统开机后的滴答数(微秒),并增加ms毫秒后的时间
    inline bool rx_tick_us(struct timespec &ts, int32_t ms)
    {
        //获取系统UTC时间
        if (clock_gettime(CLOCK_MONOTONIC_RAW, &ts) < 0)
            return false;
        time_add_ms(ts, ms);
        return true;
    }

#elif defined(RX_OS_WIN)
    //---------------------------------------------------------------
    //获取当前系统开机后的滴答数(微秒)
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
    //---------------------------------------------------------------
    //获取当前系统开机后的滴答数(毫秒)
    inline uint64_t rx_tick_ms(){return rx_tick_us()/1000;}

#endif
