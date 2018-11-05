#ifndef _RX_DTL_HASHSET_TINY_H_
#define _RX_DTL_HASHSET_TINY_H_

#include "rx_cc_macro.h"
#include "rx_dtl_hashtbl_raw.h"
#include "rx_hash_data.h"
#include "rx_hash_int.h"
#include "rx_ct_util.h"
#include "rx_str_tiny.h"

/*
    <定长哈希集合>

    //hash集比较器
    class hashset_cmp_t;

    //hash集基类,可快速检索(内部没有容器空间)
    template<class val_t,class cmp_t=hashset_cmp_t>
    class hashset_base_t;

    //默认为uin32_t类型的hash集合
    template<uint32_t max_node_count,class key_t=uint32_t,bool correct=false,class cmp_t=hashset_cmp_t>
    class tiny_hashset_t;

    //节点为tiny_string类型的轻量级集合
    template<uint32_t max_node_count,uint16_t max_str_size=12,bool correct=false,class CT=char,class cmp_t=hashset_cmp_t>
    class tiny_hashset_st;
*/

namespace rx
{
    //-----------------------------------------------------
    //简单哈希集使用的节点比较器
    class hashset_cmp_t:public hashtbl_fun_t
    {
    public:
        template<class NVT, class KT>
        static bool equ(const NVT &n, const KT &k) { return n == k; }
    };

    //-----------------------------------------------------
    //基于原始哈希表封装的轻量级集合容器
    //-----------------------------------------------------
    template<class val_t,class cmp_t=hashset_cmp_t,bool correct=false>
    class hashset_base_t
    {
    protected:
        typedef raw_hashtbl_t<val_t,cmp_t> raw_tbl_t;
        typedef typename raw_tbl_t::node_t node_t;

        raw_tbl_t   m_basetbl;                              //底层哈希功能封装

        //-------------------------------------------------
        //在集合中插入元素,原始动作,没有记录真正的值
        template<class VT>
        node_t *insert_raw(const VT &val)
        {
            uint32_t pos;
            uint32_t hash_code = cmp_t::hash(val);
            node_t *node = m_basetbl.push(hash_code, val ,pos);
            if (!node)
                return NULL;
            return node;
        }
        //-------------------------------------------------
        hashset_base_t() {}
        //构造的时候绑定节点空间
        hashset_base_t(node_t* nodes, uint32_t max_node_count, raw_hashtbl_stat_t *st)
        {
            st->max_nodes = max_node_count;
            m_basetbl.bind(nodes, st);
        }
        virtual ~hashset_base_t() { clear(); }
    public:
        //-------------------------------------------------
        //最大节点数量
        uint32_t capacity() const { return m_basetbl.capacity(); }
        //已经使用的节点数量
        uint32_t size() const { return m_basetbl.size(); }
        //内部状态
        const raw_hashtbl_stat_t& stat() const { return m_basetbl.stat(); }
        //根据给定的索引位置直接访问对应的值
        const val_t& at_value(uint32_t pos)const { return m_basetbl.node(pos)->value; }
        //根据给定的索引位置获取节点
        const node_t* at(uint32_t pos)const { return m_basetbl.node(pos); }
        //-------------------------------------------------
        //定义简单的只读迭代器
        class iterator
        {
            const hashset_base_t   &m_parent;
            uint32_t                m_pos;
            friend class hashset_base_t;

