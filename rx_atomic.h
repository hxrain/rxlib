#ifndef _RX_BASE_CORE_ATOMIC_H_
#define _RX_BASE_CORE_ATOMIC_H_

#include "rx_cc_atomic.h"

namespace rx
{
    //-----------------------------------------------------
    //ͨ��ԭ�ӱ�����װ
    template <typename T>
    class atomic
    {
        T        m_Value;
    public:
        //�����븳ֵ����
        atomic(T v = 0) : m_Value(v) {}
        atomic(const atomic& a){ store(a);}
        //����ǰԭ�ӱ�������ֵV
        void     store(T v)              { rx_atomic_store(&m_Value, v); }
        //ǿ��װ�ص�ǰԭ�ӱ��������ص�ǰֵ
        T        load() const            { return rx_atomic_load(&m_Value); }
        //ȡֵ(load�����ı���)
        T        value() const           { return load(); }

        //��ǰԭ�ӱ�����V����ֵ����,����ֵ��ֵ
        T        swap(T v)  { return rx_atomic_swap(&m_Value, v); }
        //�Ƚϲ�����:ԭ�ӱ�����ֵ��expected�Ƚ�,�����ͬ������Ϊdesired,����true;������*expected�з���ԭֵ,����false.
        bool     cas(T* expected, T desired)   { return rx_atomic_cas(&m_Value, expected, desired); }
        //�Ƚϲ�����:ԭ�ӱ�����ֵ��expected�Ƚ�,�����ͬ������Ϊdesired,����true;���򷵻�false.
        bool     cas(T expected, T desired)   { return rx_atomic_cas(&m_Value, &expected, desired); }

        //����ԭ�ӱ����ľ�ֵ��������Ӧ������
        T        inc()                   { return rx_atomic_add(&m_Value, (T)1); }
        T        dec()                   { return rx_atomic_sub(&m_Value, (T)1); }
        T        add(T v)                { return rx_atomic_add(&m_Value, v); }
        T        sub(T v)                { return rx_atomic_sub(&m_Value, v); }
        T        and_op(T v)             { return rx_atomic_and(&m_Value, v); }
        T        or_op (T v)             { return rx_atomic_or (&m_Value, v); }
        T        xor_op(T v)             { return rx_atomic_xor(&m_Value, v); }

        //���������
                 operator T (void) const { return load(); }
        atomic&  operator = (const atomic& a){store(a);return *this;}
        atomic&  operator = (T v)        { store(v); return *this; }
        T        operator++ (int)        { return inc(); }
        T        operator-- (int)        { return dec(); }
        T        operator++ (void)       { inc() ;return load(); }
        T        operator-- (void)       { dec() ;return load(); }
        T        operator+= (T v)        { add(v);return load(); }
        T        operator-= (T v)        { sub(v);return load(); }
        T        operator&= (T v)        { and_op(v);return load(); }
        T        operator|= (T v)        { or_op (v);return load(); }
        T        operator^= (T v)        { xor_op(v);return load(); }
    };

    //-----------------------------------------------------
    //���ԭ�ӱ��
    class atomic_flag
    {
        atomic<int>     m_Flag;
    public:
        //���캯��
        atomic_flag(bool v = false)    : m_Flag(v?1:0) {}
        atomic_flag(const atomic_flag& a) { m_Flag.store(a); }
        //��ֵ
        atomic_flag& operator= (const atomic_flag& a) { m_Flag.store(a); return *this; }
        atomic_flag& operator= (bool a) { m_Flag.store(a?1:0); return *this;}
        //������
        void        clear(){ m_Flag.store(0); }
        //�����λ�ҷ��ؾ�ֵ
        bool        test_and_set(){ return !!m_Flag.swap(1);}
        //��ȡ��ǰ��ǵ�ֵ
        operator bool(void) const { return !!m_Flag; }
    };

    //-----------------------------------------------------
    //����ʹ�õ�ԭ�ӱ���
    typedef atomic<int32_t>     atomic_int;
#if RX_ATOMIC64
    typedef atomic<int64_t>     atomic_long;
#endif
}

#endif
