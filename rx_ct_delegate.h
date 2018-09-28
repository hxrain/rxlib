#ifndef _RX_CT_DELEGATE_H_
#define _RX_CT_DELEGATE_H_

#include "rx_cc_macro.h"
#include "rx_ct_util.h"

namespace rx
{
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
        delegate_def_t() {reset(); }
        delegate_def_t(cb_func_t cf) { reset(); bind(cf); }
        template<class T>
        delegate_def_t(T& owner, uint32_t(T::*member_func)(void *obj, void *p1, void *p2, void *usrdat)) { reset(); bind(owner, member_func); }
        void reset(){void *ptr=(void*)m_buff; *(size_t*)ptr = 0;}
        //-------------------------------------------------
        //进行成员函数回调指针绑定
        template<class T>
        bool bind(T& owner, uint32_t(T::*member_func)(void *obj, void *p1, void *p2, void *usrdat) )
        {
            if (is_valid()) return false;
            rx_static_assert(sizeof(m_buff) >= sizeof(delegate_t<T,void>) );
            ct::OC((delegate_t<T,void>*)m_buff, owner, member_func);//针对具体的委托源类T和其成员函数,动态生成委托对象
            return true;
        }
        //-------------------------------------------------
        //进行普通函数回调指针绑定
        bool bind(cb_func_t cf)
        {
            if (is_valid()) return false;
            rx_static_assert(sizeof(m_buff) >= sizeof(delegate_t<void*,void>) );
            ct::OC((delegate_t<void*,void>*)m_buff, cf);     //针对给定的函数指针,动态生成委托对象
            return true;
        }
        //-------------------------------------------------
        //判断当前回调委托对象是否有效(是否已经绑定过)
        bool is_valid() {void *ptr=(void*)&m_buff[0]; return (*((size_t*)ptr) != 0); }
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