#ifndef _RX_DTL_SKIPLIST_H_
#define _RX_DTL_SKIPLIST_H_

#include "rx_cc_macro.h"
#include "rx_mem_alloc.h"
#include "rx_hash_rand.h"
#include "rx_raw_skiplist.h"

namespace rx
{
    //-----------------------------------------------------
    //基于skiplist的集合容器节点类型
    template<class key_t>
    struct tiny_skipset_node_t
    {
        //-------------------------------------------------
        //比较节点与给定key的大小
        //返回值:n<key为<0;n==key为0;n>key为>0
        template<class KT>
        static int cmp(const tiny_skipset_node_t &n,const KT &key){return n.key-key;}
        //-------------------------------------------------
        //计算key需要的扩展尺寸
        template<class KT>
        static int ext_size(const KT &k){return 0;}

        //-------------------------------------------------
        //进行定向构造并初始化
        template<class val_t>
        void OC(uint32_t level,val_t &val){ct::OC(&key,val);}
        //-------------------------------------------------
        key_t  key;
        struct tiny_skipset_node_t *next[1];                //跳表实现的关键:分层的后趋节点指针,必须放在节点的最后,用于弹性扩展访问
    };

    //-----------------------------------------------------
    //基于skiplist的跳表容器节点类型
    template<class key_t,class val_t>
    struct tiny_skiplist_node_t
    {
        //-------------------------------------------------
        //比较节点与给定key的大小
        //返回值:n<key为<0;n==key为0;n>key为>0
        template<class KT>
        static int cmp(const tiny_skiplist_node_t &n,const KT &key){return n.key-key;}
        //-------------------------------------------------
        //计算key需要的扩展尺寸
        template<class KT,class VT>
        static int ext_size(const KT &k,const VT& v){return 0;}

        //-------------------------------------------------
        //进行定向构造并初始化
        template<class KT,class VT>
        void OC(uint32_t level,KT &k,VT &v){ct::OC(&key,k);ct::OC(&val,v);}
        //-------------------------------------------------
        key_t  key;
        val_t  val;
        struct tiny_skiplist_node_t *next[1];               //跳表实现的关键:分层的后趋节点指针,必须放在节点的最后,用于弹性扩展访问
    };

    //-----------------------------------------------------
    //封装基于跳表的基础集合容器类
    //-----------------------------------------------------
    template<class key_t,uint32_t MAX_LEVEL=LOG2<256>::result>
    class tiny_skipset_t
    {
        typedef tiny_skipset_node_t<key_t>          node_t;
        typedef raw_skiplist_t<node_t,MAX_LEVEL>    skiplist_t;
        rand_skeeto_triple_t    m_rnd;
        skiplist_t              m_list;
    public:
        //-------------------------------------------------
        tiny_skipset_t(mem_allotter_i &ma):m_list(ma,m_rnd){}
        virtual ~tiny_skipset_t() {clear();}
        //-------------------------------------------------
        //定义简单的只读迭代器
        class iterator
        {
            node_t  *m_node;
            friend class tiny_skipset_t;
        public:
            //---------------------------------------------
            iterator(const node_t *node) :m_node(node) {}
            iterator(const iterator &i):m_node(i.m_node){}
            //---------------------------------------------
            bool operator==(const iterator &i)const { return m_node == i.m_node; }
            bool operator!=(const iterator &i)const { return !(operator==(i));}
            //---------------------------------------------
            iterator& operator=(iterator &i) {m_node=i.m_node; return *this;}
            //---------------------------------------------
            const key_t& operator*() const {return m_node->key;}
            //---------------------------------------------
            iterator& operator++()
            {
                m_node=m_node->next[0];
                return reinterpret_cast<iterator&>(*this);
            }
        };
        //-------------------------------------------------
        //在集合中插入元素并赋值构造
        template<class val_t>
        bool insert(const val_t &val,bool duplication=false)
        {
            uint32_t level=0;
            node_t *node=m_list.insert_raw(val,node_t::ext_size(val),level,duplication);
            if (!node)
                return false;
            node->OC(level,val);
            return true;
        }
        //-------------------------------------------------
        //查找元素是否存在
        template<class val_t>
        bool find(const val_t &val) const {return m_list.find(val)!=NULL;}
        //-------------------------------------------------
        //删除元素并默认析构
        template<class val_t>
        bool erase(const val_t &val) {return m_list.earse(val);}
        //已经使用的节点数量
        uint32_t size() const { return m_list.size(); }
        //-------------------------------------------------
        //准备遍历集合,返回遍历的初始位置
        iterator begin() const{return iterator(m_list.head());}
        //-------------------------------------------------
        //返回遍历的结束位置
        iterator end() const { return iterator(NULL); }
        //-------------------------------------------------
        //获取集合的尾节点迭代器(可用于快速获取尾节点的值)
        iterator rend() const { return iterator(m_list.tail()); }
        //-------------------------------------------------
        //删除指定位置的元素
        //返回值:是否删除了当前值
        bool erase(iterator &i)
        {
            rx_assert(i.m_node!=NULL);
            if (i.m_node==NULL)
                return false;

            bool rc=m_list.earse(i.m_node);
            ++i;
            return rc;
        }
        //-------------------------------------------------
        //清空全部的元素
        void clear(){m_list.clear();}
    };