        public:
            //---------------------------------------------
            iterator(const hashset_base_t &s, uint32_t pos) :m_parent(s), m_pos(pos) {}
            iterator(const iterator &i) :m_parent(i.m_parent), m_pos(i.m_pos) {}
            //---------------------------------------------
            bool operator==(const iterator &i)const { return &m_parent == &i.m_parent&&m_pos == i.m_pos; }
            bool operator!=(const iterator &i)const { return !(operator==(i)); }
            //---------------------------------------------
            iterator& operator=(const iterator &i) { m_parent = i.m_parent; m_pos = i.m_pos; return *this; }
            //---------------------------------------------
            const val_t& operator*() const
            {
                rx_assert(m_pos<m_parent.m_basetbl.capacity() &&
                    m_parent.m_basetbl.node(m_pos)->is_using());
                return m_parent.m_basetbl.node(m_pos)->value;
            }
            //---------------------------------------------
            //节点指向后移(前置运算符模式,未提供后置模式)
            iterator& operator++()
            {
                m_pos = m_parent.m_basetbl.next(m_pos);        //尝试找到下一个有效的位置
                return reinterpret_cast<iterator&>(*this);
            }
            //获取当前迭代器在容器中对应的位置索引
            uint32_t pos() const { return m_pos; }
        };
        //-------------------------------------------------
        //在集合中插入元素并赋值构造
        template<class VT>
        bool insert(const VT &val)
        {
            node_t *node=insert_raw(val);
            if (!node)
                return false;
            ct::OC(&node->value,val);
            return true;
        }
        //-------------------------------------------------
        //查找元素,通过返回迭代器是否与end()相同判断是否存在
        template<class VT>
        bool find(const VT &val) const
        {
            uint32_t pos;
            return find(val,pos);
        }
        template<class VT>
        bool operator[](const VT &val) const { return find(val); }
        //-------------------------------------------------
        //查找元素,同时获知元素的索引位置
        template<class VT>
        bool find(const VT &val,uint32_t &pos) const
        {
            uint32_t hash_code = cmp_t::hash(val);
            node_t *node = m_basetbl.find(hash_code, val, pos);
            return node != NULL;
        }
        //-------------------------------------------------
        //删除元素并默认析构,可以指定是否进行删除后的空洞校正
        template<class VT>
        bool erase(const VT &val)
        {
            uint32_t pos;
            uint32_t hash_code = cmp_t::hash(val);
            node_t *node = m_basetbl.find(hash_code, val, pos);

            if (!node || !m_basetbl.remove(node, pos))
                return false;

            ct::OD(&node->value);

            if (correct)
                m_basetbl.correct_following(pos);
            return true;
        }
        //-------------------------------------------------
        //准备遍历集合,返回遍历的初始位置
        iterator begin() const{return iterator(*this, m_basetbl.next(-1));}
        //-------------------------------------------------
        //返回遍历的结束位置
        iterator end() const { return iterator(*this, m_basetbl.capacity()); }
        //-------------------------------------------------
        //删除指定位置的元素
        //返回值:是否删除了当前值
        bool erase(iterator &i)
        {
            rx_assert(i.m_pos<m_basetbl.capacity() && &i.m_parent==this);
            if (i.m_pos>= m_basetbl.capacity() || &i.m_parent!=this)
                return false;

            node_t &node = *m_basetbl.node(i.m_pos);
            ct::OD(&node.value);

            if (m_basetbl.remove(&node, i.m_pos) && correct)
                m_basetbl.correct_following(i.m_pos);

            ++i;
            return true;
        }
        //-------------------------------------------------
        //清空全部的元素
        void clear()
        {
            for (iterator i = begin(); i != end();)
            {
                node_t &node = *m_basetbl.node(i.m_pos);
                ct::OD(&node.value);
                m_basetbl.remove(&node, i.m_pos);
                ++i;
            }
        }
    };

    //-----------------------------------------------------
    //默认节点为uint32_t类型的轻量级集合
    //-----------------------------------------------------
    template<uint32_t max_node_count,class key_t=uint32_t,bool correct=false,class cmp_t=hashset_cmp_t>
    class tiny_hashset_t :public hashset_base_t<key_t, cmp_t ,correct>
    {
        typedef hashset_base_t<key_t, cmp_t ,correct> super_t;
        typename super_t::node_t    m_nodes[max_node_count];
        raw_hashtbl_stat_t          m_stat;
    public:
        tiny_hashset_t():super_t(m_nodes, max_node_count,&m_stat){}
    };

    //-----------------------------------------------------
    //节点为tiny_string类型的轻量级集合
    //-----------------------------------------------------
    template<uint32_t max_node_count,uint16_t max_str_size=12,bool correct=false,class CT=char,class cmp_t=hashset_cmp_t>
    class tiny_hashset_st :public hashset_base_t<tiny_string_head_t<CT,max_str_size>, cmp_t ,correct>
    {
        typedef hashset_base_t<tiny_string_head_t<CT,max_str_size>, cmp_t ,correct> super_t;
        typename super_t::node_t    m_nodes[max_node_count];
        raw_hashtbl_stat_t          m_stat;
    public:
        tiny_hashset_st():super_t(m_nodes, max_node_count,&m_stat){}
    };
}

#endif
