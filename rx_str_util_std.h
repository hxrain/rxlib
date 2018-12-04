#ifndef _RX_STR_UTIL_STD_H_
#define _RX_STR_UTIL_STD_H_

#include "rx_cc_macro.h"
#include "rx_ct_util.h"
#include "rx_assert.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <stdarg.h>

#if RX_OS==RX_OS_WIN||RX_OS_POSIX
#define RX_STR_USE_FILE 1
#endif

namespace rx
{
    //-----------------------------------------------------
    //字符串相关常量封装
    template<class CT> class sc;
    template<> class sc<char>
    {
    public:
        static const uint32_t char_size() { return sizeof(char); }
        static const char* hex_upr() {return "0123456789ABCDEF";}
        static const char* hex_lwr() {return "0123456789abcdef";}
        static const char  hex_upr(uint32_t i) {return "0123456789ABCDEF"[i];}
        static const char  hex_lwr(uint32_t i) {return "0123456789abcdef"[i];}
        static const char  zero() {return '0';}
        static const char* empty() {return "";}
        static const char  CR() {return '\r';}
        static const char  LF() {return '\n';}
        static const char  space() {return ' ';}
        static const char  A() {return 'A';}
        static const char  F() {return 'F';}
        static const char  Z() {return 'Z';}
        static const char  a() {return 'a';}
        static const char  f() {return 'f';}
        static const char  x() {return 'x';}
        static const char  z() {return 'z';}
        static       bool  is_atoz(char c) { return c >= 'a'&&c <= 'z'; }
        static       bool  is_AtoZ(char c) { return c >= 'A'&&c <= 'Z'; }
    };
    template<> class sc<wchar_t>
    {
    public:
        static const uint32_t char_size(){ return sizeof(wchar_t); }
        static const wchar_t* hex_upr() {return L"0123456789ABCDEF";}
        static const wchar_t* hex_lwr() {return L"0123456789abcdef";}
        static const wchar_t  hex_upr(uint32_t i) {return L"0123456789ABCDEF"[i];}
        static const wchar_t  hex_lwr(uint32_t i) {return L"0123456789abcdef"[i];}
        static const wchar_t  zero() {return L'0';}
        static const wchar_t* empty() {return L"";}
        static const wchar_t  CR() {return L'\r';}
        static const wchar_t  LF() {return L'\n';}
        static const wchar_t  space() {return L' ';}
        static const wchar_t  A() {return L'A';}
        static const wchar_t  F() {return L'F';}
        static const wchar_t  Z() {return L'Z';}
        static const wchar_t  a() {return L'a';}
        static const wchar_t  f() {return L'f';}
        static const wchar_t  x() {return L'x';}
        static const wchar_t  z() {return L'z';}
        static bool  is_atoz(wchar_t c) { return c >= L'a'&&c <= L'z'; }
        static bool  is_AtoZ(wchar_t c) { return c >= L'A'&&c <= L'Z'; }
    };

    //-----------------------------------------------------
    //统一封装了对原始字符串处理的函数
    namespace st
    {
        //-------------------------------------------------
        //长度
        inline uint32_t         strlen(const char* Str) {return (uint32_t)::strlen(Str);}
        inline uint32_t         strlen(const wchar_t* Str) {return (uint32_t)::wcslen(Str);}

        //拷贝
        inline char*            strcpy(char* dst,const char* src) {return ::strcpy(dst,src);}
        inline wchar_t*         strcpy(wchar_t* dst,const wchar_t* src) {return wcscpy(dst,src);}

        //拼接
        inline char*            strcat(char* dst,const char* src) {return ::strcat(dst,src);}
        inline wchar_t*         strcat(wchar_t* dst,const wchar_t* src) {return wcscat(dst,src);}

        //计数拷贝
        inline char*            strncpy(char* dst,const char* src,uint32_t len) {return ::strncpy(dst,src,len);}
        inline wchar_t*         strncpy(wchar_t* dst,const wchar_t* src,uint32_t len) {return ::wcsncpy(dst,src,len);}

        //比较,区分大小写
        inline int              strcmp(const char* dst,const char* src) {return ::strcmp(dst,src);}
        inline int              strcmp(const wchar_t* dst,const wchar_t* src) {return ::wcscmp(dst,src);}

        //比较,不区分大小写
        #if RX_CC==RX_CC_BCC
        inline int              stricmp(const char *s1, const char *s2) {return ::strcmpi(s1,s2);}
        #elif RX_CC==RX_CC_GCC
        inline int              stricmp(const char *s1, const char *s2) {return ::strcasecmp(s1,s2);}
        #else
        inline int              stricmp(const char *s1, const char *s2) {return ::_strcmpi(s1,s2);}
        #endif
        #if RX_CC==RX_CC_GCC
        inline int              stricmp(const wchar_t *s1, const wchar_t *s2) {return ::wcscasecmp(s1,s2);}
        #else
        inline int              stricmp(const wchar_t *s1, const wchar_t *s2) {return ::_wcsicmp(s1,s2);}
        #endif
        //定长比较,区分大小写,规定最大长度

