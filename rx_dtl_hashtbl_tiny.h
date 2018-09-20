#ifndef _RX_DTL_HASHTBL_TINY_H_
#define _RX_DTL_HASHTBL_TINY_H_

#include "rx_cc_macro.h"
#include "rx_dtl_hashtbl_raw.h"
#include "rx_hash_data.h"
#include "rx_hash_int.h"
#include "rx_ct_util.h"

/*
    //hash集比较器
    class hashset_cmp_t;
    //hash表比较器
    class hashtbl_cmp_t;

    //hash集基类,可快速检索(内部没有容器空间)
    template<class val_t,class cmp_t=hashset_cmp_t>
    class hashset_base_t;

    //hash表基类,可快速检索访问(内部没有容器空间)
    template<class key_t,class val_t,class cmp_t=hashtbl_cmp_t>
    class hashtbl_base_t;

    //hash链基类,可快速检索访问且快速遍历(内部没有容器空间)
    template<class key_t,class val_t,class cmp_t=hashtbl_cmp_t>
    class hashlist_base_t;
*/

/*在上述三种基础容器的基础上,进行便捷功能封装的语法糖容器
    //默认为uin32_t类型的hash集合
    template<uint32_t max_node_count,class key_t=uint32_t>
    class tiny_hashset_t;

    //节点为tiny_string类型的轻量级集合
    template<uint32_t max_node_count,uint16_t max_str_size=12,class CT=char>
    class tiny_hashset_st;

    //默认key为uint32_t类型的轻量级哈希表(默认val也为uint32_t)
    template<uint32_t max_node_count,class val_t=uint32_t,class key_t=uint32_t>
    class tiny_hashtbl_t;

    //key为tiny_string类型的轻量级哈希表(默认val为uint32_t)
    template<uint32_t max_node_count,class val_t=uint32_t,uint16_t max_str_size=12,class CT=char>
    class tiny_hashtbl_st;

    //默认key为uint32_t类型的轻量级哈希链表(默认val也为uint32_t)
    template<uint32_t max_node_count,class val_t=uint32_t,class key_t=uint32_t>
    class tiny_hashlist_t;

    //key为tiny_string类型的轻量级哈希链表(默认val为uint32_t)
    template<uint32_t max_node_count,class val_t=uint32_t,uint16_t max_str_size=12,class CT=char>
    class tiny_hashlist_st;
*/

namespace rx
{
    //-----------------------------------------------------
    //进行key类型/value类型/cmp比较器的组装后,得到最终便于使用的哈希表类型
    //-----------------------------------------------------
    //简单哈希集使用的节点比较器
    class hashset_cmp_t
    {
    public:
        template<class NVT, class KT>
        static bool equ(const NVT &n, const KT &k) { return n == k; }

        template<class KT>
        static uint32_t hash(const KT &k) { return rx_hash_murmur(&k, sizeof(k)); }
        static uint32_t hash(const char *k) { return rx_hash_murmur(k,st::strlen(k)); }
        static uint32_t hash(const wchar_t *k) { return rx_hash_murmur(k, st::strlen(k)); }
        static uint32_t hash(const uint32_t &k) { return rx_hash_skeeto_3s(k); }
        static uint32_t hash(const int32_t &k) { return rx_hash_skeeto_3s(k); }
    };
    //简单哈希表使用的节点比较器
    class hashtbl_cmp_t:public hashset_cmp_t
    {
    public:
        //对节点比较函数进行覆盖,需要真正使用节点中的key字段与给定的k值进行比较
        template<class NVT, class KT>
        static bool equ(const NVT &n, const KT &k) { return n.key == k; }
    };

    //-----------------------------------------------------
    //基于原始哈希表封装的轻量级集合容器
    //-----------------------------------------------------
    template<class val_t,class cmp_t=hashset_cmp_t>
    class hashset_base_t
    {
    protected:
        typedef raw_hashtbl_t<val_t,cmp_t> baseset_t;
        typedef typename baseset_t::node_t node_t;

        baseset_t   m_basetbl;                              //底层哈希功能封装

