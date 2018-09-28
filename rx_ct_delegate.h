#ifndef _RX_CT_DELEGATE_H_
#define _RX_CT_DELEGATE_H_

#include "rx_cc_macro.h"
#include "rx_ct_util.h"

namespace rx
{
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
        delegate_def_t() {reset(); }
        delegate_def_t(cb_func_t cf) { reset(); bind(cf); }
        template<class T>
        delegate_def_t(T& owner, uint32_t(T::*member_func)(void *obj, void *p1, void *p2, void *usrdat)) { reset(); bind(owner, member_func); }
        void reset(){void *ptr=(void*)m_buff; *(size_t*)ptr = 0;}
        //-------------------------------------------------
        //���г�Ա�����ص�ָ���
        template<class T>
        bool bind(T& owner, uint32_t(T::*member_func)(void *obj, void *p1, void *p2, void *usrdat) )
        {
            if (is_valid()) return false;
            rx_static_assert(sizeof(m_buff) >= sizeof(delegate_t<T,void>) );
            ct::OC((delegate_t<T,void>*)m_buff, owner, member_func);//��Ծ����ί��Դ��T�����Ա����,��̬����ί�ж���
            return true;
        }
        //-------------------------------------------------
        //������ͨ�����ص�ָ���
        bool bind(cb_func_t cf)
        {
            if (is_valid()) return false;
            rx_static_assert(sizeof(m_buff) >= sizeof(delegate_t<void*,void>) );
            ct::OC((delegate_t<void*,void>*)m_buff, cf);     //��Ը����ĺ���ָ��,��̬����ί�ж���
            return true;
        }
        //-------------------------------------------------
        //�жϵ�ǰ�ص�ί�ж����Ƿ���Ч(�Ƿ��Ѿ��󶨹�)
        bool is_valid() {void *ptr=(void*)&m_buff[0]; return (*((size_t*)ptr) != 0); }
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