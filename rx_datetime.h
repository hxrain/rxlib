#ifndef _MY_TIME_H_
#define _MY_TIME_H_
    
    #include "rx_cc_macro.h"
    #include <stdio.h>
    #include <time.h>
#if defined(RX_OS_WIN)
    #include <winsock.h>
#endif
/*
	本单元进行UTC时间和ISO时间的相互转换处理.
		rx_make_utc()										//根据ISO时间分量生成UTC时间
		rx_leap_year()										//判断给的的年份是否为闰年
		rx_localtime()										//将UTC时间转换为ISO时间分量,可指定目标时区.
		rx_iso_time()										//将UTC时间或时间分量转换为ISO格式的时间串
        rx_add_ms()                                         //进行时间结构体的毫秒值调整
        rx_time_ms()                                        //根据时间结构体计算毫秒数
	注意:由于UTC/Local时间的转换过程中,使用了立即参数进行的时区调整,而没有访问系统的时区与冬夏令时,
	    所以还需要设计另外的时区与冬夏令时校准函数,根据配置的简单规则校准zone_offset_sec参数.
*/

    //-----------------------------------------------------
    //对时间结构体进行毫秒调整
    inline void rx_add_ms(struct timespec &ts, int32_t ms)
    {
        //将毫秒间隔变为秒和纳秒
        ts.tv_sec += ms / 1000;
        ts.tv_nsec += (ms % 1000) * 1000 * 1000;

        //处理纳秒溢出,累计到秒分量上
        const uint32_t NSECTOSEC = 1000000000;
        ts.tv_sec += ts.tv_nsec / NSECTOSEC;
        ts.tv_nsec = ts.tv_nsec%NSECTOSEC;
    }
    inline void rx_add_ms(struct timeval &ts, int32_t ms)
    {
        //将毫秒间隔变为秒和u秒
        ts.tv_sec += ms / 1000;
        ts.tv_usec += (ms % 1000) * 1000;

        //处理u秒溢出,累计到秒分量上
        const uint32_t USECTOSEC = 1000000;
        ts.tv_sec += ts.tv_usec / USECTOSEC;
        ts.tv_usec = ts.tv_usec%USECTOSEC;
    }

    //-----------------------------------------------------
    //将秒和纳秒变为毫秒
    inline uint64_t rx_time_ms(struct timeval &time)
    {
        return time.tv_sec * 1000 + time.tv_usec / 1000;
    }
    //将秒和纳秒变为毫秒
    inline uint64_t rx_time_ms(struct timespec &time)
    {
        return time.tv_sec * 1000 + time.tv_nsec / (1000 * 1000);
    }
    //-----------------------------------------------------
    //将ISO年月日时分秒转,换成1970-1-1 00:00:00距离现在的总秒数UTC,时区(默认东八区北京时间)
    inline uint64_t rx_make_utc(uint32_t year, uint32_t mon, uint32_t day, uint32_t hour, uint32_t min, uint32_t sec,int32_t zone_offset_sec = 8*60*60)
    {
        mon -= 2;
        if ((int)mon <= 0)
        {
            mon += 12;
            year -= 1;
        }
        uint64_t TotalDay = (year / 4 - year / 100 + year / 400 + 367 * mon / 12 + day) + year * 365 - 719499;
        uint64_t TotalHour = TotalDay * 24 + hour;
        uint64_t TotalMin = TotalHour * 60 + min;
        uint64_t TotalSec = TotalMin * 60 + sec;
        return  TotalSec-zone_offset_sec;
    }
    inline uint64_t rx_make_utc(const struct tm &dt,int32_t zone_offset_sec=8*60*60) { return rx_make_utc(dt.tm_year+1900,dt.tm_mon+1,dt.tm_mday,dt.tm_hour,dt.tm_min,dt.tm_sec,zone_offset_sec); }

    //-----------------------------------------------------
    //是否为闰年
    inline bool rx_leap_year(uint32_t year) { return ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0); }
    //将bool值转换为0/1值
    inline uint32_t rx_bool_num(bool b) { return (b ? 1 : 0); }

    //-----------------------------------------------------
    //将utc秒转换为年月日结构.utc时间秒,localtime结构,时区(默认东八区北京时间)
    inline void rx_localtime(uint64_t utc_time, struct tm &tp, int32_t zone_offset_sec = 8*60*60)
    {
        const uint32_t sec_per_hour = (60 * 60);
        const uint32_t sec_per_day = (sec_per_hour * 24);

        #define DIV(a, b) ((a) / (b) - rx_bool_num((a) % (b) < 0))
        #define LEAPS_THRU_END_OF(year) (DIV (year, 4) - DIV (year, 100) + DIV (year, 400))

        static const uint16_t __mon_yday[2][13] =
        {
            /* Normal years.  */
            { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
            /* Leap years.  */
            { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
        };

        int64_t days = utc_time / sec_per_day;
        int64_t rem = utc_time % sec_per_day;
        rem += zone_offset_sec;
        while (rem < 0)
        {
            rem += sec_per_day;
            --days;
        }
        while (rem >= sec_per_day)
        {
            rem -= sec_per_day;
            ++days;
        }
        tp.tm_hour = int(rem / sec_per_hour);
        rem %= sec_per_hour;
        tp.tm_min = int(rem / 60);
        tp.tm_sec = int(rem % 60);

        //1970-1-1是周四.wday:1~6周一到周六,0为周日
        int16_t w_day = (4 + days) % 7;
        if (w_day < 0)
            w_day += 7;
        tp.tm_wday = (uint8_t)w_day;

        int32_t year = 1970;
        while (days < 0 || days >= (rx_leap_year(year) ? 366 : 365))
        {
            /* Guess a corrected year, assuming 365 days per year.  */
            int32_t yg = int(year + days / 365 - rx_bool_num(days % 365 < 0));

            /* Adjust DAYS and Y to match the guessed year.  */
            days -= ((yg - year) * 365
                + LEAPS_THRU_END_OF(yg - 1)
                - LEAPS_THRU_END_OF(year - 1));
            if (days<0)
                days = (int)days;
            year = yg;
        }
        tp.tm_year = year-1900;
        tp.tm_yday = (int)days;

        const uint16_t *ip = __mon_yday[rx_leap_year(year)];
        for (year = 11; days < (int32_t)ip[year]; --year)
            continue;
        days -= ip[year];
        tp.tm_mon = year;
        tp.tm_mday = (int)days + 1;

        #undef DIV
        #undef LEAPS_THRU_END_OF
    }
    //-----------------------------------------------------
    //将日期时间结构格式化为字符串
    inline void rx_iso_time(const struct tm& tp, char str[20])
    {
        snprintf(str,20,"%d-%02d-%02d %02d:%02d:%02d", tp.tm_year+1900, tp.tm_mon+1, tp.tm_mday, tp.tm_hour, tp.tm_min, tp.tm_sec);
    }
    //将UTC时间转为标准时间字符串
    inline void rx_iso_time(uint32_t utc_time,char str[20] , int32_t zone_offset_sec = 8 * 60 * 60)
    {
        struct tm tp;
        rx_localtime(utc_time,tp, zone_offset_sec);
        rx_iso_time(tp,str);
    }

#endif