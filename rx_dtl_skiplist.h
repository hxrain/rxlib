#ifndef _RX_DTL_SKIPLIST_H_
#define _RX_DTL_SKIPLIST_H_

#include "rx_cc_macro.h"
#include "rx_mem_alloc.h"
#include "rx_mem_alloc_cntr.h"
#include "rx_hash_rand.h"
#include "rx_dtl_skiplist_raw.h"
#include <time.h>
#include "rx_str_tiny.h"

/*本单元封装实现了基于原始跳表的跳表集与跳表容器.

    //跳表容器
    template<class key_t,class val_t,uint32_t MAX_LEVEL=RX_SKLIST_MAXLEVEL,class rnd_t=skiplist_rnd_t>
    class skiplist_t;

    //便于使用的整数跳表
    typedef skiplist_t<uint32_t,uint32_t>       skiplist_ut;
    typedef skiplist_t<int32_t,int32_t>         skiplist_it;
    //便于使用的const char*为key的整数跳表
    typedef skiplist_t<const char*,uint32_t>    skiplist_ct;
    //便于使用的const wchar_t*为key的整数跳表
    typedef skiplist_t<const wchar_t*,uint32_t> skiplist_wt;
*/

namespace rx
{
    //-----------------------------------------------------
    //封装跳表基础容器类
    //-----------------------------------------------------
    template<class key_t,class val_t,uint32_t MAX_LEVEL = RX_SKLIST_MAXLEVEL,class rnd_t=skiplist_rnd_t>
    class skiplist_t
    {
        //-------------------------------------------------
        //skiplist跳表容器节点类型
        template<class kt,class vt>
        struct skiplist_node_t
        {
            //---------------------------------------------
            //比较节点与给定key的大小
            //返回值:n<key为<0;n==key为0;n>key为>0
            template<class KT>
            static int cmp(const void *ptr, const KT &key)
            {
                const skiplist_node_t &n = *(skiplist_node_t*)ptr;
                return n.key == key ? 0 : (n.key < key ? -1 : 1);
            }
            //---------------------------------------------
            //计算key与val需要的扩展尺寸
            template<class KT,class VT>
            static uint32_t ext_size(const KT &k, const VT& v, uint32_t &es1, uint32_t &es2) { return 0; }
            //计算key需要的扩展尺寸
            template<class KT>
            static uint32_t ext_size(const KT &k, uint32_t &es1) { return 0; }

            //---------------------------------------------
            //进行定向构造并初始化
            template<class KT,class VT>
            void OC(uint32_t level,KT &k,VT &v,uint32_t es1,uint32_t es2){ct::OC(&key,k);ct::OC(&val,v);}
            //进行定向构造并初始化
            template<class KT>
            void OC(uint32_t level, KT &k, uint32_t es1) { ct::OC(&key, k); ct::OC(&val);}
            //---------------------------------------------
            typedef kt node_key_t;                          //对节点内部真正的key与val的类型进行定义
            typedef vt node_val_t;
            node_key_t  key;
            node_val_t  val;
            sk_block_t  levels;                             //跳表实现的关键:分层的后趋节点指针,必须放在节点的最后,用于弹性扩展访问
        };
        //-------------------------------------------------
        //基于skiplist的跳表容器节点类型
        //进行了key类型的const char*偏特化处理,可以进行内部持久化;
        template<class vt>
        struct skiplist_node_t<const char*, vt>
        {
            //---------------------------------------------
            //比较节点与给定key的大小
            //返回值:n<key为<0;n==key为0;n>key为>0
            template<class KT>
            static int cmp(const void *ptr, const KT &key)
            {
                const skiplist_node_t &n = *(skiplist_node_t*)ptr;
                return st::strcmp(n.key.c_str(), key);
            }
            //---------------------------------------------
            //计算key需要的扩展尺寸
            template<class KT, class VT>
            static uint32_t ext_size(const KT &k, const VT& v, uint32_t &es1, uint32_t &es2) { es1 = st::strlen(k) + 1; return es1; }
            template<class KT>
            static uint32_t ext_size(const KT &k, uint32_t &es1) { es1 = st::strlen(k) + 1; return es1; }

