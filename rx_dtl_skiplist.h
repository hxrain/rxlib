#ifndef _RX_DTL_SKIPLIST_H_
#define _RX_DTL_SKIPLIST_H_

#include "rx_cc_macro.h"
#include "rx_mem_alloc.h"
#include "rx_mem_alloc_cntr.h"
#include "rx_hash_rand.h"
#include "rx_raw_skiplist.h"
#include <time.h>
#include "rx_str_tiny.h"

/*本单元封装实现了基于原始跳表的跳表集与跳表容器.
    //-----------------------------------------------------
    //跳表集容器
    template<class key_t,uint32_t MAX_LEVEL,class rnd_t=tiny_skiplist_rnd_t>
    class tiny_skipset_t;

    //便于使用的整数跳表集合
    typedef tiny_skipset_t<uint32_t,32> tiny_skipset_uint32_t;
    //便于使用的const char*串跳表集合
    typedef tiny_skipset_t<const char*,32> tiny_skipset_cstr_t;
    //便于使用的const wchar_t*串跳表集合
    typedef tiny_skipset_t<const wchar_t*,32> tiny_skipset_wstr_t;

    //-----------------------------------------------------
    //跳表容器
    template<class key_t,class val_t,uint32_t MAX_LEVEL,class rnd_t=tiny_skiplist_rnd_t>
    class tiny_skiplist_t;

    //便于使用的整数跳表
    typedef tiny_skiplist_t<uint32_t,uint32_t,32> tiny_skiplist_uint32_t;
    //便于使用的const char*为key的整数跳表
    typedef tiny_skiplist_t<const char*,uint32_t,32> tiny_skiplist_cstr_uint32_t;
    //便于使用的const wchar_t*为key的整数跳表
    typedef tiny_skiplist_t<const wchar_t*,uint32_t,32> tiny_skiplist_wstr_uint32_t;

*/

namespace rx
{

    //-----------------------------------------------------
    //进行跳表层级随机数生成器的封装
    template<class rnd_t,uint32_t MAX_LEVEL>
    class tiny_skiplist_rnd_level
    {
        rnd_t   m_rnd;
    public:
        void seed(uint32_t s)
        {
            if (s<=1) s=(uint32_t)time(NULL);
            m_rnd.seed(s);
        }
        //-----------------------------------------------------
        //生成一个随机的层数:>=1;<=最大层数
        uint32_t make()
        {
            uint32_t rc=1;
            while(rc<MAX_LEVEL&&(m_rnd.get()&0xFFFFFF)<(0xFFFFFF>>2))   //随机概率连续小于25%则层高增加(相当于4叉树)
                ++rc;
            rx_assert(rc<=MAX_LEVEL);
            return rc;
        }
    };
    //统一指定一下跳表使用的随机数发生器类型
    typedef rand_skiplist_t tiny_skiplist_rnd_t;



