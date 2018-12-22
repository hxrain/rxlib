#ifndef _RX_DTL_HASHMAP_H_
#define _RX_DTL_HASHMAP_H_

#include "rx_cc_macro.h"
#include "rx_dtl_skiplist.h"
#include "rx_hash_data.h"
#include "rx_hash_int.h"
#include "rx_ct_util.h"
#include "rx_str_tiny.h"

/*
    <�䳤��ϣӳ������,�൱�ڶ���������˹�ϣ�ֶ�>
*/

namespace rx
{
    //-----------------------------------------------------
    //��ϣmapʹ�õĹ�ϣ�ۼ�����
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
    //��ϣmap��������
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
        //�����ҵ�pos�ڵ�����һ����ʹ�õ�slot(�����м�δ��ʹ�õĲ���)
        //����ֵ:��һ��slot��λ��;��������������ͬʱ�������
        uint32_t next_slot(uint32_t pos) const
        {
            while (++pos < m_slot_size)
                if (m_slots[pos].list.size())
                    return pos;
            return m_slot_size;
        }
        //-------------------------------------------------
        //����key����������λ���
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
        //����򵥵�ֻ��������
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
            //*�������������,���ڻ�ȡ��ǰ�ڵ��valֵ
            const val_t& operator*() const { return *m_itr; }
            //---------------------------------------------
            //()���������,���ڻ�ȡ��ǰ�ڵ��keyֵ
            const key_t& operator()() const { return m_itr(); }
            //---------------------------------------------
            //�ڵ�ָ�����(ǰ�������ģʽ,δ�ṩ����ģʽ)
            iterator& operator++()
            {
                if (m_itr.next()==NULL)
                {//�����ǰ��λskiplist�ĵ�������������,����ת�Ƶ���һ����Ч�Ĳ�λ��
                    uint32_t curr = m_parent->key_slot(m_itr()); //���ݵ�ǰ�ڵ�key���������ĵ�ǰ��λ���
                    uint32_t next = m_parent->next_slot(curr);   //�ӵ�ǰ��λ��ſ�ʼ,�����ҵ���һ����Ч�Ĳ�λ
                    if (next == m_parent->m_slot_size)
                        m_itr = NULL;                       //û����Ч��λ��,����������
                    else
                        m_itr = m_parent->m_slots[next].list.begin(); //�ҵ���������Ч��λ��,���¼��������Ŀ�ʼλ��
                }
                else
                {//��ǰ��λskiplist������δ����
                    ++m_itr;                                //��ǰ��λskiplist�ĵ���������
                }
                return reinterpret_cast<iterator&>(*this);
            }
        };
        //-------------------------------------------------
        //׼��������ϣ��,���ر����ĳ�ʼλ��
        iterator begin() const
        {
            if (m_node_size==0)
                return end();                               //�����ǿյ�

            uint32_t slot = next_slot(-1);
            rx_assert(slot<m_slot_size);
            typename list_t::iterator I=m_slots[slot].list.begin();
            return iterator(*this, I);  //���س�ʼ��λ�ϵĿ�ʼ��
        }
        //-------------------------------------------------
        //���ر����Ľ���λ��
        iterator end() const { return iterator(*this); }
        //-------------------------------------------------
        //����Ԫ�ز����и�ֵ����
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
        //����Ԫ��,ͨ�����ص������Ƿ���end()��ͬ�ж��Ƿ����
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
        //ɾ��ָ����key��Ӧ�Ľڵ�
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
        //ɾ��ָ����������Ӧ�Ľڵ�
        //����ֵ:�Ƿ�ɾ���˵�ǰֵ(ɾ���ɹ�ʱ,������i����)
        bool erase(iterator &i)
        {
            if (0 == m_node_size)
                return false;

            rx_assert(&i.m_parent==this);

            uint32_t s = key_slot(i());                 //����iָ��ڵ��key����������λ
            if (!m_slots[s].list.earse(i.m_itr))        //ɾ��i.m_itrָ��Ľڵ�,����i.m_itr����
                return false;

            --m_node_size;

            if (i.m_itr == NULL)
            {//���ɾ�����ǵ���λ�ϵ����ڵ�,����Ҫ���Խ�i�ƶ�����һ����λ
                uint32_t next = next_slot(s);           //�ӵ�ǰ��λ��ſ�ʼ,�����ҵ���һ����Ч�Ĳ�λ
                if (next == m_slot_size)
                    i.m_itr = NULL;                     //û����Ч��λ��,����������
                else
                    i.m_itr = m_slots[next].list.begin();//�ҵ���������Ч��λ��,���¼��������Ŀ�ʼλ��
            }

            return true;
        }
        //-------------------------------------------------
        //���ȫ����Ԫ��
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
    //keyΪuint32_t���͵Ĺ�ϣӳ��(Ĭ��valҲΪuint32_t)
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
    //keyΪtiny_string���͵Ĺ�ϣӳ��(Ĭ��valΪuint32_t)
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
