#ifndef _RX_DTL_BUFF_H_
#define _RX_DTL_BUFF_H_
#include "rx_cc_macro.h"
#include "rx_assert.h"
#include "rx_mem_alloc.h"

/*
	//静态缓冲区,可按数据类型T访问
	template<typename T,uint32_t size>
	class buff_ft;

	//动态缓冲区
	class buff_t;

*/

namespace rx
{
	//-----------------------------------------------------
	//基础的缓冲区,按字节访问
	class buff_i
	{
	protected:
		uint8_t    *m_ptr;                                  //缓冲区指针
		uint32_t    m_capacity;                             //最大容量
		uint32_t    m_size;                                 //当前数据长度
	public:
		//-------------------------------------------------
		//最大容量,字节
		uint32_t capacity() const { return m_capacity; }

		//最大容量,元素个数
		template<typename T>
		uint32_t capacity() const { return m_capacity / sizeof(T); }
		//-------------------------------------------------
		//已经填充数据长度,字节
		uint32_t size() const { return m_size; }

		//数据长度,元素个数
		template<typename T>
		uint32_t size() const { return m_size / sizeof(T); }
		//-------------------------------------------------
		//访问缓冲区,按字节偏移
		uint8_t* ptr(uint32_t idx = 0)const { rx_assert(idx < m_capacity); return m_ptr + idx; }

		//访问缓冲区,按元素偏移
		template<typename T>
		T* ptr(uint32_t idx = 0)const { rx_assert_if(idx, idx * sizeof(T) < m_capacity); return (T*)m_ptr + idx; }
		//-------------------------------------------------
		//访问缓冲区,按元素偏移
		template<typename T>
		T& at(uint32_t idx = 0) const { rx_assert(idx * sizeof(T) < m_capacity); return ((T*)m_ptr)[idx]; }
		//-------------------------------------------------
		//数据拷贝
		void set(const uint8_t &dat) { memset(m_ptr, dat, m_capacity); m_size = m_capacity; }
		bool set(const uint8_t *dat, uint32_t size)
		{
			if (size > m_capacity)
				return false;
			memcpy(m_ptr, dat, size);
			m_size = size;
			return true;
		}
		//-------------------------------------------------
		//拼接数据
		bool cat(const uint8_t *dat, uint32_t size)
		{
			if (dat == NULL || size == 0)
				return false;
			if (m_size + size > m_capacity)
				return false;
			memcpy(m_ptr + m_size, dat, size);
			m_size += size;
			return true;
		}
		buff_i& operator()(const uint8_t *dat, uint32_t size)
		{
			cat(dat, size);
			return *this;
		}
		buff_i& operator<<(const buff_i& buff)
		{
			cat(buff.m_ptr, buff.m_size);
			return *this;
		}
	protected:
		//构造与析构函数
		buff_i(uint8_t* arr, uint32_t cap, uint32_t size = 0) :m_ptr(arr), m_capacity(cap), m_size(size) {}
		virtual ~buff_i() {}
	};

	//-----------------------------------------------------
	//静态缓冲区,可按数据类型T访问
	template<typename T, uint32_t max_size>
	class buff_ft :public buff_i
	{
		typedef buff_i super_t;
		T       m_items[max_size];
	public:
		buff_ft() :super_t((uint8_t*)m_items, sizeof(T)*max_size) {}
	};

	//-----------------------------------------------------
	//动态缓冲区
	class buff_t :public buff_i
	{
		typedef buff_i super_t;
		mem_allotter_i &m_mem;
	public:
		//-------------------------------------------------
		buff_t() :super_t(NULL, 0), m_mem(rx_global_mem_allotter()) {}
		buff_t(mem_allotter_i &m) :super_t(NULL, 0), m_mem(m) {}
		virtual ~buff_t() { clear(true); }
		//-------------------------------------------------
		//按指定数据类型分配缓冲区
		template<typename T>
		bool make(uint32_t Count)
		{
			rx_assert_msg(Count != 0, "不能初始化0长度的数组");
			if (!Count) return false;

			if (Count * sizeof(T) > super_t::m_capacity)
			{//要求的容量大于原有容量,则重新分配
				clear(true);

				super_t::m_ptr = (uint8_t*)m_mem.alloc(sizeof(T)*Count);
				if (super_t::m_ptr == NULL)
					return false;
				super_t::m_capacity = Count * sizeof(T);
				return true;
			}
			else
			{//否则仅重置数据长度
				super_t::m_size = 0;
				return true;
			}
		}
		//按字节分配缓冲区
		bool make(uint32_t Count) { return make<uint8_t>(Count); }
		//-------------------------------------------------
		//重置缓冲区,或完全清除
		inline void clear(bool force = false)
		{
			if (super_t::m_ptr&&force)
			{//强制要求释放内存的时候,才完全清除
				m_mem.free(super_t::m_ptr);
				super_t::m_ptr = NULL;
				super_t::m_capacity = 0;
			}
			//重置数据长度
			super_t::m_size = 0;
		}
		//-------------------------------------------------
		//数据复制
		buff_t& operator=(const super_t& S)
		{
			if (!make<uint8_t>(S.capacity()))               //判断容量不足则重新分配
				return *this;                               //内存不足直接返回

			super_t::set(S.ptr(), S.size());
			return *this;
		}
		//-------------------------------------------------
	};
}
#endif
