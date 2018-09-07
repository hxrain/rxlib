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
    //简单哈希集使用的节点比较器
    template<class hashfun_t>
    class tiny_hashset_cmp
    {
    public:
        template<class NVT,class KT>
        static bool equ(const NVT &n,const KT &k){return n==k;}

        template<class KT>
        static uint32_t hash(const KT &k){return hashfun_t::hash(k);}
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

    public:
        //-------------------------------------------------
        //构造的时候绑定节点空间
        tiny_hashset_t() {m_basetbl.bind(m_nodes,max_set_size);}
        virtual ~tiny_hashset_t() {clear();}
        //-------------------------------------------------
        //在集合中插入元素,原始动作,没有记录真正的值
        node_t *insert_raw(const val_t &val)
        {
            uint32_t hash_code = vkcmp::hash(val);
            node_t *node = m_basetbl.push(hash_code, val);
            if (!node)
                return NULL;
            return node;
        }
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
            node_t *node = m_basetbl.find(hash_code, val);
            return node!=NULL;
        }
        //-------------------------------------------------
        //删除元素(做节点删除标记,没有处理真正的值)
        node_t* erase_raw(const val_t &val)
        {
            uint32_t hash_code = vkcmp::hash(val);
            node_t *node = m_basetbl.find(hash_code, val);
            if (!node)
                return NULL;
            m_basetbl.remove(node);
            return node;
        }
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
    //简单哈希表使用的节点比较器
    template<class hashfun_t>
    class tiny_hashtbl_cmp
    {
    public:
        template<class NVT,class KT>
        static bool equ(const NVT &n,const KT &k){return n.key==k;}

        template<class KT>
        static uint32_t hash(const KT &k){return hashfun_t::hash(k);}
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
            //节点指向后移
            iterator& operator++()
            {
                m_pos=m_parent.m_basetbl.next(m_pos);                  //尝试找到下一个有效的位置
                return reinterpret_cast<iterator&>(*this);
            }
        };
        //-------------------------------------------------
        //准备遍历集合,返回遍历的初始位置
        iterator begin() const {return iterator(*this, m_basetbl.next(-1));}
        //-------------------------------------------------
        //返回遍历的结束位置
        iterator end() const { return iterator(*this, max_set_size); }
        //-------------------------------------------------
        //在集合中插入元素
        node_t* insert_raw(const key_t &key)
        {
            uint32_t hash_code = vkcmp::hash(key);
            node_t *node = m_basetbl.push(hash_code, key);
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
        bool find(const key_t &key) const
        {
            uint32_t hash_code = vkcmp::hash(key);
            node_t *node = m_basetbl.find(hash_code, key);
            return node!=NULL;
        }
        //-------------------------------------------------
        //删除元素
        node_t* erase_raw(const key_t &key)
        {
            uint32_t hash_code = vkcmp::hash(key);
            node_t *node = m_basetbl.find(hash_code, key);
            if (!node)
                return NULL;
            m_basetbl.remove(node);
            return node;
        }
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
        //删除指定位置的元素
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
    //简单哈希表的哈希函数适配器(默认的通用型)
    template<class val_t>
    class hashfunc_adaptor_t
    {
    public:
        //对给定的值进行真正的哈希值计算
        static uint32_t hash(const val_t& val) { return rx_hash_murmur(val,sizeof(val)); }
    };
    //-----------------------------------------------------
    //简单哈希表的哈希函数适配器(针对整型数的偏特化版本)
    template<>
    class hashfunc_adaptor_t<uint32_t>
    {
    public:
        //进行整数哈希值的计算
        static uint32_t hash(const uint32_t& val) { return rx_hash_skeeto_triple(val); }
    };

    //-----------------------------------------------------
    //进行key类型/value类型/hash函数适配器的组装后,得到最终便于使用的哈希表类型.
    //下面用uint32_t进行两个示范类的预定义
    //-----------------------------------------------------
    //uint32_t类型的轻量级集合
    template<uint32_t max_set_size>
    class tiny_hashset_uint32_t :public tiny_hashset_t<uint32_t, max_set_size, tiny_hashset_cmp<hashfunc_adaptor_t<uint32_t> > > {};

    //uint32_t(key/value)类型的轻量级哈希表
    template<uint32_t max_set_size,class val_t=uint32_t>
    class tiny_hashtbl_uint32_t :public tiny_hashtbl_t<uint32_t,val_t, max_set_size, tiny_hashtbl_cmp<hashfunc_adaptor_t<uint32_t> > > {};
}

#endif
