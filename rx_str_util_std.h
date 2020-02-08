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



#if RX_CC==RX_CC_GCC && RX_CC_VER_MAJOR>=5
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wstringop-truncation"
    #pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif

namespace rx
{
    //-----------------------------------------------------
    //�ַ�����س�����װ
    template<class CT> class sc;
    template<> class sc<char>
    {
    public:
        static const uint32_t char_size() { return sizeof(char); }
        static const char* hex_upr() {return "0123456789ABCDEF";}
        static const char* hex_lwr() {return "0123456789abcdef";}
        static const char  hex_upr(uint32_t i) {return "0123456789ABCDEF"[i];}
        static const char  hex_lwr(uint32_t i) {return "0123456789abcdef"[i];}
        static const char  zero = '0';
        static const char* empty() {return "";}
        static const char  CR = '\r';
        static const char  LF = '\n';
        static const char  space = ' ';
        static const char  A = 'A';
        static const char  F = 'F';
        static const char  Z = 'Z';
        static const char  a = 'a';
        static const char  f = 'f';
        static const char  x = 'x';
        static const char  z = 'z';
        static const char  l = 'l';
        static const char  h = 'h';
        static const char  t = 't';
        static const char  j = 'j';
        static const char  d = 'd';
        static const char  i = 'i';
        static const char  u = 'u';
        static const char  X = 'X';
        static const char  o = 'o';
        static const char  b = 'b';
        static const char  e = 'e';
        static const char  E = 'E';
        static const char  g = 'g';
        static const char  G = 'G';
        static const char  c = 'c';
        static const char  s = 's';
        static const char  p = 'p';
        static const char* nan() {return "nan";}
        static const char* fni() {return "fni";}
        static const char* fni_minus() {return "fni-";}
        static const char* fni_plus() {return "fni+";}
        static const char  sharp = '#';
        static const char  plus = '+';
        static const char  minus = '-';
        static const char  percent = '%';
        static const char  star = '*';
        static const char  dot = '.';
        static       bool  is_atoz(char c) { return c >= 'a'&&c <= 'z'; }
        static       bool  is_AtoZ(char c) { return c >= 'A'&&c <= 'Z'; }
        static       bool  is_atof(char c) { return c >= 'a'&&c <= 'f'; }
        static       bool  is_AtoF(char c) { return c >= 'A'&&c <= 'F'; }
        static       bool  is_0to9(char c) { return c >= '0'&&c <= '9'; }
    };
    template<> class sc<wchar_t>
    {
    public:
        static const uint32_t char_size(){ return sizeof(wchar_t); }
        static const wchar_t* hex_upr() {return L"0123456789ABCDEF";}
        static const wchar_t* hex_lwr() {return L"0123456789abcdef";}
        static const wchar_t  hex_upr(uint32_t i) {return L"0123456789ABCDEF"[i];}
        static const wchar_t  hex_lwr(uint32_t i) {return L"0123456789abcdef"[i];}
        static const wchar_t  zero = L'0';
        static const wchar_t* empty() {return L"";}
        static const wchar_t  CR = L'\r';
        static const wchar_t  LF = L'\n';
        static const wchar_t  space = L' ';
        static const wchar_t  A = L'A';
        static const wchar_t  F = L'F';
        static const wchar_t  Z = L'Z';
        static const wchar_t  a = L'a';
        static const wchar_t  f = L'f';
        static const wchar_t  x = L'x';
        static const wchar_t  z = L'z';
        static const wchar_t  l = L'l';
        static const wchar_t  h = L'h';
        static const wchar_t  t = L't';
        static const wchar_t  j = L'j';
        static const wchar_t  d = L'd';
        static const wchar_t  i = L'i';
        static const wchar_t  u = L'u';
        static const wchar_t  X = L'X';
        static const wchar_t  o = L'o';
        static const wchar_t  b = L'b';
        static const wchar_t  e = L'e';
        static const wchar_t  E = L'E';
        static const wchar_t  g = L'g';
        static const wchar_t  G = L'G';
        static const wchar_t  c = L'c';
        static const wchar_t  s = L's';
        static const wchar_t  p = L'p';
        static const wchar_t* nan() {return L"nan";}
        static const wchar_t* fni() {return L"fni";}
        static const wchar_t* fni_minus() {return L"fni-";}
        static const wchar_t* fni_plus() {return L"fni+";}
        static const wchar_t  sharp = L'#';
        static const wchar_t  plus = L'+';
        static const wchar_t  minus = L'-';
        static const wchar_t  percent = L'%';
        static const wchar_t  star = L'*';
        static const wchar_t  dot = L'.';
        static       bool     is_atoz(wchar_t c) { return c >= L'a'&&c <= L'z'; }
        static       bool     is_AtoZ(wchar_t c) { return c >= L'A'&&c <= L'Z'; }
        static       bool     is_atof(wchar_t c) { return c >= L'a'&&c <= L'f'; }
        static       bool     is_AtoF(wchar_t c) { return c >= L'A'&&c <= L'F'; }
        static       bool     is_0to9(wchar_t c) { return c >= L'0'&&c <= L'9'; }
    };

