#ifndef _RX_OS_DATETIME_H_
#define _RX_OS_DATETIME_H_

    #include "rx_cc_macro.h"
	#include "rx_datetime.h"
	#include <time.h>
/*
	����Ԫ����ϵͳʱ����غ����ķ�װ����.
		rx_time_zone()										��ȡϵͳ��ǰʱ��(��)
		rx_time()											��ȡϵͳ��ǰʱ��(UTC��)
*/

    //---------------------------------------------------------------
    //����ϵͳʱ�亯��(����ʱ��),��ȡϵͳ���ص�ʱ��(��ֵ)
    //ע��:�˷������߳�ʹ��ʱ��ע��
    inline int32_t rx_time_zone()
    {
        time_t dt = 0;
        struct tm dp;
    #if RX_CC==RX_CC_VC
        localtime_s(&dp, &dt);                              //win�Ͼ�˵�ǰ�ȫ��
    #elif RX_OS==RX_OS_LINUX
        localtime_r(&dt, &dp);                              //linux�Ͼ�˵�ж��߳��������ܵ�����
    #else
        dp = *localtime((time_t*)&dt);                    //���̲߳���ȫ�ı�׼����
    #endif
        return (int32_t)rx_make_utc(dp, 0);
    }

    //---------------------------------------------------------------
	//��ȡ��ǰϵͳ��ʱ��,UTC��ʽ.
	inline uint64_t rx_time(){return time(NULL);}
    //---------------------------------------------------------------
    //��ȡ��ǰϵͳ������ĵδ���(΢��)
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
