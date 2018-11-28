#ifndef _RX_STR_TINY_H_
#define _RX_STR_TINY_H_

#include "rx_cc_macro.h"
#include "rx_str_util_std.h"

namespace rx
{
#pragma pack(push,1)
    //-----------------------------------------------------
    //封装一个简易的字符串功能,用于dtl容器内部的临时key字符串存储
    template<class CT=char,uint16_t max_str_size=0>
    struct tiny_string_t
    {
        //-------------------------------------------------
        template<class ct,uint16_t buff_size>
        class head_t                                        //缓冲区内置的小串
        {
        public:
            head_t():length(0){}
            //内置缓冲区,无需绑定.仅用于接口兼容.
            void bind(ct* buff, uint16_t size) { rx_alert("don't call this method."); }
            //获取内置缓冲区的容量
            const uint16_t capacity()const{return buff_size;}

            uint16_t    length;                             //记录buff的实际长度.
            ct          buff[buff_size];                    //字符串缓冲器指针.
        };

        //-------------------------------------------------
        template<class ct>
        class head_t<ct,0>                                  //特化,使用外置绑定的缓冲区
        {
        public:
            head_t():max_size(0),length(0),buff(NULL){}
            //绑定缓冲区指针与容量
            void bind(ct* buf, uint16_t size) { buff = buf; max_size = size; length = 0; }
            //获知绑定的缓冲区的容量
            const uint16_t capacity()const{return max_size;}

            uint16_t    max_size;                           //必须告知buff的可用容量
            uint16_t    length;                             //记录buff的实际长度.
            ct         *buff;                               //字符串缓冲器指针.
        };

        //-------------------------------------------------
        head_t<CT,max_str_size>  m_head;                    //真正使用的小串缓冲区对象
    private:
        //-------------------------------------------------
        //不禁用赋值运算符,默认的时候赋值就进行m_head的硬拷贝:指针就指针;数组就数组.
        //tiny_string_t& operator=(const tiny_string_t&);
        tiny_string_t(const tiny_string_t&);
        //-------------------------------------------------
    public:
        tiny_string_t(uint32_t cap, CT* buff){m_head.bind(buff,cap);}
        tiny_string_t(){}
        tiny_string_t(const CT* str){set(str);}
        //-------------------------------------------------
        //使用给定的字符串进行赋值
        //返回值:真正拷贝的串尺寸.
        uint32_t set(const CT* str, uint32_t len = 0)
        {
            if (is_empty(str)|| m_head.capacity()==1)
            {
                m_head.length = 0;
                m_head.buff[0] = 0;
                return 0;
            }
            else
            {
                //进行字符串的拷贝
                if (!len)
                    len = (uint32_t)st::strlen(str);
                m_head.length = Min(len, uint32_t(m_head.capacity() - 1));
                st::strncpy(m_head.buff, str, m_head.length);
                m_head.buff[m_head.length] = 0;
                return m_head.length;
            }
        }
        //-------------------------------------------------
        //格式化生成内部串
        bool fmt(const CT *str, va_list arg)
        {
            m_head.length = 0;
            m_head.buff[0] = 0;

            int rc=st::vsnprintf(m_head.buff, m_head.capacity(), str, arg);
            if (rc < 0 || rc >= m_head.capacity())
                return false;
            m_head.length = rc;
            return true;
        }
        bool fmt(const CT *str,...)
        {
            va_list	ap;
            va_start(ap, str);
            return fmt(str, ap);
        }
        //-------------------------------------------------
        //绑定缓冲器并进行字符串的赋值
        uint32_t bind(CT* buff,uint32_t cap,const CT* str,uint32_t len = 0){m_head.bind(buff,cap);return set(str,len);}
        //-------------------------------------------------
        //获知缓冲区容量
        uint32_t capacity()const { return m_head.capacity(); }
        //获知缓冲区内数据长度
        uint32_t size()const { return m_head.length; }
        //获知缓冲区内容
        const CT* c_str() const { return m_head.buff; }
        operator const CT* ()const {return m_head.buff;}
        //-------------------------------------------------
        bool operator <  (const tiny_string_t& str) const {return st::strcmp(m_head.buff, str.m_head.buff) < 0;}
        bool operator <= (const tiny_string_t& str) const {return st::strcmp(m_head.buff, str.m_head.buff) <= 0;}
        bool operator == (const tiny_string_t& str) const {return st::strcmp(m_head.buff, str.m_head.buff) == 0;}
        bool operator >  (const tiny_string_t& str) const {return st::strcmp(m_head.buff, str.m_head.buff) > 0;}
        bool operator >= (const tiny_string_t& str) const {return st::strcmp(m_head.buff, str.m_head.buff) >= 0;}
        bool operator != (const tiny_string_t& str) const {return st::strcmp(m_head.buff, str.m_head.buff) != 0;}
        //-------------------------------------------------
        bool operator <  (const CT *str) const {return st::strcmp(m_head.buff, (is_empty(str)?sc<CT>::empty():str)) < 0;}
        bool operator <= (const CT *str) const {return st::strcmp(m_head.buff, (is_empty(str)?sc<CT>::empty():str)) <= 0;}
        bool operator == (const CT *str) const {return st::strcmp(m_head.buff, (is_empty(str)?sc<CT>::empty():str)) == 0;}
        bool operator >  (const CT *str) const {return st::strcmp(m_head.buff, (is_empty(str)?sc<CT>::empty():str)) > 0;}
        bool operator >= (const CT *str) const {return st::strcmp(m_head.buff, (is_empty(str)?sc<CT>::empty():str)) >= 0;}
        bool operator != (const CT *str) const {return st::strcmp(m_head.buff, (is_empty(str)?sc<CT>::empty():str)) != 0;}
        //-------------------------------------------------
    };
#pragma pack(pop)

    //-------------------------------------------------------
    //char类型的小串类
    typedef tiny_string_t<char> tiny_string_head_ct;
    //wchar_t类型的小串类
    typedef tiny_string_t<wchar_t> tiny_string_head_wt;

    ////-----------------------------------------------------
    //在给定的buff内存块上构造简易串对象并进行初始化
    //返回值:NULL失败;其他为串头对象指针
    template<class CT>
    tiny_string_t<CT>* make_tiny_string(void* buff,uint32_t buffsize,const CT* str,uint32_t len=0)
    {
        typedef struct tiny_string_t<CT> string_t;
        if (buffsize <= sizeof(string_t))       //检查最小尺寸
            return NULL;

        //在给定的缓冲区上构造简易字符串对象
        uint32_t cap = buffsize - sizeof(string_t);
        CT *strbuf=(CT*)((uint8_t*)buff+sizeof(string_t));
        string_t *s=ct::OC<string_t >((string_t*)buff, cap,strbuf);
        s->set(str,len);
        return s;
    }
}


#endif
