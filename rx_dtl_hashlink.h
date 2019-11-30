#ifndef _RX_DTL_HASHLINK_TINY_H_
#define _RX_DTL_HASHLINK_TINY_H_

#include "rx_cc_macro.h"
#include "rx_dtl_hashtbl_raw.h"
#include "rx_hash_data.h"
#include "rx_hash_int.h"
#include "rx_ct_util.h"
#include "rx_str_tiny.h"

/*
    <定长哈希表,可根据插入顺序进行快速遍历>

    //hashlink比较器
    class hashlnk_cmp_t;

    //hash链基类,可快速检索访问且快速遍历(内部没有容器空间)
    template<class key_t,class val_t,class cmp_t=hashtbl_cmp_t>
    class hashlink_base_t;

    //默认key为uint32_t类型的轻量级哈希链表(默认val也为uint32_t)
    template<uint32_t max_node_count,class val_t=uint32_t,class key_t=uint32_t,class cmp_t=hashset_cmp_t>
    class tiny_hashlink_t;

    //key为tiny_string类型的轻量级哈希链表(默认val为uint32_t)
    template<uint32_t max_node_count,class val_t=uint32_t,uint16_t max_str_size=12,class CT=char,class cmp_t=hashset_cmp_t>
    class tiny_hashlink_st;
*/

namespace rx
{
    //-----------------------------------------------------
    //简单哈希链表使用的节点比较器
    class hashlnk_cmp_t:public hashtbl_fun_t
    {
    public:
        //对节点比较函数进行覆盖,需要真正使用节点中的key字段与给定的k值进行比较
        template<class NVT, class KT>
        static bool equ(const NVT &n, const KT &k) { return n.key == k; }
    };
    /*hash link 比较器最小样例
    class hashtlnk_cmp_t
    {
    public:
        //对节点比较函数进行覆盖,需要真正使用节点中的key字段与给定的k值进行比较
        template<class NVT, class KT>
        static bool equ(const NVT &n, const KT &k) { return n.key == k; }
        static uint32_t hash(const int32_t &k) { return rx_hash_skeeto_3s(k); }
    };
    */

    //-----------------------------------------------------
    //基于原始哈希表封装的轻量级哈希链表容器(可以快速检索并按照插入顺序快速遍历)
    //最后插入的节点是尾节点;删除的节点进行前后趋调整;
    //-----------------------------------------------------
    template<class key_t,class val_t,class cmp_t=hashlnk_cmp_t>
    class hashlink_base_t
    {
    public:
        //-------------------------------------------------
        //哈希表节点的内部结构,真正进行key与value的持有
        typedef struct node_val_t
        {
            key_t   key;
            val_t   val;
            uint32_t prv_pos;
            uint32_t next_pos;
        } node_val_t;

        //-------------------------------------------------
        typedef raw_hashtbl_t<node_val_t,cmp_t> raw_tbl_t;
        typedef typename raw_tbl_t::node_t node_t;

