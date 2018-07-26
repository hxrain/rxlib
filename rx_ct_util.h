#ifndef _H_RX_CT_TEMPLATE_UTIL_H_
#define _H_RX_CT_TEMPLATE_UTIL_H_
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
        template <class T>	static inline void do_array_destroy(T* Array, const uint32_t Count, const rx_type_false_t&){for (uint32_t i=0;i<Count;i++) OD(&Array[i]);}
        //--------------------------------------------------
        //��������ƫ�ػ��жϵ����鹹�캯��
        template <class T>  static inline void do_array_construct(T* p,const uint32_t Count,const rx_type_true_t&){}
        template <class T>	static inline void do_array_construct(T* p,const uint32_t Count,const rx_type_false_t&){for(uint32_t i=0;i<Count;i++) OC(p+i);}
        template <class T,class PT1>	static inline void do_array_construct(T* p,const uint32_t Count,PT1& P1,const rx_type_false_t&){for(uint32_t i=0;i<Count;i++) OC(p+i,P1);}
    public:
        //--------------------------------------------------
        //���ж�����
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
        //���ж�������
        template <class T>
        static inline void OD(T* O){O->~T();}


        //--------------------------------------------------
        //�������鹹��
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
    //��̬����,����Log(num),����num������2����������
    //=====================================================
    template<uint32_t num> class log2{};                    //num����2���������ݵ�ͨ��ģ��,��Ӧ�ñ�ʹ��
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
    //�ߴ����϶��봦��
    //=====================================================
    //Size�����������;AlignTo���뵽ָ���Ĵ�С�߽�
    template<uint32_t Size,uint32_t AlignTo=4>
    struct size_align
    {
        enum{result= (Size + AlignTo - 1) & ~(AlignTo - 1)};
    };
    //��һ������������AlignToΪ�߽���ж���
    inline uint32_t size_align_to(uint32_t Size,uint32_t AlignTo){return (Size + AlignTo - 1) & ~(AlignTo - 1);}
    //��һ������������4Ϊ�߽����
    #define size_align4(S) ((S+3)& ~3)
    //��һ������������8Ϊ�߽����
    #define size_align8(S) ((S+7)& ~7)

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

}

#endif
