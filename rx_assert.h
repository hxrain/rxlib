#ifndef _RX_ASSERT_H_
#define _RX_ASSERT_H_

#include "rx_cc_macro.h"
#include <stdio.h>
#include <stdarg.h>
    //---------------------------------------------------------
    //默认,在调试版下开启断言
    #ifndef RX_USE_ASSERT
        #if defined(RX_CC_DEBUG)
            #define RX_USE_ASSERT       1
        #endif
    #endif

    //如果命令禁用,则关闭断言
    #ifdef RX_BAN_ASSERT
        #undef RX_USE_ASSERT
    #endif

    //---------------------------------------------------------
    //立即显示消息,在控制台下就是printf,在win32下就是弹出消息框
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
        printf("%s!%s\r\n按回车键继续\r\n",Tip,Buf);getchar();
    #endif
    }

    #define rx_assert(R)            _rx_assert((R),__LINE__,__FILE__,"")
    #define rx_assert_msg(R,Msg)    _rx_assert((R),__LINE__,__FILE__,Msg)
    #define rx_alert(Msg)           _rx_assert(0,__LINE__,__FILE__,Msg,true)
    //检查结果,要求为真
    #define rx_check(R)             rx_assert(R)
#else
    static inline void _rx_assert(){}
    #define rx_assert(R)            _rx_assert()
    #define rx_assert_msg(R,Msg)    _rx_assert()
    #define rx_alert(Msg)           _rx_assert()
    //检查结果,为了执行检查表达式
    #define rx_check(R)             (R)
#endif

#endif
