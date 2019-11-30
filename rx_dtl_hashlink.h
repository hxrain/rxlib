#ifndef _RX_DTL_HASHLINK_TINY_H_
#define _RX_DTL_HASHLINK_TINY_H_

#include "rx_cc_macro.h"
#include "rx_dtl_hashtbl_raw.h"
#include "rx_hash_data.h"
#include "rx_hash_int.h"
#include "rx_ct_util.h"
#include "rx_str_tiny.h"

/*
    <������ϣ��,�ɸ��ݲ���˳����п��ٱ���>

    //hashlink�Ƚ���
    class hashlnk_cmp_t;

    //hash������,�ɿ��ټ��������ҿ��ٱ���(�ڲ�û�������ռ�)
    template<class key_t,class val_t,class cmp_t=hashtbl_cmp_t>
    class hashlink_base_t;

    //Ĭ��keyΪuint32_t���͵���������ϣ����(Ĭ��valҲΪuint32_t)
    template<uint32_t max_node_count,class val_t=uint32_t,class key_t=uint32_t,class cmp_t=hashset_cmp_t>
    class tiny_hashlink_t;

    //keyΪtiny_string���͵���������ϣ����(Ĭ��valΪuint32_t)
    template<uint32_t max_node_count,class val_t=uint32_t,uint16_t max_str_size=12,class CT=char,class cmp_t=hashset_cmp_t>
    class tiny_hashlink_st;
*/

namespace rx
{
    //-----------------------------------------------------
    //�򵥹�ϣ����ʹ�õĽڵ�Ƚ���
    class hashlnk_cmp_t:public hashtbl_fun_t
    {
    public:
        //�Խڵ�ȽϺ������и���,��Ҫ����ʹ�ýڵ��е�key�ֶ��������kֵ���бȽ�
        template<class NVT, class KT>
        static bool equ(const NVT &n, const KT &k) { return n.key == k; }
    };
    /*hash link �Ƚ�����С����
    class hashtlnk_cmp_t
    {
    public:
        //�Խڵ�ȽϺ������и���,��Ҫ����ʹ�ýڵ��е�key�ֶ��������kֵ���бȽ�
        template<class NVT, class KT>
        static bool equ(const NVT &n, const KT &k) { return n.key == k; }
        static uint32_t hash(const int32_t &k) { return rx_hash_skeeto_3s(k); }
    };
    */

    //-----------------------------------------------------
    //����ԭʼ��ϣ���װ����������ϣ��������(���Կ��ټ��������ղ���˳����ٱ���)
    //������Ľڵ���β�ڵ�;ɾ���Ľڵ����ǰ��������;
    //-----------------------------------------------------
    template<class key_t,class val_t,class cmp_t=hashlnk_cmp_t>
    class hashlink_base_t
    {
    public:
        //-------------------------------------------------
        //��ϣ��ڵ���ڲ��ṹ,��������key��value�ĳ���
        typedef struct node_val_t
        {
            key_t   key;
            val_t   val;
            uint32_t prv_pos;
            uint32_t next_pos;
        } node_val_t;

        //-------------------------------------------------
        typedef raw_hashtbl_t<node_val_t,cmp_t> raw_tbl_t;
        typedef typename raw_tbl_t::node_t node_t;

