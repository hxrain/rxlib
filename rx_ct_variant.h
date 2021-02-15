#ifndef _RX_CT_VARIANT_H_
#define _RX_CT_VARIANT_H_
//reference https://github.com/martinmoene/variant-lite

#include "rx_cc_macro.h"
#include "rx_ct_traits.h"
#include "rx_ct_typelist.h"
#include "rx_ct_util.h"
#include "rx_assert.h"

namespace rx {
	//δ֪���ͱ��б�����˳��
	static const size_t bad_type_index = static_cast<size_t>(-1);

	//�ɱ���������ԭ�Ͷ���
	template< class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 >
	class variant_lite_t;

	//-----------------------------------------------------
	// ��ȡ����ɱ���ָ���ͱ�˳���Ӧ����ʵ�ͱ�
	template< size_t K, class T >
	struct variant_alt;

	template< size_t K, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 >
	struct variant_alt< K, variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> >
	{
		typedef typename ct_typelist_at<ct_typelist_make16(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15), K>::type type;
	};


	//-----------------------------------------------------
	//variant�ɱ������ܶ���
	//-----------------------------------------------------
	template<class T0,
		class T1 = ct_dummy_T1, class T2 = ct_dummy_T2, class T3 = ct_dummy_T3, class T4 = ct_dummy_T4, class T5 = ct_dummy_T5,
		class T6 = ct_dummy_T6, class T7 = ct_dummy_T7, class T8 = ct_dummy_T8, class T9 = ct_dummy_T9, class T10 = ct_dummy_T10,
		class T11 = ct_dummy_T11, class T12 = ct_dummy_T12, class T13 = ct_dummy_T13, class T14 = ct_dummy_T14, class T15 = ct_dummy_T15 >
		class variant_lite_t
	{
		//�����ͱ��б�����
		typedef ct_typelist_make16(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15) variant_types;

		// ����ȫ���ͱ��б���ռ�ÿռ�����ֵ
		enum { data_size = ct_typelist_max< variant_types >::value };
		//���ն������ݻ���ռ����
		uint8_t			m_buff[data_size];
		//�����ͱ���������
		size_t			m_type_index;

		//��ȡ�ڲ�������ָ��
		void * m_ptr() { return &m_buff; }
		void const * m_ptr() const { return &m_buff; }

		//ǿ�ƽ������ݻ�����ָ����ͱ�ת��
		template< class U >
		static U * as(void * data) { return reinterpret_cast<U*>(data); }
		template< class U >
		static U const * as(void const * data) { return reinterpret_cast<const U*>(data); }

	public:
		//-----------------------------------------------------
		// Ĭ�Ϲ��캯��,����ͱ��ֵΪ��Ч״̬
		variant_lite_t() : m_type_index(bad_type_index) {}

		//-----------------------------------------------------
		// ���캯��,ʹ��ָ�������ͽ����ڲ��������ľ͵ع����ʼ��
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
		//ʹ������variant_lite_t������п�������
		variant_lite_t(variant_lite_t const & other) : m_type_index(other.m_type_index)
		{
			(void)m_copy_construct(other.m_type_index, other.m_ptr(), m_ptr());
		}

