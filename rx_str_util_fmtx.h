#ifndef _RX_STR_UTIL_FMTX_H_
#define _RX_STR_UTIL_FMTX_H_

#include <stdio.h>
#include "rx_str_util_fmt.h"
#include "rx_ct_delegate.h"

#if (RX_OS==RX_IS_OS_WIN||RX_OS_POSIX)&&!defined(RX_STR_USE_FILE)
#define RX_STR_USE_FILE 1
#endif

namespace rx
{
	//-----------------------------------------------------
	//fmt格式化需要的底层输出器,进行缓冲累计后再输出
	template<class CT, uint32_t maxsize>
	class fmt_writer_buffx :public fmt_imp::fmt_writer_base<CT>
	{
		typedef fmt_imp::fmt_writer_base<CT> super_t;
	protected:
		CT	m_buffer[maxsize];
	public:
		void operator ()(char character)
		{
			m_buffer[super_t::idx++] = character;
			++super_t::count;
			if (super_t::idx >= maxsize)
			{
				on_write(m_buffer, maxsize);
				super_t::idx = 0;
			}
		}
		void end()
		{
			if (!super_t::idx) return;
			on_write(m_buffer, (uint32_t)super_t::idx);
			super_t::idx = 0;
		}
		virtual void on_write(const CT* buf, uint32_t datalen) {}
	};

	//-------------------------------------------------
	//fmt格式化需要的底层输出器,字符累积后调用输出委托对象
	template<class CT>
	class fmt_writer_cb :public fmt_writer_buffx<CT, 512>
	{
	public:
		typedef delegate2_t<const void*, size_t, bool> delegate_t;
		uint32_t fails;
	protected:
		void on_write(const CT* buf, uint32_t datalen) 
		{ 
			if (!m_cb(buf, datalen))
				fails += 1;
		}
		delegate_t		&m_cb;
	public:
		fmt_writer_cb(delegate_t &cb) :fails(0),m_cb(cb) {}
	};

	//-----------------------------------------------------
	#if RX_STR_USE_FILE
	//底层字符输出器,buff,记录字符到缓冲区
	template<class CT>
	class fmt_writer_file :public fmt_imp::fmt_writer_base<CT>
	{
		FILE *m_fp;
		typedef fmt_imp::fmt_writer_base<CT> super_t;
	public:
		fmt_writer_file(FILE *fp) :m_fp(fp) {}
		void operator ()(CT character)
		{
			if (sizeof(CT) == sizeof(char))
				fputc(character, m_fp);
			else
				fputwc(character, m_fp);
			++super_t::count;
		}
		void end() {}
	};
	#endif
}

#endif