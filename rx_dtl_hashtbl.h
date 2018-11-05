#ifndef _RX_DTL_HASHTBL_TINY_H_
#define _RX_DTL_HASHTBL_TINY_H_

#include "rx_cc_macro.h"
#include "rx_dtl_hashtbl_raw.h"
#include "rx_hash_data.h"
#include "rx_hash_int.h"
#include "rx_ct_util.h"
#include "rx_str_tiny.h"

/*
    <定长哈希表>

    //hash表比较器
    class hashtbl_cmp_t;

    //hash表基类,可快速检索访问(内部没有容器空间)
    template<class key_t,class val_t,class cmp_t=hashtbl_cmp_t>
    class hashtbl_base_t;

    //默认key为uint32_t类型的轻量级哈希表(默认val也为uint32_t)
    template<uint32_t max_node_count,class val_t=uint32_t,class key_t=uint32_t,bool correct=false,class cmp_t=hashset_cmp_t>
    class tiny_hashtbl_t;

    //key为tiny_string类型的轻量级哈希表(默认val为uint32_t)
    template<uint32_t max_node_count,class val_t=uint32_t,uint16_t max_str_size=12,bool correct=false,class CT=char,class cmp_t=hashset_cmp_t>
    class tiny_hashtbl_st;
*/

namespace rx
{
    //-----------------------------------------------------
    //简单哈希表使用的节点比较器
    class hashtbl_cmp_t:public hashtbl_fun_t
    {
    public:
        //对节点比较函数进行覆盖,需要真正使用节点中的key字段与给定的k值进行比较
        template<class NVT, class KT>
        static bool equ(const NVT &n, const KT &k) { return n.key == k; }
    };
    /*hash table 比较器最小样例
    class hashtbl_cmp_t
    {
    public:
        //对节点比较函数进行覆盖,需要真正使用节点中的key字段与给定的k值进行比较
        template<class NVT, class KT>
        static bool equ(const NVT &n, const KT &k) { return n.key == k; }
        static uint32_t hash(const int32_t &k) { return rx_hash_skeeto_3s(k); }
    };
    */

    //-----------------------------------------------------
    //基于原始哈希表封装的轻量级哈希表容器
    //-----------------------------------------------------
    template<class key_t,class val_t,class cmp_t=hashtbl_cmp_t,bool correct=false>
    class hashtbl_base_t
    {
    public:
        //-------------------------------------------------
        //哈希表节点的内部结构,真正进行key与value的持有
        typedef struct node_val_t
        {
            key_t   key;
            val_t   val;
        } node_val_t;

        //-------------------------------------------------
        typedef raw_hashtbl_t<node_val_t,cmp_t> raw_tbl_t;
        typedef typename raw_tbl_t::node_t node_t;
    protected:
        //-------------------------------------------------
        raw_tbl_t   m_basetbl;                              //底层哈希功能封装
    public:
        //-------------------------------------------------
        hashtbl_base_t() {}
        //构造的时候绑定节点空间
        hashtbl_base_t(node_t* nodes,uint32_t max_node_count,raw_hashtbl_stat_t *st)
        {
            st->max_nodes = max_node_count;
            m_basetbl.bind(nodes, st);
        }

        virtual ~hashtbl_base_t() {clear();}
        //-------------------------------------------------
        //最大节点数量
        uint32_t capacity() const { return m_basetbl.capacity(); }
        //已经使用的节点数量
        uint32_t size() const { return m_basetbl.size(); }
        //内部状态
        const raw_hashtbl_stat_t& stat() const { return m_basetbl.stat(); }
        //根据给定的索引位置直接访问对应的值
        const node_val_t& at_value(uint32_t pos)const { return m_basetbl.node(pos)->value; }
        //根据给定的索引位置获取节点
        const node_t* at(uint32_t pos)const { return m_basetbl.node(pos); }
        //-------------------------------------------------
        //定义简单的只读迭代器
        class iterator
        {
            const hashtbl_base_t   &m_parent;
            uint32_t                m_pos;
            friend class hashtbl_base_t;

