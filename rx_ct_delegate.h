#ifndef _RX_CT_DELEGATE_H_
#define _RX_CT_DELEGATE_H_

#include "rx_cc_macro.h"
#include "rx_ct_util.h"

namespace rx
{
    /*
        轻量级回调函数的委托功能封装:
            委托对象要有公共的基类接口;动态将不同回调类型的委托对象构造在局部内存块上;利用统一接口进行访问.
    */

    //-----------------------------------------------------
    //一个参数的委托功能类
    //-----------------------------------------------------
    template<class PT1,class RT>
    class delegate1_rt                                      //(核心功能,不定义构造函数,可放入union)
    {
    public:
        //回调函数原型
        typedef RT (*cb_func_t)(PT1 P1, void*);

        //成员函数原型
        //RT m_func(PT1 P1)
    private:
        //-------------------------------------------------
        //事件回调委托对象接口:定义统一抽象接口.
        class delegate_i
        {
        public:
            virtual RT operator()(PT1 P1) = 0;
        };
        //-------------------------------------------------
        //成员函数回调指针对应的委托对象
        template<class T, class rt,class dummy>
        class delegate_t :public delegate_i
        {
            T              &owner;                          //使用成员函数指针,需要绑定其拥有者对象指针
            rt (T::*cb_member_func)(PT1 P1);
        public:
            delegate_t(T& Ths, rt (T::*mf)(PT1 P1)) :owner(Ths), cb_member_func(mf) {}
            rt operator()(PT1 P1) { return (owner.*cb_member_func)(P1); }
        };
        //-------------------------------------------------
        //成员函数回调指针对应的委托对象(无返回值特化)
        template<class T, class dummy>
        class delegate_t<T,void,dummy> :public delegate_i
        {
            T              &owner;                          //使用成员函数指针,需要绑定其拥有者对象指针
            RT (T::*cb_member_func)(PT1 P1);
        public:
            delegate_t(T& Ths, void (T::*mf)(PT1 P1)) :owner(Ths), cb_member_func(mf) {}
            RT operator()(PT1 P1) { (owner.*cb_member_func)(P1); }
        };
        //-------------------------------------------------
        //普通函数回调指针对应的委托对象.
        template<class rt,class dummy>
        class delegate_t<void*,rt, dummy> :public delegate_i
        {
            void           *owner;
            cb_func_t       cb_std_func;
        public:
            delegate_t(cb_func_t cf, void* obj = NULL) :owner(obj), cb_std_func(cf) {}
            rt operator()(PT1 P1) { return cb_std_func(P1, owner); }
        };
        //-------------------------------------------------
        //普通函数回调指针对应的委托对象(无返回值特化)
        template<class dummy>
        class delegate_t<void*,void, dummy> :public delegate_i
        {
            void           *owner;
            cb_func_t       cb_std_func;
        public:
            delegate_t(cb_func_t cf, void* obj = NULL) :owner(obj), cb_std_func(cf) {}
            void operator()(PT1 P1) { cb_std_func(P1, owner); }
        };
        //-------------------------------------------------
        //委托对象实体需要的空间
        uint8_t m_buff[sizeof(delegate_t<delegate_i,void, void>)];
    public:
        void reset() { void *ptr = (void*)m_buff; *(size_t*)ptr = 0; }
        //-------------------------------------------------
        //进行成员函数回调指针绑定
        template<class T>
        void bind(T& owner, RT (T::*member_func)(PT1 P1))
        {
            rx_static_assert(sizeof(m_buff) >= sizeof(delegate_t<T,RT, void>));
            ct::OC((delegate_t<T,RT, void>*)m_buff, owner, member_func);//针对具体的委托源类T和其成员函数,动态生成委托对象
        }
        //-------------------------------------------------
        //进行普通函数回调指针绑定,可附带绑定一个参数,在调用时内部给出.
        void bind(cb_func_t cf, void* obj = NULL)
        {
            rx_static_assert(sizeof(m_buff) >= sizeof(delegate_t<void*,RT, void>));
            ct::OC((delegate_t<void*,RT, void>*)m_buff, cf, obj);//针对给定的函数指针,动态生成委托对象
        }
        //-------------------------------------------------
        //判断当前回调委托对象是否有效(是否已经绑定过)
        bool is_valid() { void *ptr = (void*)&m_buff[0]; return (*((size_t*)ptr) != 0); }
        //-------------------------------------------------
        //对委托进行调用(内部调用真正的回调函数)
        RT operator()(PT1 P1)
        {
            void *ptr = (void*)m_buff;                        //消除gcc编译警告,将固定类型缓冲区转换为void*后再进行其他类型转换.
            delegate_i &cb = reinterpret_cast<delegate_i&>(*(delegate_i*)ptr);
            return cb(P1);
        }
    };
    //-----------------------------------------------------
    template<class PT1, class RT=uint32_t>
    class delegate1_t:public delegate1_rt<PT1,RT>           //(完整独立功能单参数的委托类)
    {
        typedef delegate1_rt<PT1,RT> super;
        typedef typename super::cb_func_t cb_func_t;
    public:
        //-------------------------------------------------
        delegate1_t() { super::reset(); }
        delegate1_t(cb_func_t f,void* obj = NULL) { super::reset(); super::bind(f,obj); }
        template<class T>
        delegate1_t(T& owner, RT (T::*member_func)(PT1 P1)) { super::reset(); super::bind(owner, member_func); }
    };