    protected:
        static const uint32_t nil_pos=-1;                   //定义无效节点位置
        uint32_t    m_head;                                 //记录头结点位置
        uint32_t    m_last;                                 //记录最后节点的位置
        //-------------------------------------------------
        raw_tbl_t   m_basetbl;                              //底层哈希功能封装
        //-------------------------------------------------
        hashlink_base_t() {}
        //构造的时候绑定节点空间
        hashlink_base_t(node_t* nodes,uint32_t max_node_count,raw_hashtbl_stat_t *st):m_head(nil_pos),m_last(nil_pos)
        {
            st->max_nodes = max_node_count;
            m_basetbl.bind(nodes,st);
        }
        virtual ~hashlink_base_t() {clear();}
    public:
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
            const hashlink_base_t   *m_parent;
            uint32_t                m_pos;
            friend class hashlink_base_t;
        public:
            //---------------------------------------------
            iterator(const hashlink_base_t &s, uint32_t pos) :m_parent(&s), m_pos(pos) {}
            iterator(const iterator &i):m_parent(i.m_parent),m_pos(i.m_pos) {}
            //---------------------------------------------
            bool operator==(const iterator &i)const { return m_parent == i.m_parent&&m_pos == i.m_pos; }
            bool operator!=(const iterator &i)const { return !(operator==(i));}
            //---------------------------------------------
            iterator& operator=(const iterator &i) {m_parent=i.m_parent; m_pos=i.m_pos; return *this;}
            //---------------------------------------------
            //*提领运算符重载,用于获取当前节点的val值
            const val_t& operator*() const
            {
                rx_assert(m_pos<m_parent->m_basetbl.capacity() &&
                          m_parent->m_basetbl.node(m_pos)->is_using());
                return m_parent->m_basetbl.node(m_pos)->value.val;
            }
            //---------------------------------------------
            //()运算符重载,用于获取当前节点的key值
            const key_t& operator()() const
            {
                rx_assert(m_pos<m_parent->m_basetbl.capacity() &&
                          m_parent->m_basetbl.node(m_pos)->is_using());
                return m_parent->m_basetbl.node(m_pos)->value.key;
            }
            //获取当前迭代器在容器中对应的位置索引
            uint32_t pos() const { return m_pos; }
            //---------------------------------------------
            //节点指向后移(前置运算符模式,未提供后置模式)
            iterator& operator++()
            {
                m_pos=m_parent->m_basetbl.node(m_pos)->value.next_pos;//指向后趋的位置
                return reinterpret_cast<iterator&>(*this);
            }
        };
        //-------------------------------------------------
        //准备遍历哈希表,返回遍历的初始位置
        iterator begin() const {return iterator(*this, m_head);}
        //获取尾节点迭代器
        iterator rbegin() const {return iterator(*this, m_last);}
        //-------------------------------------------------
        //返回遍历的结束位置
        iterator end() const { return iterator(*this, nil_pos); }
        //-------------------------------------------------
        //在哈希表中插入元素,原始动作,不构造不赋值.
        //返回值:节点指针,告知是否插入成功
        template<class KT>
        node_t *insert_raw(const KT &key, uint32_t &pos)
        {
            uint32_t hash_code = cmp_t::hash(key);
            node_t *node = m_basetbl.push(hash_code, key,pos);
            if (!node)
                return NULL;

            if (m_head==nil_pos)
                m_head=pos;                                 //记录头结点位置
            if (m_last==nil_pos)
                m_last=m_head;                              //初始时记录尾节点位置

            m_basetbl.node(m_last)->value.next_pos=pos;     //前一节点的后趋指向当前节点

            node->value.prv_pos=m_last;                     //当前节点的前趋指向前一节点
            node->value.next_pos=nil_pos;                   //当前节点的后趋标记为空
            m_last=pos;                                     //记录尾节点为当前节点

            return node;
        }
        //插入元素并进行赋值构造
        template<class KT>
        iterator insert(const KT &key,const val_t &val)
        {
            uint32_t pos;
            node_t *node = insert_raw(key, pos);
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
        //查找元素,通过返回迭代器是否与end()相同判断是否存在
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
        //标记删除元素(元素内容还没有被销毁)
        //返回值:待删除的节点
        template<class KT>
        node_t* erase_raw(const KT &key)
        {
            uint32_t hash_code = cmp_t::hash(key);
            uint32_t pos;
            node_t *node = m_basetbl.find(hash_code, key,pos);
            if (!node)
                return NULL;
            m_basetbl.remove(node,pos);

            if (m_last==pos)
                m_last=node->value.prv_pos;                 //如果当前节点恰好是尾节点,则尾节点位置变为其前趋
            if (m_head==pos)
                m_head=node->value.next_pos;                //如果当前节点恰好是头节点,则头结点位置变为其后趋
            node_t &prv_node=*m_basetbl.node(node->value.prv_pos);  //获取当前节点的前趋节点
            prv_node.value.next_pos=node->value.next_pos;   //前趋节点的后趋指向当前节点的后趋
            if (node->value.next_pos!=nil_pos)
            {//如果当前节点的后趋存在
                node_t &next_node=*m_basetbl.node(node->value.next_pos);//获取当前节点的后趋节点
                next_node.value.prv_pos=node->value.prv_pos;//后趋节点的前趋指向当前节点的前趋
            }

            return node;
        }
        //删除指定key对应的节点
        template<class KT>
        bool erase(const KT &key)
        {
            node_t *node = erase_raw( key);
            if (!node)
                return false;
            ct::OD(&node->value.key);
            ct::OD(&node->value.val);
            return true;
        }
        //-------------------------------------------------
        //标记删除指定位置的元素,迭代器位置后移(元素内容还没有被销毁)
        //返回值:被删除的节点指针
        node_t* erase_raw(iterator &i)
        {
            rx_assert(i.m_pos<m_basetbl.capacity() && i.m_parent==this);
            if (i.m_pos>= m_basetbl.capacity() || i.m_parent!=this)
                return NULL;

            node_t &node = *m_basetbl.node(i.m_pos);
            m_basetbl.remove(&node,i.m_pos);

            if (m_last==i.m_pos)
                m_last=node.value.prv_pos;                 //如果当前节点恰好是尾节点,则尾节点位置变为其前趋
            if (m_head==i.m_pos)
                m_head=node.value.next_pos;                //如果当前节点恰好是头节点,则头结点位置变为其后趋

            node_t &prv_node=*m_basetbl.node(node.value.prv_pos);  //获取当前节点的前趋节点
            prv_node.value.next_pos=node.value.next_pos;   //前趋节点的后趋指向当前节点的后趋
            if (node.value.next_pos!=nil_pos)
            {//如果当前节点的后趋存在
                node_t &next_node=*m_basetbl.node(node.value.next_pos);//获取当前节点的后趋节点
                next_node.value.prv_pos=node.value.prv_pos;//后趋节点的前趋指向当前节点的前趋
            }

            ++i;
            return &node;
        }
        //删除指定迭代器对应的节点
        bool erase(iterator &i)
        {
            node_t *node = erase_raw(i);
            if (!node)
                return false;
            ct::OD(&node->value.key);
            ct::OD(&node->value.val);
            return true;
        }
        //-------------------------------------------------
        //清空全部的元素
        void clear()
        {
            for (iterator i = begin(); i != end();)
                erase(i);
        }
    };

