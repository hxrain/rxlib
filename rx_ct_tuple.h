#ifndef _RX_CT_TUPLE_H_
#define _RX_CT_TUPLE_H_
#include "rx_cc_macro.h"
#include "rx_ct_typelist.h"
#include "rx_assert.h"

//Reference https://github.com/jabernet/tuple99
//参考实现超轻量级的元组功能.

namespace rx
{
	//-----------------------------------------------------
	//轻量级元组类原型(最多5个值参数)
	//-----------------------------------------------------
	template<class T0, class T1 = ct_nulltype, class T2 = ct_nulltype, class T3 = ct_nulltype, class T4 = ct_nulltype>
	class tuple_t;

	//判断某个型别是否为元组
	template <typename T>
	struct is_tuple { static const bool value = false; };
	template<typename T0, typename T1, typename T2, typename T3, typename T4>
	struct is_tuple<tuple_t<T0, T1, T2, T3, T4> > { static const bool value = true; };

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
	//用于规避g++模板类的模板函数特化标准化,引入统一访问成员变量助理模板
	//-----------------------------------------------------
	template<int index, class H0, class H1, class H2, class H3, class H4>
	struct tuple_helper;

	template<class H0, class H1, class H2, class H3, class H4>
	struct tuple_helper<0, H0, H1, H2, H3, H4>				//助理型别进行索引值的静态特化处理,用于区分具体访问哪个元素
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
	//元组类偏特化实现,让每个有效型别仅含有对应的有效数据.
	//-----------------------------------------------------
	template<class T0>
	class tuple_t<T0, ct_nulltype, ct_nulltype, ct_nulltype, ct_nulltype>
	{
		T0	m_v0;											//定义真实有效的数据成员
		template<int, typename, typename, typename, typename, typename>
		friend struct tuple_helper;							//描述助理类型为友元
	public:
		tuple_t(const T0& v0) :m_v0(v0) {}					//构造的时候绑定具体型别的值
		static const uint32_t size = 1;						//告知本元组的有效元素数量
		//使用助理类型获取指定索引的元素
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
	//语法糖,获取元组中指定索引对应的值
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
	//语法糖,生成元组对象的便捷函数
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
	//元组遍历器
	//-----------------------------------------------------
	class tuple_foreach
	{
	public:
		//使用给定的目标仿函数对象进行元组的遍历
		template<typename tuple_type, typename Functor>
		tuple_foreach(const tuple_type& tuple, const Functor& fun)
		{
			m_each(*(typename ct_remove_const<Functor>::type *)(&fun), tuple, ct_index_t<tuple_type::size>());
		}
	private:
		//用于包装元组遍历的循环变量
		template<int pos>
		struct ct_index_t { static const int value = pos; };

		//模板函数偏特化,用于处理具体的元组数据
		template<typename Functor, typename tuple_type, typename index_type>
		inline void m_each(Functor& fun, const tuple_type& tuple, index_type);

		template<typename Functor, typename tuple_type>
		inline void m_each(Functor, const tuple_type& tuple, ct_index_t<0>);
	};

	//元组遍历处理时,循环变量有效时的特化函数
	template<typename Functor, typename tuple_type, typename index_type>
	inline void tuple_foreach::m_each(Functor& fun, const tuple_type& tuple, index_type)
	{
		static const uint32_t cur = (tuple_type::size - index_type::value);	//当前待处理元素索引
		fun(cur, tuple_type::size, tuple.template get<cur>());			//对目标仿函数发起当前元组元素的调用处理,同时传递循环变量和总数,便于处理进度
		m_each(fun, tuple, ct_index_t<index_type::value - 1>());		//之后循环变量递减,递归调用下一轮处理函数
	}

	//元组遍历处理时,循环变量无效时的特化函数,啥也不干
	template<typename Functor, typename tuple_type>
	inline void tuple_foreach::m_each(Functor, const tuple_type&, ct_index_t<0>) {}
}
#endif
