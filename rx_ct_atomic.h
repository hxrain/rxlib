#ifndef _RX_BASE_CORE_ATOMIC_H_
#define _RX_BASE_CORE_ATOMIC_H_

#include "rx_cc_atomic.h"
#include "rx_ct_traits.h"

/*
	本单元基于原子操作API进行原子变量的对象化封装.
	class atomic_flag;										//原子标记
	class atomic_int_t;										//32位整数原子量
	class atomic_long_t;									//64位整数原子量(VC只有在x64模式下有效)
*/

namespace rx
{
	//-----------------------------------------------------
	//通用原子变量封装
	template <typename T>
	class atomic_t
	{
		T        m_value;
		typedef typename ct_type_check_t<T>::signed_t vt;
	public:
		//构造与赋值函数
		atomic_t(T v = 0) : m_value(v) {}
		atomic_t(const atomic_t& a) { store(a); }
		//给当前原子变量设置值V
		void     store(T v) { rx_atomic_store((vt&)m_value, (vt)v); }
		//强制装载当前原子变量并返回当前值
		T        load() const { return rx_atomic_load((vt&)m_value); }
		//取值(load方法的别名)
		T        value() const { return load(); }

		//当前原子变量和V进行值交换,返回值旧值
		T        swap(T v) { return rx_atomic_swap((vt&)m_value, (vt)v); }
		//比较并交换:原子变量的值与expected比较,如果相同则设置为desired,返回true;否则在*expected中放入原值,返回false.
		bool     cas(T* expected, T desired) { return rx_atomic_cas((vt&)m_value, (vt*)expected, (vt)desired); }
		//比较并交换:原子变量的值与expected比较,如果相同则设置为desired,返回true;否则返回false.
		bool     cas(T expected, T desired) { return rx_atomic_cas((vt&)m_value, (vt)expected, (vt)desired); }

		//返回原子变量的旧值并进行相应的运算
		T        inc() { return rx_atomic_add((vt&)m_value, (vt)1); }
		T        dec() { return rx_atomic_sub((vt&)m_value, (vt)1); }
		T        add(T v) { return rx_atomic_add((vt&)m_value, (vt)v); }
		T        sub(T v) { return rx_atomic_sub((vt&)m_value, (vt)v); }
		T        and_op(T v) { return rx_atomic_and((vt&)m_value, (vt)v); }
		T        or_op(T v) { return rx_atomic_or((vt&)m_value, (vt)v); }
		T        xor_op(T v) { return rx_atomic_xor((vt&)m_value, (vt)v); }

		//运算符重载
		operator T (void) const { return load(); }
		atomic_t&  operator = (const atomic_t& a) { store(a); return *this; }
		atomic_t&  operator = (T v) { store(v); return *this; }
		T        operator++ (int) { return inc(); }
		T        operator-- (int) { return dec(); }
		T        operator++ (void) { inc(); return load(); }
		T        operator-- (void) { dec(); return load(); }
		T        operator+= (T v) { add(v); return load(); }
		T        operator-= (T v) { sub(v); return load(); }
		T        operator&= (T v) { and_op(v); return load(); }
		T        operator|= (T v) { or_op(v); return load(); }
		T        operator^= (T v) { xor_op(v); return load(); }
	};

	//-----------------------------------------------------
	//最简原子标记
	class atomic_flag
	{
		atomic_t<int>     m_Flag;
	public:
		//构造函数
		atomic_flag(bool v = false) : m_Flag(v ? 1 : 0) {}
		atomic_flag(const atomic_flag& a) { m_Flag.store(a); }
		//赋值
		atomic_flag& operator= (const atomic_flag& a) { m_Flag.store(a); return *this; }
		atomic_flag& operator= (bool a) { m_Flag.store(a ? 1 : 0); return *this; }
		//标记清除
		void        clear() { m_Flag.store(0); }
		//标记置位且返回旧值
		bool        test_and_set() { return !!m_Flag.swap(1); }
		//获取当前标记的值
		operator bool(void) const { return !!m_Flag; }
	};

	//-----------------------------------------------------
	//最终使用的原子变量
	typedef atomic_t<int32_t>     atomic_int_t;
	typedef atomic_t<uint32_t>    atomic_uint_t;
	#if RX_ATOMIC64
	typedef atomic_t<int64_t>     atomic_long_t;
	typedef atomic_t<uint64_t>    atomic_ulong_t;
	#endif
}

#endif
