#ifndef _RX_CT_TYPELIST_H_
#define _RX_CT_TYPELIST_H_

namespace rx
{
	//-----------------------------------------------------
	//内部占位使用的空类型
	struct ct_nulltype {};

	//-----------------------------------------------------
	// ct_typelist 型别列表基础描述
	template< class Head, class Tail >
	struct ct_typelist
	{
		typedef Head head;									//列表头
		typedef Tail tail;									//列表尾
	};

	//-----------------------------------------------------
	//对型别列表按参数个数进行逐一嵌套描述
	#define ct_typelist_make1( T1 ) ct_typelist< T1, ct_nulltype >							//一个型别参数的型别列表,列表尾使用占位的空类型
	#define ct_typelist_make2( T1, T2) ct_typelist< T1, ct_typelist_make1( T2) >			//两个型别参数的型别列表,列表尾使用嵌套的单参数型别列表
	#define ct_typelist_make3( T1, T2, T3) ct_typelist< T1, ct_typelist_make2( T2, T3) >	//三个型别参数的型别列表,列表尾使用嵌套的双参数型别列表
	#define ct_typelist_make4( T1, T2, T3, T4) ct_typelist< T1, ct_typelist_make3( T2, T3, T4) >
	#define ct_typelist_make5( T1, T2, T3, T4, T5) ct_typelist< T1, ct_typelist_make4( T2, T3, T4, T5) >
	#define ct_typelist_make6( T1, T2, T3, T4, T5, T6) ct_typelist< T1, ct_typelist_make5( T2, T3, T4, T5, T6) >
	#define ct_typelist_make7( T1, T2, T3, T4, T5, T6, T7) ct_typelist< T1, ct_typelist_make6( T2, T3, T4, T5, T6, T7) >
	#define ct_typelist_make8( T1, T2, T3, T4, T5, T6, T7, T8) ct_typelist< T1, ct_typelist_make7( T2, T3, T4, T5, T6, T7, T8) >
	#define ct_typelist_make9( T1, T2, T3, T4, T5, T6, T7, T8, T9) ct_typelist< T1, ct_typelist_make8( T2, T3, T4, T5, T6, T7, T8, T9) >
	#define ct_typelist_make10( T1, T2, T3, T4, T5, T6, T7, T8, T9, T10) ct_typelist< T1, ct_typelist_make9( T2, T3, T4, T5, T6, T7, T8, T9, T10) >
	#define ct_typelist_make11( T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11) ct_typelist< T1, ct_typelist_make10( T2, T3, T4, T5, T6, T7, T8, T9, T10, T11) >
	#define ct_typelist_make12( T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12) ct_typelist< T1, ct_typelist_make11( T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12) >
	#define ct_typelist_make13( T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13) ct_typelist< T1, ct_typelist_make12( T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13) >
	#define ct_typelist_make14( T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14) ct_typelist< T1, ct_typelist_make13( T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14) >
	#define ct_typelist_make15( T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15) ct_typelist< T1, ct_typelist_make14( T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15) >
	#define ct_typelist_make16( T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16) ct_typelist< T1, ct_typelist_make15( T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16) >

	//-----------------------------------------------------
	//定义型别参数占位使用的默认参数类.
	typedef struct {} ct_dummy_T1;
	typedef struct {} ct_dummy_T2;
	typedef struct {} ct_dummy_T3;
	typedef struct {} ct_dummy_T4;
	typedef struct {} ct_dummy_T5;
	typedef struct {} ct_dummy_T6;
	typedef struct {} ct_dummy_T7;
	typedef struct {} ct_dummy_T8;
	typedef struct {} ct_dummy_T9;
	typedef struct {} ct_dummy_T10;
	typedef struct {} ct_dummy_T11;
	typedef struct {} ct_dummy_T12;
	typedef struct {} ct_dummy_T13;
	typedef struct {} ct_dummy_T14;
	typedef struct {} ct_dummy_T15;

	//-----------------------------------------------------
	// 筛选型别列表中的最大型别尺寸值与对应类型
	template< class List >
	struct ct_typelist_max;

	template<>
	struct ct_typelist_max< ct_nulltype >
	{
		enum V { value = 0 };								//偏特化筛选空类型,列表结束,0长度
		typedef void type;
	};

	template< class Head, class Tail >						//偏特化筛选型别列表,翻底迭代运算,得到尺寸最大的型别与对应的尺寸
	struct ct_typelist_max< ct_typelist<Head, Tail> >
	{
	private:
		enum _ { tail_value = size_t(ct_typelist_max<Tail>::value) };		//嵌套引用尾节点型别,得到尾节点型别尺寸

		typedef typename ct_typelist_max<Tail>::type tail_type;				//嵌套引用尾节点型别,得到按尺寸大小筛选过的未节点型别输出

	public:
		enum V { value = (sizeof(Head) > tail_value) ? sizeof(Head) : size_t(tail_value) };			//进行头尾节点尺寸筛选,保留尺寸大的

		typedef typename ct_cond_pick< (sizeof(Head) > tail_value), Head, tail_type>::type type;	//根据当前头尾节点尺寸大小,选取大尺寸对应型别为输出型别
	};

	//-----------------------------------------------------
	// 筛选型别列表中指定型别的顺序索引(按型别查序号)
	template< class List, class T >
	struct ct_typelist_of;

	template< class T >
	struct ct_typelist_of< ct_nulltype, T >
	{
		enum V { value = -1 };								//型别T不在型别列表中,索引为-1
	};

	template< class Tail, class T >
	struct ct_typelist_of< ct_typelist<T, Tail>, T >
	{
		enum V { value = 0 };								//如果型别T与指定的尾型别相同,索引为0
	};

	template< class Head, class Tail, class T >
	struct ct_typelist_of< ct_typelist<Head, Tail>, T >
	{
	private:
		enum _ { nextVal = ct_typelist_of<Tail, T>::value };//嵌套引用尾节点型别和指定型别T,翻底查询嵌套深度

	public:
		enum V { value = nextVal == -1 ? -1 : 1 + nextVal };//根据嵌套深度得到最终结果:型别T在型别列表中的顺序位置
	};

	//-----------------------------------------------------
	//提取型别列表中指定索引顺序的型别(按序号查型别)
	template< class List, size_t i>
	struct ct_typelist_at;

	template< class Head, class Tail >
	struct ct_typelist_at< ct_typelist<Head, Tail>, 0 >
	{
		typedef Head type;									//列表顺序索引为0,则使用头型别
	};

	template< class Head, class Tail, size_t i >
	struct ct_typelist_at< ct_typelist<Head, Tail>, i >
	{
		typedef typename ct_typelist_at<Tail, i - 1>::type type;	//嵌套引用尾节点型别和递减的顺序索引,翻底查询嵌套深度对应的头型别作为输出
	};

}

#endif