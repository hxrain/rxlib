#ifndef _RX_LOGGER_H_
#define _RX_LOGGER_H_

#include "rx_cc_macro.h"
#include "rx_logger.h"
#include "rx_os_misc.h"
#include "rx_lock_os.h"
#include "rx_datetime.h"
#include "rx_str_util_bin.h"
#include "rx_str_util_fmt.h"
#include "rx_str_tiny.h"

/*
	//日志输出器接口,可以用于控制台输出/文件输出/调试输出/网络传输等等目的.
	class logger_writer_i;

	//日志输出器:控制台输出
	class logger_wrcon_t;

	//日志输出器:文件输出
	class logger_wrfile_t;

	//日志收集器,可以绑定多个日志输出器,被多线程中的日志记录器引用
	template<class LT>
	class logger_master_t;
*/

namespace rx
{

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
	//日志收集器,可以绑定多个日志输出器,可跨线程被多个日志记录器引用
	//-----------------------------------------------------
	template<class LT = null_lock_t>
	class logger_master_t :public logger_master_i
	{
		logger_writer_i     *m_writers[max_logger_writer_count];
		uint32_t            m_writer_count;                 //绑定过的输出器数量
		logger_level_t      m_can_level;                    //允许输出的日志级别,>=m_can_level才可以输出,默认为输出全部
		uint32_t            m_pid;
		LT                  m_locker;
		//-------------------------------------------------
		//fmt格式化需要的底层输出器
		class fmt_follower_logger :public fmt_imp::fmt_follower_null<char>
		{
			typedef fmt_imp::fmt_follower_null<char> super_t;
			logger_master_t        *parent;
			char            m_buff[512];                    //内部持有的临时缓冲区
			uint64_t        m_last_tex;
		public:
			//-----------------------------------------
			fmt_follower_logger(logger_master_t* o, uint64_t tex)
			{
				parent = o;
				m_last_tex = tex;
				super_t::bind(m_buff, sizeof(m_buff));
			}
			//-----------------------------------------
			void operator ()(char character)
			{//格式化后的内容先暂存一下,避免逐字符输出给底层
				super_t::buffer[super_t::idx++] = character;
				++super_t::count;
				if (super_t::idx >= super_t::maxlen)
				{
					//输出给所有绑定的输出器
					parent->_writers_output(m_last_tex, super_t::buffer, (uint32_t)super_t::maxlen);
					super_t::idx = 0;
				}
			}
			//-----------------------------------------
			void end()
			{//还有剩余的格式化结果则全部输出给底层
				if (super_t::idx)
				{
					//输出给所有绑定的输出器
					parent->_writers_output(m_last_tex, super_t::buffer, (uint32_t)super_t::idx);
					super_t::idx = 0;
				}
			}
		};
		//-----------------------------------------------------
		//循环内部输出器,逐一进行指定数据的输出操作.
		void _writers_output(uint64_t tex, const void* data, uint32_t size)
		{
			for (uint32_t i = 0; i < m_writer_count; ++i)
				m_writers[i]->on_write(tex, data, size);
		}
		//-----------------------------------------------------
		//判断是否可输出当前级别的日志内容
		virtual bool on_can_write(logger_level_t type)
		{
			return (type >= m_can_level) && m_writer_count;
		}
		//-----------------------------------------------------
		//开始一次日志事务的处理
		virtual void on_begin(logger_level_t type, uint32_t tag, uint64_t tex, const char* modname)
		{
			rx_assert(tex != 0);

			m_locker.lock();								//begin处进行整体加锁,要求必须调用end进行整体解锁.

			tiny_string_t<char, 512> scat;

			//拼装当前时间/日志级别/pid/tid
			char dt[24];
			rx_datetime2iso(dt, "%u-%02u-%02uT%02u:%02u:%02u.%03u", true);
			scat("[%s|", dt) ("%s|", logger_level_name(type)) ("PID:%4u|", m_pid) ("TID:%4u]", get_tid());

			//尝试输出有效的mod名称和tag标记
			if (!is_empty(modname))
				scat("[%s]", modname);
			if (tag != (uint32_t)-1)
				scat("[TAG:%6u]", tag);
			scat("[TEX:%016zx]", tex);
			scat << ' ';

			//循环输出给所有绑定的输出器
			for (uint32_t i = 0; i < m_writer_count; ++i)
				m_writers[i]->on_begin(tex, type, tag, scat.c_str(), scat.size());
		}
		//-----------------------------------------------------
		//在当前日志事务中输出格式化拼装内容
		virtual void on_vfmt(const char* fmt, va_list ap, uint64_t tex)
		{
			if (tex == 0 || m_writer_count == 0)
				return;
			fmt_follower_logger fbuf(this, tex);
			fmt_imp::fmt_core(fbuf, fmt, ap);
		}
		//-----------------------------------------------------
		//在当前日志事务中输出HEX数据内容
		virtual void on_hex(const void* data, uint32_t size, uint32_t pre_tab, uint32_t line_bytes, uint64_t tex)
		{
			if (tex == 0 || m_writer_count == 0)
				return;

			char line_buff[256 * 4];						//当前行输出缓冲区
			line_bytes = min(line_bytes, (uint32_t)256);	//每行最大输出字节数量
			pre_tab = min(pre_tab, (uint32_t)64);			//每行最大前缀空格数量
			const uint8_t *bytes = (const uint8_t*)data;	//待处理的缓冲区指针
			uint32_t remain = size;							//剩余数据长度
			while (remain)
			{
				uint32_t blocks = min(line_bytes, remain);	//本行待处理的字节长度
				uint32_t sl = 0;
				for (; sl < pre_tab; ++sl)
					line_buff[sl] = ' ';					//当前行填充前缀空白

				for (uint32_t t = 0; t < blocks; ++t)
				{//循环输出当前行数据为hex
					st::hex2(*bytes, line_buff + sl);
					++bytes;
					sl += 2;
					line_buff[sl++] = ' ';
				}
				remain -= blocks;
				if (remain)									//不是最后的行,那么行输出缓冲区就补上回车换行和结束符
				{
					line_buff[sl++] = '\n';
					line_buff[sl] = '0';
				}
				//输出给所有绑定的输出器
				_writers_output(tex, line_buff, sl);
			}
		}
		//-----------------------------------------------------
		//在当前日志事务中输出原始二进制数据内容
		virtual void on_bin(const void* data, uint32_t size, uint64_t tex)
		{
			if (tex == 0 || m_writer_count == 0)
				return;

			//循环输出给所有绑定的输出器
			_writers_output(tex, data, size);
		}
		//-----------------------------------------------------
		//结束一次日志事务
		virtual void on_end(uint64_t tex)
		{
			if (tex == 0 || m_writer_count == 0)
				return;

			//循环通知给所有绑定的输出器
			for (uint32_t i = 0; i < m_writer_count; ++i)
				m_writers[i]->on_end(tex);

			m_locker.unlock();								//整体解锁
		}
	public:
		logger_master_t(logger_writer_i *wr1 = NULL, logger_writer_i* wr2 = NULL) :m_writer_count(0), m_can_level(LT_LEVEL_DEBUG)
		{
			m_pid = (uint32_t)get_pid();
			if (wr1) bind(*wr1);
			if (wr2) bind(*wr2);
		}
		//-----------------------------------------------------
		//绑定输出器接口
		bool bind(logger_writer_i& w)
		{
			if (m_writer_count >= max_logger_writer_count)
				return false;
			m_writers[m_writer_count++] = &w;
			return true;
		}
		//-----------------------------------------------------
		//调整允许的输出级别,0完全禁止输出
		void level(logger_level_t lvl) { m_can_level = lvl; }
		//-----------------------------------------------------
		uint32_t writers() { return m_writer_count; }

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