    //---------------------------------------------------------
    //封装跳表基础容器类
    //---------------------------------------------------------
    template<class key_t,class val_t,uint32_t MAX_LEVEL=LOG2<256>::result>
    class tiny_skiplist_t
    {
        typedef tiny_skiplist_node_t<key_t,val_t>   node_t;
        typedef raw_skiplist_t<node_t,MAX_LEVEL>    skiplist_t;
        rand_skeeto_triple_t    m_rnd;
        skiplist_t              m_list;
    public:
        //-------------------------------------------------
        //构造的时候绑定节点空间
        tiny_skiplist_t(mem_allotter_i &ma):m_list(ma,m_rnd){}
        virtual ~tiny_skiplist_t() {clear();}
        //-------------------------------------------------
        //已经使用的节点数量
        uint32_t size() const { return m_list.size(); }

        //-------------------------------------------------
        //定义简单的只读迭代器
        class iterator
        {
            node_t  *m_node;
            friend class tiny_skiplist_t;
        public:
            //---------------------------------------------
            iterator(const node_t *node) :m_node(node) {}
            iterator(const iterator &i):m_node(i.m_node){}
            //---------------------------------------------
            bool operator==(const iterator &i)const { return m_node == i.m_node; }
            bool operator!=(const iterator &i)const { return !(operator==(i));}
            //---------------------------------------------
            iterator& operator=(iterator &i) {m_node=i.m_node; return *this;}
            //---------------------------------------------
            //* 运算符重载,用于获取当前节点的val值
            const val_t& operator* () const {return m_node->val;}
            //()运算符重载,用于获取当前节点的key值
            const key_t& operator()() const {return m_node->key;}
            //---------------------------------------------
            iterator& operator++()
            {
                m_node=m_node->next[0];
                return reinterpret_cast<iterator&>(*this);
            }
        };
        //-------------------------------------------------
        //在跳表中插入元素并赋值构造
        template<class KT>
        bool insert(const KT &key,const val_t &val,bool duplication=false)
        {
            uint32_t level=0;
            node_t *node=m_list.insert_raw(key,node_t::ext_size(key,val),level,duplication);
            if (!node)
                return false;
            node->OC(level,key,val);
            return true;
        }
        //-------------------------------------------------
        //查找元素是否存在
        template<class KT>
        bool find(const KT &key) const {return m_list.find(key)!=NULL;}
        //-------------------------------------------------
        //删除元素并默认析构
        template<class KT>
        bool erase(const KT &key) {return m_list.earse(key);}
        //-------------------------------------------------
        //准备遍历跳表,返回遍历的初始位置
        iterator begin() const{return iterator(m_list.head());}
        //-------------------------------------------------
        //返回遍历的结束位置
        iterator end() const { return iterator(NULL); }
        //-------------------------------------------------
        //获取跳表的尾节点迭代器(可用于快速获取尾节点的值)
        iterator rend() const { return iterator(m_list.tail()); }
        //-------------------------------------------------
        //删除指定位置的元素
        //返回值:是否删除了当前值
        bool erase(iterator &i)
        {
            rx_assert(i.m_node!=NULL);
            if (i.m_node==NULL)
                return false;

            bool rc=m_list.earse(i.m_node);
            ++i;
            return rc;
        }
        //-------------------------------------------------
        //清空全部的元素
        void clear(){m_list.clear();}
    };

}

#endif
