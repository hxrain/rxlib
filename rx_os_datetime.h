#ifndef _RX_OS_DATETIME_H_
#define _RX_OS_DATETIME_H_

    #include "rx_cc_macro.h"
	#include "rx_datetime.h"
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
        dp = *localtime((time_t*)&dt);                      //多线程不安全的标准函数
    #endif
        return (int32_t)rx_make_utc(dp, 0);
    }

    //---------------------------------------------------------------
	//获取当前系统的时间,UTC格式.
	inline uint64_t rx_time(){return time(NULL);}

#endif