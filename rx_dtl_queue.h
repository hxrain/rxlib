#ifndef _RX_DTL_QUEUE_H_
#define _RX_DTL_QUEUE_H_

#include "rx_cc_macro.h"
#include "rx_dtl_list_raw.h"
#include "rx_dtl_stack.h"
#include "rx_mem_alloc.h"
#include "rx_mem_alloc_cntr.h"
#include "rx_str_tiny.h"

/*
    //-----------------------------------------------------
    //简单的队列容器(或单向链表,头尾快速访问)定义
    template<class DT>
    class queue_t;

    //语法糖,定义一个便于使用的整数队列
    typedef queue_t<uint32_t>        queue_uint32_t;
    typedef queue_t<int32_t>         queue_int32_t;
    //语法糖,定义一个便于使用的const char*内容持有队列
    typedef queue_t<const char*>     queue_cstr_t;
    //语法糖,定义一个便于使用的const wchar_t*内容持有队列
    typedef queue_t<const wchar_t*>  queue_wstr_t;
*/

namespace rx
{
    //-----------------------------------------------------
    //简单的队列容器(或单向链表,头尾快速访问)定义
    //-----------------------------------------------------
    template<class DT>
    class queue_t
    {
    public:
        typedef typename stack_t<DT>::node_t node_t;
    protected:
        //-------------------------------------------------
        typedef raw_queue_t<node_t>         cntr_t;         //最终使用的原始栈容器类型
        //-------------------------------------------------
        mem_allotter_i  &m_mem;                             //内存分配器,引用
        cntr_t           m_cntr;                            //底层栈容器
    public:
        //-------------------------------------------------
        queue_t():m_mem(rx_global_mem_allotter()){}
        queue_t(mem_allotter_i& ma):m_mem(ma){}
        virtual ~queue_t(){clear();}
        mem_allotter_i& mem(){return m_mem;}
        //-------------------------------------------------
        //定义简单的只读迭代器
        class iterator
        {
            const node_t  *m_node;
            friend class queue_t;
        public:
            //---------------------------------------------
            iterator() :m_node(NULL) {}
            iterator(const node_t *node) :m_node(node) {}
            iterator(const iterator &i):m_node(i.m_node){}
            //---------------------------------------------
            bool operator==(const iterator &i)const { return m_node == i.m_node; }
            bool operator!=(const iterator &i)const { return !(operator==(i));}
            //---------------------------------------------
            iterator& operator=(const iterator &i) {m_node=i.m_node; return *this;}
            //---------------------------------------------
            //* 运算符重载,用于获取当前节点的data
            const typename node_t::node_data_t& operator* () const {return m_node->data;}
            typename node_t::node_data_t* operator-> () const { return &m_node->data; }
            //---------------------------------------------
            iterator& operator++()
            {
                m_node=m_node->next;
                return reinterpret_cast<iterator&>(*this);
            }
        };
        //-------------------------------------------------
        //查看队首;返回遍历的初始位置
        iterator begin() const {return iterator(m_cntr.head());}
        //-------------------------------------------------
        //返回遍历的结束位置
        iterator end() const { return iterator(m_cntr.origin()); }
        //-------------------------------------------------
        //查看队尾
        iterator rbegin() const {return iterator(m_cntr.tail());}
        //-------------------------------------------------
        //释放全部节点
        void clear() {while(pop_front());}
        //-------------------------------------------------
        //内部元素数量
        uint32_t size() const {return m_cntr.size();}
        //-------------------------------------------------
        //数据入队(队尾),返回最新节点.
        template<class dt>
        iterator push_back(const dt &data)
        {
            uint32_t es=node_t::ext_size(data);
            node_t *node=(node_t *)m_mem.alloc(es+sizeof(node_t));
            if (!node) return end();
            m_cntr.push_back(node);
            node->OC(data,((uint8_t*)node+sizeof(node_t)),es);
            return iterator(node);
        }
        //-------------------------------------------------
        //数据入栈(队首),返回最新节点.
        template<class dt>
        iterator push_front(const dt &data)
        {
            uint32_t es=node_t::ext_size(data);
            node_t *node=(node_t *)m_mem.alloc(es+sizeof(node_t));
            if (!node) return end();
            m_cntr.push_front(node);
            node->OC(data,((uint8_t*)node+sizeof(node_t)),es);
            return iterator(node);
        }
        //-------------------------------------------------
        //弹出队首元素并释放.
        //返回值:空栈失败,否则成功.
        bool pop_front()
        {
            if (!size()) return false;
            node_t *node=m_cntr.pop_front();
            ct::OD(node);                                   //节点析构
            m_mem.free(node);                               //节点内存释放
            return true;
        }
    };

    //-----------------------------------------------------
    //语法糖,定义一个便于使用的整数队列
    typedef queue_t<uint32_t>        queue_uint32_t;
    typedef queue_t<int32_t>         queue_int32_t;
    //语法糖,定义一个便于使用的const char*内容持有队列
    typedef queue_t<const char*>     queue_cstr_t;
    //语法糖,定义一个便于使用的const wchar_t*内容持有队列
    typedef queue_t<const wchar_t*>  queue_wstr_t;
    
    //-----------------------------------------------------
    //利用队列容器封装一个简单的对象缓存池
    template<class T>
    class queue_cache_t
    {
        typedef queue_t<T*> obj_cache_t;
        obj_cache_t     m_cache;
    public:
        //-------------------------------------------------
        queue_cache_t():m_cache(rx_global_mem_allotter()){}
        queue_cache_t(mem_allotter_i& ma):m_cache(ma){}
        ~queue_cache_t(){clear();}
        //-------------------------------------------------
        T* get()
        {
            if (m_cache.size())
            {
                T *ret=*m_cache.begin();
                m_cache.pop_front();
                return ret;
            }
            else
                return m_cache.mem().new0();
        }
        //-------------------------------------------------
        void put(T* obj)
        {
            m_cache.push_back(obj);
        }
        //-------------------------------------------------
        void clear()
        {
            if (!m_cache.size()) return;
            do{
                m_cache.mem().del(*m_cache.begin());
            }while(m_cache.pop_front());
        }
    };
}

#endif