    //-----------------------------------------------------
    //两个参数的委托功能类
    //-----------------------------------------------------
    template<class PT1, class PT2, class RT>
    class delegate2_rt                                      //(核心功能,不定义构造函数,可放入union)
    {
    public:
        //回调函数原型
        typedef RT (*cb_func_t)(PT1 P1, PT2 P2, void*);

        //成员函数原型
        //RT m_func(PT1 P1, PT2 P2)
    protected:
        //-------------------------------------------------
        //事件回调委托对象接口:定义统一抽象接口.
        class delegate_i
        {
        public:
            virtual RT operator()(PT1 P1, PT2 P2) = 0;
        };
        //-------------------------------------------------
        template<class T,class rt, class dummy>
        class delegate_t :public delegate_i
        {
            T              &owner;
            rt (T::*cb_member_func)(PT1 P1, PT2 P2);
        public:
            delegate_t(T& Ths, rt (T::*mf)(PT1 P1, PT2 P2)) :owner(Ths), cb_member_func(mf) {}
            rt operator()(PT1 P1, PT2 P2) { return (owner.*cb_member_func)(P1, P2); }
        };
        //-------------------------------------------------
        template<class T, class dummy>
        class delegate_t<T,void,dummy> :public delegate_i
        {
            T              &owner;
            RT (T::*cb_member_func)(PT1 P1, PT2 P2);
        public:
            delegate_t(T& Ths, void (T::*mf)(PT1 P1, PT2 P2)) :owner(Ths), cb_member_func(mf) {}
            RT operator()(PT1 P1, PT2 P2) { (owner.*cb_member_func)(P1, P2); }
        };
        //-------------------------------------------------
        template<class rt,class dummy>
        class delegate_t<void*,rt,dummy> :public delegate_i
        {
            void           *owner;
            cb_func_t       cb_std_func;
        public:
            delegate_t(cb_func_t cf, void* obj = NULL) :owner(obj), cb_std_func(cf) {}
            rt operator()(PT1 P1, PT2 P2) { return cb_std_func(P1, P2, owner); }
        };
        //-------------------------------------------------
        template<class dummy>
        class delegate_t<void*,void, dummy> :public delegate_i
        {
            void           *owner;
            cb_func_t       cb_std_func;
        public:
            delegate_t(cb_func_t cf, void* obj = NULL) :owner(obj), cb_std_func(cf) {}
            void operator()(PT1 P1, PT2 P2) { cb_std_func(P1, P2, owner); }
        };
        //-------------------------------------------------
        uint8_t m_buff[sizeof(delegate_t<delegate_i,void, void>)];
    public:
        void reset() { void *ptr = (void*)m_buff; *(size_t*)ptr = 0; }
        //-------------------------------------------------
        template<class T>
        void bind(T& owner, RT (T::*member_func)(PT1 P1, PT2 P2)){ct::OC((delegate_t<T,RT, void>*)m_buff, owner, member_func);}
        //-------------------------------------------------
        void bind(cb_func_t cf, void* obj = NULL){ct::OC((delegate_t<void*,RT, void>*)m_buff, cf, obj);}
        //-------------------------------------------------
        bool is_valid() { void *ptr = (void*)&m_buff[0]; return (*((size_t*)ptr) != 0); }
        //-------------------------------------------------
        RT operator()(PT1 P1, PT2 P2)
        {
            void *ptr = (void*)m_buff;
            delegate_i &cb = reinterpret_cast<delegate_i&>(*(delegate_i*)ptr);
            return cb(P1, P2);
        }
    };

    //-------------------------------------------------
    template<class PT1, class PT2, class RT=uint32_t>
    class delegate2_t:public delegate2_rt<PT1,PT2,RT>
    {
        typedef delegate2_rt<PT1,PT2,RT> super;
        typedef typename super::cb_func_t cb_func_t;
    public:
        delegate2_t() { super::reset(); }
        delegate2_t(cb_func_t cf,void* obj = NULL) { super::reset(); super::bind(cf,obj); }
        template<class T>
        delegate2_t(T& owner, RT (T::*member_func)(PT1 P1, PT2 P2)) { super::reset(); super::bind(owner, member_func); }
    };

