#ifndef _RX_TDD_TICK_H_
#define _RX_TDD_TICK_H_

#include "rx_cc_macro.h"
#include "rx_datetime_ex.h"

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <memory.h>
#include "rx_str_util_fmt.h"

/*
//��ʱ�δ���÷�ʾ��
inline void tdd_tt_demo()
{
	tdd_tt(t,"module_main","module_sub");
	//do some ...
	tdd_tt_hit(t, "action1");
	//do some ...
	tdd_tt_hit(t, "action2");
	//do some ...
	tdd_tt_hit(t, "action3:%d",1);
	//do some ...
	{tdd_tt_tab(t, "action4");
		//do some ...
		{tdd_tt_tab(t, "action4.1");
			//do some ...
		}
		{tdd_tt_tab(t, "action4.2:%d",2);
			//do some ...
		}
		tdd_tt_for(t,100,"action4.3%d",1)
		{

		}
	}
	tdd_tt_for(t, 100, "for action5")
	{

	}
}
*/

namespace rx
{
	#define tdd_tt_enable true                              //Ĭ��tdd��ʱ�Ƿ���

	//-----------------------------------------------------
	//�δ�������ʱ��,���ڼ�¼�����ִ����ʱ
	template<class tick_type = tick_us_t>
	class tdd_tick_t
	{
		tick_type    m_tick_meter;                          //�δ������
		uint64_t     m_begin_tick;                          //��ʱ��ʼ�����ʱ��
		uint32_t     m_expend_limit;                        //��ʾ�������ʱ����
		bool         m_enable;                              //�Ƿ�����ʱ����
		const char*  m_msg_a;                               //��ӡ�����,��֪���ܴ���
		const char*  m_msg_b;                               //��ӡ�����,��֪����С��
		uint32_t     m_tab_deep;                            //��ӡ���ʱ,����hit��λ�������ȴ���
		const char*  m_file;
		uint32_t     m_lineno;
		uint32_t     m_last_hit_elapsed;
		uint32_t     m_total_elapsed;
		uint64_t     m_last_hit_time;
		char         m_tmp_msg_b[128];
	public:
		//-------------------------------------------------
		//���캯��,�����������ʱ����޶�ֵ
		tdd_tick_t(bool enable = true, uint32_t expend_limit = 0, const char* file = NULL, int lineno = 0) :m_enable(enable)
		{
			m_file = (file == NULL ? "" : file);
			m_lineno = lineno;
			m_expend_limit = expend_limit;
			m_tmp_msg_b[0] = 0;
		}
		//-------------------------------------------------
		//ִ�п�ʼ,��¼��ʼʱ�������������
		void begin(const char* msg_a = NULL, const char* msg_b = NULL, ...)
		{
			if (!m_enable) return;
			va_list ap;
			va_start(ap, msg_b);
			st::vsnprintf(m_tmp_msg_b, sizeof(m_tmp_msg_b), msg_b, ap);
			va_end(ap);

			m_begin_tick = m_tick_meter.update();
			m_msg_a = (msg_a == NULL ? "" : msg_a);
			m_msg_b = m_tmp_msg_b;
			m_tab_deep = 0;
			m_last_hit_elapsed = 0;
			m_total_elapsed = 0;
			m_last_hit_time = m_begin_tick;
		}
		//-------------------------------------------------
		uint32_t &tab_deep() { return m_tab_deep; }
		//���һ��hit֮ǰ�ĺ�ʱ
		uint32_t last_hit_elapsed() { return m_last_hit_elapsed; }
		//ִ�е�end֮����ܺ�ʱ
		uint32_t total_end_elapsed() { return m_total_elapsed; }
		//���hit������ʱ���
		uint64_t last_hit_time() { return m_last_hit_time; }
		//��ȡ��ǰʱ���
		uint64_t curr_time() { return m_tick_meter.ticks(); }
		const char* msg_a() { return m_msg_a; }
		//-------------------------------------------------
		//���м�ʱ���м䶯��
		uint64_t hit()
		{
			if (!m_enable)
				return 0;
			m_last_hit_time = m_tick_meter.count();
			m_last_hit_elapsed = uint32_t(m_tick_meter.update() - m_last_hit_time);
			return m_tick_meter.count();
		}
		//-------------------------------------------------
		//��ʾ������һ�ζ�����ʱ
		void msg(bool is_hit, const char* file = NULL, int lineno = 0, const char* msg_c = NULL, ...)
		{
			if (!m_enable)
				return;

			char tab_str[32];
			uint32_t tab_deep = m_tab_deep + 1;
			if (tab_deep > sizeof(tab_str) - 1)
				tab_deep = sizeof(tab_str) - 1;
			memset(tab_str, '.', tab_deep);
			tab_str[tab_deep] = 0;

			va_list ap;
			va_start(ap, msg_c);
			char tmp1[512];
			st::vsnprintf(tmp1, sizeof(tmp1), msg_c, ap);
			va_end(ap);

			char tmp[512];
			st::snprintf(tmp, sizeof(tmp), "   ::< %s >:%s:%s (%.1f)ms {%s} : (%s:%u).", m_msg_a, (is_hit ? "hit" : "seg"), tab_str, m_last_hit_elapsed / 1000.0, tmp1, file, lineno);
			on_output(tmp);
		}
		//-------------------------------------------------
		//ִ�н���,�Զ��ж���ʱ,����ӡ�������
		void end()
		{
			if (!m_enable)
				return;
			m_total_elapsed = uint32_t(m_tick_meter.update() - m_begin_tick);
			if (m_total_elapsed < m_expend_limit)
				return;

			char tmp[512];
			st::snprintf(tmp, sizeof(tmp), "   ::< %s >:end: (%.1f)ms {%s} : (%s:%u).", m_msg_a, m_total_elapsed / 1000.0, m_msg_b, m_file, m_lineno);
			on_output(tmp);
			m_enable = false;
		}
		//-------------------------------------------------
		//��������,����ǰ��ʱ���Ƿ񳬹����޶�ֵ
		virtual ~tdd_tick_t()
		{
			end();
		}
		virtual void on_output(const char* str) { puts(str); }
	};

