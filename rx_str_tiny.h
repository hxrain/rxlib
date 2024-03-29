#ifndef _RX_STR_TINY_H_
#define _RX_STR_TINY_H_

#include "rx_cc_macro.h"
#include "rx_cc_base.h"
#include "rx_str_util_std.h"
#include "rx_str_util_ext.h"
#include "rx_str_util_fmt.h"

namespace rx
{
	#pragma pack(push,1)
	//-----------------------------------------------------
	//封装简易的字符串功能,用于dtl容器内部的临时key字符串存储和简单的字符串拼装,容量不能超过uint16_t的范围
	//max_str_size==0为绑定外部缓冲区模式;否则为内置缓冲区模式
	template<class CT = char, uint16_t max_str_size = 0>
	struct tiny_string_t
	{
	protected:
		//-------------------------------------------------
		//缓冲区内置的串模式
		template<class ct, uint16_t buff_size>
		class head_t
		{
		public:
			head_t() :length(0) { buff[0] = 0; }
			//内置缓冲区,无需绑定.仅用于接口兼容.
			void bind(ct* buff, uint16_t size) { rx_alert("don't call this method."); }
			//获取内置缓冲区的容量
			const uint16_t capacity()const { return buff_size; }

			uint16_t    length;                             //记录buff的实际长度.
			ct          buff[buff_size];                    //字符串缓冲器指针.
		};

		//-------------------------------------------------
		//特化,使用外置绑定的缓冲区
		template<class ct>
		class head_t<ct, 0>
		{
		public:
			head_t() :max_size(0), length(0), buff(NULL) {}
			//绑定缓冲区指针与容量
			void bind(ct* buf, uint16_t size) { rx_assert(size < 0xFFFF); buff = buf; max_size = size; length = 0; }
			//获知绑定的缓冲区的容量
			const uint16_t capacity()const { return max_size; }

			uint16_t    max_size;                           //必须告知buff的可用容量
			uint16_t    length;                             //记录buff的实际长度.
			ct         *buff;                               //字符串缓冲器指针.
		};

