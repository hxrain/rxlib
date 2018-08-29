#ifndef _RX_DTL_UTIL_H_
#define _RX_DTL_UTIL_H_

#include "rx_cc_macro.h"
#include "rx_ct_util.h"
#include "stdlib.h"
#include <stdio.h>

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
        static const char* hex_upr(){return "0123456789ABCDEF";}
        static const char* hex_lwr(){return "0123456789abcdef";}
        static const char  hex_upr(uint32_t i){return "0123456789ABCDEF"[i];}
        static const char  hex_lwr(uint32_t i){return "0123456789abcdef"[i];}
        static const char  zero(){return '0';}
        static const char  CR(){return '\r';}
        static const char  LF(){return '\n';}
        static const char  space(){return ' ';}
        static const char  A(){return 'A';}
        static const char  F(){return 'F';}
        static const char  a(){return 'a';}
        static const char  f(){return 'f';}
        static const char  x(){return 'x';}
    };
    template<> class sc<wchar_t>
    {
    public:
        static const wchar_t* hex_upr(){return L"0123456789ABCDEF";}
        static const wchar_t* hex_lwr(){return L"0123456789abcdef";}
        static const wchar_t  hex_upr(uint32_t i){return L"0123456789ABCDEF"[i];}
        static const wchar_t  hex_lwr(uint32_t i){return L"0123456789abcdef"[i];}
        static const wchar_t  zero(){return L'0';}
        static const wchar_t  CR(){return L'\r';}
        static const wchar_t  LF(){return L'\n';}
        static const wchar_t  space(){return L' ';}
        static const wchar_t  A(){return L'A';}
        static const wchar_t  F(){return L'F';}
        static const wchar_t  a(){return L'a';}
        static const wchar_t  f(){return L'f';}
        static const wchar_t  x(){return L'x';}
    };

    //-----------------------------------------------------
    //ͳһ��װ�˶�ԭʼ�ַ�������ĺ���
    class st
    {
    public:
        //-----------------------------------------------------
        //���ݳ��ȿ����ַ���
        //���:dstĿ�껺����;dstMaxSizeĿ����������;SrcԴ��;SrcLenԴ���ĳ���
        //����ֵ:0ʧ��,���������Ŀ�껺��������;����Ϊʵ�ʿ����ĳ���
        template<class CT>
        uint32_t strcpy(CT* dst,uint32_t dstMaxSize,const CT* Src,uint32_t SrcLen=0)
        {
            dst[0]=0;
            if (is_empty(Src)) return 0;
            if (SrcLen==0) SrcLen=strlen(Src);
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
        uint32_t strcpy2(CT* dst,uint32_t dstMaxSize,const CT* Src,uint32_t SrcLen=0)
        {
            dst[0]=0;
            if (is_empty(Src)) return 0;
            if (SrcLen==0) SrcLen=strlen(Src);
            if (SrcLen>--dstMaxSize)
                SrcLen=dstMaxSize;								//��Ҫ��֤�����������Ҫ��һ��0����������λ��
            strncpy(dst,Src,SrcLen);
            dst[SrcLen]=0;										//��Ǵ�������
            return SrcLen;
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
        //��ԭSrc��ƴ�ӵ�Dest(��������)��,Dest�������ΪDestMaxSize,Src���̫����ʧ����.
        //����ֵ:0ʧ��;���´�����������
        template<class CT>
        static uint32_t strcat(CT *Dest,uint32_t DestMaxSize,const CT* Src,uint32_t SrcLen=0)
        {
            uint32_t DestLen=strlen(Dest);                  //����õ�Ŀ���������еĴ�����
            if (SrcLen==0) SrcLen=strlen(Src);              //����õ�ԭ���������еĳ���
            uint32_t DestRemainLen=DestMaxSize-DestLen;     //����Ŀ���������
            if (SrcLen>DestRemainLen-1) return 0;           //�ռ䲻��,����������
            strncpy(&Dest[DestLen],Src,SrcLen);             //ƴ�ӿ���
            uint32_t NewLen=SrcLen+DestLen;                 //Ŀ�����ڵĳ���
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
            if (Src2Len>DestRemainLen-1) return 0;          //�ռ䲻��,����������
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
            if (SrcLen==0) SrcLen=strlen(Src);              //����õ�ԭ���������еĳ���
            uint32_t DestRemainLen=DestMaxSize-DestLen;     //����Ŀ���������
            if (SrcLen>DestRemainLen-1) return 0;           //�ռ䲻��,����������
            strncpy(&Dest[DestLen],Src,SrcLen);             //ƴ�ӿ���
            DestLen+=SrcLen;                                //Ŀ�����ڵĳ���
            Dest[DestLen]=0;                                //ȷ��Ŀ����ȷ����
            return DestLen;
        }
        template<class CT>
        static uint32_t strcat(CT *Dest,uint32_t &DestLen,uint32_t DestMaxSize,CT Src)
        {
            uint32_t DestRemainLen=DestMaxSize-DestLen;     //����Ŀ���������
            if (DestRemainLen<2)   return 0;                //�ռ䲻��,����������
            Dest[DestLen]=Src;                              //ƴ�ӿ���
            Dest[++DestLen]=0;                              //ȷ��Ŀ����ȷ����
            return DestLen;
        }

        //-----------------------------------------------------
        //��Srcƴ�ӵ�Dst�ĺ���,����Src��DstΪʲô���,ƴ�Ӻ�ķָ���ֻ��һ��SP
        template<class CT>
        static char* strcat(CT* Dst, const CT* Src, const CT SP)
        {
            if (is_empty(Dst) || !Src)
                return NULL;

            int DstLen = strlen(Dst);
            if (Dst[DstLen - 1] == SP)
            {//Ŀ���Ѿ���SP������
                if (Src[0] == SP)
                    strcpy(&Dst[DstLen], &Src[1]);
                else
                    strcpy(&Dst[DstLen], Src);
            }
            else
            {//Ŀ�겻��SP����
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
            if (!L1) return 0;
            uint32_t L2 = strlen(Src2);
            if (L1 + L2 > DestMaxSize) return 0;
            return strlen(strcat(Dest, Src2, SP));
        }
        //-----------------------------------------------------
        //����
        static uint32_t         strlen(const char* Str){return (uint32_t)::strlen(Str);}
        static uint32_t         strlen(const wchar_t* Str){return (uint32_t)::wcslen(Str);}

        //����
        static char*            strcpy(char* dst,const char* src){return ::strcpy(dst,src);}
        static wchar_t*         strcpy(wchar_t* dst,const wchar_t* src){return wcscpy(dst,src);}

        //ƴ��
        static char*            strcat(char* dst,const char* src){return ::strcat(dst,src);}
        static wchar_t*         strcat(wchar_t* dst,const wchar_t* src){return wcscat(dst,src);}

        //��������
        static char*            strncpy(char* dst,const char* src,uint32_t len){return ::strncpy(dst,src,len);}
        static wchar_t*         strncpy(wchar_t* dst,const wchar_t* src,uint32_t len){return ::wcsncpy(dst,src,len);}

        //β������(ȡsrc�ĺ�nλ)
        template<class CT>
        static CT*              strrcpy(CT* dst,const CT* src,uint32_t n,uint32_t srclen=0)
        {
            if (!srclen)
                srclen=strlen(src);
            if (n>srclen) n=srclen;
            uint32_t P=n-srclen;
            return strncpy(dst,&src[P],n);
        }

        //�Ƚ�,���ִ�Сд
        static int              strcmp(const char* dst,const char* src){return ::strcmp(dst,src);}
        static int              strcmp(const wchar_t* dst,const wchar_t* src){return ::wcscmp(dst,src);}

        //�Ƚ�,�����ִ�Сд
#if RX_CC==RX_CC_VC
        static int              stricmp(const char *s1, const char *s2){return ::_strcmpi(s1,s2);}
#else
        static int              stricmp(const char *s1, const char *s2){return ::strcmpi(s1,s2);}
#endif
        static int              stricmp(const wchar_t *s1, const wchar_t *s2){return ::_wcsicmp(s1,s2);}

        //�����Ƚ�,���ִ�Сд,�涨��󳤶�

        static int              strncmp(const char *s1, const char *s2, uint32_t  maxlen){return ::strncmp(s1,s2,maxlen);}
        static int              strncmp(const wchar_t *s1, const wchar_t *s2, uint32_t  maxlen){return ::wcsncmp(s1,s2,maxlen);}

        //�����Ƚ�,�����ִ�Сд,�涨��󳤶�
#if RX_CC==RX_CC_BCC
        static int              strncmpi(const char *s1, const char *s2, uint32_t n){return ::strncmpi(s1,s2,n);}
        static int              strncmpi(const wchar_t *s1, const wchar_t *s2, uint32_t n){return ::wcsncmpi(s1,s2,n);}
#else
        static int              strncmpi(const char *s1, const char *s2, uint32_t n){return ::_strnicmp(s1,s2,n);}
        static int              strncmpi(const wchar_t *s1, const wchar_t *s2, uint32_t n){return ::_wcsnicmp(s1,s2,n);}
#endif

        //�Ӵ�����
        static char *           strstr(char *s1, const char *s2){return ::strstr(s1,s2);}
        static wchar_t*         strstr(wchar_t *s1, const wchar_t *s2){return ::wcsstr(s1,s2);}
        static const char *     strstr(const char *s1, const char *s2){return ::strstr(s1,s2);}
        static const wchar_t*   strstr(const wchar_t *s1, const wchar_t *s2){return ::wcsstr(s1,s2);}

        //�ַ�����
        static char *           strchr(char *s, int c){return ::strchr(s,c);}
        static wchar_t *        strchr(wchar_t *s, wchar_t c){ return ::wcschr(s, c); }
        static const char *     strchr(const char *s, int c){return ::strchr(s,c);}
        static const wchar_t *  strchr(const wchar_t *s, wchar_t c){ return ::wcschr(s, c); }

        //�����ַ�����
        static char *           strrchr(char *s, int c){return ::strrchr(s,c);}
        static wchar_t *        strrchr(wchar_t *s, int c){return ::wcsrchr(s,c);}
        static const char *     strrchr(const char *s, int c){return ::strrchr(s,c);}
        static const wchar_t *  strrchr(const wchar_t *s, int c){return ::wcsrchr(s,c);}

        //�ַ���ת��Ϊ��д

#if RX_CC==RX_CC_VC
        static char *           strupr(char *s){return ::_strupr(s);}
#else
        static char *           strupr(char *s){return ::strupr(s);}
#endif
        static wchar_t *        strupr(wchar_t *s){return ::_wcsupr(s);}

        //�ַ���ת��ΪСд

#if RX_CC==RX_CC_VC
        static char *           strlwr(char *s){return ::_strlwr(s);}
#else
        static char *           strlwr(char *s){return ::strlwr(s);}
#endif
        static wchar_t *        strlwr(wchar_t *s){return ::_wcslwr(s);}

        //�ַ���ת����չ,ֱ�ӿ�����Ŀ�껺����
        template<class CT>
        static CT *             strupr(const CT *s,CT* TmpBuf){strcpy(TmpBuf,s);return strupr(TmpBuf);}
        template<class CT>
        static CT *             strlwr(const CT *s,CT* TmpBuf){strcpy(TmpBuf,s);return strlwr(TmpBuf);}

        //�ַ���תΪ�޷�������
        static uint32_t         atoul(const char* s,int radix=0){return ::strtoul(s,NULL,radix);}
        static uint32_t         atoul(const wchar_t* s,int radix=0){return ::wcstoul(s,NULL,radix);}
        template<class CT>
        static uint32_t         atoul(const CT* s,int radix,uint32_t DefVal){if (is_empty(s)) return DefVal;else return atoul(s,radix);}

        //�ַ���ת��Ϊ����
        static int              atoi(const char* s,int radix=0){return ::strtol(s,NULL,radix);}
        static int              atoi(const wchar_t* s,int radix=0){return ::wcstol(s,NULL,radix);}
        template<class CT>
        static int              atoi(const CT* s,int radix,int DefVal){if (is_empty(s))return DefVal;else return atoi(s,radix);}

        //�ַ���ת��Ϊ����
        static uint64_t         atoi64(const char* s){return ::_atoi64(s);}
        static uint64_t         atoi64(const wchar_t* s){return ::_wtoi64(s);}

        //�ַ���ת��Ϊ������
        static double           atof(const char* s){return ::atof(s);}
        static double           atof(const wchar_t* s){return ::_wtof(s);}

        //����ת��Ϊ�ַ���

#if RX_CC==RX_CC_VC
        static char *           itoa(int value, char *string, int radix=10){return ::_itoa(value,string,radix);}
#else
        static char *           itoa(int value, char *string, int radix=10){return ::itoa(value,string,radix);}
#endif
        static wchar_t *        itoa(int value, wchar_t *string, int radix=10){return ::_itow(value,string,radix);}

        static char *           itoa64(uint64_t value, char *string, int radix=10){return ::_i64toa(value,string,radix);}
        static wchar_t *        itoa64(uint64_t value, wchar_t *string, int radix=10){return ::_i64tow(value,string,radix);}


#if RX_CC==RX_CC_VC
        static char *           ultoa(uint32_t value, char *string, int radix=10){return ::_ultoa(value,string,radix);}
#else
        static char *           ultoa(uint32_t value, char *string, int radix=10){return ::ultoa(value,string,radix);}
#endif
        static wchar_t *        ultoa(uint32_t value, wchar_t *string, int radix=10){return ::_ultow(value,string,radix);}

        //�﷨��,����תΪ0xǰ׺��ʮ�����ƴ�
        template<class CT>
        static CT*              itox(int value,CT* Buff){Buff[0]=sc<CT>::zero();Buff[1]=sc<CT>::x();itoa(value,&Buff[2],16);return Buff;}
        template<class CT>
        static CT*              itox64(uint64_t value,CT* Buff){Buff[0]=sc<CT>::zero();Buff[1]=sc<CT>::x();itoa64(value,&Buff[2],16);return Buff;}
        template<class CT>
        static CT*              ultox(uint32_t value,CT* Buff){Buff[0]=sc<CT>::zero();Buff[1]=sc<CT>::x();ultoa(value,&Buff[2],16);return Buff;}

        //������תΪ�ַ���
        static char*            ftoa(const double& f,char* string,const char* Fmt=NULL){if (!Fmt) Fmt="%.2f";sprintf(string,Fmt,f);return string;}
        static wchar_t*         ftoa(const double& f,wchar_t* string,const wchar_t* Fmt=NULL){if (!Fmt) Fmt=L"%.2f";wprintf(string,Fmt,f);return string;}

        //�ж��Ƿ�Ϊʮ���������ַ�
        static bool             isnumber(char c){return c>='0'&&c<='9';}
        static bool             isnumber(wchar_t c){return c>=L'0'&&c<=L'9';}
        template<class CT>
        static bool             isnumber(const CT* s)
        {
            if (s==NULL) return false;
            for(;*s;++s)
                if (!isnumber(*s)) return false;
            return true;
        }
        //�ж��Ƿ�Ϊʮ�������ֺ���ĸ
        static bool             isalnum(char c){return (c>='0'&&c<='9')||(c>='A'&&c<='Z')||(c>='a'&&c<='z');}
        static bool             isalnum(wchar_t c){return (c>=L'0'&&c<=L'9')||(c>=L'A'&&c<=L'Z')||(c>=L'a'&&c<=L'z');}

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
            return ::_vsnwprintf(Buf,BufSize,Fmt,ap);
        }
        static int              vsnprintf(char * Buf,uint32_t BufSize, const char *Fmt,va_list arglist){return ::vsnprintf(Buf,BufSize,Fmt,arglist);}
        static int              vsnprintf(wchar_t * Buf,uint32_t BufSize, const wchar_t *Fmt,va_list arglist){return ::_vsnwprintf(Buf,BufSize,Fmt,arglist);}

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
        static int              vfprintf(FILE *stream, const char *format,va_list ap){return std::vfprintf(stream,format,ap);}
        static int              vfprintf(FILE *stream, const wchar_t *format,va_list ap){return std::vfwprintf(stream,format,ap);}
#endif
        //-------------------------------------------------
        //�﷨��,������S�����ж�,����ǿ�ֵ�򷵻ظ�����Ĭ��ֵ
        template<class CT>
        static const CT* value(const CT* S,const CT* DefValue)
        {
            if (is_empty(S)) return DefValue;
            return S;
        }
        //-------------------------------------------------
        //ͳ�Ƹ����Ĵ�S��ָ���ַ�Char������
        template<class CT>
        static const uint32_t count(const CT *S,const CT Char)
        {
            if (is_empty(S)) return 0;
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
            if (is_empty(S)) return 0;
            uint32_t Ret=0;
            const CT *Pos=strstr(S,Str);
            while(Pos)
            {
                ++Ret;
                Pos=strstr(++Pos,Str);
            }
            return Ret;
        }
        //-------------------------------------------------
        //��S��ѡ���һ��Char֮ǰ�Ĵ���Result��
        //����ֵ:NULLʧ��(��S��û��Char);�ǿ�,�ɹ�(S�е�ǰChar��λ��ָ��)
        template<class CT>
        static const CT* pick(const CT *S,const CT Char,CT* Result)
        {
            const CT* Ret=strchr(S,Char);
            if (!Ret) return NULL;
            uint32_t len=Ret-S;
            strncpy(Result,S,len);
            Result[len]=0;
            return Ret;
        }
        //-----------------------------------------------------
        //������Str��ָ����ƫ��λ�ú���,��λ����SubStr�Ӵ�
        //����ֵ:-1δ�ҵ�;����Ϊ���ƫ����(�����Str�����������StartIdx)
        template<class CT>
        static int pos(const CT* Str,const CT *SubStr,uint32_t StartIdx=0)
        {
            if (is_empty(Str)||StartIdx>=strlen(Str)) return -1;
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
            {//��A��,��ô�ͼ�¼A���ĳ���,׼������ʹ��;����A����λ��
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
                if (!TP) return -3;
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
            if (src==NULL) return false;
            return Pos(src,strlen(src),A,B,PA,PB)>=0;
        }
        //-----------------------------------------------------
        //�ҵ���A����B��֮����ַ���,����в�����A��B(AΪ�����ͷ��ʼ��ȡ,BΪ����β����)
        template<class CT>
        static bool sub(const CT* src,uint32_t srclen,const CT* AStr,const CT* BStr,CT* Result,uint32_t ResultMaxSize)
        {
            if (Result==NULL)
                return false;
            Result[0]=0;
            uint32_t APos,BPos;
            int Alen=Pos(src,srclen,AStr,BStr,APos,BPos);
            if (Alen<0)
                return false;

            uint32_t RL=BPos-APos-Alen;
            if (!RL)
                return false;
            return RL==strcpy(Result,ResultMaxSize,&src[APos+Alen],RL);
        }
        template<class CT>
        static bool sub(const CT* src,const CT* AStr,const CT* BStr,CT* Result,uint32_t ResultMaxSize)
        {
            if (Result==NULL)
                return false;
            Result[0]=0;
            if (is_empty(src))
                return false;
            return SubStr(src,strlen(src),AStr,BStr,Result,ResultMaxSize);
        }
        //-----------------------------------------------------
        //��src��AStr��������ݷ�����
        template<class CT>
        static bool sub(const CT* src,const CT* AStr,CT* Result,uint32_t ResultMaxSize){return SubStr(src,AStr,NULL,Result,ResultMaxSize);}

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

        //-----------------------------------------------------
        //������ת����ʮ�������ַ���:num=����;result=�����;CΪ���������(���ֳ��Ȳ����ǰ�油��)
        template<class CT>
        static CT* hex(uint32_t num,CT *result,uint32_t C)
        {
            uint8_t TI;
            while (C-->0)
            {
                TI=(uint8_t)(num%16);
                result[C]=uint8_t(TI+(TI>=10?(sc<CT>::A()-10):(sc<CT>::zero())));
                num>>=4;
            }
            return result;
        }
        //-----------------------------------------------------
        //������numת��Ϊʮ�����ƴ�,ֻת�����ֵ������λ,���㲹��
        template<class CT>
        static CT* hex2(uint32_t num,CT *result)
        {
            uint8_t TI;
            TI=(uint8_t)(num%16);
            result[1]=uint8_t(TI+(TI>=10?(sc<CT>::A()-10):(sc<CT>::zero())));
            num>>=4;
            TI=(uint8_t)(num%16);
            result[0]=uint8_t(TI+(TI>=10?(sc<CT>::A()-10):(sc<CT>::zero())));
            num>>=4;
            return result;
        }

        //-----------------------------------------------------
        //������numת��Ϊʮ�����ƴ�,ֻת�����ֵ������λ,���㲹��
        template<class CT>
        static CT* hex4(uint32_t num,CT *result)
        {
            uint8_t TI;
            TI=(uint8_t)(num%16);
            result[3]=uint8_t(TI+(TI>=10?(sc<CT>::A()-10):(sc<CT>::zero())));
            num>>=4;
            TI=(uint8_t)(num%16);
            result[2]=uint8_t(TI+(TI>=10?(sc<CT>::A()-10):(sc<CT>::zero())));
            num>>=4;
            TI=(uint8_t)(num%16);
            result[1]=uint8_t(TI+(TI>=10?(sc<CT>::A()-10):(sc<CT>::zero())));
            num>>=4;
            TI=(uint8_t)(num%16);
            result[0]=uint8_t(TI+(TI>=10?(sc<CT>::A()-10):(sc<CT>::zero())));
            num>>=4;
            return result;
        }
        //-----------------------------------------------------
        //������numת��Ϊʮ�����ƴ�,ת�����ֵĺ��λ,���㲹��
        template<class CT>
        static CT* hex8(uint32_t num,CT *result)
        {
            uint8_t TI;
            TI=(uint8_t)(num%16);
            result[7]=uint8_t(TI+(TI>=10?(sc<CT>::A()-10):(sc<CT>::zero())));
            num>>=4;
            TI=(uint8_t)(num%16);
            result[6]=uint8_t(TI+(TI>=10?(sc<CT>::A()-10):(sc<CT>::zero())));
            num>>=4;
            TI=(uint8_t)(num%16);
            result[5]=uint8_t(TI+(TI>=10?(sc<CT>::A()-10):(sc<CT>::zero())));
            num>>=4;
            TI=(uint8_t)(num%16);
            result[4]=uint8_t(TI+(TI>=10?(sc<CT>::A()-10):(sc<CT>::zero())));
            num>>=4;
            TI=(uint8_t)(num%16);
            result[3]=uint8_t(TI+(TI>=10?(sc<CT>::A()-10):(sc<CT>::zero())));
            num>>=4;
            TI=(uint8_t)(num%16);
            result[2]=uint8_t(TI+(TI>=10?(sc<CT>::A()-10):(sc<CT>::zero())));
            num>>=4;
            TI=(uint8_t)(num%16);
            result[1]=uint8_t(TI+(TI>=10?(sc<CT>::A()-10):(sc<CT>::zero())));
            num>>=4;
            TI=(uint8_t)(num%16);
            result[0]=uint8_t(TI+(TI>=10?(sc<CT>::A()-10):(sc<CT>::zero())));
            num>>=4;
            return result;
        }
        //-----------------------------------------------------
        //��������������ת����ʮ�����ƴ�
        template<class CT>
        static CT* hex(const uint8_t *bin, uint32_t size,CT *result,bool IsLower=false)
        {//���char�İ汾
            CT *Map=sc<CT>::hex_upr();
            if (!IsLower) Map=sc<CT>::hex_lwr();

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
                        if (RL + 2 >= result_size) break;
                        result[RL++] = sc<CT>::CR();
                        result[RL++] = sc<CT>::LF();
                    }
                    else
                    {
                        if (RL + 1 >= result_size) break;
                        result[RL++] = sc<CT>::space();
                    }
                }
                if (RL + 2 >= result_size) break;
                hex2(bin[i], &result[RL]); RL += 2;
            }
            result[RL] = 0;
            return i;
        }
        //-----------------------------------------------------
        //���������ַ�C(A~F|a~f|0~9)תΪ��Ӧ������(�ֽ�)
        template<class CT>
        static uint8_t byte(const CT C)
        {
            uint8_t R=0;
            if (C>=sc<CT>::a()&&C<=sc<CT>::f())
                R+=C-sc<CT>::a()+10;
            else if (C>=sc<CT>::A()&&C<=sc<CT>::F())
                R+=C-sc<CT>::A()+10;
            else
                R+=C-sc<CT>::zero();
            return R;
        }
        //-----------------------------------------------------
        //��������ʮ��������ʽ���ַ����еĿ�ͷ�������ַ�ת��Ϊһ���ֽڳ��ȵ�����
        template<class CT>
        static uint8_t byte(const CT *str)
        {
            uint8_t R=byte(str[0]);
            R<<=4;
            return R+byte(str[1]);
        }
        //-----------------------------------------------------
        //��ָ�����ȵ�ʮ�����ƴ�ת���ɶ�Ӧ�Ķ������ֽ���
        //���:Str,StrLen��ת���Ĵ��ͳ���
        //      Data,DataSize�����ֽ����Ļ����������ݳ���(����ǰ��֪�������ߴ�,���ú�Ϊ�ֽ�������)
        //����ֵ:NULLת��ʧ��;����Ϊ�ֽ���������ָ��
        template<class CT>
        static uint8_t* bin(const CT* Str,uint32_t StrLen,uint8_t* Data,uint32_t &DataSize)
        {
            if (is_empty(Str)||Data==NULL) return NULL;
            if (StrLen%2!=0||StrLen/2>DataSize) return NULL;
            DataSize=0;
            for(uint32_t i=0;Str[i]&&i<StrLen;i+=2)
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
            if (is_empty(Str)) return NULL;
            CT* Ret=Str;
            while(Str=strchr(Str,From))
                *Str++=To;
            return Ret;
        }
        //��Str�е����ݸ��Ƶ�Buf��,�����ַ�From�滻ΪTo
        template<class CT>
        static CT* replace(const CT *Str,CT* Buf,CT From, CT To)
        {
            if (is_empty(Str)) return NULL;
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
            const char* Src=SrcStr;
            if (is_empty(Src)||is_empty(From)||!Dst)
                return NULL;
            if (!SrcLen) SrcLen=strlen(Src);
            if (is_empty(To)) ToLen=0;
            const CT* Pos=strstr(Src,From);
            uint32_t DstLen=0;
            while(Pos)
            {
                uint32_t SegLen=Pos-Src;                          //��ǰĿ��ǰ��Ĳ���,��Ҫ������������

                if (DstLen+SegLen>=DstSize) return NULL;        //Ŀ�껺��������,�˳�
                strncpy(&Dst[DstLen],Src,SegLen);          //����ǰ��֮ǰ�Ĳ��ֿ�����Ŀ�껺����
                DstLen+=SegLen;                                 //Ŀ�곤������

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
            strncpy(&Dst[DstLen],Src,SrcLen);
            DstLen+=SrcLen;
            Dst[DstLen++]=0;
            return Dst;
        }
        template<class CT>
        static CT* replace(const CT *SrcStr,const CT* From,const CT* To,CT* Dst,uint32_t DstSize,uint32_t SrcLen=0)
        {
            const char* Src=SrcStr;
            if (is_empty(Src)||is_empty(From)||!Dst)
                return NULL;
            uint32_t FromLen=strlen(From);
            uint32_t ToLen=is_empty(To)?0:strlen(To);
            return ReplaceStr(SrcStr,From,FromLen,To,ToLen,Dst,DstSize,SrcLen);
        }
        //-----------------------------------------------------
        //�жϸ������ַ����Ƿ�Ϊ����
        template<class CT>
        static bool is_number(const CT* Str,uint32_t StrLen=0,bool IsHex=false)
        {
            if (is_empty(Str)) return false;
            if (!StrLen) StrLen=strlen(Str);
            CT C;

            for(uint32_t I=0;I<StrLen;I++)                  //��ָ�����ȵĴ�����ȫ����������������
            {
                C=Str[I];
                if (C>=sc<CT>::zero()&&C<='9')              //��ʮ��������,ֱ��׼���ж���һ��
                    continue;

                if (!IsHex)
                    return false;                           //����ʮ������ģʽ,�����ֵĴ����־ͷ��ؼ�

                if (C>=sc<CT>::A()&&C<=sc<CT>::F()||C>=sc<CT>::a()&&C<=sc<CT>::f())
                    continue;                               //ʮ������ʱ,��ǰ�ַ��ǺϷ���,����,׼���ж���һ��.
                else
                    return false;                           //����˵����ǰ���ǺϷ���ʮ���������ִ�
            }
            return true;                                    //ȫ�����ַ�����������,˵����ǰ���ǺϷ������ִ�.
        }

        //-----------------------------------------------------
        //��ȡStr�д��ײ���SP�ָ���֮ǰ�����ݵ�Result,Str�ڽ�ȡ�������SP֮��
        //����ֵ:<0����;>=0��ȡ�������ݳ���
        template<class CT>
        static int intercept(CT* &Str,const CT SP,CT *Result,uint32_t ResultSize)
        {
            if (!Str) return -1;
            CT* Pos=strchr(Str,SP);
            if (!Pos) return -2;
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
        static int intercept(CT* &Str,const CT SP,uint32_t &Result,const uint32_t radix=10)
        {
            if (!Str) return -1;
            CT* Pos=strchr(Str,SP);
            if (!Pos) return -2;
            *Pos=0;

            uint32_t Len=uint32_t(Pos-Str);
            Result=atoul(Str,radix);

            Str=Pos+1;
            return Len;
        }
    };


#if RX_CC == RX_CC_VC
    #pragma warning(disable:4200)
#endif

#if RX_CC == RX_CC_GCC||RX_CC_MINGW
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Warray-bounds"
#endif

    //-----------------------------------------------------
    //��װһ�����׵��ַ�������,����dtl�����ڲ�����ʱkey�ַ����洢
    //!!!����ע��!!!:�ڱ�����ʵ���ĺ�,�����㹻���ȵ�m_capacity�ռ�.
    template<class CT=char>
    struct tiny_string_t
    {
    private:
        //-------------------------------------------------
        tiny_string_t& operator=(const tiny_string_t&);
        tiny_string_t(const tiny_string_t&);
        //-------------------------------------------------
        uint16_t    m_capacity;                             //�����֪m_string�Ŀ�������
        uint16_t    m_length;                               //��¼m_string��ʵ�ʳ���.
        CT          m_string[0];                            //ʹ�õ������鶨�巽��,�ڵ�ǰ�ڴ�λ�����m_capacity���ֽ��ڴ��ʵ�ʵ��ַ���.
    public:
        tiny_string_t(uint16_t cap, const CT* str, uint32_t len = 0) :m_capacity(cap) { set(str,len); }
        //-------------------------------------------------
        uint16_t set(const CT* str, uint32_t len = 0)
        {
            if (is_empty(str)|| m_capacity==1)
            {
                m_length = 0;
                m_string[0] = 0;
                return 0;
            }
            else
            {
                //�����ַ����Ŀ���
                if (!len) len = (uint32_t)st::strlen(str);
                m_length = Min(len, uint32_t(m_capacity - 1));
                st::strncpy(m_string, str, m_length);
                m_string[m_length] = 0;
                return m_length;
            }
        }
        //-------------------------------------------------
        uint16_t length()const { return m_length; }
        const CT* c_str() const { return m_string; }
        operator const CT* ()const{return m_string;}
        //-------------------------------------------------
        bool operator <  (const tiny_string_t& str) const {return strcmp(m_string, str.m_string) < 0;}
        bool operator <= (const tiny_string_t& str) const {return strcmp(m_string, str.m_string) <= 0;}
        bool operator == (const tiny_string_t& str) const {return strcmp(m_string, str.m_string) == 0;}
        bool operator >  (const tiny_string_t& str) const {return strcmp(m_string, str.m_string) > 0;}
        bool operator >= (const tiny_string_t& str) const {return strcmp(m_string, str.m_string) >= 0;}
        bool operator != (const tiny_string_t& str) const {return strcmp(m_string, str.m_string) != 0;}
        //-------------------------------------------------
        bool operator <  (const CT *str) const {return strcmp(m_string, (is_empty(str)?"":str)) < 0;}
        bool operator <= (const CT *str) const {return strcmp(m_string, (is_empty(str)?"":str)) <= 0;}
        bool operator == (const CT *str) const {return strcmp(m_string, (is_empty(str)?"":str)) == 0;}
        bool operator >  (const CT *str) const {return strcmp(m_string, (is_empty(str)?"":str)) > 0;}
        bool operator >= (const CT *str) const {return strcmp(m_string, (is_empty(str)?"":str)) >= 0;}
        bool operator != (const CT *str) const {return strcmp(m_string, (is_empty(str)?"":str)) != 0;}
        //-------------------------------------------------
    };
#if RX_CC == RX_CC_GCC||RX_CC_MINGW
    #pragma GCC diagnostic pop
#endif

#if RX_CC == RX_CC_VC
    #pragma warning(default:4200)
#endif

    typedef tiny_string_t<char> tiny_stringc_t;
    typedef tiny_string_t<wchar_t> tiny_stringw_t;

    ////-----------------------------------------------------
    //�ڸ�����buff�ڴ���Ϲ�����״����󲢽��г�ʼ��
    //����ֵ:0�ڴ���С,����ʧ��;>0Ϊʵ�ʳ�ʼ��������str����
    template<class CT>
    uint32_t make_tiny_string(void* buff,uint32_t buffsize,const CT* str,uint32_t len=0)
    {
        typedef struct tiny_string_t<CT> string_t;
        if (buffsize <= sizeof(string_t))       //�����С�ߴ�
            return 0;

        //�ڸ����Ļ������Ϲ�������ַ�������
        uint32_t cap = buffsize - sizeof(string_t);
        string_t *s=ct::OC<string_t >((string_t*)buff, cap,str,len);
        return s->length();
    }
}


#endif
