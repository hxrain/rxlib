#ifndef _RX_LOCK_BASE_H_
#define _RX_LOCK_BASE_H_

#include "rx_cc_macro.h"
#include "rx_assert.h"
#include "rx_cc_atomic.h"

namespace rx
{
	//------------------------------------------------------
	//同步锁功能对象的统一接口
	class lock_i
	{
	protected:
		virtual ~lock_i() {}
	public:
		//--------------------------------------------------
		//锁定,默认写锁
		virtual bool lock(bool is_wr_lock = true) = 0;
		//--------------------------------------------------
		//解锁
		virtual bool unlock() = 0;
		//--------------------------------------------------
		//尝试锁定,默认写锁
		virtual bool trylock(bool is_wr_lock = true) { return false; }
	};

	//------------------------------------------------------
	//占位用的空锁,啥都不干.
	class null_lock_t :public lock_i
	{
	public:
		//--------------------------------------------------
		//锁定
		virtual bool lock(bool is_wr_lock = true) { return true; }
		//--------------------------------------------------
		//解锁
		virtual bool unlock() { return true; }
		//--------------------------------------------------
		//尝试锁定
		virtual bool trylock(bool is_wr_lock = true) { return true; }
	};

	//-----------------------------------------------------
	//基于原子标记,封装一个简单的自旋锁
	class spin_lock_i :public lock_i
	{
		uint8_t     padding1[CPU_CACHELINE_SIZE / 2 - sizeof(void*)];
		int32_t     m_lock;
		uint32_t	m_max_retry;
		uint8_t     padding2[CPU_CACHELINE_SIZE / 2 - sizeof(uint32_t) * 2];
	public:
		spin_lock_i(uint32_t max_retry = 100) :m_lock(0), m_max_retry(max_retry) {}
		//--------------------------------------------------
		//锁定,默认写锁
		bool lock(bool is_wr_lock = true)
		{
			//尝试设置标记为真,如果返回旧值为假,则意味着抢到了锁.否则死循环.
			mem_barrier();
			while (!rx_atomic_cas(m_lock, 0, 1));
			return true;
		}
		//--------------------------------------------------
		//解锁
		bool unlock()
		{
			mem_barrier();                                  // 编译器读写屏障
			rx_atomic_store(m_lock, 0);                     // 释放锁
			return true;
		}
		//--------------------------------------------------
		//尝试锁定,默认写锁
		bool trylock(bool is_wr_lock = true)
		{
			uint32_t lc = 0;
			while (lc < m_max_retry && !rx_atomic_cas(m_lock, 0, 1))
				++lc;
			return lc < m_max_retry;
		}
	};

	//------------------------------------------------------
	//封装一个锁定对象的卫兵对象,利用卫兵对象的构造与析构自动进行作用域内的锁定/解锁
	template<class lt, bool is_wr_lock = true>
	class guarded_t
	{
		lt  &m_locker;
		int m_flag;
	public:
		//构造的时候加锁
		guarded_t(lt &locker) :m_locker(locker), m_flag(0)
		{
			rx_check(m_locker.lock(is_wr_lock));
		}
		//用标记控制仅应该循环一次
		bool pass_one() { return 0 == m_flag++; }
		//析构的时候解锁
		~guarded_t()
		{
			m_locker.unlock();
		}
	};

	//------------------------------------------------------
	//使用宏定义,便于使用锁定对象的卫兵模式,对于读写锁来说，为写锁
	#define GUARD_T(Locker,LT) guarded_t<LT> RX_CT_SYM(_guard_)((Locker))
	#define GUARD(Locker) GUARD_T(Locker,lock_i)
	//使用for语句结构进行锁定范围限定的宏定义语法糖
	#define guard_t(Locker,LT) for(guarded_t<LT> RX_CT_SYM(_guard_for_)(Locker);RX_CT_SYM(_guard_for_).pass_one();)
	#define guard(Locker) guard_t(Locker,lock_i)
	//------------------------------------------------------
	//读写锁中，读锁的语法糖定义
	#define RGUARD_T(Locker,LT) guarded_t<LT,false> RX_CT_SYM(_guard_)((Locker))
	#define RGUARD(Locker) RGUARD_T(Locker,lock_i)
	//使用for语句结构进行锁定范围限定的宏定义语法糖
	#define rguard_t(Locker,LT) for(guarded_t<LT,false> RX_CT_SYM(_guard_)(Locker);RX_CT_SYM(_guard_).pass_one();)
	#define rguard(Locker) rguard_t(Locker,lock_i)
}


#endif