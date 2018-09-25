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
        template <class T>	static inline void do_array_destroy(T,const uint32_t, const rx_type_true_t&) {}
        template <class T>	static inline void do_array_destroy(T* Array, const uint32_t Count, const rx_type_false_t&) {for (uint32_t i=0; i<Count; i++) OD(&Array[i]);}
        //--------------------------------------------------
        //根据类型偏特化判断的数组构造函数
        template <class T>  static inline void do_array_construct(T* p,const uint32_t Count,const rx_type_true_t&) {}
        template <class T>	static inline void do_array_construct(T* p,const uint32_t Count,const rx_type_false_t&) {for(uint32_t i=0; i<Count; i++) OC(p+i);}
        template <class T,class PT1>
        static inline void do_array_construct(T* p,const uint32_t Count,PT1& P1,const rx_type_false_t&) {for(uint32_t i=0; i<Count; i++) OC(p+i,P1);}
        template <class T,class PT1,class PT2>
        static inline void do_array_construct(T* p,const uint32_t Count,PT1& P1,PT2 &P2,const rx_type_false_t&) {for(uint32_t i=0; i<Count; i++) OC(p+i,P1,P2);}
        template <class T,class PT1,class PT2,class PT3>
        static inline void do_array_construct(T* p,const uint32_t Count,PT1& P1,PT2 &P2,PT3& P3,const rx_type_false_t&) {for(uint32_t i=0; i<Count; i++) OC(p+i,P1,P2,P3);}
    public:
        //--------------------------------------------------
        //进行对象构造
        template <class T>
        static inline T* OC(T* O) {return new (O) T;}

        template <class T, class PT1>
        static inline T* OC(T* O, PT1& P1) {return new(O)T(P1);}
        template <class T, class PT1>
        static inline T* OC(T* O, const PT1& P1) {return new(O)T(P1);}

        template <class T, class PT1,class PT2>
        static inline T* OC(T* O, PT1& P1,PT2& P2) {return new (O) T(P1,P2);}

        template <class T, class PT1,class PT2,class PT3>
        static inline T* OC(T* O, PT1& P1,PT2& P2,PT3& P3) {return new (O) T(P1,P2,P3);}

        template <class T, class PT1,class PT2,class PT3,class PT4>
        static inline T* OC(T* O, PT1& P1,PT2& P2,PT3& P3,PT4& P4) {return new (O) T(P1,P2,P3,P4);}

        template <class T, class PT1,class PT2,class PT3,class PT4,class PT5>
        static inline T* OC(T* O, PT1& P1,PT2& P2,PT3& P3,PT4& P4,PT5& P5) {return new (O) T(P1,P2,P3,P4,P5);}
        //--------------------------------------------------
        //进行对象析构
        template <class T>
        static inline void OD(T* O) {O->~T();}


        //--------------------------------------------------
        //进行数组构造
        template <class T>
        static inline T* AC(T* Array,const uint32_t Count)
        {
            typedef typename rx_type_check_t<T>::has_trivial_default_constructor trivial_default_constructor;
            do_array_construct(Array,Count, trivial_default_constructor());
            return Array;
        }
        template <class T,class PT1>
        static inline T* AC(T* Array,const uint32_t Count,PT1& P1)
        {
            typedef typename rx_type_check_t<T>::has_trivial_default_constructor trivial_default_constructor;
            do_array_construct(Array,Count,P1, trivial_default_constructor());
            return Array;
        }
        template <class T,class PT1>
        static inline T* AC(T* Array,const uint32_t Count,const PT1& P1)
        {
            typedef typename rx_type_check_t<T>::has_trivial_default_constructor trivial_default_constructor;
            do_array_construct(Array,Count,P1, trivial_default_constructor());
            return Array;
        }
        template <class T,class PT1,class PT2>
        static inline T* AC(T* Array,const uint32_t Count,PT1& P1,PT2& P2)
        {
            typedef typename rx_type_check_t<T>::has_trivial_default_constructor trivial_default_constructor;
            do_array_construct(Array,Count,P1,P2, trivial_default_constructor());
            return Array;
        }
        template <class T,class PT1,class PT2>
        static inline T* AC(T* Array,const uint32_t Count,const PT1& P1,const PT2& P2)
        {
            typedef typename rx_type_check_t<T>::has_trivial_default_constructor trivial_default_constructor;
            do_array_construct(Array,Count,P1,P2, trivial_default_constructor());
            return Array;
        }
        template <class T,class PT1,class PT2,class PT3>
        static inline T* AC(T* Array,const uint32_t Count,PT1& P1,PT2& P2,PT3& P3)
        {
            typedef typename rx_type_check_t<T>::has_trivial_default_constructor trivial_default_constructor;
            do_array_construct(Array,Count,P1,P2,P3, trivial_default_constructor());
            return Array;
        }
        template <class T,class PT1,class PT2,class PT3>
        static inline T* AC(T* Array,const uint32_t Count,const PT1& P1,const PT2& P2,PT3& P3)
        {
            typedef typename rx_type_check_t<T>::has_trivial_default_constructor trivial_default_constructor;
            do_array_construct(Array,Count,P1,P2,P3, trivial_default_constructor());
            return Array;
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
    //静态函数,计算Log2(num),其中num必须是2的整数次幂
    template<uint32_t num> class LOG2 {};
    template<>class LOG2<0x00000001> { public:enum { result = 0  }; };
    template<>class LOG2<0x00000002> { public:enum { result = 1  }; };
    template<>class LOG2<0x00000004> { public:enum { result = 2  }; };
    template<>class LOG2<0x00000008> { public:enum { result = 3  }; };
    template<>class LOG2<0x00000010> { public:enum { result = 4  }; };
    template<>class LOG2<0x00000020> { public:enum { result = 5  }; };
    template<>class LOG2<0x00000040> { public:enum { result = 6  }; };
    template<>class LOG2<0x00000080> { public:enum { result = 7  }; };
    template<>class LOG2<0x00000100> { public:enum { result = 8  }; };
    template<>class LOG2<0x00000200> { public:enum { result = 9  }; };
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
    template<>class POW2<0>  { public:enum { result = 0x00000001 }; };
    template<>class POW2<1>  { public:enum { result = 0x00000002 }; };
    template<>class POW2<2>  { public:enum { result = 0x00000004 }; };
    template<>class POW2<3>  { public:enum { result = 0x00000008 }; };
    template<>class POW2<4>  { public:enum { result = 0x00000010 }; };
    template<>class POW2<5>  { public:enum { result = 0x00000020 }; };
    template<>class POW2<6>  { public:enum { result = 0x00000040 }; };
    template<>class POW2<7>  { public:enum { result = 0x00000080 }; };
    template<>class POW2<8>  { public:enum { result = 0x00000100 }; };
    template<>class POW2<9>  { public:enum { result = 0x00000200 }; };
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

    //-----------------------------------------------------
    //计算指定n的阶乘(n<=170)
    inline double factorial(uint8_t n)
    {
        if (n>170) n=170;
        double result = 1;
        while (n > 1)
            result *= n--;
        return result;
    }
    //=====================================================
    //尺寸向上对齐处理
    //=====================================================
    //Size待对齐的整数;AlignTo对齐到指定的大小边界
    template<uint32_t Size,uint32_t AlignTo=4>
    struct size_align
    {
        enum {result= (Size + AlignTo - 1) & ~(AlignTo - 1)};
    };
    //将一个数字向上以AlignTo为边界进行对齐
    inline uint32_t size_align_to(uint32_t Size,uint32_t AlignTo) {return (Size + AlignTo - 1) & ~(AlignTo - 1);}
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

    //取一个结构体T的成员F的相对于结构体的偏移量
    #define struct_offset(T,F) (uint32_t)(&((T*)0)->F)
    //得到结构体T中F1字段开始到F2之前字段的空间占用尺寸
    #define field_size(T,F1,F2) ((struct_offset(T,F2))-(struct_offset(T,F1)))

    //语法糖,禁止对象拷贝的快捷宏定义
    #define dont_copy(CN) const CN& operator=(const CN&)

    //浮点数向上对齐获取整数
    inline uint32_t round_up(const double &num)
    {
        uint32_t rc = (uint32_t)num;
        return (num - rc > 0.00000001) ? rc + 1 : rc;
    }


    //-----------------------------------------------------
    //回调函数的委托功能封装(实现最小化的特定接口成员函数与普通函数的委托调用基础.)
    //实现的原理:最终的委托对象要有公共的基类接口;动态将不同的委托对象构造在指定的内存块上,就可以用统一的接口指针进行访问了.
    //-----------------------------------------------------
    class delegate_def_t
    {
    public:
        //回调函数原型;成员函数也需要有相同的格式.
        //uint32_t cb_func_t(void *obj, void *p1, void *p2, void *usrdat)
        typedef uint32_t(*cb_func_t)(void *obj, void *p1, void *p2, void *usrdat);
    private:
        //-----------------------------------------------------
        //事件回调委托对象接口:定义统一抽象接口.
        class delegate_i
        {
        public:
            virtual uint32_t operator()(void *obj, void *p1, void *p2, void *usrdat) = 0;
        };
        //-------------------------------------------------
        //成员函数回调指针对应的委托对象
        template<class T,class dummy>
        class delegate_t :public delegate_i
        {
            T              &owner;                          //使用成员函数指针,需要绑定其拥有者对象指针
            uint32_t   (T::*cb_member_func)(void *obj, void *p1, void *p2, void *usrdat);
        public:
            delegate_t(T& Ths, uint32_t(T::*mf)(void *obj, void *p1, void *p2, void *usrdat)) :owner(Ths), cb_member_func(mf) {}
            //---------------------------------------------
            //使用给定的参数调用事件回调.返回值:<0错误;0未正确处理;Event处理完成.
            uint32_t operator()(void *obj, void *p1, void *p2, void *usrdat)
            {
                return (owner.*cb_member_func)(obj, p1, p2, usrdat);
            }
        };
        //-------------------------------------------------
        //普通函数回调指针对应的委托对象.
        template<class dummy>
        class delegate_t<void*,dummy> :public delegate_i
        {
            cb_func_t cb_std_func;
        public:
            //---------------------------------------------
            delegate_t(cb_func_t cf) :cb_std_func(cf) {}
            //---------------------------------------------
            //使用给定的参数调用事件回调.返回值:<0错误;0未正确处理;Event处理完成.
            uint32_t operator()(void *obj, void *p1, void *p2, void *usrdat)
            {
                return cb_std_func(obj, p1, p2, usrdat);
            }
        };

        //-------------------------------------------------
        uint8_t m_buff[sizeof(delegate_t<delegate_i,void>)]; //委托对象实体需要的空间
    public:
        //-------------------------------------------------
        delegate_def_t() {void *ptr=(void*)m_buff; *(size_t*)ptr = 0; }
        delegate_def_t(cb_func_t cf) { bind(cf); }
        template<class T>
        delegate_def_t(T& owner, uint32_t(T::*member_func)(void *obj, void *p1, void *p2, void *usrdat)) { bind(owner, member_func); }
        //-------------------------------------------------
        //进行成员函数回调指针绑定
        template<class T>
        void bind(T& owner, uint32_t(T::*member_func)(void *obj, void *p1, void *p2, void *usrdat) )
        {
            rx_static_assert(sizeof(m_buff) >= sizeof(delegate_t<T,void>) );
            ct::OC((delegate_t<T,void>*)m_buff, owner, member_func);//针对具体的委托源类T和其成员函数,动态生成委托对象
        }
        //-------------------------------------------------
        //进行普通函数回调指针绑定
        void bind(cb_func_t cf)
        {
            rx_static_assert(sizeof(m_buff) >= sizeof(delegate_t<void*,void>) );
            ct::OC((delegate_t<void*,void>*)m_buff, cf);     //针对给定的函数指针,动态生成委托对象
        }
        //-------------------------------------------------
        //判断当前回调委托对象是否有效(是否已经绑定过)
        bool is_valid() {void *ptr=(void*)m_buff; return (*((size_t*)ptr) != 0); }
        //-------------------------------------------------
        //对委托进行调用(内部调用真正的回调函数)
        uint32_t operator()(void *obj, void *p1, void *p2, void *usrdat)
        {
            void *ptr=(void*)m_buff;                        //消除gcc编译警告,将固定类型缓冲区转换为void*后再进行其他类型转换.
            delegate_i &cb=reinterpret_cast<delegate_i&>(*(delegate_i*)ptr);
            return cb(obj, p1, p2, usrdat);
        }
    };
}

#endif
