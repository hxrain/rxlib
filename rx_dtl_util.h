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
    //统一封装了对原始字符串处理的函数
    class s
    {
    public:
        //-----------------------------------------------------
        //根据长度拷贝字符串
        //入口:dst目标缓冲区;dstMaxSize目标的最大容量;Src源串;SrcLen源串的长度
        //返回值:0失败,参数错误或目标缓冲区不足;其他为实际拷贝的长度
        template<class CT>
        inline uint32_t strcpy(CT* dst,uint32_t dstMaxSize,const CT* Src,uint32_t SrcLen=0)
        {
            dst[0]=0;
            if (is_empty(Src)) return 0;
            if (SrcLen==0) SrcLen=strlen(Src);
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
        inline uint32_t strcpy2(CT* dst,uint32_t dstMaxSize,const CT* Src,uint32_t SrcLen=0)
        {
            dst[0]=0;
            if (is_empty(Src)) return 0;
            if (SrcLen==0) SrcLen=strlen(Src);
            if (SrcLen>--dstMaxSize)
                SrcLen=dstMaxSize;								//总要保证缓冲区的最后要有一个0串结束符的位置
            strncpy(dst,Src,SrcLen);
            dst[SrcLen]=0;										//标记串结束符
            return SrcLen;
        }
        //-----------------------------------------------------
        //长度
        static size_t           strlen(const char* Str){return ::strlen(Str);}
        static size_t           strlen(const wchar_t* Str){return ::wcslen(Str);}

        //拷贝
        static char*            strcpy(char* dst,const char* src){return ::strcpy(dst,src);}
        static wchar_t*         strcpy(wchar_t* dst,const wchar_t* src){return wcscpy(dst,src);}

        //拼接
        static char*            strcat(char* dst,const char* src){return ::strcat(dst,src);}
        static wchar_t*         strcat(wchar_t* dst,const wchar_t* src){return wcscat(dst,src);}

        //计数拷贝
        static char*            strncpy(char* dst,const char* src,size_t len){return ::strncpy(dst,src,len);}
        static wchar_t*         strncpy(wchar_t* dst,const wchar_t* src,size_t len){return ::wcsncpy(dst,src,len);}

        //尾部拷贝(取src的后n位)
        template<class CT>
        static CT*              strrcpy(CT* dst,const CT* src,size_t n,size_t srclen=0)
        {
            if (!srclen)
                srclen=strlen(src);
            if (n>srclen) n=srclen;
            uint32_t P=n-srclen;
            return strncpy(dst,&src[P],n);
        }

        //比较,区分大小写
        static int              strcmp(const char* dst,const char* src){return ::strcmp(dst,src);}
        static int              strcmp(const wchar_t* dst,const wchar_t* src){return ::wcscmp(dst,src);}

        //比较,不区分大小写
#if RX_CC==RX_CC_VC
        static int              stricmp(const char *s1, const char *s2){return ::_strcmpi(s1,s2);}
#else
        static int              stricmp(const char *s1, const char *s2){return ::strcmpi(s1,s2);}
#endif
        static int              stricmp(const wchar_t *s1, const wchar_t *s2){return ::_wcsicmp(s1,s2);}

        //定长比较,区分大小写,规定最大长度

        static int              strncmp(const char *s1, const char *s2, size_t  maxlen){return ::strncmp(s1,s2,maxlen);}
        static int              strncmp(const wchar_t *s1, const wchar_t *s2, size_t  maxlen){return ::wcsncmp(s1,s2,maxlen);}

        //定长比较,不区分大小写,规定最大长度
#if RX_CC==RX_CC_BCC
        static int              strncmpi(const char *s1, const char *s2, size_t n){return ::strncmpi(s1,s2,n);}
        static int              strncmpi(const wchar_t *s1, const wchar_t *s2, size_t n){return ::wcsncmpi(s1,s2,n);}
#else
        static int              strncmpi(const char *s1, const char *s2, size_t n){return ::_strnicmp(s1,s2,n);}
        static int              strncmpi(const wchar_t *s1, const wchar_t *s2, size_t n){return ::_wcsnicmp(s1,s2,n);}
#endif

        //子串搜索
        static char *           strstr(char *s1, const char *s2){return ::strstr(s1,s2);}
        static wchar_t*         strstr(wchar_t *s1, const wchar_t *s2){return ::wcsstr(s1,s2);}
        static const char *     strstr(const char *s1, const char *s2){return ::strstr(s1,s2);}
        static const wchar_t*   strstr(const wchar_t *s1, const wchar_t *s2){return ::wcsstr(s1,s2);}

        //字符搜索
        static char *           strchr(char *s, int c){return ::strchr(s,c);}
        static wchar_t *        strchr(wchar_t *s, wchar_t c){ return ::wcschr(s, c); }
        static const char *     strchr(const char *s, int c){return ::strchr(s,c);}
        static const wchar_t *  strchr(const wchar_t *s, wchar_t c){ return ::wcschr(s, c); }

        //反向字符搜索
        static char *           strrchr(char *s, int c){return ::strrchr(s,c);}
        static wchar_t *        strrchr(wchar_t *s, int c){return ::wcsrchr(s,c);}
        static const char *     strrchr(const char *s, int c){return ::strrchr(s,c);}
        static const wchar_t *  strrchr(const wchar_t *s, int c){return ::wcsrchr(s,c);}

        //字符串转换为大写

#if RX_CC==RX_CC_VC
        static char *           strupr(char *s){return ::_strupr(s);}
#else
        static char *           strupr(char *s){return ::strupr(s);}
#endif
        static wchar_t *        strupr(wchar_t *s){return ::_wcsupr(s);}

        //字符串转换为小写

#if RX_CC==RX_CC_VC
        static char *           strlwr(char *s){return ::_strlwr(s);}
#else
        static char *           strlwr(char *s){return ::strlwr(s);}
#endif
        static wchar_t *        strlwr(wchar_t *s){return ::_wcslwr(s);}

        //字符串转换扩展,直接拷贝到目标缓冲器
        template<class CT>
        static CT *             strupr(const CT *s,CT* TmpBuf){strcpy(TmpBuf,s);return strupr(TmpBuf);}
        template<class CT>
        static CT *             strlwr(const CT *s,CT* TmpBuf){strcpy(TmpBuf,s);return strlwr(TmpBuf);}

        //字符串转为无符号整数
        static uint32_t         atoul(const char* s,int radix=0){return ::strtoul(s,NULL,radix);}
        static uint32_t         atoul(const wchar_t* s,int radix=0){return ::wcstoul(s,NULL,radix);}
        template<class CT>
        static uint32_t         atoul(const CT* s,int radix,uint32_t DefVal){if (is_empty(s)) return DefVal;else return atoul(s,radix);}

        //字符串转换为整数
        static int              atoi(const char* s,int radix=0){return ::strtol(s,NULL,radix);}
        static int              atoi(const wchar_t* s,int radix=0){return ::wcstol(s,NULL,radix);}
        template<class CT>
        static int              atoi(const CT* s,int radix,int DefVal){if (is_empty(s))return DefVal;else return atoi(s,radix);}

        //字符串转换为整数
        static uint64_t         atoi64(const char* s){return ::_atoi64(s);}
        static uint64_t         atoi64(const wchar_t* s){return ::_wtoi64(s);}

        //字符串转换为浮点数
        static double           atof(const char* s){return ::atof(s);}
        static double           atof(const wchar_t* s){return ::_wtof(s);}

        //数字转换为字符串

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

        //语法糖,数字转为0x前缀的十六进制串
        template<class CT>
        static CT*              itox(int value,CT* Buff){Buff[0]='0';Buff[1]='x';itoa(value,&Buff[2],16);return Buff;}
        template<class CT>
        static CT*              itox64(uint64_t value,CT* Buff){Buff[0]='0';Buff[1]='x';itoa64(value,&Buff[2],16);return Buff;}
        template<class CT>
        static CT*              ultox(uint32_t value,CT* Buff){Buff[0]='0';Buff[1]='x';ultoa(value,&Buff[2],16);return Buff;}

        //浮点数转为字符串
        static char*            ftoa(const double& f,char* string,const char* Fmt=NULL){if (!Fmt) Fmt="%.2f";sprintf(string,Fmt,f);return string;}
        static wchar_t*         ftoa(const double& f,wchar_t* string,const wchar_t* Fmt=NULL){if (!Fmt) Fmt=L"%.2f";wprintf(string,Fmt,f);return string;}

        //判断是否为十进制数字字符
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
        //判断是否为十进制数字和字母
        static bool             isalnum(char c){return (c>='0'&&c<='9')||(c>='A'&&c<='Z')||(c>='a'&&c<='z');}
        static bool             isalnum(wchar_t c){return (c>=L'0'&&c<=L'9')||(c>=L'A'&&c<=L'Z')||(c>=L'a'&&c<=L'z');}

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
        //语法糖,对输入S进行判断,如果是空值则返回给定的默认值
        template<class CT>
        static const CT* value(const CT* S,const CT* DefValue)
        {
            if (is_empty(S)) return DefValue;
            return S;
        }
        //-------------------------------------------------
        //统计给定的串S中指定字符Char的数量
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
        //统计给定的串S中指定子串Str的数量
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
        //从S中选择第一个Char之前的串到Result中
        //返回值:NULL失败(在S中没有Char);非空,成功(S中当前Char的位置指针)
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
        //尝试在Str串指定的偏移位置后面,定位查找SubStr子串
        //返回值:-1未找到;其他为结果偏移量(相对于Str而不是相对于StartIdx)
        template<class CT>
        static int pos(const CT* Str,const CT *SubStr,uint32_t StartIdx=0)
        {
            if (is_empty(Str)||StartIdx>=strlen(Str)) return -1;
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
            {//有A串,那么就记录A串的长度,准备返回使用;查找A串的位置
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
                TP=strstr(TP+Alen,B);                      //在A串出现的位置之后查找B串的位置
                if (!TP) return -3;
            }
            else
                TP=src+srclen;                                  //没有B串,那么就指向src的尾字符

            PB=uint32_t(TP-src);                                  //得到B串的出现位置
            return Alen;
        }

        //在当前串中顺序查找A与B开始的位置,比如在"abc=[123]456"中查找定位'['与']'出现的位置
        //如果A为NULL,则从src头开始;B为NULL则到src结尾
        //返回值:true查找成功,A与B都存在,并且B在A的后面;false失败
        //出口:PA是A的位置偏移量;PB是B的位置偏移量
        template<class CT>
        static bool pos(const CT* src,const CT* A,const CT* B,uint32_t &PA,uint32_t &PB)
        {
            if (src==NULL) return false;
            return Pos(src,strlen(src),A,B,PA,PB)>=0;
        }
        //-----------------------------------------------------
        //找到在A串和B串之间的字符串,结果中不包含A与B(A为空则从头开始截取,B为空则到尾结束)
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
        //将src中AStr后面的内容放入结果
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
    //封装一个简易的字符串功能,用于dtl容器内部的临时key字符串存储
    template<class CT=char>
    struct tiny_string
    {
    private:
        //-------------------------------------------------
        tiny_string& operator=(const tiny_string&);
        tiny_string(const tiny_string&);
        //-------------------------------------------------
        uint16_t    m_capacity;                             //必须告知m_string的可用容量
        uint16_t    m_length;                               //记录m_string的实际长度.
        CT          m_string[0];                            //使用弹性数组定义方法,在当前内存位置向后m_capacity个字节内存放实际的字符串.
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
                //进行字符串的拷贝
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
    //在给定的buff内存块上构造简易串对象并进行初始化
    //返回值:0内存块过小,构造失败;>0为实际初始化拷贝的str长度
    template<class CT>
    uint32_t make_tiny_string(void* buff,uint32_t buffsize,const CT* str,uint32_t len=0)
    {
        typedef struct tiny_string<CT> string_t;
        if (buffsize <= sizeof(string_t))       //检查最小尺寸
            return 0;

        //在给定的缓冲区上构造简易字符串对象
        uint32_t cap = buffsize - sizeof(string_t);
        string_t *s=ct::OC<string_t >((string_t*)buff, cap,str,len);
        return s->length();
    }
}


#endif
