#ifndef _RX_DTL_HASHTBL_TINY_H_
#define _RX_DTL_HASHTBL_TINY_H_

#include "rx_cc_macro.h"
#include "rx_dtl_hashtbl_raw.h"
#include "rx_hash_data.h"
#include "rx_hash_int.h"
#include "rx_ct_util.h"
#include "rx_str_tiny.h"

/*
    <������ϣ��>

    //hash��Ƚ���
    class hashtbl_cmp_t;

    //hash�����,�ɿ��ټ�������(�ڲ�û�������ռ�)
    template<class key_t,class val_t,class cmp_t=hashtbl_cmp_t>
    class hashtbl_base_t;

    //Ĭ��keyΪuint32_t���͵���������ϣ��(Ĭ��valҲΪuint32_t)
    template<uint32_t max_node_count,class val_t=uint32_t,class key_t=uint32_t,bool correct=false,class cmp_t=hashset_cmp_t>
    class tiny_hashtbl_t;

    //keyΪtiny_string���͵���������ϣ��(Ĭ��valΪuint32_t)
    template<uint32_t max_node_count,class val_t=uint32_t,uint16_t max_str_size=12,bool correct=false,class CT=char,class cmp_t=hashset_cmp_t>
    class tiny_hashtbl_st;
*/

namespace rx
{
    //-----------------------------------------------------
    //�򵥹�ϣ��ʹ�õĽڵ�Ƚ���
    class hashtbl_cmp_t:public hashtbl_fun_t
    {
    public:
        //�Խڵ�ȽϺ������и���,��Ҫ����ʹ�ýڵ��е�key�ֶ��������kֵ���бȽ�
        template<class NVT, class KT>
        static bool equ(const NVT &n, const KT &k) { return n.key == k; }
    };
    /*hash table �Ƚ�����С����
    class hashtbl_cmp_t
    {
    public:
        //�Խڵ�ȽϺ������и���,��Ҫ����ʹ�ýڵ��е�key�ֶ��������kֵ���бȽ�
        template<class NVT, class KT>
        static bool equ(const NVT &n, const KT &k) { return n.key == k; }
        static uint32_t hash(const int32_t &k) { return rx_hash_skeeto_3s(k); }
    };
    */

    //-----------------------------------------------------
    //����ԭʼ��ϣ���װ����������ϣ������
    //-----------------------------------------------------
    template<class key_t,class val_t,class cmp_t=hashtbl_cmp_t,bool correct=false>
    class hashtbl_base_t
    {
    public:
        //-------------------------------------------------
        //��ϣ��ڵ���ڲ��ṹ,��������key��value�ĳ���
        typedef struct node_val_t
        {
            key_t   key;
            val_t   val;
        } node_val_t;

        //-------------------------------------------------
        typedef raw_hashtbl_t<node_val_t,cmp_t> raw_tbl_t;
        typedef typename raw_tbl_t::node_t node_t;
    protected:
        //-------------------------------------------------
        raw_tbl_t   m_basetbl;                              //�ײ��ϣ���ܷ�װ
    public:
        //-------------------------------------------------
        hashtbl_base_t() {}
        //�����ʱ��󶨽ڵ�ռ�
        hashtbl_base_t(node_t* nodes,uint32_t max_node_count,raw_hashtbl_stat_t *st)
        {
            st->max_nodes = max_node_count;
            m_basetbl.bind(nodes, st);
        }

        virtual ~hashtbl_base_t() {clear();}
        //-------------------------------------------------
        //���ڵ�����
        uint32_t capacity() const { return m_basetbl.capacity(); }
        //�Ѿ�ʹ�õĽڵ�����
        uint32_t size() const { return m_basetbl.size(); }
        //�ڲ�״̬
        const raw_hashtbl_stat_t& stat() const { return m_basetbl.stat(); }
        //���ݸ���������λ��ֱ�ӷ��ʶ�Ӧ��ֵ
        const node_val_t& at_value(uint32_t pos)const { return m_basetbl.node(pos)->value; }
        //���ݸ���������λ�û�ȡ�ڵ�
        const node_t* at(uint32_t pos)const { return m_basetbl.node(pos); }
        //-------------------------------------------------
        //����򵥵�ֻ��������
        class iterator
        {
            const hashtbl_base_t   &m_parent;
            uint32_t                m_pos;
            friend class hashtbl_base_t;

