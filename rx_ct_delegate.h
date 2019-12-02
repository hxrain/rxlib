#ifndef _RX_CT_DELEGATE_H_
#define _RX_CT_DELEGATE_H_

#include "rx_cc_macro.h"
#include "rx_ct_util.h"

namespace rx
{
    /*
        轻量级回调函数的委托功能封装:
            委托对象要有公共的基类接口;动态将不同回调类型的委托对象构造在局部内存块上;利用统一接口进行访问.
            每个委托对象,在VC/32bit环境占用12BYTE;在VC/64bit环境占用24BYTE;在gcc/64bit环境占用32BYTE(成员函数指针占用16BYTE);
    */

    //语法糖,拼装类成员函数指针的用法
    #define cf_ptr(CLASS,FUNC) &CLASS::FUNC

    //-----------------------------------------------------
    //一个参数的委托功能类
    //-----------------------------------------------------
    template<class PT1,class RT>
    //(核心功能,不定义构造函数,可放入union)
    class delegate1_rt
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
            virtual cb_func_t cb_func() { return NULL; }
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
            void           *usrdat;
            cb_func_t       cb_std_func;
        public:
            delegate_t(cb_func_t cf, void* dat = NULL) :usrdat(dat), cb_std_func(cf) {}
            rt operator()(PT1 P1) { return cb_std_func(P1, usrdat); }
            virtual cb_func_t cb_func() { return cb_std_func; }
        };
        //-------------------------------------------------
        //普通函数回调指针对应的委托对象(无返回值特化)
        template<class dummy>
        class delegate_t<void*,void, dummy> :public delegate_i
        {
            void           *usrdat;
            cb_func_t       cb_std_func;
        public:
            delegate_t(cb_func_t cf, void* dat = NULL) :usrdat(dat), cb_std_func(cf) {}
            void operator()(PT1 P1) { cb_std_func(P1, usrdat); }
            virtual cb_func_t cb_func() { return cb_std_func; }
        };
        //-------------------------------------------------
        //定义委托对象实体需要的空间
        uint8_t m_core[sizeof(delegate_t<delegate_i,void, void>)];
    public:
        void reset() { void *ptr = (void*)m_core; *(size_t*)ptr = 0; }
        //-------------------------------------------------
        //进行成员函数回调指针绑定
        template<class T>
        void bind(T& owner, RT (T::*member_func)(PT1 P1))
        {
            rx_static_assert(sizeof(m_core) >= sizeof(delegate_t<T,RT, void>));
            //(委托功能的核心原理,动态构造对应的数据结构)针对具体的委托源类T和其成员函数,动态生成委托对象
            ct::OC((delegate_t<T,RT, void>*)m_core, owner, member_func);
        }
        //-------------------------------------------------
        //进行普通函数回调指针绑定,可附带绑定一个参数,在调用时内部给出.
        void bind(cb_func_t cf, void* dat = NULL)
        {
            rx_static_assert(sizeof(m_core) >= sizeof(delegate_t<void*,RT, void>));
            //(委托功能的核心原理,动态构造对应的数据结构)针对给定的函数指针,动态生成委托对象
            ct::OC((delegate_t<void*,RT, void>*)m_core, cf, dat);
        }
        //-------------------------------------------------
        //判断当前回调委托对象是否有效(是否已经绑定过)
        bool is_valid() { void *ptr = (void*)&m_core[0]; return (*((size_t*)ptr) != 0); }
        //-------------------------------------------------
        //对委托进行调用(内部调用真正的回调函数)
        RT operator()(PT1 P1)
        {
            void *ptr = (void*)m_core;                        //消除gcc编译警告,将固定类型缓冲区转换为void*后再进行其他类型转换.
            delegate_i &cb = reinterpret_cast<delegate_i&>(*(delegate_i*)ptr);
            return cb(P1);
        }
        //-------------------------------------------------
        //获取绑定的回调函数
        cb_func_t cb_func()
        {
            void *ptr = (void*)m_core;
            delegate_i &cb = reinterpret_cast<delegate_i&>(*(delegate_i*)ptr);
            return cb.cb_func();
        }
    };
    //-----------------------------------------------------
    template<class PT1, class RT=uint32_t>
    //回调函数拥有一个参数的完整独立功能委托类
    class delegate1_t:public delegate1_rt<PT1,RT>
    {
        typedef delegate1_rt<PT1,RT> super;
    public:
        typedef typename super::cb_func_t cb_func_t;
        //-------------------------------------------------
        delegate1_t() { super::reset(); }
        delegate1_t(cb_func_t f,void* dat = NULL) { super::reset(); super::bind(f,dat); }
        template<class T>
        delegate1_t(T& owner, RT (T::*member_func)(PT1 P1)) { super::reset(); super::bind(owner, member_func); }
    };


    //-----------------------------------------------------
    //回调函数具有两个参数的委托功能类
    //-----------------------------------------------------
    template<class PT1, class PT2, class RT>
    class delegate2_rt
    {
    public:
        typedef RT (*cb_func_t)(PT1 P1, PT2 P2, void*);
        //RT m_func(PT1 P1, PT2 P2)
    protected:
        class delegate_i
        {
        public:
            virtual RT operator()(PT1 P1, PT2 P2) = 0;
            virtual cb_func_t cb_func() { return NULL; }
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
            void           *usrdat;
            cb_func_t       cb_std_func;
        public:
            delegate_t(cb_func_t cf, void* dat = NULL) :usrdat(dat), cb_std_func(cf) {}
            rt operator()(PT1 P1, PT2 P2) { return cb_std_func(P1, P2, usrdat); }
            virtual cb_func_t cb_func() { return cb_std_func; }
        };
        //-------------------------------------------------
        template<class dummy>
        class delegate_t<void*,void, dummy> :public delegate_i
        {
            void           *usrdat;
            cb_func_t       cb_std_func;
        public:
            delegate_t(cb_func_t cf, void* dat = NULL) :usrdat(dat), cb_std_func(cf) {}
            void operator()(PT1 P1, PT2 P2) { cb_std_func(P1, P2, usrdat); }
            virtual cb_func_t cb_func() { return cb_std_func; }
        };
        //-------------------------------------------------
        uint8_t m_core[sizeof(delegate_t<delegate_i,void, void>)];
    public:
        void reset() { void *ptr = (void*)m_core; *(size_t*)ptr = 0; }
        //-------------------------------------------------
        template<class T>
        void bind(T& owner, RT (T::*member_func)(PT1 P1, PT2 P2)){ct::OC((delegate_t<T,RT, void>*)m_core, owner, member_func);}
        //-------------------------------------------------
        void bind(cb_func_t cf, void* dat = NULL){ct::OC((delegate_t<void*,RT, void>*)m_core, cf, dat);}
        //-------------------------------------------------
        bool is_valid() { void *ptr = (void*)&m_core[0]; return (*((size_t*)ptr) != 0); }
        //-------------------------------------------------
        RT operator()(PT1 P1, PT2 P2)
        {
            void *ptr = (void*)m_core;
            delegate_i &cb = reinterpret_cast<delegate_i&>(*(delegate_i*)ptr);
            return cb(P1, P2);
        }
        //-------------------------------------------------
        cb_func_t cb_func()
        {
            void *ptr = (void*)m_core;
            delegate_i &cb = reinterpret_cast<delegate_i&>(*(delegate_i*)ptr);
            return cb.cb_func();
        }
    };

    //-------------------------------------------------
    template<class PT1, class PT2, class RT=uint32_t>
    class delegate2_t:public delegate2_rt<PT1,PT2,RT>
    {
        typedef delegate2_rt<PT1,PT2,RT> super;
    public:
        typedef typename super::cb_func_t cb_func_t;
        delegate2_t() { super::reset(); }
        delegate2_t(cb_func_t cf,void* dat = NULL) { super::reset(); super::bind(cf,dat); }
        template<class T>
        delegate2_t(T& owner, RT (T::*member_func)(PT1 P1, PT2 P2)) { super::reset(); super::bind(owner, member_func); }
    };


    //-----------------------------------------------------
    //回调函数具有三个参数的委托功能类
    //-----------------------------------------------------
    template<class PT1, class PT2, class PT3,class RT>
    class delegate3_rt
    {
    public:
        typedef RT (*cb_func_t)(PT1 P1, PT2 P2, PT3 P3, void*);
        //RT m_func(PT1 P1, PT2 P2, PT3 P3)
    private:
        //-------------------------------------------------
        class delegate_i
        {
        public:
            virtual RT operator()(PT1 P1, PT2 P2, PT3 P3) = 0;
            virtual cb_func_t cb_func() { return NULL; }
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
            void           *usrdat;
            cb_func_t       cb_std_func;
        public:
            delegate_t(cb_func_t cf, void* dat = NULL) :usrdat(dat), cb_std_func(cf) {}
            rt operator()(PT1 P1, PT2 P2, PT3 P3) { return cb_std_func(P1, P2, P3, usrdat); }
            virtual cb_func_t cb_func() { return cb_std_func; }
        };
        //-------------------------------------------------
        template<class dummy>
        class delegate_t<void*,void, dummy> :public delegate_i
        {
            void           *usrdat;
            cb_func_t       cb_std_func;
        public:
            delegate_t(cb_func_t cf, void* dat = NULL) :usrdat(dat), cb_std_func(cf) {}
            RT operator()(PT1 P1, PT2 P2, PT3 P3) { return cb_std_func(P1, P2, P3, usrdat); }
            virtual cb_func_t cb_func() { return cb_std_func; }
        };
        //-------------------------------------------------
        uint8_t m_core[sizeof(delegate_t<delegate_i,void, void>)];
    public:
        void reset() { void *ptr = (void*)m_core; *(size_t*)ptr = 0; }
        //-------------------------------------------------
        template<class T>
        void bind(T& owner, RT (T::*member_func)(PT1 P1, PT2 P2, PT3 P3)){ct::OC((delegate_t<T,RT, void>*)m_core, owner, member_func);}
        //-------------------------------------------------
        void bind(cb_func_t cf, void* dat = NULL){ct::OC((delegate_t<void*,RT, void>*)m_core, cf, dat);}
        //-------------------------------------------------
        bool is_valid() { void *ptr = (void*)&m_core[0]; return (*((size_t*)ptr) != 0); }
        //-------------------------------------------------
        RT operator()(PT1 P1, PT2 P2, PT3 P3)
        {
            void *ptr = (void*)m_core;
            delegate_i &cb = reinterpret_cast<delegate_i&>(*(delegate_i*)ptr);
            return cb(P1, P2, P3);
        }
        //-------------------------------------------------
        cb_func_t cb_func()
        {
            void *ptr = (void*)m_core;
            delegate_i &cb = reinterpret_cast<delegate_i&>(*(delegate_i*)ptr);
            return cb.cb_func();
        }
    };
    //-----------------------------------------------------
    template<class PT1, class PT2, class PT3, class RT=uint32_t>
    class delegate3_t:public delegate3_rt<PT1,PT2,PT3,RT>
    {
        typedef delegate3_rt<PT1,PT2,PT3,RT> super;
    public:
        typedef typename super::cb_func_t cb_func_t;
        //-------------------------------------------------
        delegate3_t() { super::reset(); }
        delegate3_t(cb_func_t cf,void* dat = NULL) { super::reset(); super::bind(cf,dat); }
        template<class T>
        delegate3_t(T& owner, RT (T::*member_func)(PT1 P1, PT2 P2, PT3 P3)) { super::reset(); super::bind(owner, member_func); }
    };


    //-----------------------------------------------------
    //回调函数具有四个参数的委托功能类
    //-----------------------------------------------------
    template<class PT1,class PT2,class PT3,class PT4,class RT>
    class delegate4_rt
    {
    public:
        typedef RT(*cb_func_t)(PT1 P1, PT2 P2, PT3 P3,PT4 P4,void*);
        //RT m_func(PT1 &P1, PT2 &P2, PT3 &P3, PT4 &P4)
    private:
        //-------------------------------------------------
        class delegate_i
        {
        public:
            virtual RT operator()(PT1 P1, PT2 P2, PT3 P3,PT4 P4) = 0;
            virtual cb_func_t cb_func() { return NULL; }
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
            void           *usrdat;
            cb_func_t       cb_std_func;
        public:
            delegate_t(cb_func_t cf,void* dat=NULL) :usrdat(dat),cb_std_func(cf) {}
            rt operator()(PT1 P1, PT2 P2, PT3 P3,PT4 P4){return cb_std_func(P1,P2,P3,P4, usrdat);}
            virtual cb_func_t cb_func() { return cb_std_func; }
        };
        //-------------------------------------------------
        template<class dummy>
        class delegate_t<void*,void, dummy> :public delegate_i
        {
            void           *usrdat;
            cb_func_t       cb_std_func;
        public:
            delegate_t(cb_func_t cf,void* dat=NULL) :usrdat(dat),cb_std_func(cf) {}
            RT operator()(PT1 P1, PT2 P2, PT3 P3,PT4 P4){cb_std_func(P1,P2,P3,P4, usrdat);}
            virtual cb_func_t cb_func() { return cb_std_func; }
        };
        //-------------------------------------------------
        uint8_t m_core[sizeof(delegate_t<delegate_i,void, void>)];
    public:
        //-------------------------------------------------
        void reset() { void *ptr = (void*)m_core; *(size_t*)ptr = 0; }
        //-------------------------------------------------
        template<class T>
        void bind(T& owner, RT (T::*member_func)(PT1 P1, PT2 P2, PT3 P3,PT4 P4)){ct::OC((delegate_t<T,RT, void>*)m_core, owner, member_func);}
        //-------------------------------------------------
        void bind(cb_func_t cf,void* dat=NULL){ct::OC((delegate_t<void*,RT, void>*)m_core,cf,dat);}
        //-------------------------------------------------
        bool is_valid() { void *ptr = (void*)&m_core[0]; return (*((size_t*)ptr) != 0); }
        //-------------------------------------------------
        RT operator()(PT1 P1, PT2 P2, PT3 P3,PT4 P4)
        {
            void *ptr = (void*)m_core;
            delegate_i &cb = reinterpret_cast<delegate_i&>(*(delegate_i*)ptr);
            return cb(P1,P2,P3,P4);
        }
        //-------------------------------------------------
        cb_func_t cb_func()
        {
            void *ptr = (void*)m_core;
            delegate_i &cb = reinterpret_cast<delegate_i&>(*(delegate_i*)ptr);
            return cb.cb_func();
        }
    };
    //-----------------------------------------------------
    template<class PT1, class PT2, class PT3, class PT4, class RT=uint32_t>
    class delegate4_t:public delegate4_rt<PT1,PT2,PT3,PT4,RT>
    {
        typedef delegate4_rt<PT1,PT2,PT3,PT4,RT> super;
    public:
        typedef typename super::cb_func_t cb_func_t;
        //-------------------------------------------------
        delegate4_t() { super::reset(); }
        delegate4_t(cb_func_t cf,void* dat = NULL) { super::reset(); super::bind(cf,dat); }
        template<class T>
        delegate4_t(T& owner, RT (T::*member_func)(PT1 P1, PT2 P2, PT3 P3, PT4 P4)) { super::reset(); super::bind(owner, member_func); }
    };
}
#endif
