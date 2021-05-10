#ifndef _RX_LOGGER_BASE_H_
#define _RX_LOGGER_BASE_H_

#include "rx_cc_macro.h"
#include "rx_str_util_fmt.h"
/*
	��־��¼����ϵ����:
		logger_master_i/logger_master_t		�ռ���:��Ϊ��־��¼��ϵ������,����󶨶�������,��Ϊ��¼���ṩ��������.
		logger_filter_i						������:��Ϊ�ռ����еĹ��˲�,�����ھ����߼����ⲿ����ɸѡ��������ľ�������.
		logger_writer_i	    				�����:��־��������Խ��߼���־�������������̨���ļ�.
		logger_t							��¼��:����Ҫ��¼��־��ģ����,ʹ����־��¼��,��¼��������߼�����.��Ҫ�󶨵��ռ������ܹ���.
	Ϊ�˼򻯶�����󶨹���,ʵ����make_logger_confile/make_logger_file/make_logger_con����,��һ���Թ���ȫ��ʹ�õ���־��¼�����������.

	//��־�ռ����ӿ�,����־��¼��ʹ��
	class logger_master_i;
*/

namespace rx
{
	//-----------------------------------------------------
	//��־��Ϣ����
	typedef enum
	{
		LT_LEVEL_ERROR = 5,									//������Ϣ
		LT_LEVEL_WARN = 4,									//������Ϣ
		LT_LEVEL_INFO = 3,									//������Ϣ
		LT_LEVEL_CYCLE = 2,									//������Ϣ
		LT_LEVEL_DEBUG = 1,									//������Ϣ
		LT_LEVEL_NONE = 0,									//��ֹ���
	}logger_level_t;

	inline const char* logger_level_name(logger_level_t type)
	{
		switch (type)
		{
			case LT_LEVEL_ERROR:return "ERR";
			case LT_LEVEL_WARN:return "WRN";
			case LT_LEVEL_INFO:return "INF";
			case LT_LEVEL_CYCLE:return "CYC";
			case LT_LEVEL_DEBUG:return "DBG";
			default:return "";
		}
	}

	//ÿ����־��¼���������������������
	const uint32_t max_logger_writer_count = 4;

	//�﷨��,����־��¼���ӿڰ�ģ������
	#define logger_modname(logger) logger.modname(rx_src_filename,__LINE__)

	//-----------------------------------------------------
	//��־�ռ����ӿ�,��¼��logger_t��Ҫ�󶨶�Ӧ���ռ���.
	class logger_master_i
	{
		friend class logger_t;
	protected:
		virtual bool on_can_write(logger_level_t type) = 0;
		virtual void on_begin(logger_level_t type, uint32_t tag, uint64_t tex, const char* modname) = 0;
		virtual void on_vfmt(const char* fmt, va_list ap, uint64_t tex) = 0;
		virtual void on_hex(const void* data, uint32_t size, uint32_t pre_tab, uint32_t line_bytes, uint64_t tex) = 0;
		virtual void on_bin(const void* data, uint32_t size, uint64_t tex) = 0;
		virtual void on_end(uint64_t tex) = 0;
	};

	//-----------------------------------------------------
	//��־��¼��,����Ҫ��¼��־�ĵط��������ݵ�׫д,�����ù����������������.
	class logger_t
	{
		//-------------------------------------------------
		//�ڲ�ʹ�õ���־��д��
		class recoder_t
		{
			friend class logger_t;
			logger_master_i*m_master;                       //��־��¼�ӿڵĵײ�ת���ӿ�ָ��,ָ����־�ռ���
			uint32_t        m_last_seq;                     //������־���
			uint64_t        m_last_tex;                     //������־�����,this<<32|seq,��֪��־��¼��ÿ�ε�Ψһ����
			char            m_mod_name[32];                 //��־��¼�������Ĺ���ģ��
		public:
			//---------------------------------------------
			recoder_t():m_master(NULL), m_last_seq(0), m_last_tex(0) { m_mod_name[0] = 0; }
			//---------------------------------------------
			//�����־��ǰ�˹̶�����
			recoder_t& begin(logger_level_t type = LT_LEVEL_INFO, uint32_t tag = -1)
			{
				rx_assert_msg(m_last_tex == 0, "must call end()");//Ҫ��֮ǰ��end()���뱻����

				if (!m_master||!m_master->on_can_write(type))
					return *this;

				//���챾���������
				m_last_tex = (size_t)this;
				m_last_tex <<= 32;
				m_last_tex |= ++m_last_seq;

				m_master->on_begin(type, tag, m_last_tex, m_mod_name);

				return *this;
			}
			//---------------------------------------------
			//��ʽ�������־�ľ������ݲ���
			recoder_t& vfmt(const char* fmt, va_list ap)
			{
				if (m_master)
					m_master->on_vfmt(fmt, ap, m_last_tex);
				return *this;
			}
			recoder_t& fmt(const char* str, ...)
			{
				va_list ap;
				va_start(ap, str);
				vfmt(str, ap);
				va_end(ap);
				return *this;
			}
			recoder_t& operator()(const char* fmt, ...)
			{
				va_list ap;
				va_start(ap, fmt);
				vfmt(fmt, ap);
				va_end(ap);
				return *this;
			}
			//---------------------------------------------
			//��ʮ�������ַ�����ʽ������ݲ���:���ݻ�����ָ��,���ݳ���,ǰ׺�ո�����(���64),ÿ���ֽ�����(���256)
			recoder_t& hex(const void* data, uint32_t size, uint32_t pre_tab = 4, uint32_t line_bytes = 32)
			{
				if (m_master)
					m_master->on_hex(data, size, pre_tab, line_bytes, m_last_tex);
				return *this;
			}
			//---------------------------------------------
			//��ԭʼ�����Ʒ�ʽ������ݲ���
			recoder_t& bin(const void* data, uint32_t size)
			{
				if (m_master)
					m_master->on_bin(data, size, m_last_tex);
				return *this;
			}
			//---------------------------------------------
			//��֪������־���������
			void end()
			{
				if (m_master)
					m_master->on_end(m_last_tex);
				m_last_tex = 0;
			}
		};