        public:
            //---------------------------------------------
            iterator(const hashtbl_base_t &s, uint32_t pos) :m_parent(s), m_pos(pos) {}
            iterator(const iterator &i):m_parent(i.m_parent),m_pos(i.m_pos) {}
            //---------------------------------------------
            bool operator==(const iterator &i)const { return &m_parent == &i.m_parent&&m_pos == i.m_pos; }
            bool operator!=(const iterator &i)const { return !(operator==(i));}
            //---------------------------------------------
            iterator& operator=(const iterator &i) {m_parent=i.m_parent; m_pos=i.m_pos; return *this;}
            //---------------------------------------------
            //*�������������,���ڻ�ȡ��ǰ�ڵ��valֵ
            const val_t& operator*() const
            {
                rx_assert(m_pos<m_parent.m_basetbl.capacity() &&
                          m_parent.m_basetbl.node(m_pos)->is_using());
                return m_parent.m_basetbl.node(m_pos)->value.val;
            }
            //---------------------------------------------
            //()���������,���ڻ�ȡ��ǰ�ڵ��keyֵ
            const key_t& operator()() const
            {
                rx_assert(m_pos<m_parent.m_basetbl.capacity()&&
                          m_parent.m_basetbl.node(m_pos)->is_using());
                return m_parent.m_basetbl.node(m_pos)->value.key;
            }
            //��ȡ��ǰ�������������ж�Ӧ��λ������
            uint32_t pos() const { return m_pos; }
            //---------------------------------------------
            //�ڵ�ָ�����(ǰ�������ģʽ,δ�ṩ����ģʽ)
            iterator& operator++()
            {
                m_pos=m_parent.m_basetbl.next(m_pos);                  //�����ҵ���һ����Ч��λ��
                return reinterpret_cast<iterator&>(*this);
            }
        };
        //-------------------------------------------------
        //׼��������ϣ��,���ر����ĳ�ʼλ��
        iterator begin() const {return iterator(*this, m_basetbl.next(-1));}
        //-------------------------------------------------
        //���ر����Ľ���λ��
        iterator end() const { return iterator(*this, m_basetbl.capacity()); }
        //-------------------------------------------------
        //�ڹ�ϣ���в���Ԫ��,ԭʼ����,������,����ֵ.
        //����ֵ:����Ľڵ�ָ��,��֪�Ƿ�ɹ�
        template<class KT>
        node_t *insert_raw(const KT &key, uint32_t &pos)
        {
            uint32_t hash_code = cmp_t::hash(key);
            return m_basetbl.push(hash_code, key,pos);
        }
        //����Ԫ�ز����и�ֵ����
        template<class KT>
        iterator insert(const KT &key,const val_t &val)
        {
            uint32_t pos;
            node_t *node=insert_raw(key,pos);
            if (node==NULL)
                return end();
            ct::OC(&node->value.key,key);
            ct::OC(&node->value.val,val);
            return iterator(*this, pos);
        }
        //-------------------------------------------------
        //����Ԫ��,ͨ�����ص������Ƿ���end()��ͬ�ж��Ƿ����
        template<class KT>
        iterator find(const KT &key) const
        {
            uint32_t pos;
            return find(key,pos);
        }
        template<class KT>
        iterator operator[](const KT &key) const { return find(key); }
        //-------------------------------------------------
        //����Ԫ��,ͬʱ��֪Ԫ�صĽڵ�����λ��
        template<class KT>
        iterator find(const KT &key,uint32_t &pos) const
        {
            uint32_t hash_code = cmp_t::hash(key);
            node_t *node = m_basetbl.find(hash_code, key, pos);
            if (!node)
                return end();
            return iterator(*this, pos);
        }
        //-------------------------------------------------
        //���ɾ��ָ����Ԫ��(Ԫ�����ݻ�û�б�����)
        //����ֵ:��ɾ���Ľڵ�
        template<class KT>
        node_t* erase_raw(const KT &key, uint32_t &pos)
        {
            uint32_t hash_code = cmp_t::hash(key);
            node_t *node = m_basetbl.find(hash_code, key,pos);
            if (!node|| !m_basetbl.remove(node, pos))
                return NULL;
            return node;
        }
        //ɾ��ָ����key��Ӧ�Ľڵ�
        template<class KT>
        bool erase(const KT &key)
        {
            uint32_t pos;
            node_t *node = erase_raw(key, pos);
            if (!node)
                return false;

            ct::OD(&node->value.key);
            ct::OD(&node->value.val);

            if (correct)
                m_basetbl.correct_following(pos);
            return true;
        }