	//-----------------------------------------------------
	//���еδ��ʱ��tab��ȹ���,ͬʱ�����ʱ�Ĳ�������
	template<class tt = tdd_tick_t<> >
	class tdd_tick_tab_t
	{
		tt  &m_tdd_tick;
		const char* m_file;
		uint32_t m_lineno;
		char m_msg[128];
	public:
		//-------------------------------------------------
		tdd_tick_tab_t(tt& tc, const char* file = NULL, uint32_t lineno = 0, const char* msg_c = NULL, ...) :m_tdd_tick(tc)
		{
			m_file = file;
			m_lineno = lineno;

			va_list ap;
			va_start(ap, msg_c);
			st::vsnprintf(m_msg, sizeof(m_msg), msg_c, ap);
			va_end(ap);

			++m_tdd_tick.tab_deep();
		}
		~tdd_tick_tab_t()
		{
			m_tdd_tick.hit();
			m_tdd_tick.msg(false, m_file, m_lineno, m_msg);

			--m_tdd_tick.tab_deep();
		}
	};

	//-----------------------------------------------------
	//���еδ��ʱ��forѭ������,ͳ����ʱ���ƽ��ʱ��
	template<class tt = tdd_tick_t<> >
	class tdd_tick_for_t
	{
		tt          &m_tdd_tick;
		uint32_t    m_for_count;
		uint32_t    m_for_idx;
		uint64_t    m_begin_time;
		const char* m_file;
		uint32_t    m_lineno;
		char        m_msg[128];

