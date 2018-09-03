#ifndef _RX_DTL_HASHTBL_TINY_H_
#define _RX_DTL_HASHTBL_TINY_H_

#include "rx_cc_macro.h"
#include "rx_raw_hashtbl_tiny.h"
#include "rx_hash_data.h"
#include "rx_hash_int.h"

namespace rx
{
    //-----------------------------------------------------
    //简单哈希表的哈希函数适配器(默认的通用型)
    template<class val_t,class hashfunc_t>
    class hashfunc_adaptor_t 
    {
        hashfunc_t  m_func;
    public:
        //默认使用murmur哈希函数
        hashfunc_adaptor_t():m_func(rx_hash_murmur) {}
        //可以绑定其他种类的函数
        hashfunc_t bind(hashfunc_t func) { hashfunc_t old = m_func; m_func = func; return old; }
        //对给定的值进行真正的哈希值计算
        uint32_t operator()(const val_t& val)const { return m_func(val,sizeof(val)); }
    };

    //-----------------------------------------------------
    //简单哈希表的哈希函数适配器(针对整型数的偏特化版本)
    template<>
    class hashfunc_adaptor_t<uint32_t, rx_int_hash32_t>
    {
        rx_int_hash32_t m_func;
    public:
        //默认使用skeeto三绕哈希函数
        hashfunc_adaptor_t() :m_func(rx_hash_skeeto_triple) {}
        //可以绑定其他的整数哈希函数
        rx_int_hash32_t bind(rx_int_hash32_t func) { rx_int_hash32_t old = m_func; m_func = func; return old; }
        //进行整数哈希值的计算
        uint32_t operator()(const uint32_t& val)const { return m_func(val); }
    };
    typedef hashfunc_adaptor_t<uint32_t, rx_int_hash32_t> uint_hash_adaptor_t;

    //-----------------------------------------------------
    //进行key类型/value类型/hash函数适配器的组装后,得到最终便于使用的哈希表类型.
    //下面用uint32_t进行两个示范类的预定义
    //-----------------------------------------------------
    //uint32_t类型的轻量级集合
    template<uint32_t max_set_size>
    class tiny_set_uint32_t :public tiny_set_t<uint32_t, max_set_size, uint_hash_adaptor_t> {};

    //uint32_t(key/value)类型的轻量级哈希表
    template<uint32_t max_set_size,class val_t=uint32_t>
    class tiny_hashtbl_uint32_t :public tiny_hashtbl_t<uint32_t,val_t, max_set_size, uint_hash_adaptor_t> {};
}

#endif
