#ifndef _RX_DTL_HASHTBL_TINY_H_
#define _RX_DTL_HASHTBL_TINY_H_

#include "rx_cc_macro.h"
#include "rx_raw_hashtbl.h"
#include "rx_hash_data.h"
#include "rx_hash_int.h"
#include "rx_ct_util.h"

namespace rx
{
    //-----------------------------------------------------
    //基础的哈希函数功能封装
    class tiny_hash_fun
    {
    public:
        template<class KT>
        static uint32_t hash(const KT &k){return rx_hash_murmur(&k,sizeof(k));}

        static uint32_t hash(const uint32_t &k){return rx_hash_skeeto_triple(k);}
    };

    //-----------------------------------------------------
    //简单哈希集使用的节点比较器
    template<class hash_t=tiny_hash_fun>
    class tiny_hashset_cmp:public hash_t
    {
    public:
        template<class NVT,class KT>
        static bool equ(const NVT &n,const KT &k){return n==k;}
    };

    //-----------------------------------------------------
    //简单哈希表使用的节点比较器
    template<class hash_t=tiny_hash_fun>
    class tiny_hashtbl_cmp:public hash_t
    {
    public:
        template<class NVT,class KT>
        static bool equ(const NVT &n,const KT &k){return n.key==k;}
    };

    //-----------------------------------------------------
    //基于原始哈希表封装的轻量级集合容器
    //-----------------------------------------------------
    template<class val_t,uint32_t max_set_size,class vkcmp>
    class tiny_hashset_t
    {
        typedef raw_hashtbl_t<val_t,vkcmp> baseset_t;
        typedef typename baseset_t::node_t node_t;

        baseset_t   m_basetbl;                              //底层哈希功能封装
        node_t      m_nodes[max_set_size];                  //真实的哈希表节点数组空间

        //-------------------------------------------------
        //删除元素(做节点删除标记,没有处理真正的值)
        node_t* erase_raw(const val_t &val)
        {
            uint32_t hash_code = vkcmp::hash(val);
            uint32_t pos;
            node_t *node = m_basetbl.find(hash_code, val,pos);
            if (!node)
                return NULL;
            m_basetbl.remove(node);
            return node;
        }
        //-------------------------------------------------
        //在集合中插入元素,原始动作,没有记录真正的值
        node_t *insert_raw(const val_t &val)
        {
            uint32_t pos;
            uint32_t hash_code = vkcmp::hash(val);
            node_t *node = m_basetbl.push(hash_code, val ,pos);
            if (!node)
                return NULL;
            return node;
        }
    public:
        //-------------------------------------------------
        //构造的时候绑定节点空间
        tiny_hashset_t() {m_basetbl.bind(m_nodes,max_set_size);}
        virtual ~tiny_hashset_t() {clear();}
        //-------------------------------------------------
        //在集合中插入元素并赋值构造
        bool insert(const val_t &val)
        {
            node_t *node=insert_raw(val);
            if (!node)
                return false;
            ct::OC(&node->value,val);
            return true;
        }
        //-------------------------------------------------
        //查找元素是否存在
        bool find(const val_t &val) const
        {
            uint32_t hash_code = vkcmp::hash(val);
            uint32_t pos;
            node_t *node = m_basetbl.find(hash_code, val,pos);
            return node!=NULL;
        }
        //-------------------------------------------------
        //删除元素并默认析构
        bool erase(const val_t &val)
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

        //-------------------------------------------------
        //定义简单的只读迭代器
        class iterator
        {
            const tiny_hashset_t   &m_parent;
            uint32_t                m_pos;
            friend class tiny_hashset_t;

        public:
            //---------------------------------------------
            iterator(const tiny_hashset_t &s, uint32_t pos) :m_parent(s), m_pos(pos) {}
            iterator(const iterator &i):m_parent(i.m_parent),m_pos(i.m_pos) {}
            //---------------------------------------------
            bool operator==(const iterator &i)const { return &m_parent == &i.m_parent&&m_pos == i.m_pos; }
            bool operator!=(const iterator &i)const { return !(operator==(i));}
            //---------------------------------------------
            iterator& operator=(iterator &i) {m_parent=i.m_parent; m_pos=i.m_pos; return *this;}
            //---------------------------------------------
            const val_t& operator*() const
            {
                rx_assert(m_pos<max_set_size&&m_parent.m_nodes[m_pos].state);
                return m_parent.m_nodes[m_pos].value;
            }
            //---------------------------------------------
            //节点指向后移(前置运算符模式,未提供后置模式)
            iterator& operator++()
            {
                m_pos=m_parent.m_basetbl.next(m_pos);        //尝试找到下一个有效的位置
                return reinterpret_cast<iterator&>(*this);
            }
        };

