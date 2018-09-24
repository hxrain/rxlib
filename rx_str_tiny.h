#ifndef _RX_STR_TINY_H_
#define _RX_STR_TINY_H_

#include "rx_cc_macro.h"
#include "rx_str_util.h"

namespace rx
{
#pragma pack(push,1)
    //-----------------------------------------------------
    //封装一个简易的字符串功能,用于dtl容器内部的临时key字符串存储
    template<class CT=char,uint16_t max_str_size=0>
    struct tiny_string_head_t
    {
        //-------------------------------------------------
        template<class ct,uint16_t buff_size>
        class str_head
        {
        public:
            str_head():length(0){}
            const uint16_t capacity()const{return buff_size;}
            void bind(ct* buff,uint16_t size){rx_alert("don't call this method.");}
            uint16_t    length;                             //记录buff的实际长度.
            ct          buff[buff_size];                    //字符串缓冲器指针.
        };

        //-------------------------------------------------
        template<class ct>
        class str_head<ct,0>
        {
        public:
            str_head():max_size(0),length(0),buff(NULL){}
            const uint16_t capacity()const{return max_size;}
            void bind(ct* buf,uint16_t size){buff=buf;max_size=size;length=0;}
            uint16_t    max_size;                           //必须告知buff的可用容量
            uint16_t    length;                             //记录buff的实际长度.
            ct         *buff;                               //字符串缓冲器指针.
        };
        //-------------------------------------------------
        str_head<CT,max_str_size>  m_tinystr;
    private:
        //-------------------------------------------------
        tiny_string_head_t& operator=(const tiny_string_head_t&);
        tiny_string_head_t(const tiny_string_head_t&);
        //-------------------------------------------------
    public:
        tiny_string_head_t(uint32_t cap, CT* buff){m_tinystr.bind(buff,cap);}
        tiny_string_head_t(){}
        tiny_string_head_t(const CT* str){set(str);}
        //-------------------------------------------------
        //使用给定的字符串进行赋值
        //返回值:真正拷贝的串尺寸.
        uint32_t set(const CT* str, uint32_t len = 0)
        {
            if (is_empty(str)|| m_tinystr.capacity()==1)
            {
                m_tinystr.length = 0;
                m_tinystr.buff[0] = 0;
                return 0;
            }
            else
            {
                //进行字符串的拷贝
                if (!len)
                    len = (uint32_t)st::strlen(str);
                m_tinystr.length = Min(len, uint32_t(m_tinystr.capacity() - 1));
                st::strncpy(m_tinystr.buff, str, m_tinystr.length);
                m_tinystr.buff[m_tinystr.length] = 0;
                return m_tinystr.length;
            }
        }
        //绑定缓冲器并进行字符串的赋值
        uint32_t bind(CT* buff,uint32_t cap,const CT* str,uint32_t len = 0){m_tinystr.bind(buff,cap);return set(str,len);}
        //-------------------------------------------------
        uint32_t capacity()const { return m_tinystr.capacity(); }
        uint32_t length()const { return m_tinystr.length; }
        const CT* c_str() const { return m_tinystr.buff; }
        operator const CT* ()const {return m_tinystr.buff;}
        //-------------------------------------------------
        bool operator <  (const tiny_string_head_t& str) const {return st::strcmp(m_tinystr.buff, str.m_tinystr.buff) < 0;}
        bool operator <= (const tiny_string_head_t& str) const {return st::strcmp(m_tinystr.buff, str.m_tinystr.buff) <= 0;}
        bool operator == (const tiny_string_head_t& str) const {return st::strcmp(m_tinystr.buff, str.m_tinystr.buff) == 0;}
        bool operator >  (const tiny_string_head_t& str) const {return st::strcmp(m_tinystr.buff, str.m_tinystr.buff) > 0;}
        bool operator >= (const tiny_string_head_t& str) const {return st::strcmp(m_tinystr.buff, str.m_tinystr.buff) >= 0;}
        bool operator != (const tiny_string_head_t& str) const {return st::strcmp(m_tinystr.buff, str.m_tinystr.buff) != 0;}
        //-------------------------------------------------
        bool operator <  (const CT *str) const {return st::strcmp(m_tinystr.buff, (is_empty(str)?sc<CT>::empty():str)) < 0;}
        bool operator <= (const CT *str) const {return st::strcmp(m_tinystr.buff, (is_empty(str)?sc<CT>::empty():str)) <= 0;}
        bool operator == (const CT *str) const {return st::strcmp(m_tinystr.buff, (is_empty(str)?sc<CT>::empty():str)) == 0;}
        bool operator >  (const CT *str) const {return st::strcmp(m_tinystr.buff, (is_empty(str)?sc<CT>::empty():str)) > 0;}
        bool operator >= (const CT *str) const {return st::strcmp(m_tinystr.buff, (is_empty(str)?sc<CT>::empty():str)) >= 0;}
        bool operator != (const CT *str) const {return st::strcmp(m_tinystr.buff, (is_empty(str)?sc<CT>::empty():str)) != 0;}
        //-------------------------------------------------
    };
#pragma pack(pop)

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
        CT  m_tinystr[32];
    public:
        n2str(){m_tinystr[0]=0;}
        n2str(uint32_t n,uint32_t r=10){st::ultoa(n,m_tinystr,r);}
        n2str(int64_t n,uint32_t r=10){st::itoa64(n,m_tinystr,r);}
        operator CT* ()const {return m_tinystr;}
        operator const CT* ()const {return m_tinystr;}
        const CT* operator()(uint32_t n,uint32_t r=10){st::ultoa(n,m_tinystr,r);return m_tinystr;}
        const CT* operator()(int64_t n,uint32_t r=10){st::itoa64(n,m_tinystr,r);return m_tinystr;}
    };
    typedef n2str<char>     n2s;
    typedef n2str<wchar_t>  n2w;
}


#endif
