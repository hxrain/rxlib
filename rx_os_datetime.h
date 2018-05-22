#ifndef _RX_OS_DATETIME_H_
#define _RX_OS_DATETIME_H_

    #include "rx_cc_macro.h"
	#include "rx_datetime.h"
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
        dp = *localtime((time_t*)&dt);                      //���̲߳���ȫ�ı�׼����
    #endif
        return (int32_t)rx_make_utc(dp, 0);
    }

    //---------------------------------------------------------------
	//��ȡ��ǰϵͳ��ʱ��,UTC��ʽ.
	inline uint64_t rx_time(){return time(NULL);}

#endif