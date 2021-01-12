#ifndef _RX_OS_DATETIME_H_
#define _RX_OS_DATETIME_H_

#include "rx_cc_macro.h"
#include "rx_datetime.h"
#include "rx_ct_atomic.h"
#include <time.h>

/*
	本单元进行系统时间相关函数的封装处理.
		rx_time_zone()										获取系统当前时区(秒)
		rx_time()											获取系统当前时间(UTC秒)
		rx_get_tick_us()                                    获取系统开机后至今的滴答数(微秒)
		rx_get_tick_ms()                                    获取系统开机后至今的滴答数(毫秒)
		rx_timeout()                                        判断两个滴答数是否超过指定的间隔
		class tick_us_t                                     微妙滴答计数器
		class tick_ms_t                                     毫秒滴答计数器
		class meter_us_t                                    微妙被动计时器
		class meter_ms_t                                    毫秒被动计时器
		class speeder_us_t                                  微妙周期计速器
		class speeder_ms_t                                  毫妙周期计速器
*/

//---------------------------------------------------------
//利用系统时间函数(本地时间),获取系统本地的时区(秒值)
//注意:此方法多线程使用时须注意
inline int32_t rx_time_zone(time_t dt = 0)
{
	struct tm dp;
	//先用给定的时间,获取对应的本地时间.
	#if RX_CC==RX_CC_VC||RX_CC_MINGW
	localtime_s(&dp, &dt);                              //win上据说是安全的
	#elif RX_OS==RX_OS_LINUX
	localtime_r(&dt, &dp);                              //linux上据说有多线程锁定性能的问题
	#else
	dp = *localtime((time_t*)&dt);                      //多线程不安全的标准函数
	#endif
	//再将本地时间转换为时区偏差0处的UTC时间.
	return (int32_t)(rx_make_utc(dp, 0) - dt);
}

#if RX_OS==RX_OS_LINUX
//---------------------------------------------------------
//获取当前系统开机后的滴答数(微秒)
inline uint64_t rx_get_tick_us()
{
	struct timespec tp;
	clock_gettime(CLOCK_MONOTONIC_RAW, &tp);
	return (tp.tv_sec * 1000 * 1000 + tp.tv_nsec / 1000);
}
//---------------------------------------------------------
//获取当前系统开机后的滴答数(微秒),并增加ms毫秒后的时间
inline bool rx_get_tick_us(struct timespec &ts, int32_t ms)
{
	//获取系统UTC时间
	if (clock_gettime(CLOCK_MONOTONIC_RAW, &ts) < 0)
		return false;
	rx_add_ms(ts, ms);
	return true;
}
#elif RX_IS_OS_WIN
//---------------------------------------------------------
//获取当前系统开机后的滴答数(微秒)
inline uint64_t rx_get_tick_us()
{
	LARGE_INTEGER t;
	static uint64_t m_timer_freq = 0;
	if (!m_timer_freq)
	{
		if (QueryPerformanceFrequency(&t))
			m_timer_freq = t.QuadPart;
	}

	if (!QueryPerformanceCounter(&t) || !m_timer_freq)
		return -1;
	return uint64_t(((t.QuadPart * 1000 * 1000) / m_timer_freq));
}
#else
inline uint64_t rx_get_tick_us()
{
	uint64_t monotonic_time;
	syscall(SYS_clock_gettime, CLOCK_MONOTONIC_RAW, &monotonic_time)
		return monotonic_time / 1000;
}
#endif
//---------------------------------------------------------
//获取当前系统开机后的滴答数(毫秒)
inline uint64_t rx_get_tick_ms() { return rx_get_tick_us() / 1000; }
//获取滴答数的函数类型
typedef uint64_t(*rx_tick_func_t)();

//---------------------------------------------------------
//判断滴答数是否超时
template<class DT>
inline bool rx_timeout(DT tick_old, DT tick_new, DT interval)
{
	return (tick_old <= tick_new) ? (tick_new - tick_old >= interval) : (tick_new + (DT(-1) - tick_old) >= interval);
}

namespace rx
{
	//=====================================================
	//进行滴答数简单管理的类,内部持有滴答数计数器
	template<rx_tick_func_t func>
	class tick_t
	{
		uint64_t m_tick_count;
	public:
		typedef uint64_t TickType;
		tick_t() :m_tick_count(0) {}
		//-------------------------------------------------
		//立即更新内部滴答数并返回
		TickType update() { m_tick_count = func(); return m_tick_count; }
		void update(uint64_t Tick) { m_tick_count = Tick; }
		//-------------------------------------------------
		//获取最后update记录的滴答数
		TickType count() { return m_tick_count; }
		//-------------------------------------------------
		//得到最新滴答数
		static TickType ticks() { return func(); }
	};
	//-----------------------------------------------------
	//微妙滴答数计数器
	typedef tick_t<rx_get_tick_us> tick_us_t;
	//毫秒滴答数计数器
	typedef tick_t<rx_get_tick_ms> tick_ms_t;

