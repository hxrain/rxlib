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

    #if defined(RX_IS_CONSOLE) && RX_IS_CONSOLE
        printf("%s\r\n",Txt);
    #elif RX_OS_WIN
        MessageBoxA(NULL,Txt,"Message",MB_OK);
    #else
        printf("%s\r\n",Txt);
    #endif
}

//---------------------------------------------------------
//统一的断言判断处理函数
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
        printf("按回车键继续\r\n");
        getchar();
    #endif

    R=!!R;                                                  //便于设置断点使用的临时语句.出现断言后在这里设置断点,然后再进行键盘动作即可.
}

#if defined(RX_USE_ASSERT)
    #define rx_assert(R)            rx_assert_fun((R),__LINE__,__FILE__,"")
    #define rx_assert_msg(R,Msg)    rx_assert_fun((R),__LINE__,__FILE__,Msg)
    #define rx_alert(Msg)           rx_assert_fun(false,__LINE__,__FILE__,Msg,true)
    //检查结果,要求为真
    #define rx_check(R)             rx_assert(R)
#else
    #define rx_assert(R)            {}
    #define rx_assert_msg(R,Msg)    {}
    #define rx_alert(Msg)           {}
    //检查结果,为了执行检查表达式
    #define rx_check(R)             (R)
#endif

//运行时失败提示
#define rx_fail(R) rx_assert_fun((R),__LINE__,__FILE__,"RX RUNTIME FAIL CHECK!")
#define rx_fail_msg(R,msg) rx_assert_fun((R),__LINE__,__FILE__,msg)

//断言条件判断,如果C成立,则要求R也成立.
#define rx_assert_if(C,R) rx_assert(((C)?(R):true))

#endif