            //---------------------------------------------
            //进行定向构造并初始化
            template<class KT, class VT>
            void OC(uint32_t level, KT &k, VT &v, uint32_t es1, uint32_t es2)
            {
                ct::OC(&key);
                ct::OC(&val, v);
                key.bind((char*)&levels.next[level], es1, k, es1 - 1);
            }
            template<class KT>
            void OC(uint32_t level, KT &k, uint32_t es1)
            {
                ct::OC(&key);
                ct::OC(&val);
                key.bind((char*)&levels.next[level], es1, k, es1 - 1);
            }
            //---------------------------------------------
            typedef tiny_string_t<char> node_key_t;         //对节点内部真正的key与val的类型进行定义
            typedef vt node_val_t;
            node_key_t  key;
            node_val_t  val;
            sk_block_t  levels;                             //跳表实现的关键:分层的后趋节点指针,必须放在节点的最后,用于弹性扩展访问
        };
        //-------------------------------------------------
        template<class vt>
        struct skiplist_node_t<const wchar_t*,vt>
        {
            //---------------------------------------------
            //比较节点与给定key的大小
            //返回值:n<key为<0;n==key为0;n>key为>0
            template<class KT>
            static int cmp(const void *ptr, const KT &key)
            {
                const skiplist_node_t &n = *(skiplist_node_t*)ptr;
                return st::strcmp(n.key.c_str(), key);
            }
            //---------------------------------------------
            //计算key与val需要的扩展尺寸
            template<class KT,class VT>
            static uint32_t ext_size(const KT &k,const VT& v,uint32_t &es1,uint32_t &es2){es1=(st::strlen(k)+1) * sc<wchar_t>::char_size();return es1;}
            template<class KT>
            static uint32_t ext_size(const KT &k, uint32_t &es1) { es1 = (st::strlen(k) + 1) * sc<wchar_t>::char_size(); return es1; }

            //---------------------------------------------
            //进行定向构造并初始化
            template<class KT,class VT>
            void OC(uint32_t level,KT &k,VT &v,uint32_t es1,uint32_t es2)
            {
                ct::OC(&key);
                ct::OC(&val,v);
                uint32_t cap = es1 / sc<wchar_t>::char_size();
                key.bind((wchar_t*)&levels.next[level],cap,k,cap-1);
            }
            template<class KT>
            void OC(uint32_t level, KT &k, uint32_t es1)
            {
                ct::OC(&key);
                ct::OC(&val);
                uint32_t cap = es1 / sc<wchar_t>::char_size();
                key.bind((wchar_t*)&levels.next[level], cap, k, cap - 1);
            }
            //---------------------------------------------
            typedef tiny_string_t<wchar_t> node_key_t;      //对节点内部真正的key与val的类型进行定义
            typedef vt node_val_t;
            node_key_t  key;
            node_val_t  val;
            sk_block_t  levels;                             //跳表实现的关键:分层的后趋节点指针,必须放在节点的最后,用于弹性扩展访问
        };

