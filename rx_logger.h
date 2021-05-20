#ifndef _RX_LOGGER_BASE_H_
#define _RX_LOGGER_BASE_H_

#include "rx_cc_macro.h"
#include "rx_str_util_fmt.h"
/*
	��־��¼����ϵ����:
		logger_master_i/logger_master_t		�ռ���:��Ϊ��־��¼��ϵ������,����󶨶�������,��Ϊ��¼���ṩ��������.
		logger_filter_i						������:��Ϊ�ռ����еĹ��˲�,�����ھ����߼����ⲿ����ɸѡ��������ľ�������.
		logger_writer_i/logger_wrcon_t		�����:��־��������Խ��߼���־�������������̨���ļ�.
		logger_t							��¼��:����Ҫ��¼��־��ģ����,ʹ����־��¼��,��¼��������߼�����.��Ҫ�󶨵��ռ������ܹ���.
	Ϊ�˼򻯶�����󶨹���,ʵ����make_logger_confile/make_logger_file/make_logger_con����,��һ���Թ���ȫ��ʹ�õ���־��¼�����������.

	//��־�ռ����ӿ�,����־��¼��ʹ��
	class logger_master_i;

	//logger_t�ĳ������ʾ��:
		logger.warn("test p1=%d p2=%s",2,"p2");				//������Ϣ��ʽ�����

	//logger_t�ĸ߼�����ʾ��:
		logger(LL_WRN).fmt("test p1=%d p2=%s",2,"p2").end();//����ͬ��,��ʽ����,�����Ҫ�ֶ�����end()
		logger(LL_WRN,2)("test p1=%d p2=%s",2,"p2").end();	//ͬ��,����������ȷ��tag
		logger(LL_ERR) << "logger stream output." << NULL;	//��ʽ��Ϣ���,�����������ָ�����end()����
		logger(LL_INF,4).hex(data,size).end();				//�����ݽ���ʮ�����ƴ���ʽ���,��������ȷ��tag
		logger(LL_WRN).bin(data,size).end();				//�����ݽ��ж�����ԭʼ���

	//�ӿڵ���ʱ�����Դ����û�tag,�����ڵ�ǰģ���м���ϸ��������ܵ�,Ҳ���ڽ�����־���˴���.
*/

namespace rx
{
	//-----------------------------------------------------
	//��־��Ϣ����
	typedef enum
	{
		LL_ERR = 5,											//������Ϣ
		LL_WRN = 4,											//������Ϣ
		LL_INF = 3,											//������Ϣ
		LL_CYC = 2,											//������Ϣ
		LL_DBG = 1,											//������Ϣ
		LL_NON = 0,											//��ֹ���
	}logger_level_t;

	//������־��Ϣ���ͻ�ȡ��Ӧ�����ִ�
	inline const char* logger_level_name(logger_level_t type)
	{
		static const char* NAMES[] = { "","DBG","CYC","INF","WRN","ERR" };
		return type <= LL_ERR ? NAMES[type] : "";
	}

	//�﷨��,����־��¼���ӿڰ�ģ������
	#define logger_modname(logger) logger.bind(rx_src_filename,__LINE__)

	//-----------------------------------------------------
	//��־�ռ����ӿ�,��¼��logger_t��Ҫ�󶨶�Ӧ���ռ���.
	class logger_master_i
	{
		friend class logger_t;
	protected:
		virtual bool on_level(logger_level_t type) = 0;
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
		logger_master_i	   *m_master;						//��־��¼�ӿڵĵײ�ת���ӿ�ָ��,ָ����־�ռ���
		uint32_t			m_last_seq;						//������־���
		char				m_mod_name[32];					//��־��¼�������Ĺ���ģ��

		//-------------------------------------------------
		//�ڲ�ʹ�õ���־��д��
		class recoder_t
		{
			logger_t	   &m_owner;
			uint64_t        m_last_txn;                     //������־�����,this<<32|seq,���ÿ�������Ψһ����
			bool			m_level_drop;					//ָʾ���������Ƿ��Ѿ�����־�����˵���.
		public:
			//---------------------------------------------
			recoder_t(logger_t& owner) :m_owner(owner), m_last_txn(0), m_level_drop(true) {  }
			//---------------------------------------------
			//��鵱ǰ��־�����Ƿ���Ҫ������.
			bool check_level(logger_level_t type)
			{
				if (m_owner.m_master == NULL)
					m_level_drop = true;
				else
					m_level_drop = !m_owner.m_master->on_level(type);
				return m_level_drop;
			}
			//---------------------------------------------
			//�����־��ǰ�˹̶�����:��־���type;�û����tag;
			recoder_t& begin(logger_level_t type = LL_INF, uint32_t tag = -1)
			{
				rx_assert_msg(m_last_txn == 0, "must call end()");//Ҫ��ǰһ�����������end()�������뱻����

				if (check_level(type))
					return *this;

				m_last_txn = (size_t)&m_owner << 32 | ++m_owner.m_last_seq;//���챾���������
				m_owner.m_master->on_begin(type, tag, m_last_txn, m_owner.m_mod_name);

				return *this;
			}
			//---------------------------------------------
			//��ʽ�������־�ľ������ݲ���
			recoder_t& vfmt(const char* fmt, va_list ap)
			{
				if (!m_level_drop)
					m_owner.m_master->on_vfmt(fmt, ap, m_last_txn);
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
				if (!m_level_drop)
					m_owner.m_master->on_hex(data, size, pre_tab, line_bytes, m_last_txn);
				return *this;
			}
			//---------------------------------------------
			//��ԭʼ�����Ʒ�ʽ������ݲ���
			recoder_t& bin(const void* data, uint32_t size)
			{
				if (!m_level_drop)
					m_owner.m_master->on_bin(data, size, m_last_txn);
				return *this;
			}
			//---------------------------------------------
			//��֪������־���������
			void end()
			{
				if (!m_level_drop)
					m_owner.m_master->on_end(m_last_txn);
				m_last_txn = 0;
			}
			//---------------------------------------------
			//�ڵ���begin����Խ��ж����ʽ���,ֱ����ָ�����,���ֶ�����end().
			recoder_t& operator<<(const char* msg)
			{
				if (msg == NULL)
					end();
				else
					fmt(msg);

				return *this;
			}
		};

