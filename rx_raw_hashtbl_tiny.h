#ifndef _RX_RAW_HASHTBL_TINY_H_
#define _RX_RAW_HASHTBL_TINY_H_

#include "rx_cc_macro.h"
#include "rx_assert.h"

namespace rx
{
    //-----------------------------------------------------
    //�򵥵Ķ�����λ��ϣ��,ʹ�ÿ���ַ��ʽ���г�ͻ����,���ڼ򵥵Ĺ�ϣ����������,���������κ��ڴ����.
    //-----------------------------------------------------
    template<class val_t>
    class raw_hashtbl_t
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
        node_t         *m_nodes;                            //�ڵ�����
        uint32_t        m_using;                            //��ǰ�ڵ������
        uint32_t        m_collision;                        //������value��ϣ��ͻ����
        uint32_t        m_max_slot_size;
    public:
        //-------------------------------------------------
        raw_hashtbl_t():m_nodes(NULL) {}
        //�����տ��õĽڵ�ռ�
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
        //��ӽڵ�
        //����ֵ:NULLʧ��;�����ɹ�.
        template<class key_t>
        node_t *push(uint32_t hash_code,const key_t &value)
        {
            for(uint32_t i=0;i<m_max_slot_size;++i)
            {
                uint32_t pos=(hash_code+i)%m_max_slot_size; //����λ��
                node_t &node = m_nodes[pos];                //�õ��ڵ�
                if (!node.state)
                {//�ýڵ���δʹ��,��ô��ֱ��ʹ��
                    node.state=i+1;                         //��¼��ǰ�ڵ��ͻ˳��
                    m_collision+=i;                         //��¼��ͻ����
                    ++m_using;
                    return &node;
                }
                else if (node.value==value)
                    return &node;                           //�ýڵ��Ѿ���ʹ����,��valueҲ���ظ���,��ô��ֱ�Ӹ�����
            }

            return NULL;                                    //ת��һȦû�ط���!
        }

