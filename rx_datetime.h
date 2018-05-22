#ifndef _MY_TIME_H_
#define _MY_TIME_H_
    
    #include "rx_cc_macro.h"
    #include <stdio.h>
    #include <time.h>

    //---------------------------------------------------------------
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

    //---------------------------------------------------------------
    //是否为闰年
    inline bool rx_leap_year(uint32_t year) { return (year % 4 == 0) && (year % 100 != 0) || (year % 400 == 0); }
    //将bool值转换为0/1值
    inline uint32_t rx_bool_num(bool b) { return (b ? 1 : 0); }

    //---------------------------------------------------------------
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
    //---------------------------------------------------------------
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
    //---------------------------------------------------------------
    //利用系统时间函数(本地时间),获取本地的时区(秒值)
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

#endif