		//-------------------------------------------------
		head_t<CT, max_str_size>  m_head;                   //真正使用的小串缓冲区对象
	private:
		tiny_string_t(const tiny_string_t&);
	public:
		tiny_string_t() {}
		//绑定缓冲区,并告知缓冲区内部已有内容偏移
		tiny_string_t(uint32_t cap, CT* buff, uint32_t init_offset = 0) { m_head.bind(buff, cap); m_head.length = init_offset; }
		//对于内置缓冲区模式,可以直接进行串赋值或格式化生成
		tiny_string_t(const CT* fmt, ...)
		{
			rx_static_assert(max_str_size != 0);            //要求必须是内置缓冲区模式,才可以直接赋值
			va_list	ap;
			va_start(ap, fmt);
			vfmt(fmt, ap);
			va_end(ap);
		}
		//-------------------------------------------------
		//获知缓冲区容量
		uint32_t capacity()const { return m_head.capacity(); }
		//获知缓冲区内数据长度,如果等于容量,则说明出现了拼装溢出错误
		uint32_t size()const { return m_head.length; }
		//获知缓冲区内容
		const CT* c_str() const { return m_head.buff; }
		operator const CT* ()const { return m_head.buff; }
		//-------------------------------------------------
		//开放对外的缓冲区访问方法
		CT* ptr() { return m_head.buff; }
		bool end(uint32_t pos)
		{
			if (pos >= m_head.capacity())
				return false;
			m_head.buff[pos] = 0;
			m_head.length = pos;
			return true;
		}
		//-------------------------------------------------
		//比较运算符重载
		bool operator <  (const tiny_string_t& str) const { return st::strcmp(m_head.buff, str.m_head.buff) < 0; }
		bool operator <= (const tiny_string_t& str) const { return st::strcmp(m_head.buff, str.m_head.buff) <= 0; }
		bool operator == (const tiny_string_t& str) const { return st::strcmp(m_head.buff, str.m_head.buff) == 0; }
		bool operator >  (const tiny_string_t& str) const { return st::strcmp(m_head.buff, str.m_head.buff) > 0; }
		bool operator >= (const tiny_string_t& str) const { return st::strcmp(m_head.buff, str.m_head.buff) >= 0; }
		bool operator != (const tiny_string_t& str) const { return st::strcmp(m_head.buff, str.m_head.buff) != 0; }
		//-------------------------------------------------
		bool operator <  (const CT *str) const { return st::strcmp(m_head.buff, (is_empty(str) ? sc<CT>::empty() : str)) < 0; }
		bool operator <= (const CT *str) const { return st::strcmp(m_head.buff, (is_empty(str) ? sc<CT>::empty() : str)) <= 0; }
		bool operator == (const CT *str) const { return st::strcmp(m_head.buff, (is_empty(str) ? sc<CT>::empty() : str)) == 0; }
		bool operator >  (const CT *str) const { return st::strcmp(m_head.buff, (is_empty(str) ? sc<CT>::empty() : str)) > 0; }
		bool operator >= (const CT *str) const { return st::strcmp(m_head.buff, (is_empty(str) ? sc<CT>::empty() : str)) >= 0; }
		bool operator != (const CT *str) const { return st::strcmp(m_head.buff, (is_empty(str) ? sc<CT>::empty() : str)) != 0; }
		//-------------------------------------------------
		//绑定缓冲器并进行字符串的赋值
		uint32_t bind(CT* buff, uint32_t cap, const CT* str, uint32_t len = 0)
		{
			rx_static_assert(max_str_size == 0);            //要求必须为缓冲区绑定模式,才可以重新绑定
			m_head.bind(buff, cap);
			return assign(str, len);
		}
		//-------------------------------------------------
		//在给定的buff内存块上构造简易串对象并进行初始化
		//返回值:NULL失败;其他为串头对象指针
		static tiny_string_t* make(void* buff, uint32_t buffsize, const CT* str, uint32_t len = 0)
		{
			if (buffsize <= sizeof(tiny_string_t))			//检查最小尺寸
				return NULL;

			//在给定的缓冲区上构造简易字符串对象
			uint32_t cap = buffsize - sizeof(tiny_string_t);
			CT *strbuf = (CT*)((uint8_t*)buff + sizeof(tiny_string_t));
			tiny_string_t *s = ct::OC<tiny_string_t >((tiny_string_t*)buff, cap, strbuf);
			s->assign(str, len);
			return s;
		}
		//-------------------------------------------------
		//使用给定的字符串进行赋值(能放多少放多少)
		//返回值:真正拷贝的串尺寸.
		uint32_t assign(const CT* str = NULL, uint32_t len = 0)
		{
			if (is_empty(str) || m_head.capacity() == 1)
			{
				m_head.length = 0;
				m_head.buff[0] = 0;
				return 0;
			}
			else
			{
				//进行字符串的拷贝
				if (!len)
					len = (uint32_t)st::strlen(str);
				m_head.length = Min(len, uint32_t(m_head.capacity() - 1));
				st::strncpy(m_head.buff, str, m_head.length);
				m_head.buff[m_head.length] = 0;
				return m_head.length;
			}
		}
		//-------------------------------------------------
		tiny_string_t& replace(CT from, CT to)
		{
			st::replace(m_head.buff, from, to);
			return *this;
		}
		//-------------------------------------------------
		//格式化生成内部串
		bool vfmt(const CT *str, va_list arg)
		{
			m_head.length = 0;
			m_head.buff[0] = 0;

			int rc = st::vsnprintf(m_head.buff, m_head.capacity(), str, arg);
			if (rc < 0 || rc >= m_head.capacity())
				return false;
			m_head.length = rc;
			return true;
		}
		bool fmt(const CT *str, ...)
		{
			va_list	ap;
			va_start(ap, str);
			bool ret = vfmt(str, ap);
			va_end(ap);
			return ret;
		}
		//-------------------------------------------------
		//拼装字符,最后需要检查size()是否等于capacity(),相等则代表出现了缓冲区不足的错误.
		tiny_string_t& operator<<(const CT c)
		{
			if (m_head.length < m_head.capacity() - 1)
			{
				m_head.buff[m_head.length++] = c;
				m_head.buff[m_head.length] = 0;
			}
			else
				m_head.length = m_head.capacity();          //容量不足,标记错误

			return *this;
		}
		//-------------------------------------------------
		//拼装字符串,最后需要检查size()是否等于capacity(),相等则代表出现了缓冲区不足的错误.
		tiny_string_t& operator<<(const CT *str) { return (*this)(st::strlen(str), str); }
		//-------------------------------------------------
		//拼装定长字符串,最后需要检查size()是否等于capacity(),相等则代表出现了缓冲区不足的错误.
		tiny_string_t& operator()(uint32_t len, const CT *str)
		{
			if (len == 0)
				return *this;
			//尝试在剩余的空间中放入指定字符串
			uint32_t rc = st::strcpy(m_head.buff + m_head.length, m_head.capacity() - m_head.length, str, len);
			if (rc)
				m_head.length += rc;
			else
				m_head.length = m_head.capacity();          //容量不足,标记错误
			return *this;
		}
		//-------------------------------------------------
		//格式化拼装,可连续调用,避免出现超长的format格式化串与参数列表;最后需要检查size()是否等于capacity(),相等则代表出现了缓冲区不足的错误.
		tiny_string_t& operator()(const CT *str, ...)
		{
			va_list ap;
			va_start(ap, str);
			cat(str, ap);
			va_end(ap);
			return *this;
		}
		//-------------------------------------------------
		//格式化拼装,可连续调用,避免出现超长的format格式化串与参数列表;最后需要检查size()是否等于capacity(),相等则代表出现了缓冲区不足的错误.
		tiny_string_t& cat(const CT *str, va_list ap)
		{
			if (is_empty(str))
				return *this;

			//尝试在剩余的空间中放入指定字符串
			int remain = m_head.capacity() - m_head.length;
			if (remain > 0)
			{
				int len = st::vsnprintf(m_head.buff + m_head.length, remain, str, ap);
				if (len <= remain)
					m_head.length += len;                   //拼装成功,缓冲区数据长度增加
				else
					m_head.length = m_head.capacity();      //容量不足,标记错误
			}
			return *this;
		}
		//-------------------------------------------------
		//字符串拷贝赋值,赋值溢出的时候,保留最大长度的值
		tiny_string_t& operator=(const CT *str) { assign(str); return *this; }
	};
	#pragma pack(pop)
	//绑定外部缓冲区的拼装器
	typedef tiny_string_t<char>        cat_t;
	typedef tiny_string_t<wchar_t>     wcat_t;
	rx_static_assert(sizeof(cat_t) == 4 + sizeof(size_t));

	//固定缓冲区的微型串
	typedef tiny_string_t<char, 14>     str14_t;
	typedef tiny_string_t<wchar_t, 12>  wstr14_t;
	rx_static_assert(sizeof(str14_t) == 16);

	typedef tiny_string_t<char, 30>     str30_t;
	typedef tiny_string_t<wchar_t, 30>  wstr30_t;
	rx_static_assert(sizeof(str30_t) == 32);

	typedef tiny_string_t<char, 62>     str62_t;
	typedef tiny_string_t<wchar_t, 62>  wstr62_t;
	rx_static_assert(sizeof(str62_t) == 64);

	//固定缓冲区的小型串尺寸定义
	#ifndef RX_XSTR_SIZE
		#define RX_XSTR_SIZE 510
	#endif
	//固定缓冲区的小型串型别定义
	typedef tiny_string_t<char, RX_XSTR_SIZE>    xstr;
	typedef tiny_string_t<wchar_t, RX_XSTR_SIZE> xwstr;
	rx_static_assert(sizeof(xstr) == RX_XSTR_SIZE+2);
}


#endif
