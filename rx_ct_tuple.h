#ifndef _RX_CT_TUPLE_H_
#define _RX_CT_TUPLE_H_
#include "rx_cc_macro.h"
#include "rx_ct_typelist.h"
#include "rx_assert.h"

//Reference https://github.com/jabernet/tuple99
//�ο�ʵ�ֳ���������Ԫ�鹦��.

namespace rx
{
	//-----------------------------------------------------
	//������Ԫ����ԭ��(���5��ֵ����)
	//-----------------------------------------------------
	template<class T0, class T1 = ct_nulltype, class T2 = ct_nulltype, class T3 = ct_nulltype, class T4 = ct_nulltype>
	class tuple_t;

	//�ж�ĳ���ͱ��Ƿ�ΪԪ��
	template <typename T>
	struct is_tuple { static const bool value = false; };
	template<typename T0, typename T1, typename T2, typename T3, typename T4>
	struct is_tuple<tuple_t<T0, T1, T2, T3, T4> > { static const bool value = true; };

	//-----------------------------------------------------
	// ��ȡ����Ԫ��ָ���ͱ�˳���Ӧ����ʵ�ͱ�
	template< uint32_t K, class T >
	struct tuple_alt;

	template< uint32_t K, class T0, class T1, class T2, class T3, class T4>
	struct tuple_alt< K, tuple_t<T0, T1, T2, T3, T4> >
	{
		typedef typename ct_typelist_at<ct_typelist_make5(T0, T1, T2, T3, T4), K>::type type;
	};

	//-----------------------------------------------------
	//���ڹ��g++ģ�����ģ�庯���ػ���׼��,����ͳһ���ʳ�Ա��������ģ��
	//-----------------------------------------------------
	template<int index, class H0, class H1, class H2, class H3, class H4>
	struct tuple_helper;

	template<class H0, class H1, class H2, class H3, class H4>
	struct tuple_helper<0, H0, H1, H2, H3, H4>				//�����ͱ��������ֵ�ľ�̬�ػ�����,�������־�������ĸ�Ԫ��
	{
		typedef tuple_t<H0, H1, H2, H3, H4> Tuple;
		static typename tuple_alt< 0, Tuple >::type &get(Tuple *t) { return t->m_v0; }
		static typename tuple_alt< 0, Tuple >::type const &get(const Tuple *t) { return t->m_v0; }
	};
	template<class H0, class H1, class H2, class H3, class H4>
	struct tuple_helper<1, H0, H1, H2, H3, H4>
	{
		typedef tuple_t<H0, H1, H2, H3, H4> Tuple;
		static typename tuple_alt< 1, Tuple >::type &get(Tuple *t) { return t->m_v1; }
		static typename tuple_alt< 1, Tuple >::type const &get(const Tuple *t) { return t->m_v1; }
	};
	template<class H0, class H1, class H2, class H3, class H4>
	struct tuple_helper<2, H0, H1, H2, H3, H4>
	{
		typedef tuple_t<H0, H1, H2, H3, H4> Tuple;
		static typename tuple_alt< 2, Tuple >::type &get(Tuple *t) { return t->m_v2; }
		static typename tuple_alt< 2, Tuple >::type const &get(const Tuple *t) { return t->m_v2; }
	};
	template<class H0, class H1, class H2, class H3, class H4>
	struct tuple_helper<3, H0, H1, H2, H3, H4>
	{
		typedef tuple_t<H0, H1, H2, H3, H4> Tuple;
		static typename tuple_alt< 3, Tuple >::type &get(Tuple *t) { return t->m_v3; }
		static typename tuple_alt< 3, Tuple >::type const &get(const Tuple *t) { return t->m_v3; }
	};
	template<class H0, class H1, class H2, class H3, class H4>
	struct tuple_helper<4, H0, H1, H2, H3, H4>
	{
		typedef tuple_t<H0, H1, H2, H3, H4> Tuple;
		static typename tuple_alt< 4, Tuple >::type &get(Tuple *t) { return t->m_v4; }
		static typename tuple_alt< 4, Tuple >::type const &get(const Tuple *t) { return t->m_v4; }
	};


