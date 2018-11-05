#ifndef _RX_DTL_HASHSET_TINY_H_
#define _RX_DTL_HASHSET_TINY_H_

#include "rx_cc_macro.h"
#include "rx_dtl_hashtbl_raw.h"
#include "rx_hash_data.h"
#include "rx_hash_int.h"
#include "rx_ct_util.h"
#include "rx_str_tiny.h"

/*
    <������ϣ����>

    //hash���Ƚ���
    class hashset_cmp_t;

    //hash������,�ɿ��ټ���(�ڲ�û�������ռ�)
    template<class val_t,class cmp_t=hashset_cmp_t>
    class hashset_base_t;

    //Ĭ��Ϊuin32_t���͵�hash����
    template<uint32_t max_node_count,class key_t=uint32_t,bool correct=false,class cmp_t=hashset_cmp_t>
    class tiny_hashset_t;

    //�ڵ�Ϊtiny_string���͵�����������
    template<uint32_t max_node_count,uint16_t max_str_size=12,bool correct=false,class CT=char,class cmp_t=hashset_cmp_t>
    class tiny_hashset_st;
*/

namespace rx
{
    //-----------------------------------------------------
    //�򵥹�ϣ��ʹ�õĽڵ�Ƚ���
    class hashset_cmp_t:public hashtbl_fun_t
    {
    public:
        template<class NVT, class KT>
        static bool equ(const NVT &n, const KT &k) { return n == k; }
    };

    //-----------------------------------------------------
    //����ԭʼ��ϣ���װ����������������
    //-----------------------------------------------------
    template<class val_t,class cmp_t=hashset_cmp_t,bool correct=false>
    class hashset_base_t
    {
    protected:
        typedef raw_hashtbl_t<val_t,cmp_t> raw_tbl_t;
        typedef typename raw_tbl_t::node_t node_t;

        raw_tbl_t   m_basetbl;                              //�ײ��ϣ���ܷ�װ

        //-------------------------------------------------
        //�ڼ����в���Ԫ��,ԭʼ����,û�м�¼������ֵ
        template<class VT>
        node_t *insert_raw(const VT &val)
        {
            uint32_t pos;
            uint32_t hash_code = cmp_t::hash(val);
            node_t *node = m_basetbl.push(hash_code, val ,pos);
            if (!node)
                return NULL;
            return node;
        }
        //-------------------------------------------------
        hashset_base_t() {}
        //�����ʱ��󶨽ڵ�ռ�
        hashset_base_t(node_t* nodes, uint32_t max_node_count, raw_hashtbl_stat_t *st)
        {
            st->max_nodes = max_node_count;
            m_basetbl.bind(nodes, st);
        }
        virtual ~hashset_base_t() { clear(); }
    public:
        //-------------------------------------------------
        //���ڵ�����
        uint32_t capacity() const { return m_basetbl.capacity(); }
        //�Ѿ�ʹ�õĽڵ�����
        uint32_t size() const { return m_basetbl.size(); }
        //�ڲ�״̬
        const raw_hashtbl_stat_t& stat() const { return m_basetbl.stat(); }
        //���ݸ���������λ��ֱ�ӷ��ʶ�Ӧ��ֵ
        const val_t& at_value(uint32_t pos)const { return m_basetbl.node(pos)->value; }
        //���ݸ���������λ�û�ȡ�ڵ�
        const node_t* at(uint32_t pos)const { return m_basetbl.node(pos); }
        //-------------------------------------------------
        //����򵥵�ֻ��������
        class iterator
        {
            const hashset_base_t   &m_parent;
            uint32_t                m_pos;
            friend class hashset_base_t;

