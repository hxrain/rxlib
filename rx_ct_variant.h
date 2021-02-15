#ifndef _RX_CT_VARIANT_H_
#define _RX_CT_VARIANT_H_
//reference https://github.com/martinmoene/variant-lite

#include "rx_cc_macro.h"
#include "rx_ct_traits.h"
#include "rx_ct_typelist.h"
#include "rx_ct_util.h"
#include "rx_assert.h"

namespace rx {
	//未知的型别列表索引顺序
	static const size_t bad_type_index = static_cast<size_t>(-1);

	//可变量功能类原型定义
	template< class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 >
	class variant_lite_t;

	//-----------------------------------------------------
	// 获取具体可变量指定型别顺序对应的真实型别
	template< size_t K, class T >
	struct variant_alt;

	template< size_t K, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 >
	struct variant_alt< K, variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> >
	{
		typedef typename ct_typelist_at<ct_typelist_make16(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15), K>::type type;
	};


	//-----------------------------------------------------
	//variant可变量功能定义
	//-----------------------------------------------------
	template<class T0,
		class T1 = ct_dummy_T1, class T2 = ct_dummy_T2, class T3 = ct_dummy_T3, class T4 = ct_dummy_T4, class T5 = ct_dummy_T5,
		class T6 = ct_dummy_T6, class T7 = ct_dummy_T7, class T8 = ct_dummy_T8, class T9 = ct_dummy_T9, class T10 = ct_dummy_T10,
		class T11 = ct_dummy_T11, class T12 = ct_dummy_T12, class T13 = ct_dummy_T13, class T14 = ct_dummy_T14, class T15 = ct_dummy_T15 >
		class variant_lite_t
	{
		//定义型别列表类型
		typedef ct_typelist_make16(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15) variant_types;

		// 估算全部型别列表中占用空间的最大值
		enum { data_size = ct_typelist_max< variant_types >::value };
		//最终定义数据缓存空间对象
		uint8_t			m_buff[data_size];
		//定义型别索引类型
		size_t			m_type_index;

		//获取内部缓冲区指针
		void * m_ptr() { return &m_buff; }
		void const * m_ptr() const { return &m_buff; }

		//强制进行数据缓冲区指针的型别转换
		template< class U >
		static U * as(void * data) { return reinterpret_cast<U*>(data); }
		template< class U >
		static U const * as(void const * data) { return reinterpret_cast<const U*>(data); }

	public:
		//-----------------------------------------------------
		// 默认构造函数,标记型别或值为无效状态
		variant_lite_t() : m_type_index(bad_type_index) {}

		//-----------------------------------------------------
		// 构造函数,使用指定的类型进行内部缓冲区的就地构造初始化
		variant_lite_t(T0 const & v) : m_type_index(0) { ct::OC((T0*)m_ptr(), v); }
		variant_lite_t(T1 const & v) : m_type_index(1) { ct::OC((T1*)m_ptr(), v); }
		variant_lite_t(T2 const & v) : m_type_index(2) { ct::OC((T2*)m_ptr(), v); }
		variant_lite_t(T3 const & v) : m_type_index(3) { ct::OC((T3*)m_ptr(), v); }
		variant_lite_t(T4 const & v) : m_type_index(4) { ct::OC((T4*)m_ptr(), v); }
		variant_lite_t(T5 const & v) : m_type_index(5) { ct::OC((T5*)m_ptr(), v); }
		variant_lite_t(T6 const & v) : m_type_index(6) { ct::OC((T6*)m_ptr(), v); }
		variant_lite_t(T7 const & v) : m_type_index(7) { ct::OC((T7*)m_ptr(), v); }
		variant_lite_t(T8 const & v) : m_type_index(8) { ct::OC((T8*)m_ptr(), v); }
		variant_lite_t(T9 const & v) : m_type_index(9) { ct::OC((T9*)m_ptr(), v); }
		variant_lite_t(T10 const & v) : m_type_index(10) { ct::OC((T10*)m_ptr(), v); }
		variant_lite_t(T11 const & v) : m_type_index(11) { ct::OC((T11*)m_ptr(), v); }
		variant_lite_t(T12 const & v) : m_type_index(12) { ct::OC((T12*)m_ptr(), v); }
		variant_lite_t(T13 const & v) : m_type_index(13) { ct::OC((T13*)m_ptr(), v); }
		variant_lite_t(T14 const & v) : m_type_index(14) { ct::OC((T14*)m_ptr(), v); }
		variant_lite_t(T15 const & v) : m_type_index(15) { ct::OC((T15*)m_ptr(), v); }

