#ifndef _H_RX_CT_TEMPLATE_UTIL_H_
#define _H_RX_CT_TEMPLATE_UTIL_H_

#include <math.h>
#include "rx_ct_traits.h"
#include <new>

/*
������ģ�庯����
*/
namespace rx
{
	//=====================================================
	//�����ڹ�������������,���������ʹ�õĹ�������������
	//OC,ODΪ������,��������;AC,ADΪ���鹹��,��������.
	//=====================================================
	class ct
	{
		//--------------------------------------------------
		//����������Ϣ,��һ�������е�����Ԫ����һ������������
		template <class T>	static inline void do_array_destroy(T, const uint32_t, const ct_type_true_t&) {}
		template <class T>	static inline void do_array_destroy(T* Array, const uint32_t Count, const ct_type_false_t&) { for (uint32_t i = 0; i < Count; i++) OD(&Array[i]); }
		//--------------------------------------------------
		//��������ƫ�ػ��жϵ����鹹�캯��
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
		//���ж�����
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
		//���ж�������
		template <class T>
		static inline void OD(T* O) { O->~T(); }


		//--------------------------------------------------
		//�������鹹��
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
		//������������
		template <class T>
		static inline void AD(T* Array, const uint32_t Count)
		{
			typedef typename ct_type_check_t<T>::has_trivial_destructor trivial_destructor;
			do_array_destroy(Array, Count, trivial_destructor());
		}
	};

	//=====================================================
	//��̬����,����Log2(num),����num������2����������
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
	//��̬����,����POW2(num),����num������2����������
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
	//����ָ��n�Ľ׳�(n<=170)
	inline double factorial(uint8_t n)
	{
		if (n > 170) n = 170;
		double result = 1;
		while (n > 1)
			result *= n--;
		return result;
	}

	//=====================================================
	//�ߴ����϶��봦��
	//=====================================================
	//Size�����������;AlignTo���뵽ָ���Ĵ�С�߽�
	template<uint32_t Size, uint32_t AlignTo = 4>
	struct size_align
	{
		enum { result = (Size + AlignTo - 1) & ~(AlignTo - 1) };
	};
	//��һ������������AlignToΪ�߽���ж���
	inline uint32_t size_align_to(uint32_t Size, uint32_t AlignTo) { return (Size + AlignTo - 1) & ~(AlignTo - 1); }
	//��һ������������4Ϊ�߽����
	#define size_align4(S) ((S+3)& ~3)
	//��һ������������8Ϊ�߽����
	#define size_align8(S) ((S+7)& ~7)
	//��һ������s������xΪ�߽����
	#define size_alignx(S,x) ((S+x-1)& ~(x-1))

	//�����������ͼ�����ߴ粢����������4�ֽڱ߽����
	#define type_align4(T) size_align4(sizeof(T))
	//�����������ͼ�����ߴ粢����������8�ֽڱ߽����
	#define type_align8(T) size_align8(sizeof(T))

	//=====================================================
	//ȡһ���ṹ��T�ĳ�ԱF������ڽṹ����ʼ��ַ��ƫ����
	#define field_offset(type,field) ((size_t)&((type*)0)->field)

	//�õ��ṹ��T��F1�ֶο�ʼ��F2֮ǰ�ֶεĿռ�ռ�óߴ�(����F1ʵ��ռ�õĳߴ�)
	#define field_size(type,field1,field2) ((field_offset(type,field2))-(field_offset(type,field1)))

	//����type������field�ֶε�ָ��ptr,��ȡ���type���׵�ַ
	#define struct_head(ptr, type, field) (type *)( (char *)ptr - field_offset(type,field) )

	//=====================================================
	//�﷨��,��ֹ���󿽱��Ŀ�ݺ궨��
	#define dont_copy(CN) const CN& operator=(const CN&)

	//=====================================================
	//���������϶����ȡ����
	inline uint32_t round_up(const double &num)
	{
		uint32_t rc = (uint32_t)num;
		return (num - rc > 0.00000001) ? rc + 1 : rc;
	}

	//=====================================================
	//����ָ��ָ��ƫ�ƺ�ĵ�ַ
	#define byte_ptr(p,offset) (((uint8_t*)p)+offset)

