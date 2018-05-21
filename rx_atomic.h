#ifndef _RX_BASE_CORE_ATOMIC_H_
#define _RX_BASE_CORE_ATOMIC_H_

#include "rx_cc_atomic.h"

namespace rx
{
    //-----------------------------------------------------
    //通用原子变量封装
    template <typename T>
    class atomic
    {
        T        m_Value;
    public:
        //构造与赋值函数
        atomic(T v = 0) : m_Value(v) {}
        atomic(const atomic& a){ store(a);}
        //给当前原子变量设置值V
        void     store(T v)              { rx_atomic_store(&m_Value, v); }
        //强制装载当前原子变量并返回当前值
        T        load() const            { return rx_atomic_load(&m_Value); }
        //取值(load方法的别名)
        T        value() const           { return load(); }

        //当前原子变量和V进行值交换,返回值旧值
        T        swap(T v)  { return rx_atomic_swap(&m_Value, v); }
        //比较并交换:原子变量的值与expected比较,如果相同则设置为desired,返回true;否则在*expected中放入原值,返回false.
        bool     cas(T* expected, T desired)   { return rx_atomic_cas(&m_Value, expected, desired); }
        //比较并交换:原子变量的值与expected比较,如果相同则设置为desired,返回true;否则返回false.
        bool     cas(T expected, T desired)   { return rx_atomic_cas(&m_Value, &expected, desired); }

        //返回原子变量的旧值并进行相应的运算
        T        inc()                   { return rx_atomic_add(&m_Value, (T)1); }
        T        dec()                   { return rx_atomic_sub(&m_Value, (T)1); }
        T        add(T v)                { return rx_atomic_add(&m_Value, v); }
        T        sub(T v)                { return rx_atomic_sub(&m_Value, v); }
        T        and_op(T v)             { return rx_atomic_and(&m_Value, v); }
        T        or_op (T v)             { return rx_atomic_or (&m_Value, v); }
        T        xor_op(T v)             { return rx_atomic_xor(&m_Value, v); }

        //运算符重载
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
    //最简原子标记
    class atomic_flag
    {
        atomic<int>     m_Flag;
    public:
        //构造函数
        atomic_flag(bool v = false)    : m_Flag(v?1:0) {}
        atomic_flag(const atomic_flag& a) { m_Flag.store(a); }
        //赋值
        atomic_flag& operator= (const atomic_flag& a) { m_Flag.store(a); return *this; }
        atomic_flag& operator= (bool a) { m_Flag.store(a?1:0); return *this;}
        //标记清除
        void        clear(){ m_Flag.store(0); }
        //标记置位且返回旧值
        bool        test_and_set(){ return !!m_Flag.swap(1);}
        //获取当前标记的值
        operator bool(void) const { return !!m_Flag; }
    };

    //-----------------------------------------------------
    //最终使用的原子变量
    typedef atomic<int32_t>     atomic_int;
#if RX_ATOMIC64
    typedef atomic<int64_t>     atomic_long;
#endif
}

#endif