        //-------------------------------------------------
        //���ɾ��ָ����Ԫ��(Ԫ�����ݻ�û�б�����)
        //����ֵ:�Ƿ�ɾ���˵�ǰֵ(ɾ���ɹ�ʱ,������i����)
        node_t* erase_raw(iterator &i)
        {
            rx_assert(i.m_pos<m_basetbl.capacity() && &i.m_parent==this);
            if (i.m_pos>= m_basetbl.capacity() || &i.m_parent!=this)
                return NULL;

            node_t &node = *m_basetbl.node(i.m_pos);
            m_basetbl.remove(&node,i.m_pos);
            ++i;
            return &node;
        }
        //ɾ��ָ����������Ӧ�Ľڵ�
        //����ֵ:�Ƿ�ɾ���˵�ǰֵ(ɾ���ɹ�ʱ,������i����)
        bool erase(iterator &i)
        {
            uint32_t pos = i.m_pos;
            node_t *node = erase_raw(i);
            if (!node)
                return false;

            ct::OD(&node->value.key);
            ct::OD(&node->value.val);

            if (correct)
                m_basetbl.correct_following(pos);

            return true;
        }
        //-------------------------------------------------
        //���ȫ����Ԫ��
        void clear()
        {
            if (!m_basetbl.is_valid())
                return;
            for (iterator i = begin(); i != end();)
                erase(i);
        }
    };

    //-----------------------------------------------------
    //keyΪuint32_t���͵���������ϣ��(Ĭ��valҲΪuint32_t)
    //-----------------------------------------------------
    template<uint32_t max_node_count,class val_t=uint32_t,class key_t=uint32_t,bool correct=false,class cmp_t=hashtbl_cmp_t>
    class tiny_hashtbl_t :public hashtbl_base_t<key_t,val_t, cmp_t ,correct>
    {
        typedef hashtbl_base_t<key_t, val_t, cmp_t ,correct> super_t;
        typename super_t::node_t    m_nodes[max_node_count];
        raw_hashtbl_stat_t          m_stat;
    public:
        tiny_hashtbl_t():super_t(m_nodes, max_node_count,&m_stat) {}
    };

    //-----------------------------------------------------
    //keyΪtiny_string���͵���������ϣ��(Ĭ��valΪuint32_t)
    //-----------------------------------------------------
    template<uint32_t max_node_count,class val_t=uint32_t,uint16_t max_str_size=12,bool correct=false,class CT=char,class cmp_t=hashtbl_cmp_t>
    class tiny_hashtbl_st :public hashtbl_base_t<tiny_string_head_t<CT,max_str_size>,val_t, cmp_t ,correct>
    {
        typedef hashtbl_base_t<tiny_string_head_t<CT,max_str_size>, val_t, cmp_t ,correct> super_t;
        typename super_t::node_t    m_nodes[max_node_count];
        raw_hashtbl_stat_t          m_stat;
    public:
        tiny_hashtbl_st():super_t(m_nodes, max_node_count,&m_stat) {}
    };
}

#endif
