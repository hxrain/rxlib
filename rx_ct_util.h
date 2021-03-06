#ifndef _H_RX_CT_TEMPLATE_UTIL_H_
#define _H_RX_CT_TEMPLATE_UTIL_H_

#include <math.h>
#include "rx_ct_traits.h"
#include <new>

/*
编译期模板函数库
*/
namespace rx
{
	//=====================================================
	//编译期构造析构处理类,方便分配器使用的构造与析构工具
	//OC,OD为对象构造,析构函数;AC,AD为数组构造,析构函数.
	//=====================================================
	class ct
	{
		//--------------------------------------------------
		//根据类型信息,对一个数组中的所有元素逐一调用析构函数
		template <class T>	static inline void do_array_destroy(T, const uint32_t, const ct_type_true_t&) {}
		template <class T>	static inline void do_array_destroy(T* Array, const uint32_t Count, const ct_type_false_t&) { for (uint32_t i = 0; i < Count; i++) OD(&Array[i]); }
		//--------------------------------------------------
		//根据类型偏特化判断的数组构造函数
		template <class T>  static inline void do_array_construct(T* p, const uint32_t Count, const ct_type_true_t&) {}
		template <class T>	static inline void do_array_construct(T* p, const uint32_t Count, const ct_type_false_t&) { for (uint32_t i = 0; i < Count; i++) OC(p + i); }
		template <class T, class PT1>
		static inline void do_array_construct(T* p, const uint32_t Count, PT1& P1, const ct_type_false_t&) { for (uint32_t i = 0; i < Count; i++) OC(p + i, P1); }
		template <class T, class PT1, class PT2>
		static inline void do_array_construct(T* p, const uint32_t Count, PT1& P1, PT2 &P2, const ct_type_false_t&) { for (uint32_t i = 0; i < Count; i++) OC(p + i, P1, P2); }
		template <class T, class PT1, class PT2, class PT3>
		static inline void do_array_construct(T* p, const uint32_t Count, PT1& P1, PT2 &P2, PT3& P3, const ct_type_false_t&) { for (uint32_t i = 0; i < Count; i++) OC(p + i, P1, P2, P3); }
	public:
		//--------------------------------------------------
		//进行对象构造
		template <class T>
		static inline T* OC(T* O) { return new (O) T; }

		template <class T, class PT1>
		static inline T* OC(T* O, PT1& P1) { return new(O)T(P1); }
		template <class T, class PT1>
		static inline T* OC(T* O, const PT1& P1) { return new(O)T(P1); }

		template <class T, class PT1, class PT2>
		static inline T* OC(T* O, PT1& P1, PT2& P2) { return new (O) T(P1, P2); }

		template <class T, class PT1, class PT2, class PT3>
		static inline T* OC(T* O, PT1& P1, PT2& P2, PT3& P3) { return new (O) T(P1, P2, P3); }

		template <class T, class PT1, class PT2, class PT3, class PT4>
		static inline T* OC(T* O, PT1& P1, PT2& P2, PT3& P3, PT4& P4) { return new (O) T(P1, P2, P3, P4); }

		template <class T, class PT1, class PT2, class PT3, class PT4, class PT5>
		static inline T* OC(T* O, PT1& P1, PT2& P2, PT3& P3, PT4& P4, PT5& P5) { return new (O) T(P1, P2, P3, P4, P5); }
		//--------------------------------------------------
		//进行对象析构
		template <class T>
		static inline void OD(T* O) { O->~T(); }


