#ifndef _RX_STR_TINY_H_
#define _RX_STR_TINY_H_

#include "rx_cc_macro.h"
#include "rx_str_util.h"

namespace rx
{
    //-----------------------------------------------------
    //封装一个简易的字符串功能,用于dtl容器内部的临时key字符串存储
    template<class CT=char>
    struct tiny_string_head_t
    {
    private:
        //-------------------------------------------------
        tiny_string_head_t& operator=(const tiny_string_head_t&);
        tiny_string_head_t(const tiny_string_head_t&);
        //-------------------------------------------------
        uint32_t    m_capacity;                             //必须告知m_string的可用容量
        uint32_t    m_length;                               //记录m_string的实际长度.
        CT         *m_string;                               //字符串缓冲器指针.
    public:
        tiny_string_head_t(uint32_t cap, CT* buff) :m_capacity(cap),m_length(0),m_string(buff){}
        tiny_string_head_t() :m_capacity(0),m_length(0),m_string(){}
        //-------------------------------------------------
        //使用给定的字符串进行赋值
        //返回值:真正拷贝的串尺寸.
        uint32_t set(const CT* str, uint32_t len = 0)
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
                if (!len)
                    len = (uint32_t)st::strlen(str);
                m_length = Min(len, uint32_t(m_capacity - 1));
                st::strncpy(m_string, str, m_length);
                m_string[m_length] = 0;
                return m_length;
            }
        }
        uint32_t bind(CT* buff,uint32_t cap,const CT* str,uint32_t len = 0){m_string=buff;m_capacity=cap;return set(str,len);}
        //-------------------------------------------------
        uint32_t capacity()const { return m_capacity; }
        uint32_t length()const { return m_length; }
        const CT* c_str() const { return m_string; }
        operator const CT* ()const {return m_string;}
        //-------------------------------------------------
        bool operator <  (const tiny_string_head_t& str) const {return st::strcmp(m_string, str.m_string) < 0;}
        bool operator <= (const tiny_string_head_t& str) const {return st::strcmp(m_string, str.m_string) <= 0;}
        bool operator == (const tiny_string_head_t& str) const {return st::strcmp(m_string, str.m_string) == 0;}
        bool operator >  (const tiny_string_head_t& str) const {return st::strcmp(m_string, str.m_string) > 0;}
        bool operator >= (const tiny_string_head_t& str) const {return st::strcmp(m_string, str.m_string) >= 0;}
        bool operator != (const tiny_string_head_t& str) const {return st::strcmp(m_string, str.m_string) != 0;}
        //-------------------------------------------------
        bool operator <  (const CT *str) const {return st::strcmp(m_string, (is_empty(str)?sc<CT>::empty():str)) < 0;}
        bool operator <= (const CT *str) const {return st::strcmp(m_string, (is_empty(str)?sc<CT>::empty():str)) <= 0;}
        bool operator == (const CT *str) const {return st::strcmp(m_string, (is_empty(str)?sc<CT>::empty():str)) == 0;}
        bool operator >  (const CT *str) const {return st::strcmp(m_string, (is_empty(str)?sc<CT>::empty():str)) > 0;}
        bool operator >= (const CT *str) const {return st::strcmp(m_string, (is_empty(str)?sc<CT>::empty():str)) >= 0;}
        bool operator != (const CT *str) const {return st::strcmp(m_string, (is_empty(str)?sc<CT>::empty():str)) != 0;}
        //-------------------------------------------------
    };

    typedef tiny_string_head_t<char> tiny_string_head_ct;
    typedef tiny_string_head_t<wchar_t> tiny_string_head_wt;

    ////-----------------------------------------------------
    //在给定的buff内存块上构造简易串对象并进行初始化
    //返回值:NULL失败;其他为串头对象指针
    template<class CT>
    tiny_string_head_t<CT>* make_tiny_string(void* buff,uint32_t buffsize,const CT* str,uint32_t len=0)
    {
        typedef struct tiny_string_head_t<CT> string_t;
        if (buffsize <= sizeof(string_t))       //检查最小尺寸
            return NULL;

        //在给定的缓冲区上构造简易字符串对象
        uint32_t cap = buffsize - sizeof(string_t);
        CT *strbuf=(CT*)((uint8_t*)buff+sizeof(string_t));
        string_t *s=ct::OC<string_t >((string_t*)buff, cap,strbuf);
        s->set(str,len);
        return s;
    }

    //-----------------------------------------------------
    //数字转为字符串的工具对象
    template<class CT>
    class n2str
    {
        CT  m_str[32];
    public:
        n2str(){m_str[0]=0;}
        n2str(uint32_t n,uint32_t r=10){st::ultoa(n,m_str,r);}
        n2str(int64_t n,uint32_t r=10){st::itoa64(n,m_str,r);}
        operator CT* ()const {return m_str;}
        operator const CT* ()const {return m_str;}
        const CT* operator()(uint32_t n,uint32_t r=10){st::ultoa(n,m_str,r);return m_str;}
        const CT* operator()(int64_t n,uint32_t r=10){st::itoa64(n,m_str,r);return m_str;}
    };
    typedef n2str<char> n2s;
    typedef n2str<wchar_t> n2w;
}


#endif
