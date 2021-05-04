#ifndef _RX_LOGGER_H_
#define _RX_LOGGER_H_

#include "rx_cc_macro.h"
#include "rx_logger_base.h"
#include "rx_os_misc.h"
#include "rx_lock_os.h"
/*
	//��־��¼��,���԰󶨶����־�����,�����߳��е���־��¼���ӿ�����
	template<class LT>
	class logger_t;
*/

namespace rx
{
	//-----------------------------------------------------
	//��־��¼��,���԰󶨶����־�����,�ɿ��̱߳������־��¼���ӿ�����
	//-----------------------------------------------------
	template<class LT = null_lock_t>
	class logger_t :public logger_i
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
			logger_t        *parent;
			char            m_buff[512];                    //�ڲ����е���ʱ������
			uint64_t        m_last_tex;
		public:
			//-----------------------------------------
			fmt_follower_logger(logger_t* o, uint64_t tex)
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
					//ѭ����������а󶨵������
					for (uint32_t i = 0; i < parent->m_writer_count; ++i)
						parent->m_writers[i]->on_write(m_last_tex, super_t::buffer, (uint32_t)super_t::maxlen);
					super_t::idx = 0;
				}
			}
			//-----------------------------------------
			void end()
			{//����ʣ��ĸ�ʽ�������ȫ��������ײ�
				if (super_t::idx)
				{
					//ѭ����������а󶨵������
					for (uint32_t i = 0; i < parent->m_writer_count; ++i)
						parent->m_writers[i]->on_write(m_last_tex, super_t::buffer, (uint32_t)super_t::idx);
					super_t::idx = 0;
				}
			}
		};
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
					line_buff[sl] = ' ';					//��ǰ�����ǰ׺

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
				//ѭ����������а󶨵������
				for (uint32_t i = 0; i < m_writer_count; ++i)
					m_writers[i]->on_write(tex, line_buff, sl);
			}
		}
		//-----------------------------------------------------
		//�ڵ�ǰ��־���������ԭʼ��������������
		virtual void on_bin(const void* data, uint32_t size, uint64_t tex)
		{
			if (tex == 0 || m_writer_count == 0)
				return;

			//ѭ����������а󶨵������
			for (uint32_t i = 0; i < m_writer_count; ++i)
				m_writers[i]->on_write(tex, data, size);
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
		//�������ظ��ӿ��е�ת���󶨹��ܽӿ�
		void bind(logger_i log) {}
	public:
		logger_t(logger_writer_i *wr1 = NULL, logger_writer_i* wr2 = NULL) :m_writer_count(0), m_can_level(LT_LEVEL_DEBUG)
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

	//---------------------------------------------------------
	//��ȫ��ʹ��,������ļ��Ϳ���̨����־��¼��(��ʵ��)
	logger_i& make_logger_confile(const char* fname = "./log.txt", bool append = false)
	{
		static logger_t<locker_t> logger;
		static logger_wrfile_t<> logger_wrfile;
		static logger_wrcon_t<> logger_wrcon;
		if (logger.writers() == 0)
		{
			rx_check(logger_wrfile.open(fname, append));
			logger.bind(logger_wrfile);
			logger.bind(logger_wrcon);
		}
		return logger;
	}

	//��ȫ��ʹ��,������ļ�����־��¼��(ͨ�����ֲ�ͬ��seq����ʹ�ö��ʵ��)
	template<int seq>
	logger_i& make_logger_file(const char* fname = "./log.txt", bool append = false)
	{
		static logger_t<locker_t> logger;
		static logger_wrfile_t<> logger_wrfile;
		if (logger.writers() == 0)
		{
			rx_check(logger_wrfile.open(fname, append));
			logger.bind(logger_wrfile);
		}
		return logger;
	}

	//��ȫ��ʹ��,���������̨����־��¼��(��ʵ��)
	logger_i& make_logger_con()
	{
		static logger_wrcon_t<> logger_wrcon;
		static logger_t<locker_t> logger(&logger_wrcon);
		return logger;
	}

}


#endif
