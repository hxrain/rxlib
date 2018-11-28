#ifndef _RX_STR_UTIL_TINY_H_
#define _RX_STR_UTIL_TINY_H_

#include "rx_cc_macro.h"
#include "rx_str_util_std.h"

namespace rx
{
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

    //-----------------------------------------------------
    //简易的字符串拼装器
    template<class CT>
    class strcat_t
    {
        CT         *m_buff;
        uint32_t    m_max_size;
        uint32_t   &m_size;
        uint32_t    m_dummy_size;
    public:
        strcat_t(CT *buff, uint32_t max_size) :m_buff(buff),m_max_size(max_size), m_size(m_dummy_size), m_dummy_size(0){}
        strcat_t(CT *buff, uint32_t max_size, uint32_t initsize) :m_buff(buff), m_max_size(max_size), m_size(m_dummy_size) { m_dummy_size = initsize; }
        uint32_t capacity() { return m_max_size; }
        uint32_t size() { return m_size; }
        //拼装字符
        strcat_t& operator<<(const CT c)
        {
            if (m_size < m_max_size - 1)
                m_buff[m_size++] = c;
            m_buff[m_size] = 0;
            return *this;
        }
        //拼装字符串
        strcat_t& operator<<(const CT *str)
        {
            uint32_t rc = st::strcpy(m_buff + m_size, m_max_size - m_size, str);
            if (rc) m_size += rc;
            return *this;
        }
        //直接赋值
        strcat_t& operator=(const CT *str)
        {
            m_size = st::strcpy(m_buff, m_max_size, str);
            return *this;
        }
    };

    typedef strcat_t<char> strcat_ct;
    typedef strcat_t<wchar_t> strcat_wt;
}


#endif