	//=====================================================
	//被动计时器(不建议跨线程使用)
	template<class tick_t>
	class tick_meter_t
	{
	public:
		typedef typename tick_t::TickType TickType;
	private:
		tick_t                  m_tick;
		TickType                m_interval;
	public:
		//-------------------------------------------------
		//构造函数,指定定时器间隔时间,告知是否首次(立即)触发(默认正常等待超时才触发)
		tick_meter_t(TickType interval, bool first_hit = false) :m_interval(interval) { can_first_hit(first_hit); }
		//默认首次触发
		tick_meter_t() :m_interval(0) {}
		virtual ~tick_meter_t() {}
		//-------------------------------------------------
		//设置是否进行首次触发(设置前提:从未被触发过)
		void can_first_hit(bool first_hit = true)
		{
			if (first_hit)                                  //要求首次触发(立即触发),那么就清理掉内部Tick计数
				reset();
			else if (m_tick.count() == 0)                   //不要求首次触发,需要检查更新初始计数值
				m_tick.update();
		}
		//-------------------------------------------------
		//获取定时间隔
		TickType interval() { return m_interval; }
		//改变定时间隔
		void  interval(TickType I) { m_interval = I; }
		//-------------------------------------------------
		//查看当前滴答数
		TickType ticks() { return m_tick.count(); }
		//-------------------------------------------------
		//判断是否到达了指定的定时间隔
		bool is_timing()
		{
			TickType NewTick = tick_t::ticks();
			TickType OldTick = m_tick.count();
			if (OldTick == 0 || rx_timeout(OldTick, NewTick, m_interval))
			{
				//m_tick.count()为0表示本计时器需要立即被触发
				m_tick.update(NewTick);
				return true;
			}
			return false;
		}
		//-------------------------------------------------
		//手动更新滴答计数器
		void update() { m_tick.update(); }
		//-------------------------------------------------
		//计时器清零
		void reset() { m_tick.update(0); }
		//-------------------------------------------------
	};
	typedef tick_meter_t<tick_us_t> meter_us_t;
	typedef tick_meter_t<tick_ms_t> meter_ms_t;


	//=====================================================
	//基于滴答计时器的周期计速器
	template<class meter_t, uint32_t interval>
	class speeder_t
	{
		meter_t		        m_tick_meter;                   //最后的更新时间
		rx::atomic_uint_t	m_working_value;                //临时计数器
		rx::atomic_uint_t	m_total;                        //累计数据总量
		rx::atomic_uint_t	m_updated;						//累计被更新的次数
		double		        m_speed_value;                  //上一统计周期内的速度值
		uint32_t	        m_calc_cycle;				    //统计周期数量
		speeder_t& operator=(const speeder_t&);
	public:
		//-------------------------------------------------
		speeder_t() :m_speed_value(0), m_calc_cycle(1) { set(interval, 1); m_tick_meter.update(); }
		virtual ~speeder_t() {}
		//-------------------------------------------------
		//获取上一个统计周期内的速度值
		uint32_t  value(uint32_t Divisor = 1)const { return (uint32_t)m_speed_value / Divisor; }
		double    valuef(double Divisor = 1) const { return m_speed_value / Divisor; }
		//累计总量
		uint32_t total()const { return m_total.value(); }
		//取Update的更新次数,同时可以将其置零.
		uint32_t count(bool ToZero = false)
		{
			uint32_t Ret = m_updated.value();
			if (ToZero)
			{
				m_updated = 0;
				m_total = 0;
				m_speed_value = 0;
			}
			return Ret;
		}
		//-------------------------------------------------
		//更新计速器:告知累计增加的字节数量
		//返回值:当前是否到了统计周期,是否得到了最新的速率值
		bool hit(uint32_t IncCount = 1)
		{
			if (IncCount)
			{
				m_working_value += IncCount;			    //数值累计
				m_total += IncCount;
				++m_updated;
			}
			if (m_tick_meter.is_timing())					//判断是否累计了N个周期间隔
			{
				m_speed_value = m_working_value / (double)m_calc_cycle;           //计算单个周期间隔内的平均速度
				m_working_value = 0;                        //清理字节累计值,为下一轮做准备
				return true;                                //告知外面目前Speed已经被更新了
			}
			return false;
		}
		//-------------------------------------------------
		//初始化,告知统计间隔与周期.最终计算得到的Speed值,是间隔Interval时长的Cycle个周期的平均值.
		void set(uint32_t Interval, uint32_t Cycle = 1)
		{
			if (!Cycle)
				Cycle = 1;
			m_tick_meter.interval(Interval*Cycle);
			m_calc_cycle = Cycle;
		}
	};
	typedef speeder_t<meter_us_t, 1000 * 1000>  speeder_us_t;
	typedef speeder_t<meter_ms_t, 1000>         speeder_ms_t;

	//-----------------------------------------------------
	//滴答增量计时器,用于记录代码段执行用时
	template<class tick_type = tick_us_t>
	class timepiece_t
	{
		uint64_t        m_begin_tick;                       //计时开始时间
		uint64_t        m_usrdat;                           //用户数据
	public:
		//-------------------------------------------------
		//构造函数,可传入用户数据标识,默认记录开始时间
		timepiece_t(uint64_t usrdat = 0) :m_usrdat(usrdat) { begin(); }
		//-------------------------------------------------
		//可手动更新开始时间
		void begin() { m_begin_tick = tick_type::ticks(); }
		//-------------------------------------------------
		//执行结束,自动计算前后用时并给出事件
		void end() { on_time(tick_type::ticks() - m_begin_tick, m_usrdat); }
		//-------------------------------------------------
		//析构函数,自动计算前后用时
		virtual ~timepiece_t() { end(); }
	protected:
		//给出动作事件,告知持续的时间与用户数据
		virtual void on_time(uint64_t duration, uint64_t usrdat) {}
	};
}
#endif