	//---------------------------------------------------------
	//可全局使用,输出到文件和控制台的日志记录器(单实例)
	logger_t& make_logger_confile(const char* fname = "./log.txt", bool append = false)
	{
		static logger_master_t<locker_t> logger_master;
		static logger_t	logger;
		static logger_wrfile_t<> logger_wrfile;
		static logger_wrcon_t<> logger_wrcon;
		if (logger_master.writers() == 0)
		{
			rx_check(logger_wrfile.open(fname, append));
			logger_master.bind(logger_wrfile);
			logger_master.bind(logger_wrcon);
			logger.bind(logger_master);
		}
		return logger;
	}

	//可全局使用,输出到文件的日志记录器(通过区分不同的seq可以使用多个实例)
	template<int seq>
	logger_t& make_logger_file(const char* fname = "./log.txt", bool append = false)
	{
		static logger_master_t<locker_t> logger_master;
		static logger_t	logger;
		static logger_wrfile_t<> logger_wrfile;
		if (logger_master.writers() == 0)
		{
			rx_check(logger_wrfile.open(fname, append));
			logger_master.bind(logger_wrfile);
			logger.bind(logger_master);
		}
		return logger;
	}

	//可全局使用,输出到控制台的日志记录器(单实例)
	logger_t& make_logger_con()
	{
		static logger_wrcon_t<> logger_wrcon;
		static logger_master_t<locker_t> logger_master(&logger_wrcon);
		static logger_t logger(logger_master);
		return logger;
	}

}


#endif
