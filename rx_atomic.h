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
        T           m_Value;
    public:
        inline atomic(T v = 0) : m_Value(v) {}
        atomic(const atomic& a){ store(a);}
        atomic& operator= (const atomic& a){store(a);return *this;}
        //����ǰԭ�ӱ�������ֵV
        inline void     store(T v) { rx_atomic_store(&m_Value, v); }
        //ǿ��װ�ص�ǰԭ�ӱ��������ص�ǰֵ
        inline T        load() const { return rx_atomic_load(&m_Value); }
        //ȡֵ(load�����ı���)
        inline T        value() const { return load(); }

        //��ǰԭ�ӱ�����V����ֵ����,����ֵΪԭ�ӱ���֮ǰ��ֵ
        inline T        swap(T v) { return rx_atomic_swap(&m_Value, v); }
        //�Ƚϲ�����:ԭ�ӱ�����ֵ��expected�Ƚ�,�����ͬ������Ϊdesired,����true;������*expected�з���ԭֵ,����false.
        inline bool     cas(T* expected, T desired)   { return rx_atomic_cas(&m_Value, expected, desired); }
        //�Ƚϲ�����:ԭ�ӱ�����ֵ��expected�Ƚ�,�����ͬ������Ϊdesired,����true;���򷵻�false.
        inline bool     cas(T expected, T desired)   { return rx_atomic_cas(&m_Value, &expected, desired); }
                
        //����ԭ�ӱ����ľ�ֵ��������Ӧ������
        inline T        inc()    { return rx_atomic_add(&m_Value, (T)1); }
        inline T        dec()    { return rx_atomic_sub(&m_Value, (T)1); }
        inline T        add(T v) { return rx_atomic_add(&m_Value, v); }
        inline T        sub(T v) { return rx_atomic_sub(&m_Value, v); }
        inline T        and(T v) { return rx_atomic_and(&m_Value, v); }
        inline T        or (T v) { return rx_atomic_or (&m_Value, v); }
        inline T        xor(T v) { return rx_atomic_xor(&m_Value, v); }
        
        //���������
        inline          operator T (void) const { return load(); }
        inline T        operator = (T v)        { store(v); return v; }
        inline T        operator++ (int)        { return inc(); }
        inline T        operator-- (int)        { return dec(); }
        inline T        operator++ (void)       { add(1);return load(); }
        inline T        operator-- (void)       { sub(1);return load(); }
        inline T        operator+= (T v)        { add(v);return load(); }
        inline T        operator-= (T v)        { sub(v);return load(); }
        inline T        operator&= (T v)        { and(1);return load(); }
        inline T        operator|= (T v)        { or (1);return load(); }
        inline T        operator^= (T v)        { xor(1);return load(); }
    };

    //-----------------------------------------------------
    //���ԭ�ӱ��
    class atomic_flag
    {
        atomic<int>     m_Flag;
    public:
        inline    atomic_flag(bool v = false)    : m_Flag(v?1:0) {}
        //��ֹ��������
        atomic_flag(const atomic_flag&) = delete;
        //��ֹ������ֵ
        atomic_flag& operator= (const atomic_flag&) = delete;
        //������
        void        clear(){ m_Flag.store(0); }
        //�����λ�ҷ��ؾ�ֵ
        bool        test_and_set(){ return !!m_Flag.swap(1);}
        //��ȡ��ǰ��ǵ�ֵ
        inline operator bool(void) const { return !!m_Flag; }
    };

    //-----------------------------------------------------
    //����ʹ�õ�ԭ�ӱ���
    typedef atomic<int32_t>     atomic_int;
    typedef atomic<int64_t>     atomic_long;
}

#endif