		//-----------------------------------------------------
		//��������(�������������)
		~variant_lite_t()
		{
			if (is_valid())
				m_destroy(m_type_index, m_ptr());
		}
		//-----------------------------------------------------
		// ʹ������������п�����ֵ
		variant_lite_t & operator=(variant_lite_t const & other) { return m_copy_assign(other); }
		//-----------------------------------------------------
		// ʹ�������ͱ��ֵ���и�ֵ,�������ͱ��趨��Ӧ���ͱ�����
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
		//��ȡ��ǰ���ͱ�����
		size_t index() const { return m_type_index; }
		//-----------------------------------------------------
		//�жϵ�ǰ�Ŀɱ����Ƿ���Ч
		bool is_valid() const { return m_type_index != bad_type_index; }
		//-----------------------------------------------------
		//����ǰ�Ŀɱ��������������Ķ������ֵ����
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
		//��ȡָ��������T�ڵ�ǰ�ͱ��б��е�����˳��,Ҳ�����ж��Ƿ���ָ���ͱ�.
		//����ֵ:bad_type_indexʧ��,�ɱ���������ָ���ͱ�;����Ϊ�ҵ����ͱ�����.
		template< class T >
		static  size_t index_of()
		{
			return ct_typelist_of<variant_types, typename ct_remove_cv<T>::type >::value;
		}
		//-----------------------------------------------------
		//��ȡ�ڲ�������������Ϊָ�����͵�ֵ(Ҫ��ָ����������ȷ�ĵ�ǰ�ͱ�)
		template< class T >
		T &
			get()
		{
			rx_assert(index_of<T>() == index()); //����Ŀ�����͵��ͱ���
			return *as<T>(m_ptr()); //���ڲ�������ָ������ת��Ϊָ����Ŀ������
		}

		template< class T >
		T const &
			get() const
		{
			rx_assert(index_of<T>() == index());
			return *as<const T>(m_ptr());
		}
		//-----------------------------------------------------
		// ��ָ���ͱ�������ȡ�ɱ�����ֵ
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
		//�����ͱ��������,������Ӧ�ͱ�����
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

		//����ָ���ͱ�˳��Ŀ�������(��Ŀ�껺�����о͵ع����¶���,��ʹ��Դ���������ݽ��и�ֵ)
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

		//����ָ���ͱ�˳��Ŀ�����ֵ
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
		//�������varaint����Ա�����ֵ
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

		//��ָ���ͱ��ֵ�Ա�����ֵ
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

		//��ָ���ͱ�����,���е�ǰ�ɱ������������������ֵ����
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
	//�ж�ָ���ͱ��ֵ�Ƿ�ɷ���
	template< class T, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 >
	inline bool can(variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & v)
	{
	    typedef variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> vat_t;
		return v.index() == vat_t::template index_of<T>();
	}
	//-----------------------------------------------------
	//�ж�ָ���ͱ��Ƿ����
	template< class T, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 >
	inline bool has(variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & v)
	{
	    typedef variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> vat_t;
		return bad_type_index != vat_t::template index_of<T>();
	}

	//-----------------------------------------------------
	//��ȡָ���ͱ��ֵ
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
	//��ȡָ���ͱ�������Ӧ��ֵ
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
	//���������ɱ�����ֵ
	template< class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15>
	inline void swap(variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> & a, variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> & b)
	{
		a.swap(b);
	}


	//���пɱ����Ĺ�ϵ����Ƚ����Ķ���
	namespace detail {

		template< class Variant >
		struct Comparator
		{
			//�ж������ɱ�����ֵ�Ƿ���ͬ
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

			//�жϿɱ���a��ֵ�Ƿ�С��b��ֵ
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
	//�ж������ɱ�����ֵ�Ƿ���ͬ
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
	//�ж������ɱ�����ֵ�Ƿ�ͬ
	template< class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 >
	inline bool operator!=(
		variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & v,
		variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & w)
	{
		return !(v == w);
	}
	//-----------------------------------------------------
	//�жϿɱ���v��ֵ�Ƿ�С��w
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
	//�жϿɱ���v��ֵ�Ƿ����w
	template< class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 >
	inline bool operator>(
		variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & v,
		variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & w)
	{
		return w < v;
	}
	//-----------------------------------------------------
	//�жϿɱ���v��ֵ�Ƿ�С�ڵ���w
	template< class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 >
	inline bool operator<=(
		variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & v,
		variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & w)
	{
		return !(v > w);
	}
	//-----------------------------------------------------
	//�жϿɱ���v��ֵ�Ƿ���ڵ���w
	template< class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 >
	inline bool operator>=(
		variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & v,
		variant_lite_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & w)
	{
		return !(v < w);
	}

} // namespace rx

#endif
