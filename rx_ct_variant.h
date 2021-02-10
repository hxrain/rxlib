#ifndef _RX_CT_VARIANT_H_
#define _RX_CT_VARIANT_H_
//reference https://github.com/martinmoene/variant-lite

#include <assert.h>
#include <new>


namespace rx {

	// C++11 emulation:
	/*
	//型别操作:丢弃引用型别,保留原始型别
	template< class T > struct remove_reference { typedef T type; };
	template< class T > struct remove_reference<T&> { typedef T type; };

	//型别操作:基于非指针型别定义指针型别
	template< class T > struct add_pointer
	{
	typedef typename remove_reference<T>::type * type;
	};

	*/
	namespace std11 {
		//型别操作:丢弃const描述,保留原始型别
		template< class T > struct remove_const { typedef T type; };
		template< class T > struct remove_const<const T> { typedef T type; };

		//型别操作:丢弃volatile描述,保留原始型别
		template< class T > struct remove_volatile { typedef T type; };
		template< class T > struct remove_volatile<volatile T> { typedef T type; };

		//型别操作:丢弃const和volatile描述,保留原始型别
		template< class T >
		struct remove_cv
		{
			typedef typename remove_volatile<typename remove_const<T>::type>::type type;
		};

		//型别条件判断,根据Cond条件的真假,决定选取Then或Else型别输出
		template< bool Cond, class Then, class Else >
		struct ct_cond;

		template< class Then, class Else >
		struct ct_cond< true, Then, Else > { typedef Then type; };

		template< class Then, class Else >
		struct ct_cond< false, Then, Else > { typedef Else type; };
	} // namespace std11

	//未知的型别列表索引顺序
	static const std::size_t bad_type_index = static_cast<std::size_t>(-1);

	namespace detail {

		//内部占位使用的空类型
		struct nulltype {};

		// typelist 型别列表基础描述
		template< class Head, class Tail >
		struct typelist
		{
			typedef Head head;	//列表头
			typedef Tail tail;	//列表尾
		};

		//对型别列表按参数个数进行逐一嵌套描述
#define variant_typelist_desc1( T1 ) detail::typelist< T1, detail::nulltype >						//一个型别参数的型别列表,列表尾使用占位的空类型
#define variant_typelist_desc2( T1, T2) detail::typelist< T1, variant_typelist_desc1( T2) >			//两个型别参数的型别列表,列表尾使用嵌套的单参数型别列表
#define variant_typelist_desc3( T1, T2, T3) detail::typelist< T1, variant_typelist_desc2( T2, T3) >	//三个型别参数的型别列表,列表尾使用嵌套的双参数型别列表
#define variant_typelist_desc4( T1, T2, T3, T4) detail::typelist< T1, variant_typelist_desc3( T2, T3, T4) >
#define variant_typelist_desc5( T1, T2, T3, T4, T5) detail::typelist< T1, variant_typelist_desc4( T2, T3, T4, T5) >
#define variant_typelist_desc6( T1, T2, T3, T4, T5, T6) detail::typelist< T1, variant_typelist_desc5( T2, T3, T4, T5, T6) >
#define variant_typelist_desc7( T1, T2, T3, T4, T5, T6, T7) detail::typelist< T1, variant_typelist_desc6( T2, T3, T4, T5, T6, T7) >
#define variant_typelist_desc8( T1, T2, T3, T4, T5, T6, T7, T8) detail::typelist< T1, variant_typelist_desc7( T2, T3, T4, T5, T6, T7, T8) >
#define variant_typelist_desc9( T1, T2, T3, T4, T5, T6, T7, T8, T9) detail::typelist< T1, variant_typelist_desc8( T2, T3, T4, T5, T6, T7, T8, T9) >
#define variant_typelist_desc10( T1, T2, T3, T4, T5, T6, T7, T8, T9, T10) detail::typelist< T1, variant_typelist_desc9( T2, T3, T4, T5, T6, T7, T8, T9, T10) >
#define variant_typelist_desc11( T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11) detail::typelist< T1, variant_typelist_desc10( T2, T3, T4, T5, T6, T7, T8, T9, T10, T11) >
#define variant_typelist_desc12( T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12) detail::typelist< T1, variant_typelist_desc11( T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12) >
#define variant_typelist_desc13( T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13) detail::typelist< T1, variant_typelist_desc12( T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13) >
#define variant_typelist_desc14( T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14) detail::typelist< T1, variant_typelist_desc13( T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14) >
#define variant_typelist_desc15( T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15) detail::typelist< T1, variant_typelist_desc14( T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15) >
#define variant_typelist_desc16( T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16) detail::typelist< T1, variant_typelist_desc15( T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16) >