	//-----------------------------------------------------
	//Ԫ����ƫ�ػ�ʵ��,��ÿ����Ч�ͱ�����ж�Ӧ����Ч����.
	//-----------------------------------------------------
	template<class T0>
	class tuple_t<T0, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>
	{
		T0	m_v0;											//������ʵ��Ч�����ݳ�Ա
		template<int, typename, typename, typename, typename, typename>
		friend struct tuple_helper;							//������������Ϊ��Ԫ
	public:
		tuple_t(const T0& v0) :m_v0(v0) {}					//�����ʱ��󶨾����ͱ��ֵ
		static const uint32_t size = 1;						//��֪��Ԫ�����ЧԪ������
		//ʹ���������ͻ�ȡָ��������Ԫ��
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type & get() { return tuple_helper<K, T0, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>::get(this); }
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type const &get() const { return tuple_helper<K, T0, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>::get(this); }
	};

	template<class T0, class T1>
	class tuple_t<T0, T1, ct_nulltype, ct_nulltype, ct_nulltype>
	{
		T0 m_v0; T1 m_v1;
		template<int, typename, typename, typename, typename, typename>
		friend struct tuple_helper;
	public:
		tuple_t(const T0& v0, const T1& v1) :m_v0(v0), m_v1(v1) {}
		static const uint32_t size = 2;
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type & get() { return tuple_helper<K, T0, T1, ct_nulltype, ct_nulltype, ct_nulltype>::get(this); }
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type const &get() const { return tuple_helper<K, T0, T1, ct_nulltype, ct_nulltype, ct_nulltype>::get(this); }
	};

	template<class T0, class T1, class T2>
	class tuple_t<T0, T1, T2, ct_nulltype, ct_nulltype>
	{
		T0 m_v0; T1 m_v1; T2	m_v2;
		template<int, typename, typename, typename, typename, typename>
		friend struct tuple_helper;
	public:
		tuple_t(const T0& v0, const T1& v1, const T2& v2) :m_v0(v0), m_v1(v1), m_v2(v2) {}
		static const uint32_t size = 3;
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type & get() { return tuple_helper<K, T0, T1, T2, ct_nulltype, ct_nulltype>::get(this); }
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type const &get() const { return tuple_helper<K, T0, T1, T2, ct_nulltype, ct_nulltype>::get(this); }
	};