        inline int              strncmp(const char *s1, const char *s2, uint32_t  maxlen) {return ::strncmp(s1,s2,maxlen);}
        inline int              strncmp(const wchar_t *s1, const wchar_t *s2, uint32_t  maxlen) {return ::wcsncmp(s1,s2,maxlen);}

        //定长比较,不区分大小写,规定最大长度
        #if RX_CC==RX_CC_BCC
        inline int              strncmpi(const char *s1, const char *s2, uint32_t n) {return ::strncmpi(s1,s2,n);}
        inline int              strncmpi(const wchar_t *s1, const wchar_t *s2, uint32_t n) {return ::wcsncmpi(s1,s2,n);}
        #elif RX_CC==RX_CC_GCC
        inline int              strncmpi(const char *s1, const char *s2, uint32_t n) {return ::strncasecmp(s1,s2,n);}
        inline int              strncmpi(const wchar_t *s1, const wchar_t *s2, uint32_t n) {return ::wcsncasecmp(s1,s2,n);}
        #else
        inline int              strncmpi(const char *s1, const char *s2, uint32_t n) {return ::_strnicmp(s1,s2,n);}
        inline int              strncmpi(const wchar_t *s1, const wchar_t *s2, uint32_t n) {return ::_wcsnicmp(s1,s2,n);}
        #endif

        //子串搜索
        inline char *           strstr(char *s1, const char *s2) {return ::strstr(s1,s2);}
        inline wchar_t*         strstr(wchar_t *s1, const wchar_t *s2) {return ::wcsstr(s1,s2);}
        inline const char *     strstr(const char *s1, const char *s2) {return ::strstr(s1,s2);}
        inline const wchar_t*   strstr(const wchar_t *s1, const wchar_t *s2) {return ::wcsstr(s1,s2);}

        //字符搜索
        inline char *           strchr(char *s, int c) {return ::strchr(s,c);}
        inline wchar_t *        strchr(wchar_t *s, wchar_t c) { return ::wcschr(s, c); }
        inline const char *     strchr(const char *s, int c) {return ::strchr(s,c);}
        inline const wchar_t *  strchr(const wchar_t *s, wchar_t c) { return ::wcschr(s, c); }

        //反向字符搜索
        inline char *           strrchr(char *s, int c) {return ::strrchr(s,c);}
        inline wchar_t *        strrchr(wchar_t *s, int c) {return ::wcsrchr(s,c);}
        inline const char *     strrchr(const char *s, int c) {return ::strrchr(s,c);}
        inline const wchar_t *  strrchr(const wchar_t *s, int c) {return ::wcsrchr(s,c);}

        //字符串转为无符号整数
        inline uint32_t         atoul(const char* s,int radix=0) {return ::strtoul(s,NULL,radix);}
        inline uint32_t         atoul(const wchar_t* s,int radix=0) {return ::wcstoul(s,NULL,radix);}
        template<class CT>
        inline uint32_t         atoul(const CT* s,int radix,uint32_t DefVal) {if (is_empty(s)) return DefVal; else return atoul(s,radix);}

        //字符串转换为整数
        inline int              atoi(const char* s,int radix=0) {return ::strtol(s,NULL,radix);}
        inline int              atoi(const wchar_t* s,int radix=0) {return ::wcstol(s,NULL,radix);}
        template<class CT>
        inline int              atoi(const CT* s,int radix,int DefVal) {if (is_empty(s))return DefVal; else return atoi(s,radix);}

        #if RX_CC==RX_CC_GCC
        //字符串转换为整数
        inline uint64_t         atoi64(const char* s) {return ::strtoll(s,NULL,10);}
        inline uint64_t         atoi64(const wchar_t* s) {return ::wcstoll(s,NULL,10);}
        #else
        //字符串转换为整数
        inline uint64_t         atoi64(const char* s) {return ::_atoi64(s);}
        inline uint64_t         atoi64(const wchar_t* s) {return ::_wtoi64(s);}
        #endif

        //字符串转换为浮点数
        inline double           atof(const char* s) {return ::atof(s);}
        inline double           atof(const wchar_t* s) {return ::wcstod(s,NULL);}

        //数字转换为字符串

        #if RX_CC==RX_CC_BCC
        inline char *           itoa(int value, char *string, int radix=10) {return ::itoa(value,string,radix);}
        #elif RX_CC==RX_CC_GCC
        inline char *           itoa(int value, char *string, int radix=10)
        {
            const char* fmt="%d";
            if (radix==16) fmt="%x";
            sprintf(string,fmt,value);
            return string;
        }
        #else
        inline char *           itoa(int value, char *string, int radix=10) {return ::_itoa(value,string,radix);}
        #endif

        #if RX_CC==RX_CC_GCC
        inline wchar_t *           itoa(int value, wchar_t *string, int radix=10)
        {
            const wchar_t* fmt=L"%d";
            if (radix==16) fmt=L"%x";
            wprintf(string,fmt,value);
            return string;
        }
        #else
        inline wchar_t *        itoa(int value, wchar_t *string, int radix=10) {return ::_itow(value,string,radix);}
        #endif