    //-----------------------------------------------------
    //ͳһ��װ�˶�ԭʼ�ַ�������ĺ���
    namespace st
    {
        //-------------------------------------------------
        //����
        inline uint32_t         strlen(const char* Str) { return Str == NULL ? 0 : (uint32_t)::strlen(Str); }
        inline uint32_t         strlen(const wchar_t* Str) {return Str == NULL ? 0 : (uint32_t)::wcslen(Str);}

        //����
        inline char*            strcpy(char* dst,const char* src) {return ::strcpy(dst,src);}
        inline wchar_t*         strcpy(wchar_t* dst,const wchar_t* src) {return wcscpy(dst,src);}

        //ƴ��
        inline char*            strcat(char* dst,const char* src) {return ::strcat(dst,src);}
        inline wchar_t*         strcat(wchar_t* dst,const wchar_t* src) {return wcscat(dst,src);}

        //��������
        inline char*            strncpy(char* dst,const char* src,uint32_t len) {return ::strncpy(dst,src,len);}
        inline wchar_t*         strncpy(wchar_t* dst,const wchar_t* src,uint32_t len) {return ::wcsncpy(dst,src,len);}

        //�Ƚ�,���ִ�Сд
        inline int              strcmp(const char* dst,const char* src) {return ::strcmp(dst,src);}
        inline int              strcmp(const wchar_t* dst,const wchar_t* src) {return ::wcscmp(dst,src);}

        //�Ƚ�,�����ִ�Сд
        #if RX_CC==RX_CC_BCC
        inline int              stricmp(const char *s1, const char *s2) {return ::strcmpi(s1,s2);}
        #elif RX_CC==RX_CC_GCC
        inline int              stricmp(const char *s1, const char *s2) {return ::strcasecmp(s1,s2);}
        #else
        inline int              stricmp(const char *s1, const char *s2) {return ::_strcmpi(s1,s2);}
        #endif
        #if RX_CC==RX_CC_GCC && RX_CC_VER_MAJOR<9
        inline int              stricmp(const wchar_t *s1, const wchar_t *s2) {return ::wcscasecmp(s1,s2);}
        #else
        inline int              stricmp(const wchar_t *s1, const wchar_t *s2) {return ::_wcsicmp(s1,s2);}
        #endif
        //�����Ƚ�,���ִ�Сд,�涨��󳤶�

        inline int              strncmp(const char *s1, const char *s2, uint32_t  maxlen) {return ::strncmp(s1,s2,maxlen);}
        inline int              strncmp(const wchar_t *s1, const wchar_t *s2, uint32_t  maxlen) {return ::wcsncmp(s1,s2,maxlen);}