		//--------------------------------------------------
		//进行数组构造
		template <class T>
		static inline T* AC(T* Array, const uint32_t Count)
		{
			typedef typename ct_type_check_t<T>::has_trivial_default_constructor trivial_default_constructor;
			do_array_construct(Array, Count, trivial_default_constructor());
			return Array;
		}
		template <class T, class PT1>
		static inline T* AC(T* Array, const uint32_t Count, PT1& P1)
		{
			typedef typename ct_type_check_t<T>::has_trivial_default_constructor trivial_default_constructor;
			do_array_construct(Array, Count, P1, trivial_default_constructor());
			return Array;
		}
		template <class T, class PT1>
		static inline T* AC(T* Array, const uint32_t Count, const PT1& P1)
		{
			typedef typename ct_type_check_t<T>::has_trivial_default_constructor trivial_default_constructor;
			do_array_construct(Array, Count, P1, trivial_default_constructor());
			return Array;
		}
		template <class T, class PT1, class PT2>
		static inline T* AC(T* Array, const uint32_t Count, PT1& P1, PT2& P2)
		{
			typedef typename ct_type_check_t<T>::has_trivial_default_constructor trivial_default_constructor;
			do_array_construct(Array, Count, P1, P2, trivial_default_constructor());
			return Array;
		}
		template <class T, class PT1, class PT2>
		static inline T* AC(T* Array, const uint32_t Count, const PT1& P1, const PT2& P2)
		{
			typedef typename ct_type_check_t<T>::has_trivial_default_constructor trivial_default_constructor;
			do_array_construct(Array, Count, P1, P2, trivial_default_constructor());
			return Array;
		}
		template <class T, class PT1, class PT2, class PT3>
		static inline T* AC(T* Array, const uint32_t Count, PT1& P1, PT2& P2, PT3& P3)
		{
			typedef typename ct_type_check_t<T>::has_trivial_default_constructor trivial_default_constructor;
			do_array_construct(Array, Count, P1, P2, P3, trivial_default_constructor());
			return Array;
		}
		template <class T, class PT1, class PT2, class PT3>
		static inline T* AC(T* Array, const uint32_t Count, const PT1& P1, const PT2& P2, PT3& P3)
		{
			typedef typename ct_type_check_t<T>::has_trivial_default_constructor trivial_default_constructor;
			do_array_construct(Array, Count, P1, P2, P3, trivial_default_constructor());
			return Array;
		}
		//--------------------------------------------------
		//进行数组析构
		template <class T>
		static inline void AD(T* Array, const uint32_t Count)
		{
			typedef typename ct_type_check_t<T>::has_trivial_destructor trivial_destructor;
			do_array_destroy(Array, Count, trivial_destructor());
		}
	};

	//=====================================================
	//静态函数,计算Log2(num),其中num必须是2的整数次幂
	template<uint32_t num> class LOG2 {};
	template<>class LOG2<0x00000001> { public:enum { result = 0 }; };
	template<>class LOG2<0x00000002> { public:enum { result = 1 }; };
	template<>class LOG2<0x00000004> { public:enum { result = 2 }; };
	template<>class LOG2<0x00000008> { public:enum { result = 3 }; };
	template<>class LOG2<0x00000010> { public:enum { result = 4 }; };
	template<>class LOG2<0x00000020> { public:enum { result = 5 }; };
	template<>class LOG2<0x00000040> { public:enum { result = 6 }; };
	template<>class LOG2<0x00000080> { public:enum { result = 7 }; };
	template<>class LOG2<0x00000100> { public:enum { result = 8 }; };
	template<>class LOG2<0x00000200> { public:enum { result = 9 }; };
	template<>class LOG2<0x00000400> { public:enum { result = 10 }; };
	template<>class LOG2<0x00000800> { public:enum { result = 11 }; };
	template<>class LOG2<0x00001000> { public:enum { result = 12 }; };
	template<>class LOG2<0x00002000> { public:enum { result = 13 }; };
	template<>class LOG2<0x00004000> { public:enum { result = 14 }; };
	template<>class LOG2<0x00008000> { public:enum { result = 15 }; };
	template<>class LOG2<0x00010000> { public:enum { result = 16 }; };
	template<>class LOG2<0x00020000> { public:enum { result = 17 }; };
	template<>class LOG2<0x00040000> { public:enum { result = 18 }; };
	template<>class LOG2<0x00080000> { public:enum { result = 19 }; };
	template<>class LOG2<0x00100000> { public:enum { result = 20 }; };
	template<>class LOG2<0x00200000> { public:enum { result = 21 }; };
	template<>class LOG2<0x00400000> { public:enum { result = 22 }; };
	template<>class LOG2<0x00800000> { public:enum { result = 23 }; };
	template<>class LOG2<0x01000000> { public:enum { result = 24 }; };
	template<>class LOG2<0x02000000> { public:enum { result = 25 }; };
	template<>class LOG2<0x04000000> { public:enum { result = 26 }; };
	template<>class LOG2<0x08000000> { public:enum { result = 27 }; };
	template<>class LOG2<0x10000000> { public:enum { result = 28 }; };
	template<>class LOG2<0x20000000> { public:enum { result = 29 }; };
	template<>class LOG2<0x40000000> { public:enum { result = 30 }; };
	template<>class LOG2<0x80000000> { public:enum { result = 31 }; };

	inline double rx_log2(double v) { return log(v) / 0.693147180559945309417; }