	template<class T0, class T1, class T2, class T3>
	class tuple_t<T0, T1, T2, T3, ct_nulltype>
	{
		T0 m_v0; T1 m_v1; T2 m_v2; T3 m_v3;
		template<int, typename, typename, typename, typename, typename>
		friend struct tuple_helper;
	public:
		tuple_t(const T0& v0, const T1& v1, const T2& v2, const T3& v3) :m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3) {}
		static const uint32_t size = 4;
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type & get() { return tuple_helper<K, T0, T1, T2, T3, ct_nulltype>::get(this); }
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type const &get() const { return tuple_helper<K, T0, T1, T2, T3, ct_nulltype>::get(this); }
	};

	template<class T0, class T1, class T2, class T3, class T4>
	class tuple_t
	{
		T0 m_v0; T1 m_v1; T2 m_v2; T3 m_v3; T4 m_v4;
		template<int, typename, typename, typename, typename, typename>
		friend struct tuple_helper;
	public:
		tuple_t(const T0& v0, const T1& v1, const T2& v2, const T3& v3, const T4 &v4) :m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4) {}
		static const uint32_t size = 5;
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type & get() { return tuple_helper<K, T0, T1, T2, T3, T4>::get(this); }
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type const &get() const { return tuple_helper<K, T0, T1, T2, T3, T4>::get(this); }
	};

	//-----------------------------------------------------
	//�﷨��,��ȡԪ����ָ��������Ӧ��ֵ
	//-----------------------------------------------------
	template< uint32_t K, class T0, class T1, class T2, class T3, class T4>
	inline typename tuple_alt< K, tuple_t<T0, T1, T2, T3, T4> >::type &
		get(tuple_t<T0, T1, T2, T3, T4> & v)
	{
		rx_assert(K < v.size);
		return v.template get<K>();
	}

	template< uint32_t K, class T0, class T1, class T2, class T3, class T4>
	inline typename tuple_alt< K, tuple_t<T0, T1, T2, T3, T4> >::type const &
		get(tuple_t<T0, T1, T2, T3, T4> const & v)
	{
		rx_assert(K < v.size);
		return v.template get<K>();
	}

	//-----------------------------------------------------
	//�﷨��,����Ԫ�����ı�ݺ���
	//-----------------------------------------------------
	template< class T0>
	inline tuple_t<T0>	make_tuple(const T0 &v0) { return tuple_t<T0>(v0); }
	template< class T0, class T1>
	inline tuple_t<T0, T1>	make_tuple(const T0 &v0, const T1 &v1) { return tuple_t<T0, T1>(v0, v1); }
	template< class T0, class T1, class T2>
	inline tuple_t<T0, T1, T2>	make_tuple(const T0 &v0, const T1 &v1, const T2 &v2) { return tuple_t<T0, T1, T2>(v0, v1, v2); }
	template< class T0, class T1, class T2, class T3>
	inline tuple_t<T0, T1, T2, T3>	make_tuple(const T0 &v0, const T1 &v1, const T2 &v2, const T3 &v3) { return tuple_t<T0, T1, T2, T3>(v0, v1, v2, v3); }
	template< class T0, class T1, class T2, class T3, class T4>
	inline tuple_t<T0, T1, T2, T3, T4>	make_tuple(const T0 &v0, const T1 &v1, const T2 &v2, const T3 &v3, const T4 &v4) { return tuple_t<T0, T1, T2, T3, T4>(v0, v1, v2, v3, v4); }

	//-----------------------------------------------------
	//Ԫ�������
	//-----------------------------------------------------
	class tuple_foreach
	{
	public:
		//ʹ�ø�����Ŀ��º����������Ԫ��ı���
		template<typename tuple_type, typename Functor>
		tuple_foreach(const tuple_type& tuple, const Functor& fun)
		{
			m_each(*(typename ct_remove_const<Functor>::type *)(&fun), tuple, ct_index_t<tuple_type::size>());
		}
	private:
		//���ڰ�װԪ�������ѭ������
		template<int pos>
		struct ct_index_t { static const int value = pos; };

		//ģ�庯��ƫ�ػ�,���ڴ�������Ԫ������
		template<typename Functor, typename tuple_type, typename index_type>
		inline void m_each(Functor& fun, const tuple_type& tuple, index_type);

		template<typename Functor, typename tuple_type>
		inline void m_each(Functor, const tuple_type& tuple, ct_index_t<0>);
	};

	//Ԫ���������ʱ,ѭ��������Чʱ���ػ�����
	template<typename Functor, typename tuple_type, typename index_type>
	inline void tuple_foreach::m_each(Functor& fun, const tuple_type& tuple, index_type)
	{
		static const uint32_t cur = (tuple_type::size - index_type::value);	//��ǰ������Ԫ������
		fun(cur, tuple_type::size, tuple.template get<cur>());			//��Ŀ��º�������ǰԪ��Ԫ�صĵ��ô���,ͬʱ����ѭ������������,���ڴ������
		m_each(fun, tuple, ct_index_t<index_type::value - 1>());		//֮��ѭ�������ݼ�,�ݹ������һ�ִ�����
	}

	//Ԫ���������ʱ,ѭ��������Чʱ���ػ�����,ɶҲ����
	template<typename Functor, typename tuple_type>
	inline void tuple_foreach::m_each(Functor, const tuple_type&, ct_index_t<0>) {}
}
#endif
