#ifndef _RX_CT_TYPEOF_H_
#define _RX_CT_TYPEOF_H_

#include "rx_cc_macro.h"

//C++03的typeof和auto的模拟实现
//https://github.com/heromapwrd/typeof/blob/master/typeof/typeof.h
//https://blog.csdn.net/markl22222/article/details/10474591

#if RX_CC==RX_CC_VC
    #include <typeinfo>
    namespace to
    {
        //利用typeid的结果定义一个新的类型标识类别
        template <const std::type_info& i> struct tid {};
        //通过给定的实例,用typeid关键字的结果定义一个对应的类型标识(VC特性,GCC不能编译)
        #define make_id(...) to::tid<typeid(__VA_ARGS__)>

        struct empty_t {};
        template<typename ID, typename T = empty_t> struct extract_t;
        template<typename ID> struct extract_t<ID, empty_t> { template <bool> struct id2type; };
        template<typename ID, typename T> struct extract_t : extract_t<ID, empty_t>
        {template <> struct id2type<true> { typedef T type_t; };};
        //VC的黑魔法(VC特性,GCC不能编译):类模板的内部又有一个类模板，并且使用继承让“特化在特化中生效”
        #define type_extract(...) to::extract_t<make_id(__VA_ARGS__) >::id2type<true>::type_t

        //自动生成类型与ID的'注册'
        template<typename T, typename ID>
        struct register_t : extract_t<ID, T> {typedef typename id2type<true>::type_t type_t;};

        //编码包装,完成自动类型注册
        template <typename T>
        struct encode_t
        {
            typedef T* enc_type_t;
            typedef register_t<enc_type_t, make_id(enc_type_t)> reg_type;
            typedef typename reg_type::type_t type_t;
        };

        template<typename T> typename encode_t<T>::type_t encode(const T&);
        template<typename T> typename encode_t<T>::type_t encode(T&);

        //解码包装得到最终的类型
        template <typename T> struct decode_t;
        template <typename T> struct decode_t<T*>{typedef T type_t;};
    }

    #define type_of(...) to::decode_t<type_extract(to::encode((__VA_ARGS__)))>::type_t

#elif RX_CC==RX_CC_GCC || RX_CC_MINGW
    #define type_of(...) __typeof((__VA_ARGS__))
#endif

    //根据给定的值自动生成指定名字的新值,比如auto_of(I,list.begin()),自动描述迭代器
    #define auto_of(name, ...) type_of(__VA_ARGS__) name((__VA_ARGS__))

#endif
