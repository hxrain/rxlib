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
	//��־������ӿ�,�������ڿ���̨���/�ļ����/�������/���紫��ȵ�Ŀ��.
	class logger_writer_i;

	//��־�����:����̨���
	class logger_wrcon_t;

	//��־�����:�ļ����
	class logger_wrfile_t;

	//��־�ռ���,���԰󶨶����־�����,�����߳��е���־��¼������
	template<class LT>
	class logger_master_t;
*/

namespace rx
{

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
	//��־�ռ���,���԰󶨶����־�����,�ɿ��̱߳������־��¼������
	//-----------------------------------------------------
	template<class LT = null_lock_t>
	class logger_master_t :public logger_master_i
	{
		logger_writer_i     *m_writers[max_logger_writer_count];
		uint32_t            m_writer_count;                 //�󶨹������������
		logger_level_t      m_can_level;                    //�����������־����,>=m_can_level�ſ������,Ĭ��Ϊ���ȫ��
		uint32_t            m_pid;
		LT                  m_locker;
		//-------------------------------------------------
		//fmt��ʽ����Ҫ�ĵײ������
		class fmt_follower_logger :public fmt_imp::fmt_follower_null<char>
		{
			typedef fmt_imp::fmt_follower_null<char> super_t;
			logger_master_t        *parent;
			char            m_buff[512];                    //�ڲ����е���ʱ������
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
			{//��ʽ������������ݴ�һ��,�������ַ�������ײ�
				super_t::buffer[super_t::idx++] = character;
				++super_t::count;
				if (super_t::idx >= super_t::maxlen)
				{
					//��������а󶨵������
					parent->_writers_output(m_last_tex, super_t::buffer, (uint32_t)super_t::maxlen);
					super_t::idx = 0;
				}
			}
			//-----------------------------------------
			void end()
			{//����ʣ��ĸ�ʽ�������ȫ��������ײ�
				if (super_t::idx)
				{
					//��������а󶨵������
					parent->_writers_output(m_last_tex, super_t::buffer, (uint32_t)super_t::idx);
					super_t::idx = 0;
				}
			}
		};
		//-----------------------------------------------------
		//ѭ���ڲ������,��һ����ָ�����ݵ��������.
		void _writers_output(uint64_t tex, const void* data, uint32_t size)
		{
			for (uint32_t i = 0; i < m_writer_count; ++i)
				m_writers[i]->on_write(tex, data, size);
		}
		//-----------------------------------------------------
		//�ж��Ƿ�������ǰ�������־����
		virtual bool on_can_write(logger_level_t type)
		{
			return (type >= m_can_level) && m_writer_count;
		}
		//-----------------------------------------------------
		//��ʼһ����־����Ĵ���
		virtual void on_begin(logger_level_t type, uint32_t tag, uint64_t tex, const char* modname)
		{
			rx_assert(tex != 0);

			m_locker.lock();								//begin�������������,Ҫ��������end�����������.

			tiny_string_t<char, 512> scat;

			//ƴװ��ǰʱ��/��־����/pid/tid
			char dt[24];
			rx_datetime2iso(dt, "%u-%02u-%02uT%02u:%02u:%02u.%03u", true);
			scat("[%s|", dt) ("%s|", logger_level_name(type)) ("PID:%4u|", m_pid) ("TID:%4u]", get_tid());

			//���������Ч��mod���ƺ�tag���
			if (!is_empty(modname))
				scat("[%s]", modname);
			if (tag != (uint32_t)-1)
				scat("[TAG:%6u]", tag);
			scat("[TEX:%016zx]", tex);
			scat << ' ';

			//ѭ����������а󶨵������
			for (uint32_t i = 0; i < m_writer_count; ++i)
				m_writers[i]->on_begin(tex, type, tag, scat.c_str(), scat.size());
		}
		//-----------------------------------------------------
		//�ڵ�ǰ��־�����������ʽ��ƴװ����
		virtual void on_vfmt(const char* fmt, va_list ap, uint64_t tex)
		{
			if (tex == 0 || m_writer_count == 0)
				return;
			fmt_follower_logger fbuf(this, tex);
			fmt_imp::fmt_core(fbuf, fmt, ap);
		}
		//-----------------------------------------------------
		//�ڵ�ǰ��־���������HEX��������
		virtual void on_hex(const void* data, uint32_t size, uint32_t pre_tab, uint32_t line_bytes, uint64_t tex)
		{
			if (tex == 0 || m_writer_count == 0)
				return;

			char line_buff[256 * 4];						//��ǰ�����������
			line_bytes = min(line_bytes, (uint32_t)256);	//ÿ���������ֽ�����
			pre_tab = min(pre_tab, (uint32_t)64);			//ÿ�����ǰ׺�ո�����
			const uint8_t *bytes = (const uint8_t*)data;	//������Ļ�����ָ��
			uint32_t remain = size;							//ʣ�����ݳ���
			while (remain)
			{
				uint32_t blocks = min(line_bytes, remain);	//���д�������ֽڳ���
				uint32_t sl = 0;
				for (; sl < pre_tab; ++sl)
					line_buff[sl] = ' ';					//��ǰ�����ǰ׺�հ�

				for (uint32_t t = 0; t < blocks; ++t)
				{//ѭ�������ǰ������Ϊhex
					st::hex2(*bytes, line_buff + sl);
					++bytes;
					sl += 2;
					line_buff[sl++] = ' ';
				}
				remain -= blocks;
				if (remain)									//����������,��ô������������Ͳ��ϻس����кͽ�����
				{
					line_buff[sl++] = '\n';
					line_buff[sl] = '0';
				}
				//��������а󶨵������
				_writers_output(tex, line_buff, sl);
			}
		}
		//-----------------------------------------------------
		//�ڵ�ǰ��־���������ԭʼ��������������
		virtual void on_bin(const void* data, uint32_t size, uint64_t tex)
		{
			if (tex == 0 || m_writer_count == 0)
				return;

			//ѭ����������а󶨵������
			_writers_output(tex, data, size);
		}
		//-----------------------------------------------------
		//����һ����־����
		virtual void on_end(uint64_t tex)
		{
			if (tex == 0 || m_writer_count == 0)
				return;

			//ѭ��֪ͨ�����а󶨵������
			for (uint32_t i = 0; i < m_writer_count; ++i)
				m_writers[i]->on_end(tex);

			m_locker.unlock();								//�������
		}
	public:
		logger_master_t(logger_writer_i *wr1 = NULL, logger_writer_i* wr2 = NULL) :m_writer_count(0), m_can_level(LT_LEVEL_DEBUG)
		{
			m_pid = (uint32_t)get_pid();
			if (wr1) bind(*wr1);
			if (wr2) bind(*wr2);
		}
		//-----------------------------------------------------
		//��������ӿ�
		bool bind(logger_writer_i& w)
		{
			if (m_writer_count >= max_logger_writer_count)
				return false;
			m_writers[m_writer_count++] = &w;
			return true;
		}
		//-----------------------------------------------------
		//����������������,0��ȫ��ֹ���
		void level(logger_level_t lvl) { m_can_level = lvl; }
		//-----------------------------------------------------
		uint32_t writers() { return m_writer_count; }

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


	//---------------------------------------------------------
	//��ȫ��ʹ��,������ļ��Ϳ���̨����־��¼��(��ʵ��)
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

	//��ȫ��ʹ��,������ļ�����־��¼��(ͨ�����ֲ�ͬ��seq����ʹ�ö��ʵ��)
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

	//��ȫ��ʹ��,���������̨����־��¼��(��ʵ��)
	logger_t& make_logger_con()
	{
		static logger_wrcon_t<> logger_wrcon;
		static logger_master_t<locker_t> logger_master(&logger_wrcon);
		static logger_t logger(logger_master);
		return logger;
	}

}


#endif
