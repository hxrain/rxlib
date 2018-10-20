#ifndef _RX_CT_TYPEOF_H_
#define _RX_CT_TYPEOF_H_

#include "rx_cc_macro.h"

//C++03��typeof��auto��ģ��ʵ��
//https://github.com/heromapwrd/typeof/blob/master/typeof/typeof.h
//https://blog.csdn.net/markl22222/article/details/10474591

#if RX_CC==RX_CC_VC
    #include <typeinfo>
    namespace to
    {
        //����typeid�Ľ������һ���µ����ͱ�ʶ���
        template <const std::type_info& i> struct tid {};
        //ͨ��������ʵ��,��typeid�ؼ��ֵĽ������һ����Ӧ�����ͱ�ʶ(VC����,GCC���ܱ���)
        #define make_id(...) to::tid<typeid(__VA_ARGS__)>

        struct empty_t {};
        template<typename ID, typename T = empty_t> struct extract_t;
        template<typename ID> struct extract_t<ID, empty_t> { template <bool> struct id2type; };
        template<typename ID, typename T> struct extract_t : extract_t<ID, empty_t>
        {template <> struct id2type<true> { typedef T type_t; };};
        //VC�ĺ�ħ��(VC����,GCC���ܱ���):��ģ����ڲ�����һ����ģ�壬����ʹ�ü̳��á��ػ����ػ�����Ч��
        #define type_extract(...) to::extract_t<make_id(__VA_ARGS__) >::id2type<true>::type_t

        //�Զ�����������ID��'ע��'
        template<typename T, typename ID>
        struct register_t : extract_t<ID, T> {typedef typename id2type<true>::type_t type_t;};

        //�����װ,����Զ�����ע��
        template <typename T>
        struct encode_t
        {
            typedef T* enc_type_t;
            typedef register_t<enc_type_t, make_id(enc_type_t)> reg_type;
            typedef typename reg_type::type_t type_t;
        };

        template<typename T> typename encode_t<T>::type_t encode(const T&);
        template<typename T> typename encode_t<T>::type_t encode(T&);

        //�����װ�õ����յ�����
        template <typename T> struct decode_t;
        template <typename T> struct decode_t<T*>{typedef T type_t;};
    }

    #define type_of(...) to::decode_t<type_extract(to::encode((__VA_ARGS__)))>::type_t

#elif RX_CC==RX_CC_GCC || RX_CC_MINGW
    #define type_of(...) __typeof((__VA_ARGS__))
#endif

    //���ݸ�����ֵ�Զ�����ָ�����ֵ���ֵ,����auto_of(I,list.begin()),�Զ�����������
    #define auto_of(name, ...) type_of(__VA_ARGS__) name((__VA_ARGS__))

#endif
