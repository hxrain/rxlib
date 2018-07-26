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

        #ifdef RX_IS_CONSOLE
            printf("%s\r\n",Txt);
        #elif RX_OS_WIN
            MessageBoxA(NULL,Txt,"Message",MB_OK);
		#else
			printf("%s\r\n",Txt);
        #endif
    }

#if defined(RX_USE_ASSERT)
    static inline void _rx_assert(bool R,int LineNo,const char *File,const char *Msg,bool IsAlert=false)
    {
        if (R) return;
        char Buf[1024];
        const char* Tip=IsAlert?"debug":"assert";
        if (Msg==NULL||Msg[0]==0) snprintf(Buf,sizeof(Buf),"file <%s>\r\nline <%d>\r\n%s!\r\n",File,LineNo,Tip);
        else snprintf(Buf,sizeof(Buf),"file <%s>\r\nline <%d>\r\n%s!\r\ninfo <%s>",File,LineNo,Tip,Msg);
    #ifdef WIN32
        MessageBoxA(NULL,Buf,Tip,MB_OK);
        R=R;
    #else
        printf("%s!%s\r\n���س�������\r\n",Tip,Buf);getchar();
    #endif
    }

    #define rx_assert(R)            _rx_assert((R),__LINE__,__FILE__,"")
    #define rx_assert_msg(R,Msg)    _rx_assert((R),__LINE__,__FILE__,Msg)
    #define rx_alert(Msg)           _rx_assert(0,__LINE__,__FILE__,Msg,true)
    //�����,Ҫ��Ϊ��
    #define rx_check(R)             rx_assert(R)
#else
    static inline void _rx_assert(){}
    #define rx_assert(R)            _rx_assert()
    #define rx_assert_msg(R,Msg)    _rx_assert()
    #define rx_alert(Msg)           _rx_assert()
    //�����,Ϊ��ִ�м����ʽ
    #define rx_check(R)             (R)
#endif

#endif
