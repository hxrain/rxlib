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
        template <class T>	static inline void do_array_destroy(T,const uint32_t, const rx_type_true_t&) {}
        template <class T>	static inline void do_array_destroy(T* Array, const uint32_t Count, const rx_type_false_t&) {for (uint32_t i=0; i<Count; i++) OD(&Array[i]);}
        //--------------------------------------------------
        //��������ƫ�ػ��жϵ����鹹�캯��
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
        //���ж�����
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
        //���ж�������
        template <class T>
        static inline void OD(T* O) {O->~T();}


        //--------------------------------------------------
        //�������鹹��
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
        //������������
        template <class T>
        static inline void AD(T* Array,const uint32_t Count)
        {
            typedef typename rx_type_check_t<T>::has_trivial_destructor trivial_destructor;
            do_array_destroy(Array,Count, trivial_destructor());
        }
    };

    //=====================================================
    //�Զ���ʵ�����ֵ��Сֵ�ȽϹ���
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
    //��̬����,����Log2(num),����num������2����������
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
    //��̬����,����POW2(num),����num������2����������
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
    //����ָ��n�Ľ׳�(n<=170)
    inline double factorial(uint8_t n)
    {
        if (n>170) n=170;
        double result = 1;
        while (n > 1)
            result *= n--;
        return result;
    }
    //=====================================================
    //�ߴ����϶��봦��
    //=====================================================
    //Size�����������;AlignTo���뵽ָ���Ĵ�С�߽�
    template<uint32_t Size,uint32_t AlignTo=4>
    struct size_align
    {
        enum {result= (Size + AlignTo - 1) & ~(AlignTo - 1)};
    };
    //��һ������������AlignToΪ�߽���ж���
    inline uint32_t size_align_to(uint32_t Size,uint32_t AlignTo) {return (Size + AlignTo - 1) & ~(AlignTo - 1);}
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

    //ȡһ���ṹ��T�ĳ�ԱF������ڽṹ���ƫ����
    #define struct_offset(T,F) (uint32_t)(&((T*)0)->F)
    //�õ��ṹ��T��F1�ֶο�ʼ��F2֮ǰ�ֶεĿռ�ռ�óߴ�
    #define field_size(T,F1,F2) ((struct_offset(T,F2))-(struct_offset(T,F1)))

    //�﷨��,��ֹ���󿽱��Ŀ�ݺ궨��
    #define dont_copy(CN) const CN& operator=(const CN&)

    //���������϶����ȡ����
    inline uint32_t round_up(const double &num)
    {
        uint32_t rc = (uint32_t)num;
        return (num - rc > 0.00000001) ? rc + 1 : rc;
    }


    //-----------------------------------------------------
    //�ص�������ί�й��ܷ�װ(ʵ����С�����ض��ӿڳ�Ա��������ͨ������ί�е��û���.)
    //ʵ�ֵ�ԭ��:���յ�ί�ж���Ҫ�й����Ļ���ӿ�;��̬����ͬ��ί�ж�������ָ�����ڴ����,�Ϳ�����ͳһ�Ľӿ�ָ����з�����.
    //-----------------------------------------------------
    class delegate_def_t
    {
    public:
        //�ص�����ԭ��;��Ա����Ҳ��Ҫ����ͬ�ĸ�ʽ.
        //uint32_t cb_func_t(void *obj, void *p1, void *p2, void *usrdat)
        typedef uint32_t(*cb_func_t)(void *obj, void *p1, void *p2, void *usrdat);
    private:
        //-----------------------------------------------------
        //�¼��ص�ί�ж���ӿ�:����ͳһ����ӿ�.
        class delegate_i
        {
        public:
            virtual uint32_t operator()(void *obj, void *p1, void *p2, void *usrdat) = 0;
        };
        //-------------------------------------------------
        //��Ա�����ص�ָ���Ӧ��ί�ж���
        template<class T,class dummy>
        class delegate_t :public delegate_i
        {
            T              &owner;                          //ʹ�ó�Ա����ָ��,��Ҫ����ӵ���߶���ָ��
            uint32_t   (T::*cb_member_func)(void *obj, void *p1, void *p2, void *usrdat);
        public:
            delegate_t(T& Ths, uint32_t(T::*mf)(void *obj, void *p1, void *p2, void *usrdat)) :owner(Ths), cb_member_func(mf) {}
            //---------------------------------------------
            //ʹ�ø����Ĳ��������¼��ص�.����ֵ:<0����;0δ��ȷ����;Event�������.
            uint32_t operator()(void *obj, void *p1, void *p2, void *usrdat)
            {
                return (owner.*cb_member_func)(obj, p1, p2, usrdat);
            }
        };
        //-------------------------------------------------
        //��ͨ�����ص�ָ���Ӧ��ί�ж���.
        template<class dummy>
        class delegate_t<void*,dummy> :public delegate_i
        {
            cb_func_t cb_std_func;
        public:
            //---------------------------------------------
            delegate_t(cb_func_t cf) :cb_std_func(cf) {}
            //---------------------------------------------
            //ʹ�ø����Ĳ��������¼��ص�.����ֵ:<0����;0δ��ȷ����;Event�������.
            uint32_t operator()(void *obj, void *p1, void *p2, void *usrdat)
            {
                return cb_std_func(obj, p1, p2, usrdat);
            }
        };

        //-------------------------------------------------
        uint8_t m_buff[sizeof(delegate_t<delegate_i,void>)]; //ί�ж���ʵ����Ҫ�Ŀռ�
    public:
        //-------------------------------------------------
        delegate_def_t() {void *ptr=(void*)m_buff; *(size_t*)ptr = 0; }
        delegate_def_t(cb_func_t cf) { bind(cf); }
        template<class T>
        delegate_def_t(T& owner, uint32_t(T::*member_func)(void *obj, void *p1, void *p2, void *usrdat)) { bind(owner, member_func); }
        //-------------------------------------------------
        //���г�Ա�����ص�ָ���
        template<class T>
        void bind(T& owner, uint32_t(T::*member_func)(void *obj, void *p1, void *p2, void *usrdat) )
        {
            rx_static_assert(sizeof(m_buff) >= sizeof(delegate_t<T,void>) );
            ct::OC((delegate_t<T,void>*)m_buff, owner, member_func);//��Ծ����ί��Դ��T�����Ա����,��̬����ί�ж���
        }
        //-------------------------------------------------
        //������ͨ�����ص�ָ���
        void bind(cb_func_t cf)
        {
            rx_static_assert(sizeof(m_buff) >= sizeof(delegate_t<void*,void>) );
            ct::OC((delegate_t<void*,void>*)m_buff, cf);     //��Ը����ĺ���ָ��,��̬����ί�ж���
        }
        //-------------------------------------------------
        //�жϵ�ǰ�ص�ί�ж����Ƿ���Ч(�Ƿ��Ѿ��󶨹�)
        bool is_valid() {void *ptr=(void*)m_buff; return (*((size_t*)ptr) != 0); }
        //-------------------------------------------------
        //��ί�н��е���(�ڲ����������Ļص�����)
        uint32_t operator()(void *obj, void *p1, void *p2, void *usrdat)
        {
            void *ptr=(void*)m_buff;                        //����gcc���뾯��,���̶����ͻ�����ת��Ϊvoid*���ٽ�����������ת��.
            delegate_i &cb=reinterpret_cast<delegate_i&>(*(delegate_i*)ptr);
            return cb(obj, p1, p2, usrdat);
        }
    };
}

#endif