	public:
		//-------------------------------------------------
		logger_t() {}
		logger_t(logger_master_i &log) { bind(log); }
		//-------------------------------------------------
		//����־�ռ���
		void bind(logger_master_i &log) { recoder.m_master = &log; }
		void bind(logger_t &log) { recoder.m_master = log.recoder.m_master; }
		logger_master_i* master() { return recoder.m_master; }
		//-------------------------------------------------
		//��ģ������(�����Ǹ��Ի���,������־�ӿڵĲ������ͬ)
		void modname(const char* name, uint32_t lno = 0)
		{
			if (lno)
				st::snprintf(recoder.m_mod_name, sizeof(recoder.m_mod_name), "%s:%u", name, lno);
			else
				st::strcpy(recoder.m_mod_name, sizeof(recoder.m_mod_name), name);
		}
		//-------------------------------------------------
		//�ڲ���д��,ʹ�ø����
		recoder_t recoder;
		//�﷨��,����recoder.begin()ʹ��
		recoder_t& operator()(logger_level_t type = LT_LEVEL_INFO, uint32_t tag = -1) { return recoder.begin(type, tag); }
		//-------------------------------------------------
		//��������������ڲ�ͳһʵ��
		void print(logger_level_t type, uint32_t tag, const char* fmt, va_list ap)
		{//��Ҳ����־��д������ȷ�÷�ʾ��,������begin(),�����end()
			recoder.begin(type, tag).vfmt(fmt, ap).end();
		}
		//-------------------------------------------------
		//���������Ϣ
		void error(uint32_t tag, const char* fmt, ...)
		{
			va_list ap;
			va_start(ap, fmt);
			print(LT_LEVEL_ERROR, tag, fmt, ap);
			va_end(ap);
		}
		void error(const char* fmt, ...)
		{
			va_list ap;
			va_start(ap, fmt);
			print(LT_LEVEL_ERROR, -1, fmt, ap);
			va_end(ap);
		}
		//-------------------------------------------------
		//���������Ϣ
		void warn(uint32_t tag, const char* fmt, ...)
		{
			va_list ap;
			va_start(ap, fmt);
			print(LT_LEVEL_WARN, tag, fmt, ap);
			va_end(ap);
		}
		void warn(const char* fmt, ...)
		{
			va_list ap;
			va_start(ap, fmt);
			print(LT_LEVEL_WARN, -1, fmt, ap);
			va_end(ap);
		}
		//-------------------------------------------------
		//�����ͨ��Ϣ
		void info(uint32_t tag, const char* fmt, ...)
		{
			va_list ap;
			va_start(ap, fmt);
			print(LT_LEVEL_INFO, tag, fmt, ap);
			va_end(ap);
		}
		void info(const char* fmt, ...)
		{
			va_list ap;
			va_start(ap, fmt);
			print(LT_LEVEL_INFO, -1, fmt, ap);
			va_end(ap);
		}
		//-------------------------------------------------
		//�����������Ϣ
		void cycle(uint32_t tag, const char* fmt, ...)
		{
			va_list ap;
			va_start(ap, fmt);
			print(LT_LEVEL_CYCLE, tag, fmt, ap);
			va_end(ap);
		}
		void cycle(const char* fmt, ...)
		{
			va_list ap;
			va_start(ap, fmt);
			print(LT_LEVEL_CYCLE, -1, fmt, ap);
			va_end(ap);
		}
		//-------------------------------------------------
		//���������Ϣ
		void debug(uint32_t tag, const char* fmt, ...)
		{
			va_list ap;
			va_start(ap, fmt);
			print(LT_LEVEL_DEBUG, tag, fmt, ap);
			va_end(ap);
		}
		void debug(const char* fmt, ...)
		{
			va_list ap;
			va_start(ap, fmt);
			print(LT_LEVEL_DEBUG, -1, fmt, ap);
			va_end(ap);
		}
	};
}


#endif
