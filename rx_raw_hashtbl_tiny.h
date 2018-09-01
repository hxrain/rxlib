#ifndef _RX_RAW_HASHTBL_TINY_H_
#define _RX_RAW_HASHTBL_TINY_H_

#include "rx_cc_macro.h"
#include "rx_assert.h"

namespace rx
{
    //-----------------------------------------------------
    //简单的定长槽位哈希表,使用开地址方式进行冲突处理,用于简单的哈希搜索管理器,自身不进行任何内存管理.
    //-----------------------------------------------------
    template<class val_t>
    class raw_hashtbl_t
    {
    public:
        //-------------------------------------------------
        //哈希表节点基类,使用的时候,必须让真正的节点继承于此
        typedef struct node_t
        {
            uint32_t    state;                              //记录当前节点是否被使用,0未使用;1正常被使用;>1哈希冲突序数(比如3就是在冲突点后第三个位置)
            val_t       value;                              //哈希节点的value值
        }node_t;
    private:
        node_t         *m_nodes;                            //节点数组
        uint32_t        m_using;                            //当前节点的数量
        uint32_t        m_collision;                        //发生的value哈希冲突总数
        uint32_t        m_max_slot_size;
    public:
        //-------------------------------------------------
        raw_hashtbl_t():m_nodes(NULL) {}
        //绑定最终可用的节点空间
        //-------------------------------------------------
        bool bind(node_t* nodes,uint32_t max_slot_size)
        {
            if (m_nodes)
                return false;
            m_nodes=nodes;
            memset(m_nodes,0,sizeof(node_t)*max_slot_size);
            m_max_slot_size=max_slot_size;
            m_using=0;
            m_collision=0;
            return true;
        }
        //-------------------------------------------------
        //添加节点
        //返回值:NULL失败;其他成功.
        template<class key_t>
        node_t *push(uint32_t hash_code,const key_t &value)
        {
            for(uint32_t i=0;i<m_max_slot_size;++i)
            {
                uint32_t pos=(hash_code+i)%m_max_slot_size; //计算位置
                node_t &node = m_nodes[pos];                //得到节点
                if (!node.state)
                {//该节点尚未使用,那么就直接使用
                    node.state=i+1;                         //记录当前节点冲突顺序
                    m_collision+=i;                         //记录冲突总数
                    ++m_using;
                    return &node;
                }
                else if (node.value==value)
                    return &node;                           //该节点已经被使用了,且value也是重复的,那么就直接给出吧
            }

            return NULL;                                    //转了一圈没地方了!
        }

        //-------------------------------------------------
        //搜索节点
        //返回值:NULL未找到;其他成功
        template<class key_t>
        node_t *find(uint32_t hash_code,const key_t &value) const
        {
            for(uint32_t i=0;i<m_max_slot_size;++i)
            {
                uint32_t I=(hash_code+i)%m_max_slot_size;   //计算位置
                node_t &node = m_nodes[I];                  //得到节点
                if (!node.state)
                    return NULL;                            //直接就碰到空档了,不用继续了

                if (node.value==value)
                    return &node;                           //顺延后找到了
            }

            return NULL;                                    //转了一圈没找到!
        }
        //-------------------------------------------------
        //删除节点
        bool remove(node_t *node)
        {
            if (!node|| node->state==0)
                return false;
            node->state=0;
            --m_using;
            return true;
        }
        //-------------------------------------------------
        //最大节点数量
        uint32_t capacity() const { return m_max_slot_size; }
        //已经使用的节点数量
        uint32_t size() const { return m_using; }
        //插入位槽冲突总数
        uint32_t collision() const { return m_collision; }
    };

    //-----------------------------------------------------
    //基于原始哈希表封装的轻量级集合容器
    //-----------------------------------------------------
    template<class val_t,uint32_t max_set_size,class hash_t>
    class tiny_set_t
    {
        typedef raw_hashtbl_t<val_t> baseset_t;
        typedef typename baseset_t::node_t node_t;

        hash_t      m_hash_func;                            //值处理的哈希函数适配器对象
        baseset_t   m_base_sets;                            //底层哈希功能封装
        node_t      m_nodes[max_set_size];                  //真实的哈希表节点数组空间

