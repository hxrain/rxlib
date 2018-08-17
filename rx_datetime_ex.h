#ifndef _RX_OS_DATETIME_H_
#define _RX_OS_DATETIME_H_

    #include "rx_cc_macro.h"
	#include "rx_datetime.h"
    #include "rx_atomic.h"
	#include <time.h>
    
/*
	����Ԫ����ϵͳʱ����غ����ķ�װ����.
		rx_time_zone()										��ȡϵͳ��ǰʱ��(��)
		rx_time()											��ȡϵͳ��ǰʱ��(UTC��)
        rx_get_tick_us()                                    ��ȡϵͳ����������ĵδ���(΢��)
        rx_get_tick_ms()                                    ��ȡϵͳ����������ĵδ���(����)
        rx_timeout()                                        �ж������δ����Ƿ񳬹�ָ���ļ��
        class rx_tick_us                                    ΢��δ������
        class rx_tick_ms                                    ����δ������
        class rx_tick_meter_us                              ΢�����ʱ��
        class rx_tick_meter_ms                              ���뱻����ʱ��
        class rx_speed_meter_us                             ΢�����ڼ�����
        class rx_speed_meter_ms                             �������ڼ�����
*/

    //---------------------------------------------------------------
    //����ϵͳʱ�亯��(����ʱ��),��ȡϵͳ���ص�ʱ��(��ֵ)
    //ע��:�˷������߳�ʹ��ʱ��ע��
    inline int32_t rx_time_zone()
    {
        time_t dt = 0;
        struct tm dp;
    #if RX_CC==RX_CC_VC
        localtime_s(&dp, &dt);                              //win�Ͼ�˵�ǰ�ȫ��
    #elif RX_OS==RX_OS_LINUX
        localtime_r(&dt, &dp);                              //linux�Ͼ�˵�ж��߳��������ܵ�����
    #else
        dp = *localtime((time_t*)&dt);                      //���̲߳���ȫ�ı�׼����
    #endif
        return (int32_t)rx_make_utc(dp, 0);
    }

    //---------------------------------------------------------------
	//��ȡ��ǰϵͳ��ʱ��,UTC��ʽ.
	inline uint64_t rx_time(){return time(NULL);}

#if RX_OS==RX_OS_LINUX
    //---------------------------------------------------------------
    //��ȡ��ǰϵͳ������ĵδ���(΢��)
    inline uint64_t rx_get_tick_us()
    {
        struct timespec tp;
        clock_gettime(CLOCK_MONOTONIC_RAW, &tp);
        return (tp.tv_sec * 1000 * 1000 + tp.tv_nsec / 1000);
    }
    //---------------------------------------------------------
    //��ȡ��ǰϵͳ������ĵδ���(΢��),������ms������ʱ��
    inline bool rx_get_tick_us(struct timespec &ts, int32_t ms)
    {
        //��ȡϵͳUTCʱ��
        if (clock_gettime(CLOCK_MONOTONIC_RAW, &ts) < 0)
            return false;
        time_add_ms(ts, ms);
        return true;
    }

#elif defined(RX_OS_WIN)
    //---------------------------------------------------------------
    //��ȡ��ǰϵͳ������ĵδ���(΢��)
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
        return uint64_t((double(t.QuadPart) / m_timer_freq) * 1000 * 1000);
    }
#else
    inline uint64_t rx_get_tick_us()
    {
        rx_st_assert(false, "unsupport os.");
        return -1;
    }
