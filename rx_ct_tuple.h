#ifndef _RX_CT_TUPLE_H_
#define _RX_CT_TUPLE_H_
#include "rx_cc_macro.h"
#include "rx_ct_typelist.h"
#include "rx_assert.h"

namespace rx
{
	//-----------------------------------------------------
	//轻量级元组类原型(最多5个值参数)
	template<class T0, class T1 = ct_nulltype, class T2 = ct_nulltype, class T3 = ct_nulltype, class T4 = ct_nulltype>
	class tuple_t;

	//-----------------------------------------------------
	// 获取具体元组指定型别顺序对应的真实型别
	template< uint32_t K, class T >
	struct tuple_alt;

	template< uint32_t K, class T0, class T1, class T2, class T3, class T4>
	struct tuple_alt< K, tuple_t<T0, T1, T2, T3, T4> >
	{
		typedef typename ct_typelist_at<ct_typelist_make5(T0, T1, T2, T3, T4), K>::type type;
	};

	//-----------------------------------------------------
	//元组类偏特化实现
	template<class T0>
	class tuple_t<T0, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>
	{
		T0	m_v0;
		static const uint32_t m_size = 1;
		typedef ct_typelist_make1(T0)	typelist_t;
	public:
		tuple_t(const T0& v0) :m_v0(v0) {}
		uint32_t size() const { return m_size; }
		//-------------------------------------------------
		// 按指定型别索引获取元组的值
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type & get() { rx_alert("tuple index bad."); }
		template<> typename tuple_alt< 0, tuple_t >::type & get<0>() { return m_v0; }

		template< uint32_t K > typename tuple_alt< K, tuple_t >::type const &get() const { rx_alert("tuple index bad."); }
		template<> typename tuple_alt< 0, tuple_t >::type const &get<0>() const { return m_v0; }
	};

	template<class T0, class T1>
	class tuple_t<T0, T1, ct_nulltype, ct_nulltype, ct_nulltype>
	{
		T0 m_v0; T1 m_v1;
		static const uint32_t m_size = 2;
		typedef ct_typelist_make2(T0, T1)	typelist_t;
	public:
		tuple_t(const T0& v0, const T1& v1) :m_v0(v0), m_v1(v1) {}
		uint32_t size() const { return m_size; }
		//-------------------------------------------------
		// 按指定型别索引获取元组的值
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type & get() { rx_alert("tuple index bad."); }
		template<> typename tuple_alt< 0, tuple_t >::type & get<0>() { return m_v0; }
		template<> typename tuple_alt< 1, tuple_t >::type & get<1>() { return m_v1; }

		template< uint32_t K > typename tuple_alt< K, tuple_t >::type const &get() const { rx_alert("tuple index bad."); }
		template<> typename tuple_alt< 0, tuple_t >::type const &get<0>() const { return m_v0; }
		template<> typename tuple_alt< 1, tuple_t >::type const &get<1>() const { return m_v1; }
	};

	template<class T0, class T1, class T2>
	class tuple_t<T0, T1, T2, ct_nulltype, ct_nulltype>
	{
		T0 m_v0; T1 m_v1;T2	m_v2;
		static const uint32_t m_size = 3;
		typedef ct_typelist_make3(T0, T1, T2)	typelist_t;
	public:
		tuple_t(const T0& v0, const T1& v1, const T2& v2) :m_v0(v0), m_v1(v1), m_v2(v2) {}
		uint32_t size() const { return m_size; }
		//-------------------------------------------------
		// 按指定型别索引获取元组的值
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type & get() { rx_alert("tuple index bad."); }
		template<> typename tuple_alt< 0, tuple_t >::type & get<0>() { return m_v0; }
		template<> typename tuple_alt< 1, tuple_t >::type & get<1>() { return m_v1; }
		template<> typename tuple_alt< 2, tuple_t >::type & get<2>() { return m_v2; }

		template< uint32_t K > typename tuple_alt< K, tuple_t >::type const &get() const { rx_alert("tuple index bad."); }
		template<> typename tuple_alt< 0, tuple_t >::type const &get<0>() const { return m_v0; }
		template<> typename tuple_alt< 1, tuple_t >::type const &get<1>() const { return m_v1; }
		template<> typename tuple_alt< 2, tuple_t >::type const &get<2>() const { return m_v2; }
	};

	template<class T0, class T1, class T2, class T3>
	class tuple_t<T0, T1, T2, T3, ct_nulltype>
	{
		T0 m_v0; T1 m_v1; T2 m_v2; T3 m_v3;
		static const uint32_t m_size = 4;
		typedef ct_typelist_make4(T0, T1, T2, T3)	typelist_t;
	public:
		tuple_t(const T0& v0, const T1& v1, const T2& v2, const T3& v3) :m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3) {}
		uint32_t size() const { return m_size; }
		//-------------------------------------------------
		// 按指定型别索引获取元组的值
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type & get() { rx_alert("tuple index bad."); }
		template<> typename tuple_alt< 0, tuple_t >::type & get<0>() { return m_v0; }
		template<> typename tuple_alt< 1, tuple_t >::type & get<1>() { return m_v1; }
		template<> typename tuple_alt< 2, tuple_t >::type & get<2>() { return m_v2; }
		template<> typename tuple_alt< 3, tuple_t >::type & get<3>() { return m_v3; }