	public:
		//-------------------------------------------------
		tdd_tick_for_t(tt& tc, uint32_t count, const char* file, uint32_t lineno, const char* fmt, ...) :m_tdd_tick(tc)
		{
			m_file = file;
			m_lineno = lineno;

			m_for_count = count;
			m_for_idx = 0;
			m_begin_time = m_tdd_tick.curr_time();

			va_list ap;
			va_start(ap, fmt);
			st::vsnprintf(m_msg, sizeof(m_msg), fmt, ap);
			va_end(ap);
		}
		~tdd_tick_for_t()
		{
			uint32_t usetime = uint32_t(m_tdd_tick.curr_time() - m_begin_time);

			char tab_str[32];
			uint32_t tab_deep = m_tdd_tick.tab_deep() + 1;
			if (tab_deep > sizeof(tab_str) - 1)
				tab_deep = sizeof(tab_str) - 1;
			memset(tab_str, '.', tab_deep);
			tab_str[tab_deep] = 0;

			char tmp[512];

			double total_ms = usetime / 1000.0;
			st::snprintf(tmp, sizeof(tmp), "   ::< %s >:for:%s (%.1f)ms/%d=(%.1f)ms {%s} : (%s:%u).", m_tdd_tick.msg_a(), tab_str, total_ms, m_for_count, total_ms / m_for_count, m_msg, m_file, m_lineno);
			m_tdd_tick.on_output(tmp);
		}
		//-------------------------------------------------
		bool step()
		{
			return (++m_for_idx <= m_for_count);
		}
	};
}

//-----------------------------------------------------
#if RX_USE_TDD_TICK
	//��������һ���δ��ʱ����,���Ը��������Ĳ���:sym������;��Ϣa;��Ϣb;��ѡ����ʱ����(Ĭ��Ϊ0)
#define tdd_tt(sym,msg_a,msg_b,...) rx::tdd_tick_t<> __tdd_tt_obj_##sym(tdd_tt_enable,0,__FILE__,__LINE__);__tdd_tt_obj_##sym.begin(msg_a,msg_b,##__VA_ARGS__)
//�����δ��ʱ������м����,����������ĺ�ʱ
#define tdd_tt_msg(sym,msgc,...) {__tdd_tt_obj_##sym.hit();__tdd_tt_obj_##sym.msg(true,__FILE__,__LINE__,msgc,##__VA_ARGS__);}

//���ڼ򻯶���һ��tt�δ��ʱ������м��Ƕ�׼�ʱ����
#define _tdd_tt_tab_0(sym,dis,msgc,...) rx::tdd_tick_tab_t<> __rx_tdd_tick_hit_obj_##dis(__tdd_tt_obj_##sym,__FILE__,__LINE__,msgc,##__VA_ARGS__)
//�м�궨��,Ϊ�˽���dis��չ��ת��
#define _tdd_tt_tab_1(sym,dis,msgc,...) _tdd_tt_tab_0(sym,dis,msgc,##__VA_ARGS__)
//����rx_tdd_rtl��,���ڱ�ݵĽ���һ��ָ�����ֺ����м��Ĳ�������
#define tdd_tt_tab(sym,msgc,...) _tdd_tt_tab_1(sym,RX_CT_SYM(sym),msgc,##__VA_ARGS__)

//���ж��ѭ�������ƽ��ִ��ʱ��
#define tdd_tt_for(sym,count,msgc,...) for(rx::tdd_tick_for_t<> __tt_obj(__tdd_tt_obj_##sym,count,__FILE__,__LINE__,msgc,##__VA_ARGS__);__tt_obj.step();)
#else
#define tdd_tt(sym,msg_a,msg_b,...)
#define tdd_tt_hit(sym,msgc,...)
#define _tdd_tt_tab(sym,dis,msgc,...)
#define _tdd_tt_tab_(sym,dis,msgc,...)
#define tdd_tt_tab(sym,msgc,...)
#define tdd_tt_for(sym,count,msgc,...)
#endif

#endif