			//---------------------------------------------
			//定义型别参数占位使用的默认参数类.
		typedef struct {} dummy_T1;
		typedef struct {} dummy_T2;
		typedef struct {} dummy_T3;
		typedef struct {} dummy_T4;
		typedef struct {} dummy_T5;
		typedef struct {} dummy_T6;
		typedef struct {} dummy_T7;
		typedef struct {} dummy_T8;
		typedef struct {} dummy_T9;
		typedef struct {} dummy_T10;
		typedef struct {} dummy_T11;
		typedef struct {} dummy_T12;
		typedef struct {} dummy_T13;
		typedef struct {} dummy_T14;
		typedef struct {} dummy_T15;

		//---------------------------------------------
		// 筛选型别列表中的最大型别尺寸值与对应类型
		template< class List >
		struct typelist_max;

		template<>
		struct typelist_max< nulltype >
		{
			enum V { value = 0 };						//偏特化筛选空类型,列表结束,0长度
			typedef void type;
		};

		template< class Head, class Tail >				//偏特化筛选型别列表,翻底迭代运算,得到尺寸最大的型别与对应的尺寸
		struct typelist_max< typelist<Head, Tail> >
		{
		private:
			enum _ { tail_value = size_t(typelist_max<Tail>::value) };	//嵌套引用尾节点型别,得到尾节点型别尺寸

			typedef typename typelist_max<Tail>::type tail_type;		//嵌套引用尾节点型别,得到按尺寸大小筛选过的未节点型别输出

		public:
			enum V { value = (sizeof(Head) > tail_value) ? sizeof(Head) : std::size_t(tail_value) };		//进行头尾节点尺寸筛选,保留尺寸大的

			typedef typename std11::ct_cond< (sizeof(Head) > tail_value), Head, tail_type>::type type;	//根据当前头尾节点尺寸大小,选取大尺寸对应型别为输出型别
		};

		//---------------------------------------------
		// 筛选型别列表中指定型别的顺序索引
		template< class List, class T >
		struct typelist_index_of;

		template< class T >
		struct typelist_index_of< nulltype, T >
		{
			enum V { value = -1 };										//型别T不在型别列表中,索引为-1
		};

		template< class Tail, class T >
		struct typelist_index_of< typelist<T, Tail>, T >
		{
			enum V { value = 0 };										//如果型别T与指定的尾型别相同,索引为0
		};

		template< class Head, class Tail, class T >
		struct typelist_index_of< typelist<Head, Tail>, T >
		{
		private:
			enum _ { nextVal = typelist_index_of<Tail, T>::value };		//嵌套引用尾节点型别和指定型别T,翻底查询嵌套深度

		public:
			enum V { value = nextVal == -1 ? -1 : 1 + nextVal };		//根据嵌套深度得到最终结果:型别T在型别列表中的顺序位置
		};


		//---------------------------------------------
		//提取型别列表中指定索引顺序的型别
		template< class List, std::size_t i>
		struct typelist_type_at;

		template< class Head, class Tail >
		struct typelist_type_at< typelist<Head, Tail>, 0 >
		{
			typedef Head type;											//列表顺序索引为0,则使用头型别
		};

		template< class Head, class Tail, std::size_t i >
		struct typelist_type_at< typelist<Head, Tail>, i >
		{
			typedef typename typelist_type_at<Tail, i - 1>::type type;	//嵌套引用尾节点型别和递减的顺序索引,翻底查询嵌套深度对应的头型别作为输出
		};

		//---------------------------------------------
		//型别列表操作功能助理类
		template< class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 >
		struct helper
		{
			//定义型别索引类型
			typedef signed char type_index_t;
			//定义型别列表类型
			typedef variant_typelist_desc16(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15) variant_types;

			//强制进行数据缓冲区指针的型别转换
			template< class U >
			static U * as(void * data)
			{
				return reinterpret_cast<U*>(data);
			}

			template< class U >
			static U const * as(void const * data)
			{
				return reinterpret_cast<const U*>(data);
			}

			//根据型别索引序号,析构对应型别数据
			static void destroy(type_index_t index, void * data)
			{
				switch (index)
				{
				case 0: as<T0>(data)->~T0(); break;
				case 1: as<T1>(data)->~T1(); break;
				case 2: as<T2>(data)->~T2(); break;
				case 3: as<T3>(data)->~T3(); break;
				case 4: as<T4>(data)->~T4(); break;
				case 5: as<T5>(data)->~T5(); break;
				case 6: as<T6>(data)->~T6(); break;
				case 7: as<T7>(data)->~T7(); break;
				case 8: as<T8>(data)->~T8(); break;
				case 9: as<T9>(data)->~T9(); break;
				case 10: as<T10>(data)->~T10(); break;
				case 11: as<T11>(data)->~T11(); break;
				case 12: as<T12>(data)->~T12(); break;
				case 13: as<T13>(data)->~T13(); break;
				case 14: as<T14>(data)->~T14(); break;
				case 15: as<T15>(data)->~T15(); break;
				default:assert(0);
				}
			}