        //-------------------------------------------------
        //准备遍历集合,返回遍历的初始位置
        iterator begin() const{return iterator(*this, m_basetbl.next(-1));}
        //-------------------------------------------------
        //返回遍历的结束位置
        iterator end() const { return iterator(*this, max_set_size); }
        //-------------------------------------------------
        //删除指定位置的元素
        //返回值:是否删除了当前值
        bool erase(iterator &i)
        {
            rx_assert(i.m_pos<max_set_size && &i.m_parent==this);
            if (i.m_pos>=max_set_size || &i.m_parent!=this)
                return false;

            node_t &node = m_nodes[i.m_pos];
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
    template<class key_t,class val_t,uint32_t max_set_size,class vkcmp>
    class tiny_hashtbl_t
    {
        //-------------------------------------------------
        //哈希表节点的内部结构,真正进行key与value的持有
        typedef struct node_val_t
        {
            key_t   key;
            val_t   val;
        } node_val_t;

        //-------------------------------------------------
        typedef raw_hashtbl_t<node_val_t,vkcmp> raw_tbl_t;
        typedef typename raw_tbl_t::node_t node_t;

        //-------------------------------------------------
        raw_tbl_t   m_basetbl;                              //底层哈希功能封装
        node_t      m_nodes[max_set_size];                  //真实的哈希表节点数组空间
    public:
        //-------------------------------------------------
        //构造的时候绑定节点空间
        tiny_hashtbl_t() {m_basetbl.bind(m_nodes,max_set_size);}
        virtual ~tiny_hashtbl_t() {clear();}
        //-------------------------------------------------
        //最大节点数量
        uint32_t capacity() const { return m_basetbl.capacity(); }
        //已经使用的节点数量
        uint32_t size() const { return m_basetbl.size(); }
        //插入位槽冲突总数
        uint32_t collision() const { return m_basetbl.collision(); }

        //-------------------------------------------------
        //定义简单的只读迭代器
        class iterator
        {
            const tiny_hashtbl_t   &m_parent;
            uint32_t                m_pos;
            friend class tiny_hashtbl_t;

        public:
            //---------------------------------------------
            iterator(const tiny_hashtbl_t &s, uint32_t pos) :m_parent(s), m_pos(pos) {}
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
                rx_assert(m_pos<max_set_size&&m_parent.m_nodes[m_pos].state);
                return m_parent.m_nodes[m_pos].value.val;
            }
            //---------------------------------------------
            //()运算符重载,用于获取当前节点的key值
            const key_t& operator()() const
            {
                rx_assert(m_pos<max_set_size&&m_parent.m_nodes[m_pos].state);
                return m_parent.m_nodes[m_pos].value.key;
            }
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
        iterator end() const { return iterator(*this, max_set_size); }
        //-------------------------------------------------
        //在哈希表中插入元素
        node_t* insert_raw(const key_t &key)
        {
            uint32_t pos;
            uint32_t hash_code = vkcmp::hash(key);
            node_t *node = m_basetbl.push(hash_code, key,pos);
            if (!node)
                return NULL;
            return node;
        }
        //插入元素并进行赋值构造
        bool insert(const key_t &key,const val_t &val)
        {
            node_t *node = insert_raw(key);
            if (!node)
                return false;
            ct::OC(&node->value.key,key);
            ct::OC(&node->value.val,val);
            return true;
        }
        //-------------------------------------------------
        //查找元素是否存在
        iterator find(const key_t &key) const
        {
            uint32_t hash_code = vkcmp::hash(key);
            uint32_t pos;
            node_t *node = m_basetbl.find(hash_code, key,pos);
            if (!node)
                return end();
            return iterator(*this,pos);
        }
        //-------------------------------------------------
        //标记删除指定的元素
        //返回值:待删除的节点
        node_t* erase_raw(const key_t &key)
        {
            uint32_t hash_code = vkcmp::hash(key);
            uint32_t pos;
            node_t *node = m_basetbl.find(hash_code, key,pos);
            if (!node)
                return NULL;
            m_basetbl.remove(node);
            return node;
        }
        //删除指定的key对应的节点
        bool erase(const key_t &key)
        {
            node_t *node = erase_raw( key);
            if (!node)
                return false;
            ct::OD(&node->value.key);
            ct::OD(&node->value.val);
            return true;
        }

        //-------------------------------------------------
        //标记删除指定位置的元素
        //返回值:是否删除了当前值
        node_t* erase_raw(iterator &i)
        {
            rx_assert(i.m_pos<max_set_size && &i.m_parent==this);
            if (i.m_pos>=max_set_size || &i.m_parent!=this)
                return NULL;

            node_t &node = m_nodes[i.m_pos];
            m_basetbl.remove(&node);
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
    //基于原始哈希表封装的轻量级哈希链表容器(可以快速检索并按照插入顺序快速遍历)
    //最后插入的节点是尾节点;删除的节点进行前后趋调整;
    //-----------------------------------------------------
    template<class key_t,class val_t,uint32_t max_set_size,class vkcmp>
    class tiny_hashlist_t
    {
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
        typedef raw_hashtbl_t<node_val_t,vkcmp> raw_tbl_t;
        typedef typename raw_tbl_t::node_t node_t;
        static const uint32_t nil_pos=-1;                   //定义无效节点位置
        uint32_t    m_head;                                 //记录头结点位置
        uint32_t    m_last;                                 //记录最后节点的位置
        //-------------------------------------------------
        raw_tbl_t   m_basetbl;                              //底层哈希功能封装
        node_t      m_nodes[max_set_size];                  //真实的哈希表节点数组空间
    public:
        //-------------------------------------------------
        //构造的时候绑定节点空间
        tiny_hashlist_t():m_head(nil_pos),m_last(nil_pos) {m_basetbl.bind(m_nodes,max_set_size);}
        virtual ~tiny_hashlist_t() {clear();}
        //-------------------------------------------------
        //最大节点数量
        uint32_t capacity() const { return m_basetbl.capacity(); }
        //已经使用的节点数量
        uint32_t size() const { return m_basetbl.size(); }
        //插入位槽冲突总数
        uint32_t collision() const { return m_basetbl.collision(); }

        //-------------------------------------------------
        //定义简单的只读迭代器
        class iterator
        {
            const tiny_hashlist_t   &m_parent;
            uint32_t                m_pos;
            friend class tiny_hashlist_t;
        public:
            //---------------------------------------------
            iterator(const tiny_hashlist_t &s, uint32_t pos) :m_parent(s), m_pos(pos) {}
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
                rx_assert(m_pos<max_set_size&&m_parent.m_nodes[m_pos].state);
                return m_parent.m_nodes[m_pos].value.val;
            }
            //---------------------------------------------
            //()运算符重载,用于获取当前节点的key值
            const key_t& operator()() const
            {
                rx_assert(m_pos<max_set_size&&m_parent.m_nodes[m_pos].state);
                return m_parent.m_nodes[m_pos].value.key;
            }
            //---------------------------------------------
            //节点指向后移(前置运算符模式,未提供后置模式)
            iterator& operator++()
            {
                m_pos=m_parent.m_nodes[m_pos].value.next_pos;//指向后趋的位置
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
        //在哈希表中插入元素
        node_t* insert_raw(const key_t &key)
        {
            uint32_t pos;
            uint32_t hash_code = vkcmp::hash(key);
            node_t *node = m_basetbl.push(hash_code, key,pos);
            if (!node)
                return NULL;

            if (m_head==nil_pos)
                m_head=pos;                                 //记录头结点位置
            if (m_last==nil_pos)
                m_last=m_head;                              //初始时记录尾节点位置

            m_nodes[m_last].value.next_pos=pos;             //前一节点的后趋指向当前节点

            node->value.prv_pos=m_last;                     //当前节点的前趋指向前一节点
            node->value.next_pos=nil_pos;                   //当前节点的后趋标记为空
            m_last=pos;                                     //记录尾节点为当前节点

            return node;
        }
        //插入元素并进行赋值构造
        bool insert(const key_t &key,const val_t &val)
        {
            node_t *node = insert_raw(key);
            if (!node)
                return false;
            ct::OC(&node->value.key,key);
            ct::OC(&node->value.val,val);
            return true;
        }
        //-------------------------------------------------
        //查找元素是否存在
        iterator find(const key_t &key) const
        {
            uint32_t hash_code = vkcmp::hash(key);
            uint32_t pos;
            node_t *node = m_basetbl.find(hash_code, key,pos);
            if (!node)
                return end();
            return iterator(*this,pos);
        }
        //-------------------------------------------------
        //标记删除元素
        //返回值:待删除的节点
        node_t* erase_raw(const key_t &key)
        {
            uint32_t hash_code = vkcmp::hash(key);
            uint32_t pos;
            node_t *node = m_basetbl.find(hash_code, key,pos);
            if (!node)
                return NULL;
            m_basetbl.remove(node);

            if (m_last==pos)
                m_last=node->value.prv_pos;                 //如果当前节点恰好是尾节点,则尾节点位置变为其前趋
            if (m_head==pos)
                m_head=node->value.next_pos;                //如果当前节点恰好是头节点,则头结点位置变为其后趋
            node_t &prv_node=m_nodes[node->value.prv_pos];  //获取当前节点的前趋节点
            prv_node.value.next_pos=node->value.next_pos;   //前趋节点的后趋指向当前节点的后趋
            if (node->value.next_pos!=nil_pos)
            {//如果当前节点的后趋存在
                node_t &next_node=m_nodes[node->value.next_pos];//获取当前节点的后趋节点
                next_node.value.prv_pos=node->value.prv_pos;//后趋节点的前趋指向当前节点的前趋
            }

            return node;
        }
        //删除指定key对应的节点
        bool erase(const key_t &key)
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
            rx_assert(i.m_pos<max_set_size && &i.m_parent==this);
            if (i.m_pos>=max_set_size || &i.m_parent!=this)
                return NULL;

            node_t &node = m_nodes[i.m_pos];
            m_basetbl.remove(&node);

            if (m_last==i.m_pos)
                m_last=node.value.prv_pos;                 //如果当前节点恰好是尾节点,则尾节点位置变为其前趋
            if (m_head==i.m_pos)
                m_head=node.value.next_pos;                //如果当前节点恰好是头节点,则头结点位置变为其后趋

            node_t &prv_node=m_nodes[node.value.prv_pos];  //获取当前节点的前趋节点
            prv_node.value.next_pos=node.value.next_pos;   //前趋节点的后趋指向当前节点的后趋
            if (node.value.next_pos!=nil_pos)
            {//如果当前节点的后趋存在
                node_t &next_node=m_nodes[node.value.next_pos];//获取当前节点的后趋节点
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
    //进行key类型/value类型/cmp比较器的组装后,得到最终便于使用的哈希表类型.uint32_t进行两个示范类的预定义
    //-----------------------------------------------------
    //uint32_t类型的轻量级集合
    template<uint32_t max_set_size,class hash_t=tiny_hash_fun>
    class tiny_hashset_uint32_t :public tiny_hashset_t<uint32_t, max_set_size, tiny_hashset_cmp<hash_t> > {};

    //uint32_t(key/value)类型的轻量级哈希表
    template<uint32_t max_set_size,class val_t=uint32_t,class hash_t=tiny_hash_fun>
    class tiny_hashtbl_uint32_t :public tiny_hashtbl_t<uint32_t,val_t, max_set_size, tiny_hashtbl_cmp<hash_t> > {};

    //uint32_t(key/value)类型的轻量级哈希链表
    template<uint32_t max_set_size,class val_t=uint32_t,class hash_t=tiny_hash_fun>
    class tiny_hashlist_uint32_t :public tiny_hashlist_t<uint32_t,val_t, max_set_size, tiny_hashtbl_cmp<hash_t> > {};

}

#endif