        //-------------------------------------------------
        //�����ڵ�
        //����ֵ:NULLδ�ҵ�;�����ɹ�
        template<class key_t>
        node_t *find(uint32_t hash_code,const key_t &value) const
        {
            for(uint32_t i=0;i<m_max_slot_size;++i)
            {
                uint32_t I=(hash_code+i)%m_max_slot_size;   //����λ��
                node_t &node = m_nodes[I];                  //�õ��ڵ�
                if (!node.state)
                    return NULL;                            //ֱ�Ӿ������յ���,���ü�����

                if (node.value==value)
                    return &node;                           //˳�Ӻ��ҵ���
            }

            return NULL;                                    //ת��һȦû�ҵ�!
        }
        //-------------------------------------------------
        //ɾ���ڵ�
        bool remove(node_t *node)
        {
            if (!node|| node->state==0)
                return false;
            node->state=0;
            --m_using;
            return true;
        }
        //-------------------------------------------------
        //���ڵ�����
        uint32_t capacity() const { return m_max_slot_size; }
        //�Ѿ�ʹ�õĽڵ�����
        uint32_t size() const { return m_using; }
        //����λ�۳�ͻ����
        uint32_t collision() const { return m_collision; }
    };

    //-----------------------------------------------------
    //����ԭʼ��ϣ���װ����������������
    //-----------------------------------------------------
    template<class val_t,uint32_t max_set_size,class hash_t>
    class tiny_set_t
    {
        typedef raw_hashtbl_t<val_t> baseset_t;
        typedef typename baseset_t::node_t node_t;

        hash_t      m_hash_func;                            //ֵ����Ĺ�ϣ��������������
        baseset_t   m_base_sets;                            //�ײ��ϣ���ܷ�װ
        node_t      m_nodes[max_set_size];                  //��ʵ�Ĺ�ϣ��ڵ�����ռ�

        //�����ҵ�pos����һ����Ч��λ��
        uint32_t m_next(uint32_t pos) const
        {
            while (++pos < max_set_size)
                if (m_nodes[pos].state)
                    return pos;
            return max_set_size;
        }
    public:
        //-------------------------------------------------
        //�����ʱ��󶨽ڵ�ռ�
        tiny_set_t(){m_base_sets.bind(m_nodes,max_set_size);}
        virtual ~tiny_set_t(){clear();}
        //-------------------------------------------------
        //���Ⱪ¶hash������װ����,������ʱ��������Ĺ�ϣ����
        hash_t& hash_func() const { return m_hash_func; }
        //-------------------------------------------------
        //�ڼ����в���Ԫ��
        bool insert(const val_t &val)
        {
            uint32_t hash_code = m_hash_func(val);
            node_t *node = m_base_sets.push(hash_code, val);
            if (!node) return false;
            node->value = val;
            return true;
        }
        //-------------------------------------------------
        //����Ԫ���Ƿ����
        bool find(const val_t &val) const
        {
            uint32_t hash_code = m_hash_func(val);
            node_t *node = m_base_sets.find(hash_code, val);
            return node!=NULL;
        }
        //-------------------------------------------------
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
        uint32_t capacity() const { return m_base_sets.capacity(); }
        //�Ѿ�ʹ�õĽڵ�����
        uint32_t size() const { return m_base_sets.size(); }
        //����λ�۳�ͻ����
        uint32_t collision() const { return m_base_sets.collision(); }

        //-------------------------------------------------
        //����򵥵�ֻ��������
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
                m_pos=m_set.m_next(m_pos);                  //�����ҵ���һ����Ч��λ��
                return reinterpret_cast<iterator&>(*this);
            }
        };

        //-------------------------------------------------
        //׼����������,���ر����ĳ�ʼλ��
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
        //���ر����Ľ���λ��
        iterator end() const { return iterator(*this, max_set_size); }
        //-------------------------------------------------
        //ɾ��ָ��λ�õ�Ԫ��
        //����ֵ:�Ƿ�ɾ���˵�ǰֵ
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
        //���ȫ����Ԫ��
        void clear()
        {
            for (iterator i = begin(); i != end();)
                erase(i);
        }
    };


    //-----------------------------------------------------
    //����ԭʼ��ϣ���װ����������ϣ������
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

        hash_t      m_hash_func;                            //ֵ����Ĺ�ϣ��������������
        baseset_t   m_base_sets;                            //�ײ��ϣ���ܷ�װ
        node_t      m_nodes[max_set_size];                  //��ʵ�Ĺ�ϣ��ڵ�����ռ�

        //�����ҵ�pos����һ����Ч��λ��
        uint32_t m_next(uint32_t pos) const
        {
            while (++pos < max_set_size)
                if (m_nodes[pos].state)
                    return pos;
            return max_set_size;
        }
    public:
        //-------------------------------------------------
        //�����ʱ��󶨽ڵ�ռ�
        tiny_hashtbl_t(){m_base_sets.bind(m_nodes,max_set_size);}
        virtual ~tiny_hashtbl_t(){clear();}
        //-------------------------------------------------
        //���Ⱪ¶hash������װ����,������ʱ��������Ĺ�ϣ����
        hash_t& hash_func() const { return m_hash_func; }
        //-------------------------------------------------
        //�ڼ����в���Ԫ��
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
        //����Ԫ���Ƿ����
        bool find(const key_t &key) const
        {
            uint32_t hash_code = m_hash_func(key);
            node_t *node = m_base_sets.find(hash_code, key);
            return node!=NULL;
        }
        //-------------------------------------------------
        //ɾ��Ԫ��
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
        //���ڵ�����
        uint32_t capacity() const { return m_base_sets.capacity(); }
        //�Ѿ�ʹ�õĽڵ�����
        uint32_t size() const { return m_base_sets.size(); }
        //����λ�۳�ͻ����
        uint32_t collision() const { return m_base_sets.collision(); }

        //-------------------------------------------------
        //����򵥵�ֻ��������
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
                m_pos=m_set.m_next(m_pos);                  //�����ҵ���һ����Ч��λ��
                return reinterpret_cast<iterator&>(*this);
            }
        };

        //-------------------------------------------------
        //׼����������,���ر����ĳ�ʼλ��
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
        //���ر����Ľ���λ��
        iterator end() const { return iterator(*this, max_set_size); }
        //-------------------------------------------------
        //ɾ��ָ��λ�õ�Ԫ��
        //����ֵ:�Ƿ�ɾ���˵�ǰֵ
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
        //���ȫ����Ԫ��
        void clear()
        {
            for (iterator i = begin(); i != end();)
                erase(i);
        }
    };
 }
#endif