		//-----------------------------------------------------
		//使用其他variant_lite_t对象进行拷贝构造
		variant_lite_t(variant_lite_t const & other) : m_type_index(other.m_type_index)
		{
			(void)m_copy_construct(other.m_type_index, other.m_ptr(), m_ptr());
		}

		//-----------------------------------------------------
		//析构函数(调用助手类完成)
		~variant_lite_t()
		{
			if (is_valid())
				m_destroy(m_type_index, m_ptr());
		}
		//-----------------------------------------------------
		// 使用其他对象进行拷贝赋值
		variant_lite_t & operator=(variant_lite_t const & other) { return m_copy_assign(other); }
		//-----------------------------------------------------
		// 使用其他型别的值进行赋值,并根据型别设定对应的型别索引
		variant_lite_t & operator=(T0 const & t0) { return m_assign_value<0>(t0); }
		variant_lite_t & operator=(T1 const & t1) { return m_assign_value<1>(t1); }
		variant_lite_t & operator=(T2 const & t2) { return m_assign_value<2>(t2); }
		variant_lite_t & operator=(T3 const & t3) { return m_assign_value<3>(t3); }
		variant_lite_t & operator=(T4 const & t4) { return m_assign_value<4>(t4); }
		variant_lite_t & operator=(T5 const & t5) { return m_assign_value<5>(t5); }
		variant_lite_t & operator=(T6 const & t6) { return m_assign_value<6>(t6); }
		variant_lite_t & operator=(T7 const & t7) { return m_assign_value<7>(t7); }
		variant_lite_t & operator=(T8 const & t8) { return m_assign_value<8>(t8); }
		variant_lite_t & operator=(T9 const & t9) { return m_assign_value<9>(t9); }
		variant_lite_t & operator=(T10 const & t10) { return m_assign_value<10>(t10); }
		variant_lite_t & operator=(T11 const & t11) { return m_assign_value<11>(t11); }
		variant_lite_t & operator=(T12 const & t12) { return m_assign_value<12>(t12); }
		variant_lite_t & operator=(T13 const & t13) { return m_assign_value<13>(t13); }
		variant_lite_t & operator=(T14 const & t14) { return m_assign_value<14>(t14); }
		variant_lite_t & operator=(T15 const & t15) { return m_assign_value<15>(t15); }
		//-----------------------------------------------------
		//获取当前的型别索引
		size_t index() const { return m_type_index; }
		//-----------------------------------------------------
		//判断当前的可变量是否有效
		bool is_valid() const { return m_type_index != bad_type_index; }
		//-----------------------------------------------------
		//将当前的可变量对象与其他的对象进行值交换
		void swap(variant_lite_t & other)
		{
			if (!is_valid() && !other.is_valid())
			{
				// no effect
			}
			else if (m_type_index == other.m_type_index)
			{
				m_swap_value(m_type_index, other);
			}
			else
			{
				variant_lite_t tmp(*this);
				*this = other;
				other = tmp;
			}
		}
		//-----------------------------------------------------
		//获取指定的类型T在当前型别列表中的索引顺序,也可以判断是否含有指定型别.
		//返回值:bad_type_index失败,可变量不含有指定型别;其他为找到的型别索引.
		template< class T >
		static  size_t index_of()
		{
			return ct_typelist_of<variant_types, typename ct_remove_cv<T>::type >::value;
		}
		//-----------------------------------------------------
		//获取内部缓冲区的内容为指定类型的值(要求指定类型是正确的当前型别)
		template< class T >
		T &
			get()
		{
			rx_assert(index_of<T>() == index()); //进行目标类型的型别检查
			return *as<T>(m_ptr()); //将内部缓冲区指针提领转换为指定的目标类型
		}