	//=====================================================
	//��ͳһ���ݽṹ����������Ԫ�ؽڵ����.����ֵ:�������Ľڵ�����
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

//==========================================================
// �ο� https://github.com/swansontec/map-macro
//����ԭ��: ���ú�Ķ�̬չ��,�Լ��������,�ٽ��������㹻������չ������.
//�ݹ�Ƕ�׺�չ��,���Ƕ�׵���ȷ��չ�������㹻(Ĭ��չ��365��)
#define MACRO_EVAL0(...) __VA_ARGS__
#define MACRO_EVAL1(...) MACRO_EVAL0(MACRO_EVAL0(MACRO_EVAL0(__VA_ARGS__)))
#define MACRO_EVAL2(...) MACRO_EVAL1(MACRO_EVAL1(MACRO_EVAL1(__VA_ARGS__)))
#define MACRO_EVAL3(...) MACRO_EVAL2(MACRO_EVAL2(MACRO_EVAL2(__VA_ARGS__)))
#define MACRO_EVAL4(...) MACRO_EVAL3(MACRO_EVAL3(MACRO_EVAL3(__VA_ARGS__)))
#define MACRO_EVAL(...)  MACRO_EVAL4(MACRO_EVAL4(MACRO_EVAL4(__VA_ARGS__)))

//����Ԫ,ֻ����ռλ
#define MACRO_DUMMY
//���ŷָ���
#define MAP_COMMA ,

//��Ƕ�׵ݹ�,��EAT�����,���ڽ��������б�ı���
#define _MACRO_MAP_END0(...)
#define _MACRO_MAP_END1() 0, _MACRO_MAP_END0 //0��Ϊ��Ҫ�����Ե���ռλ��
#define _MACRO_MAP_END2(...) _MACRO_MAP_END1
#define _MACRO_MAP_END(...)  _MACRO_MAP_END2

//��Ƕ�׵ݹ�,���������б��е�ǰһ��,���ú�һ��
#define _MACRO_MAP_EAT0(discard, next, ...) next MACRO_DUMMY
#define _MACRO_MAP_EAT1(discard, next) _MACRO_MAP_EAT0(discard, next, 0)
#define _MACRO_MAP_EAT(discard, next)  _MACRO_MAP_EAT1(_MACRO_MAP_END discard, next)

//��Ƕ�׵ݹ�,ǰ��ʹ�����f(x)��,���ö�̬��չ��,����������һ���������б�,�������յ�չ�����
#define _MACRO_MAP0(f, x, nextx, ...) f(x) _MACRO_MAP_EAT(nextx, _MACRO_MAP1)(f, nextx, __VA_ARGS__)
#define _MACRO_MAP1(f, x, nextx, ...) f(x) _MACRO_MAP_EAT(nextx, _MACRO_MAP0)(f, nextx, __VA_ARGS__)

//��Ƕ�׵ݹ�,���м�չ����ʱ�����ָ���,���ڹ�������б�
#define _MACRO_MAP_EATL1(test, next) _MACRO_MAP_EAT0(test, MAP_COMMA next, 0)
#define _MACRO_MAP_EATL(test, next)  _MACRO_MAP_EATL1(_MACRO_MAP_END test, next)
#define _MACRO_MAPL0(f, x, nextx, ...) f(x) _MACRO_MAP_EATL(nextx, _MACRO_MAPL1)(f, nextx, __VA_ARGS__)
#define _MACRO_MAPL1(f, x, nextx, ...) f(x) _MACRO_MAP_EATL(nextx, _MACRO_MAPL0)(f, nextx, __VA_ARGS__)

//�����б���,�м����ɶ��ŷָ���
#define MACRO_MAPL(f, ...) MACRO_EVAL(_MACRO_MAPL1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))
//����ӳ�书��,ԭ�����f�Ͳ����б�
#define MACRO_MAP(f, ...) MACRO_EVAL(_MACRO_MAP1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0)) //()()()����_MACRO_MAP_END�㼶���,ȷ���ݹ�ɽ���

/*
����1:
#define STRING(x) char const *x##_string = #x;
MACRO_MAP(STRING, foo, bar) 
���:
char const *foo_string = "foo"; char const *bar_string = "bar";
�м����:
					   _MACRO_MAP_EAT1(_MACRO_MAP_END ()()(), _MACRO_MAP1) -> _MACRO_MAP_EAT0(0, _MACRO_MAP_END0, _MACRO_MAP1, 0) -> _MACRO_MAP_END0 MACRO_DUMMY -> _MACRO_MAP_END0
     	   STRING(bar) _MACRO_MAP_EAT(()()(), _MACRO_MAP1)(STRING, ()()(),  ()()(), ()()(), 0)
					   _MACRO_MAP_EAT1(_MACRO_MAP_END bar, _MACRO_MAP0) -> _MACRO_MAP_EAT0(_MACRO_MAP_END bar, _MACRO_MAP0, 0) -> _MACRO_MAP0 MACRO_DUMMY -> _MACRO_MAP0
     	   STRING(foo) _MACRO_MAP_EAT(bar, _MACRO_MAP0)(STRING, bar,  ()()(), ()()(), ()()(), 0)
MACRO_EVAL(_MACRO_MAP1(STRING,  foo, bar, ()()(), ()()(), ()()(), 0))

����2:
#define sp(x) int x
void tst(MACRO_MAPL(sp,a,b,c));
���:
void tst(int a , int b , int c);
*/

#endif