	//=====================================================
	//静态函数,计算POW2(num),其中num必须是2的整数次幂
	template<uint32_t num> class POW2 {};
	template<>class POW2<0> { public:enum { result = 0x00000001 }; };
	template<>class POW2<1> { public:enum { result = 0x00000002 }; };
	template<>class POW2<2> { public:enum { result = 0x00000004 }; };
	template<>class POW2<3> { public:enum { result = 0x00000008 }; };
	template<>class POW2<4> { public:enum { result = 0x00000010 }; };
	template<>class POW2<5> { public:enum { result = 0x00000020 }; };
	template<>class POW2<6> { public:enum { result = 0x00000040 }; };
	template<>class POW2<7> { public:enum { result = 0x00000080 }; };
	template<>class POW2<8> { public:enum { result = 0x00000100 }; };
	template<>class POW2<9> { public:enum { result = 0x00000200 }; };
	template<>class POW2<10> { public:enum { result = 0x00000400 }; };
	template<>class POW2<11> { public:enum { result = 0x00000800 }; };
	template<>class POW2<12> { public:enum { result = 0x00001000 }; };
	template<>class POW2<13> { public:enum { result = 0x00002000 }; };
	template<>class POW2<14> { public:enum { result = 0x00004000 }; };
	template<>class POW2<15> { public:enum { result = 0x00008000 }; };
	template<>class POW2<16> { public:enum { result = 0x00010000 }; };
	template<>class POW2<17> { public:enum { result = 0x00020000 }; };
	template<>class POW2<18> { public:enum { result = 0x00040000 }; };
	template<>class POW2<19> { public:enum { result = 0x00080000 }; };
	template<>class POW2<20> { public:enum { result = 0x00100000 }; };
	template<>class POW2<21> { public:enum { result = 0x00200000 }; };
	template<>class POW2<22> { public:enum { result = 0x00400000 }; };
	template<>class POW2<23> { public:enum { result = 0x00800000 }; };
	template<>class POW2<24> { public:enum { result = 0x01000000 }; };
	template<>class POW2<25> { public:enum { result = 0x02000000 }; };
	template<>class POW2<26> { public:enum { result = 0x04000000 }; };
	template<>class POW2<27> { public:enum { result = 0x08000000 }; };
	template<>class POW2<28> { public:enum { result = 0x10000000 }; };
	template<>class POW2<29> { public:enum { result = 0x20000000 }; };
	template<>class POW2<30> { public:enum { result = 0x40000000 }; };
	template<>class POW2<31> { public:enum { result = 0x80000000 }; };

	//=====================================================
	//计算指定n的阶乘(n<=170)
	inline double factorial(uint8_t n)
	{
		if (n > 170) n = 170;
		double result = 1;
		while (n > 1)
			result *= n--;
		return result;
	}

	//=====================================================
	//尺寸向上对齐处理
	//=====================================================
	//Size待对齐的整数;AlignTo对齐到指定的大小边界
	template<uint32_t Size, uint32_t AlignTo = 4>
	struct size_align
	{
		enum { result = (Size + AlignTo - 1) & ~(AlignTo - 1) };
	};
	//将一个数字向上以AlignTo为边界进行对齐
	inline uint32_t size_align_to(uint32_t Size, uint32_t AlignTo) { return (Size + AlignTo - 1) & ~(AlignTo - 1); }
	//将一个数字向上以4为边界对齐
	#define size_align4(S) ((S+3)& ~3)
	//将一个数字向上以8为边界对齐
	#define size_align8(S) ((S+7)& ~7)
	//将一个数字s向上以x为边界对齐
	#define size_alignx(S,x) ((S+x-1)& ~(x-1))

	//根据数据类型计算其尺寸并将其向上以4字节边界对齐
	#define type_align4(T) size_align4(sizeof(T))
	//根据数据类型计算其尺寸并将其向上以8字节边界对齐
	#define type_align8(T) size_align8(sizeof(T))

	//=====================================================
	//取一个结构体T的成员F的相对于结构体起始地址的偏移量
	#define field_offset(type,field) ((size_t)&((type*)0)->field)

	//得到结构体T中F1字段开始到F2之前字段的空间占用尺寸(就是F1实际占用的尺寸)
	#define field_size(type,field1,field2) ((field_offset(type,field2))-(field_offset(type,field1)))

	//根据type类型中field字段的指针ptr,获取这个type的首地址
	#define struct_head(ptr, type, field) (type *)( (char *)ptr - field_offset(type,field) )

	//=====================================================
	//语法糖,禁止对象拷贝的快捷宏定义
	#define dont_copy(CN) const CN& operator=(const CN&)

	//=====================================================
	//浮点数向上对齐获取整数
	inline uint32_t round_up(const double &num)
	{
		uint32_t rc = (uint32_t)num;
		return (num - rc > 0.00000001) ? rc + 1 : rc;
	}

	//=====================================================
	//计算指定指针偏移后的地址
	#define byte_ptr(p,offset) (((uint8_t*)p)+offset)

	//=====================================================
	//对统一数据结构的容器进行元素节点遍历.返回值:遍历过的节点数量
	template<class CNTR,class FUNC>
	inline uint32_t rx_foreach(const CNTR& cntr, const FUNC& fun)
	{
		uint32_t rc = 0;
		for (typename CNTR::iterator I = cntr.begin();I != cntr.end();++I)
		{
			if (!fun(rc++, cntr.size(), *I))
				break;
		}
		return rc;
	}
}



#endif
