#ifndef _RX_DTL_HASHMAP_H_
#define _RX_DTL_HASHMAP_H_

#include "rx_cc_macro.h"
#include "rx_dtl_skiplist.h"
#include "rx_hash_data.h"
#include "rx_hash_int.h"
#include "rx_ct_util.h"
#include "rx_str_tiny.h"

/*
    <变长哈希映射容器,相当于对跳表进行了哈希分段>
*/

namespace rx
{
    //-----------------------------------------------------
    //哈希map使用的哈希槽计算器
    class hashmap_cmp_t
    {
    public:
        template<class KT>
        static uint32_t hash(const KT &k) { return rx_hash_murmur(&k, sizeof(k)); }
        static uint32_t hash(const char *k) { return rx_hash_murmur(k, st::strlen(k)); }
        static uint32_t hash(const wchar_t *k) { return rx_hash_murmur(k, st::strlen(k)) * sizeof(wchar_t); }
        static uint32_t hash(const uint32_t &k) { return rx_hash_skeeto_3s(k); }
        static uint32_t hash(const int32_t &k) { return rx_hash_skeeto_3s(k); }
    };

    //-----------------------------------------------------
    //哈希map容器基类
    //-----------------------------------------------------
    template<class key_t, class val_t, class cmp_t = hashmap_cmp_t, uint32_t MAX_LEVEL = 32, class rnd_t = skiplist_rnd_t>
    class hashmap_base_t
    {
    public:
        typedef skiplist_t<key_t, val_t, MAX_LEVEL, rnd_t>      list_t;

        typedef struct slot_t
        {
            list_t list;
        }slot_t;

    protected:
        //-------------------------------------------------
        slot_t             *m_slots;
        uint32_t            m_slot_size;
        uint32_t            m_node_size;

        //-------------------------------------------------
        //尝试找到pos节点后的下一个被使用的slot(跳过中间未被使用的部分)
        //返回值:下一个slot的位置;与容器的容量相同时代表结束
        uint32_t next_slot(uint32_t pos) const
        {
            while (++pos < m_slot_size)
                if (m_slots[pos].list.size())
                    return pos;
            return m_slot_size;
        }
        //-------------------------------------------------
        //根据key计算所属槽位序号
        uint32_t key_slot(const key_t &key) const
        {
            return cmp_t::hash(key) % m_slot_size;
        }
        //-------------------------------------------------
        hashmap_base_t(slot_t *slots, uint32_t slot_size) :m_slots(slots), m_slot_size(slot_size), m_node_size(0) {}
        virtual ~hashmap_base_t() {}
    public:
        //-------------------------------------------------
        uint32_t size() { return m_node_size; }
        //-------------------------------------------------
        //定义简单的只读迭代器
        class iterator
        {
            const hashmap_base_t           *m_parent;
            typename list_t::iterator   m_itr;
            friend class hashmap_base_t;
        public:
            //---------------------------------------------
            iterator(const hashmap_base_t &s) :m_parent(&s), m_itr(NULL) {}
            iterator(const hashmap_base_t &s, typename list_t::iterator &it) :m_parent(&s), m_itr(it) {}
            iterator(const iterator &i) :m_parent(i.m_parent), m_itr(i.m_itr) {}
            //---------------------------------------------
            bool operator==(const iterator &i)const { return m_parent == i.m_parent && m_itr == i.m_itr; }
            bool operator!=(const iterator &i)const { return !(operator==(i)); }
            //---------------------------------------------
            iterator& operator=(const iterator &i) { m_parent = i.m_parent; m_itr = i.m_itr; return *this; }
            //---------------------------------------------
            //*提领运算符重载,用于获取当前节点的val值
            const val_t& operator*() const { return *m_itr; }
            //---------------------------------------------
            //()运算符重载,用于获取当前节点的key值
            const key_t& operator()() const { return m_itr(); }
            //---------------------------------------------
            //节点指向后移(前置运算符模式,未提供后置模式)
            iterator& operator++()
            {
                if (m_itr.next()==NULL)
                {//如果当前槽位skiplist的迭代器即将结束,则尝试转移到下一个有效的槽位上
                    uint32_t curr = m_parent->key_slot(m_itr()); //根据当前节点key计算所属的当前槽位序号
                    uint32_t next = m_parent->next_slot(curr);   //从当前槽位序号开始,尝试找到下一个有效的槽位
                    if (next == m_parent->m_slot_size)
                        m_itr = NULL;                       //没有有效槽位了,迭代器结束
                    else
                        m_itr = m_parent->m_slots[next].list.begin(); //找到后续的有效槽位了,则记录其迭代器的开始位置
                }
                else
                {//当前槽位skiplist迭代尚未结束
                    ++m_itr;                                //当前槽位skiplist的迭代器后移
                }
                return reinterpret_cast<iterator&>(*this);
            }
        };
        //-------------------------------------------------
        //准备遍历哈希表,返回遍历的初始位置
        iterator begin() const
        {
            if (m_node_size==0)
                return end();                               //容器是空的

            uint32_t slot = next_slot(-1);
            rx_assert(slot<m_slot_size);
            typename list_t::iterator I=m_slots[slot].list.begin();
            return iterator(*this, I);  //返回初始槽位上的开始点
        }
        //-------------------------------------------------
        //返回遍历的结束位置
        iterator end() const { return iterator(*this); }
        //-------------------------------------------------
        //插入元素并进行赋值构造
        template<class KT>
        iterator insert(const KT &key, const val_t &val,bool *dup=NULL)
        {
            uint32_t s = key_slot(key);
            bool d = false;
            typename list_t::iterator I = m_slots[s].list.insert(key, val, &d);
            if (dup) *dup = d;
            if (I!=NULL && !d) ++m_node_size;
            return iterator(*this, I);
        }
        template<class KT>
        iterator insert(const KT &key, bool *dup = NULL)
        {
            uint32_t s = key_slot(key);
            bool d = false;
            typename list_t::iterator I = m_slots[s].list.insert(key, &d);
            if (dup) *dup = d;
            if (I != NULL && !d) ++m_node_size;
            return iterator(*this, I);
        }
        //-------------------------------------------------
        //查找元素,通过返回迭代器是否与end()相同判断是否存在
        template<class KT>
        iterator find(const KT &key) const
        {
            if (0 == m_node_size)
                return end();
            uint32_t s = key_slot(key);
            typename list_t::iterator I=m_slots[s].list.find(key);
            return iterator(*this, I);
        }
        template<class KT>
        iterator operator[](const KT &key) const { return find(key); }
        //-------------------------------------------------
        //删除指定的key对应的节点
        template<class KT>
        bool erase(const KT &key)
        {
            if (0 == m_node_size)
                return false;

            uint32_t s = key_slot(key);
            if (m_slots[s].list.earse(key))
            {
                --m_node_size;
                return true;
            }
            return false;
        }
        //删除指定迭代器对应的节点
        //返回值:是否删除了当前值(删除成功时,迭代器i后移)
        bool erase(iterator &i)
        {
            if (0 == m_node_size)
                return false;

            rx_assert(&i.m_parent==this);

            uint32_t s = key_slot(i());                 //根据i指向节点的key计算所属槽位
            if (!m_slots[s].list.earse(i.m_itr))        //删除i.m_itr指向的节点,并将i.m_itr后移
                return false;

            --m_node_size;

            if (i.m_itr == NULL)
            {//如果删除的是当槽位上的最后节点,则需要尝试将i移动到下一个槽位
                uint32_t next = next_slot(s);           //从当前槽位序号开始,尝试找到下一个有效的槽位
                if (next == m_slot_size)
                    i.m_itr = NULL;                     //没有有效槽位了,迭代器结束
                else
                    i.m_itr = m_slots[next].list.begin();//找到后续的有效槽位了,则记录其迭代器的开始位置
            }

            return true;
        }
        //-------------------------------------------------
        //清空全部的元素
        void clear()
        {
            if (m_node_size == 0)
                return;
            for (uint32_t pos = next_slot(-1); pos != m_slot_size; pos = next_slot(pos))
                m_slots[pos].list.clear();
            m_node_size = 0;
        }
    };