		template< class T >
		T const &
			get() const
		{
			rx_assert(index_of<T>() == index());
			return *as<const T>(m_ptr());
		}
		//-----------------------------------------------------
		// 按指定型别索引获取可变量的值
		template< size_t K >
		typename variant_alt< K, variant_lite_t >::type &
			get()
		{
			return get< typename ct_typelist_at< variant_types, K >::type >();
		}

		template< size_t K >
		typename variant_alt< K, variant_lite_t >::type const &
			get() const
		{
			return get< typename ct_typelist_at< variant_types, K >::type >();
		}

	private:
		//根据型别索引序号,析构对应型别数据
		static void m_destroy(size_t &index, void * data)
		{
			switch (index)
			{
			case bad_type_index:break;
			case 0: ct::OD(as<T0>(data)); break;
			case 1: ct::OD(as<T1>(data)); break;
			case 2: ct::OD(as<T2>(data)); break;
			case 3: ct::OD(as<T3>(data)); break;
			case 4: ct::OD(as<T4>(data)); break;
			case 5: ct::OD(as<T5>(data)); break;
			case 6: ct::OD(as<T6>(data)); break;
			case 7: ct::OD(as<T7>(data)); break;
			case 8: ct::OD(as<T8>(data)); break;
			case 9: ct::OD(as<T9>(data)); break;
			case 10: ct::OD(as<T10>(data)); break;
			case 11: ct::OD(as<T11>(data)); break;
			case 12: ct::OD(as<T12>(data)); break;
			case 13: ct::OD(as<T13>(data)); break;
			case 14: ct::OD(as<T14>(data)); break;
			case 15: ct::OD(as<T15>(data)); break;
			default:rx_assert(0);
			}
			index = bad_type_index;
		}

		//进行指定型别顺序的拷贝构造(在目标缓冲区中就地构造新对象,并使用源缓冲区数据进行赋值)
		static size_t m_copy_construct(size_t const from_index, const void * from_data, void * to_data)
		{
			switch (from_index)
			{
			case 0: ct::OC((T0*)to_data, *as<T0>(from_data)); break;
			case 1: ct::OC((T1*)to_data, *as<T1>(from_data)); break;
			case 2: ct::OC((T2*)to_data, *as<T2>(from_data)); break;
			case 3: ct::OC((T3*)to_data, *as<T3>(from_data)); break;
			case 4: ct::OC((T4*)to_data, *as<T4>(from_data)); break;
			case 5: ct::OC((T5*)to_data, *as<T5>(from_data)); break;
			case 6: ct::OC((T6*)to_data, *as<T6>(from_data)); break;
			case 7: ct::OC((T7*)to_data, *as<T7>(from_data)); break;
			case 8: ct::OC((T8*)to_data, *as<T8>(from_data)); break;
			case 9: ct::OC((T9*)to_data, *as<T9>(from_data)); break;
			case 10: ct::OC((T10*)to_data, *as<T10>(from_data)); break;
			case 11: ct::OC((T11*)to_data, *as<T11>(from_data)); break;
			case 12: ct::OC((T12*)to_data, *as<T12>(from_data)); break;
			case 13: ct::OC((T13*)to_data, *as<T13>(from_data)); break;
			case 14: ct::OC((T14*)to_data, *as<T14>(from_data)); break;
			case 15: ct::OC((T15*)to_data, *as<T15>(from_data)); break;
			default:rx_assert(0);
			}
			return from_index;
		}

