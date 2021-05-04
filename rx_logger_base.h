#ifndef _RX_LOGGER_BASE_H_
#define _RX_LOGGER_BASE_H_

#include "rx_cc_macro.h"
#include "rx_datetime.h"
#include "rx_str_util_bin.h"
#include "rx_str_util_fmt.h"
#include "rx_str_tiny.h"
/*
	//日志记录器接口
	class logger_i;

	//日志输出器接口,可以用于控制台输出/文件输出/调试输出/网络传输等等目的.
	class logger_writer_i;

	//日志输出器:控制台输出
	class logger_wrcon_t;

	//日志输出器:文件输出
	class logger_wrfile_t;

	日志记录器体系简述:
		在外部或全局定义日志输出器,可以是输出到控制台或文件.
		在外部或全局定义日志记录器对象,并绑定输出器.
		在需要记录日志的模块中,使用日志记录器接口logger_i,绑定具体的记录器后进行使用.
	为了简化定义与绑定过程,实现了make_logger_confile/make_logger_file/make_logger_con函数,可一次性构造全局使用的日志记录器并绑定输出器.
*/

namespace rx
{
	//-----------------------------------------------------
	//日志消息类型
	typedef enum
	{
		LT_LEVEL_ERROR = 5,									//错误信息
		LT_LEVEL_WARN = 4,									//警告信息
		LT_LEVEL_INFO = 3,									//常规信息
		LT_LEVEL_CYCLE = 2,									//周期消息
		LT_LEVEL_DEBUG = 1,									//调试消息
		LT_LEVEL_NONE = 0,									//禁止输出
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

	//每个日志记录器允许绑定输出器的最大数量
	const uint32_t max_logger_writer_count = 4;

	//语法糖,给日志记录器接口绑定模块特征
	#define logger_modname(logger) logger.modname(rx_src_filename,__LINE__)

	//-----------------------------------------------------
	//日志记录器接口
	class logger_i
	{
	protected:
		//日志记录器接口的核心功能可由子类来实现;默认时日志记录器接口的功能由绑定的其他接口处理.
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
		logger_i    *m_logger;                              //日志记录接口的底层转发接口指针
		//-----------------------------------------------------
		//内部使用的日志输出器
		class writer_t
		{
			friend class logger_i;
			logger_i        *parent;                        //父对象指针
			uint32_t        m_last_seq;                     //最后的日志序号
			uint64_t        m_last_tex;                     //最后的日志事务号,this<<32|seq,告知日志记录器每次的唯一事务
			char            m_mod_name[32];                 //日志记录器所属的功能模块
		public:
			//-------------------------------------------------
			writer_t() :parent(NULL), m_last_seq(0), m_last_tex(0) { m_mod_name[0] = 0; }
			//-------------------------------------------------
			//输出日志的前端固定部分
			writer_t& begin(logger_level_t type = LT_LEVEL_INFO, uint32_t tag = -1)
			{
				rx_assert(parent != NULL);
				rx_assert_msg(m_last_tex == 0, "must call end()");//要求之前的end()必须被调用

				if (!parent->on_can_write(type))
					return *this;

				//构造本次事务序号
				m_last_tex = (size_t)this;
				m_last_tex <<= 32;
				m_last_tex |= ++m_last_seq;

				parent->on_begin(type, tag, m_last_tex, m_mod_name);

				return *this;
			}
			//-------------------------------------------------
			//格式化输出日志的具体内容部分
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
			//以十六进制字符串方式输出数据部分:数据缓冲区指针,数据长度,前缀空格数量(最大64),每行字节数量(最大256)
			writer_t& hex(const void* data, uint32_t size, uint32_t pre_tab = 4, uint32_t line_bytes = 32)
			{
				parent->on_hex(data, size, pre_tab, line_bytes, m_last_tex);
				return *this;
			}
			//-------------------------------------------------
			//以原始二进制方式输出数据部分
			writer_t& bin(const void* data, uint32_t size)
			{
				parent->on_bin(data, size, m_last_tex);
				return *this;
			}
			//-------------------------------------------------
			//告知本次日志输出结束了
			void end()
			{
				parent->on_end(m_last_tex);
				m_last_tex = 0;
			}
		};

	public:
		logger_i() :m_logger(NULL) { writer.parent = this; }
		//-----------------------------------------------------
		//绑定日志功能转发接口
		void bind(logger_i &log) { m_logger = &log; }
		//-----------------------------------------------------
		//绑定模块名称(名称是个性化的,随着日志接口的部署而不同)
		void modname(const char* name, uint32_t lno = 0)
		{
			if (lno)
				st::snprintf(writer.m_mod_name, sizeof(writer.m_mod_name), "%s:%u", name, lno);
			else
				st::strcpy(writer.m_mod_name, sizeof(writer.m_mod_name), name);
		}
		//-----------------------------------------------------
		//底层输出接口,更灵活
		writer_t writer;
		//语法糖,当作writer.begin()使用
		writer_t& operator()(logger_level_t type = LT_LEVEL_INFO, uint32_t tag = -1) { return writer.begin(type, tag); }
		//-----------------------------------------------------
		//输出错误信息
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
		//输出警告信息
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
		//输出普通信息
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
		//输出周期性信息
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
		//输出调试信息
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
		//各类输出信息的底层格式化方法
		void print(logger_level_t type, uint32_t tag, const char* fmt, va_list ap)
		{//这也是日志编写器的正确用法示例,必须先begin(),最后再end()
			writer.begin(type, tag).vfmt(fmt, ap).end();
		}
	};

	//-----------------------------------------------------
	//日志输出器接口,可以用于控制台输出/文件输出/调试输出/网络传输等等目的.根据具体的实现,可以跨线程被多个日志记录器并发使用.
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
	//日志输出器:控制台输出
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
	//日志输出器:文件输出
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