    //-----------------------------------------------------
    //skiplist集合容器节点类型
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
        void OC(uint32_t level,val_t &val,uint32_t es){ct::OC(&key,val);}
        //-------------------------------------------------
        typedef key_t node_key_t;
        key_t  key;
        struct tiny_skipset_node_t *next[1];                //跳表实现的关键:分层的后趋节点指针,必须放在节点的最后,用于弹性扩展访问
    };

    //对于skipset的const char*类型的key进行偏特化,内部统一进行内存分配与key内容记录
    //对于char*的key类型,扔保留原始key指针类型
    template<class CT>
    struct tiny_skipset_node_t<const CT*>
    {
        //-------------------------------------------------
        //比较节点与给定key的大小
        //返回值:n<key为<0;n==key为0;n>key为>0
        template<class KT>
        static int cmp(const tiny_skipset_node_t &n,const KT &key){return st::strcmp(n.key.c_str(),key);}
        //-------------------------------------------------
        //计算key需要的扩展尺寸
        static int ext_size(const CT* k){return st::strlen(k)+1;}

        //-------------------------------------------------
        //进行定向构造并初始化
        template<class val_t>
        void OC(uint32_t level,val_t &val,uint32_t es)
        {
            ct::OC(&key);
            key.bind((CT*)&next[level],es,val,es-1);
        }
        //-------------------------------------------------
        typedef tiny_string_head_t<CT> node_key_t;
        node_key_t  key;
        struct tiny_skipset_node_t *next[1];                //跳表实现的关键:分层的节点后趋数组,必须放在节点的最后,用于弹性扩展访问
    };



    //-----------------------------------------------------
    //封装基于跳表的基础集合容器类
    //-----------------------------------------------------
    template<class key_t,uint32_t MAX_LEVEL=32,class rnd_t=tiny_skiplist_rnd_t>
    class tiny_skipset_t
    {
        typedef tiny_skipset_node_t<key_t>          sk_node_t;
        typedef raw_skiplist_t<sk_node_t,MAX_LEVEL> skiplist_t;
        tiny_skiplist_rnd_level<rnd_t,MAX_LEVEL>    m_rnd_level;
        skiplist_t                                  m_list;
    public:
        typedef sk_node_t node_t;
        //-------------------------------------------------
        tiny_skipset_t(mem_allotter_i &ma,uint32_t seed=1):m_list(ma){m_rnd_level.seed(seed);}
        tiny_skipset_t(uint32_t seed=1):m_list(global_mem_allotter()){m_rnd_level.seed(seed);}
        virtual ~tiny_skipset_t() {clear();}
        //-------------------------------------------------
        //定义简单的只读迭代器
        class iterator
        {
            const node_t  *m_node;
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
            const typename node_t::node_key_t& operator*() const {return m_node->key;}
            //---------------------------------------------
            iterator& operator++()
            {
                m_node=m_node->next[0];
                return reinterpret_cast<iterator&>(*this);
            }
        };
        //-------------------------------------------------
        //在集合中插入元素并赋值构造
        //返回值:<0-内存不足;0-值重复;>0-成功
        template<class val_t>
        int insert(const val_t &val)
        {
            uint32_t level=m_rnd_level.make();
            bool duplication=false;
            uint32_t es=node_t::ext_size(val);
            node_t *node=m_list.insert_raw(val,duplication,es,level);
            if (duplication)
                return 0;
            if (!node)
                return -1;
            node->OC(level,val,es);
            return 1;
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
        iterator rbegin() const { return iterator(m_list.tail()); }
        //-------------------------------------------------
        //删除指定位置的元素
        //返回值:是否删除了当前值
        bool erase(iterator &i)
        {
            rx_assert(i.m_node!=NULL);
            if (i.m_node==NULL)
                return false;

            bool rc=m_list.earse(i.m_node->key);
            ++i;
            return rc;
        }
        //-------------------------------------------------
        #if RX_RAW_SKIPLIST_DEBUG_PRINT
        void print(){m_list.print();}
        #endif
        //-------------------------------------------------
        //清空全部的元素
        void clear(){m_list.clear();}
    };

    //语法糖,定义一个便于使用的整数跳表集合
    typedef tiny_skipset_t<uint32_t,32> tiny_skipset_uint32_t;
    //语法糖,定义一个便于使用的const char*串跳表集合
    typedef tiny_skipset_t<const char*,32> tiny_skipset_cstr_t;
    //语法糖,定义一个便于使用的const wchar_t*串跳表集合
    typedef tiny_skipset_t<const wchar_t*,32> tiny_skipset_wstr_t;




    //-----------------------------------------------------
    //skiplist跳表容器节点类型
    template<class key_t,class val_t>
    struct tiny_skiplist_node_t
    {
        //-------------------------------------------------
        //比较节点与给定key的大小
        //返回值:n<key为<0;n==key为0;n>key为>0
        template<class KT>
        static int cmp(const tiny_skiplist_node_t &n,const KT &key){return int(n.key-key);}
        template<class CT>
        static int cmp(const tiny_skiplist_node_t &n,const CT* key){return st::strcmp(n.key.c_str(),key);}
        //-------------------------------------------------
        //计算key需要的扩展尺寸
        template<class KT,class VT>
        static uint32_t ext_size(const KT &k,const VT& v,uint32_t &es1,uint32_t &es2){return 0;}

        //-------------------------------------------------
        //进行定向构造并初始化
        template<class KT,class VT>
        void OC(uint32_t level,KT &k,VT &v,uint32_t es1,uint32_t es2){ct::OC(&key,k);ct::OC(&val,v);}
        //-------------------------------------------------
        typedef key_t node_key_t;                           //对节点内部真正的key与val的类型进行定义
        typedef val_t node_val_t;
        node_key_t  key;
        node_val_t  val;
        struct tiny_skiplist_node_t *next[1];               //跳表实现的关键:分层的后趋节点指针,必须放在节点的最后,用于弹性扩展访问
    };

    //基于skiplist的跳表容器节点类型
    //进行了key类型的const char*偏特化处理,可以进行内部持久化;
    template<class CT,class val_t>
    struct tiny_skiplist_node_t<const CT*,val_t>
    {
        //-------------------------------------------------
        //比较节点与给定key的大小
        //返回值:n<key为<0;n==key为0;n>key为>0
        template<class KT>
        static int cmp(const tiny_skiplist_node_t &n,const KT &key){return st::strcmp(n.key.c_str(),key);}
        //-------------------------------------------------
        //计算key需要的扩展尺寸
        template<class KT,class VT>
        static uint32_t ext_size(const KT &k,const VT& v,uint32_t &es1,uint32_t &es2){es1=st::strlen(k)+1;return es1;}

        //-------------------------------------------------
        //进行定向构造并初始化
        template<class KT,class VT>
        void OC(uint32_t level,KT &k,VT &v,uint32_t es1,uint32_t es2)
        {
            ct::OC(&key);
            ct::OC(&val,v);
            key.bind((CT*)&next[level],es1,k,es1-1);
        }
        //-------------------------------------------------
        typedef tiny_string_head_t<CT> node_key_t;          //对节点内部真正的key与val的类型进行定义
        typedef val_t node_val_t;
        node_key_t  key;
        node_val_t  val;
        struct tiny_skiplist_node_t *next[1];               //跳表实现的关键:分层的后趋节点指针,必须放在节点的最后,用于弹性扩展访问
    };

    //---------------------------------------------------------
    //封装跳表基础容器类
    //---------------------------------------------------------
    template<class key_t,class val_t,uint32_t MAX_LEVEL=32,class rnd_t=tiny_skiplist_rnd_t>
    class tiny_skiplist_t
    {
        typedef tiny_skiplist_node_t<key_t,val_t>   sk_node_t;
        typedef raw_skiplist_t<sk_node_t,MAX_LEVEL> skiplist_t;
        tiny_skiplist_rnd_level<rnd_t,MAX_LEVEL>    m_rnd_level;
        skiplist_t                                  m_list;
    public:
        typedef sk_node_t node_t;
        //-------------------------------------------------
        //构造的时候绑定节点空间
        tiny_skiplist_t(mem_allotter_i &ma,uint32_t seed=1):m_list(ma){m_rnd_level.seed(seed);}
        tiny_skiplist_t(uint32_t seed=1):m_list(global_mem_allotter()){m_rnd_level.seed(seed);}
        virtual ~tiny_skiplist_t() {clear();}
        //-------------------------------------------------
        //已经使用的节点数量
        uint32_t size() const { return m_list.size(); }

        //-------------------------------------------------
        //定义简单的只读迭代器
        class iterator
        {
            const node_t  *m_node;
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
            const typename node_t::node_val_t& operator* () const {return m_node->val;}
            //()运算符重载,用于获取当前节点的key值
            const typename node_t::node_key_t& operator()() const {return m_node->key;}
            //---------------------------------------------
            iterator& operator++()
            {
                m_node=m_node->next[0];
                return reinterpret_cast<iterator&>(*this);
            }
        };
        //-------------------------------------------------
        //在跳表中插入元素并赋值构造
        //返回值:<0-内存不足;0-值重复;>0-成功
        template<class KT>
        int insert(const KT &key,const val_t &val)
        {
            uint32_t level=m_rnd_level.make();
            bool duplication=false;
            uint32_t es1=0,es2=0;
            node_t *node=m_list.insert_raw(key,duplication,node_t::ext_size(key,val,es1,es2),level);
            if (duplication)
                return 0;
            if (!node)
                return -1;
            node->OC(level,key,val,es1,es2);
            return 1;
        }
        //-------------------------------------------------
        //查找元素是否存在
        template<class KT>
        node_t* find(const KT &key) const {return m_list.find(key);}
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
        iterator rbegin() const { return iterator(m_list.tail()); }
        //-------------------------------------------------
        //删除指定位置的元素
        //返回值:是否删除了当前值
        bool erase(iterator &i)
        {
            rx_assert(i.m_node!=NULL);
            if (i.m_node==NULL)
                return false;

            bool rc=m_list.earse(i.m_node.key);
            ++i;
            return rc;
        }
        //-------------------------------------------------
        #if RX_RAW_SKIPLIST_DEBUG_PRINT
        void print(){m_list.print();}
        #endif
        //-------------------------------------------------
        //清空全部的元素
        void clear(){m_list.clear();}
    };

    //语法糖,定义一个便于使用的整数跳表
    typedef tiny_skiplist_t<uint32_t,uint32_t,32> tiny_skiplist_uint32_t;
    //语法糖,定义一个便于使用的const char*为key的整数跳表
    typedef tiny_skiplist_t<const char*,uint32_t,32> tiny_skiplist_cstr_uint32_t;
    //语法糖,定义一个便于使用的const wchar_t*为key的整数跳表
    typedef tiny_skiplist_t<const wchar_t*,uint32_t,32> tiny_skiplist_wstr_uint32_t;
}

#endif
