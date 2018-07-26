#ifndef _H_RX_CT_TEMPLATE_UTIL_H_
#define _H_RX_CT_TEMPLATE_UTIL_H_
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
        template <class T>	static inline void do_array_destroy(T,const uint32_t, const rx_type_true_t&) {}
        template <class T>	static inline void do_array_destroy(T* Array, const uint32_t Count, const rx_type_false_t&){for (uint32_t i=0;i<Count;i++) OD(&Array[i]);}
        //--------------------------------------------------
        //根据类型偏特化判断的数组构造函数
        template <class T>  static inline void do_array_construct(T* p,const uint32_t Count,const rx_type_true_t&){}
        template <class T>	static inline void do_array_construct(T* p,const uint32_t Count,const rx_type_false_t&){for(uint32_t i=0;i<Count;i++) OC(p+i);}
        template <class T,class PT1>	static inline void do_array_construct(T* p,const uint32_t Count,PT1& P1,const rx_type_false_t&){for(uint32_t i=0;i<Count;i++) OC(p+i,P1);}
    public:
        //--------------------------------------------------
        //进行对象构造
        template <class T>
        static inline T* OC(T* O){return new (O) T;}

        template <class T, class PT1>
        static inline T* OC(T* O, PT1& P1){return new(O)T(P1);}
        template <class T, class PT1>
        static inline T* OC(T* O, const PT1& P1){return new(O)T(P1);}

        template <class T, class PT1,class PT2>
        static inline T* OC(T* O, PT1& P1,PT2& P2){return new (O) T(P1,P2);}

        template <class T, class PT1,class PT2,class PT3>
        static inline T* OC(T* O, PT1& P1,PT2& P2,PT3& P3){return new (O) T(P1,P2,P3);}

        template <class T, class PT1,class PT2,class PT3,class PT4>
        static inline T* OC(T* O, PT1& P1,PT2& P2,PT3& P3,PT4& P4){return new (O) T(P1,P2,P3,P4);}

        template <class T, class PT1,class PT2,class PT3,class PT4,class PT5>
        static inline T* OC(T* O, PT1& P1,PT2& P2,PT3& P3,PT4& P4,PT5& P5){return new (O) T(P1,P2,P3,P4,P5);}
        //--------------------------------------------------
        //进行对象析构
        template <class T>
        static inline void OD(T* O){O->~T();}


        //--------------------------------------------------
        //进行数组构造
        template <class T>
        static inline void AC(T* Array,const uint32_t Count)
        {
            typedef typename rx_type_check_t<T>::has_trivial_default_constructor trivial_default_constructor;
            do_array_construct(Array,Count, trivial_default_constructor());
        }
        template <class T,class PT1>
        static inline void AC(T* Array,const uint32_t Count,PT1& P1)
        {
            typedef typename rx_type_check_t<T>::has_trivial_default_constructor trivial_default_constructor;
            do_array_construct(Array,Count,P1, trivial_default_constructor());
        }
        template <class T,class PT1>
        static inline void AC(T* Array,const uint32_t Count,const PT1& P1)
        {
            typedef typename rx_type_check_t<T>::has_trivial_default_constructor trivial_default_constructor;
            do_array_construct(Array,Count,P1, trivial_default_constructor());
        }
        //--------------------------------------------------
        //进行数组析构
        template <class T>
        static inline void AD(T* Array,const uint32_t Count)
        {
            typedef typename rx_type_check_t<T>::has_trivial_destructor trivial_destructor;
            do_array_destroy(Array,Count, trivial_destructor());
        }
    };

    //=====================================================
    //自定义实现最大值最小值比较功能
    //=====================================================
    template <class T>
    inline const T& Min(const T& ValueA, const T& ValueB) { return ValueB < ValueA ? ValueB : ValueA; }
    template <class T>
    inline const T& Max(const T& ValueA, const T& ValueB) {  return  ValueA < ValueB ? ValueB : ValueA; }
    template <class T>
    inline const T& Min(const T& ValueA, const T& ValueB,const T& ValueC) { return Min(Min(ValueA,ValueB),ValueC); }
    template <class T>
    inline const T& Max(const T& ValueA, const T& ValueB,const T& ValueC) { return Max(Max(ValueA,ValueB),ValueC); }

    //=====================================================
    //静态函数,计算Log(num),其中num必须是2的整数次幂
    //=====================================================
    template<uint32_t num> class log2{};                    //num不是2的整数次幂的通用模板,不应该被使用
	template<>class log2<0x00000000>{public:enum{result=0};};
    template<>class log2<0x00000002>{public:enum{result=1};};
    template<>class log2<0x00000004>{public:enum{result=2};};
    template<>class log2<0x00000008>{public:enum{result=3};};
    template<>class log2<0x00000010>{public:enum{result=4};};
    template<>class log2<0x00000020>{public:enum{result=5};};
    template<>class log2<0x00000040>{public:enum{result=6};};
    template<>class log2<0x00000080>{public:enum{result=7};};
    template<>class log2<0x00000100>{public:enum{result=8};};
    template<>class log2<0x00000200>{public:enum{result=9};};
    template<>class log2<0x00000400>{public:enum{result=10};};
    template<>class log2<0x00000800>{public:enum{result=11};};
    template<>class log2<0x00001000>{public:enum{result=12};};
    template<>class log2<0x00002000>{public:enum{result=13};};
    template<>class log2<0x00004000>{public:enum{result=14};};
    template<>class log2<0x00008000>{public:enum{result=15};};
    template<>class log2<0x00010000>{public:enum{result=16};};
    template<>class log2<0x00020000>{public:enum{result=17};};
    template<>class log2<0x00040000>{public:enum{result=18};};
    template<>class log2<0x00080000>{public:enum{result=19};};
    template<>class log2<0x00100000>{public:enum{result=20};};
    template<>class log2<0x00200000>{public:enum{result=21};};
    template<>class log2<0x00400000>{public:enum{result=22};};
    template<>class log2<0x00800000>{public:enum{result=23};};
    template<>class log2<0x01000000>{public:enum{result=24};};
    template<>class log2<0x02000000>{public:enum{result=25};};
    template<>class log2<0x04000000>{public:enum{result=26};};
    template<>class log2<0x08000000>{public:enum{result=27};};
    template<>class log2<0x10000000>{public:enum{result=28};};
    template<>class log2<0x20000000>{public:enum{result=29};};
    template<>class log2<0x40000000>{public:enum{result=30};};
    template<>class log2<0x80000000>{public:enum{result=31};};

    //=====================================================
    //尺寸向上对齐处理
    //=====================================================
    //Size待对齐的整数;AlignTo对齐到指定的大小边界
    template<uint32_t Size,uint32_t AlignTo=4>
    struct size_align
    {
        enum{result= (Size + AlignTo - 1) & ~(AlignTo - 1)};
    };
    //将一个数字向上以AlignTo为边界进行对齐
    inline uint32_t size_align_to(uint32_t Size,uint32_t AlignTo){return (Size + AlignTo - 1) & ~(AlignTo - 1);}
    //将一个数字向上以4为边界对齐
    #define size_align4(S) ((S+3)& ~3)
    //将一个数字向上以8为边界对齐
    #define size_align8(S) ((S+7)& ~7)

    //根据数据类型计算其尺寸并将其向上以4字节边界对齐
    #define type_align4(T) size_align4(sizeof(T))
    //根据数据类型计算其尺寸并将其向上以8字节边界对齐
    #define type_align8(T) size_align8(sizeof(T))

    //取一个结构体T的成员F的相对于结构体的偏移量
    #define struct_offset(T,F) (uint32_t)(&((T*)0)->F)
    //得到结构体T中F1字段开始到F2之前字段的空间占用尺寸
    #define field_size(T,F1,F2) ((struct_offset(T,F2))-(struct_offset(T,F1)))

    //语法糖,禁止对象拷贝的快捷宏定义
    #define dont_copy(CN) const CN& operator=(const CN&)

}

#endif
