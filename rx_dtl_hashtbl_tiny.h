#ifndef _RX_DTL_HASHTBL_TINY_H_
#define _RX_DTL_HASHTBL_TINY_H_

#include "rx_cc_macro.h"
#include "rx_raw_hashtbl_tiny.h"
#include "rx_hash_data.h"
#include "rx_hash_int.h"

namespace rx
{
    //-----------------------------------------------------
    //�򵥹�ϣ��Ĺ�ϣ����������(Ĭ�ϵ�ͨ����)
    template<class val_t,class hashfunc_t>
    class hashfunc_adaptor_t 
    {
        hashfunc_t  m_func;
    public:
        //Ĭ��ʹ��murmur��ϣ����
        hashfunc_adaptor_t():m_func(rx_hash_murmur) {}
        //���԰���������ĺ���
        hashfunc_t bind(hashfunc_t func) { hashfunc_t old = m_func; m_func = func; return old; }
        //�Ը�����ֵ���������Ĺ�ϣֵ����
        uint32_t operator()(const val_t& val)const { return m_func(val,sizeof(val)); }
    };

    //-----------------------------------------------------
    //�򵥹�ϣ��Ĺ�ϣ����������(�����������ƫ�ػ��汾)
    template<>
    class hashfunc_adaptor_t<uint32_t, rx_int_hash32_t>
    {
        rx_int_hash32_t m_func;
    public:
        //Ĭ��ʹ��skeeto���ƹ�ϣ����
        hashfunc_adaptor_t() :m_func(rx_hash_skeeto_triple) {}
        //���԰�������������ϣ����
        rx_int_hash32_t bind(rx_int_hash32_t func) { rx_int_hash32_t old = m_func; m_func = func; return old; }
        //����������ϣֵ�ļ���
        uint32_t operator()(const uint32_t& val)const { return m_func(val); }
    };
    typedef hashfunc_adaptor_t<uint32_t, rx_int_hash32_t> uint_hash_adaptor_t;

    //-----------------------------------------------------
    //����key����/value����/hash��������������װ��,�õ����ձ���ʹ�õĹ�ϣ������.
    //������uint32_t��������ʾ�����Ԥ����
    //-----------------------------------------------------
    //uint32_t���͵�����������
    template<uint32_t max_set_size>
    class tiny_set_uint32_t :public tiny_set_t<uint32_t, max_set_size, uint_hash_adaptor_t> {};

    //uint32_t(key/value)���͵���������ϣ��
    template<uint32_t max_set_size,class val_t=uint32_t>
    class tiny_hashtbl_uint32_t :public tiny_hashtbl_t<uint32_t,val_t, max_set_size, uint_hash_adaptor_t> {};
}

#endif