        public:
            //---------------------------------------------
            iterator(const hashset_base_t &s, uint32_t pos) :m_parent(s), m_pos(pos) {}
            iterator(const iterator &i) :m_parent(i.m_parent), m_pos(i.m_pos) {}
            //---------------------------------------------
            bool operator==(const iterator &i)const { return &m_parent == &i.m_parent&&m_pos == i.m_pos; }
            bool operator!=(const iterator &i)const { return !(operator==(i)); }
            //---------------------------------------------
            iterator& operator=(const iterator &i) { m_parent = i.m_parent; m_pos = i.m_pos; return *this; }
            //---------------------------------------------
            const val_t& operator*() const
            {
                rx_assert(m_pos<m_parent.m_basetbl.capacity() &&
                    m_parent.m_basetbl.node(m_pos)->is_using());
                return m_parent.m_basetbl.node(m_pos)->value;
            }
            //---------------------------------------------
            //�ڵ�ָ�����(ǰ�������ģʽ,δ�ṩ����ģʽ)
            iterator& operator++()
            {
                m_pos = m_parent.m_basetbl.next(m_pos);        //�����ҵ���һ����Ч��λ��
                return reinterpret_cast<iterator&>(*this);
            }
            //��ȡ��ǰ�������������ж�Ӧ��λ������
            uint32_t pos() const { return m_pos; }
        };
        //-------------------------------------------------
        //�ڼ����в���Ԫ�ز���ֵ����
        template<class VT>
        bool insert(const VT &val)
        {
            node_t *node=insert_raw(val);
            if (!node)
                return false;
            ct::OC(&node->value,val);
            return true;
        }
        //-------------------------------------------------
        //����Ԫ��,ͨ�����ص������Ƿ���end()��ͬ�ж��Ƿ����
        template<class VT>
        bool find(const VT &val) const
        {
            uint32_t pos;
            return find(val,pos);
        }
        template<class VT>
        bool operator[](const VT &val) const { return find(val); }
        //-------------------------------------------------
        //����Ԫ��,ͬʱ��֪Ԫ�ص�����λ��
        template<class VT>
        bool find(const VT &val,uint32_t &pos) const
        {
            uint32_t hash_code = cmp_t::hash(val);
            node_t *node = m_basetbl.find(hash_code, val, pos);
            return node != NULL;
        }
        //-------------------------------------------------
        //ɾ��Ԫ�ز�Ĭ������,����ָ���Ƿ����ɾ����Ŀն�У��
        template<class VT>
        bool erase(const VT &val)
        {
            uint32_t pos;
            uint32_t hash_code = cmp_t::hash(val);
            node_t *node = m_basetbl.find(hash_code, val, pos);

            if (!node || !m_basetbl.remove(node, pos))
                return false;

            ct::OD(&node->value);

            if (correct)
                m_basetbl.correct_following(pos);
            return true;
        }
        //-------------------------------------------------
        //׼����������,���ر����ĳ�ʼλ��
        iterator begin() const{return iterator(*this, m_basetbl.next(-1));}
        //-------------------------------------------------
        //���ر����Ľ���λ��
        iterator end() const { return iterator(*this, m_basetbl.capacity()); }
        //-------------------------------------------------
        //ɾ��ָ��λ�õ�Ԫ��
        //����ֵ:�Ƿ�ɾ���˵�ǰֵ
        bool erase(iterator &i)
        {
            rx_assert(i.m_pos<m_basetbl.capacity() && &i.m_parent==this);
            if (i.m_pos>= m_basetbl.capacity() || &i.m_parent!=this)
                return false;

            node_t &node = *m_basetbl.node(i.m_pos);
            ct::OD(&node.value);

            if (m_basetbl.remove(&node, i.m_pos) && correct)
                m_basetbl.correct_following(i.m_pos);

            ++i;
            return true;
        }
        //-------------------------------------------------
        //���ȫ����Ԫ��
        void clear()
        {
            for (iterator i = begin(); i != end();)
            {
                node_t &node = *m_basetbl.node(i.m_pos);
                ct::OD(&node.value);
                m_basetbl.remove(&node, i.m_pos);
                ++i;
            }
        }
    };

    //-----------------------------------------------------
    //Ĭ�Ͻڵ�Ϊuint32_t���͵�����������
    //-----------------------------------------------------
    template<uint32_t max_node_count,class key_t=uint32_t,bool correct=false,class cmp_t=hashset_cmp_t>
    class tiny_hashset_t :public hashset_base_t<key_t, cmp_t ,correct>
    {
        typedef hashset_base_t<key_t, cmp_t ,correct> super_t;
        typename super_t::node_t    m_nodes[max_node_count];
        raw_hashtbl_stat_t          m_stat;
    public:
        tiny_hashset_t():super_t(m_nodes, max_node_count,&m_stat){}
    };

    //-----------------------------------------------------
    //�ڵ�Ϊtiny_string���͵�����������
    //-----------------------------------------------------
    template<uint32_t max_node_count,uint16_t max_str_size=12,bool correct=false,class CT=char,class cmp_t=hashset_cmp_t>
    class tiny_hashset_st :public hashset_base_t<tiny_string_head_t<CT,max_str_size>, cmp_t ,correct>
    {
        typedef hashset_base_t<tiny_string_head_t<CT,max_str_size>, cmp_t ,correct> super_t;
        typename super_t::node_t    m_nodes[max_node_count];
        raw_hashtbl_stat_t          m_stat;
    public:
        tiny_hashset_st():super_t(m_nodes, max_node_count,&m_stat){}
    };
}

#endif
