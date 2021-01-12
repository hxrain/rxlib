#ifndef _RX_OS_COND_H_
#define _RX_OS_COND_H_

#include "rx_cc_macro.h"
#include "rx_assert.h"
#include "rx_datetime_ex.h"
#include "rx_os_lock.h"

namespace rx
{
	#if RX_OS_POSIX
	//---------------------------------------------------------
	//对条件变量进行功能封装
	class cond_t
	{
		class cond_attr
		{
			pthread_condattr_t  m_attr;
			bool m_is_valid;
		public:
			cond_attr() { m_is_valid = (pthread_condattr_init(&m_attr) == 0); }
			bool operator()() { return m_is_valid; }
			bool set_monotonic() { return 0 == pthread_condattr_setclock(&m_attr, CLOCK_MONOTONIC); }
			pthread_condattr_t & attr() { return m_attr; }
			~cond_attr()
			{
				if (!m_is_valid)
					return;
				m_is_valid = false;
				pthread_condattr_destroy(&m_attr);
			}
		};

		pthread_cond_t      m_cond;

		//-----------------------------------------------------
		bool m_init()
		{
			cond_attr ca;
			if (!ca())
				return false;
			//要求使用系统滴答相对时间进行等待
			if (!ca.set_monotonic())
				return false;
			return 0 == pthread_cond_init(&m_cond, &ca.attr());
		}
		//-----------------------------------------------------
		void m_uninit()
		{
			pthread_cond_destroy(&m_cond);
		}

	public:
		cond_t()
		{
			memset(&m_cond, 0, sizeof(m_cond));
			m_init();
		}
		~cond_t() { m_uninit(); }
		//-----------------------------------------------------
		//对条件变量进行等待:默认无限等待(毫秒)
		bool wait(locker_t &locker, int ms = -1)
		{
			if (ms == -1)
				return pthread_cond_wait(&m_cond, locker.handle()) == 0;
			else
			{
				struct timespec ts;
				rx_get_tick_us(ts, ms);
				return pthread_cond_timedwait(&m_cond, locker.handle(), &ts) == 0;
			}
		}
		//-----------------------------------------------------
		//对条件变量发送通知:是否为广播通知
		bool post(bool to_all = false)
		{
			if (to_all)
				return 0 == pthread_cond_broadcast(&m_cond);
			else
				return 0 == pthread_cond_signal(&m_cond);
		}
	};
	
	#elif RX_IS_OS_WIN

	//---------------------------------------------------------
	//对条件变量进行功能封装
	class cond_t
	{
		CONDITION_VARIABLE      m_cond;

		//-----------------------------------------------------
		bool m_init()
		{
			InitializeConditionVariable(&m_cond);
			return true;
		}
		//-----------------------------------------------------
		void m_uninit() {}
	public:
		//-----------------------------------------------------
		cond_t()
		{
			memset(&m_cond, 0, sizeof(m_cond));
			m_init();
		}
		~cond_t() { m_uninit(); }
		//-----------------------------------------------------
		//对条件变量进行等待:默认无限等待(毫秒)
		bool wait(locker_t &locker, int ms = -1)
		{
			if (ms == -1)
				return !!SleepConditionVariableCS(&m_cond, locker.handle(), INFINITE);
			else
				return !!SleepConditionVariableCS(&m_cond, locker.handle(), ms);
		}
		//-----------------------------------------------------
		//对条件变量发送通知:是否为广播通知
		bool post(bool to_all = false)
		{
			if (to_all)
				WakeAllConditionVariable(&m_cond);
			else
				WakeConditionVariable(&m_cond);
			return true;
		}
	};

	#endif
}

#endif // _HX_COND_H_
