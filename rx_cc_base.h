#ifndef RX_CC_BASE_H
#define RX_CC_BASE_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include "rx_cc_macro.h"


/*
    ����Ԫ���һЩ��������������ĳ��ù��ܺ���.
*/
namespace rx
{
    //-----------------------------------------------------
    //���ڽ��к�Ĭ�ϲ�����ֵ���
    template<typename T>
    struct macro_def_argv
    {
        static bool value() { return 0; }
        static T value(T v) { return v; }
    };

    //-----------------------------------------------------
    //����ָ������
    typedef void* ptr_t;
    //-----------------------------------------------------
    //���򵥵��޷�������ת�ַ�������.
    //����ֵ:��������ַ�������
    inline uint8_t u2s(uint32_t num,char *result)
    {
        if (num==0)
        {
            result[0]='0';
            result[1]=0;
            return 1;
        }           

        const uint8_t MAX_LEN=10;               //������ִ�����
        uint8_t N=MAX_LEN;
        char buff[MAX_LEN];
        while (N-->0)                           //����ѭ��ȡ�෴��������󳤶����ִ�,ǰ��'0'���
        {
            buff[N]=((char)(num%10)+'0');
            num/=10;
        }
        uint8_t ret=0;
        for(uint8_t i=0;i<MAX_LEN;++i)          //��������ɨ��,����ǰ��'0'������Ч�������
        {
            if (ret==0&&buff[i]=='0')
                continue;
            result[ret++]=buff[i];
        }
        
        result[ret]=0;
        return ret;
    }

    //-----------------------------------------------------
    //���򵥵ĵ������ַ�����ʽ������,Ŀǰ��֧��%u��%s���ָ�ʽ������.
    //��sfmt(buff,"abc<%u>",123),��buff�����ɽ��Ϊ"abc<123>"
    //����ֵ:���ɽ���ĳ���
    inline uint8_t vsfmt(char* buff,const char* fmt,va_list ap)
    {
        char c;
        uint8_t ret=0;
        while((c=*fmt))
        {
            if (c!='%')
            {//���������ַ�,ֱ�Ӹ��ƹ�ȥ
                buff[ret++]=c;
                ++fmt;
                continue;
            }
            switch(*(fmt+1))
            {
                case '%':
                {//�����ַ���ת���ַ�,ֱ�Ӹ��ƹ�ȥ
                    buff[ret++]='%';
                    fmt+=2;
                    continue;
                }
                case 'u':
                {//�޷������ֲ���
                    ret+=u2s(va_arg(ap,uint32_t),buff+ret);
                    fmt+=2;
                    break;
                }
                case 's':
                {//�ַ�������
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
    //��������ʽ�����ɺ���
    inline uint8_t sfmt(char* buff,const char* fmt,...)
    {
        va_list ap;
        va_start(ap,fmt);
        uint8_t ret=vsfmt(buff,fmt,ap);
        va_end(ap);
        return ret;
    }

    //-----------------------------------------------------
    //�������������ַ�����������ʽ��ƴװ����
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
    //����os_cc_desc()����,���ڻ�ȡ��ǰ��������Ϣ
    //visual studio, eg : "CPU=X64(LE)/OS=win64/CC=<native>'Microsoft Visual C++'<16.0.1600.1>/WORDS=64Bit"
    inline const char* os_cc_desc()
    {
        static sncat<128> scat;
        scat("CPU=%s(%s)",RX_CPU_ARCH,RX_CPU_LEBE)("/OS=%s",RX_OS_NAME)("/CC=<%s>'%s'",RX_CC_ENV_NAME,RX_CC_NAME)
            ("<%u.%u.%u.%u>",RX_CC_VER_MAJOR,RX_CC_VER_MINOR,RX_CC_VER_PATCH,RX_CC_VER_BUILD)("/WORDS=%uBit",RX_CC_BIT);
        return scat.str;
    }
    //-----------------------------------------------------
    //ȡ˫ֵ�е���Сֵ
#if !defined(min)
    template<class DT>
    inline const DT min(const DT& a,const DT& b){return a<b?a:b;}
#endif
    template<class DT>
    inline const DT Min(const DT& a,const DT& b){return a<b?a:b;}
    //ȡ˫ֵ�е����ֵ
#if !defined(max)
    template<class DT>
    inline const DT max(const DT& a,const DT& b){return a>b?a:b;}
#endif
    template<class DT>
    inline const DT Max(const DT& a,const DT& b){return a>b?a:b;}
    //-----------------------------------------------------
    //ȡ��ֵ�е���Сֵ
    template<class DT>
    inline const DT Min(const DT& a,const DT& b,const DT& c){return min(a,min(b,c));}
    //ȡ��ֵ�е����ֵ
    template<class DT>
    inline const DT Max(const DT& a,const DT& b,const DT& c){return max(a,max(b,c));}

    //��������������ֵ
    template <class T>
    inline void Swap(T& A, T& B) { T C(A); A = B; B = C; }
    //-----------------------------------------------------
    //���峣�õ���ѧ����
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

