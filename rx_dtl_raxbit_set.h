#ifndef _RX_DTL_RAXBIT_SET_H_
#define _RX_DTL_RAXBIT_SET_H_

#include "rx_cc_macro.h"
#include "rx_dtl_raxbit_raw.h"
#include "rx_ct_util.h"

/*
    <基于原始bit分段radix树封装的集合容器>
    template<class val_t,class op_t= raxbit_op_int<val_t> >
    class raxbit_set_base_t;

    <默认节点为uint32_t类型的rax集合>
    template<class key_t=uint32_t,class op_t= raxbit_op_int<key_t> >
    class raxbit_set_t;
*/

namespace rx
{
    //-----------------------------------------------------
    //基于原始bit分段rax封装的集合容器
    //-----------------------------------------------------
    template<class val_t,class op_t= raxbit_op_int<val_t> >
    class raxbit_set_base_t
    {
        typedef raw_raxbit_t<val_t, op_t> raw_rax_t;
    public:
        //-------------------------------------------------
        //节点类型
        typedef typename raw_rax_t::leaf_t node_t;

        //-------------------------------------------------
        //定义迭代器辅助类
        class looper_t :public raw_rax_t::back_path_t
        {
            friend class iterator;
            friend class raxbit_set_base_t;
            raw_rax_t       &m_rax;
            node_t          *m_node;
            looper_t(raw_rax_t &r) :m_rax(r), m_node(NULL) {}
        public:
            looper_t(raxbit_set_base_t &set) :m_rax(set.m_rax), m_node(NULL) {}
        };