    //-----------------------------------------------------
    //三个参数的委托功能类
    //-----------------------------------------------------
    template<class PT1, class PT2, class PT3,class RT>
    class delegate3_rt                                      //(核心功能,不定义构造函数,可放入union)
    {
    public:
        //回调函数原型
        typedef RT (*cb_func_t)(PT1 P1, PT2 P2, PT3 P3, void*);

        //成员函数原型
        //RT m_func(PT1 P1, PT2 P2, PT3 P3)
    private:
        //-------------------------------------------------
        class delegate_i
        {
        public:
            virtual RT operator()(PT1 P1, PT2 P2, PT3 P3) = 0;
        };
        //-------------------------------------------------
        template<class T,class rt, class dummy>
        class delegate_t :public delegate_i
        {
            T              &owner;
            rt (T::*cb_member_func)(PT1 P1, PT2 P2, PT3 P3);
        public:
            delegate_t(T& Ths, rt (T::*mf)(PT1 P1, PT2 P2, PT3 P3)) :owner(Ths), cb_member_func(mf) {}
            rt operator()(PT1 P1, PT2 P2, PT3 P3) { return (owner.*cb_member_func)(P1, P2, P3); }
        };
        //-------------------------------------------------
        template<class T, class dummy>
        class delegate_t<T,void,dummy> :public delegate_i
        {
            T              &owner;
            RT (T::*cb_member_func)(PT1 P1, PT2 P2, PT3 P3);
        public:
            delegate_t(T& Ths, RT (T::*mf)(PT1 P1, PT2 P2, PT3 P3)) :owner(Ths), cb_member_func(mf) {}
            RT operator()(PT1 P1, PT2 P2, PT3 P3) { (owner.*cb_member_func)(P1, P2, P3); }
        };
        //-------------------------------------------------
        template<class rt,class dummy>
        class delegate_t<void*,rt, dummy> :public delegate_i
        {
            void           *owner;
            cb_func_t       cb_std_func;
        public:
            delegate_t(cb_func_t cf, void* obj = NULL) :owner(obj), cb_std_func(cf) {}
            rt operator()(PT1 P1, PT2 P2, PT3 P3) { return cb_std_func(P1, P2, P3, owner); }
        };
        //-------------------------------------------------
        template<class dummy>
        class delegate_t<void*,void, dummy> :public delegate_i
        {
            void           *owner;
            cb_func_t       cb_std_func;
        public:
            delegate_t(cb_func_t cf, void* obj = NULL) :owner(obj), cb_std_func(cf) {}
            RT operator()(PT1 P1, PT2 P2, PT3 P3) { return cb_std_func(P1, P2, P3, owner); }
        };
        //-------------------------------------------------
        uint8_t m_buff[sizeof(delegate_t<delegate_i,void, void>)];
    public:
        void reset() { void *ptr = (void*)m_buff; *(size_t*)ptr = 0; }
        //-------------------------------------------------
        template<class T>
        void bind(T& owner, RT (T::*member_func)(PT1 P1, PT2 P2, PT3 P3)){ct::OC((delegate_t<T,RT, void>*)m_buff, owner, member_func);}
        //-------------------------------------------------
        void bind(cb_func_t cf, void* obj = NULL){ct::OC((delegate_t<void*,RT, void>*)m_buff, cf, obj);}
        //-------------------------------------------------
        bool is_valid() { void *ptr = (void*)&m_buff[0]; return (*((size_t*)ptr) != 0); }
        //-------------------------------------------------
        RT operator()(PT1 P1, PT2 P2, PT3 P3)
        {
            void *ptr = (void*)m_buff;
            delegate_i &cb = reinterpret_cast<delegate_i&>(*(delegate_i*)ptr);
            return cb(P1, P2, P3);
        }
    };
    //-----------------------------------------------------
    template<class PT1, class PT2, class PT3, class RT=uint32_t>
    class delegate3_t:public delegate3_rt<PT1,PT2,PT3,RT>
    {
        typedef delegate3_rt<PT1,PT2,PT3,RT> super;
        typedef typename super::cb_func_t cb_func_t;
    public:
        //-------------------------------------------------
        delegate3_t() { super::reset(); }
        delegate3_t(cb_func_t cf,void* obj = NULL) { super::reset(); super::bind(cf,obj); }
        template<class T>
        delegate3_t(T& owner, RT (T::*member_func)(PT1 P1, PT2 P2, PT3 P3)) { super::reset(); super::bind(owner, member_func); }
    };