    protected:
        static const uint32_t nil_pos=-1;                   //������Ч�ڵ�λ��
        uint32_t    m_head;                                 //��¼ͷ���λ��
        uint32_t    m_last;                                 //��¼���ڵ��λ��
        //-------------------------------------------------
        raw_tbl_t   m_basetbl;                              //�ײ��ϣ���ܷ�װ
        //-------------------------------------------------
        hashlink_base_t() {}
        //�����ʱ��󶨽ڵ�ռ�
        hashlink_base_t(node_t* nodes,uint32_t max_node_count,raw_hashtbl_stat_t *st):m_head(nil_pos),m_last(nil_pos)
        {
            st->max_nodes = max_node_count;
            m_basetbl.bind(nodes,st);
        }
        virtual ~hashlink_base_t() {clear();}
    public:
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
            const hashlink_base_t   *m_parent;
            uint32_t                m_pos;
            friend class hashlink_base_t;
        public:
            //---------------------------------------------
            iterator(const hashlink_base_t &s, uint32_t pos) :m_parent(&s), m_pos(pos) {}
            iterator(const iterator &i):m_parent(i.m_parent),m_pos(i.m_pos) {}
            //---------------------------------------------
            bool operator==(const iterator &i)const { return m_parent == i.m_parent&&m_pos == i.m_pos; }
            bool operator!=(const iterator &i)const { return !(operator==(i));}
            //---------------------------------------------
            iterator& operator=(const iterator &i) {m_parent=i.m_parent; m_pos=i.m_pos; return *this;}
            //---------------------------------------------
            //*�������������,���ڻ�ȡ��ǰ�ڵ��valֵ
            const val_t& operator*() const
            {
                rx_assert(m_pos<m_parent->m_basetbl.capacity() &&
                          m_parent->m_basetbl.node(m_pos)->is_using());
                return m_parent->m_basetbl.node(m_pos)->value.val;
            }
            //---------------------------------------------
            //()���������,���ڻ�ȡ��ǰ�ڵ��keyֵ
            const key_t& operator()() const
            {
                rx_assert(m_pos<m_parent->m_basetbl.capacity() &&
                          m_parent->m_basetbl.node(m_pos)->is_using());
                return m_parent->m_basetbl.node(m_pos)->value.key;
            }
            //��ȡ��ǰ�������������ж�Ӧ��λ������
            uint32_t pos() const { return m_pos; }
            //---------------------------------------------
            //�ڵ�ָ�����(ǰ�������ģʽ,δ�ṩ����ģʽ)
            iterator& operator++()
            {
                m_pos=m_parent->m_basetbl.node(m_pos)->value.next_pos;//ָ�������λ��
                return reinterpret_cast<iterator&>(*this);
            }
        };
        //-------------------------------------------------
        //׼��������ϣ��,���ر����ĳ�ʼλ��
        iterator begin() const {return iterator(*this, m_head);}
        //��ȡβ�ڵ������
        iterator rbegin() const {return iterator(*this, m_last);}
        //-------------------------------------------------
        //���ر����Ľ���λ��
        iterator end() const { return iterator(*this, nil_pos); }
        //-------------------------------------------------
        //�ڹ�ϣ���в���Ԫ��,ԭʼ����,�����첻��ֵ.
        //����ֵ:�ڵ�ָ��,��֪�Ƿ����ɹ�
        template<class KT>
        node_t *insert_raw(const KT &key, uint32_t &pos)
        {
            uint32_t hash_code = cmp_t::hash(key);
            node_t *node = m_basetbl.push(hash_code, key,pos);
            if (!node)
                return NULL;

            if (m_head==nil_pos)
                m_head=pos;                                 //��¼ͷ���λ��
            if (m_last==nil_pos)
                m_last=m_head;                              //��ʼʱ��¼β�ڵ�λ��

            m_basetbl.node(m_last)->value.next_pos=pos;     //ǰһ�ڵ�ĺ���ָ��ǰ�ڵ�

            node->value.prv_pos=m_last;                     //��ǰ�ڵ��ǰ��ָ��ǰһ�ڵ�
            node->value.next_pos=nil_pos;                   //��ǰ�ڵ�ĺ������Ϊ��
            m_last=pos;                                     //��¼β�ڵ�Ϊ��ǰ�ڵ�

            return node;
        }
        //����Ԫ�ز����и�ֵ����
        template<class KT>
        iterator insert(const KT &key,const val_t &val)
        {
            uint32_t pos;
            node_t *node = insert_raw(key, pos);
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
        //����Ԫ��,ͨ�����ص������Ƿ���end()��ͬ�ж��Ƿ����
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
        //���ɾ��Ԫ��(Ԫ�����ݻ�û�б�����)
        //����ֵ:��ɾ���Ľڵ�
        template<class KT>
        node_t* erase_raw(const KT &key)
        {
            uint32_t hash_code = cmp_t::hash(key);
            uint32_t pos;
            node_t *node = m_basetbl.find(hash_code, key,pos);
            if (!node)
                return NULL;
            m_basetbl.remove(node,pos);

            if (m_last==pos)
                m_last=node->value.prv_pos;                 //�����ǰ�ڵ�ǡ����β�ڵ�,��β�ڵ�λ�ñ�Ϊ��ǰ��
            if (m_head==pos)
                m_head=node->value.next_pos;                //�����ǰ�ڵ�ǡ����ͷ�ڵ�,��ͷ���λ�ñ�Ϊ�����
            node_t &prv_node=*m_basetbl.node(node->value.prv_pos);  //��ȡ��ǰ�ڵ��ǰ���ڵ�
            prv_node.value.next_pos=node->value.next_pos;   //ǰ���ڵ�ĺ���ָ��ǰ�ڵ�ĺ���
            if (node->value.next_pos!=nil_pos)
            {//�����ǰ�ڵ�ĺ�������
                node_t &next_node=*m_basetbl.node(node->value.next_pos);//��ȡ��ǰ�ڵ�ĺ����ڵ�
                next_node.value.prv_pos=node->value.prv_pos;//�����ڵ��ǰ��ָ��ǰ�ڵ��ǰ��
            }

            return node;
        }
        //ɾ��ָ��key��Ӧ�Ľڵ�
        template<class KT>
        bool erase(const KT &key)
        {
            node_t *node = erase_raw( key);
            if (!node)
                return false;
            ct::OD(&node->value.key);
            ct::OD(&node->value.val);
            return true;
        }
        //-------------------------------------------------
        //���ɾ��ָ��λ�õ�Ԫ��,������λ�ú���(Ԫ�����ݻ�û�б�����)
        //����ֵ:��ɾ���Ľڵ�ָ��
        node_t* erase_raw(iterator &i)
        {
            rx_assert(i.m_pos<m_basetbl.capacity() && i.m_parent==this);
            if (i.m_pos>= m_basetbl.capacity() || i.m_parent!=this)
                return NULL;

            node_t &node = *m_basetbl.node(i.m_pos);
            m_basetbl.remove(&node,i.m_pos);

            if (m_last==i.m_pos)
                m_last=node.value.prv_pos;                 //�����ǰ�ڵ�ǡ����β�ڵ�,��β�ڵ�λ�ñ�Ϊ��ǰ��
            if (m_head==i.m_pos)
                m_head=node.value.next_pos;                //�����ǰ�ڵ�ǡ����ͷ�ڵ�,��ͷ���λ�ñ�Ϊ�����

            node_t &prv_node=*m_basetbl.node(node.value.prv_pos);  //��ȡ��ǰ�ڵ��ǰ���ڵ�
            prv_node.value.next_pos=node.value.next_pos;   //ǰ���ڵ�ĺ���ָ��ǰ�ڵ�ĺ���
            if (node.value.next_pos!=nil_pos)
            {//�����ǰ�ڵ�ĺ�������
                node_t &next_node=*m_basetbl.node(node.value.next_pos);//��ȡ��ǰ�ڵ�ĺ����ڵ�
                next_node.value.prv_pos=node.value.prv_pos;//�����ڵ��ǰ��ָ��ǰ�ڵ��ǰ��
            }

            ++i;
            return &node;
        }
        //ɾ��ָ����������Ӧ�Ľڵ�
        bool erase(iterator &i)
        {
            node_t *node = erase_raw(i);
            if (!node)
                return false;
            ct::OD(&node->value.key);
            ct::OD(&node->value.val);
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
    //keyΪuint32_t���͵���������ϣ����(Ĭ��valҲΪuint32_t)
    //-----------------------------------------------------
    template<uint32_t max_node_count,class val_t=uint32_t,class key_t=uint32_t,class cmp_t=hashtbl_cmp_t>
    class tiny_hashlink_t :public hashlink_base_t<key_t,val_t, cmp_t >
    {
        typedef hashlink_base_t<key_t, val_t, cmp_t > super_t;
        typename super_t::node_t    m_nodes[max_node_count];
        raw_hashtbl_stat_t          m_stat;
    public:
        tiny_hashlink_t():super_t(m_nodes, max_node_count,&m_stat),m_stat(max_node_count) {}
    };

    //-----------------------------------------------------
    //keyΪtiny_string���͵���������ϣ����(Ĭ��valΪuint32_t)
    //-----------------------------------------------------
    template<uint32_t max_node_count,class val_t=uint32_t,uint16_t max_str_size=12,class CT=char,class cmp_t=hashtbl_cmp_t>
    class tiny_hashlink_st :public hashlink_base_t<tiny_string_t<CT,max_str_size>,val_t, cmp_t >
    {
        typedef hashlink_base_t<tiny_string_t<CT,max_str_size>, val_t, cmp_t > super_t;
        typename super_t::node_t    m_nodes[max_node_count];
        raw_hashtbl_stat_t          m_stat;
    public:
        tiny_hashlink_st():super_t(m_nodes, max_node_count,&m_stat),m_stat(max_node_count) {}
    };
}

#endif
