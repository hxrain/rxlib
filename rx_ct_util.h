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
		template <class T>	static inline void do_array_destroy(T, const uint32_t, const rx_type_true_t&) {}
		template <class T>	static inline void do_array_destroy(T* Array, const uint32_t Count, const rx_type_false_t&) { for (uint32_t i = 0; i < Count; i++) OD(&Array[i]); }
		//--------------------------------------------------
		//��������ƫ�ػ��жϵ����鹹�캯��
		template <class T>  static inline void do_array_construct(T* p, const uint32_t Count, const rx_type_true_t&) {}
		template <class T>	static inline void do_array_construct(T* p, const uint32_t Count, const rx_type_false_t&) { for (uint32_t i = 0; i < Count; i++) OC(p + i); }
		template <class T, class PT1>
		static inline void do_array_construct(T* p, const uint32_t Count, PT1& P1, const rx_type_false_t&) { for (uint32_t i = 0; i < Count; i++) OC(p + i, P1); }
		template <class T, class PT1, class PT2>
		static inline void do_array_construct(T* p, const uint32_t Count, PT1& P1, PT2 &P2, const rx_type_false_t&) { for (uint32_t i = 0; i < Count; i++) OC(p + i, P1, P2); }
		template <class T, class PT1, class PT2, class PT3>
		static inline void do_array_construct(T* p, const uint32_t Count, PT1& P1, PT2 &P2, PT3& P3, const rx_type_false_t&) { for (uint32_t i = 0; i < Count; i++) OC(p + i, P1, P2, P3); }
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
			typedef typename rx_type_check_t<T>::has_trivial_default_constructor trivial_default_constructor;
			do_array_construct(Array, Count, trivial_default_constructor());
			return Array;
		}
		template <class T, class PT1>
		static inline T* AC(T* Array, const uint32_t Count, PT1& P1)
		{
			typedef typename rx_type_check_t<T>::has_trivial_default_constructor trivial_default_constructor;
			do_array_construct(Array, Count, P1, trivial_default_constructor());
			return Array;
		}
		template <class T, class PT1>
		static inline T* AC(T* Array, const uint32_t Count, const PT1& P1)
		{
			typedef typename rx_type_check_t<T>::has_trivial_default_constructor trivial_default_constructor;
			do_array_construct(Array, Count, P1, trivial_default_constructor());
			return Array;
		}
		template <class T, class PT1, class PT2>
		static inline T* AC(T* Array, const uint32_t Count, PT1& P1, PT2& P2)
		{
			typedef typename rx_type_check_t<T>::has_trivial_default_constructor trivial_default_constructor;
			do_array_construct(Array, Count, P1, P2, trivial_default_constructor());
			return Array;
		}
		template <class T, class PT1, class PT2>
		static inline T* AC(T* Array, const uint32_t Count, const PT1& P1, const PT2& P2)
		{
			typedef typename rx_type_check_t<T>::has_trivial_default_constructor trivial_default_constructor;
			do_array_construct(Array, Count, P1, P2, trivial_default_constructor());
			return Array;
		}
		template <class T, class PT1, class PT2, class PT3>
		static inline T* AC(T* Array, const uint32_t Count, PT1& P1, PT2& P2, PT3& P3)
		{
			typedef typename rx_type_check_t<T>::has_trivial_default_constructor trivial_default_constructor;
			do_array_construct(Array, Count, P1, P2, P3, trivial_default_constructor());
			return Array;
		}
		template <class T, class PT1, class PT2, class PT3>
		static inline T* AC(T* Array, const uint32_t Count, const PT1& P1, const PT2& P2, PT3& P3)
		{
			typedef typename rx_type_check_t<T>::has_trivial_default_constructor trivial_default_constructor;
			do_array_construct(Array, Count, P1, P2, P3, trivial_default_constructor());
			return Array;
		}
		//--------------------------------------------------
		//������������
		template <class T>
		static inline void AD(T* Array, const uint32_t Count)
		{
			typedef typename rx_type_check_t<T>::has_trivial_destructor trivial_destructor;
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
	/*
	//=====================================================
	//�ж�ָ��ָ�����ϵn�ֽ������Ƿ�Ϊ0
	#define equ_zero_1(p)  (*(uint8_t*)p==0)
	#define equ_zero_2(p)  (*(uint16_t*)p==0)
	#define equ_zero_3(p)  (equ_zero_2 (p)&&equ_zero_1(byte_ptr(p,2)))
	#define equ_zero_4(p)  (*(uint32_t*)p==0)
	#define equ_zero_5(p)  (equ_zero_4 (p)&&equ_zero_1(byte_ptr(p,4)))
	#define equ_zero_6(p)  (equ_zero_4 (p)&&equ_zero_2(byte_ptr(p,4)))
	#define equ_zero_7(p)  (equ_zero_4 (p)&&equ_zero_3(byte_ptr(p,4)))
	#if RX_CC_BIT==64
	#define equ_zero_8(p)  (*(uint64_t*)p==0)
	#else
	#define equ_zero_8(p)  (equ_zero_4(p)&&equ_zero_4(byte_ptr(p,4)))
	#endif
	#define equ_zero_9(p)  (equ_zero_8 (p)&&equ_zero_1(byte_ptr(p,8)))
	#define equ_zero_10(p) (equ_zero_8 (p)&&equ_zero_2(byte_ptr(p,8)))
	#define equ_zero_11(p) (equ_zero_8 (p)&&equ_zero_3(byte_ptr(p,8)))
	#define equ_zero_12(p) (equ_zero_8 (p)&&equ_zero_4(byte_ptr(p,8)))
	#define equ_zero_13(p) (equ_zero_8 (p)&&equ_zero_5(byte_ptr(p,8)))
	#define equ_zero_14(p) (equ_zero_8 (p)&&equ_zero_6(byte_ptr(p,8)))
	#define equ_zero_15(p) (equ_zero_8 (p)&&equ_zero_7(byte_ptr(p,8)))
	#define equ_zero_16(p) (equ_zero_8 (p)&&equ_zero_8(byte_ptr(p,8)))
	#define equ_zero_17(p) (equ_zero_16(p)&&equ_zero_1(byte_ptr(p,16)))
	#define equ_zero_18(p) (equ_zero_16(p)&&equ_zero_2(byte_ptr(p,16)))
	#define equ_zero_19(p) (equ_zero_16(p)&&equ_zero_3(byte_ptr(p,16)))
	#define equ_zero_20(p) (equ_zero_16(p)&&equ_zero_4(byte_ptr(p,16)))
	#define equ_zero_21(p) (equ_zero_16(p)&&equ_zero_5(byte_ptr(p,16)))
	#define equ_zero_22(p) (equ_zero_16(p)&&equ_zero_6(byte_ptr(p,16)))
	#define equ_zero_23(p) (equ_zero_16(p)&&equ_zero_7(byte_ptr(p,16)))
	#define equ_zero_24(p) (equ_zero_16(p)&&equ_zero_8(byte_ptr(p,16)))
	#define equ_zero_25(p) (equ_zero_24(p)&&equ_zero_1(byte_ptr(p,24)))
	#define equ_zero_26(p) (equ_zero_24(p)&&equ_zero_2(byte_ptr(p,24)))
	#define equ_zero_27(p) (equ_zero_24(p)&&equ_zero_3(byte_ptr(p,24)))
	#define equ_zero_28(p) (equ_zero_24(p)&&equ_zero_4(byte_ptr(p,24)))
	#define equ_zero_29(p) (equ_zero_24(p)&&equ_zero_5(byte_ptr(p,24)))
	#define equ_zero_30(p) (equ_zero_24(p)&&equ_zero_6(byte_ptr(p,24)))
	#define equ_zero_31(p) (equ_zero_24(p)&&equ_zero_7(byte_ptr(p,24)))
	#define equ_zero_32(p) (equ_zero_24(p)&&equ_zero_8(byte_ptr(p,24)))
	#define equ_zero_33(p) (equ_zero_32(p)&&equ_zero_1(byte_ptr(p,32)))
	#define equ_zero_34(p) (equ_zero_32(p)&&equ_zero_2(byte_ptr(p,32)))
	#define equ_zero_35(p) (equ_zero_32(p)&&equ_zero_3(byte_ptr(p,32)))
	#define equ_zero_36(p) (equ_zero_32(p)&&equ_zero_4(byte_ptr(p,32)))
	#define equ_zero_37(p) (equ_zero_32(p)&&equ_zero_5(byte_ptr(p,32)))
	#define equ_zero_38(p) (equ_zero_32(p)&&equ_zero_6(byte_ptr(p,32)))
	#define equ_zero_39(p) (equ_zero_32(p)&&equ_zero_7(byte_ptr(p,32)))
	#define equ_zero_40(p) (equ_zero_32(p)&&equ_zero_8(byte_ptr(p,32)))
	#define equ_zero_41(p) (equ_zero_40(p)&&equ_zero_1(byte_ptr(p,40)))
	#define equ_zero_42(p) (equ_zero_40(p)&&equ_zero_2(byte_ptr(p,40)))
	#define equ_zero_43(p) (equ_zero_40(p)&&equ_zero_3(byte_ptr(p,40)))
	#define equ_zero_44(p) (equ_zero_40(p)&&equ_zero_4(byte_ptr(p,40)))
	#define equ_zero_45(p) (equ_zero_40(p)&&equ_zero_5(byte_ptr(p,40)))
	#define equ_zero_46(p) (equ_zero_40(p)&&equ_zero_6(byte_ptr(p,40)))
	#define equ_zero_47(p) (equ_zero_40(p)&&equ_zero_7(byte_ptr(p,40)))
	#define equ_zero_48(p) (equ_zero_40(p)&&equ_zero_8(byte_ptr(p,40)))
	#define equ_zero_n(p,n) RX_CT_CONCAT(equ_zero_,n)(p)
	inline bool equ_zero_x(const void *p, size_t x)
	{
		for (size_t i = 0; i < x; ++i)
			if (*(uint8_t*)p)
				return false;
		return true;
	}
	inline bool equ_zero(const void *p, size_t x)
	{
		switch (x)
		{
			case 1: return equ_zero_1(p);
			case 2: return equ_zero_2(p);
			case 3: return equ_zero_3(p);
			case 4: return equ_zero_4(p);
			case 5: return equ_zero_5(p);
			case 6: return equ_zero_6(p);
			case 7: return equ_zero_7(p);
			case 8: return equ_zero_8(p);
			case 9: return equ_zero_9(p);
			case 10:return equ_zero_10(p);
			case 11:return equ_zero_11(p);
			case 12:return equ_zero_12(p);
			case 13:return equ_zero_13(p);
			case 14:return equ_zero_14(p);
			case 15:return equ_zero_15(p);
			case 16:return equ_zero_16(p);
			case 17:return equ_zero_17(p);
			case 18:return equ_zero_18(p);
			case 19:return equ_zero_19(p);
			case 20:return equ_zero_20(p);
			case 21:return equ_zero_21(p);
			case 22:return equ_zero_22(p);
			case 23:return equ_zero_23(p);
			case 24:return equ_zero_24(p);
			case 25:return equ_zero_25(p);
			case 26:return equ_zero_26(p);
			case 27:return equ_zero_27(p);
			case 28:return equ_zero_28(p);
			case 29:return equ_zero_29(p);
			case 30:return equ_zero_30(p);
			case 31:return equ_zero_31(p);
			case 32:return equ_zero_32(p);
			case 33:return equ_zero_33(p);
			case 34:return equ_zero_34(p);
			case 35:return equ_zero_35(p);
			case 36:return equ_zero_36(p);
			case 37:return equ_zero_37(p);
			case 38:return equ_zero_38(p);
			case 39:return equ_zero_39(p);
			case 40:return equ_zero_40(p);
			case 41:return equ_zero_41(p);
			case 42:return equ_zero_42(p);
			case 43:return equ_zero_43(p);
			case 44:return equ_zero_44(p);
			case 45:return equ_zero_45(p);
			case 46:return equ_zero_46(p);
			case 47:return equ_zero_47(p);
			case 48:return equ_zero_48(p);
			default:return equ_zero_x(p, x);
		}
	}*/
}

#endif