        //尝试找到pos的下一个有效的位置
        uint32_t m_next(uint32_t pos) const
        {
            while (++pos < max_set_size)
                if (m_nodes[pos].state)
                    return pos;
            return max_set_size;
        }
    public:
        //-------------------------------------------------
        //构造的时候绑定节点空间
        tiny_set_t(){m_base_sets.bind(m_nodes,max_set_size);}
        virtual ~tiny_set_t(){clear();}
        //-------------------------------------------------
        //对外暴露hash函数包装对象,便于随时更换具体的哈希函数
        hash_t& hash_func() const { return m_hash_func; }
        //-------------------------------------------------
        //在集合中插入元素
        bool insert(const val_t &val)
        {
            uint32_t hash_code = m_hash_func(val);
            node_t *node = m_base_sets.push(hash_code, val);
            if (!node) return false;
            node->value = val;
            return true;
        }
        //-------------------------------------------------
        //查找元素是否存在
        bool find(const val_t &val) const
        {
            uint32_t hash_code = m_hash_func(val);
            node_t *node = m_base_sets.find(hash_code, val);
            return node!=NULL;
        }
        //-------------------------------------------------
        //删除元素
        bool erase(const val_t &val)
        {
            uint32_t hash_code = m_hash_func(val);
            node_t *node = m_base_sets.find(hash_code, val);
            if (!node) return false;
            node->value = 0;
            return m_base_sets.remove(node);
        }
        //-------------------------------------------------
        //最大节点数量
        uint32_t capacity() const { return m_base_sets.capacity(); }
        //已经使用的节点数量
        uint32_t size() const { return m_base_sets.size(); }
        //插入位槽冲突总数
        uint32_t collision() const { return m_base_sets.collision(); }

        //-------------------------------------------------
        //定义简单的只读迭代器
        class iterator
        {
            const tiny_set_t &m_set;
            uint32_t        m_pos;
            friend class tiny_set_t;

        public:
            //---------------------------------------------
            iterator(const tiny_set_t &s, uint32_t pos) :m_set(s), m_pos(pos) {}
            iterator(const iterator &i):m_set(i.m_set),m_pos(i.m_pos){}
            //---------------------------------------------
            bool operator==(const iterator &i)const { return &m_set == &i.m_set&&m_pos == i.m_pos; }
            bool operator!=(const iterator &i)const { return !(operator==(i));}
            //---------------------------------------------
            iterator& operator=(iterator &i){m_set=i.m_set;m_pos=i.m_pos;return *this;}
            //---------------------------------------------
            const val_t& operator*() const
            {
                rx_assert(m_pos<max_set_size&&m_set.m_nodes[m_pos].state);
                return m_set.m_nodes[m_pos].value;
            }
            //---------------------------------------------
            iterator& operator++()
            {
                m_pos=m_set.m_next(m_pos);                  //尝试找到下一个有效的位置
                return reinterpret_cast<iterator&>(*this);
            }
        };