		template< uint32_t K > typename tuple_alt< K, tuple_t >::type const &get() const { rx_alert("tuple index bad."); }
		template<> typename tuple_alt< 0, tuple_t >::type const &get<0>() const { return m_v0; }
		template<> typename tuple_alt< 1, tuple_t >::type const &get<1>() const { return m_v1; }
		template<> typename tuple_alt< 2, tuple_t >::type const &get<2>() const { return m_v2; }
		template<> typename tuple_alt< 3, tuple_t >::type const &get<3>() const { return m_v3; }
	};

	template<class T0, class T1, class T2, class T3, class T4>
	class tuple_t
	{
		T0 m_v0; T1 m_v1; T2 m_v2; T3 m_v3; T4 m_v4;
		static const uint32_t m_size = 5;
		typedef ct_typelist_make5(T0, T1, T2, T3, T4)	typelist_t;
	public:
		tuple_t(const T0& v0, const T1& v1, const T2& v2, const T3& v3, const T4 &v4) :m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3), m_v4(v4) {}
		uint32_t size() const { return m_size; }
		//-------------------------------------------------
		// 按指定型别索引获取元组的值
		template< uint32_t K > typename tuple_alt< K, tuple_t >::type & get() { rx_alert("tuple index bad."); }
		template<> typename tuple_alt< 0, tuple_t >::type & get<0>() { return m_v0; }
		template<> typename tuple_alt< 1, tuple_t >::type & get<1>() { return m_v1; }
		template<> typename tuple_alt< 2, tuple_t >::type & get<2>() { return m_v2; }
		template<> typename tuple_alt< 3, tuple_t >::type & get<3>() { return m_v3; }
		template<> typename tuple_alt< 4, tuple_t >::type & get<4>() { return m_v4; }

		template< uint32_t K > typename tuple_alt< K, tuple_t >::type const &get() const { rx_alert("tuple index bad."); }
		template<> typename tuple_alt< 0, tuple_t >::type const &get<0>() const { return m_v0; }
		template<> typename tuple_alt< 1, tuple_t >::type const &get<1>() const { return m_v1; }
		template<> typename tuple_alt< 2, tuple_t >::type const &get<2>() const { return m_v2; }
		template<> typename tuple_alt< 3, tuple_t >::type const &get<3>() const { return m_v3; }
		template<> typename tuple_alt< 4, tuple_t >::type const &get<4>() const { return m_v4; }
	};

	//-----------------------------------------------------
	//获取指定索引对应的值
	template< uint32_t K, class T0, class T1, class T2, class T3, class T4>
	inline typename tuple_alt< K, tuple_t<T0, T1, T2, T3, T4> >::type &
		get(tuple_t<T0, T1, T2, T3, T4> & v)
	{
		rx_assert(K <= v.size());
		return v.template get<K>();
	}

	template< uint32_t K, class T0, class T1, class T2, class T3, class T4>
	inline typename tuple_alt< K, tuple_t<T0, T1, T2, T3, T4> >::type const &
		get(tuple_t<T0, T1, T2, T3, T4> const & v)
	{
		rx_assert(K <= v.size());
		return v.template get<K>();
	}

	//-----------------------------------------------------
	//生成元组对象
	template< class T0>
	inline typename tuple_t<T0>	make_tuple(const T0 &v0) { return tuple_t<T0>(v0); }
	template< class T0, class T1>
	inline typename tuple_t<T0, T1>	make_tuple(const T0 &v0, const T1 &v1) { return tuple_t<T0, T1>(v0, v1); }
	template< class T0, class T1, class T2>
	inline typename tuple_t<T0, T1, T2>	make_tuple(const T0 &v0, const T1 &v1, const T2 &v2) { return tuple_t<T0, T1, T2>(v0, v1, v2); }
	template< class T0, class T1, class T2, class T3>
	inline typename tuple_t<T0, T1, T2, T3>	make_tuple(const T0 &v0, const T1 &v1, const T2 &v2, const T3 &v3) { return tuple_t<T0, T1, T2, T3>(v0, v1, v2, v3); }
	template< class T0, class T1, class T2, class T3, class T4>
	inline typename tuple_t<T0, T1, T2, T3, T4>	make_tuple(const T0 &v0, const T1 &v1, const T2 &v2, const T3 &v3, const T4 &v4) { return tuple_t<T0, T1, T2, T3, T4>(v0, v1, v2, v3, v4); }
}
#endif