    //-----------------------------------------------------
    //key为uint32_t类型的轻量级哈希链表(默认val也为uint32_t)
    //-----------------------------------------------------
    template<uint32_t max_node_count,class val_t=uint32_t,class key_t=uint32_t,class cmp_t=hashtbl_cmp_t>
    class tiny_hashlink_t :public hashlink_base_t<key_t,val_t, cmp_t >
    {
        typedef hashlink_base_t<key_t, val_t, cmp_t > super_t;
        typename super_t::node_t    m_nodes[max_node_count];
        raw_hashtbl_stat_t          m_stat;
    public:
        tiny_hashlink_t():super_t(m_nodes, max_node_count,&m_stat),m_stat(max_node_count) {}
    };

    //-----------------------------------------------------
    //key为tiny_string类型的轻量级哈希链表(默认val为uint32_t)
    //-----------------------------------------------------
    template<uint32_t max_node_count,class val_t=uint32_t,uint16_t max_str_size=12,class CT=char,class cmp_t=hashtbl_cmp_t>
    class tiny_hashlink_st :public hashlink_base_t<tiny_string_t<CT,max_str_size>,val_t, cmp_t >
    {
        typedef hashlink_base_t<tiny_string_t<CT,max_str_size>, val_t, cmp_t > super_t;
        typename super_t::node_t    m_nodes[max_node_count];
        raw_hashtbl_stat_t          m_stat;
    public:
        tiny_hashlink_st():super_t(m_nodes, max_node_count,&m_stat),m_stat(max_node_count) {}
    };
}

#endif
