#ifndef _RX_CT_DELEGATE_H_
#define _RX_CT_DELEGATE_H_

#include "rx_cc_macro.h"
#include "rx_ct_util.h"

namespace rx
{
    /*
        �������ص�������ί�й��ܷ�װ:
            ί�ж���Ҫ�й����Ļ���ӿ�;��̬����ͬ�ص����͵�ί�ж������ھֲ��ڴ����;����ͳһ�ӿڽ��з���.
            ÿ��ί�ж���,��VC/32bit����ռ��12BYTE;��VC/64bit����ռ��24BYTE;��gcc/64bit����ռ��32BYTE(��Ա����ָ��ռ��16BYTE);
    */

    //�﷨��,ƴװ���Ա����ָ����÷�
    #define cf_ptr(CLASS,FUNC) &CLASS::FUNC

    //-----------------------------------------------------
    //һ��������ί�й�����
    //-----------------------------------------------------
    template<class PT1,class RT>
    //(���Ĺ���,�����幹�캯��,�ɷ���union)
    class delegate1_rt
    {
    public:
        //�ص�����ԭ��
        typedef RT (*cb_func_t)(PT1 P1, void*);

        //��Ա����ԭ��
        //RT m_func(PT1 P1)
    private:
        //-------------------------------------------------
        //�¼��ص�ί�ж���ӿ�:����ͳһ����ӿ�.
        class delegate_i
        {
        public:
            virtual RT operator()(PT1 P1) = 0;
            virtual cb_func_t cb_func() { return NULL; }
        };
        //-------------------------------------------------
        //��Ա�����ص�ָ���Ӧ��ί�ж���
        template<class T, class rt,class dummy>
        class delegate_t :public delegate_i
        {
            T              &owner;                          //ʹ�ó�Ա����ָ��,��Ҫ����ӵ���߶���ָ��
            rt (T::*cb_member_func)(PT1 P1);
        public:
            delegate_t(T& Ths, rt (T::*mf)(PT1 P1)) :owner(Ths), cb_member_func(mf) {}
            rt operator()(PT1 P1) { return (owner.*cb_member_func)(P1); }
        };
        //-------------------------------------------------
        //��Ա�����ص�ָ���Ӧ��ί�ж���(�޷���ֵ�ػ�)
        template<class T, class dummy>
        class delegate_t<T,void,dummy> :public delegate_i
        {
            T              &owner;                          //ʹ�ó�Ա����ָ��,��Ҫ����ӵ���߶���ָ��
            RT (T::*cb_member_func)(PT1 P1);
        public:
            delegate_t(T& Ths, void (T::*mf)(PT1 P1)) :owner(Ths), cb_member_func(mf) {}
            RT operator()(PT1 P1) { (owner.*cb_member_func)(P1); }
        };
        //-------------------------------------------------
        //��ͨ�����ص�ָ���Ӧ��ί�ж���.
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
        //��ͨ�����ص�ָ���Ӧ��ί�ж���(�޷���ֵ�ػ�)
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
        //����ί�ж���ʵ����Ҫ�Ŀռ�
        uint8_t m_core[sizeof(delegate_t<delegate_i,void, void>)];
    public:
        void reset() { void *ptr = (void*)m_core; *(size_t*)ptr = 0; }
        //-------------------------------------------------
        //���г�Ա�����ص�ָ���
        template<class T>
        void bind(T& owner, RT (T::*member_func)(PT1 P1))
        {
            rx_static_assert(sizeof(m_core) >= sizeof(delegate_t<T,RT, void>));
            //(ί�й��ܵĺ���ԭ��,��̬�����Ӧ�����ݽṹ)��Ծ����ί��Դ��T�����Ա����,��̬����ί�ж���
            ct::OC((delegate_t<T,RT, void>*)m_core, owner, member_func);
        }
        //-------------------------------------------------
        //������ͨ�����ص�ָ���,�ɸ�����һ������,�ڵ���ʱ�ڲ�����.
        void bind(cb_func_t cf, void* dat = NULL)
        {
            rx_static_assert(sizeof(m_core) >= sizeof(delegate_t<void*,RT, void>));
            //(ί�й��ܵĺ���ԭ��,��̬�����Ӧ�����ݽṹ)��Ը����ĺ���ָ��,��̬����ί�ж���
            ct::OC((delegate_t<void*,RT, void>*)m_core, cf, dat);
        }
        //-------------------------------------------------
        //�жϵ�ǰ�ص�ί�ж����Ƿ���Ч(�Ƿ��Ѿ��󶨹�)
        bool is_valid() { void *ptr = (void*)&m_core[0]; return (*((size_t*)ptr) != 0); }
        //-------------------------------------------------
        //��ί�н��е���(�ڲ����������Ļص�����)
        RT operator()(PT1 P1)
        {
            void *ptr = (void*)m_core;                        //����gcc���뾯��,���̶����ͻ�����ת��Ϊvoid*���ٽ�����������ת��.
            delegate_i &cb = reinterpret_cast<delegate_i&>(*(delegate_i*)ptr);
            return cb(P1);
        }
        //-------------------------------------------------
        //��ȡ�󶨵Ļص�����
        cb_func_t cb_func()
        {
            void *ptr = (void*)m_core;
            delegate_i &cb = reinterpret_cast<delegate_i&>(*(delegate_i*)ptr);
            return cb.cb_func();
        }
    };
    //-----------------------------------------------------
    template<class PT1, class RT=uint32_t>
    //�ص�����ӵ��һ��������������������ί����
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
    //�ص�������������������ί�й�����
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
    //�ص�������������������ί�й�����
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
    //�ص����������ĸ�������ί�й�����
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