#endif
    //---------------------------------------------------------------
    //��ȡ��ǰϵͳ������ĵδ���(����)
    inline uint64_t rx_get_tick_ms(){return rx_get_tick_us()/1000;}
    //��ȡ�δ����ĺ�������
    typedef uint64_t (*rx_tick_func_t)();

    //-----------------------------------------------------
    //�жϵδ����Ƿ�ʱ
    template<class DT>
    inline bool rx_timeout(DT tick_old,DT tick_new,DT interval)
    {
        return (tick_old<=tick_new)?(tick_new-tick_old>=interval):(tick_new+(DT(-1)-tick_old)>=interval);
	}

    //=====================================================
    //���еδ����򵥹������,�ڲ����еδ���������
    template<rx_tick_func_t func>
    class rx_tick
    {
        uint64_t m_tick_count;
    public:
        typedef uint64_t TickType;
        rx_tick():m_tick_count(0){}
        //-------------------------------------------------
        //�����ڲ��δ���
        TickType update(){m_tick_count=func();return m_tick_count;}
        void update(uint64_t Tick){m_tick_count=Tick;}
        //-------------------------------------------------
        //�õ��ϴ�update�õ��ĵδ���
        TickType count(){return m_tick_count;}
        //-------------------------------------------------
        //�õ�ϵͳ��ǰ�δ���
        static TickType ticks(){return func();}
    };
    //-----------------------------------------------------
    //΢��δ���������
    typedef rx_tick<rx_get_tick_us> rx_tick_us;
    //����δ���������
    typedef rx_tick<rx_get_tick_ms> rx_tick_ms;

    //=====================================================
    //������ʱ��(��������߳�ʹ��)
    template<class tick_t>
    class rx_tick_meter
    {
    public:
        typedef typename tick_t::TickType TickType;
    private:
        tick_t                  m_tick;
        TickType                m_interval;
    public:
        //-------------------------------------------------
        //���캯��,ָ����ʱ�����ʱ��,��֪�Ƿ��״�(����)����(Ĭ�������ȴ���ʱ�Ŵ���)
        rx_tick_meter(TickType interval,bool first_hit=false):m_interval(interval){can_first_hit(first_hit);}
        //Ĭ���״δ���
        rx_tick_meter():m_interval(0){}
        virtual ~rx_tick_meter(){}
        //-------------------------------------------------
        //�����Ƿ�����״δ���(����ǰ��:��δ��������)
        void can_first_hit(bool first_hit=true)
        {
            if (first_hit)                  //Ҫ���״δ���(��������),��ô��������ڲ�Tick����
                reset();
            else if (m_tick.count()==0)     //��Ҫ���״δ���,��Ҫ�����³�ʼ����ֵ
                m_tick.update();
        }
        //-------------------------------------------------
        //��ȡ��ʱ���
        TickType interval(){return m_interval;}
        //�ı䶨ʱ���
        void  interval(TickType I){m_interval=I;}
        //-------------------------------------------------
        //�鿴��ǰ�δ���
        TickType TickCount(){return m_tick.count();}
        //-------------------------------------------------
        //�ж��Ƿ񵽴���ָ���Ķ�ʱ���
        bool is_timing()
        {
            TickType NewTick=tick_t::ticks();
            TickType OldTick=m_tick.count();
            if (OldTick==0||rx_timeout(OldTick,NewTick,m_interval))
            {//m_tick.count()Ϊ0��ʾ����ʱ����Ҫ����������
                m_tick.update(NewTick);
                return true;
            }
            return false;
        }
        //-------------------------------------------------
        //�ֶ����µδ������
        void update(){m_tick.update();}
        //-------------------------------------------------
        //��ʱ������
        void reset(){m_tick.update(0);}
        //-------------------------------------------------
    };
    typedef rx_tick_meter<rx_tick_us> rx_tick_meter_us;
    typedef rx_tick_meter<rx_tick_ms> rx_tick_meter_ms;


	//=====================================================
	//���ڵδ��ʱ�������ڼ�����
	template<class meter_t,uint32_t interval>
	class rx_speed_meter
	{
		meter_t		    m_tick_meter;                       //���ĸ���ʱ��
		rx::atomic_uint	m_working_value;                    //��ʱ������
		rx::atomic_uint	m_total;                            //�ۼ���������
		rx::atomic_uint	m_updated;							//�ۼƱ����µĴ���
		double		    m_speed_value;                      //��һͳ�������ڵ��ٶ�ֵ
		uint32_t	    m_calc_cycle;						//ͳ����������
		rx_speed_meter& operator=(const rx_speed_meter&);
	public:
		//-------------------------------------------------
		rx_speed_meter() :m_speed_value(0), m_calc_cycle(1) {set(interval,1);m_tick_meter.update();}
		virtual ~rx_speed_meter() {}
		//-------------------------------------------------
		//��ȡ��һ��ͳ�������ڵ��ٶ�ֵ
		uint32_t  value(uint32_t Divisor = 1)const { return (uint32_t)m_speed_value / Divisor; }
		double    valuef(double Divisor = 1) const { return m_speed_value / Divisor; }
		//�ۼ�����
		uint32_t total()const { return m_total.value(); }
		//ȡUpdate�ĸ��´���,ͬʱ���Խ�������.
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
		//���¼�����:��֪�ۼ����ӵ��ֽ�����
		//����ֵ:��ǰ�Ƿ���ͳ������,�Ƿ�õ������µ�����ֵ
		bool hit(uint32_t IncCount=1)
		{
			if (IncCount)
			{
				m_working_value+=IncCount;				    //��ֵ�ۼ�
				m_total += IncCount;
				++m_updated;
			}
			if (m_tick_meter.is_timing())					//�ж��Ƿ��ۼ���N�����ڼ��
			{
				m_speed_value = m_working_value / (double)m_calc_cycle;           //���㵥�����ڼ���ڵ�ƽ���ٶ�
				m_working_value = 0;                        //�����ֽ��ۼ�ֵ,Ϊ��һ����׼��
				return true;                                //��֪����ĿǰSpeed�Ѿ���������
			}
			return false;
		}
		//-------------------------------------------------
		//��ʼ��,��֪ͳ�Ƽ��������.���ռ���õ���Speedֵ,�Ǽ��Intervalʱ����Cycle�����ڵ�ƽ��ֵ.
		void set(uint32_t Interval, uint32_t Cycle = 1)
		{
			if (!Cycle) Cycle = 1;
			m_tick_meter.interval(Interval*Cycle);
			m_calc_cycle = Cycle;
		}
	};
    typedef rx_speed_meter<rx_tick_meter_us,1000*1000> rx_speed_meter_us;
    typedef rx_speed_meter<rx_tick_meter_ms,1000> rx_speed_meter_ms;


#endif