		//进行指定型别顺序的拷贝赋值
		static size_t m_copy_assign(size_t const from_index, const void * from_data, void * to_data)
		{
			switch (from_index)
			{
			case 0: *as<T0>(to_data) = *as<T0>(from_data); break;
			case 1: *as<T1>(to_data) = *as<T1>(from_data); break;
			case 2: *as<T2>(to_data) = *as<T2>(from_data); break;
			case 3: *as<T3>(to_data) = *as<T3>(from_data); break;
			case 4: *as<T4>(to_data) = *as<T4>(from_data); break;
			case 5: *as<T5>(to_data) = *as<T5>(from_data); break;
			case 6: *as<T6>(to_data) = *as<T6>(from_data); break;
			case 7: *as<T7>(to_data) = *as<T7>(from_data); break;
			case 8: *as<T8>(to_data) = *as<T8>(from_data); break;
			case 9: *as<T9>(to_data) = *as<T9>(from_data); break;
			case 10: *as<T10>(to_data) = *as<T10>(from_data); break;
			case 11: *as<T11>(to_data) = *as<T11>(from_data); break;
			case 12: *as<T12>(to_data) = *as<T12>(from_data); break;
			case 13: *as<T13>(to_data) = *as<T13>(from_data); break;
			case 14: *as<T14>(to_data) = *as<T14>(from_data); break;
			case 15: *as<T15>(to_data) = *as<T15>(from_data); break;
			default:rx_assert(0);
			}
			return from_index;
		}
		//用另外的varaint对象对本对象赋值
		variant_lite_t & m_copy_assign(variant_lite_t const & other)
		{
			if (!is_valid() && !other.is_valid())
			{
				// no effect
			}
			else if (is_valid() && !other.is_valid())
			{
				m_destroy(m_type_index, m_ptr());
			}
			else if (index() == other.index())
			{
				m_type_index = m_copy_assign(other.m_type_index, other.m_ptr(), m_ptr());
			}
			else
			{
				m_destroy(m_type_index, m_ptr());
				m_type_index = m_copy_construct(other.m_type_index, other.m_ptr(), m_ptr());
			}
			return *this;
		}

		//用指定型别的值对本对象赋值
		template< size_t K, class T >
		variant_lite_t & m_assign_value(T const & value)
		{
			if (index() == K)
			{
				*as<T>(m_ptr()) = value;
			}
			else
			{
				m_destroy(m_type_index, m_ptr());
				ct::OC((T*)m_ptr(), value);
				m_type_index = K;
			}
			return *this;
		}

		//按指定型别索引,进行当前可变量对象与其他对象的值交换
		void m_swap_value(size_t index, variant_lite_t & other)
		{
			switch (index)
			{
			case 0: swap(this->get<0>(), other.get<0>()); break;
			case 1: swap(this->get<1>(), other.get<1>()); break;
			case 2: swap(this->get<2>(), other.get<2>()); break;
			case 3: swap(this->get<3>(), other.get<3>()); break;
			case 4: swap(this->get<4>(), other.get<4>()); break;
			case 5: swap(this->get<5>(), other.get<5>()); break;
			case 6: swap(this->get<6>(), other.get<6>()); break;
			case 7: swap(this->get<7>(), other.get<7>()); break;
			case 8: swap(this->get<8>(), other.get<8>()); break;
			case 9: swap(this->get<9>(), other.get<9>()); break;
			case 10: swap(this->get<10>(), other.get<10>()); break;
			case 11: swap(this->get<11>(), other.get<11>()); break;
			case 12: swap(this->get<12>(), other.get<12>()); break;
			case 13: swap(this->get<13>(), other.get<13>()); break;
			case 14: swap(this->get<14>(), other.get<14>()); break;
			case 15: swap(this->get<15>(), other.get<15>()); break;
			}
		}
	};

	//-----------------------------------------------------
	//判断指定型别的值是否可访问
	template< class T, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 >
	inline bool can(variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & v)
	{
	    typedef variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> vat_t;
		return v.index() == vat_t::template index_of<T>();
	}
	//-----------------------------------------------------
	//判断指定型别是否存在
	template< class T, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 >
	inline bool has(variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & v)
	{
	    typedef variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> vat_t;
		return bad_type_index != vat_t::template index_of<T>();
	}

	//-----------------------------------------------------
	//获取指定型别的值
	template< class R, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 >
	inline R & get(variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> & v)
	{
		return v.template get<R>();
	}

	template< class R, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 >
	inline R const & get(variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & v)
	{
		return v.template get<R>();
	}

	//-----------------------------------------------------
	//获取指定型别索引对应的值
	template< size_t K, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 >
	inline typename variant_alt< K, variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> >::type &
		get(variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> & v)
	{
		rx_assert(K == v.index());
		return v.template get<K>();
	}

	template< size_t K, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 >
	inline typename variant_alt< K, variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> >::type const &
		get(variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & v)
	{
		rx_assert(K == v.index());
		return v.template get<K>();
	}

