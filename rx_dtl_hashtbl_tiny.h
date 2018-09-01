#ifndef _RX_DTL_HASHTBL_TINY_H_
#define _RX_DTL_HASHTBL_TINY_H_

#include "rx_cc_macro.h"
#include "rx_raw_hashtbl_tiny.h"
#include "rx_hash_data.h"
#include "rx_hash_int.h"

namespace rx
{
    //-----------------------------------------------------
    //针对不同的值进行哈希函数的适配封装(默认的通用型)
    template<class val_t,class hashfunc_t>
    class hashfunc_adaptor_t 
    {
        hashfunc_t  m_func;
    public:
        hashfunc_adaptor_t():m_func(NULL) {}
        hashfunc_t bind(hashfunc_t func) { hashfunc_t old = m_func; m_func = func; return old; }
        uint32_t operator()(const val_t& val)const { return m_func(val,sizeof(val)); }
    };

    //-----------------------------------------------------
    //针对不同的值进行哈希函数的适配封装(针对整数值类型)
    template<>
    class hashfunc_adaptor_t<uint32_t, rx_int_hash32_t>
    {
        rx_int_hash32_t m_func;
    public:
        hashfunc_adaptor_t() :m_func(rx_hash_skeeto_triple) {}
        rx_int_hash32_t bind(rx_int_hash32_t func) { rx_int_hash32_t old = m_func; m_func = func; return old; }
        uint32_t operator()(const uint32_t& val)const { return m_func(val); }
    };
    typedef hashfunc_adaptor_t<uint32_t, rx_int_hash32_t> uint_hash_adaptor_t;

    //-----------------------------------------------------
    //uint32_t类型的集合
    template<uint32_t max_set_size>
    class uint32_set_t :public raw_set_t<uint32_t, max_set_size, uint_hash_adaptor_t> {};
}

#endif
