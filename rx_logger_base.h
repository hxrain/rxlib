#ifndef _RX_LOGGER_BASE_H_
#define _RX_LOGGER_BASE_H_

#include "rx_cc_macro.h"
#include "rx_datetime.h"
#include "rx_str_util_bin.h"
#include "rx_str_util_fmt.h"
#include "rx_str_tiny.h"
/*
	//��־��¼���ӿ�
	class logger_i;

	//��־������ӿ�,�������ڿ���̨���/�ļ����/�������/���紫��ȵ�Ŀ��.
	class logger_writer_i;

	//��־�����:����̨���
	class logger_wrcon_t;

	//��־�����:�ļ����
	class logger_wrfile_t;

	��־��¼����ϵ����:
		���ⲿ��ȫ�ֶ�����־�����,���������������̨���ļ�.
		���ⲿ��ȫ�ֶ�����־��¼������,���������.
		����Ҫ��¼��־��ģ����,ʹ����־��¼���ӿ�logger_i,�󶨾���ļ�¼�������ʹ��.
	Ϊ�˼򻯶�����󶨹���,ʵ����make_logger_confile/make_logger_file/make_logger_con����,��һ���Թ���ȫ��ʹ�õ���־��¼�����������.
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
	//��־��¼���ӿ�
	class logger_i
	{
	protected:
		//��־��¼���ӿڵĺ��Ĺ��ܿ���������ʵ��;Ĭ��ʱ��־��¼���ӿڵĹ����ɰ󶨵������ӿڴ���.
		virtual bool on_can_write(logger_level_t type)
		{
			if (!m_logger) return false;
			return m_logger->on_can_write(type);
		}
		virtual void on_begin(logger_level_t type, uint32_t tag, uint64_t tex, const char* modname)
		{
			if (m_logger)
				m_logger->on_begin(type, tag, tex, modname);
		}
		virtual void on_vfmt(const char* fmt, va_list ap, uint64_t tex)
		{
			if (m_logger)
				m_logger->on_vfmt(fmt, ap, tex);
		}
		virtual void on_hex(const void* data, uint32_t size, uint32_t pre_tab, uint32_t line_bytes, uint64_t tex)
		{
			if (m_logger)
				m_logger->on_hex(data, size, pre_tab, line_bytes, tex);
		}
		virtual void on_bin(const void* data, uint32_t size, uint64_t tex)
		{
			if (m_logger)
				m_logger->on_bin(data, size, tex);
		}
		virtual void on_end(uint64_t tex)
		{
			if (m_logger)
				m_logger->on_end(tex);
		}
		logger_i    *m_logger;                              //��־��¼�ӿڵĵײ�ת���ӿ�ָ��
		//-----------------------------------------------------
		//�ڲ�ʹ�õ���־�����
		class writer_t
		{
			friend class logger_i;
			logger_i        *parent;                        //������ָ��
			uint32_t        m_last_seq;                     //������־���
			uint64_t        m_last_tex;                     //������־�����,this<<32|seq,��֪��־��¼��ÿ�ε�Ψһ����
			char            m_mod_name[32];                 //��־��¼�������Ĺ���ģ��
		public:
			//-------------------------------------------------
			writer_t() :parent(NULL), m_last_seq(0), m_last_tex(0) { m_mod_name[0] = 0; }
			//-------------------------------------------------
			//�����־��ǰ�˹̶�����
			writer_t& begin(logger_level_t type = LT_LEVEL_INFO, uint32_t tag = -1)
			{
				rx_assert(parent != NULL);
				rx_assert_msg(m_last_tex == 0, "must call end()");//Ҫ��֮ǰ��end()���뱻����

				if (!parent->on_can_write(type))
					return *this;

				//���챾���������
				m_last_tex = (size_t)this;
				m_last_tex <<= 32;
				m_last_tex |= ++m_last_seq;

				parent->on_begin(type, tag, m_last_tex, m_mod_name);

				return *this;
			}
			//-------------------------------------------------
			//��ʽ�������־�ľ������ݲ���
			writer_t& vfmt(const char* fmt, va_list ap)
			{
				parent->on_vfmt(fmt, ap, m_last_tex);
				return *this;
			}
			writer_t& fmt(const char* str, ...)
			{
				va_list ap;
				va_start(ap, str);
				vfmt(str, ap);
				va_end(ap);
				return *this;
			}
			writer_t& operator()(const char* fmt, ...)
			{
				va_list ap;
				va_start(ap, fmt);
				vfmt(fmt, ap);
				va_end(ap);
				return *this;
			}
			//-------------------------------------------------
			//��ʮ�������ַ�����ʽ������ݲ���:���ݻ�����ָ��,���ݳ���,ǰ׺�ո�����(���64),ÿ���ֽ�����(���256)
			writer_t& hex(const void* data, uint32_t size, uint32_t pre_tab = 4, uint32_t line_bytes = 32)
			{
				parent->on_hex(data, size, pre_tab, line_bytes, m_last_tex);
				return *this;
			}
			//-------------------------------------------------
			//��ԭʼ�����Ʒ�ʽ������ݲ���
			writer_t& bin(const void* data, uint32_t size)
			{
				parent->on_bin(data, size, m_last_tex);
				return *this;
			}
			//-------------------------------------------------
			//��֪������־���������
			void end()
			{
				parent->on_end(m_last_tex);
				m_last_tex = 0;
			}
		};

	public:
		logger_i() :m_logger(NULL) { writer.parent = this; }
		//-----------------------------------------------------
		//����־����ת���ӿ�
		void bind(logger_i &log) { m_logger = &log; }
		//-----------------------------------------------------
		//��ģ������(�����Ǹ��Ի���,������־�ӿڵĲ������ͬ)
		void modname(const char* name, uint32_t lno = 0)
		{
			if (lno)
				st::snprintf(writer.m_mod_name, sizeof(writer.m_mod_name), "%s:%u", name, lno);
			else
				st::strcpy(writer.m_mod_name, sizeof(writer.m_mod_name), name);
		}
		//-----------------------------------------------------
		//�ײ�����ӿ�,�����
		writer_t writer;
		//�﷨��,����writer.begin()ʹ��
		writer_t& operator()(logger_level_t type = LT_LEVEL_INFO, uint32_t tag = -1) { return writer.begin(type, tag); }
		//-----------------------------------------------------
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
		//-----------------------------------------------------
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
		//-----------------------------------------------------
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
		//-----------------------------------------------------
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
		//-----------------------------------------------------
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
		//-------------------------------------------------
		//���������Ϣ�ĵײ��ʽ������
		void print(logger_level_t type, uint32_t tag, const char* fmt, va_list ap)
		{//��Ҳ����־��д������ȷ�÷�ʾ��,������begin(),�����end()
			writer.begin(type, tag).vfmt(fmt, ap).end();
		}
	};

	//-----------------------------------------------------
	//��־������ӿ�,�������ڿ���̨���/�ļ����/�������/���紫��ȵ�Ŀ��.���ݾ����ʵ��,���Կ��̱߳������־��¼������ʹ��.
	//-----------------------------------------------------
	class logger_writer_i
	{
	public:
		virtual void on_begin(uint64_t tex, logger_level_t type, uint32_t tag, const char* msg, uint32_t msgsize) = 0;
		virtual void on_write(uint64_t tex, const void* data, uint32_t size) = 0;
		virtual void on_end(uint64_t tex) = 0;
		virtual ~logger_writer_i() {}
	};


	//-----------------------------------------------------
	//��־�����:����̨���
	template<class LT = null_lock_t>
	class logger_wrcon_t :public logger_writer_i
	{
	protected:
		LT          m_locker;
		FILE        *m_file;
		//-----------------------------------------------------
		void on_begin(uint64_t tex, logger_level_t type, uint32_t tag, const char* msg, uint32_t msgsize)
		{
			if (!is_valid()) return;
			m_locker.lock();
			fwrite(msg, 1, msgsize, m_file);
		}
		//-----------------------------------------------------
		void on_write(uint64_t tex, const void* data, uint32_t size)
		{
			if (!is_valid()) return;
			fwrite(data, 1, size, m_file);
		}
		//-----------------------------------------------------
		void on_end(uint64_t tex)
		{
			if (!is_valid()) return;
			putc('\n', m_file);
			fflush(m_file);
			m_locker.unlock();
		}
		//-----------------------------------------------------
		virtual bool is_valid() { return true; }
	public:
		logger_wrcon_t() :m_file(stdout) {}
	};

	//-----------------------------------------------------
	//��־�����:�ļ����
	template<class LT = null_lock_t>
	class logger_wrfile_t :public logger_wrcon_t<LT>
	{
		typedef logger_wrcon_t<LT> super_t;
	public:
		//-----------------------------------------------------
		logger_wrfile_t() { super_t::m_file = NULL; }
		~logger_wrfile_t() { close(); }
		bool is_valid() { return super_t::m_file != NULL; }
		//-----------------------------------------------------
		bool open(const char* filepath, bool append = false)
		{
			super_t::m_file = fopen(filepath, append ? "ab+" : "wb+");
			if (super_t::m_file == NULL)
				return false;
			return true;
		}
		//-----------------------------------------------------
		void close()
		{
			if (super_t::m_file)
			{
				fclose(super_t::m_file);
				super_t::m_file = NULL;
			}
		}
		//-----------------------------------------------------
	};
}


#endif