			//进行指定型别顺序的拷贝构造(在目标缓冲区中就地构造新对象,并使用源缓冲区数据进行赋值)
			static type_index_t copy_construct(type_index_t const from_index, const void * from_data, void * to_data)
			{
				switch (from_index)
				{
				case 0: new(to_data) T0(*as<T0>(from_data)); break;
				case 1: new(to_data) T1(*as<T1>(from_data)); break;
				case 2: new(to_data) T2(*as<T2>(from_data)); break;
				case 3: new(to_data) T3(*as<T3>(from_data)); break;
				case 4: new(to_data) T4(*as<T4>(from_data)); break;
				case 5: new(to_data) T5(*as<T5>(from_data)); break;
				case 6: new(to_data) T6(*as<T6>(from_data)); break;
				case 7: new(to_data) T7(*as<T7>(from_data)); break;
				case 8: new(to_data) T8(*as<T8>(from_data)); break;
				case 9: new(to_data) T9(*as<T9>(from_data)); break;
				case 10: new(to_data) T10(*as<T10>(from_data)); break;
				case 11: new(to_data) T11(*as<T11>(from_data)); break;
				case 12: new(to_data) T12(*as<T12>(from_data)); break;
				case 13: new(to_data) T13(*as<T13>(from_data)); break;
				case 14: new(to_data) T14(*as<T14>(from_data)); break;
				case 15: new(to_data) T15(*as<T15>(from_data)); break;
				default:assert(0);
				}
				return from_index;
			}

