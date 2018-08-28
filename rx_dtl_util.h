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
    //ͳһ��װ�˶�ԭʼ�ַ�������ĺ���
    class s
    {
    public:
        //-----------------------------------------------------
        //���ݳ��ȿ����ַ���
        //���:dstĿ�껺����;dstMaxSizeĿ����������;SrcԴ��;SrcLenԴ���ĳ���
        //����ֵ:0ʧ��,���������Ŀ�껺��������;����Ϊʵ�ʿ����ĳ���
        template<class CT>
        inline uint32_t strcpy(CT* dst,uint32_t dstMaxSize,const CT* Src,uint32_t SrcLen=0)
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
        inline uint32_t strcpy2(CT* dst,uint32_t dstMaxSize,const CT* Src,uint32_t SrcLen=0)
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
        //����
        static size_t           strlen(const char* Str){return ::strlen(Str);}
        static size_t           strlen(const wchar_t* Str){return ::wcslen(Str);}

        //����
        static char*            strcpy(char* dst,const char* src){return ::strcpy(dst,src);}
        static wchar_t*         strcpy(wchar_t* dst,const wchar_t* src){return wcscpy(dst,src);}

        //ƴ��
        static char*            strcat(char* dst,const char* src){return ::strcat(dst,src);}
        static wchar_t*         strcat(wchar_t* dst,const wchar_t* src){return wcscat(dst,src);}

        //��������
        static char*            strncpy(char* dst,const char* src,size_t len){return ::strncpy(dst,src,len);}
        static wchar_t*         strncpy(wchar_t* dst,const wchar_t* src,size_t len){return ::wcsncpy(dst,src,len);}

        //β������(ȡsrc�ĺ�nλ)
        template<class CT>
        static CT*              strrcpy(CT* dst,const CT* src,size_t n,size_t srclen=0)
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

        static int              strncmp(const char *s1, const char *s2, size_t  maxlen){return ::strncmp(s1,s2,maxlen);}
        static int              strncmp(const wchar_t *s1, const wchar_t *s2, size_t  maxlen){return ::wcsncmp(s1,s2,maxlen);}

        //�����Ƚ�,�����ִ�Сд,�涨��󳤶�
#if RX_CC==RX_CC_BCC
        static int              strncmpi(const char *s1, const char *s2, size_t n){return ::strncmpi(s1,s2,n);}
        static int              strncmpi(const wchar_t *s1, const wchar_t *s2, size_t n){return ::wcsncmpi(s1,s2,n);}
#else
        static int              strncmpi(const char *s1, const char *s2, size_t n){return ::_strnicmp(s1,s2,n);}
        static int              strncmpi(const wchar_t *s1, const wchar_t *s2, size_t n){return ::_wcsnicmp(s1,s2,n);}
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
        static CT*              itox(int value,CT* Buff){Buff[0]='0';Buff[1]='x';itoa(value,&Buff[2],16);return Buff;}
        template<class CT>
        static CT*              itox64(uint64_t value,CT* Buff){Buff[0]='0';Buff[1]='x';itoa64(value,&Buff[2],16);return Buff;}
        template<class CT>
        static CT*              ultox(uint32_t value,CT* Buff){Buff[0]='0';Buff[1]='x';ultoa(value,&Buff[2],16);return Buff;}

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
                TP=strstr(TP+Alen,B);                      //��A�����ֵ�λ��֮�����B����λ��
                if (!TP) return -3;
            }
            else
                TP=src+srclen;                                  //û��B��,��ô��ָ��src��β�ַ�

            PB=uint32_t(TP-src);                                  //�õ�B���ĳ���λ��
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
            return RL==RLIB_StrCpy(Result,ResultMaxSize,&src[APos+Alen],RL);
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
        static bool sub(const CT* src,const CT* AStr,CT* Result,uint32_t ResultMaxSize){return SubStr(src,AStr,"",Result,ResultMaxSize);}

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
    template<class CT=char>
    struct tiny_string
    {
    private:
        //-------------------------------------------------
        tiny_string& operator=(const tiny_string&);
        tiny_string(const tiny_string&);
        //-------------------------------------------------
        uint16_t    m_capacity;                             //�����֪m_string�Ŀ�������
        uint16_t    m_length;                               //��¼m_string��ʵ�ʳ���.
        CT          m_string[0];                            //ʹ�õ������鶨�巽��,�ڵ�ǰ�ڴ�λ�����m_capacity���ֽ��ڴ��ʵ�ʵ��ַ���.
    public:
        tiny_string(uint16_t cap, const CT* str, uint32_t len = 0) :m_capacity(cap) { set(str,len); }
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
                if (!len) len = (uint32_t)s::strlen(str);
                m_length = Min(len, uint32_t(m_capacity - 1));
                s::strncpy(m_string, str, m_length);
                m_string[m_length] = 0;
                return m_length;
            }
        }
        //-------------------------------------------------
        uint16_t length()const { return m_length; }
        const CT* c_str() const { return m_string; }
        operator const CT* ()const{return m_string;}
        //-------------------------------------------------
        bool operator <  (const tiny_string& str) const {return strcmp(m_string, str.m_string) < 0;}
        bool operator <= (const tiny_string& str) const {return strcmp(m_string, str.m_string) <= 0;}
        bool operator == (const tiny_string& str) const {return strcmp(m_string, str.m_string) == 0;}
        bool operator >  (const tiny_string& str) const {return strcmp(m_string, str.m_string) > 0;}
        bool operator >= (const tiny_string& str) const {return strcmp(m_string, str.m_string) >= 0;}
        bool operator != (const tiny_string& str) const {return strcmp(m_string, str.m_string) != 0;}
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

    typedef tiny_string<char> tiny_stringc;
    typedef tiny_string<wchar_t> tiny_stringw;

    ////-----------------------------------------------------
    //�ڸ�����buff�ڴ���Ϲ�����״����󲢽��г�ʼ��
    //����ֵ:0�ڴ���С,����ʧ��;>0Ϊʵ�ʳ�ʼ��������str����
    template<class CT>
    uint32_t make_tiny_string(void* buff,uint32_t buffsize,const CT* str,uint32_t len=0)
    {
        typedef struct tiny_string<CT> string_t;
        if (buffsize <= sizeof(string_t))       //�����С�ߴ�
            return 0;

        //�ڸ����Ļ������Ϲ�������ַ�������
        uint32_t cap = buffsize - sizeof(string_t);
        string_t *s=ct::OC<string_t >((string_t*)buff, cap,str,len);
        return s->length();
    }
}


#endif