	public:
		//-------------------------------------------------
		logger_t() :m_master(NULL), m_last_seq(0), recoder(*this) { m_mod_name[0] = 0; }
		logger_t(logger_master_i &log) :m_master(NULL), m_last_seq(0), recoder(*this) { bind(log); }
		//-------------------------------------------------
		//����־�ռ���
		void bind(logger_master_i &log) { m_master = &log; }
		void bind(logger_t &log) { m_master = log.m_master; }
		logger_master_i* master() { return m_master; }
		//-------------------------------------------------
		//��ģ������(�����Ǹ��Ի���,��ͬ����־��¼��ʵ�������в�ͬ��ģ������)
		void bind(const char* name, uint32_t lno = 0)
		{
			if (lno)
				st::snprintf(m_mod_name, sizeof(m_mod_name), "%s:%u", name, lno);
			else
				st::strcpy(m_mod_name, sizeof(m_mod_name), name);
		}
		//-------------------------------------------------
		//�ڲ���д��,ʹ�ø����
		recoder_t recoder;
		//�﷨��,��Ϊ��¼��ʼrecoder.begin()ʹ��,�����ʽ����ܷ���.��: logger(LL_ERR) << "logger stream output." << NULL;
		recoder_t& operator()(logger_level_t type = LL_INF, uint32_t tag = -1) { return recoder.begin(type, tag); }
		//-------------------------------------------------
		//��������������ڲ�ͳһʵ��
		void print(logger_level_t type, uint32_t tag, const char* fmt, va_list ap)
		{//��Ҳ����־��д������ȷ�÷�ʾ��,������begin(),�����end()
			if (recoder.check_level(type))
				return;
			recoder.begin(type, tag).vfmt(fmt, ap).end();
		}
		//-------------------------------------------------
		//���������Ϣ
		void error(uint32_t tag, const char* fmt, ...)
		{
			va_list ap;
			va_start(ap, fmt);
			print(LL_ERR, tag, fmt, ap);
			va_end(ap);
		}
		void error(const char* fmt, ...)
		{
			va_list ap;
			va_start(ap, fmt);
			print(LL_ERR, -1, fmt, ap);
			va_end(ap);
		}
		//-------------------------------------------------
		//���������Ϣ
		void warn(uint32_t tag, const char* fmt, ...)
		{
			va_list ap;
			va_start(ap, fmt);
			print(LL_WRN, tag, fmt, ap);
			va_end(ap);
		}
		void warn(const char* fmt, ...)
		{
			va_list ap;
			va_start(ap, fmt);
			print(LL_WRN, -1, fmt, ap);
			va_end(ap);
		}
		//-------------------------------------------------
		//�����ͨ��Ϣ
		void info(uint32_t tag, const char* fmt, ...)
		{
			va_list ap;
			va_start(ap, fmt);
			print(LL_INF, tag, fmt, ap);
			va_end(ap);
		}
		void info(const char* fmt, ...)
		{
			va_list ap;
			va_start(ap, fmt);
			print(LL_INF, -1, fmt, ap);
			va_end(ap);
		}
		//-------------------------------------------------
		//�����������Ϣ
		void cycle(uint32_t tag, const char* fmt, ...)
		{
			va_list ap;
			va_start(ap, fmt);
			print(LL_CYC, tag, fmt, ap);
			va_end(ap);
		}
		void cycle(const char* fmt, ...)
		{
			va_list ap;
			va_start(ap, fmt);
			print(LL_CYC, -1, fmt, ap);
			va_end(ap);
		}
		//-------------------------------------------------
		//���������Ϣ
		void debug(uint32_t tag, const char* fmt, ...)
		{
			va_list ap;
			va_start(ap, fmt);
			print(LL_DBG, tag, fmt, ap);
			va_end(ap);
		}
		void debug(const char* fmt, ...)
		{
			va_list ap;
			va_start(ap, fmt);
			print(LL_DBG, -1, fmt, ap);
			va_end(ap);
		}
	};
}


#endif