        //-------------------------------------------------
        typedef skiplist_node_t<key_t,val_t>   sk_node_t;   //定义最终使用的原始跳表节点类型
        typedef raw_skiplist_t<sk_node_t>      sk_list_t;   //定义最终使用的原始跳表容器类型
        //计算节点中在尾块之前需要保留的尺寸
        static const uint32_t node_rsv_size = sizeof(sk_node_t::node_key_t) + sizeof(sk_node_t::node_val_t);
        //-------------------------------------------------
        skiplist_rnd_level<rnd_t,MAX_LEVEL>    m_rnd_level; //随机层数生成器
        sk_list_t                              m_raw_list;  //最终使用的底层原始跳表容器
    public:
        typedef sk_node_t node_t;
        //-------------------------------------------------
        //构造的时候绑定节点空间
        skiplist_t(mem_allotter_i &ma,uint32_t seed=1):m_rnd_level(seed),m_raw_list(ma, node_rsv_size){}
        skiplist_t(uint32_t seed=1):m_rnd_level(seed),m_raw_list(rx_global_mem_allotter(), node_rsv_size){}
        virtual ~skiplist_t() {clear();}
        //-------------------------------------------------
        //定义简单的只读迭代器
        class iterator
        {
            const node_t  *m_node;
            friend class skiplist_t;
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
            //* 运算符重载,用于获取当前节点的val值
            const typename node_t::node_val_t& operator* () const {return m_node->val;}
            //()运算符重载,用于获取当前节点的key值
            const typename node_t::node_key_t& operator()() const {return m_node->key;}
            //---------------------------------------------
            iterator& operator++()
            {
                m_node = next();
                return reinterpret_cast<iterator&>(*this);
            }
            //---------------------------------------------
            //判断当前迭代器是否无效
            bool is_end() { return m_node == NULL; }
            //---------------------------------------------
            //尝试获取当前迭代器的后趋节点指针
            const node_t* next() 
            { 
                uint8_t *ptr = (uint8_t*)m_node->levels.next[0];
                return (node_t*)(ptr ? ptr - node_rsv_size : NULL);
            }
        };
        //-------------------------------------------------
        //已经使用的节点数量
        uint32_t size() const { return m_raw_list.size(); }
        //-------------------------------------------------
        //准备遍历跳表,返回遍历的初始位置
        iterator begin() const { return iterator((node_t*)m_raw_list.head()); }
        //-------------------------------------------------
        //返回遍历的结束位置
        iterator end() const { return iterator(NULL); }
        //-------------------------------------------------
        //获取跳表的尾节点迭代器(可用于快速获取尾节点的值)
        iterator rbegin() const { return iterator((node_t*)m_raw_list.tail()); }
        //-------------------------------------------------
        //删除指定位置的元素
        //返回值:是否删除了当前值
        bool erase(iterator &i)
        {
            rx_assert(i.m_node != NULL);
            if (i.m_node == NULL)
                return false;
            const node_t::node_key_t &key = i.m_node->key;
            ++i;
            return erase(key);
        }
        //-------------------------------------------------
        //在跳表中插入元素并赋值构造,dup可获知是否重复
        //返回值:end()-内存不足;其他成功
        template<class KT>
        iterator insert(const KT &key,const val_t &val, bool *dup = NULL)
        {
            uint32_t level=m_rnd_level.make();
            bool duplication=false;
            uint32_t es1=0,es2=0;
            node_t *node = (node_t*)m_raw_list.insert_raw(key, duplication, node_t::ext_size(key, val, es1, es2), level);
            if (!node)
                return end();

            if (dup) *dup = duplication;

            if (duplication)
                return iterator(node);

            node->OC(level,key,val,es1,es2);
            return iterator(node);
        }
        //-------------------------------------------------
        //在跳表中插入元素key(首次val默认构造);dup可获取key是否重复;
        //返回值:end()-内存不足;其他成功
        template<class KT>
        iterator insert(const KT &key, bool *dup=NULL)
        {
            uint32_t level = m_rnd_level.make();
            bool duplication = false;
            uint32_t es1 = 0;
            node_t *node = m_raw_list.insert_raw(key, duplication, node_t::ext_size(key, es1), level);
            if (!node)
                return end();

            if (dup) *dup = duplication;

            if (duplication)
            {//重复的时候,直接返回val指针
                return iterator(node);
            }

            //新增的时候进行节点与key/val的初始化
            node->OC(level, key, es1);
            return iterator(node);
        }
        //-------------------------------------------------
        //查找元素是否存在
        template<class KT>
        iterator find(const KT &key) const {return iterator((node_t*)m_raw_list.find(key));}
        //-------------------------------------------------
        //删除元素并默认析构
        //删除元素并默认析构
        template<class KT>
        bool erase(const KT &key)
        {
            node_t *node = (node_t*)m_raw_list.remove_find(key);
            if (!node) return false;
            ct::OD(node);
            m_raw_list.remove_free(node);
            return true;
        }
        //-------------------------------------------------
        //清空全部的元素
        void clear()
        {
            for (iterator I = begin(); I != end(); ++I)
                ct::OD(I.m_node);
            m_raw_list.clear();
        }
    };

    //-----------------------------------------------------
    //语法糖,定义一个便于使用的整数跳表
    typedef skiplist_t<uint32_t,uint32_t>        skiplist_ut;
    typedef skiplist_t<int32_t,int32_t>          skiplist_it;
    //语法糖,定义一个便于使用的const char*为key的整数跳表
    typedef skiplist_t<const char*,uint32_t>     skiplist_ct;
    //语法糖,定义一个便于使用的const wchar_t*为key的整数跳表
    typedef skiplist_t<const wchar_t*,uint32_t>  skiplist_wt;
}

#endif
