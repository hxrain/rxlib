#ifndef _RX_ASSERT_H_
#define _RX_ASSERT_H_

#include "rx_cc_macro.h"
#include <stdio.h>
#include <stdarg.h>
//---------------------------------------------------------
//Ĭ��,�ڵ��԰��¿�������
#ifndef RX_USE_ASSERT
    #if defined(RX_CC_DEBUG)
        #define RX_USE_ASSERT       1
    #endif
#endif

//����������,��رն���
#ifdef RX_BAN_ASSERT
    #undef RX_USE_ASSERT
#endif

//---------------------------------------------------------
//������ʾ��Ϣ,�ڿ���̨�¾���printf,��win32�¾��ǵ�����Ϣ��
inline void rx_show_msg(const char* Msg,...)
{
    char Txt[2048];
    va_list	ap;

    va_start(ap, Msg);
    vsnprintf(Txt,sizeof(Txt), Msg, ap);
    va_end(ap);

    #if defined(RX_IS_CONSOLE) && RX_IS_CONSOLE
        printf("%s\r\n",Txt);
    #elif RX_OS_WIN
        MessageBoxA(NULL,Txt,"Message",MB_OK);
    #else
        printf("%s\r\n",Txt);
    #endif
}

//---------------------------------------------------------
//ͳһ�Ķ����жϴ�����
inline void rx_assert_fun(bool R,int LineNo,const char *File,const char *Msg,bool IsAlert=false)
{
    if (R)
        return;

    const char* Tip=IsAlert?"DEBUG":"ASSERT FAIL";
    if (Msg==NULL||Msg[0]==0)
        rx_show_msg("\n<%s> : <%s:%u>\n",Tip,File,LineNo);
    else
        rx_show_msg("\n<%s> : {%s} : <%s:%u>\n",Tip,Msg,File,LineNo);

    #if RX_IS_CONSOLE
        printf("���س�������\r\n");
        getchar();
    #endif

    R=!!R;                                                  //�������öϵ�ʹ�õ���ʱ���.���ֶ��Ժ����������öϵ�,Ȼ���ٽ��м��̶�������.
}

#if defined(RX_USE_ASSERT)
    #define rx_assert(R)            rx_assert_fun((R),__LINE__,__FILE__,"")
    #define rx_assert_msg(R,Msg)    rx_assert_fun((R),__LINE__,__FILE__,Msg)
    #define rx_alert(Msg)           rx_assert_fun(false,__LINE__,__FILE__,Msg,true)
    //�����,Ҫ��Ϊ��
    #define rx_check(R)             rx_assert(R)
#else
    #define rx_assert(R)            {}
    #define rx_assert_msg(R,Msg)    {}
    #define rx_alert(Msg)           {}
    //�����,Ϊ��ִ�м����ʽ
    #define rx_check(R)             (R)
#endif

//����ʱʧ����ʾ
#define rx_fail(R) rx_assert_fun((R),__LINE__,__FILE__,"RX RUNTIME FAIL CHECK!")
#define rx_fail_msg(R,msg) rx_assert_fun((R),__LINE__,__FILE__,msg)

//���������ж�,���C����,��Ҫ��RҲ����.
#define rx_assert_if(C,R) rx_assert(((C)?(R):true))

#endif