    //-----------------------------------------------------
    //key为uint32_t类型的哈希映射(默认val也为uint32_t)
    //-----------------------------------------------------
    template<uint32_t max_slot_count, class val_t = uint32_t, class key_t = uint32_t, class cmp_t = hashmap_cmp_t, uint32_t MAX_LEVEL = 32, class rnd_t = skiplist_rnd_t>
    class hashmap_t :public hashmap_base_t<key_t, val_t, cmp_t, MAX_LEVEL, rnd_t>
    {
        typedef hashmap_base_t<key_t, val_t, cmp_t, MAX_LEVEL, rnd_t> super_t;
        typename super_t::slot_t    m_slots[max_slot_count];
    public:
        ~hashmap_t() { super_t::clear(); }
        hashmap_t() :super_t(m_slots, max_slot_count) {}
        hashmap_t(mem_allotter_i &ma, uint32_t seed = 1) :super_t(m_slots, max_slot_count) { ct::AC(m_slots, max_slot_count, ma, seed); }
    };

    //-----------------------------------------------------
    //key为tiny_string类型的哈希映射(默认val为uint32_t)
    //-----------------------------------------------------
    template<uint32_t max_slot_count, class val_t = uint32_t, uint16_t max_str_size = 12, class CT = char, class cmp_t = hashmap_cmp_t, uint32_t MAX_LEVEL = 32, class rnd_t = skiplist_rnd_t>
    class hashmap_st :public hashmap_base_t<tiny_string_t<CT, max_str_size>, val_t, cmp_t, MAX_LEVEL, rnd_t>
    {
        typedef hashmap_base_t<tiny_string_t<CT, max_str_size>, val_t, cmp_t, MAX_LEVEL, rnd_t> super_t;
        typename super_t::slot_t    m_slots[max_slot_count];
    public:
        ~hashmap_st() { super_t::clear(); }
        hashmap_st() :super_t(m_slots, max_slot_count) {}
        hashmap_st(mem_allotter_i &ma, uint32_t seed = 1) :super_t(m_slots, max_slot_count) { ct::AC(m_slots, max_slot_count, ma, seed); }
    };
}


#endif