        //-------------------------------------------------
        //删除元素(做节点删除标记,没有处理真正的值)
        template<class VT>
        node_t* erase_raw(const VT &val)
        {
            uint32_t hash_code = cmp_t::hash(val);
            uint32_t pos;
            node_t *node = m_basetbl.find(hash_code, val,pos);
            if (!node)
                return NULL;
            m_basetbl.remove(node);
            return node;
        }
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
    public:
        //-------------------------------------------------
        //构造的时候绑定节点空间
        hashset_base_t(node_t* nodes,uint32_t max_node_count) {m_basetbl.bind(nodes,max_node_count);}
        virtual ~hashset_base_t() {clear();}
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
        //查找元素是否存在
        template<class VT>
        bool find(const VT &val) const
        {
            uint32_t hash_code = cmp_t::hash(val);
            uint32_t pos;
            node_t *node = m_basetbl.find(hash_code, val,pos);
            return node!=NULL;
        }
        //-------------------------------------------------
        //删除元素并默认析构
        template<class VT>
        bool erase(const VT &val)
        {
            node_t *node = erase_raw(val);
            if (!node)
                return false;
            ct::OD(&node->value);
            return true;
        }
        //-------------------------------------------------
        //最大节点数量
        uint32_t capacity() const { return m_basetbl.capacity(); }
        //已经使用的节点数量
        uint32_t size() const { return m_basetbl.size(); }
        //插入位槽冲突总数
        uint32_t collision() const { return m_basetbl.collision(); }
        //根据给定的索引位置直接访问对应的值
        const val_t& at(uint32_t pos)const { return m_basetbl.node(pos)->value; }
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
            iterator(const iterator &i):m_parent(i.m_parent),m_pos(i.m_pos) {}
            //---------------------------------------------
            bool operator==(const iterator &i)const { return &m_parent == &i.m_parent&&m_pos == i.m_pos; }
            bool operator!=(const iterator &i)const { return !(operator==(i));}
            //---------------------------------------------
            iterator& operator=(iterator &i) {m_parent=i.m_parent; m_pos=i.m_pos; return *this;}
            //---------------------------------------------
            const val_t& operator*() const
            {
                rx_assert(m_pos<m_parent.m_basetbl.capacity() &&m_parent.m_basetbl.node(m_pos)->state);
                return m_parent.m_basetbl.node(m_pos)->value;
            }
            //---------------------------------------------
            //节点指向后移(前置运算符模式,未提供后置模式)
            iterator& operator++()
            {
                m_pos=m_parent.m_basetbl.next(m_pos);        //尝试找到下一个有效的位置
                return reinterpret_cast<iterator&>(*this);
            }
            //获取当前迭代器在容器中对应的位置索引
            uint32_t pos() const { return m_pos; }
        };

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
            m_basetbl.remove(&node);
            ++i;
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
    //基于原始哈希表封装的轻量级哈希表容器
    //-----------------------------------------------------
    template<class key_t,class val_t,class cmp_t=hashtbl_cmp_t>
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
        //构造的时候绑定节点空间
        hashtbl_base_t(node_t *nodes,uint32_t max_node_count) {m_basetbl.bind(nodes,max_node_count);}
        virtual ~hashtbl_base_t() {clear();}
        //-------------------------------------------------
        //最大节点数量
        uint32_t capacity() const { return m_basetbl.capacity(); }
        //已经使用的节点数量
        uint32_t size() const { return m_basetbl.size(); }
        //插入位槽冲突总数
        uint32_t collision() const { return m_basetbl.collision(); }
        //根据给定的索引位置直接访问对应的节点内容
        const node_val_t& at(uint32_t pos)const { return m_basetbl.node(pos)->value; }
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
            iterator& operator=(iterator &i) {m_parent=i.m_parent; m_pos=i.m_pos; return *this;}
            //---------------------------------------------
            //*提领运算符重载,用于获取当前节点的val值
            const val_t& operator*() const
            {
                rx_assert(m_pos<m_parent.m_basetbl.capacity() &&m_parent.m_basetbl.node(m_pos)->state);
                return m_parent.m_basetbl.node(m_pos)->value.val;
            }
            //---------------------------------------------
            //()运算符重载,用于获取当前节点的key值
            const key_t& operator()() const
            {
                rx_assert(m_pos<m_parent.m_basetbl.capacity()&&m_parent.m_basetbl.node(m_pos)->state);
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
        //查找元素是否存在
        template<class KT>
        iterator find(const KT &key) const
        {
            uint32_t hash_code = cmp_t::hash(key);
            uint32_t pos;
            node_t *node = m_basetbl.find(hash_code, key,pos);
            if (!node)
                return end();
            return iterator(*this,pos);
        }
        //-------------------------------------------------
        //标记删除指定的元素
        //返回值:待删除的节点
        template<class KT>
        node_t* erase_raw(const KT &key)
        {
            uint32_t hash_code = cmp_t::hash(key);
            uint32_t pos;
            node_t *node = m_basetbl.find(hash_code, key,pos);
            if (!node)
                return NULL;
            m_basetbl.remove(node);
            return node;
        }
        //删除指定的key对应的节点
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
        //标记删除指定的元素
        //返回值:是否删除了当前值(删除成功时,迭代器i后移)
        node_t* erase_raw(iterator &i)
        {
            rx_assert(i.m_pos<m_basetbl.capacity() && &i.m_parent==this);
            if (i.m_pos>= m_basetbl.capacity() || &i.m_parent!=this)
                return NULL;

            node_t &node = *m_basetbl.node(i.m_pos);
            m_basetbl.remove(&node);
            ++i;
            return &node;
        }
        //删除指定迭代器对应的节点
        //返回值:是否删除了当前值(删除成功时,迭代器i后移)
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
    //基于原始哈希表封装的轻量级哈希链表容器(可以快速检索并按照插入顺序快速遍历)
    //最后插入的节点是尾节点;删除的节点进行前后趋调整;
    //-----------------------------------------------------
    template<class key_t,class val_t,class cmp_t=hashtbl_cmp_t>
    class hashlist_base_t
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
    public:
        //-------------------------------------------------
        //构造的时候绑定节点空间
        hashlist_base_t(node_t *nodes, uint32_t max_node_count):m_head(nil_pos),m_last(nil_pos) {m_basetbl.bind(nodes,max_node_count);}
        virtual ~hashlist_base_t() {clear();}
        //-------------------------------------------------
        //最大节点数量
        uint32_t capacity() const { return m_basetbl.capacity(); }
        //已经使用的节点数量
        uint32_t size() const { return m_basetbl.size(); }
        //插入位槽冲突总数
        uint32_t collision() const { return m_basetbl.collision(); }
        //根据给定的索引位置直接访问对应的节点内容
        const node_val_t& at(uint32_t pos)const { return m_basetbl.node(pos)->value; }
        //-------------------------------------------------
        //定义简单的只读迭代器
        class iterator
        {
            const hashlist_base_t   &m_parent;
            uint32_t                m_pos;
            friend class hashlist_base_t;
        public:
            //---------------------------------------------
            iterator(const hashlist_base_t &s, uint32_t pos) :m_parent(s), m_pos(pos) {}
            iterator(const iterator &i):m_parent(i.m_parent),m_pos(i.m_pos) {}
            //---------------------------------------------
            bool operator==(const iterator &i)const { return &m_parent == &i.m_parent&&m_pos == i.m_pos; }
            bool operator!=(const iterator &i)const { return !(operator==(i));}
            //---------------------------------------------
            iterator& operator=(iterator &i) {m_parent=i.m_parent; m_pos=i.m_pos; return *this;}
            //---------------------------------------------
            //*提领运算符重载,用于获取当前节点的val值
            const val_t& operator*() const
            {
                rx_assert(m_pos<m_parent.m_basetbl.capacity() &&m_parent.m_basetbl.node(m_pos)->state);
                return m_parent.m_basetbl.node(m_pos)->value.val;
            }
            //---------------------------------------------
            //()运算符重载,用于获取当前节点的key值
            const key_t& operator()() const
            {
                rx_assert(m_pos<m_parent.m_basetbl.capacity() &&m_parent.m_basetbl.node(m_pos)->state);
                return m_parent.m_basetbl.node(m_pos)->value.key;
            }
            //获取当前迭代器在容器中对应的位置索引
            uint32_t pos() const { return m_pos; }
            //---------------------------------------------
            //节点指向后移(前置运算符模式,未提供后置模式)
            iterator& operator++()
            {
                m_pos=m_parent.m_basetbl.node(m_pos)->value.next_pos;//指向后趋的位置
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
        //查找元素是否存在
        template<class KT>
        iterator find(const KT &key) const
        {
            uint32_t hash_code = cmp_t::hash(key);
            uint32_t pos;
            node_t *node = m_basetbl.find(hash_code, key,pos);
            if (!node)
                return end();
            return iterator(*this,pos);
        }
        //-------------------------------------------------
        //标记删除元素
        //返回值:待删除的节点
        template<class KT>
        node_t* erase_raw(const KT &key)
        {
            uint32_t hash_code = cmp_t::hash(key);
            uint32_t pos;
            node_t *node = m_basetbl.find(hash_code, key,pos);
            if (!node)
                return NULL;
            m_basetbl.remove(node);

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
        //标记删除指定位置的元素,迭代器位置后移
        //返回值:被删除的节点指针
        node_t* erase_raw(iterator &i)
        {
            rx_assert(i.m_pos<m_basetbl.capacity() && &i.m_parent==this);
            if (i.m_pos>= m_basetbl.capacity() || &i.m_parent!=this)
                return NULL;

            node_t &node = *m_basetbl.node(i.m_pos);
            m_basetbl.remove(&node);

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
    //默认节点为uint32_t类型的轻量级集合
    //-----------------------------------------------------
    template<uint32_t max_node_count,class key_t=uint32_t>
    class tiny_hashset_t :public hashset_base_t<key_t, hashset_cmp_t >
    {
        typedef hashset_base_t<key_t, hashset_cmp_t > super_t;
        typename super_t::node_t    m_nodes[max_node_count];
    public:
        tiny_hashset_t():super_t(m_nodes, max_node_count){}
    };

    //-----------------------------------------------------
    //节点为tiny_string类型的轻量级集合
    //-----------------------------------------------------
    template<uint32_t max_node_count,uint16_t max_str_size=12,class CT=char>
    class tiny_hashset_st :public hashset_base_t<tiny_string_head_t<CT,max_str_size>, hashset_cmp_t >
    {
        typedef hashset_base_t<tiny_string_head_t<CT,max_str_size>, hashset_cmp_t > super_t;
        typename super_t::node_t    m_nodes[max_node_count];
    public:
        tiny_hashset_st():super_t(m_nodes, max_node_count){}
    };

    //-----------------------------------------------------
    //key为uint32_t类型的轻量级哈希表(默认val也为uint32_t)
    //-----------------------------------------------------
    template<uint32_t max_node_count,class val_t=uint32_t,class key_t=uint32_t>
    class tiny_hashtbl_t :public hashtbl_base_t<key_t,val_t, hashtbl_cmp_t >
    {
        typedef hashtbl_base_t<key_t, val_t, hashtbl_cmp_t > super_t;
        typename super_t::node_t    m_nodes[max_node_count];
    public:
        tiny_hashtbl_t():super_t(m_nodes, max_node_count) {}
    };

    //-----------------------------------------------------
    //key为tiny_string类型的轻量级哈希表(默认val为uint32_t)
    //-----------------------------------------------------
    template<uint32_t max_node_count,class val_t=uint32_t,uint16_t max_str_size=12,class CT=char>
    class tiny_hashtbl_st :public hashtbl_base_t<tiny_string_head_t<CT,max_str_size>,val_t, hashtbl_cmp_t >
    {
        typedef hashtbl_base_t<tiny_string_head_t<CT,max_str_size>, val_t, hashtbl_cmp_t > super_t;
        typename super_t::node_t    m_nodes[max_node_count];
    public:
        tiny_hashtbl_st():super_t(m_nodes, max_node_count) {}
    };

    //-----------------------------------------------------
    //key为uint32_t类型的轻量级哈希链表(默认val也为uint32_t)
    //-----------------------------------------------------
    template<uint32_t max_node_count,class val_t=uint32_t,class key_t=uint32_t>
    class tiny_hashlist_t :public hashlist_base_t<key_t,val_t, hashtbl_cmp_t >
    {
        typedef hashlist_base_t<key_t, val_t, hashtbl_cmp_t > super_t;
        typename super_t::node_t    m_nodes[max_node_count];
    public:
        tiny_hashlist_t():super_t(m_nodes, max_node_count) {}
    };

    //-----------------------------------------------------
    //key为tiny_string类型的轻量级哈希链表(默认val为uint32_t)
    //-----------------------------------------------------
    template<uint32_t max_node_count,class val_t=uint32_t,uint16_t max_str_size=12,class CT=char>
    class tiny_hashlist_st :public hashlist_base_t<tiny_string_head_t<CT,max_str_size>,val_t, hashtbl_cmp_t >
    {
        typedef hashlist_base_t<tiny_string_head_t<CT,max_str_size>, val_t, hashtbl_cmp_t > super_t;
        typename super_t::node_t    m_nodes[max_node_count];
    public:
        tiny_hashlist_st():super_t(m_nodes, max_node_count) {}
    };
}

#endif