        //-------------------------------------------------
        //准备遍历集合,返回遍历的初始位置
        iterator begin() const
        {
            for (uint32_t pos = 0; pos < max_set_size; ++pos)
            {
                const node_t &node = m_nodes[pos];
                if (node.state)
                    return iterator(*this,pos);
            }
            return iterator(*this, max_set_size);
        }
        //-------------------------------------------------
        //返回遍历的结束位置
        iterator end() const { return iterator(*this, max_set_size); }
        //-------------------------------------------------
        //删除指定位置的元素
        //返回值:是否删除了当前值
        bool erase(iterator &i)
        {
            rx_assert(i.m_pos<max_set_size && &i.m_set==this);
            if (i.m_pos>=max_set_size || &i.m_set!=this)
                return false;

            node_t &node = m_nodes[i.m_pos];
            node.value = 0;
            m_base_sets.remove(&node);
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
    template<class key_t,class val_t,uint32_t max_set_size,class hash_t>
    class tiny_hashtbl_t
    {
        typedef struct node_val_t
        {
            key_t   key;
            val_t   val;
            bool operator==(const key_t& k){return key==k;}
        }node_val_t;

        typedef raw_hashtbl_t<node_val_t> baseset_t;
        typedef typename baseset_t::node_t node_t;

        hash_t      m_hash_func;                            //值处理的哈希函数适配器对象
        baseset_t   m_base_sets;                            //底层哈希功能封装
        node_t      m_nodes[max_set_size];                  //真实的哈希表节点数组空间

        //尝试找到pos的下一个有效的位置
        uint32_t m_next(uint32_t pos) const
        {
            while (++pos < max_set_size)
                if (m_nodes[pos].state)
                    return pos;
            return max_set_size;
        }
    public:
        //-------------------------------------------------
        //构造的时候绑定节点空间
        tiny_hashtbl_t(){m_base_sets.bind(m_nodes,max_set_size);}
        virtual ~tiny_hashtbl_t(){clear();}
        //-------------------------------------------------
        //对外暴露hash函数包装对象,便于随时更换具体的哈希函数
        hash_t& hash_func() const { return m_hash_func; }
        //-------------------------------------------------
        //在集合中插入元素
        bool insert(const key_t &key,const val_t &val)
        {
            uint32_t hash_code = m_hash_func(key);
            node_t *node = m_base_sets.push(hash_code, key);
            if (!node) return false;
            node->value.key = key;
            node->value.val = val;
            return true;
        }
        //-------------------------------------------------
        //查找元素是否存在
        bool find(const key_t &key) const
        {
            uint32_t hash_code = m_hash_func(key);
            node_t *node = m_base_sets.find(hash_code, key);
            return node!=NULL;
        }
        //-------------------------------------------------
        //删除元素
        bool erase(const key_t &key)
        {
            uint32_t hash_code = m_hash_func(key);
            node_t *node = m_base_sets.find(hash_code, key);
            if (!node) return false;
            node->value.key = 0;
            node->value.val = 0;
            return m_base_sets.remove(node);
        }
        //-------------------------------------------------
        //最大节点数量
        uint32_t capacity() const { return m_base_sets.capacity(); }
        //已经使用的节点数量
        uint32_t size() const { return m_base_sets.size(); }
        //插入位槽冲突总数
        uint32_t collision() const { return m_base_sets.collision(); }

        //-------------------------------------------------
        //定义简单的只读迭代器
        class iterator
        {
            const tiny_hashtbl_t &m_set;
            uint32_t        m_pos;
            friend class tiny_hashtbl_t;

        public:
            //---------------------------------------------
            iterator(const tiny_hashtbl_t &s, uint32_t pos) :m_set(s), m_pos(pos) {}
            iterator(const iterator &i):m_set(i.m_set),m_pos(i.m_pos){}
            //---------------------------------------------
            bool operator==(const iterator &i)const { return &m_set == &i.m_set&&m_pos == i.m_pos; }
            bool operator!=(const iterator &i)const { return !(operator==(i));}
            //---------------------------------------------
            iterator& operator=(iterator &i){m_set=i.m_set;m_pos=i.m_pos;return *this;}
            //---------------------------------------------
            const val_t& operator*() const
            {
                rx_assert(m_pos<max_set_size&&m_set.m_nodes[m_pos].state);
                return m_set.m_nodes[m_pos].value.val;
            }
            //---------------------------------------------
            const key_t& key() const
            {
                rx_assert(m_pos<max_set_size&&m_set.m_nodes[m_pos].state);
                return m_set.m_nodes[m_pos].value.key;
            }
            //---------------------------------------------
            iterator& operator++()
            {
                m_pos=m_set.m_next(m_pos);                  //尝试找到下一个有效的位置
                return reinterpret_cast<iterator&>(*this);
            }
        };

        //-------------------------------------------------
        //准备遍历集合,返回遍历的初始位置
        iterator begin() const
        {
            for (uint32_t pos = 0; pos < max_set_size; ++pos)
            {
                const node_t &node = m_nodes[pos];
                if (node.state)
                    return iterator(*this,pos);
            }
            return iterator(*this, max_set_size);
        }
        //-------------------------------------------------
        //返回遍历的结束位置
        iterator end() const { return iterator(*this, max_set_size); }
        //-------------------------------------------------
        //删除指定位置的元素
        //返回值:是否删除了当前值
        bool erase(iterator &i)
        {
            rx_assert(i.m_pos<max_set_size && &i.m_set==this);
            if (i.m_pos>=max_set_size || &i.m_set!=this)
                return false;

            node_t &node = m_nodes[i.m_pos];
            node.value.key = 0;
            node.value.val = 0;
            m_base_sets.remove(&node);
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
 }
#endif