        public:
            //---------------------------------------------
            iterator(const hashtbl_base_t &s, uint32_t pos) :m_parent(s), m_pos(pos) {}
            iterator(const iterator &i):m_parent(i.m_parent),m_pos(i.m_pos) {}
            //---------------------------------------------
            bool operator==(const iterator &i)const { return &m_parent == &i.m_parent&&m_pos == i.m_pos; }
            bool operator!=(const iterator &i)const { return !(operator==(i));}
            //---------------------------------------------
            iterator& operator=(const iterator &i) {m_parent=i.m_parent; m_pos=i.m_pos; return *this;}
            //---------------------------------------------
            //*提领运算符重载,用于获取当前节点的val值
            const val_t& operator*() const
            {
                rx_assert(m_pos<m_parent.m_basetbl.capacity() &&
                          m_parent.m_basetbl.node(m_pos)->is_using());
                return m_parent.m_basetbl.node(m_pos)->value.val;
            }
            //---------------------------------------------
            //()运算符重载,用于获取当前节点的key值
            const key_t& operator()() const
            {
                rx_assert(m_pos<m_parent.m_basetbl.capacity()&&
                          m_parent.m_basetbl.node(m_pos)->is_using());
                return m_parent.m_basetbl.node(m_pos)->value.key;
            }
            //获取当前迭代器在容器中对应的位置索引
            uint32_t pos() const { return m_pos; }
            //---------------------------------------------
            //节点指向后移(前置运算符模式,未提供后置模式)
            iterator& operator++()
            {
                m_pos=m_parent.m_basetbl.next(m_pos);                  //尝试找到下一个有效的位置
                return reinterpret_cast<iterator&>(*this);
            }
        };
        //-------------------------------------------------
        //准备遍历哈希表,返回遍历的初始位置
        iterator begin() const {return iterator(*this, m_basetbl.next(-1));}
        //-------------------------------------------------
        //返回遍历的结束位置
        iterator end() const { return iterator(*this, m_basetbl.capacity()); }
        //-------------------------------------------------
        //在哈希表中插入元素,原始动作,不构造,不赋值.
        //返回值:插入的节点指针,告知是否成功
        template<class KT>
        node_t *insert_raw(const KT &key, uint32_t &pos)
        {
            uint32_t hash_code = cmp_t::hash(key);
            return m_basetbl.push(hash_code, key,pos);
        }
        //插入元素并进行赋值构造
        template<class KT>
        iterator insert(const KT &key,const val_t &val)
        {
            uint32_t pos;
            node_t *node=insert_raw(key,pos);
            if (node==NULL)
                return end();
            ct::OC(&node->value.key,key);
            ct::OC(&node->value.val,val);
            return iterator(*this, pos);
        }
        //-------------------------------------------------
        //查找元素,通过返回迭代器是否与end()相同判断是否存在
        template<class KT>
        iterator find(const KT &key) const
        {
            uint32_t pos;
            return find(key,pos);
        }
        template<class KT>
        iterator operator[](const KT &key) const { return find(key); }
        //-------------------------------------------------
        //查找元素,同时获知元素的节点索引位置
        template<class KT>
        iterator find(const KT &key,uint32_t &pos) const
        {
            uint32_t hash_code = cmp_t::hash(key);
            node_t *node = m_basetbl.find(hash_code, key, pos);
            if (!node)
                return end();
            return iterator(*this, pos);
        }
        //-------------------------------------------------
        //标记删除指定的元素(元素内容还没有被销毁)
        //返回值:待删除的节点
        template<class KT>
        node_t* erase_raw(const KT &key, uint32_t &pos)
        {
            uint32_t hash_code = cmp_t::hash(key);
            node_t *node = m_basetbl.find(hash_code, key,pos);
            if (!node|| !m_basetbl.remove(node, pos))
                return NULL;
            return node;
        }
        //删除指定的key对应的节点
        template<class KT>
        bool erase(const KT &key)
        {
            uint32_t pos;
            node_t *node = erase_raw(key, pos);
            if (!node)
                return false;

            ct::OD(&node->value.key);
            ct::OD(&node->value.val);

            if (correct)
                m_basetbl.correct_following(pos);
            return true;
        }

        //-------------------------------------------------
        //标记删除指定的元素(元素内容还没有被销毁)
        //返回值:是否删除了当前值(删除成功时,迭代器i后移)
        node_t* erase_raw(iterator &i)
        {
            rx_assert(i.m_pos<m_basetbl.capacity() && &i.m_parent==this);
            if (i.m_pos>= m_basetbl.capacity() || &i.m_parent!=this)
                return NULL;

            node_t &node = *m_basetbl.node(i.m_pos);
            m_basetbl.remove(&node,i.m_pos);
            ++i;
            return &node;
        }
        //删除指定迭代器对应的节点
        //返回值:是否删除了当前值(删除成功时,迭代器i后移)
        bool erase(iterator &i)
        {
            uint32_t pos = i.m_pos;
            node_t *node = erase_raw(i);
            if (!node)
                return false;

            ct::OD(&node->value.key);
            ct::OD(&node->value.val);

            if (correct)
                m_basetbl.correct_following(pos);

            return true;
        }
        //-------------------------------------------------
        //清空全部的元素
        void clear()
        {
            if (!m_basetbl.is_valid())
                return;
            for (iterator i = begin(); i != end();)
                erase(i);
        }
    };

    //-----------------------------------------------------
    //key为uint32_t类型的轻量级哈希表(默认val也为uint32_t)
    //-----------------------------------------------------
    template<uint32_t max_node_count,class val_t=uint32_t,class key_t=uint32_t,bool correct=false,class cmp_t=hashtbl_cmp_t>
    class tiny_hashtbl_t :public hashtbl_base_t<key_t,val_t, cmp_t ,correct>
    {
        typedef hashtbl_base_t<key_t, val_t, cmp_t ,correct> super_t;
        typename super_t::node_t    m_nodes[max_node_count];
        raw_hashtbl_stat_t          m_stat;
    public:
        tiny_hashtbl_t():super_t(m_nodes, max_node_count,&m_stat) {}
    };

    //-----------------------------------------------------
    //key为tiny_string类型的轻量级哈希表(默认val为uint32_t)
    //-----------------------------------------------------
    template<uint32_t max_node_count,class val_t=uint32_t,uint16_t max_str_size=12,bool correct=false,class CT=char,class cmp_t=hashtbl_cmp_t>
    class tiny_hashtbl_st :public hashtbl_base_t<tiny_string_head_t<CT,max_str_size>,val_t, cmp_t ,correct>
    {
        typedef hashtbl_base_t<tiny_string_head_t<CT,max_str_size>, val_t, cmp_t ,correct> super_t;
        typename super_t::node_t    m_nodes[max_node_count];
        raw_hashtbl_stat_t          m_stat;
    public:
        tiny_hashtbl_st():super_t(m_nodes, max_node_count,&m_stat) {}
    };
}

#endif