			//进行指定型别顺序的拷贝赋值
			static type_index_t copy_assign(type_index_t const from_index, const void * from_data, void * to_data)
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
				default:assert(0);
				}
				return from_index;
			}
		};

	} // namespace detail

	//可变量功能类原型定义
	template< class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 >
	class variant_lite;

	//-------------------------------------------------
	// 获取具体可变量指定型别顺序对应的真实型别
	template< std::size_t K, class T >
	struct variant_alt;

	template< std::size_t K, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 >
	struct variant_alt< K, variant_lite<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> >
	{
		typedef typename detail::typelist_type_at<variant_typelist_desc16(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15), K>::type type;
	};


	//-------------------------------------------------
	//variant可变量功能定义
	//-------------------------------------------------
	template<
		class T0,
		class T1 = detail::dummy_T1,
		class T2 = detail::dummy_T2,
		class T3 = detail::dummy_T3,
		class T4 = detail::dummy_T4,
		class T5 = detail::dummy_T5,
		class T6 = detail::dummy_T6,
		class T7 = detail::dummy_T7,
		class T8 = detail::dummy_T8,
		class T9 = detail::dummy_T9,
		class T10 = detail::dummy_T10,
		class T11 = detail::dummy_T11,
		class T12 = detail::dummy_T12,
		class T13 = detail::dummy_T13,
		class T14 = detail::dummy_T14,
		class T15 = detail::dummy_T15
	>
		class variant_lite
	{
		//在当前指定型别列表下的助理功能类型
		typedef detail::helper< T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15 > helper_type;
		//当前的指定型别列表
		typedef variant_typelist_desc16(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15) variant_types;

	public:
		// 构造函数,使用指定的类型进行内部缓冲区的就地构造初始化
		variant_lite() : type_index(0) { new(ptr()) T0(); }
		variant_lite(T0 const & t0) : type_index(0) { new(ptr()) T0(t0); }
		variant_lite(T1 const & t1) : type_index(1) { new(ptr()) T1(t1); }
		variant_lite(T2 const & t2) : type_index(2) { new(ptr()) T2(t2); }
		variant_lite(T3 const & t3) : type_index(3) { new(ptr()) T3(t3); }
		variant_lite(T4 const & t4) : type_index(4) { new(ptr()) T4(t4); }
		variant_lite(T5 const & t5) : type_index(5) { new(ptr()) T5(t5); }
		variant_lite(T6 const & t6) : type_index(6) { new(ptr()) T6(t6); }
		variant_lite(T7 const & t7) : type_index(7) { new(ptr()) T7(t7); }
		variant_lite(T8 const & t8) : type_index(8) { new(ptr()) T8(t8); }
		variant_lite(T9 const & t9) : type_index(9) { new(ptr()) T9(t9); }
		variant_lite(T10 const & t10) : type_index(10) { new(ptr()) T10(t10); }
		variant_lite(T11 const & t11) : type_index(11) { new(ptr()) T11(t11); }
		variant_lite(T12 const & t12) : type_index(12) { new(ptr()) T12(t12); }
		variant_lite(T13 const & t13) : type_index(13) { new(ptr()) T13(t13); }
		variant_lite(T14 const & t14) : type_index(14) { new(ptr()) T14(t14); }
		variant_lite(T15 const & t15) : type_index(15) { new(ptr()) T15(t15); }


		//使用其他variant_lite对象进行拷贝构造
		variant_lite(variant_lite const & other) : type_index(other.type_index)
		{
			(void)helper_type::copy_construct(other.type_index, other.ptr(), ptr());
		}

		//析构函数(调用助手类完成)
		~variant_lite()
		{
			if (is_valid())
			{
				helper_type::destroy(type_index, ptr());
			}
		}

		// 使用其他对象进行拷贝赋值
		variant_lite & operator=(variant_lite const & other)
		{
			return copy_assign(other);
		}

		// 使用其他型别的值进行赋值,并根据型别设定对应的型别索引
		variant_lite & operator=(T0 const & t0) { return assign_value<0>(t0); }
		variant_lite & operator=(T1 const & t1) { return assign_value<1>(t1); }
		variant_lite & operator=(T2 const & t2) { return assign_value<2>(t2); }
		variant_lite & operator=(T3 const & t3) { return assign_value<3>(t3); }
		variant_lite & operator=(T4 const & t4) { return assign_value<4>(t4); }
		variant_lite & operator=(T5 const & t5) { return assign_value<5>(t5); }
		variant_lite & operator=(T6 const & t6) { return assign_value<6>(t6); }
		variant_lite & operator=(T7 const & t7) { return assign_value<7>(t7); }
		variant_lite & operator=(T8 const & t8) { return assign_value<8>(t8); }
		variant_lite & operator=(T9 const & t9) { return assign_value<9>(t9); }
		variant_lite & operator=(T10 const & t10) { return assign_value<10>(t10); }
		variant_lite & operator=(T11 const & t11) { return assign_value<11>(t11); }
		variant_lite & operator=(T12 const & t12) { return assign_value<12>(t12); }
		variant_lite & operator=(T13 const & t13) { return assign_value<13>(t13); }
		variant_lite & operator=(T14 const & t14) { return assign_value<14>(t14); }
		variant_lite & operator=(T15 const & t15) { return assign_value<15>(t15); }

		//获取当前的型别索引
		std::size_t index() const
		{
			return to_size_t(type_index);
		}

		//判断当前的可变量是否有效
		bool is_valid() const
		{
			return type_index != (type_index_t)bad_type_index;
		}

		//将当前的可变量对象与其他的对象进行值交换
		void swap(variant_lite & other)
		{
			if (!is_valid() && other.!is_valid())
			{
				// no effect
			}
			else if (type_index == other.type_index)
			{
				this->swap_value(type_index, other);
			}
			else
			{
				variant_lite tmp(*this);
				*this = other;
				other = tmp;
			}
		}

		//获取指定的类型T在当前型别列表中的索引顺序
		template< class T >
		static  std::size_t index_of()
		{
			return to_size_t(detail::typelist_index_of<variant_types, typename std11::remove_cv<T>::type >::value);
		}

		//获取内部缓冲区的内容为指定类型的值(要求指定类型是正确的当前型别)
		template< class T >
		T & get()
		{
			const std::size_t i = index_of<T>();
			assert(i == index()); //进行目标类型的型别检查
			return *as<T>(); //将内部缓冲区指针提领转换为指定的目标类型
		}

		template< class T >
		T const & get() const
		{
			const std::size_t i = index_of<T>();
			assert(i == index());
			return *as<const T>();
		}

		// 按指定型别索引获取可变量的值
		template< std::size_t K >
		typename variant_alt< K, variant_lite >::type &
			get()
		{
			return this->get< typename detail::typelist_type_at< variant_types, K >::type >();
		}

		template< std::size_t K >
		typename variant_alt< K, variant_lite >::type const &
			get() const
		{
			return this->template get< typename detail::typelist_type_at< variant_types, K >::type >();
		}

	private:
		//使用助理功能类的型别索引类型作为最终使用的型别索引类型
		typedef typename helper_type::type_index_t type_index_t;
		//获取内部缓冲区指针
		void * ptr()
		{
			return &data;
		}

		void const * ptr() const
		{
			return &data;
		}
		//安全的转换内部缓冲区指针为指定型别指针
		template< class U >
		U * as()
		{
			return reinterpret_cast<U*>(ptr());
		}

		template< class U >
		U const * as() const
		{
			return reinterpret_cast<U const *>(ptr());
		}

		//安全的整数型别转换(语法糖,避免不同型别比较的警告)
		template< class U >
		static  std::size_t to_size_t(U index)
		{
			return static_cast<std::size_t>(index);
		}

		//用另外的varaint对象对本对象赋值
		variant_lite & copy_assign(variant_lite const & other)
		{
			if (!is_valid() && other.!is_valid())
			{
				// no effect
			}
			else if (is_valid() && other.!is_valid())
			{
				helper_type::destroy(type_index, ptr());
				type_index = (type_index_t)bad_type_index;
			}
			else if (index() == other.index())
			{
				type_index = helper_type::copy_assign(other.type_index, other.ptr(), ptr());
			}
			else
			{
				helper_type::destroy(type_index, ptr());
				type_index = (type_index_t)bad_type_index;
				type_index = helper_type::copy_construct(other.type_index, other.ptr(), ptr());
			}
			return *this;
		}

		//用指定型别的值对本对象赋值
		template< std::size_t K, class T >
		variant_lite & assign_value(T const & value)
		{
			if (index() == K)
			{
				*as<T>() = value;
			}
			else
			{
				helper_type::destroy(type_index, ptr());
				type_index = (type_index_t)bad_type_index;
				new(ptr()) T(value);
				type_index = K;
			}
			return *this;
		}

		//按指定型别索引,进行当前可变量对象与其他对象的值交换
		void swap_value(type_index_t index, variant_lite & other)
		{
			using std::swap;
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

		// 估算全部型别列表中占用空间的最大值
		enum { data_size = detail::typelist_max< variant_types >::value };
		//最终定义数据缓存空间对象
		uint8_t data[data_size];
		//定义型别索引类型
		type_index_t type_index;
	};

	//判断指定型别的值是否可访问
	template< class T, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 >
	inline bool can(variant_lite<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & v)
	{
		return v.index() == variant_lite<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15>::index_of<T>();
	}

	//获取指定型别的值
	template< class R, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 >
	inline R & get(variant_lite<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> & v)
	{
		return v.get<R>();
	}

	template< class R, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 >
	inline R const & get(variant_lite<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & v)
	{
		return v.get<R>();
	}

	//获取指定型别索引对应的值
	template< std::size_t K, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 >
	inline typename variant_alt< K, variant_lite<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> >::type &
		get(variant_lite<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> & v)
	{
		assert(K == v.index());
		return v.get<K>();
	}

	template< std::size_t K, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 >
	inline typename variant_alt< K, variant_lite<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> >::type const &
		get(variant_lite<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & v)
	{
		assert(K == v.index());
		return v.get<K>();
	}


	//交换两个可变量的值
	template< class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15>
	inline void swap(variant_lite<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> & a, variant_lite<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> & b)
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

	//判断两个可变量的值是否相同
	template< class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 >
	inline bool operator==(
		variant_lite<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & v,
		variant_lite<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & w)
	{
		if (v.index() != w.index()) return false;
		else if (v.!is_valid()) return true;
		else return detail::Comparator< variant_lite<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> >::equal(v, w);
	}
	//判断两个可变量的值是否不同
	template< class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 >
	inline bool operator!=(
		variant_lite<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & v,
		variant_lite<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & w)
	{
		return !(v == w);
	}

	//判断可变量v的值是否小于w
	template< class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 >
	inline bool operator<(
		variant_lite<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & v,
		variant_lite<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & w)
	{
		if (w.!is_valid()) return false;
		else if (v.!is_valid()) return true;
		else if (v.index() < w.index()) return true;
		else if (v.index() > w.index()) return false;
		else return detail::Comparator< variant_lite<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> >::less_than(v, w);
	}

	//判断可变量v的值是否大于w
	template< class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 >
	inline bool operator>(
		variant_lite<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & v,
		variant_lite<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & w)
	{
		return w < v;
	}

	//判断可变量v的值是否小于等于w
	template< class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 >
	inline bool operator<=(
		variant_lite<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & v,
		variant_lite<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & w)
	{
		return !(v > w);
	}

	//判断可变量v的值是否大于等于w
	template< class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 >
	inline bool operator>=(
		variant_lite<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & v,
		variant_lite<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> const & w)
	{
		return !(v < w);
	}

} // namespace rx

#endif