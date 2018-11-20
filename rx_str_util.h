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
    //ͳһ��װ�˶�ԭʼ�ַ�������ĺ���
    class st
    {
    public:
        //-----------------------------------------------------
        //����
        static uint32_t         strlen(const char* Str) {return (uint32_t)::strlen(Str);}
        static uint32_t         strlen(const wchar_t* Str) {return (uint32_t)::wcslen(Str);}

        //����
        static char*            strcpy(char* dst,const char* src) {return ::strcpy(dst,src);}
        static wchar_t*         strcpy(wchar_t* dst,const wchar_t* src) {return wcscpy(dst,src);}

        //ƴ��
        static char*            strcat(char* dst,const char* src) {return ::strcat(dst,src);}
        static wchar_t*         strcat(wchar_t* dst,const wchar_t* src) {return wcscat(dst,src);}

        //��������
        static char*            strncpy(char* dst,const char* src,uint32_t len) {return ::strncpy(dst,src,len);}
        static wchar_t*         strncpy(wchar_t* dst,const wchar_t* src,uint32_t len) {return ::wcsncpy(dst,src,len);}

        //β������(ȡsrc�ĺ�nλ)
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

        //�Ƚ�,���ִ�Сд
        static int              strcmp(const char* dst,const char* src) {return ::strcmp(dst,src);}
        static int              strcmp(const wchar_t* dst,const wchar_t* src) {return ::wcscmp(dst,src);}

        //�Ƚ�,�����ִ�Сд
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
        //�����Ƚ�,���ִ�Сд,�涨��󳤶�

        static int              strncmp(const char *s1, const char *s2, uint32_t  maxlen) {return ::strncmp(s1,s2,maxlen);}
        static int              strncmp(const wchar_t *s1, const wchar_t *s2, uint32_t  maxlen) {return ::wcsncmp(s1,s2,maxlen);}

        //�����Ƚ�,�����ִ�Сд,�涨��󳤶�
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

        //�Ӵ�����
        static char *           strstr(char *s1, const char *s2) {return ::strstr(s1,s2);}
        static wchar_t*         strstr(wchar_t *s1, const wchar_t *s2) {return ::wcsstr(s1,s2);}
        static const char *     strstr(const char *s1, const char *s2) {return ::strstr(s1,s2);}
        static const wchar_t*   strstr(const wchar_t *s1, const wchar_t *s2) {return ::wcsstr(s1,s2);}

        //�ַ�����
        static char *           strchr(char *s, int c) {return ::strchr(s,c);}
        static wchar_t *        strchr(wchar_t *s, wchar_t c) { return ::wcschr(s, c); }
        static const char *     strchr(const char *s, int c) {return ::strchr(s,c);}
        static const wchar_t *  strchr(const wchar_t *s, wchar_t c) { return ::wcschr(s, c); }

        //�����ַ�����
        static char *           strrchr(char *s, int c) {return ::strrchr(s,c);}
        static wchar_t *        strrchr(wchar_t *s, int c) {return ::wcsrchr(s,c);}
        static const char *     strrchr(const char *s, int c) {return ::strrchr(s,c);}
        static const wchar_t *  strrchr(const wchar_t *s, int c) {return ::wcsrchr(s,c);}

        //�ַ���ת��Ϊ��д
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

        //�ַ���ת��ΪСд
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

        //�ַ���ת����չ,ֱ�ӿ�����Ŀ�껺����
        template<class CT>
        static CT *             strupr(const CT *s,CT* TmpBuf) {strcpy(TmpBuf,s); return strupr(TmpBuf);}
        template<class CT>
        static CT *             strlwr(const CT *s,CT* TmpBuf) {strcpy(TmpBuf,s); return strlwr(TmpBuf);}

        //�ַ���תΪ�޷�������
        static uint32_t         atoul(const char* s,int radix=0) {return ::strtoul(s,NULL,radix);}
        static uint32_t         atoul(const wchar_t* s,int radix=0) {return ::wcstoul(s,NULL,radix);}
        template<class CT>
        static uint32_t         atoul(const CT* s,int radix,uint32_t DefVal) {if (is_empty(s)) return DefVal; else return atoul(s,radix);}

        //�ַ���ת��Ϊ����
        static int              atoi(const char* s,int radix=0) {return ::strtol(s,NULL,radix);}
        static int              atoi(const wchar_t* s,int radix=0) {return ::wcstol(s,NULL,radix);}
        template<class CT>
        static int              atoi(const CT* s,int radix,int DefVal) {if (is_empty(s))return DefVal; else return atoi(s,radix);}

        #if RX_CC==RX_CC_GCC
        //�ַ���ת��Ϊ����
        static uint64_t         atoi64(const char* s) {return ::strtoll(s,NULL,10);}
        static uint64_t         atoi64(const wchar_t* s) {return ::wcstoll(s,NULL,10);}
        #else
        //�ַ���ת��Ϊ����
        static uint64_t         atoi64(const char* s) {return ::_atoi64(s);}
        static uint64_t         atoi64(const wchar_t* s) {return ::_wtoi64(s);}
        #endif

        //�ַ���ת��Ϊ������
        static double           atof(const char* s) {return ::atof(s);}
        static double           atof(const wchar_t* s) {return ::wcstod(s,NULL);}

        //����ת��Ϊ�ַ���

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

        //�﷨��,����תΪ0xǰ׺��ʮ�����ƴ�
        template<class CT>
        static CT*              itox(int value,CT* Buff) {Buff[0]=sc<CT>::zero(); Buff[1]=sc<CT>::x(); itoa(value,&Buff[2],16); return Buff;}
        template<class CT>
        static CT*              itox64(uint64_t value,CT* Buff) {Buff[0]=sc<CT>::zero(); Buff[1]=sc<CT>::x(); itoa64(value,&Buff[2],16); return Buff;}
        template<class CT>
        static CT*              ultox(uint32_t value,CT* Buff) {Buff[0]=sc<CT>::zero(); Buff[1]=sc<CT>::x(); ultoa(value,&Buff[2],16); return Buff;}

        //������תΪ�ַ���
        static char*            ftoa(const double& f,char* string,const char* Fmt=NULL) {if (!Fmt) Fmt="%.2f"; sprintf(string,Fmt,f); return string;}
        static wchar_t*         ftoa(const double& f,wchar_t* string,const wchar_t* Fmt=NULL) {if (!Fmt) Fmt=L"%.2f"; wprintf(string,Fmt,f); return string;}

        //�ж��Ƿ�Ϊʮ���������ַ�
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
        //�ж��Ƿ�Ϊʮ�������ֺ���ĸ
        static bool             isalnum(char c) {return (c>='0'&&c<='9')||(c>='A'&&c<='Z')||(c>='a'&&c<='z');}
        static bool             isalnum(wchar_t c) {return (c>=L'0'&&c<=L'9')||(c>=L'A'&&c<=L'Z')||(c>=L'a'&&c<=L'z');}

        //�ַ�����ʽ��
        //����ֵ:<0����;>=0Ϊ������ݳ���
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
        //�﷨��,������S�����ж�,����ǿ�ֵ�򷵻ظ�����Ĭ��ֵ
        template<class CT>
        static const CT* value(const CT* S,const CT* DefValue)
        {
            if (is_empty(S))
                return DefValue;
            return S;
        }
        //-----------------------------------------------------
        //��ԭSrc��ƴ�ӵ�Dest(��������)��,Dest�������ΪDestMaxSize,Src���̫����ʧ����.
        //����ֵ:0ʧ��;���´�����������
        template<class CT>
        static uint32_t strcat(CT *Dest,uint32_t DestMaxSize,const CT* Src,uint32_t SrcLen=0)
        {
            uint32_t DestLen=strlen(Dest);                  //����õ�Ŀ���������еĴ�����
            if (SrcLen==0)
                SrcLen=strlen(Src);              //����õ�ԭ���������еĳ���
            uint32_t DestRemainLen=DestMaxSize-DestLen;     //����Ŀ���������
            if (SrcLen>DestRemainLen-1)
                return 0;           //�ռ䲻��,����������
            strncpy(&Dest[DestLen],Src,SrcLen);             //ƴ�ӿ���
            uint32_t NewLen=SrcLen+DestLen;                 //Ŀ�����ڵĳ���
            Dest[NewLen]=0;                                 //ȷ��Ŀ����ȷ����
            return NewLen;
        }

        //-----------------------------------------------------
        //��ԭSrc��ƴ�ӵ�Dest(��������)��,Dest�������ΪDestMaxSize,Src���̫����ֻ��ƴ��һ����.
        //����ֵ:�´�����������
        template<class CT>
        static uint32_t strcat2(CT *Dest,uint32_t DestMaxSize,const CT* Src,uint32_t SrcLen=0)
        {
            uint32_t DestLen=strlen(Dest);                  //����õ�Ŀ���������еĴ�����
            if (SrcLen==0)
                SrcLen=strlen(Src);                         //����õ�ԭ���������еĳ���
            uint32_t DestRemainLen=DestMaxSize-DestLen-1;   //����Ŀ���������
            if (SrcLen>DestRemainLen)
                SrcLen=DestRemainLen;
            strncpy(&Dest[DestLen],Src,SrcLen);			    //ƴ�ӿ���
            uint32_t NewLen=SrcLen+DestLen;					//Ŀ�����ڵĳ���
            Dest[NewLen]=0;                                 //ȷ��Ŀ����ȷ����
            return NewLen;
        }
        //-----------------------------------------------------
        //������,��Src1��Src2��ƴװ��Dest��.
        //����ֵ:0ʧ��;����Ϊ�½���ĳ���
        template<class CT>
        static uint32_t strcat(CT *Dest,uint32_t DestMaxSize,const CT* Src1,const CT* Src2)
        {
            uint32_t DestLen=strcpy(Dest,DestMaxSize,Src1);
            uint32_t Src2Len=strlen(Src2);                  //����õ�ԭ���������еĳ���
            uint32_t DestRemainLen=DestMaxSize-DestLen;     //����Ŀ���������
            if (Src2Len>DestRemainLen-1)
                return 0;          //�ռ䲻��,����������
            strncpy(&Dest[DestLen],Src2,Src2Len);           //ƴ�ӿ���
            uint32_t NewLen=Src2Len+DestLen;                //Ŀ�����ڵĳ���
            Dest[NewLen]=0;                                 //ȷ��Ŀ����ȷ����
            return NewLen;
        }
        //-----------------------------------------------------
        //��ԭSrc��ƴ�ӵ�Dest(��������)��,Dest�������ΪDestMaxSize,Src���̫����ʧ����.
        //����ֵ:0ʧ��;���´�����������
        template<class CT>
        static uint32_t strcat(CT *Dest,uint32_t &DestLen,uint32_t DestMaxSize,const CT* Src,uint32_t SrcLen=0)
        {
            if (SrcLen==0)
                SrcLen=strlen(Src);              //����õ�ԭ���������еĳ���
            uint32_t DestRemainLen=DestMaxSize-DestLen;     //����Ŀ���������
            if (SrcLen>DestRemainLen-1)
                return 0;           //�ռ䲻��,����������
            strncpy(&Dest[DestLen],Src,SrcLen);             //ƴ�ӿ���
            DestLen+=SrcLen;                                //Ŀ�����ڵĳ���
            Dest[DestLen]=0;                                //ȷ��Ŀ����ȷ����
            return DestLen;
        }
        template<class CT>
        static uint32_t strcat(CT *Dest,uint32_t &DestLen,uint32_t DestMaxSize,CT Src)
        {
            uint32_t DestRemainLen=DestMaxSize-DestLen;     //����Ŀ���������
            if (DestRemainLen<2)
                return 0;                //�ռ䲻��,����������
            Dest[DestLen]=Src;                              //ƴ�ӿ���
            Dest[++DestLen]=0;                              //ȷ��Ŀ����ȷ����
            return DestLen;
        }

        //-----------------------------------------------------
        //��Srcƴ�ӵ�Dst�ĺ���,����Src��DstΪʲô���,ƴ�Ӻ�ķָ���ֻ��һ��SP
        template<class CT>
        static CT* strcat(CT* Dst, const CT* Src, const CT SP)
        {
            if (is_empty(Dst) || !Src)
                return NULL;

            int DstLen = strlen(Dst);
            if (Dst[DstLen - 1] == SP)
            {
                //Ŀ���Ѿ���SP������
                if (Src[0] == SP)
                    strcpy(&Dst[DstLen], &Src[1]);
                else
                    strcpy(&Dst[DstLen], Src);
            }
            else
            {
                //Ŀ�겻��SP����
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
        //��str1��str2����sp�ָ���ƴ�ӵ�dest��
        //����ֵ:ƴ�Ӻ��ʵ�ʳ���.0Ŀ�껺��������
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
        //���ݳ��ȿ����ַ���
        //���:dstĿ�껺����;dstMaxSizeĿ����������;SrcԴ��;SrcLenԴ���ĳ���
        //����ֵ:0ʧ��,���������Ŀ�껺��������;����Ϊʵ�ʿ����ĳ���
        template<class CT>
        static uint32_t strcpy(CT* dst,uint32_t dstMaxSize,const CT* Src,uint32_t SrcLen=0)
        {
            dst[0]=0;
            if (is_empty(Src))
                return 0;
            if (SrcLen==0)
                SrcLen=strlen(Src);
            if (SrcLen>dstMaxSize-1)                        //��Ҫ��֤�����������Ҫ��һ��0����������λ��
                return 0;
            strncpy(dst,Src,SrcLen);
            dst[SrcLen]=0;                                  //��Ǵ�������
            return SrcLen;
        }

        //������Ŀ������,��������
        //dstĿ�껺����;MaxSizeĿ���������;Srcԭ��
        //����ֵ:ʵ�ʿ����ĳ���
        template<class CT>
        static uint32_t strcpy2(CT* dst,uint32_t dstMaxSize,const CT* Src,uint32_t SrcLen=0)
        {
            dst[0]=0;
            if (is_empty(Src))
                return 0;
            if (SrcLen==0)
                SrcLen=strlen(Src);
            if (SrcLen>--dstMaxSize)
                SrcLen=dstMaxSize;								//��Ҫ��֤�����������Ҫ��һ��0����������λ��
            strncpy(dst,Src,SrcLen);
            dst[SrcLen]=0;										//��Ǵ�������
            return SrcLen;
        }


        //-------------------------------------------------
        //ͳ�Ƹ����Ĵ�S��ָ���ַ�Char������
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
        //ͳ�Ƹ����Ĵ�S��ָ���Ӵ�Str������
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
        //������Str��ָ����ƫ��λ�ú���,��λ����SubStr�Ӵ�
        //����ֵ:-1δ�ҵ�;����Ϊ���ƫ����(�����Str�����������StartIdx)
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
        //��src����˳�����A��B��ʼ��λ��,������"abc=[123]456"�в��Ҷ�λ'['��']'���ֵ�λ��
        //���AΪNULL,���srcͷ��ʼ;BΪNULL��src��β
        //����ֵ:<0����;>=0���ҳɹ�,ΪA���ĳ���
        //����:PA��A��λ��ƫ����;PB��B��λ��ƫ����
        template<class CT>
        static int pos(const CT* src,uint32_t srclen,const CT* A,const CT* B,uint32_t &PA,uint32_t &PB)
        {
            if (is_empty(src))
                return -1;
            const CT *TP=src;
            uint32_t Alen=0;
            if (!is_empty(A))
            {
                //��A��,��ô�ͼ�¼A���ĳ���,׼������ʹ��;����A����λ��
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
                TP=strstr(TP+Alen,B);                       //��A�����ֵ�λ��֮�����B����λ��
                if (!TP)
                    return -3;
            }
            else
                TP=src+srclen;                              //û��B��,��ô��ָ��src��β�ַ�

            PB=uint32_t(TP-src);                            //�õ�B���ĳ���λ��
            return Alen;
        }

        //�ڵ�ǰ����˳�����A��B��ʼ��λ��,������"abc=[123]456"�в��Ҷ�λ'['��']'���ֵ�λ��
        //���AΪNULL,���srcͷ��ʼ;BΪNULL��src��β
        //����ֵ:true���ҳɹ�,A��B������,����B��A�ĺ���;falseʧ��
        //����:PA��A��λ��ƫ����;PB��B��λ��ƫ����
        template<class CT>
        static bool pos(const CT* src,const CT* A,const CT* B,uint32_t &PA,uint32_t &PB)
        {
            if (src==NULL)
                return false;
            return pos(src,strlen(src),A,B,PA,PB)>=0;
        }
        //-----------------------------------------------------
        //�ҵ���A����B��֮����ַ���,����в�����A��B(AΪ�����ͷ��ʼ��ȡ,BΪ����β����)
        //����ֵ:0ʧ��;>0Ϊ��ȡ�����ݳ���
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
        //��src��AStr��������ݷ�����
        template<class CT>
        static uint32_t sub(const CT* src,const CT* AStr,CT* Result,uint32_t ResultMaxSize) {return sub(src,AStr,NULL,Result,ResultMaxSize);}
        //-----------------------------------------------------
        //��ȡStr�д��ײ���SP�ָ���֮ǰ�����ݵ�Result,Str�ڽ�ȡ�������SP֮��
        //����ֵ:<0����;>=0��ȡ�������ݳ���
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
        //��ȡStr�д��ײ���SP�ָ���֮ǰ�����ݲ�ת��������Result,Str�ڽ�ȡ�������SP֮��
        //����ֵ:<0����;>=0��ȡ�������ݳ���
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
        //��S��ѡ���һ��Char֮ǰ�Ĵ���Result��
        //����ֵ:NULLʧ��(��S��û��Char);�ǿ�,�ɹ�(S�е�ǰChar��λ��ָ��)
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
        //������ת����ʮ�����ַ���:num=����;result=�����;CΪ���������(���ֳ��Ȳ����ǰ�油��)
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
        //������numת��Ϊʮ���ƴ�,ֻת�����ֵ������λ,���㲹��
        template<class CT>
        static CT* dec2(uint32_t num,CT *result)
        {
            result[1]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero());
            num/=10;
            result[0]=(uint8_t)((uint8_t)(num%10)+sc<CT>::zero());
            return result;
        }
        //-----------------------------------------------------
        //������numת��Ϊʮ���ƴ�,ֻת�����ֵ������λ,���㲹��
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
        //������numת��Ϊʮ���ƴ�,ֻת�����ֵ������λ,���㲹��
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
        //������numת��Ϊʮ���ƴ�,ֻת�����ֵ�����λ,���㲹��
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

        //��4bit����ת����ʮ�������ַ�
        template<class CT>
        static CT hex(uint32_t num)
        {
            rx_assert(num<=0x0F);
            return sc<CT>::hex_upr(num);
        }
        //-----------------------------------------------------
        //������ת����ʮ�������ַ���:num=����;result=�����;CΪ���������(���ֳ��Ȳ����ǰ�油��)
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
        //������numת��Ϊʮ�����ƴ�,ֻת�����ֵ������λ,���㲹��
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
        //������numת��Ϊʮ�����ƴ�,ֻת�����ֵ������λ,���㲹��
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
        //������numת��Ϊʮ�����ƴ�,ת�����ֵĺ��λ,���㲹��
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
        //��������������ת����ʮ�����ƴ�
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
        //������������ת��Ϊ16�����ַ���,ÿ���ֽڼ�ʹ�ÿո���,ÿ��LineLength���Ƚ���һ�λ���.
        //����ֵ:�Ѿ����������������,��size��ͬ��ʱ��Ϊ�������.
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
        //���������ַ�C(A~F|a~f|0~9)תΪ��Ӧ������(�ֽ�)
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
        //��������ʮ��������ʽ���ַ����еĿ�ͷ�������ַ�ת��Ϊһ���ֽڳ��ȵ�����
        template<class CT>
        static uint8_t byte(const CT *str)
        {
            uint8_t R=byte<CT>((CT)str[0]);
            R<<=4;
            return R+byte<CT>((CT)str[1]);
        }
        //-----------------------------------------------------
        //��ָ�����ȵ�ʮ�����ƴ�ת���ɶ�Ӧ�Ķ������ֽ���
        //���:Str,StrLen��ת���Ĵ��ͳ���
        //      Data,DataSize�����ֽ����Ļ����������ݳ���(����ǰ��֪�������ߴ�,���ú�Ϊ�ֽ�������)
        //����ֵ:NULLת��ʧ��;����Ϊ�ֽ���������ָ��
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
        //��һ������Str�б���,��To�滻���������е�From�ַ�
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
        //��Str�е����ݸ��Ƶ�Buf��,�����ַ�From�滻ΪTo
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
        //��Src���е�����From���滻ΪTo,���������Dst
        //����ֵ:�ɹ�ʱΪDst,ʧ��ʱΪNULL(Ŀ��ռ䲻��)
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
                uint32_t SegLen=uint32_t(Pos-Src);          //��ǰĿ��ǰ��Ĳ���,��Ҫ������������

                if (DstLen+SegLen>=DstSize)
                    return NULL;    //Ŀ�껺��������,�˳�
                strncpy(&Dst[DstLen],Src,SegLen);           //����ǰ��֮ǰ�Ĳ��ֿ�����Ŀ�껺����
                DstLen+=SegLen;                             //Ŀ�곤������

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
            strncpy(&Dst[DstLen],Src,SrcLen);               //��ʣ�ಿ�ֿ�����Ŀ��
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
        //�жϸ������ַ����Ƿ�ȫ����Ϊ����(ʮ���ƻ�ʮ����������)
        template<class CT>
        static bool isnumber_str(const CT* Str,uint32_t StrLen=0,bool IsHex=false)
        {
            if (is_empty(Str))
                return false;
            if (!StrLen)
                StrLen=strlen(Str);
            CT C;

            for(uint32_t I=0; I<StrLen; I++)                //��ָ�����ȵĴ�����ȫ����������������
            {
                C=Str[I];
                if (C>=sc<CT>::zero()&&C<='9')              //��ʮ��������,ֱ��׼���ж���һ��
                    continue;

                if (!IsHex)
                    return false;                           //����ʮ������ģʽ,�����ֵĴ����־ͷ��ؼ�

                if ((C>=sc<CT>::A()&&C<=sc<CT>::F())||(C>=sc<CT>::a()&&C<=sc<CT>::f()))
                    continue;                               //ʮ������ʱ,��ǰ�ַ��ǺϷ���,����,׼���ж���һ��.
                else
                    return false;                           //����˵����ǰ���ǺϷ���ʮ���������ִ�
            }
            return true;                                    //ȫ�����ַ�����������,˵����ǰ���ǺϷ������ִ�.
        }
    };

    //-----------------------------------------------------
    //����תΪ�ַ����Ĺ��߶���
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
    //�����ַ�����Сдת���Ĺ��ܷ�װ(mode=0-��ת��;1-תΪСд;2תΪ��д)
    template<uint32_t max_size,typename CT,uint32_t mode>
    class icstr;

    //��ת��
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
    //תΪСд��ĸ
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
    //תΪ��д��ĸ
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