        //�����Ƚ�,�����ִ�Сд,�涨��󳤶�
        #if RX_CC==RX_CC_BCC
        inline int              strncmpi(const char *s1, const char *s2, uint32_t n) {return ::strncmpi(s1,s2,n);}
        inline int              strncmpi(const wchar_t *s1, const wchar_t *s2, uint32_t n) {return ::wcsncmpi(s1,s2,n);}
        #elif RX_CC==RX_CC_GCC && RX_CC_VER_MAJOR<9
        inline int              strncmpi(const char *s1, const char *s2, uint32_t n) {return ::strncasecmp(s1,s2,n);}
        inline int              strncmpi(const wchar_t *s1, const wchar_t *s2, uint32_t n) {return ::wcsncasecmp(s1,s2,n);}
        #else
        inline int              strncmpi(const char *s1, const char *s2, uint32_t n) {return ::_strnicmp(s1,s2,n);}
        inline int              strncmpi(const wchar_t *s1, const wchar_t *s2, uint32_t n) {return ::_wcsnicmp(s1,s2,n);}
        #endif

        //�Ӵ�����
        inline char *           strstr(char *s1, const char *s2) {return ::strstr(s1,s2);}
        inline wchar_t*         strstr(wchar_t *s1, const wchar_t *s2) {return ::wcsstr(s1,s2);}
        inline const char *     strstr(const char *s1, const char *s2) {return ::strstr(s1,s2);}
        inline const wchar_t*   strstr(const wchar_t *s1, const wchar_t *s2) {return ::wcsstr(s1,s2);}

        //�ַ�����
        inline char *           strchr(char *s, int c) {return ::strchr(s,c);}
        inline wchar_t *        strchr(wchar_t *s, wchar_t c) { return ::wcschr(s, c); }
        inline const char *     strchr(const char *s, int c) {return ::strchr(s,c);}
        inline const wchar_t *  strchr(const wchar_t *s, wchar_t c) { return ::wcschr(s, c); }

        //�����ַ�����
        inline char *           strrchr(char *s, int c) {return ::strrchr(s,c);}
        inline wchar_t *        strrchr(wchar_t *s, int c) {return ::wcsrchr(s,c);}
        inline const char *     strrchr(const char *s, int c) {return ::strrchr(s,c);}
        inline const wchar_t *  strrchr(const wchar_t *s, int c) {return ::wcsrchr(s,c);}

        //�ַ���תΪ�޷�������
        inline uint32_t         atoul(const char* s,int radix=0) {return ::strtoul(s,NULL,radix);}
        inline uint32_t         atoul(const wchar_t* s,int radix=0) {return ::wcstoul(s,NULL,radix);}
        template<class CT>
        inline uint32_t         atoul(const CT* s,int radix,uint32_t DefVal) {if (is_empty(s)) return DefVal; else return atoul(s,radix);}

        //�ַ���ת��Ϊ����
        inline int              atoi(const char* s,int radix=0) {return ::strtol(s,NULL,radix);}
        inline int              atoi(const wchar_t* s,int radix=0) {return ::wcstol(s,NULL,radix);}
        template<class CT>
        inline int              atoi(const CT* s,int radix,int DefVal) {if (is_empty(s))return DefVal; else return atoi(s,radix);}

        #if RX_CC==RX_CC_GCC
        //�ַ���ת��Ϊ����
        inline uint64_t         atoi64(const char* s) {return ::strtoll(s,NULL,10);}
        inline uint64_t         atoi64(const wchar_t* s) {return ::wcstoll(s,NULL,10);}
        #else
        //�ַ���ת��Ϊ����
        inline uint64_t         atoi64(const char* s) {return ::_atoi64(s);}
        inline uint64_t         atoi64(const wchar_t* s) {return ::_wtoi64(s);}
        #endif

        //�ַ���ת��Ϊ������
        inline double           atof(const char* s) {return ::atof(s);}
        inline double           atof(const wchar_t* s) {return ::wcstod(s,NULL);}

        //����ת��Ϊ�ַ���,�μ�rx_str_util_fmt.h�е�ntoaϵ�к���

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

#if RX_CC==RX_CC_GCC && RX_CC_VER_MAJOR>=5
    #pragma GCC diagnostic pop
#endif

#endif
