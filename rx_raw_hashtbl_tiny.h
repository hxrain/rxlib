#ifndef _RX_RAW_HASHTBL_TINY_H_
#define _RX_RAW_HASHTBL_TINY_H_

#include "rx_cc_macro.h"
#include "rx_assert.h"

namespace rx
{
    //-----------------------------------------------------
    //简单的定长槽位哈希表,使用开地址方式进行冲突处理,用于简单的哈希搜索管理器,自身不进行任何内存管理.
    template<class val_t,uint32_t max_slot_size>
    class raw_hashtbl_tiny_t
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
        node_t          m_nodes[max_slot_size];             //节点数组
        uint32_t        m_using;                            //当前节点的数量
        uint32_t        m_collision;                        //发生的value哈希冲突总数
    public:
        raw_hashtbl_tiny_t() { clear(); }
        //-------------------------------------------------
        //添加节点
        //返回值:NULL失败;其他成功.
        inline node_t *push(uint32_t hash_code,const val_t &value)
        {
            for(uint32_t i=0;i<max_slot_size;++i)
            {
                uint32_t pos=(hash_code+i)%max_slot_size;  //计算位置
                node_t &node = m_nodes[pos];               //得到节点
                if (!node.state)
                {//该节点尚未使用,那么就直接使用
                    node.state=i+1;                        //记录当前节点冲突顺序
                    m_collision+=i;                        //记录冲突总数
                    ++m_using;
                    return &node;
                }
                else if (node.value==value)
                    return &node;                          //该节点已经被使用了,且value也是重复的,那么就直接给出吧
            }

            return NULL;                                   //转了一圈没地方了!
        }

        //-------------------------------------------------
        //搜索节点
        //返回值:NULL未找到;其他成功
        inline node_t *find(uint32_t hash_code,const val_t &value) const
        {
            for(uint32_t i=0;i<max_slot_size;++i)
            {
                uint32_t I=(hash_code+i)%max_slot_size;    //计算位置
                node_t &node = m_nodes[I];                 //得到节点
                if (!node.state)
                    return NULL;                           //直接就碰到空档了,不用继续了

                if (node.value==value)
                    return &node;                          //顺延后找到了
            }

            return NULL;                                   //转了一圈没找到!
        }
        //-------------------------------------------------
        //删除节点
        inline bool remove(node_t *node)
        {
            if (!node|| node->state==0)
                return false;
            node->state=0;
            --m_using;
            return true;
        }
        //-------------------------------------------------
        //清除哈希搜索
        inline void clear()
        {
            memset(m_nodes, 0, sizeof(m_nodes));
            m_using = 0;
            m_collision = 0;
        }
        //-------------------------------------------------
        //最大节点数量
        uint32_t capacity() { return max_slot_size; }
        //已经使用的节点数量
        uint32_t size() { return m_using; }
        //插入位槽冲突总数
        uint32_t collision() { return m_collision; }
        //根据节点序号访问节点
        node_t* nodes(uint32_t pos) { return m_nodes[pos]; }
    };

    //-----------------------------------------------------
    //基于原始哈希表封装一个轻量级的集合容器
    template<class val_t,uint32_t max_set_size,class hash_t>
    class raw_set_t
    {
        typedef raw_hashtbl_tiny_t<val_t, max_set_size> baseset_t;
        typedef typename baseset_t::node_t node_t;
        hash_t      m_hash_func;
        baseset_t   m_base_sets;
    public:
        //对外暴露hash函数包装对象,便于随时更换具体的哈希函数
        hash_t& hash_func() { return m_hash_func; }
        //在集合中插入元素
        bool insert(const val_t &val)
        {
            uint32_t hash_code = m_hash_func(val);
            node_t *node = m_base_sets.push(hash_code, val);
            if (!node) return false;
            node->value = val;
            return true;
        }
        //查找元素是否存在
        bool find(const val_t &val) const
        {
            uint32_t hash_code = m_hash_func(val);
            node_t *node = m_base_sets.find(hash_code, val);
            return node!=NULL;
        }
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
        uint32_t capacity() { return m_base_sets.capacity(); }
        //已经使用的节点数量
        uint32_t size() { return m_base_sets.size(); }
        //插入位槽冲突总数
        uint32_t collision() { return m_base_sets.collision(); }

        //-------------------------------------------------
        //定义简单的只读迭代器
        class iterator
        {
            const raw_set_t &m_set;
            uint32_t        m_pos;
        protected:
            iterator(const raw_set_t &s, uint32_t pos) :m_set(s), m_pos(pos) {}
        public:
            bool operator==(const iterator &i)const { return &m_set == &i.m_set&&m_pos == i.m_pos; }
            const val_t& operator*() const
            {
                rx_assert(m_pos<max_set_size&&m_set.nodes(m_pos)->state);
                return m_set.nodes(m_pos)->value;
            }
            iterator& operator++()
            {
                while (m_pos < max_set_size)
                    if (m_set.nodes(m_pos++)->state)
                        break;                          //尝试找到下一个有效的位置
                return *this;
            }
        };

        //-------------------------------------------------
        //准备遍历集合,返回遍历的初始位置
        iterator& begin() const
        {
            for (uint32_t pos = 0; pos < max_set_size; ++pos)
            {
                node_t *node = m_base_sets.nodes(pos);
                if (node->state)
                    return iterator(*this,pos);
            }
            return iterator(*this, max_set_size);
        }
        //-------------------------------------------------
        //返回遍历的结束位置
        iterator& end() const { iterator(*this, max_set_size); }
        //-------------------------------------------------
        //删除指定位置的元素
        iterator& erase(const iterator &i)
        {
            rx_assert(i.m_pos<max_set_size && &i.m_set==this);
            node_t *node = m_base_sets.nodes(i.m_pos);
            node->value = 0;
            m_base_sets.remove(node);
            return ++i;
        }
        //-------------------------------------------------
        //清空全部的元素
        void clear()
        {
            for (iterator i = begin(); i != end(); ++i)
                i = erase(i);
        }
    };

 }
#endif
