#ifndef _RX_LOGGER_BASE_H_
#define _RX_LOGGER_BASE_H_

#include "rx_cc_macro.h"
#include "rx_str_util_fmt.h"
/*
	日志记录器体系简述:
		logger_master_i/logger_master_t		收集器:作为日志记录体系的中心,管理绑定多个输出器,并为记录器提供基础服务.
		logger_filter_i						过滤器:作为收集器中的过滤层,可以在具体逻辑的外部灵活的筛选控制输出的具体内容.
		logger_writer_i	    				输出器:日志输出器可以将逻辑日志内容输出到控制台或文件.
		logger_t							记录器:在需要记录日志的模块中,使用日志记录器,记录待输出的逻辑内容.需要绑定到收集器才能工作.
	为了简化定义与绑定过程,实现了make_logger_confile/make_logger_file/make_logger_con函数,可一次性构造全局使用的日志记录器并绑定输出器.

	//日志收集器接口,供日志记录器使用
	class logger_master_i;
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
	//日志收集器接口,记录器logger_t需要绑定对应的收集器.
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
	//日志记录器,在需要记录日志的地方进行内容的撰写,但不用关心内容输出到哪里.
	class logger_t
	{
		//-------------------------------------------------
		//内部使用的日志编写器
		class recoder_t
		{
			friend class logger_t;
			logger_master_i*m_master;                       //日志记录接口的底层转发接口指针,指向日志收集器
			uint32_t        m_last_seq;                     //最后的日志序号
			uint64_t        m_last_tex;                     //最后的日志事务号,this<<32|seq,告知日志记录器每次的唯一事务
			char            m_mod_name[32];                 //日志记录器所属的功能模块
		public:
			//---------------------------------------------
			recoder_t():m_master(NULL), m_last_seq(0), m_last_tex(0) { m_mod_name[0] = 0; }
			//---------------------------------------------
			//输出日志的前端固定部分
			recoder_t& begin(logger_level_t type = LT_LEVEL_INFO, uint32_t tag = -1)
			{
				rx_assert_msg(m_last_tex == 0, "must call end()");//要求之前的end()必须被调用

				if (!m_master||!m_master->on_can_write(type))
					return *this;

				//构造本次事务序号
				m_last_tex = (size_t)this;
				m_last_tex <<= 32;
				m_last_tex |= ++m_last_seq;

				m_master->on_begin(type, tag, m_last_tex, m_mod_name);

				return *this;
			}
			//---------------------------------------------
			//格式化输出日志的具体内容部分
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
			//以十六进制字符串方式输出数据部分:数据缓冲区指针,数据长度,前缀空格数量(最大64),每行字节数量(最大256)
			recoder_t& hex(const void* data, uint32_t size, uint32_t pre_tab = 4, uint32_t line_bytes = 32)
			{
				if (m_master)
					m_master->on_hex(data, size, pre_tab, line_bytes, m_last_tex);
				return *this;
			}
			//---------------------------------------------
			//以原始二进制方式输出数据部分
			recoder_t& bin(const void* data, uint32_t size)
			{
				if (m_master)
					m_master->on_bin(data, size, m_last_tex);
				return *this;
			}
			//---------------------------------------------
			//告知本次日志输出结束了
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
		//绑定日志收集器
		void bind(logger_master_i &log) { recoder.m_master = &log; }
		void bind(logger_t &log) { recoder.m_master = log.recoder.m_master; }
		logger_master_i* master() { return recoder.m_master; }
		//-------------------------------------------------
		//绑定模块名称(名称是个性化的,随着日志接口的部署而不同)
		void modname(const char* name, uint32_t lno = 0)
		{
			if (lno)
				st::snprintf(recoder.m_mod_name, sizeof(recoder.m_mod_name), "%s:%u", name, lno);
			else
				st::strcpy(recoder.m_mod_name, sizeof(recoder.m_mod_name), name);
		}
		//-------------------------------------------------
		//内部编写器,使用更灵活
		recoder_t recoder;
		//语法糖,当作recoder.begin()使用
		recoder_t& operator()(logger_level_t type = LT_LEVEL_INFO, uint32_t tag = -1) { return recoder.begin(type, tag); }
		//-------------------------------------------------
		//各类输出方法的内部统一实现
		void print(logger_level_t type, uint32_t tag, const char* fmt, va_list ap)
		{//这也是日志编写器的正确用法示例,必须先begin(),最后再end()
			recoder.begin(type, tag).vfmt(fmt, ap).end();
		}
		//-------------------------------------------------
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
		//-------------------------------------------------
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
		//-------------------------------------------------
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
		//-------------------------------------------------
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
		//-------------------------------------------------
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
	};
}


#endif
