#ifndef RX_CC_BASE_H
#define RX_CC_BASE_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include "rx_cc_macro.h"


/*
    本单元存放一些最基础简单无依赖的常用功能函数.
*/
namespace rx
{
    //-----------------------------------------------------
    //用于进行宏默认参数的值替代
    template<typename T>
    struct macro_def_argv
    {
        static bool value() { return 0; }
        static T value(T v) { return v; }
    };

    //-----------------------------------------------------
    //定义指针类型
    typedef void* ptr_t;
    //-----------------------------------------------------
    //超简单的无符号数字转字符串函数.
    //返回值:本次输出字符串长度
    inline uint8_t u2s(uint32_t num,char *result)
    {
        if (num==0)
        {
            result[0]='0';
            result[1]=0;
            return 1;
        }           

        const uint8_t MAX_LEN=10;               //最大数字串长度
        uint8_t N=MAX_LEN;
        char buff[MAX_LEN];
        while (N-->0)                           //进行循环取余反向生成最大长度数字串,前部'0'填充
        {
            buff[N]=((char)(num%10)+'0');
            num/=10;
        }
        uint8_t ret=0;
        for(uint8_t i=0;i<MAX_LEN;++i)          //进行正向扫描,跳过前导'0'并将有效数字输出
        {
            if (ret==0&&buff[i]=='0')
                continue;
            result[ret++]=buff[i];
        }
        
        result[ret]=0;
        return ret;
    }

    //-----------------------------------------------------
    //超简单的单参数字符串格式化函数,目前仅支持%u和%s两种格式化功能.
    //如sfmt(buff,"abc<%u>",123),在buff中生成结果为"abc<123>"
    //返回值:生成结果的长度
    inline uint8_t vsfmt(char* buff,const char* fmt,va_list ap)
    {
        char c;
        uint8_t ret=0;
        while((c=*fmt))
        {
            if (c!='%')
            {//不是特殊字符,直接复制过去
                buff[ret++]=c;
                ++fmt;
                continue;
            }
            switch(*(fmt+1))
            {
                case '%':
                {//特殊字符的转义字符,直接复制过去
                    buff[ret++]='%';
                    fmt+=2;
                    continue;
                }
                case 'u':
                {//无符号数字参数
                    ret+=u2s(va_arg(ap,uint32_t),buff+ret);
                    fmt+=2;
                    break;
                }
                case 's':
                {//字符串参数
                    const char* str=va_arg(ap,char*);
                    while(*str)
                        buff[ret++]=*str++;
                    fmt+=2;
                    break;
                }
                default:
                    buff[ret++]='%';
                    ++fmt;
            }
        }
        buff[ret]=0;
        return ret;
    }
    //-----------------------------------------------------
    //轻量级格式化生成函数
    inline uint8_t sfmt(char* buff,const char* fmt,...)
    {
        va_list ap;
        va_start(ap,fmt);
        uint8_t ret=vsfmt(buff,fmt,ap);
        va_end(ap);
        return ret;
    }

    //-----------------------------------------------------
    //简易轻量级的字符串缓冲区格式化拼装功能
    template<uint32_t maxsize>
    class sncat
    {
    public:
        char str[maxsize];
        uint32_t size;
        sncat():size(0){str[0]=0;}
        sncat& operator () (const char* fmt,...) {va_list ap;va_start(ap,fmt);size+=vsfmt(str+size,fmt,ap);va_end(ap);return *this;}
    };
    template<>
    class sncat<0>
    {
    public:
        char *str;
        uint32_t size;
        sncat(char* buff):str(buff),size(0){str[0]=0;}
        sncat& operator () (const char* fmt,...) {va_list ap;va_start(ap,fmt);size+=vsfmt(str+size,fmt,ap);va_end(ap);return *this;}
    };

    //-------------------------------------------------
    //构造os_cc_desc()函数,便于获取当前编译期信息
    //visual studio, eg : "CPU=X64(LE)/OS=win64/CC=<native>'Microsoft Visual C++'<16.0.1600.1>/WORDS=64Bit"
    inline const char* os_cc_desc()
    {
        static sncat<128> scat;
        scat("CPU=%s(%s)",RX_CPU_ARCH,RX_CPU_LEBE)("/OS=%s",RX_OS_NAME)("/CC=<%s>'%s'",RX_CC_ENV_NAME,RX_CC_NAME)
            ("<%u.%u.%u.%u>",RX_CC_VER_MAJOR,RX_CC_VER_MINOR,RX_CC_VER_PATCH,RX_CC_VER_BUILD)("/WORDS=%uBit",RX_CC_BIT);
        return scat.str;
    }
    //-----------------------------------------------------
    //取双值中的最小值
#if !defined(min)
    template<class DT>
    inline const DT min(const DT& a,const DT& b){return a<b?a:b;}
#endif
    template<class DT>
    inline const DT Min(const DT& a,const DT& b){return a<b?a:b;}
    //取双值中的最大值
#if !defined(max)
    template<class DT>
    inline const DT max(const DT& a,const DT& b){return a>b?a:b;}
#endif
    template<class DT>
    inline const DT Max(const DT& a,const DT& b){return a>b?a:b;}
    //-----------------------------------------------------
    //取三值中的最小值
    template<class DT>
    inline const DT Min(const DT& a,const DT& b,const DT& c){return min(a,min(b,c));}
    //取三值中的最大值
    template<class DT>
    inline const DT Max(const DT& a,const DT& b,const DT& c){return max(a,max(b,c));}

    //交换两个变量的值
    template <class T>
    inline void Swap(T& A, T& B) { T C(A); A = B; B = C; }
    //-----------------------------------------------------
    //定义常用的数学常量
    static const double MATH_E        =2.71828182845904523536   ;// e
    static const double MATH_LOG2E    =1.44269504088896340736   ;// log2(e)
    static const double MATH_LOG10E   =0.434294481903251827651  ;// log10(e)
    static const double MATH_LN2      =0.693147180559945309417  ;// ln(2)
    static const double MATH_LN10     =2.30258509299404568402   ;// ln(10)
    static const double MATH_PI       =3.14159265358979323846   ;// pi
    static const double MATH_PI_2     =1.57079632679489661923   ;// pi/2
    static const double MATH_PI_4     =0.785398163397448309616  ;// pi/4
    static const double MATH_1_PI     =0.318309886183790671538  ;// 1/pi
    static const double MATH_2_PI     =0.636619772367581343076  ;// 2/pi
    static const double MATH_2_SQRTPI =1.12837916709551257390   ;// 2/sqrt(pi)
    static const double MATH_SQRT2    =1.41421356237309504880   ;// sqrt(2)
    static const double MATH_SQRT1_2  =0.707106781186547524401  ;// 1/sqrt(2)
}
#endif