    //-----------------------------------------------------
    //四个参数的委托功能类
    //-----------------------------------------------------
    template<class PT1,class PT2,class PT3,class PT4,class RT>
    class delegate4_rt                                      //(核心功能,不定义构造函数,可放入union)
    {
    public:
        //回调函数原型
        typedef RT(*cb_func_t)(PT1 P1, PT2 P2, PT3 P3,PT4 P4,void*);

        //成员函数原型
        //RT m_func(PT1 &P1, PT2 &P2, PT3 &P3, PT4 &P4)
    private:
        //-------------------------------------------------
        class delegate_i
        {
        public:
            virtual RT operator()(PT1 P1, PT2 P2, PT3 P3,PT4 P4) = 0;
        };
        //-------------------------------------------------
        template<class T,class rt, class dummy>
        class delegate_t :public delegate_i
        {
            T              &owner;
            rt (T::*cb_member_func)(PT1 P1, PT2 P2, PT3 P3,PT4 P4);
        public:
            delegate_t(T& Ths, rt (T::*mf)(PT1 P1, PT2 P2, PT3 P3,PT4 P4)) :owner(Ths), cb_member_func(mf) {}
            rt operator()(PT1 P1, PT2 P2, PT3 P3,PT4 P4){return (owner.*cb_member_func)(P1,P2,P3,P4);}
        };
        //-------------------------------------------------
        template<class T, class dummy>
        class delegate_t<T,void,dummy> :public delegate_i
        {
            T              &owner;
            RT (T::*cb_member_func)(PT1 P1, PT2 P2, PT3 P3,PT4 P4);
        public:
            delegate_t(T& Ths, RT (T::*mf)(PT1 P1, PT2 P2, PT3 P3,PT4 P4)) :owner(Ths), cb_member_func(mf) {}
            RT operator()(PT1 P1, PT2 P2, PT3 P3,PT4 P4){(owner.*cb_member_func)(P1,P2,P3,P4);}
        };
        //-------------------------------------------------
        template<class rt,class dummy>
        class delegate_t<void*,rt, dummy> :public delegate_i
        {
            void           *owner;
            cb_func_t       cb_std_func;
        public:
            delegate_t(cb_func_t cf,void* obj=NULL) :owner(obj),cb_std_func(cf) {}
            rt operator()(PT1 P1, PT2 P2, PT3 P3,PT4 P4){return cb_std_func(P1,P2,P3,P4,owner);}
        };
        //-------------------------------------------------
        template<class dummy>
        class delegate_t<void*,void, dummy> :public delegate_i
        {
            void           *owner;
            cb_func_t       cb_std_func;
        public:
            delegate_t(cb_func_t cf,void* obj=NULL) :owner(obj),cb_std_func(cf) {}
            RT operator()(PT1 P1, PT2 P2, PT3 P3,PT4 P4){cb_std_func(P1,P2,P3,P4,owner);}
        };
        //-------------------------------------------------
        uint8_t m_buff[sizeof(delegate_t<delegate_i,void, void>)];
    public:
        //-------------------------------------------------
        void reset() { void *ptr = (void*)m_buff; *(size_t*)ptr = 0; }
        //-------------------------------------------------
        template<class T>
        void bind(T& owner, RT (T::*member_func)(PT1 P1, PT2 P2, PT3 P3,PT4 P4)){ct::OC((delegate_t<T,RT, void>*)m_buff, owner, member_func);}
        //-------------------------------------------------
        void bind(cb_func_t cf,void* obj=NULL){ct::OC((delegate_t<void*,RT, void>*)m_buff,cf,obj);}
        //-------------------------------------------------
        bool is_valid() { void *ptr = (void*)&m_buff[0]; return (*((size_t*)ptr) != 0); }
        //-------------------------------------------------
        RT operator()(PT1 P1, PT2 P2, PT3 P3,PT4 P4)
        {
            void *ptr = (void*)m_buff;
            delegate_i &cb = reinterpret_cast<delegate_i&>(*(delegate_i*)ptr);
            return cb(P1,P2,P3,P4);
        }
    };
    //-----------------------------------------------------
    template<class PT1, class PT2, class PT3, class PT4, class RT=uint32_t>
    class delegate4_t:public delegate4_rt<PT1,PT2,PT3,PT4,RT>
    {
        typedef delegate4_rt<PT1,PT2,PT3,PT4,RT> super;
        typedef typename super::cb_func_t cb_func_t;
    public:
        //-------------------------------------------------
        delegate4_t() { super::reset(); }
        delegate4_t(cb_func_t cf,void* obj = NULL) { super::reset(); super::bind(cf,obj); }
        template<class T>
        delegate4_t(T& owner, RT (T::*member_func)(PT1 P1, PT2 P2, PT3 P3, PT4 P4)) { super::reset(); super::bind(owner, member_func); }
    };
}
#endif
