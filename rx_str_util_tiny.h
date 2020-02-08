#ifndef _RX_STR_UTIL_TINY_H_
#define _RX_STR_UTIL_TINY_H_

#include "rx_cc_macro.h"
#include "rx_str_util_std.h"
#include "rx_str_util_ex.h"

namespace rx
{
    //-----------------------------------------------------
    //数字转为字符串的工具对象
    template<class CT>
    class n2str
    {
        CT  m_buff[32];
    public:
        n2str() { m_buff[0] = 0; }
        n2str(uint32_t n, uint32_t r = 10) { st::ultoa(n, m_buff, r); }
        n2str(int64_t n, uint32_t r = 10) { st::itoa64(n, m_buff, r); }
        operator CT* ()const { return m_buff; }
        operator const CT* ()const { return m_buff; }
        const CT* c_str() const {return m_buff;}
        const CT* operator()(uint32_t n, uint32_t r = 10) { st::utoa(n, m_buff, r); return m_buff; }
        const CT* operator()(int64_t n, uint32_t r = 10) { st::itoa64(n, m_buff, r); return m_buff; }
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
