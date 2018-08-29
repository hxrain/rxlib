#ifndef _RX_BASE_CORE_ATOMIC_H_
#define _RX_BASE_CORE_ATOMIC_H_

#include "rx_cc_atomic.h"
#include "rx_ct_traits.h"

/*
	����Ԫ����ԭ�Ӳ���API����ԭ�ӱ����Ķ��󻯷�װ.
	class atomic_flag;										//ԭ�ӱ��
	class atomic_int_t;										//32λ����ԭ����
	class atomic_long_t;										//64λ����ԭ����(VCֻ����x64ģʽ����Ч)
*/

namespace rx
{
    //-----------------------------------------------------
    //ͨ��ԭ�ӱ�����װ
    template <typename T>
    class atomic_t
    {
        T        m_value;
        typedef typename rx_type_check_t<T>::signed_t vt;
    public:
        //�����븳ֵ����
        atomic_t(T v = 0) : m_value(v) {}
        atomic_t(const atomic_t& a){ store(a);}
        //����ǰԭ�ӱ�������ֵV
        void     store(T v)              { rx_atomic_store((vt*)&m_value, (vt)v); }
        //ǿ��װ�ص�ǰԭ�ӱ��������ص�ǰֵ
        T        load() const            { return rx_atomic_load((vt*)&m_value); }
        //ȡֵ(load�����ı���)
        T        value() const           { return load(); }

        //��ǰԭ�ӱ�����V����ֵ����,����ֵ��ֵ
        T        swap(T v)  { return rx_atomic_swap((vt*)&m_value, (vt)v); }
        //�Ƚϲ�����:ԭ�ӱ�����ֵ��expected�Ƚ�,�����ͬ������Ϊdesired,����true;������*expected�з���ԭֵ,����false.
        bool     cas(T* expected, T desired)   { return rx_atomic_cas((vt*)&m_value, (vt*)expected, (vt)desired); }
        //�Ƚϲ�����:ԭ�ӱ�����ֵ��expected�Ƚ�,�����ͬ������Ϊdesired,����true;���򷵻�false.
        bool     cas(T expected, T desired)   { return rx_atomic_cas((vt*)&m_value, (vt)expected, (vt)desired); }

        //����ԭ�ӱ����ľ�ֵ��������Ӧ������
        T        inc()                   { return rx_atomic_add((vt*)&m_value, (vt)1); }
        T        dec()                   { return rx_atomic_sub((vt*)&m_value, (vt)1); }
        T        add(T v)                { return rx_atomic_add((vt*)&m_value, (vt)v); }
        T        sub(T v)                { return rx_atomic_sub((vt*)&m_value, (vt)v); }
        T        and_op(T v)             { return rx_atomic_and((vt*)&m_value, (vt)v); }
        T        or_op (T v)             { return rx_atomic_or ((vt*)&m_value, (vt)v); }
        T        xor_op(T v)             { return rx_atomic_xor((vt*)&m_value, (vt)v); }

        //���������
                 operator T (void) const { return load(); }
        atomic_t&  operator = (const atomic_t& a){store(a);return *this;}
        atomic_t&  operator = (T v)        { store(v); return *this; }
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
        atomic_t<int>     m_Flag;
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
    //����ԭ�ӱ��,��װһ���򵥵�������
    class spin_lock
    {
        atomic_flag     m_flag;
    public:
        //--------------------------------------------------
        //����
        bool lock()
        {
            //�������ñ��Ϊ��,������ؾ�ֵΪ��,����ζ����������.������ѭ��.
            while(m_flag.test_and_set());
            return true;
        }
        //--------------------------------------------------
        //����
        bool unlock()
        {
            //ԭ�ӱ�������,����˲���ͱ������߳���λ��,ҲҪ���سɹ�.
            m_flag.clear();
            return true;
        }
        //--------------------------------------------------
        //��������
        bool trylock(uint32_t max_retry=4000)
        {
            uint32_t lc=0;
            while(lc<max_retry&&m_flag.test_and_set())
                ++lc;
            return lc<max_retry;
        }
    };

    //-----------------------------------------------------
    //����ʹ�õ�ԭ�ӱ���
    typedef atomic_t<int32_t>     atomic_int_t;
    typedef atomic_t<uint32_t>    atomic_uint_t;
#if RX_ATOMIC64
    typedef atomic_t<int64_t>     atomic_long_t;
    typedef atomic_t<uint64_t>    atomic_ulong_t;
#endif
}

#endif
