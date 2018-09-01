#ifndef _RX_RAW_HASHTBL_TINY_H_
#define _RX_RAW_HASHTBL_TINY_H_

#include "rx_cc_macro.h"
#include "rx_assert.h"

namespace rx
{
    //-----------------------------------------------------
    //�򵥵Ķ�����λ��ϣ��,ʹ�ÿ���ַ��ʽ���г�ͻ����,���ڼ򵥵Ĺ�ϣ����������,���������κ��ڴ����.
    template<class val_t,uint32_t max_slot_size>
    class raw_hashtbl_tiny_t
    {
    public:
        //-------------------------------------------------
        //��ϣ��ڵ����,ʹ�õ�ʱ��,�����������Ľڵ�̳��ڴ�
        typedef struct node_t
        {
            uint32_t    state;                              //��¼��ǰ�ڵ��Ƿ�ʹ��,0δʹ��;1������ʹ��;>1��ϣ��ͻ����(����3�����ڳ�ͻ��������λ��)
            val_t       value;                              //��ϣ�ڵ��valueֵ
        }node_t;
    private:
        node_t          m_nodes[max_slot_size];             //�ڵ�����
        uint32_t        m_using;                            //��ǰ�ڵ������
        uint32_t        m_collision;                        //������value��ϣ��ͻ����
    public:
        raw_hashtbl_tiny_t() { clear(); }
        //-------------------------------------------------
        //��ӽڵ�
        //����ֵ:NULLʧ��;�����ɹ�.
        inline node_t *push(uint32_t hash_code,const val_t &value)
        {
            for(uint32_t i=0;i<max_slot_size;++i)
            {
                uint32_t pos=(hash_code+i)%max_slot_size;  //����λ��
                node_t &node = m_nodes[pos];               //�õ��ڵ�
                if (!node.state)
                {//�ýڵ���δʹ��,��ô��ֱ��ʹ��
                    node.state=i+1;                        //��¼��ǰ�ڵ��ͻ˳��
                    m_collision+=i;                        //��¼��ͻ����
                    ++m_using;
                    return &node;
                }
                else if (node.value==value)
                    return &node;                          //�ýڵ��Ѿ���ʹ����,��valueҲ���ظ���,��ô��ֱ�Ӹ�����
            }

            return NULL;                                   //ת��һȦû�ط���!
        }

        //-------------------------------------------------
        //�����ڵ�
        //����ֵ:NULLδ�ҵ�;�����ɹ�
        inline node_t *find(uint32_t hash_code,const val_t &value) const
        {
            for(uint32_t i=0;i<max_slot_size;++i)
            {
                uint32_t I=(hash_code+i)%max_slot_size;    //����λ��
                node_t &node = m_nodes[I];                 //�õ��ڵ�
                if (!node.state)
                    return NULL;                           //ֱ�Ӿ������յ���,���ü�����

                if (node.value==value)
                    return &node;                          //˳�Ӻ��ҵ���
            }

            return NULL;                                   //ת��һȦû�ҵ�!
        }
        //-------------------------------------------------
        //ɾ���ڵ�
        inline bool remove(node_t *node)
        {
            if (!node|| node->state==0)
                return false;
            node->state=0;
            --m_using;
            return true;
        }
        //-------------------------------------------------
        //�����ϣ����
        inline void clear()
        {
            memset(m_nodes, 0, sizeof(m_nodes));
            m_using = 0;
            m_collision = 0;
        }
        //-------------------------------------------------
        //���ڵ�����
        uint32_t capacity() { return max_slot_size; }
        //�Ѿ�ʹ�õĽڵ�����
        uint32_t size() { return m_using; }
        //����λ�۳�ͻ����
        uint32_t collision() { return m_collision; }
        //���ݽڵ���ŷ��ʽڵ�
        node_t* nodes(uint32_t pos) { return m_nodes[pos]; }
    };

    //-----------------------------------------------------
    //����ԭʼ��ϣ���װһ���������ļ�������
    template<class val_t,uint32_t max_set_size,class hash_t>
    class raw_set_t
    {
        typedef raw_hashtbl_tiny_t<val_t, max_set_size> baseset_t;
        typedef typename baseset_t::node_t node_t;
        hash_t      m_hash_func;
        baseset_t   m_base_sets;
    public:
        //���Ⱪ¶hash������װ����,������ʱ��������Ĺ�ϣ����
        hash_t& hash_func() { return m_hash_func; }
        //�ڼ����в���Ԫ��
        bool insert(const val_t &val)
        {
            uint32_t hash_code = m_hash_func(val);
            node_t *node = m_base_sets.push(hash_code, val);
            if (!node) return false;
            node->value = val;
            return true;
        }
        //����Ԫ���Ƿ����
        bool find(const val_t &val) const
        {
            uint32_t hash_code = m_hash_func(val);
            node_t *node = m_base_sets.find(hash_code, val);
            return node!=NULL;
        }
        //ɾ��Ԫ��
        bool erase(const val_t &val)
        {
            uint32_t hash_code = m_hash_func(val);
            node_t *node = m_base_sets.find(hash_code, val);
            if (!node) return false;
            node->value = 0;
            return m_base_sets.remove(node);
        }
        //-------------------------------------------------
        //���ڵ�����
        uint32_t capacity() { return m_base_sets.capacity(); }
        //�Ѿ�ʹ�õĽڵ�����
        uint32_t size() { return m_base_sets.size(); }
        //����λ�۳�ͻ����
        uint32_t collision() { return m_base_sets.collision(); }

        //-------------------------------------------------
        //����򵥵�ֻ��������
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
                        break;                          //�����ҵ���һ����Ч��λ��
                return *this;
            }
        };

        //-------------------------------------------------
        //׼����������,���ر����ĳ�ʼλ��
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
        //���ر����Ľ���λ��
        iterator& end() const { iterator(*this, max_set_size); }
        //-------------------------------------------------
        //ɾ��ָ��λ�õ�Ԫ��
        iterator& erase(const iterator &i)
        {
            rx_assert(i.m_pos<max_set_size && &i.m_set==this);
            node_t *node = m_base_sets.nodes(i.m_pos);
            node->value = 0;
            m_base_sets.remove(node);
            return ++i;
        }
        //-------------------------------------------------
        //���ȫ����Ԫ��
        void clear()
        {
            for (iterator i = begin(); i != end(); ++i)
                i = erase(i);
        }
    };

 }
#endif