        #if RX_CC==RX_CC_GCC
        inline char *           itoa64(int value, char *string, int radix=10)
        {
            const char* fmt="%lld";
            if (radix==16) fmt="%llx";
            sprintf(string,fmt,value);
            return string;
        }
        inline wchar_t *           itoa64(int value, wchar_t *string, int radix=10)
        {
            const wchar_t* fmt=L"%lld";
            if (radix==16) fmt=L"%llx";
            wprintf(string,fmt,value);
            return string;
        }
        #else
        inline char *           itoa64(int64_t value, char *string, int radix=10) {return ::_i64toa(value,string,radix);}
        inline wchar_t *        itoa64(int64_t value, wchar_t *string, int radix=10) {return ::_i64tow(value,string,radix);}
        #endif


        #if RX_CC==RX_CC_BCC
        inline char *           ultoa(uint32_t value, char *string, int radix=10) {return ::ultoa(value,string,radix);}
        #elif RX_CC==RX_CC_GCC
        inline char *           ultoa(uint32_t value, char *string, int radix=10)
        {
            const char* fmt="%u";
            if (radix==16) fmt="%x";
            sprintf(string,fmt,value);
            return string;
        }
        #else
        inline char *           ultoa(uint32_t value, char *string, int radix=10) {return ::_ultoa(value,string,radix);}
        #endif

        #if RX_CC==RX_CC_GCC
        inline wchar_t *        ultoa(uint32_t value, wchar_t *string, int radix=10)
        {
            const wchar_t* fmt=L"%u";
            if (radix==16) fmt=L"%x";
            wprintf(string,fmt,value);
            return string;
        }
        #else
        inline wchar_t *        ultoa(uint32_t value, wchar_t *string, int radix=10) {return ::_ultow(value,string,radix);}
        #endif

        //语法糖,数字转为0x前缀的十六进制串
        template<class CT>
        inline CT*              itox(int value,CT* Buff) {Buff[0]=sc<CT>::zero(); Buff[1]=sc<CT>::x(); itoa(value,&Buff[2],16); return Buff;}
        template<class CT>
        inline CT*              itox64(uint64_t value,CT* Buff) {Buff[0]=sc<CT>::zero(); Buff[1]=sc<CT>::x(); itoa64(value,&Buff[2],16); return Buff;}
        template<class CT>
        inline CT*              ultox(uint32_t value,CT* Buff) {Buff[0]=sc<CT>::zero(); Buff[1]=sc<CT>::x(); ultoa(value,&Buff[2],16); return Buff;}

        //浮点数转为字符串
        inline char*            ftoa(const double& f,char* string,const char* Fmt=NULL) {if (!Fmt) Fmt="%.2f"; sprintf(string,Fmt,f); return string;}
        inline wchar_t*         ftoa(const double& f,wchar_t* string,const wchar_t* Fmt=NULL) {if (!Fmt) Fmt=L"%.2f"; wprintf(string,Fmt,f); return string;}

        //字符串格式化
        //返回值:<0错误;>=0为输出内容长度,需要与BufSize进行比较
        inline int              snprintf(char * Buf,uint32_t BufSize, const char *Fmt,...)
        {
            va_list ap;
            va_start(ap, Fmt);
            int ret = ::vsnprintf(Buf,BufSize,Fmt,ap);
            va_end(ap);
            return ret;
        }
        inline int              snprintf(wchar_t * Buf,uint32_t BufSize, const wchar_t *Fmt,...)
        {
            va_list ap;
            va_start(ap, Fmt);
            int ret = ::vswprintf(Buf,BufSize,Fmt,ap);
            va_end(ap);
            return ret;
        }
        inline int              vsnprintf(char * Buf,uint32_t BufSize, const char *Fmt,va_list arglist) {return ::vsnprintf(Buf,BufSize,Fmt,arglist);}
        inline int              vsnprintf(wchar_t * Buf,uint32_t BufSize, const wchar_t *Fmt,va_list arglist) {return ::vswprintf(Buf,BufSize,Fmt,arglist);}

        #if RX_STR_USE_FILE
        inline int              fprintf(FILE *stream, const char *format,...)
        {
            va_list ap;
            va_start(ap, format);
            int ret = vfprintf(stream,format,ap);
            va_end(ap);
            return ret;
        }
        inline int              fprintf(FILE *stream, const wchar_t *format,...)
        {
            va_list ap;
            va_start(ap, format);
            int ret = vfwprintf(stream,format,ap);
            va_end(ap);
            return ret;
        }
        inline int              vfprintf(FILE *stream, const char *format,va_list ap) {return ::vfprintf(stream,format,ap);}
        inline int              vfprintf(FILE *stream, const wchar_t *format,va_list ap) {return ::vfwprintf(stream,format,ap);}
        #endif
    }
}


#endif