        //-------------------------------------------------
        //定义简单的只读迭代器
        class iterator
        {
            friend class raxbit_set_base_t;
            const looper_t   *m_looper;                     //由于有辅助looper_t的存在,所以迭代器中仅记录辅助对象即可
        public:
            //---------------------------------------------
            iterator(const looper_t &s) :m_looper(&s) {}
            //---------------------------------------------
            bool operator==(const iterator &i)const { return m_looper->m_node == i.m_looper->m_node; }
            bool operator!=(const iterator &i)const { return !(operator==(i)); }
            //---------------------------------------------
            iterator& operator=(const iterator &i) { m_looper = i.m_looper; return *this; }
            //---------------------------------------------
            const val_t& operator*() const
            {
                rx_assert(m_looper!= NULL);
                rx_assert(m_looper->m_node != NULL);
                return m_looper->m_node->key;
            }
            //---------------------------------------------
            //节点指向后移(前置运算符模式,未提供后置模式)
            iterator& operator++()
            {
                typename raw_rax_t::back_path_t *path=(typename raw_rax_t::back_path_t*)m_looper;
                ((looper_t*)m_looper)->m_node = m_looper->m_rax.next(*path);
                return reinterpret_cast<iterator&>(*this);
            }
            //---------------------------------------------
            //节点指向后移(前置运算符模式,未提供后置模式)
            iterator& operator--()
            {
                typename raw_rax_t::back_path_t *path=(typename raw_rax_t::back_path_t*)m_looper;
                ((looper_t*)m_looper)->m_node = m_looper->m_rax.prev(*path);
                return reinterpret_cast<iterator&>(*this);
            }
        };
    protected:
        raw_rax_t   m_rax;                              //底层rax容器
        looper_t    m_dummy_end;                        //便于end()使用
        looper_t    m_dummy_begin;                      //便于begin()使用
    public:
        //-------------------------------------------------
        raxbit_set_base_t():m_dummy_end(m_rax), m_dummy_begin(m_rax){}
        raxbit_set_base_t(mem_allotter_i &m):m_rax(m),m_dummy_end(m_rax), m_dummy_begin(m_rax) {}
        virtual ~raxbit_set_base_t() { clear(); }
        //-------------------------------------------------
        //枝干数量
        uint32_t limbs() const { return m_rax.limbs(); }
        //已经使用的叶子节点数量
        uint32_t size() const { return m_rax.size(); }
        //-------------------------------------------------
        //在集合中插入元素并赋值构造
        template<class VT>
        bool insert(const VT &val)
        {
            bool dup;
            return insert(val, dup);
        }
        template<class VT>
        bool insert(const VT &val,bool &dup)
        {
            node_t *node = m_rax.insert(val, dup);
            if (!node)
                return false;
            ct::OC(&node->key, val);
            return true;
        }
        //-------------------------------------------------
        //查找元素,判断是否存在
        template<class VT>
        bool find(const VT &val) const { return m_rax.find(val) != NULL; }
        template<class VT>
        bool operator[](const VT &val) const { return find(val); }
        //-------------------------------------------------
        //查找元素,同时获得遍历迭代器
        template<class VT>
        iterator find(const VT &val, looper_t &looper) const
        {
            looper.m_node = m_rax.find(val,looper);
            if (looper.m_node == NULL) return end();
            return iterator(looper);
        }
        //-------------------------------------------------
        //删除元素并默认析构,可以指定是否进行删除后的空洞校正
        template<class VT>
        bool erase(const VT &val)
        {
            typename raw_rax_t::ref_path_t path;
            node_t *node = m_rax.find(val,path);
            if (!node) return false;
            ct::OD(&node->key);
            return m_rax.remove(node, path);
        }
        //-------------------------------------------------
        //获取最小key对应的迭代器;需要辅助的looper遍历器
        iterator begin(looper_t &looper) const
        {
            looper.m_node = m_rax.left(looper);
            return iterator(looper);
        }
        //单线程简单使用的迭代器初始方法,迭代需要的looper使用内置对象
        iterator begin() const
        {
            looper_t *looper = (looper_t*)&m_dummy_begin;
            return begin(*looper);
        }
        //尝试直接获取最小key对应的节点,不进行后续遍历
        node_t* left() const { return m_rax.left(); }
        //-------------------------------------------------
        //获取最大key对应的迭代器;需要辅助的looper遍历器
        iterator rbegin(looper_t &looper) const
        {
            looper.m_node = m_rax.right(looper);
            return iterator(looper);
        }
        //单线程简单使用的迭代器初始方法,迭代需要的looper使用内置对象
        iterator rbegin() const
        {
            looper_t *looper = (looper_t*)&m_dummy_begin;
            return rbegin(*looper);
        }
        //尝试直接获取最大key对应的节点,不进行后续遍历
        node_t* right() const { return m_rax.right(); }
        //-------------------------------------------------
        //返回遍历的结束位置
        iterator end() const { return iterator(m_dummy_end); }
        iterator rend() const { return iterator(m_dummy_end); }
        //-------------------------------------------------
        //删除指定位置的元素
        //返回值:是否删除了当前值
        bool erase(iterator &i)
        {
            rx_assert(i != end());
            //先获取当前迭代器对应节点的key值
            const val_t &val = *i;
            //迭代器后移
            ++i;

            //查找待删除key对应节点
            typename raw_rax_t::ref_path_t path;
            node_t *node = m_rax.find(val, path);
            rx_assert(node != NULL);
            //待删除节点析构
            ct::OD(&node->key);
            //最终删除节点并尝试收缩
            return m_rax.remove(node, path);
        }
        //-------------------------------------------------
        //清空全部的元素
        void clear(bool raw=false)
        {
            if (raw) m_rax.clear();
            else
            {
                looper_t looper(*this);
                for (iterator i = begin(looper); i != end();)
                    erase(i);
            }
        }
    };

    //-----------------------------------------------------
    //默认节点为uint32_t类型的集合
    //-----------------------------------------------------
    template<class key_t=uint32_t,class op_t= raxbit_op_int<key_t> >
    class raxbit_set_t :public raxbit_set_base_t<key_t, op_t>
    {
        typedef raxbit_set_base_t<key_t, op_t> super_t;
    public:
        raxbit_set_t(){}
        raxbit_set_t(mem_allotter_i &m) :super_t(m) {}
    };

}

#endif
