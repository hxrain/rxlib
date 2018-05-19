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
        T           m_Value;
    public:
        inline atomic(T v = 0) : m_Value(v) {}
        atomic(const atomic& a){ store(a);}
        atomic& operator= (const atomic& a){store(a);return *this;}
        //给当前原子变量设置值V
        inline void     store(T v) { rx_atomic_store(&m_Value, v); }
        //强制装载当前原子变量并返回当前值
        inline T        load() const { return rx_atomic_load(&m_Value); }
        //取值(load方法的别名)
        inline T        value() const { return load(); }

        //当前原子变量和V进行值交换,返回值为原子变量之前的值
        inline T        swap(T v) { return rx_atomic_swap(&m_Value, v); }
        //比较并交换:原子变量的值与expected比较,如果相同则设置为desired,返回true;否则在*expected中放入原值,返回false.
        inline bool     cas(T* expected, T desired)   { return rx_atomic_cas(&m_Value, expected, desired); }
        //比较并交换:原子变量的值与expected比较,如果相同则设置为desired,返回true;否则返回false.
        inline bool     cas(T expected, T desired)   { return rx_atomic_cas(&m_Value, &expected, desired); }
                
        //返回原子变量的旧值并进行相应的运算
        inline T        inc()    { return rx_atomic_add(&m_Value, (T)1); }
        inline T        dec()    { return rx_atomic_sub(&m_Value, (T)1); }
        inline T        add(T v) { return rx_atomic_add(&m_Value, v); }
        inline T        sub(T v) { return rx_atomic_sub(&m_Value, v); }
        inline T        and(T v) { return rx_atomic_and(&m_Value, v); }
        inline T        or (T v) { return rx_atomic_or (&m_Value, v); }
        inline T        xor(T v) { return rx_atomic_xor(&m_Value, v); }
        
        //运算符重载
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
    //最简原子标记
    class atomic_flag
    {
        atomic<int>     m_Flag;
    public:
        inline    atomic_flag(bool v = false)    : m_Flag(v?1:0) {}
        //禁止拷贝构造
        atomic_flag(const atomic_flag&) = delete;
        //禁止拷贝赋值
        atomic_flag& operator= (const atomic_flag&) = delete;
        //标记清除
        void        clear(){ m_Flag.store(0); }
        //标记置位且返回旧值
        bool        test_and_set(){ return !!m_Flag.swap(1);}
        //获取当前标记的值
        inline operator bool(void) const { return !!m_Flag; }
    };

    //-----------------------------------------------------
    //最终使用的原子变量
    typedef atomic<int32_t>     atomic_int;
    typedef atomic<int64_t>     atomic_long;
}

#endif