	//-----------------------------------------------------
	//交换两个可变量的值
	template< class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15>
	inline void swap(variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> & a, variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> & b)
	{
		a.swap(b);
	}


	//进行可变量的关系运算比较器的定义
	namespace detail {

		template< class Variant >
		struct Comparator
		{
			//判断两个可变量的值是否相同
			static inline bool equal(Variant const & v, Variant const & w)
			{
				if (v.index() != w.index())
					return false;

				switch (v.index())
				{
				case 0: return get<0>(v) == get<0>(w);
				case 1: return get<1>(v) == get<1>(w);
				case 2: return get<2>(v) == get<2>(w);
				case 3: return get<3>(v) == get<3>(w);
				case 4: return get<4>(v) == get<4>(w);
				case 5: return get<5>(v) == get<5>(w);
				case 6: return get<6>(v) == get<6>(w);
				case 7: return get<7>(v) == get<7>(w);
				case 8: return get<8>(v) == get<8>(w);
				case 9: return get<9>(v) == get<9>(w);
				case 10: return get<10>(v) == get<10>(w);
				case 11: return get<11>(v) == get<11>(w);
				case 12: return get<12>(v) == get<12>(w);
				case 13: return get<13>(v) == get<13>(w);
				case 14: return get<14>(v) == get<14>(w);
				case 15: return get<15>(v) == get<15>(w);

				default: return false;
				}
			}

			//判断可变量a的值是否小于b的值
			static inline bool less_than(Variant const & v, Variant const & w)
			{
				if (v.index() != w.index())
					return false;

				switch (v.index())
				{
				case 0: return get<0>(v) < get<0>(w);
				case 1: return get<1>(v) < get<1>(w);
				case 2: return get<2>(v) < get<2>(w);
				case 3: return get<3>(v) < get<3>(w);
				case 4: return get<4>(v) < get<4>(w);
				case 5: return get<5>(v) < get<5>(w);
				case 6: return get<6>(v) < get<6>(w);
				case 7: return get<7>(v) < get<7>(w);
				case 8: return get<8>(v) < get<8>(w);
				case 9: return get<9>(v) < get<9>(w);
				case 10: return get<10>(v) < get<10>(w);
				case 11: return get<11>(v) < get<11>(w);
				case 12: return get<12>(v) < get<12>(w);
				case 13: return get<13>(v) < get<13>(w);
				case 14: return get<14>(v) < get<14>(w);
				case 15: return get<15>(v) < get<15>(w);

				default: return false;
				}
			}
		};
	} //namespace detail

	//-----------------------------------------------------
	//判断两个可变量的值是否相同
	template< class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 >
	inline bool operator==(
		variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & v,
		variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & w)
	{
		if (v.index() != w.index()) return false;
		else if (!v.is_valid()) return true;
		else return detail::Comparator< variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> >::equal(v, w);
	}
	//-----------------------------------------------------
	//判断两个可变量的值是否不同
	template< class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 >
	inline bool operator!=(
		variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & v,
		variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & w)
	{
		return !(v == w);
	}
	//-----------------------------------------------------
	//判断可变量v的值是否小于w
	template< class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 >
	inline bool operator<(
		variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & v,
		variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & w)
	{
		if (!w.is_valid()) return false;
		else if (!v.is_valid()) return true;
		else if (v.index() < w.index()) return true;
		else if (v.index() > w.index()) return false;
		else return detail::Comparator< variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> >::less_than(v, w);
	}
	//-----------------------------------------------------
	//判断可变量v的值是否大于w
	template< class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 >
	inline bool operator>(
		variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & v,
		variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & w)
	{
		return w < v;
	}
	//-----------------------------------------------------
	//判断可变量v的值是否小于等于w
	template< class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 >
	inline bool operator<=(
		variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & v,
		variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & w)
	{
		return !(v > w);
	}
	//-----------------------------------------------------
	//判断可变量v的值是否大于等于w
	template< class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 >
	inline bool operator>=(
		variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & v,
		variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & w)
	{
		return !(v < w);
	}

} // namespace rx

#endif
