#ifndef _RX_CT_DELEGATE_H_
#define _RX_CT_DELEGATE_H_

#include "rx_cc_macro.h"
#include "rx_ct_util.h"

namespace rx
{
    /*
        �������ص�������ί�й��ܷ�װ:
            ί�ж���Ҫ�й����Ļ���ӿ�;��̬����ͬ�ص����͵�ί�ж������ھֲ��ڴ����;����ͳһ�ӿڽ��з���.
    */

    //-----------------------------------------------------
    //һ��������ί�й�����
    //-----------------------------------------------------
    template<class PT1,class RT>
    class delegate1_rt                                      //(���Ĺ���,�����幹�캯��,�ɷ���union)
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
            void           *owner;
            cb_func_t       cb_std_func;
        public:
            delegate_t(cb_func_t cf, void* obj = NULL) :owner(obj), cb_std_func(cf) {}
            rt operator()(PT1 P1) { return cb_std_func(P1, owner); }
        };
        //-------------------------------------------------
        //��ͨ�����ص�ָ���Ӧ��ί�ж���(�޷���ֵ�ػ�)
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
        //ί�ж���ʵ����Ҫ�Ŀռ�
        uint8_t m_buff[sizeof(delegate_t<delegate_i,void, void>)];
    public:
        void reset() { void *ptr = (void*)m_buff; *(size_t*)ptr = 0; }
        //-------------------------------------------------
        //���г�Ա�����ص�ָ���
        template<class T>
        void bind(T& owner, RT (T::*member_func)(PT1 P1))
        {
            rx_static_assert(sizeof(m_buff) >= sizeof(delegate_t<T,RT, void>));
            ct::OC((delegate_t<T,RT, void>*)m_buff, owner, member_func);//��Ծ����ί��Դ��T�����Ա����,��̬����ί�ж���
        }
        //-------------------------------------------------
        //������ͨ�����ص�ָ���,�ɸ�����һ������,�ڵ���ʱ�ڲ�����.
        void bind(cb_func_t cf, void* obj = NULL)
        {
            rx_static_assert(sizeof(m_buff) >= sizeof(delegate_t<void*,RT, void>));
            ct::OC((delegate_t<void*,RT, void>*)m_buff, cf, obj);//��Ը����ĺ���ָ��,��̬����ί�ж���
        }
        //-------------------------------------------------
        //�жϵ�ǰ�ص�ί�ж����Ƿ���Ч(�Ƿ��Ѿ��󶨹�)
        bool is_valid() { void *ptr = (void*)&m_buff[0]; return (*((size_t*)ptr) != 0); }
        //-------------------------------------------------
        //��ί�н��е���(�ڲ����������Ļص�����)
        RT operator()(PT1 P1)
        {
            void *ptr = (void*)m_buff;                        //����gcc���뾯��,���̶����ͻ�����ת��Ϊvoid*���ٽ�����������ת��.
            delegate_i &cb = reinterpret_cast<delegate_i&>(*(delegate_i*)ptr);
            return cb(P1);
        }
    };
    //-----------------------------------------------------
    template<class PT1, class RT=uint32_t>
    class delegate1_t:public delegate1_rt<PT1,RT>           //(�����������ܵ�������ί����)
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
    //����������ί�й�����
    //-----------------------------------------------------
    template<class PT1, class PT2, class RT>
    class delegate2_rt                                      //(���Ĺ���,�����幹�캯��,�ɷ���union)
    {
    public:
        //�ص�����ԭ��
        typedef RT (*cb_func_t)(PT1 P1, PT2 P2, void*);

        //��Ա����ԭ��
        //RT m_func(PT1 P1, PT2 P2)
    protected:
        //-------------------------------------------------
        //�¼��ص�ί�ж���ӿ�:����ͳһ����ӿ�.
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
    //����������ί�й�����
    //-----------------------------------------------------
    template<class PT1, class PT2, class PT3,class RT>
    class delegate3_rt                                      //(���Ĺ���,�����幹�캯��,�ɷ���union)
    {
    public:
        //�ص�����ԭ��
        typedef RT (*cb_func_t)(PT1 P1, PT2 P2, PT3 P3, void*);

        //��Ա����ԭ��
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
    //�ĸ�������ί�й�����
    //-----------------------------------------------------
    template<class PT1,class PT2,class PT3,class PT4,class RT>
    class delegate4_rt                                      //(���Ĺ���,�����幹�캯��,�ɷ���union)
    {
    public:
        //�ص�����ԭ��
        typedef RT(*cb_func_t)(PT1 P1, PT2 P2, PT3 P3,PT4 P4,void*);

        //��Ա����ԭ��
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
