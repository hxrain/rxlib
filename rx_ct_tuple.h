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
	//������Ԫ����ԭ��(���16��ֵ����)
	//-----------------------------------------------------
	template<class T0, class T1 = ct_nulltype, class T2 = ct_nulltype, class T3 = ct_nulltype, class T4 = ct_nulltype, class T5 = ct_nulltype,
		class T6 = ct_nulltype, class T7 = ct_nulltype, class T8 = ct_nulltype, class T9 = ct_nulltype, class T10 = ct_nulltype, class T11 = ct_nulltype,
		class T12 = ct_nulltype, class T13 = ct_nulltype, class T14 = ct_nulltype, class T15 = ct_nulltype>
		class tuple_t;

	//�ж�ĳ���ͱ��Ƿ�ΪԪ��
	template <typename T>
	struct is_tuple { static const bool value = false; };
	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7,
		typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14, typename T15>
		struct is_tuple<tuple_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> > { static const bool value = true; };

	//-----------------------------------------------------
	// ��ȡ����Ԫ��ָ���ͱ�˳���Ӧ����ʵ�ͱ�
	template< uint32_t K, class Tuple >
	struct tuple_alt;

	template< uint32_t K, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7,
		class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15>
		struct tuple_alt< K, tuple_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> >
	{
		typedef typename ct_typelist_at<ct_typelist_make16(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15), K>::type type;
	};

	//-----------------------------------------------------
	//���ڹ��g++ģ�����ģ�庯���ػ���׼��,����ͳһ���ʳ�Ա��������ģ��
	//-----------------------------------------------------
	template<int index, class H0, class H1, class H2, class H3, class H4, class H5, class H6, class H7,
		class H8, class H9, class H10, class H11, class H12, class H13, class H14, class H15>
		struct tuple_helper;

	template<class H0, class H1, class H2, class H3, class H4, class H5, class H6, class H7,
		class H8, class H9, class H10, class H11, class H12, class H13, class H14, class H15>
		struct tuple_helper<0, H0, H1, H2, H3, H4, H5, H6, H7, H8, H9, H10, H11, H12, H13, H14, H15>				//�����ͱ��������ֵ�ľ�̬�ػ�����,�������־�������ĸ�Ԫ��
	{
		typedef tuple_t<H0, H1, H2, H3, H4, H5, H6, H7, H8, H9, H10, H11, H12, H13, H14, H15> Tuple;
		static typename tuple_alt< 0, Tuple >::type &get(Tuple *t) { return t->m_v0; }
		static typename tuple_alt< 0, Tuple >::type const &get(const Tuple *t) { return t->m_v0; }
	};
	template<class H0, class H1, class H2, class H3, class H4, class H5, class H6, class H7,
		class H8, class H9, class H10, class H11, class H12, class H13, class H14, class H15>
		struct tuple_helper<1, H0, H1, H2, H3, H4, H5, H6, H7, H8, H9, H10, H11, H12, H13, H14, H15>				//�����ͱ��������ֵ�ľ�̬�ػ�����,�������־�������ĸ�Ԫ��
	{
		typedef tuple_t<H0, H1, H2, H3, H4, H5, H6, H7, H8, H9, H10, H11, H12, H13, H14, H15> Tuple;
		static typename tuple_alt< 1, Tuple >::type &get(Tuple *t) { return t->m_v1; }
		static typename tuple_alt< 1, Tuple >::type const &get(const Tuple *t) { return t->m_v1; }
	};
	template<class H0, class H1, class H2, class H3, class H4, class H5, class H6, class H7,
		class H8, class H9, class H10, class H11, class H12, class H13, class H14, class H15>
		struct tuple_helper<2, H0, H1, H2, H3, H4, H5, H6, H7, H8, H9, H10, H11, H12, H13, H14, H15>				//�����ͱ��������ֵ�ľ�̬�ػ�����,�������־�������ĸ�Ԫ��
	{
		typedef tuple_t<H0, H1, H2, H3, H4, H5, H6, H7, H8, H9, H10, H11, H12, H13, H14, H15> Tuple;
		static typename tuple_alt< 2, Tuple >::type &get(Tuple *t) { return t->m_v2; }
		static typename tuple_alt< 2, Tuple >::type const &get(const Tuple *t) { return t->m_v2; }
	};
	template<class H0, class H1, class H2, class H3, class H4, class H5, class H6, class H7,
		class H8, class H9, class H10, class H11, class H12, class H13, class H14, class H15>
		struct tuple_helper<3, H0, H1, H2, H3, H4, H5, H6, H7, H8, H9, H10, H11, H12, H13, H14, H15>				//�����ͱ��������ֵ�ľ�̬�ػ�����,�������־�������ĸ�Ԫ��
	{
		typedef tuple_t<H0, H1, H2, H3, H4, H5, H6, H7, H8, H9, H10, H11, H12, H13, H14, H15> Tuple;
		static typename tuple_alt< 3, Tuple >::type &get(Tuple *t) { return t->m_v3; }
		static typename tuple_alt< 3, Tuple >::type const &get(const Tuple *t) { return t->m_v3; }
	};
	template<class H0, class H1, class H2, class H3, class H4, class H5, class H6, class H7,
		class H8, class H9, class H10, class H11, class H12, class H13, class H14, class H15>
		struct tuple_helper<4, H0, H1, H2, H3, H4, H5, H6, H7, H8, H9, H10, H11, H12, H13, H14, H15>				//�����ͱ��������ֵ�ľ�̬�ػ�����,�������־�������ĸ�Ԫ��
	{
		typedef tuple_t<H0, H1, H2, H3, H4, H5, H6, H7, H8, H9, H10, H11, H12, H13, H14, H15> Tuple;
		static typename tuple_alt< 4, Tuple >::type &get(Tuple *t) { return t->m_v4; }
		static typename tuple_alt< 4, Tuple >::type const &get(const Tuple *t) { return t->m_v4; }
	};
	template<class H0, class H1, class H2, class H3, class H4, class H5, class H6, class H7,
		class H8, class H9, class H10, class H11, class H12, class H13, class H14, class H15>
		struct tuple_helper<5, H0, H1, H2, H3, H4, H5, H6, H7, H8, H9, H10, H11, H12, H13, H14, H15>				//�����ͱ��������ֵ�ľ�̬�ػ�����,�������־�������ĸ�Ԫ��
	{
		typedef tuple_t<H0, H1, H2, H3, H4, H5, H6, H7, H8, H9, H10, H11, H12, H13, H14, H15> Tuple;
		static typename tuple_alt< 5, Tuple >::type &get(Tuple *t) { return t->m_v5; }
		static typename tuple_alt< 5, Tuple >::type const &get(const Tuple *t) { return t->m_v5; }
	};
	template<class H0, class H1, class H2, class H3, class H4, class H5, class H6, class H7,
		class H8, class H9, class H10, class H11, class H12, class H13, class H14, class H15>
		struct tuple_helper<6, H0, H1, H2, H3, H4, H5, H6, H7, H8, H9, H10, H11, H12, H13, H14, H15>				//�����ͱ��������ֵ�ľ�̬�ػ�����,�������־�������ĸ�Ԫ��
	{
		typedef tuple_t<H0, H1, H2, H3, H4, H5, H6, H7, H8, H9, H10, H11, H12, H13, H14, H15> Tuple;
		static typename tuple_alt< 6, Tuple >::type &get(Tuple *t) { return t->m_v6; }
		static typename tuple_alt< 6, Tuple >::type const &get(const Tuple *t) { return t->m_v6; }
	};
	template<class H0, class H1, class H2, class H3, class H4, class H5, class H6, class H7,
		class H8, class H9, class H10, class H11, class H12, class H13, class H14, class H15>
		struct tuple_helper<7, H0, H1, H2, H3, H4, H5, H6, H7, H8, H9, H10, H11, H12, H13, H14, H15>				//�����ͱ��������ֵ�ľ�̬�ػ�����,�������־�������ĸ�Ԫ��
	{
		typedef tuple_t<H0, H1, H2, H3, H4, H5, H6, H7, H8, H9, H10, H11, H12, H13, H14, H15> Tuple;
		static typename tuple_alt< 7, Tuple >::type &get(Tuple *t) { return t->m_v7; }
		static typename tuple_alt< 7, Tuple >::type const &get(const Tuple *t) { return t->m_v7; }
	};
	template<class H0, class H1, class H2, class H3, class H4, class H5, class H6, class H7,
		class H8, class H9, class H10, class H11, class H12, class H13, class H14, class H15>
		struct tuple_helper<8, H0, H1, H2, H3, H4, H5, H6, H7, H8, H9, H10, H11, H12, H13, H14, H15>				//�����ͱ��������ֵ�ľ�̬�ػ�����,�������־�������ĸ�Ԫ��
	{
		typedef tuple_t<H0, H1, H2, H3, H4, H5, H6, H7, H8, H9, H10, H11, H12, H13, H14, H15> Tuple;
		static typename tuple_alt< 8, Tuple >::type &get(Tuple *t) { return t->m_v8; }
		static typename tuple_alt< 8, Tuple >::type const &get(const Tuple *t) { return t->m_v8; }
	};
	template<class H0, class H1, class H2, class H3, class H4, class H5, class H6, class H7,
		class H8, class H9, class H10, class H11, class H12, class H13, class H14, class H15>
		struct tuple_helper<9, H0, H1, H2, H3, H4, H5, H6, H7, H8, H9, H10, H11, H12, H13, H14, H15>				//�����ͱ��������ֵ�ľ�̬�ػ�����,�������־�������ĸ�Ԫ��
	{
		typedef tuple_t<H0, H1, H2, H3, H4, H5, H6, H7, H8, H9, H10, H11, H12, H13, H14, H15> Tuple;
		static typename tuple_alt< 9, Tuple >::type &get(Tuple *t) { return t->m_v9; }
		static typename tuple_alt< 9, Tuple >::type const &get(const Tuple *t) { return t->m_v9; }
	};
	template<class H0, class H1, class H2, class H3, class H4, class H5, class H6, class H7,
		class H8, class H9, class H10, class H11, class H12, class H13, class H14, class H15>
		struct tuple_helper<10, H0, H1, H2, H3, H4, H5, H6, H7, H8, H9, H10, H11, H12, H13, H14, H15>				//�����ͱ��������ֵ�ľ�̬�ػ�����,�������־�������ĸ�Ԫ��
	{
		typedef tuple_t<H0, H1, H2, H3, H4, H5, H6, H7, H8, H9, H10, H11, H12, H13, H14, H15> Tuple;
		static typename tuple_alt< 10, Tuple >::type &get(Tuple *t) { return t->m_v10; }
		static typename tuple_alt< 10, Tuple >::type const &get(const Tuple *t) { return t->m_v10; }
	};
	template<class H0, class H1, class H2, class H3, class H4, class H5, class H6, class H7,
		class H8, class H9, class H10, class H11, class H12, class H13, class H14, class H15>
		struct tuple_helper<11, H0, H1, H2, H3, H4, H5, H6, H7, H8, H9, H10, H11, H12, H13, H14, H15>				//�����ͱ��������ֵ�ľ�̬�ػ�����,�������־�������ĸ�Ԫ��
	{
		typedef tuple_t<H0, H1, H2, H3, H4, H5, H6, H7, H8, H9, H10, H11, H12, H13, H14, H15> Tuple;
		static typename tuple_alt< 11, Tuple >::type &get(Tuple *t) { return t->m_v11; }
		static typename tuple_alt< 11, Tuple >::type const &get(const Tuple *t) { return t->m_v11; }
	};
	template<class H0, class H1, class H2, class H3, class H4, class H5, class H6, class H7,
		class H8, class H9, class H10, class H11, class H12, class H13, class H14, class H15>
		struct tuple_helper<12, H0, H1, H2, H3, H4, H5, H6, H7, H8, H9, H10, H11, H12, H13, H14, H15>				//�����ͱ��������ֵ�ľ�̬�ػ�����,�������־�������ĸ�Ԫ��
	{
		typedef tuple_t<H0, H1, H2, H3, H4, H5, H6, H7, H8, H9, H10, H11, H12, H13, H14, H15> Tuple;
		static typename tuple_alt< 12, Tuple >::type &get(Tuple *t) { return t->m_v12; }
		static typename tuple_alt< 12, Tuple >::type const &get(const Tuple *t) { return t->m_v12; }
	};
	template<class H0, class H1, class H2, class H3, class H4, class H5, class H6, class H7,
		class H8, class H9, class H10, class H11, class H12, class H13, class H14, class H15>
		struct tuple_helper<13, H0, H1, H2, H3, H4, H5, H6, H7, H8, H9, H10, H11, H12, H13, H14, H15>				//�����ͱ��������ֵ�ľ�̬�ػ�����,�������־�������ĸ�Ԫ��
	{
		typedef tuple_t<H0, H1, H2, H3, H4, H5, H6, H7, H8, H9, H10, H11, H12, H13, H14, H15> Tuple;
		static typename tuple_alt< 13, Tuple >::type &get(Tuple *t) { return t->m_v13; }
		static typename tuple_alt< 13, Tuple >::type const &get(const Tuple *t) { return t->m_v13; }
	};
	template<class H0, class H1, class H2, class H3, class H4, class H5, class H6, class H7,
		class H8, class H9, class H10, class H11, class H12, class H13, class H14, class H15>
		struct tuple_helper<14, H0, H1, H2, H3, H4, H5, H6, H7, H8, H9, H10, H11, H12, H13, H14, H15>				//�����ͱ��������ֵ�ľ�̬�ػ�����,�������־�������ĸ�Ԫ��
	{
		typedef tuple_t<H0, H1, H2, H3, H4, H5, H6, H7, H8, H9, H10, H11, H12, H13, H14, H15> Tuple;
		static typename tuple_alt< 14, Tuple >::type &get(Tuple *t) { return t->m_v14; }
		static typename tuple_alt< 14, Tuple >::type const &get(const Tuple *t) { return t->m_v14; }
	};
	template<class H0, class H1, class H2, class H3, class H4, class H5, class H6, class H7,
		class H8, class H9, class H10, class H11, class H12, class H13, class H14, class H15>
		struct tuple_helper<15, H0, H1, H2, H3, H4, H5, H6, H7, H8, H9, H10, H11, H12, H13, H14, H15>				//�����ͱ��������ֵ�ľ�̬�ػ�����,�������־�������ĸ�Ԫ��
	{
		typedef tuple_t<H0, H1, H2, H3, H4, H5, H6, H7, H8, H9, H10, H11, H12, H13, H14, H15> Tuple;
		static typename tuple_alt< 15, Tuple >::type &get(Tuple *t) { return t->m_v15; }
		static typename tuple_alt< 15, Tuple >::type const &get(const Tuple *t) { return t->m_v15; }
	};


	//-----------------------------------------------------
	//Ԫ����ƫ�ػ�ʵ��,��ÿ����Ч�ͱ�����ж�Ӧ����Ч����.
	//-----------------------------------------------------
	template<class T0>
	class tuple_t<T0, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype,
		ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>
	{
		T0	m_v0;											//������ʵ��Ч�����ݳ�Ա
		template<int, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename>
		friend struct tuple_helper;							//������������Ϊ��Ԫ
	public:
		tuple_t(const T0& v0) :m_v0(v0) {}					//�����ʱ��󶨾����ͱ��ֵ
		static const uint32_t size = 1;						//��֪��Ԫ�����ЧԪ������
		//ʹ���������ͻ�ȡָ��������Ԫ��
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type & get() {
			return tuple_helper<K, T0, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype,
				ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>::get(this);
		}
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type const &get() const {
			return tuple_helper<K, T0, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype,
				ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>::get(this);
		}
		template<typename ret_type>
		inline ret_type& take(uint32_t idx) { return *(ret_type*)&m_v0; }
	};

	template<class T0, class T1>
	class tuple_t<T0, T1, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype,
		ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>
	{
		T0 m_v0; T1 m_v1;
		template<int, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename>
		friend struct tuple_helper;							//������������Ϊ��Ԫ
	public:
		tuple_t(const T0& v0, const T1& v1) :m_v0(v0), m_v1(v1) {}
		static const uint32_t size = 2;
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type & get() {
			return tuple_helper<K, T0, T1, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype,
				ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>::get(this);
		}
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type const &get() const {
			return tuple_helper<K, T0, T1, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype,
				ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>::get(this);
		}
		template<typename ret_type>
		inline ret_type& take(uint32_t idx) {
			switch (idx) { case 0:return *(ret_type*)&m_v0;case 1:return *(ret_type*)&m_v1;default:return *(ret_type*)0; }
		}
	};

	template<class T0, class T1, class T2>
	class tuple_t<T0, T1, T2, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype,
		ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>
	{
		T0 m_v0; T1 m_v1; T2	m_v2;
		template<int, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename>
		friend struct tuple_helper;							//������������Ϊ��Ԫ
	public:
		tuple_t(const T0& v0, const T1& v1, const T2& v2) :m_v0(v0), m_v1(v1), m_v2(v2) {}
		static const uint32_t size = 3;
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type & get() {
			return tuple_helper<K, T0, T1, T2, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype,
				ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>::get(this);
		}
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type const &get() const {
			return tuple_helper<K, T0, T1, T2, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype,
				ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>::get(this);
		}
		template<typename ret_type>
		inline ret_type& take(uint32_t idx) {
			switch (idx) { case 0:return *(ret_type*)&m_v0;case 1:return *(ret_type*)&m_v1;case 2:return *(ret_type*)&m_v2;default:return *(ret_type*)0; }
		}
	};

	template<class T0, class T1, class T2, class T3>
	class tuple_t<T0, T1, T2, T3, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype,
		ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>
	{
		T0 m_v0; T1 m_v1; T2 m_v2; T3 m_v3;
		template<int, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename>
		friend struct tuple_helper;							//������������Ϊ��Ԫ
	public:
		tuple_t(const T0& v0, const T1& v1, const T2& v2, const T3& v3) :m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3) {}
		static const uint32_t size = 4;
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type & get() {
			return tuple_helper<K, T0, T1, T2, T3, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype,
				ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>::get(this);
		}
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type const &get() const {
			return tuple_helper<K, T0, T1, T2, T3, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype,
				ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>::get(this);
		}
		template<typename ret_type>
		inline ret_type& take(uint32_t idx) {
			switch (idx) { case 0:return *(ret_type*)&m_v0;case 1:return *(ret_type*)&m_v1;case 2:return *(ret_type*)&m_v2;case 3:return *(ret_type*)&m_v3;default:return *(ret_type*)0; }
		}
	};

	template<class T0, class T1, class T2, class T3, class T4>
	class tuple_t<T0, T1, T2, T3, T4, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype,
		ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>
	{
		T0 m_v0; T1 m_v1; T2 m_v2; T3 m_v3; T4 m_v4;
		template<int, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename>
		friend struct tuple_helper;							//������������Ϊ��Ԫ
	public:
		tuple_t(const T0& v0, const T1& v1, const T2& v2, const T3& v3, const T4 &v4) :m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4) {}
		static const uint32_t size = 5;
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type & get() {
			return tuple_helper<K, T0, T1, T2, T3, T4, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype,
				ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>::get(this);
		}
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type const &get() const {
			return tuple_helper<K, T0, T1, T2, T3, T4, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype,
				ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>::get(this);
		}
		template<typename ret_type>
		inline ret_type& take(uint32_t idx) {
			switch (idx) { case 0:return *(ret_type*)&m_v0;case 1:return *(ret_type*)&m_v1;case 2:return *(ret_type*)&m_v2;case 3:return *(ret_type*)&m_v3;case 4:return *(ret_type*)&m_v4;default:return *(ret_type*)0; }
		}
	};

	template<class T0, class T1, class T2, class T3, class T4, class T5>
	class tuple_t<T0, T1, T2, T3, T4, T5, ct_nulltype, ct_nulltype, ct_nulltype,
		ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>
	{
		T0 m_v0; T1 m_v1; T2 m_v2; T3 m_v3; T4 m_v4; T5 m_v5;
		template<int, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename>
		friend struct tuple_helper;							//������������Ϊ��Ԫ
	public:
		tuple_t(const T0& v0, const T1& v1, const T2& v2, const T3& v3, const T4 &v4, const T5 &v5) :m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5) {}
		static const uint32_t size = 6;
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type & get() {
			return tuple_helper<K, T0, T1, T2, T3, T4, T5, ct_nulltype, ct_nulltype, ct_nulltype,
				ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>::get(this);
		}
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type const &get() const {
			return tuple_helper<K, T0, T1, T2, T3, T4, T5, ct_nulltype, ct_nulltype, ct_nulltype,
				ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>::get(this);
		}
		template<typename ret_type>
		inline ret_type& take(uint32_t idx) {
			switch (idx) { 
				case 0:return *(ret_type*)&m_v0;case 1:return *(ret_type*)&m_v1;case 2:return *(ret_type*)&m_v2;case 3:return *(ret_type*)&m_v3;case 4:return *(ret_type*)&m_v4;
				case 5:return *(ret_type*)&m_v5;default:return *(ret_type*)0; 
			}
		}
	};

	template<class T0, class T1, class T2, class T3, class T4, class T5, class T6>
	class tuple_t<T0, T1, T2, T3, T4, T5, T6, ct_nulltype, ct_nulltype,
		ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>
	{
		T0 m_v0; T1 m_v1; T2 m_v2; T3 m_v3; T4 m_v4; T5 m_v5; T6 m_v6;
		template<int, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename>
		friend struct tuple_helper;							//������������Ϊ��Ԫ
	public:
		tuple_t(const T0& v0, const T1& v1, const T2& v2, const T3& v3, const T4 &v4, const T5 &v5, const T6 &v6) :m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6) {}
		static const uint32_t size = 7;
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type & get() {
			return tuple_helper<K, T0, T1, T2, T3, T4, T5, T6, ct_nulltype, ct_nulltype,
				ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>::get(this);
		}
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type const &get() const {
			return tuple_helper<K, T0, T1, T2, T3, T4, T5, T6, ct_nulltype, ct_nulltype,
				ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>::get(this);
		}
		template<typename ret_type>
		inline ret_type& take(uint32_t idx) {
			switch (idx) {
				case 0:return *(ret_type*)&m_v0;case 1:return *(ret_type*)&m_v1;case 2:return *(ret_type*)&m_v2;case 3:return *(ret_type*)&m_v3;case 4:return *(ret_type*)&m_v4;
				case 5:return *(ret_type*)&m_v5;case 6:return *(ret_type*)&m_v6;default:return *(ret_type*)0;
			}
		}
	};

	template<class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
	class tuple_t<T0, T1, T2, T3, T4, T5, T6, T7, ct_nulltype,
		ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>
	{
		T0 m_v0; T1 m_v1; T2 m_v2; T3 m_v3; T4 m_v4; T5 m_v5; T6 m_v6; T7 m_v7;
		template<int, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename>
		friend struct tuple_helper;							//������������Ϊ��Ԫ
	public:
		tuple_t(const T0& v0, const T1& v1, const T2& v2, const T3& v3, const T4 &v4, const T5 &v5, const T6 &v6, const T7 &v7) :m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6), m_v7(v7) {}
		static const uint32_t size = 8;
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type & get() {
			return tuple_helper<K, T0, T1, T2, T3, T4, T5, T6, T7, ct_nulltype,
				ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>::get(this);
		}
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type const &get() const {
			return tuple_helper<K, T0, T1, T2, T3, T4, T5, T6, T7, ct_nulltype,
				ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>::get(this);
		}
		template<typename ret_type>
		inline ret_type& take(uint32_t idx) {
			switch (idx) {
				case 0:return *(ret_type*)&m_v0;case 1:return *(ret_type*)&m_v1;case 2:return *(ret_type*)&m_v2;case 3:return *(ret_type*)&m_v3;case 4:return *(ret_type*)&m_v4;
				case 5:return *(ret_type*)&m_v5;case 6:return *(ret_type*)&m_v6;case 7:return *(ret_type*)&m_v7;default:return *(ret_type*)0;
			}
		}
	};

	template<class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
	class tuple_t<T0, T1, T2, T3, T4, T5, T6, T7, T8,
		ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>
	{
		T0 m_v0; T1 m_v1; T2 m_v2; T3 m_v3; T4 m_v4; T5 m_v5; T6 m_v6; T7 m_v7;  T8 m_v8;
		template<int, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename>
		friend struct tuple_helper;							//������������Ϊ��Ԫ
	public:
		tuple_t(const T0& v0, const T1& v1, const T2& v2, const T3& v3, const T4 &v4, const T5 &v5, const T6 &v6, const T7 &v7, const T8 &v8) :m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6), m_v7(v7), m_v8(v8) {}
		static const uint32_t size = 9;
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type & get() {
			return tuple_helper<K, T0, T1, T2, T3, T4, T5, T6, T7, T8,
				ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>::get(this);
		}
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type const &get() const {
			return tuple_helper<K, T0, T1, T2, T3, T4, T5, T6, T7, T8,
				ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>::get(this);
		}
		template<typename ret_type>
		inline ret_type& take(uint32_t idx) {
			switch (idx) {
				case 0:return *(ret_type*)&m_v0;case 1:return *(ret_type*)&m_v1;case 2:return *(ret_type*)&m_v2;case 3:return *(ret_type*)&m_v3;case 4:return *(ret_type*)&m_v4;
				case 5:return *(ret_type*)&m_v5;case 6:return *(ret_type*)&m_v6;case 7:return *(ret_type*)&m_v7;case 8:return *(ret_type*)&m_v8;default:return *(ret_type*)0;
			}
		}
	};

	template<class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
	class tuple_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>
	{
		T0 m_v0; T1 m_v1; T2 m_v2; T3 m_v3; T4 m_v4; T5 m_v5; T6 m_v6; T7 m_v7;  T8 m_v8;  T9 m_v9;
		template<int, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename>
		friend struct tuple_helper;							//������������Ϊ��Ԫ
	public:
		tuple_t(const T0& v0, const T1& v1, const T2& v2, const T3& v3, const T4 &v4, const T5 &v5, const T6 &v6, const T7 &v7, const T8 &v8, const T9 &v9) :m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6), m_v7(v7), m_v8(v8), m_v9(v9) {}
		static const uint32_t size = 10;
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type & get() {
			return tuple_helper<K, T0, T1, T2, T3, T4, T5, T6, T7, T8,
				T9, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>::get(this);
		}
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type const &get() const {
			return tuple_helper<K, T0, T1, T2, T3, T4, T5, T6, T7, T8,
				T9, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>::get(this);
		}
		template<typename ret_type>
		inline ret_type& take(uint32_t idx) {
			switch (idx) {
				case 0:return *(ret_type*)&m_v0;case 1:return *(ret_type*)&m_v1;case 2:return *(ret_type*)&m_v2;case 3:return *(ret_type*)&m_v3;case 4:return *(ret_type*)&m_v4;
				case 5:return *(ret_type*)&m_v5;case 6:return *(ret_type*)&m_v6;case 7:return *(ret_type*)&m_v7;case 8:return *(ret_type*)&m_v8;case 9:return *(ret_type*)&m_v9;default:return *(ret_type*)0;
			}
		}
	};

	template<class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
	class tuple_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>
	{
		T0 m_v0; T1 m_v1; T2 m_v2; T3 m_v3; T4 m_v4; T5 m_v5; T6 m_v6; T7 m_v7;  T8 m_v8;  T9 m_v9; T10 m_v10;
		template<int, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename>
		friend struct tuple_helper;							//������������Ϊ��Ԫ
	public:
		tuple_t(const T0& v0, const T1& v1, const T2& v2, const T3& v3, const T4 &v4, const T5 &v5, const T6 &v6, const T7 &v7, const T8 &v8, const T9 &v9, const T10 &v10) :m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6), m_v7(v7), m_v8(v8), m_v9(v9), m_v10(v10) {}
		static const uint32_t size = 11;
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type & get() {
			return tuple_helper<K, T0, T1, T2, T3, T4, T5, T6, T7, T8,
				T9, T10, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>::get(this);
		}
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type const &get() const {
			return tuple_helper<K, T0, T1, T2, T3, T4, T5, T6, T7, T8,
				T9, T10, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>::get(this);
		}
		template<typename ret_type>
		inline ret_type& take(uint32_t idx) {
			switch (idx) {
				case 0:return *(ret_type*)&m_v0;case 1:return *(ret_type*)&m_v1;case 2:return *(ret_type*)&m_v2;case 3:return *(ret_type*)&m_v3;case 4:return *(ret_type*)&m_v4;
				case 5:return *(ret_type*)&m_v5;case 6:return *(ret_type*)&m_v6;case 7:return *(ret_type*)&m_v7;case 8:return *(ret_type*)&m_v8;case 9:return *(ret_type*)&m_v9;
				case 10:return *(ret_type*)&m_v10;default:return *(ret_type*)0;
			}
		}
	};

	template<class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11>
	class tuple_t<T0, T1, T2, T3, T4, T5, T6, T7, T8,
		T9, T10, T11, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>
	{
		T0 m_v0; T1 m_v1; T2 m_v2; T3 m_v3; T4 m_v4; T5 m_v5; T6 m_v6; T7 m_v7;  T8 m_v8;  T9 m_v9; T10 m_v10; T11 m_v11;
		template<int, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename>
		friend struct tuple_helper;							//������������Ϊ��Ԫ
	public:
		tuple_t(const T0& v0, const T1& v1, const T2& v2, const T3& v3, const T4 &v4, const T5 &v5, const T6 &v6, const T7 &v7, const T8 &v8, const T9 &v9, const T10 &v10, const T11 &v11) :m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6), m_v7(v7), m_v8(v8), m_v9(v9), m_v10(v10), m_v11(v11) {}
		static const uint32_t size = 12;
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type & get() {
			return tuple_helper<K, T0, T1, T2, T3, T4, T5, T6, T7, T8,
				T9, T10, T11, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>::get(this);
		}
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type const &get() const {
			return tuple_helper<K, T0, T1, T2, T3, T4, T5, T6, T7, T8,
				T9, T10, T11, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>::get(this);
		}
		template<typename ret_type>
		inline ret_type& take(uint32_t idx) {
			switch (idx) {
				case 0:return *(ret_type*)&m_v0;case 1:return *(ret_type*)&m_v1;case 2:return *(ret_type*)&m_v2;case 3:return *(ret_type*)&m_v3;case 4:return *(ret_type*)&m_v4;
				case 5:return *(ret_type*)&m_v5;case 6:return *(ret_type*)&m_v6;case 7:return *(ret_type*)&m_v7;case 8:return *(ret_type*)&m_v8;case 9:return *(ret_type*)&m_v9;
				case 10:return *(ret_type*)&m_v10;case 11:return *(ret_type*)&m_v11;default:return *(ret_type*)0;
			}
		}
	};

	template<class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12>
	class tuple_t<T0, T1, T2, T3, T4, T5, T6, T7, T8,
		T9, T10, T11, T12, ct_nulltype, ct_nulltype, ct_nulltype>
	{
		T0 m_v0; T1 m_v1; T2 m_v2; T3 m_v3; T4 m_v4; T5 m_v5; T6 m_v6; T7 m_v7;  T8 m_v8;  T9 m_v9; T10 m_v10; T11 m_v11; T12 m_v12;
		template<int, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename>
		friend struct tuple_helper;							//������������Ϊ��Ԫ
	public:
		tuple_t(const T0& v0, const T1& v1, const T2& v2, const T3& v3, const T4 &v4, const T5 &v5, const T6 &v6, const T7 &v7, const T8 &v8, const T9 &v9, const T10 &v10, const T11 &v11, const T12 &v12) :m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6), m_v7(v7), m_v8(v8), m_v9(v9), m_v10(v10), m_v11(v11), m_v12(v12) {}
		static const uint32_t size = 13;
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type & get() {
			return tuple_helper<K, T0, T1, T2, T3, T4, T5, T6, T7, T8,
				T9, T10, T11, T12, ct_nulltype, ct_nulltype, ct_nulltype>::get(this);
		}
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type const &get() const {
			return tuple_helper<K, T0, T1, T2, T3, T4, T5, T6, T7, T8,
				T9, T10, T11, T12, ct_nulltype, ct_nulltype, ct_nulltype>::get(this);
		}
		template<typename ret_type>
		inline ret_type& take(uint32_t idx) {
			switch (idx) {
				case 0:return *(ret_type*)&m_v0;case 1:return *(ret_type*)&m_v1;case 2:return *(ret_type*)&m_v2;case 3:return *(ret_type*)&m_v3;case 4:return *(ret_type*)&m_v4;
				case 5:return *(ret_type*)&m_v5;case 6:return *(ret_type*)&m_v6;case 7:return *(ret_type*)&m_v7;case 8:return *(ret_type*)&m_v8;case 9:return *(ret_type*)&m_v9;
				case 10:return *(ret_type*)&m_v10;case 11:return *(ret_type*)&m_v11;case 12:return *(ret_type*)&m_v12;default:return *(ret_type*)0;
			}
		}
	};

	template<class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13>
	class tuple_t<T0, T1, T2, T3, T4, T5, T6, T7, T8,
		T9, T10, T11, T12, T13, ct_nulltype, ct_nulltype>
	{
		T0 m_v0; T1 m_v1; T2 m_v2; T3 m_v3; T4 m_v4; T5 m_v5; T6 m_v6; T7 m_v7;  T8 m_v8;  T9 m_v9; T10 m_v10; T11 m_v11; T12 m_v12; T13 m_v13;
		template<int, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename>
		friend struct tuple_helper;							//������������Ϊ��Ԫ
	public:
		tuple_t(const T0& v0, const T1& v1, const T2& v2, const T3& v3, const T4 &v4, const T5 &v5, const T6 &v6, const T7 &v7, const T8 &v8, const T9 &v9, const T10 &v10, const T11 &v11, const T12 &v12, const T13 &v13) :m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6), m_v7(v7), m_v8(v8), m_v9(v9), m_v10(v10), m_v11(v11), m_v12(v12), m_v13(v13) {}
		static const uint32_t size = 14;
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type & get() {
			return tuple_helper<K, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, ct_nulltype, ct_nulltype>::get(this);
		}
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type const &get() const {
			return tuple_helper<K, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, ct_nulltype, ct_nulltype>::get(this);
		}
		template<typename ret_type>
		inline ret_type& take(uint32_t idx) {
			switch (idx) {
				case 0:return *(ret_type*)&m_v0;case 1:return *(ret_type*)&m_v1;case 2:return *(ret_type*)&m_v2;case 3:return *(ret_type*)&m_v3;case 4:return *(ret_type*)&m_v4;
				case 5:return *(ret_type*)&m_v5;case 6:return *(ret_type*)&m_v6;case 7:return *(ret_type*)&m_v7;case 8:return *(ret_type*)&m_v8;case 9:return *(ret_type*)&m_v9;
				case 10:return *(ret_type*)&m_v10;case 11:return *(ret_type*)&m_v11;case 12:return *(ret_type*)&m_v12;case 13:return *(ret_type*)&m_v13;default:return *(ret_type*)0;
			}
		}
	};

	template<class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14>
	class tuple_t<T0, T1, T2, T3, T4, T5, T6, T7, T8,
		T9, T10, T11, T12, T13, T14, ct_nulltype>
	{
		T0 m_v0; T1 m_v1; T2 m_v2; T3 m_v3; T4 m_v4; T5 m_v5; T6 m_v6; T7 m_v7;  T8 m_v8;  T9 m_v9; T10 m_v10; T11 m_v11; T12 m_v12; T13 m_v13; T14 m_v14;
		template<int, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename>
		friend struct tuple_helper;							//������������Ϊ��Ԫ
	public:
		tuple_t(const T0& v0, const T1& v1, const T2& v2, const T3& v3, const T4 &v4, const T5 &v5, const T6 &v6, const T7 &v7, const T8 &v8, const T9 &v9, const T10 &v10, const T11 &v11, const T12 &v12, const T13 &v13, const T14 &v14) :m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6), m_v7(v7), m_v8(v8), m_v9(v9), m_v10(v10), m_v11(v11), m_v12(v12), m_v13(v13), m_v14(v14) {}
		static const uint32_t size = 15;
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type & get() {
			return tuple_helper<K, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, ct_nulltype>::get(this);
		}
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type const &get() const {
			return tuple_helper<K, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, ct_nulltype>::get(this);
		}
		template<typename ret_type>
		inline ret_type& take(uint32_t idx) {
			switch (idx) {
				case 0:return *(ret_type*)&m_v0;case 1:return *(ret_type*)&m_v1;case 2:return *(ret_type*)&m_v2;case 3:return *(ret_type*)&m_v3;case 4:return *(ret_type*)&m_v4;
				case 5:return *(ret_type*)&m_v5;case 6:return *(ret_type*)&m_v6;case 7:return *(ret_type*)&m_v7;case 8:return *(ret_type*)&m_v8;case 9:return *(ret_type*)&m_v9;
				case 10:return *(ret_type*)&m_v10;case 11:return *(ret_type*)&m_v11;case 12:return *(ret_type*)&m_v12;case 13:return *(ret_type*)&m_v13;
				case 14:return *(ret_type*)&m_v14;default:return *(ret_type*)0;
			}
		}
	};

	template<class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15>
	class tuple_t
	{
		T0 m_v0; T1 m_v1; T2 m_v2; T3 m_v3; T4 m_v4; T5 m_v5; T6 m_v6; T7 m_v7;  T8 m_v8;  T9 m_v9; T10 m_v10; T11 m_v11; T12 m_v12; T13 m_v13; T14 m_v14; T15 m_v15;
		template<int, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename, typename>
		friend struct tuple_helper;							//������������Ϊ��Ԫ
	public:
		tuple_t(const T0& v0, const T1& v1, const T2& v2, const T3& v3, const T4 &v4, const T5 &v5, const T6 &v6, const T7 &v7, const T8 &v8, const T9 &v9, const T10 &v10, const T11 &v11, const T12 &v12, const T13 &v13, const T14 &v14, const T15 &v15) :m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4), m_v5(v5), m_v6(v6), m_v7(v7), m_v8(v8), m_v9(v9), m_v10(v10), m_v11(v11), m_v12(v12), m_v13(v13), m_v14(v14), m_v15(v15) {}
		static const uint32_t size = 16;
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type & get() {
			return tuple_helper<K, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15>::get(this);
		}
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type const &get() const {
			return tuple_helper<K, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15>::get(this);
		}
		template<typename ret_type>
		inline ret_type& take(uint32_t idx) {
			switch (idx) {
				case 0:return *(ret_type*)&m_v0;case 1:return *(ret_type*)&m_v1;case 2:return *(ret_type*)&m_v2;case 3:return *(ret_type*)&m_v3;case 4:return *(ret_type*)&m_v4;
				case 5:return *(ret_type*)&m_v5;case 6:return *(ret_type*)&m_v6;case 7:return *(ret_type*)&m_v7;case 8:return *(ret_type*)&m_v8;case 9:return *(ret_type*)&m_v9;
				case 10:return *(ret_type*)&m_v10;case 11:return *(ret_type*)&m_v11;case 12:return *(ret_type*)&m_v12;case 13:return *(ret_type*)&m_v13;
				case 14:return *(ret_type*)&m_v14;case 15:return *(ret_type*)&m_v15;default:return *(ret_type*)0;
			}
		}
	};
	//-----------------------------------------------------
	//�﷨��,��ȡԪ����ָ��������Ӧ��ֵ
	//-----------------------------------------------------
	template< uint32_t K, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15>
	inline typename tuple_alt< K, tuple_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> >::type &
		get(tuple_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> & v)
	{
		rx_assert(K < v.size);
		return v.template get<K>();
	}

	template< uint32_t K, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15>
	inline typename tuple_alt< K, tuple_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> >::type const &
		get(tuple_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & v)
	{
		rx_assert(K < v.size);
		return v.template get<K>();
	}

	//��̬��ȡָ��Ԫ���ָ����ŵ�Ԫ��ֵ(��ע��Ĭ�Ϸ���ֵ�ͱ�).
	template<class tuple_type, class ret_type = uint32_t>
	inline ret_type& take(tuple_type& tuple, uint32_t idx)
	{
		return tuple.template take<ret_type>(idx);
	}
	//-----------------------------------------------------
	//�﷨��,����Ԫ�����ı�ݺ���
	//-----------------------------------------------------
	template< class T0>
	inline tuple_t<T0>
		make_tuple(const T0 &v0) { return tuple_t<T0>(v0); }

	template< class T0, class T1>
	inline tuple_t<T0, T1>
		make_tuple(const T0 &v0, const T1 &v1) { return tuple_t<T0, T1>(v0, v1); }

	template< class T0, class T1, class T2>
	inline tuple_t<T0, T1, T2>
		make_tuple(const T0 &v0, const T1 &v1, const T2 &v2) { return tuple_t<T0, T1, T2>(v0, v1, v2); }

	template< class T0, class T1, class T2, class T3>
	inline tuple_t<T0, T1, T2, T3>
		make_tuple(const T0 &v0, const T1 &v1, const T2 &v2, const T3 &v3) { return tuple_t<T0, T1, T2, T3>(v0, v1, v2, v3); }

	template< class T0, class T1, class T2, class T3, class T4>
	inline tuple_t<T0, T1, T2, T3, T4>
		make_tuple(const T0 &v0, const T1 &v1, const T2 &v2, const T3 &v3, const T4 &v4) { return tuple_t<T0, T1, T2, T3, T4>(v0, v1, v2, v3, v4); }

	template< class T0, class T1, class T2, class T3, class T4, class T5>
	inline tuple_t<T0, T1, T2, T3, T4, T5>
		make_tuple(const T0 &v0, const T1 &v1, const T2 &v2, const T3 &v3, const T4 &v4, const T5 &v5) { return tuple_t<T0, T1, T2, T3, T4, T5>(v0, v1, v2, v3, v4, v5); }

	template< class T0, class T1, class T2, class T3, class T4, class T5, class T6>
	inline tuple_t<T0, T1, T2, T3, T4, T5, T6>
		make_tuple(const T0 &v0, const T1 &v1, const T2 &v2, const T3 &v3, const T4 &v4, const T5 &v5, const T6 &v6) { return tuple_t<T0, T1, T2, T3, T4, T5, T6>(v0, v1, v2, v3, v4, v5, v6); }

	template< class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
	inline tuple_t<T0, T1, T2, T3, T4, T5, T6, T7>
		make_tuple(const T0 &v0, const T1 &v1, const T2 &v2, const T3 &v3, const T4 &v4, const T5 &v5, const T6 &v6, const T7 &v7) { return tuple_t<T0, T1, T2, T3, T4, T5, T6, T7>(v0, v1, v2, v3, v4, v5, v6, v7); }

	template< class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
	inline tuple_t<T0, T1, T2, T3, T4, T5, T6, T7, T8>
		make_tuple(const T0 &v0, const T1 &v1, const T2 &v2, const T3 &v3, const T4 &v4, const T5 &v5, const T6 &v6, const T7 &v7, const T8 &v8) { return tuple_t<T0, T1, T2, T3, T4, T5, T6, T7, T8>(v0, v1, v2, v3, v4, v5, v6, v7, v8); }

	template< class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
	inline tuple_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>
		make_tuple(const T0 &v0, const T1 &v1, const T2 &v2, const T3 &v3, const T4 &v4, const T5 &v5, const T6 &v6, const T7 &v7, const T8 &v8, const T9 &v9) { return tuple_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9); }

	template< class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
	inline tuple_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>
		make_tuple(const T0 &v0, const T1 &v1, const T2 &v2, const T3 &v3, const T4 &v4, const T5 &v5, const T6 &v6, const T7 &v7, const T8 &v8, const T9 &v9, const T10 &v10) { return tuple_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10); }

	template< class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11>
	inline tuple_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11>
		make_tuple(const T0 &v0, const T1 &v1, const T2 &v2, const T3 &v3, const T4 &v4, const T5 &v5, const T6 &v6, const T7 &v7, const T8 &v8, const T9 &v9, const T10 &v10, const T11 &v11) { return tuple_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11>(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11); }

	template< class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12>
	inline tuple_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12>
		make_tuple(const T0 &v0, const T1 &v1, const T2 &v2, const T3 &v3, const T4 &v4, const T5 &v5, const T6 &v6, const T7 &v7, const T8 &v8, const T9 &v9, const T10 &v10, const T11 &v11, const T12 &v12) { return tuple_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12>(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12); }

	template< class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13>
	inline tuple_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13>
		make_tuple(const T0 &v0, const T1 &v1, const T2 &v2, const T3 &v3, const T4 &v4, const T5 &v5, const T6 &v6, const T7 &v7, const T8 &v8, const T9 &v9, const T10 &v10, const T11 &v11, const T12 &v12, const T13 &v13) { return tuple_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13>(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13); }

	template< class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14>
	inline tuple_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14>
		make_tuple(const T0 &v0, const T1 &v1, const T2 &v2, const T3 &v3, const T4 &v4, const T5 &v5, const T6 &v6, const T7 &v7, const T8 &v8, const T9 &v9, const T10 &v10, const T11 &v11, const T12 &v12, const T13 &v13, const T14 &v14) { return tuple_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14>(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14); }

	template< class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15>
	inline tuple_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15>
		make_tuple(const T0 &v0, const T1 &v1, const T2 &v2, const T3 &v3, const T4 &v4, const T5 &v5, const T6 &v6, const T7 &v7, const T8 &v8, const T9 &v9, const T10 &v10, const T11 &v11, const T12 &v12, const T13 &v13, const T14 &v14, const T15 &v15) { return tuple_t<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15>(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15); }

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
		//ģ�庯��ƫ�ػ�,���ڴ�������Ԫ������
		template<typename Functor, typename tuple_type, typename index_type>
		inline void m_each(Functor& fun, const tuple_type& tuple, index_type)
		{
			const uint32_t cur = (tuple_type::size - index_type::value);	//��ǰ������Ԫ������
			fun(cur, tuple_type::size, tuple.template get<cur>());			//��Ŀ��º�������ǰԪ��Ԫ�صĵ��ô���,ͬʱ����ѭ������������,���ڴ������
			m_each(fun, tuple, ct_index_t<index_type::value - 1>());		//֮��ѭ�������ݼ�,�ݹ������һ�ִ�����
		}

		//Ԫ���������ʱ,ѭ��������Чʱ���ػ�����,ɶҲ����
		template<typename Functor, typename tuple_type>
		inline void m_each(Functor, const tuple_type& tuple, ct_index_t<0>) {}
	};

}
#endif
