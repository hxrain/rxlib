#ifndef _RX_STR_UTIL_H_
#define _RX_STR_UTIL_H_

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
    class st
    {
    public:
        //-----------------------------------------------------
        //长度
        static uint32_t         strlen(const char* Str) {return (uint32_t)::strlen(Str);}
        static uint32_t         strlen(const wchar_t* Str) {return (uint32_t)::wcslen(Str);}

        //拷贝
        static char*            strcpy(char* dst,const char* src) {return ::strcpy(dst,src);}
        static wchar_t*         strcpy(wchar_t* dst,const wchar_t* src) {return wcscpy(dst,src);}

        //拼接
        static char*            strcat(char* dst,const char* src) {return ::strcat(dst,src);}
        static wchar_t*         strcat(wchar_t* dst,const wchar_t* src) {return wcscat(dst,src);}

        //计数拷贝
        static char*            strncpy(char* dst,const char* src,uint32_t len) {return ::strncpy(dst,src,len);}
        static wchar_t*         strncpy(wchar_t* dst,const wchar_t* src,uint32_t len) {return ::wcsncpy(dst,src,len);}

        //尾部拷贝(取src的后n位)
        template<class CT>
        static CT*              strrcpy(CT* dst,const CT* src,uint32_t n,uint32_t srclen=0)
        {
            if (!srclen)
                srclen=strlen(src);
            if (n>srclen)
                n=srclen;
            uint32_t P=n-srclen;
            return strncpy(dst,&src[P],n);
        }

        //比较,区分大小写
        static int              strcmp(const char* dst,const char* src) {return ::strcmp(dst,src);}
        static int              strcmp(const wchar_t* dst,const wchar_t* src) {return ::wcscmp(dst,src);}

        //比较,不区分大小写
        #if RX_CC==RX_CC_BCC
        static int              stricmp(const char *s1, const char *s2) {return ::strcmpi(s1,s2);}
        #elif RX_CC==RX_CC_GCC
        static int              stricmp(const char *s1, const char *s2) {return ::strcasecmp(s1,s2);}
        #else
        static int              stricmp(const char *s1, const char *s2) {return ::_strcmpi(s1,s2);}
        #endif
        #if RX_CC==RX_CC_GCC
        static int              stricmp(const wchar_t *s1, const wchar_t *s2) {return ::wcscasecmp(s1,s2);}
        #else
        static int              stricmp(const wchar_t *s1, const wchar_t *s2) {return ::_wcsicmp(s1,s2);}
        #endif
        //定长比较,区分大小写,规定最大长度

        static int              strncmp(const char *s1, const char *s2, uint32_t  maxlen) {return ::strncmp(s1,s2,maxlen);}
        static int              strncmp(const wchar_t *s1, const wchar_t *s2, uint32_t  maxlen) {return ::wcsncmp(s1,s2,maxlen);}

        //定长比较,不区分大小写,规定最大长度
        #if RX_CC==RX_CC_BCC
        static int              strncmpi(const char *s1, const char *s2, uint32_t n) {return ::strncmpi(s1,s2,n);}
        static int              strncmpi(const wchar_t *s1, const wchar_t *s2, uint32_t n) {return ::wcsncmpi(s1,s2,n);}
        #elif RX_CC==RX_CC_GCC
        static int              strncmpi(const char *s1, const char *s2, uint32_t n) {return ::strncasecmp(s1,s2,n);}
        static int              strncmpi(const wchar_t *s1, const wchar_t *s2, uint32_t n) {return ::wcsncasecmp(s1,s2,n);}
        #else
        static int              strncmpi(const char *s1, const char *s2, uint32_t n) {return ::_strnicmp(s1,s2,n);}
        static int              strncmpi(const wchar_t *s1, const wchar_t *s2, uint32_t n) {return ::_wcsnicmp(s1,s2,n);}
        #endif

        //子串搜索
        static char *           strstr(char *s1, const char *s2) {return ::strstr(s1,s2);}
        static wchar_t*         strstr(wchar_t *s1, const wchar_t *s2) {return ::wcsstr(s1,s2);}
        static const char *     strstr(const char *s1, const char *s2) {return ::strstr(s1,s2);}
        static const wchar_t*   strstr(const wchar_t *s1, const wchar_t *s2) {return ::wcsstr(s1,s2);}

        //字符搜索
        static char *           strchr(char *s, int c) {return ::strchr(s,c);}
        static wchar_t *        strchr(wchar_t *s, wchar_t c) { return ::wcschr(s, c); }
        static const char *     strchr(const char *s, int c) {return ::strchr(s,c);}
        static const wchar_t *  strchr(const wchar_t *s, wchar_t c) { return ::wcschr(s, c); }

        //反向字符搜索
        static char *           strrchr(char *s, int c) {return ::strrchr(s,c);}
        static wchar_t *        strrchr(wchar_t *s, int c) {return ::wcsrchr(s,c);}
        static const char *     strrchr(const char *s, int c) {return ::strrchr(s,c);}
        static const wchar_t *  strrchr(const wchar_t *s, int c) {return ::wcsrchr(s,c);}

        //字符串转换为大写
        template<class CT>  static CT *strupr(CT *str)
        {
            for(CT* s=str;*s;++s)
            {
                if(sc<CT>::is_atoz(*s))
                    *s -= sc<CT>::a()-sc<CT>::A();
            }
            return str;
        }
        template<class CT>  static uint32_t strupr(CT *buff,uint32_t buffsize,const CT *str)
        {
            uint32_t len = 0;
            for (CT* s = str; *s&&len < buffsize; ++s,++len)
            {
                if (sc<CT>::is_atoz(*s))
                    buff[len] = *s - (sc<CT>::a() - sc<CT>::A());
                else
                    buff[len] = *s;
            }
            buff[len] = 0;
            return len;
        }

        //字符串转换为小写
        template<class CT>  static CT *strlwr(CT *str)
        {
            for(CT* s=str;*s;++s)
            {
                if(sc<CT>::is_AtoZ(*s))
                    *s += sc<CT>::a()-sc<CT>::A();
            }
            return str;
        }
        template<class CT>  static uint32_t strlwr(CT *buff, uint32_t buffsize, const CT *str)
        {
            --buffsize;
            uint32_t len = 0;
            for (CT* s = str; *s&&len < buffsize; ++s, ++len)
            {
                if (sc<CT>::is_AtoZ(*s))
                    buff[len] = *s + (sc<CT>::a() - sc<CT>::A());
                else
                    buff[len] = *s;
            }
            buff[len] = 0;
            return len;
        }

        //字符串转换扩展,直接拷贝到目标缓冲器
        template<class CT>
        static CT *             strupr(const CT *s,CT* TmpBuf) {strcpy(TmpBuf,s); return strupr(TmpBuf);}
        template<class CT>
        static CT *             strlwr(const CT *s,CT* TmpBuf) {strcpy(TmpBuf,s); return strlwr(TmpBuf);}

        //字符串转为无符号整数
        static uint32_t         atoul(const char* s,int radix=0) {return ::strtoul(s,NULL,radix);}
        static uint32_t         atoul(const wchar_t* s,int radix=0) {return ::wcstoul(s,NULL,radix);}
        template<class CT>
        static uint32_t         atoul(const CT* s,int radix,uint32_t DefVal) {if (is_empty(s)) return DefVal; else return atoul(s,radix);}

        //字符串转换为整数
        static int              atoi(const char* s,int radix=0) {return ::strtol(s,NULL,radix);}
        static int              atoi(const wchar_t* s,int radix=0) {return ::wcstol(s,NULL,radix);}
        template<class CT>
        static int              atoi(const CT* s,int radix,int DefVal) {if (is_empty(s))return DefVal; else return atoi(s,radix);}

        #if RX_CC==RX_CC_GCC
        //字符串转换为整数
        static uint64_t         atoi64(const char* s) {return ::strtoll(s,NULL,10);}
        static uint64_t         atoi64(const wchar_t* s) {return ::wcstoll(s,NULL,10);}
        #else
        //字符串转换为整数
        static uint64_t         atoi64(const char* s) {return ::_atoi64(s);}
        static uint64_t         atoi64(const wchar_t* s) {return ::_wtoi64(s);}
        #endif

        //字符串转换为浮点数
        static double           atof(const char* s) {return ::atof(s);}
        static double           atof(const wchar_t* s) {return ::wcstod(s,NULL);}

        //数字转换为字符串

        #if RX_CC==RX_CC_BCC
        static char *           itoa(int value, char *string, int radix=10) {return ::itoa(value,string,radix);}
        #elif RX_CC==RX_CC_GCC
        static char *           itoa(int value, char *string, int radix=10)
        {
            const char* fmt="%d";
            if (radix==16) fmt="%x";
            sprintf(string,fmt,value);
            return string;
        }
        #else
        static char *           itoa(int value, char *string, int radix=10) {return ::_itoa(value,string,radix);}
        #endif

        #if RX_CC==RX_CC_GCC
        static wchar_t *           itoa(int value, wchar_t *string, int radix=10)
        {
            const wchar_t* fmt=L"%d";
            if (radix==16) fmt=L"%x";
            wprintf(string,fmt,value);
            return string;
        }
        #else
        static wchar_t *        itoa(int value, wchar_t *string, int radix=10) {return ::_itow(value,string,radix);}
        #endif


        #if RX_CC==RX_CC_GCC
        static char *           itoa64(int value, char *string, int radix=10)
        {
            const char* fmt="%lld";
            if (radix==16) fmt="%llx";
            sprintf(string,fmt,value);
            return string;
        }
        static wchar_t *           itoa64(int value, wchar_t *string, int radix=10)
        {
            const wchar_t* fmt=L"%lld";
            if (radix==16) fmt=L"%llx";
            wprintf(string,fmt,value);
            return string;
        }
        #else
        static char *           itoa64(int64_t value, char *string, int radix=10) {return ::_i64toa(value,string,radix);}
        static wchar_t *        itoa64(int64_t value, wchar_t *string, int radix=10) {return ::_i64tow(value,string,radix);}
        #endif


        #if RX_CC==RX_CC_BCC
        static char *           ultoa(uint32_t value, char *string, int radix=10) {return ::ultoa(value,string,radix);}
        #elif RX_CC==RX_CC_GCC
        static char *           ultoa(uint32_t value, char *string, int radix=10)
        {
            const char* fmt="%u";
            if (radix==16) fmt="%x";
            sprintf(string,fmt,value);
            return string;
        }
        #else
        static char *           ultoa(uint32_t value, char *string, int radix=10) {return ::_ultoa(value,string,radix);}
        #endif

        #if RX_CC==RX_CC_GCC
        static wchar_t *        ultoa(uint32_t value, wchar_t *string, int radix=10)
        {
            const wchar_t* fmt=L"%u";
            if (radix==16) fmt=L"%x";
            wprintf(string,fmt,value);
            return string;
        }
        #else
        static wchar_t *        ultoa(uint32_t value, wchar_t *string, int radix=10) {return ::_ultow(value,string,radix);}
        #endif

        //语法糖,数字转为0x前缀的十六进制串
        template<class CT>
        static CT*              itox(int value,CT* Buff) {Buff[0]=sc<CT>::zero(); Buff[1]=sc<CT>::x(); itoa(value,&Buff[2],16); return Buff;}
        template<class CT>
        static CT*              itox64(uint64_t value,CT* Buff) {Buff[0]=sc<CT>::zero(); Buff[1]=sc<CT>::x(); itoa64(value,&Buff[2],16); return Buff;}
        template<class CT>
        static CT*              ultox(uint32_t value,CT* Buff) {Buff[0]=sc<CT>::zero(); Buff[1]=sc<CT>::x(); ultoa(value,&Buff[2],16); return Buff;}

        //浮点数转为字符串
        static char*            ftoa(const double& f,char* string,const char* Fmt=NULL) {if (!Fmt) Fmt="%.2f"; sprintf(string,Fmt,f); return string;}
        static wchar_t*         ftoa(const double& f,wchar_t* string,const wchar_t* Fmt=NULL) {if (!Fmt) Fmt=L"%.2f"; wprintf(string,Fmt,f); return string;}

        //判断是否为十进制数字字符
        static bool             isnumber(char c) {return c>='0'&&c<='9';}
        static bool             isnumber(wchar_t c) {return c>=L'0'&&c<=L'9';}
        template<class CT>
        static bool             isnumber(const CT* s)
        {
            if (s==NULL)
                return false;
            for(; *s; ++s)
                if (!isnumber(*s))
                    return false;
            return true;
        }
        //判断是否为十进制数字和字母
        static bool             isalnum(char c) {return (c>='0'&&c<='9')||(c>='A'&&c<='Z')||(c>='a'&&c<='z');}
        static bool             isalnum(wchar_t c) {return (c>=L'0'&&c<=L'9')||(c>=L'A'&&c<=L'Z')||(c>=L'a'&&c<=L'z');}

        //字符串格式化
        //返回值:<0错误;>=0为输出内容长度
        static int              snprintf(char * Buf,uint32_t BufSize, const char *Fmt,...)
        {
            va_list ap;
            va_start(ap, Fmt);
            return ::vsnprintf(Buf,BufSize,Fmt,ap);
        }
        static int              snprintf(wchar_t * Buf,uint32_t BufSize, const wchar_t *Fmt,...)
        {
            va_list ap;
            va_start(ap, Fmt);
            return ::vswprintf(Buf,BufSize,Fmt,ap);
        }
        static int              vsnprintf(char * Buf,uint32_t BufSize, const char *Fmt,va_list arglist) {return ::vsnprintf(Buf,BufSize,Fmt,arglist);}
        static int              vsnprintf(wchar_t * Buf,uint32_t BufSize, const wchar_t *Fmt,va_list arglist) {return ::vswprintf(Buf,BufSize,Fmt,arglist);}

        #if RX_STR_USE_FILE
        static int              fprintf(FILE *stream, const char *format,...)
        {
            va_list ap;
            va_start(ap, format);
            return vfprintf(stream,format,ap);
        }
        static int              fprintf(FILE *stream, const wchar_t *format,...)
        {
            va_list ap;
            va_start(ap, format);
            return vfwprintf(stream,format,ap);
        }
        static int              vfprintf(FILE *stream, const char *format,va_list ap) {return ::vfprintf(stream,format,ap);}
        static int              vfprintf(FILE *stream, const wchar_t *format,va_list ap) {return ::vfwprintf(stream,format,ap);}
        #endif
        //-------------------------------------------------
        //语法糖,对输入S进行判断,如果是空值则返回给定的默认值
        template<class CT>
        static const CT* value(const CT* S,const CT* DefValue)
        {
            if (is_empty(S))
                return DefValue;
            return S;
        }
        //-----------------------------------------------------
        //将原Src串拼接到Dest(已有内容)中,Dest最大容量为DestMaxSize,Src如果太长就失败了.
        //返回值:0失败;或新串的完整长度
        template<class CT>
        static uint32_t strcat(CT *Dest,uint32_t DestMaxSize,const CT* Src,uint32_t SrcLen=0)
        {
            uint32_t DestLen=strlen(Dest);                  //计算得到目标现在已有的串长度
            if (SrcLen==0)
                SrcLen=strlen(Src);              //计算得到原串现在已有的长度
            uint32_t DestRemainLen=DestMaxSize-DestLen;     //计算目标可用容量
            if (SrcLen>DestRemainLen-1)
                return 0;           //空间不足,不能连接了
            strncpy(&Dest[DestLen],Src,SrcLen);             //拼接拷贝
            uint32_t NewLen=SrcLen+DestLen;                 //目标现在的长度
            Dest[NewLen]=0;                                 //确保目标正确结束
            return NewLen;
        }

        //-----------------------------------------------------
        //将原Src串拼接到Dest(已有内容)中,Dest最大容量为DestMaxSize,Src如果太长就只能拼接一部分.
        //返回值:新串的完整长度
        template<class CT>
        static uint32_t strcat2(CT *Dest,uint32_t DestMaxSize,const CT* Src,uint32_t SrcLen=0)
        {
            uint32_t DestLen=strlen(Dest);                  //计算得到目标现在已有的串长度
            if (SrcLen==0)
                SrcLen=strlen(Src);                         //计算得到原串现在已有的长度
            uint32_t DestRemainLen=DestMaxSize-DestLen-1;   //计算目标可用容量
            if (SrcLen>DestRemainLen)
                SrcLen=DestRemainLen;
            strncpy(&Dest[DestLen],Src,SrcLen);			    //拼接拷贝
            uint32_t NewLen=SrcLen+DestLen;					//目标现在的长度
            Dest[NewLen]=0;                                 //确保目标正确结束
            return NewLen;
        }
        //-----------------------------------------------------
        //串连接,将Src1和Src2都拼装到Dest中.
        //返回值:0失败;其他为新结果的长度
        template<class CT>
        static uint32_t strcat(CT *Dest,uint32_t DestMaxSize,const CT* Src1,const CT* Src2)
        {
            uint32_t DestLen=strcpy(Dest,DestMaxSize,Src1);
            uint32_t Src2Len=strlen(Src2);                  //计算得到原串现在已有的长度
            uint32_t DestRemainLen=DestMaxSize-DestLen;     //计算目标可用容量
            if (Src2Len>DestRemainLen-1)
                return 0;          //空间不足,不能连接了
            strncpy(&Dest[DestLen],Src2,Src2Len);           //拼接拷贝
            uint32_t NewLen=Src2Len+DestLen;                //目标现在的长度
            Dest[NewLen]=0;                                 //确保目标正确结束
            return NewLen;
        }
        //-----------------------------------------------------
        //将原Src串拼接到Dest(已有内容)中,Dest最大容量为DestMaxSize,Src如果太长就失败了.
        //返回值:0失败;或新串的完整长度
        template<class CT>
        static uint32_t strcat(CT *Dest,uint32_t &DestLen,uint32_t DestMaxSize,const CT* Src,uint32_t SrcLen=0)
        {
            if (SrcLen==0)
                SrcLen=strlen(Src);              //计算得到原串现在已有的长度
            uint32_t DestRemainLen=DestMaxSize-DestLen;     //计算目标可用容量
            if (SrcLen>DestRemainLen-1)
                return 0;           //空间不足,不能连接了
            strncpy(&Dest[DestLen],Src,SrcLen);             //拼接拷贝
            DestLen+=SrcLen;                                //目标现在的长度
            Dest[DestLen]=0;                                //确保目标正确结束
            return DestLen;
        }
        template<class CT>
        static uint32_t strcat(CT *Dest,uint32_t &DestLen,uint32_t DestMaxSize,CT Src)
        {
            uint32_t DestRemainLen=DestMaxSize-DestLen;     //计算目标可用容量
            if (DestRemainLen<2)
                return 0;                //空间不足,不能连接了
            Dest[DestLen]=Src;                              //拼接拷贝
            Dest[++DestLen]=0;                              //确保目标正确结束
            return DestLen;
        }

        //-----------------------------------------------------
        //将Src拼接到Dst的后面,不论Src与Dst为什么情况,拼接后的分隔符只有一个SP
        template<class CT>
        static CT* strcat(CT* Dst, const CT* Src, const CT SP)
        {
            if (is_empty(Dst) || !Src)
                return NULL;

            int DstLen = strlen(Dst);
            if (Dst[DstLen - 1] == SP)
            {
                //目标已经是SP结束了
                if (Src[0] == SP)
                    strcpy(&Dst[DstLen], &Src[1]);
                else
                    strcpy(&Dst[DstLen], Src);
            }
            else
            {
                //目标不是SP结束
                if (Src[0] == SP)
                    strcpy(&Dst[DstLen], Src);
                else
                {
                    Dst[DstLen++] = SP;
                    strcpy(&Dst[DstLen], Src);
                }
            }
            return Dst;
        }
        //将str1和str2仅用sp分隔后拼接到dest中
        //返回值:拼接后的实际长度.0目标缓冲器不足
        template<class CT>
        static uint32_t strcpy(CT *Dest, uint32_t DestMaxSize, const CT* Src1, const CT* Src2,const CT SP)
        {
            uint32_t L1 = strcpy(Dest, DestMaxSize, Src1);
            if (!L1)
                return 0;
            uint32_t L2 = strlen(Src2);
            if (L1 + L2 > DestMaxSize)
                return 0;
            return strlen(strcat(Dest, Src2, SP));
        }
        //-----------------------------------------------------
        //根据长度拷贝字符串
        //入口:dst目标缓冲区;dstMaxSize目标的最大容量;Src源串;SrcLen源串的长度
        //返回值:0失败,参数错误或目标缓冲区不足;其他为实际拷贝的长度
        template<class CT>
        static uint32_t strcpy(CT* dst,uint32_t dstMaxSize,const CT* Src,uint32_t SrcLen=0)
        {
            dst[0]=0;
            if (is_empty(Src))
                return 0;
            if (SrcLen==0)
                SrcLen=strlen(Src);
            if (SrcLen>dstMaxSize-1)                        //总要保证缓冲区的最后要有一个0串结束符的位置
                return 0;
            strncpy(dst,Src,SrcLen);
            dst[SrcLen]=0;                                  //标记串结束符
            return SrcLen;
        }

        //不超过目标容量,尽量拷贝
        //dst目标缓冲区;MaxSize目标最大容量;Src原串
        //返回值:实际拷贝的长度
        template<class CT>
        static uint32_t strcpy2(CT* dst,uint32_t dstMaxSize,const CT* Src,uint32_t SrcLen=0)
        {
            dst[0]=0;
            if (is_empty(Src))
                return 0;
            if (SrcLen==0)
                SrcLen=strlen(Src);
            if (SrcLen>--dstMaxSize)
                SrcLen=dstMaxSize;								//总要保证缓冲区的最后要有一个0串结束符的位置
            strncpy(dst,Src,SrcLen);
            dst[SrcLen]=0;										//标记串结束符
            return SrcLen;
        }


        //-------------------------------------------------
        //统计给定的串S中指定字符Char的数量
        template<class CT>
        static const uint32_t count(const CT *S,const CT Char)
        {
            if (is_empty(S))
                return 0;
            uint32_t Ret=0;
            const CT *Pos=strchr(S,Char);
            while(Pos)
            {
                ++Ret;
                Pos=strchr(++Pos,Char);
            }
            return Ret;
        }
        //统计给定的串S中指定子串Str的数量
        template<class CT>
        static const uint32_t count(const CT *S,const CT* Str)
        {
            if (is_empty(S))
                return 0;
            uint32_t Ret=0;
            const CT *Pos=strstr(S,Str);
            while(Pos)
            {
                ++Ret;
                Pos=strstr(++Pos,Str);
            }
            return Ret;
        }

        //-----------------------------------------------------
        //尝试在Str串指定的偏移位置后面,定位查找SubStr子串
        //返回值:-1未找到;其他为结果偏移量(相对于Str而不是相对于StartIdx)
        template<class CT>
        static int pos(const CT* Str,const CT *SubStr,uint32_t StartIdx=0)
        {
            if (is_empty(Str)||StartIdx>=strlen(Str))
                return -1;
            const CT* StartStr=&Str[StartIdx];
            const CT* P=strstr(StartStr,SubStr);
            return (P==NULL?-1:P-Str);
        }
        //-----------------------------------------------------
        //在src串中顺序查找A与B开始的位置,比如在"abc=[123]456"中查找定位'['与']'出现的位置
        //如果A为NULL,则从src头开始;B为NULL则到src结尾
        //返回值:<0错误;>=0查找成功,为A串的长度
        //出口:PA是A的位置偏移量;PB是B的位置偏移量
        template<class CT>
        static int pos(const CT* src,uint32_t srclen,const CT* A,const CT* B,uint32_t &PA,uint32_t &PB)
        {
            if (is_empty(src))
                return -1;
            const CT *TP=src;
            uint32_t Alen=0;
            if (!is_empty(A))
            {
                //有A串,那么就记录A串的长度,准备返回使用;查找A串的位置
                Alen=strlen(A);
                TP=strstr(src,A);
                if (TP!=NULL)
                    PA=uint32_t(TP-src);
                else
                    return -2;
            }
            else
                PA=0;

            if (!is_empty(B))
            {
                TP=strstr(TP+Alen,B);                       //在A串出现的位置之后查找B串的位置
                if (!TP)
                    return -3;
            }
            else
                TP=src+srclen;                              //没有B串,那么就指向src的尾字符

            PB=uint32_t(TP-src);                            //得到B串的出现位置
            return Alen;
        }

        //在当前串中顺序查找A与B开始的位置,比如在"abc=[123]456"中查找定位'['与']'出现的位置
        //如果A为NULL,则从src头开始;B为NULL则到src结尾
        //返回值:true查找成功,A与B都存在,并且B在A的后面;false失败
        //出口:PA是A的位置偏移量;PB是B的位置偏移量
        template<class CT>
        static bool pos(const CT* src,const CT* A,const CT* B,uint32_t &PA,uint32_t &PB)
        {
            if (src==NULL)
                return false;
            return pos(src,strlen(src),A,B,PA,PB)>=0;
        }
        //-----------------------------------------------------
        //找到在A串和B串之间的字符串,结果中不包含A与B(A为空则从头开始截取,B为空则到尾结束)
        //返回值:0失败;>0为截取的内容长度
        template<class CT>
        static uint32_t sub(const CT* src,uint32_t srclen,const CT* AStr,const CT* BStr,CT* Result,uint32_t ResultMaxSize)
        {
            if (Result==NULL)
                return 0;
            Result[0]=0;
            uint32_t APos,BPos;
            int Alen=pos(src,srclen,AStr,BStr,APos,BPos);
            if (Alen<0)
                return 0;

            uint32_t RL=BPos-APos-Alen;
            if (!RL)
                return 0;
            return strcpy(Result,ResultMaxSize,&src[APos+Alen],RL);
        }
        template<class CT>
        static uint32_t sub(const CT* src,const CT* AStr,const CT* BStr,CT* Result,uint32_t ResultMaxSize)
        {
            if (Result==NULL)
                return 0;
            Result[0]=0;
            if (is_empty(src))
                return false;
            return sub(src,strlen(src),AStr,BStr,Result,ResultMaxSize);
        }
        //-----------------------------------------------------
        //将src中AStr后面的内容放入结果
        template<class CT>
        static uint32_t sub(const CT* src,const CT* AStr,CT* Result,uint32_t ResultMaxSize) {return sub(src,AStr,NULL,Result,ResultMaxSize);}
        //-----------------------------------------------------
        //截取Str中从首部到SP分隔符之前的内容到Result,Str在截取后调整到SP之后
        //返回值:<0错误;>=0截取到的内容长度
        template<class CT>
        static int sub(CT* &Str,const CT SP,CT *Result,uint32_t ResultSize)
        {
            if (!Str)
                return -1;
            CT* Pos=strchr(Str,SP);
            if (!Pos)
                return -2;
            uint32_t Len=uint32_t(Pos-Str);
            if (!Len)
            {
                ++Str;
                return 0;
            }

            if (strcpy(Result,ResultSize,Str,Len)==Len)
            {
                Str=Pos+1;
                return Len;
            }
            return -3;
        }
        //-----------------------------------------------------
        //截取Str中从首部到SP分隔符之前的内容并转换到整数Result,Str在截取后调整到SP之后
        //返回值:<0错误;>=0截取到的内容长度
        template<class CT>
        static int sub(CT* &Str,const CT SP,uint32_t &Result,const uint32_t radix=10)
        {
            if (!Str)
                return -1;
            CT* Pos=strchr(Str,SP);
            if (!Pos)
                return -2;
            *Pos=0;

            uint32_t Len=uint32_t(Pos-Str);
            Result=atoul(Str,radix);

            Str=Pos+1;
            return Len;
        }
        //-------------------------------------------------
        //从S中选择第一个Char之前的串到Result中
        //返回值:NULL失败(在S中没有Char);非空,成功(S中当前Char的位置指针)
        template<class CT>
        static const CT* sub(const CT *S,const CT Char,CT* Result)
        {
            const CT* Ret=strchr(S,Char);
            if (!Ret)
                return NULL;
            uint32_t len=Ret-S;
            strncpy(Result,S,len);
            Result[len]=0;
            return Ret;
        }

        //-----------------------------------------------------
        //将数字转换成十进制字符串:num=数字;result=结果串;C为结果串长度(数字长度不足的前面补零)
        template<class CT>
        static CT* dec(uint32_t num,CT *result,uint32_t C)
        {
            while (C-->0)
            {
                result[C]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero());
                num/=10;
            }
            return result;
        }
        //-----------------------------------------------------
        //将数字num转换为十进制串,只转换数字的最后两位,不足补零
        template<class CT>
        static CT* dec2(uint32_t num,CT *result)
        {
            result[1]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero());
            num/=10;
            result[0]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero());
            return result;
        }
        //-----------------------------------------------------
        //将数字num转换为十进制串,只转换数字的最后三位,不足补零
        template<class CT>
        static CT* dec3(uint32_t num,CT *result)
        {
            result[2]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero());
            num/=10;
            result[1]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero());
            num/=10;
            result[0]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero());
            num/=10;
            return result;
        }

        //-----------------------------------------------------
        //将数字num转换为十进制串,只转换数字的最后四位,不足补零
        template<class CT>
        static CT* dec4(uint32_t num,CT *result)
        {
            result[3]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero());
            num/=10;
            result[2]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero());
            num/=10;
            result[1]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero());
            num/=10;
            result[0]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero());
            num/=10;
            return result;
        }
        //-----------------------------------------------------
        //将数字num转换为十进制串,只转换数字的最后八位,不足补零
        template<class CT>
        static CT* dec8(uint32_t num,CT *result)
        {
            result[7]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero());
            num/=10;
            result[6]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero());
            num/=10;
            result[5]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero());
            num/=10;
            result[4]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero());
            num/=10;
            result[3]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero());
            num/=10;
            result[2]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero());
            num/=10;
            result[1]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero());
            num/=10;
            result[0]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero());
            num/=10;
            return result;
        }

        //将4bit数字转换成十六进制字符
        template<class CT>
        static CT hex(uint32_t num)
        {
            rx_assert(num<=0x0F);
            return sc<CT>::hex_upr(num);
        }
        //-----------------------------------------------------
        //将数字转换成十六进制字符串:num=数字;result=结果串;C为结果串长度(数字长度不足的前面补零)
        template<class CT>
        static CT* hex(uint32_t num,CT *result,uint32_t C)
        {
            while (C-->0)
            {
                result[C]=sc<CT>::hex_upr(num&0xf);
                num>>=4;
            }
            return result;
        }
        //-----------------------------------------------------
        //将数字num转换为十六进制串,只转换数字的最后两位,不足补零
        template<class CT>
        static CT* hex2(uint32_t num,CT *result)
        {
            result[1]=sc<CT>::hex_upr(num&0xf);
            num>>=4;
            result[0]=sc<CT>::hex_upr(num&0xf);
            num>>=4;
            return result;
        }

        //-----------------------------------------------------
        //将数字num转换为十六进制串,只转换数字的最后四位,不足补零
        template<class CT>
        static CT* hex4(uint32_t num,CT *result)
        {
            result[3]=sc<CT>::hex_upr(num&0xf);
            num>>=4;
            result[2]=sc<CT>::hex_upr(num&0xf);
            num>>=4;
            result[1]=sc<CT>::hex_upr(num&0xf);
            num>>=4;
            result[0]=sc<CT>::hex_upr(num&0xf);
            num>>=4;
            return result;
        }
        //-----------------------------------------------------
        //将数字num转换为十六进制串,转换数字的后八位,不足补零
        template<class CT>
        static CT* hex8(uint32_t num,CT *result)
        {
            result[7]=sc<CT>::hex_upr(num&0xf);
            num>>=4;
            result[6]=sc<CT>::hex_upr(num&0xf);
            num>>=4;
            result[5]=sc<CT>::hex_upr(num&0xf);
            num>>=4;
            result[4]=sc<CT>::hex_upr(num&0xf);
            num>>=4;
            result[3]=sc<CT>::hex_upr(num&0xf);
            num>>=4;
            result[2]=sc<CT>::hex_upr(num&0xf);
            num>>=4;
            result[1]=sc<CT>::hex_upr(num&0xf);
            num>>=4;
            result[0]=sc<CT>::hex_upr(num&0xf);
            num>>=4;
            return result;
        }
        //-----------------------------------------------------
        //将二进制数据流转换成十六进制串
        template<class CT>
        static CT* hex(const uint8_t *bin, uint32_t size,CT *result,bool IsLower=false)
        {
            CT *Map=sc<CT>::hex_upr();
            if (!IsLower)
                Map=sc<CT>::hex_lwr();

            for (; size--; bin++)
            {
                *result++ = Map[bin[0] >> 4];
                *result++ = Map[bin[0] & 0x0f];
            }
            *result = 0;
            return result;
        }
        //将二进制数据转换为16进制字符串,每个字节间使用空格间隔,每隔LineLength长度进行一次换行.
        //返回值:已经处理过的数据数量,与size相同的时候为正常完成.
        template<class CT>
        static uint32_t hex(const uint8_t* bin, uint32_t size, CT* result, uint32_t result_size, bool IsLower = false, uint32_t LineLength = 32)
        {
            uint32_t RL = 0;
            uint32_t i = 0;
            for (; i < size; ++i)
            {
                if (i)
                {
                    if (i%LineLength == 0)
                    {
                        if (RL + 2 >= result_size)
                            break;
                        result[RL++] = sc<CT>::CR();
                        result[RL++] = sc<CT>::LF();
                    }
                    else
                    {
                        if (RL + 1 >= result_size)
                            break;
                        result[RL++] = sc<CT>::space();
                    }
                }
                if (RL + 2 >= result_size)
                    break;
                hex2(bin[i], &result[RL]);
                RL += 2;
            }
            result[RL] = 0;
            return i;
        }
        //-----------------------------------------------------
        //将给定的字符C(A~F|a~f|0~9)转为对应的数字(字节)
        template<class CT>
        static uint8_t byte(CT C)
        {
            if (C>=sc<CT>::a()&&C<=sc<CT>::f())
                return C-sc<CT>::a()+10;
            else if (C>=sc<CT>::A()&&C<=sc<CT>::F())
                return C-sc<CT>::A()+10;
            else
                return C-sc<CT>::zero();
        }
        //-----------------------------------------------------
        //将给定的十六进制形式的字符串中的开头的两个字符转换为一个字节长度的数字
        template<class CT>
        static uint8_t byte(const CT *str)
        {
            uint8_t R=byte<CT>((CT)str[0]);
            R<<=4;
            return R+byte<CT>((CT)str[1]);
        }
        //-----------------------------------------------------
        //将指定长度的十六进制串转换成对应的二进制字节流
        //入口:Str,StrLen待转换的串和长度
        //      Data,DataSize接收字节流的缓冲区和数据长度(调用前告知缓冲区尺寸,调用后为字节流长度)
        //返回值:NULL转换失败;其他为字节流缓冲区指针
        template<class CT>
        static uint8_t* bin(const CT* Str,uint32_t StrLen,uint8_t* Data,uint32_t &DataSize)
        {
            if (is_empty(Str)||Data==NULL)
                return NULL;
            if (StrLen%2!=0||StrLen/2>DataSize)
                return NULL;
            DataSize=0;
            for(uint32_t i=0; Str[i]&&i<StrLen; i+=2)
                Data[DataSize++]=byte(&Str[i]);
            return Data;
        }
        template<class CT>
        static uint8_t* bin(const CT* Str,uint32_t StrLen,uint8_t* Data,const uint32_t DataSize)
        {
            uint32_t DS=DataSize;
            return bin(Str,StrLen,Data,DS);
        }



        //-----------------------------------------------------
        //对一个串进Str行遍历,用To替换掉里面所有的From字符
        template<class CT>
        static CT* replace(CT *Str,CT From,CT To)
        {
            if (is_empty(Str))
                return NULL;
            CT* Ret=Str;
            while(Str=strchr(Str,From))
                *Str++=To;
            return Ret;
        }
        //将Str中的内容复制到Buf中,并将字符From替换为To
        template<class CT>
        static CT* replace(const CT *Str,CT* Buf,CT From, CT To)
        {
            if (is_empty(Str))
                return NULL;
            while (*Str)
            {
                if (*Str == From)
                    *Buf = To;
                else
                    *Buf = *Str;
                ++Buf;
                ++Str;
            }
            return Buf;
        }
        //将Src串中的所有From都替换为To,将结果放入Dst
        //返回值:成功时为Dst,失败时为NULL(目标空间不足)
        template<class CT>
        static CT* replace(const CT *SrcStr,const CT* From,uint32_t FromLen,const CT* To,uint32_t ToLen,CT* Dst,uint32_t DstSize,uint32_t SrcLen=0)
        {
            const CT* Src=SrcStr;
            if (is_empty(Src)||is_empty(From)||!Dst)
                return NULL;
            if (!SrcLen)
                SrcLen=strlen(Src);
            if (is_empty(To))
                ToLen=0;
            const CT* Pos=strstr(Src,From);
            uint32_t DstLen=0;
            while(Pos)
            {
                uint32_t SegLen=uint32_t(Pos-Src);          //当前目标前面的部分,需要放入结果缓冲区

                if (DstLen+SegLen>=DstSize)
                    return NULL;    //目标缓冲区不足,退出
                strncpy(&Dst[DstLen],Src,SegLen);           //将当前段之前的部分拷贝到目标缓冲区
                DstLen+=SegLen;                             //目标长度增加

                if (DstLen+ToLen>=DstSize)
                    return NULL;

                if (ToLen)
                {
                    strncpy(&Dst[DstLen],To,ToLen);
                    DstLen+=ToLen;
                }

                Src=Pos+FromLen;
                SrcLen-=SegLen+FromLen;
                Pos=strstr(Src,From);
            }
            strncpy(&Dst[DstLen],Src,SrcLen);               //将剩余部分拷贝到目标
            DstLen+=SrcLen;
            Dst[DstLen++]=0;
            return Dst;
        }
        template<class CT>
        static CT* replace(const CT *SrcStr,const CT* From,const CT* To,CT* Dst,uint32_t DstSize,uint32_t SrcLen=0)
        {
            const CT* Src=SrcStr;
            if (is_empty(Src)||is_empty(From)||!Dst)
                return NULL;
            uint32_t FromLen=strlen(From);
            uint32_t ToLen=is_empty(To)?0:strlen(To);
            return replace(SrcStr,From,FromLen,To,ToLen,Dst,DstSize,SrcLen);
        }
        //-----------------------------------------------------
        //判断给定的字符串是否全部都为数字(十进制或十六进制整形)
        template<class CT>
        static bool isnumber_str(const CT* Str,uint32_t StrLen=0,bool IsHex=false)
        {
            if (is_empty(Str))
                return false;
            if (!StrLen)
                StrLen=strlen(Str);
            CT C;

            for(uint32_t I=0; I<StrLen; I++)                //对指定长度的串进行全遍历或遇到结束符
            {
                C=Str[I];
                if (C>=sc<CT>::zero()&&C<='9')              //是十进制数字,直接准备判断下一个
                    continue;

                if (!IsHex)
                    return false;                           //不是十六进制模式,非数字的串出现就返回假

                if ((C>=sc<CT>::A()&&C<=sc<CT>::F())||(C>=sc<CT>::a()&&C<=sc<CT>::f()))
                    continue;                               //十六进制时,当前字符是合法的,跳过,准备判断下一个.
                else
                    return false;                           //否则说明当前不是合法的十六进制数字串
            }
            return true;                                    //全部的字符都检查完成了,说明当前串是合法的数字串.
        }
    };

    //-----------------------------------------------------
    //数字转为字符串的工具对象
    template<class CT>
    class n2str
    {
        CT  m_tinystr[32];
    public:
        n2str() { m_tinystr[0] = 0; }
        n2str(uint32_t n, uint32_t r = 10) { st::ultoa(n, m_tinystr, r); }
        n2str(int64_t n, uint32_t r = 10) { st::itoa64(n, m_tinystr, r); }
        operator CT* ()const { return m_tinystr; }
        operator const CT* ()const { return m_tinystr; }
        const CT* operator()(uint32_t n, uint32_t r = 10) { st::ultoa(n, m_tinystr, r); return m_tinystr; }
        const CT* operator()(int64_t n, uint32_t r = 10) { st::itoa64(n, m_tinystr, r); return m_tinystr; }
    };
    typedef n2str<char>     n2s_t;
    typedef n2str<wchar_t>  n2w_t;

    //-----------------------------------------------------
    //进行字符串大小写转换的功能封装(mode=0-不转换;1-转为小写;2转为大写)
    template<uint32_t max_size,typename CT,uint32_t mode>
    class icstr;

    //不转换
    template<uint32_t max_size, typename CT>
    class icstr<max_size,CT, 0>
    {
        const CT* m_src_str;
    public:
        icstr():m_src_str(NULL) {}
        icstr(const CT *src) :m_src_str(src) {}
        operator const CT* () { return m_src_str; }
        const CT* to(const CT *src) { return src; }
        const CT* c_str() { return m_src_str; }
    };
    //转为小写字母
    template<uint32_t max_size, typename CT>
    class icstr<max_size, CT, 1>
    {
        CT m_str[max_size];
    public:
        icstr() { m_str[0] = 0; }
        icstr(const CT *src) {st::strlwr(m_str, max_size, src);}
        operator const CT* () { return m_str; }
        const CT* to(const CT *src) { st::strlwr(m_str, max_size, src); return m_str; }
        const CT* c_str() { return m_str; }
    };
    //转为大写字母
    template<uint32_t max_size, typename CT>
    class icstr<max_size, CT, 2>
    {
        CT m_str[max_size];
    public:
        icstr() { m_str[0] = 0; }
        icstr(const CT *src) { st::strupr(m_str, max_size, src); }
        operator const CT* () { return m_str; }
        const CT* to(const CT *src) { st::strupr(m_str, max_size, src); return m_str; }
        const CT* c_str() { return m_str; }
    };
}


#endif
