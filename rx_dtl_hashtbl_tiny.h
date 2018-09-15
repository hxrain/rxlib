#ifndef _RX_DTL_HASHTBL_TINY_H_
#define _RX_DTL_HASHTBL_TINY_H_

#include "rx_cc_macro.h"
#include "rx_dtl_hashtbl_raw.h"
#include "rx_hash_data.h"
#include "rx_hash_int.h"
#include "rx_ct_util.h"

namespace rx
{
    //-----------------------------------------------------
    //����ԭʼ��ϣ���װ����������������
    //-----------------------------------------------------
    template<class val_t,class cmp_t>
    class hashset_base_t
    {
    protected:
        typedef raw_hashtbl_t<val_t,cmp_t> baseset_t;
        typedef typename baseset_t::node_t node_t;

        baseset_t   m_basetbl;                              //�ײ��ϣ���ܷ�װ

        //-------------------------------------------------
        //ɾ��Ԫ��(���ڵ�ɾ�����,û�д���������ֵ)
        node_t* erase_raw(const val_t &val)
        {
            uint32_t hash_code = cmp_t::hash(val);
            uint32_t pos;
            node_t *node = m_basetbl.find(hash_code, val,pos);
            if (!node)
                return NULL;
            m_basetbl.remove(node);
            return node;
        }
        //-------------------------------------------------
        //�ڼ����в���Ԫ��,ԭʼ����,û�м�¼������ֵ
        node_t *insert_raw(const val_t &val)
        {
            uint32_t pos;
            uint32_t hash_code = cmp_t::hash(val);
            node_t *node = m_basetbl.push(hash_code, val ,pos);
            if (!node)
                return NULL;
            return node;
        }
    public:
        //-------------------------------------------------
        //�����ʱ��󶨽ڵ�ռ�
        hashset_base_t(node_t* nodes,uint32_t max_node_count) {m_basetbl.bind(nodes,max_node_count);}
        virtual ~hashset_base_t() {clear();}
        //-------------------------------------------------
        //�ڼ����в���Ԫ�ز���ֵ����
        bool insert(const val_t &val)
        {
            node_t *node=insert_raw(val);
            if (!node)
                return false;
            ct::OC(&node->value,val);
            return true;
        }
        //-------------------------------------------------
        //����Ԫ���Ƿ����
        bool find(const val_t &val) const
        {
            uint32_t hash_code = cmp_t::hash(val);
            uint32_t pos;
            node_t *node = m_basetbl.find(hash_code, val,pos);
            return node!=NULL;
        }
        //-------------------------------------------------
        //ɾ��Ԫ�ز�Ĭ������
        bool erase(const val_t &val)
        {
            node_t *node = erase_raw(val);
            if (!node)
                return false;
            ct::OD(&node->value);
            return true;
        }
        //-------------------------------------------------
        //���ڵ�����
        uint32_t capacity() const { return m_basetbl.capacity(); }
        //�Ѿ�ʹ�õĽڵ�����
        uint32_t size() const { return m_basetbl.size(); }
        //����λ�۳�ͻ����
        uint32_t collision() const { return m_basetbl.collision(); }

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
            iterator(const iterator &i):m_parent(i.m_parent),m_pos(i.m_pos) {}
            //---------------------------------------------
            bool operator==(const iterator &i)const { return &m_parent == &i.m_parent&&m_pos == i.m_pos; }
            bool operator!=(const iterator &i)const { return !(operator==(i));}
            //---------------------------------------------
            iterator& operator=(iterator &i) {m_parent=i.m_parent; m_pos=i.m_pos; return *this;}
            //---------------------------------------------
            const val_t& operator*() const
            {
                rx_assert(m_pos<m_parent.m_basetbl.capacity() &&m_parent.m_basetbl.node(m_pos)->state);
                return m_parent.m_basetbl.node(m_pos)->value;
            }
            //---------------------------------------------
            //�ڵ�ָ�����(ǰ�������ģʽ,δ�ṩ����ģʽ)
            iterator& operator++()
            {
                m_pos=m_parent.m_basetbl.next(m_pos);        //�����ҵ���һ����Ч��λ��
                return reinterpret_cast<iterator&>(*this);
            }
        };

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
            m_basetbl.remove(&node);
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
    template<class key_t,class val_t,class cmp_t>
    class hashtbl_base_t
    {
    protected:
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

        //-------------------------------------------------
        raw_tbl_t   m_basetbl;                              //�ײ��ϣ���ܷ�װ
    public:
        //-------------------------------------------------
        //�����ʱ��󶨽ڵ�ռ�
        hashtbl_base_t(node_t *nodes,uint32_t max_node_count) {m_basetbl.bind(nodes,max_node_count);}
        virtual ~hashtbl_base_t() {clear();}
        //-------------------------------------------------
        //���ڵ�����
        uint32_t capacity() const { return m_basetbl.capacity(); }
        //�Ѿ�ʹ�õĽڵ�����
        uint32_t size() const { return m_basetbl.size(); }
        //����λ�۳�ͻ����
        uint32_t collision() const { return m_basetbl.collision(); }

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
            iterator& operator=(iterator &i) {m_parent=i.m_parent; m_pos=i.m_pos; return *this;}
            //---------------------------------------------
            //*�������������,���ڻ�ȡ��ǰ�ڵ��valֵ
            const val_t& operator*() const
            {
                rx_assert(m_pos<m_parent.m_basetbl.capacity() &&m_parent.m_basetbl.node(m_pos)->state);
                return m_parent.m_basetbl.node(m_pos)->value.val;
            }
            //---------------------------------------------
            //()���������,���ڻ�ȡ��ǰ�ڵ��keyֵ
            const key_t& operator()() const
            {
                rx_assert(m_pos<m_parent.m_basetbl.capacity()&&m_parent.m_basetbl.node(m_pos)->state);
                return m_parent.m_basetbl.node(m_pos)->value.key;
            }
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
        //�ڹ�ϣ���в���Ԫ��
        node_t* insert_raw(const key_t &key)
        {
            uint32_t pos;
            uint32_t hash_code = cmp_t::hash(key);
            node_t *node = m_basetbl.push(hash_code, key,pos);
            if (!node)
                return NULL;
            return node;
        }
        //����Ԫ�ز����и�ֵ����
        bool insert(const key_t &key,const val_t &val)
        {
            node_t *node = insert_raw(key);
            if (!node)
                return false;
            ct::OC(&node->value.key,key);
            ct::OC(&node->value.val,val);
            return true;
        }
        //-------------------------------------------------
        //����Ԫ���Ƿ����
        iterator find(const key_t &key) const
        {
            uint32_t hash_code = cmp_t::hash(key);
            uint32_t pos;
            node_t *node = m_basetbl.find(hash_code, key,pos);
            if (!node)
                return end();
            return iterator(*this,pos);
        }
        //-------------------------------------------------
        //���ɾ��ָ����Ԫ��
        //����ֵ:��ɾ���Ľڵ�
        node_t* erase_raw(const key_t &key)
        {
            uint32_t hash_code = cmp_t::hash(key);
            uint32_t pos;
            node_t *node = m_basetbl.find(hash_code, key,pos);
            if (!node)
                return NULL;
            m_basetbl.remove(node);
            return node;
        }
        //ɾ��ָ����key��Ӧ�Ľڵ�
        bool erase(const key_t &key)
        {
            node_t *node = erase_raw( key);
            if (!node)
                return false;
            ct::OD(&node->value.key);
            ct::OD(&node->value.val);
            return true;
        }

        //-------------------------------------------------
        //���ɾ��ָ��λ�õ�Ԫ��
        //����ֵ:�Ƿ�ɾ���˵�ǰֵ
        node_t* erase_raw(iterator &i)
        {
            rx_assert(i.m_pos<m_basetbl.capacity() && &i.m_parent==this);
            if (i.m_pos>= m_basetbl.capacity() || &i.m_parent!=this)
                return NULL;

            node_t &node = *m_basetbl.node(i.m_pos);
            m_basetbl.remove(&node);
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
    //����ԭʼ��ϣ���װ����������ϣ��������(���Կ��ټ��������ղ���˳����ٱ���)
    //������Ľڵ���β�ڵ�;ɾ���Ľڵ����ǰ��������;
    //-----------------------------------------------------
    template<class key_t,class val_t,class cmp_t>
    class hashlist_base_t
    {
    protected:
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
        static const uint32_t nil_pos=-1;                   //������Ч�ڵ�λ��
        uint32_t    m_head;                                 //��¼ͷ���λ��
        uint32_t    m_last;                                 //��¼���ڵ��λ��
        //-------------------------------------------------
        raw_tbl_t   m_basetbl;                              //�ײ��ϣ���ܷ�װ
    public:
        //-------------------------------------------------
        //�����ʱ��󶨽ڵ�ռ�
        hashlist_base_t(node_t *nodes, uint32_t max_node_count):m_head(nil_pos),m_last(nil_pos) {m_basetbl.bind(nodes,max_node_count);}
        virtual ~hashlist_base_t() {clear();}
        //-------------------------------------------------
        //���ڵ�����
        uint32_t capacity() const { return m_basetbl.capacity(); }
        //�Ѿ�ʹ�õĽڵ�����
        uint32_t size() const { return m_basetbl.size(); }
        //����λ�۳�ͻ����
        uint32_t collision() const { return m_basetbl.collision(); }

        //-------------------------------------------------
        //����򵥵�ֻ��������
        class iterator
        {
            const hashlist_base_t   &m_parent;
            uint32_t                m_pos;
            friend class hashlist_base_t;
        public:
            //---------------------------------------------
            iterator(const hashlist_base_t &s, uint32_t pos) :m_parent(s), m_pos(pos) {}
            iterator(const iterator &i):m_parent(i.m_parent),m_pos(i.m_pos) {}
            //---------------------------------------------
            bool operator==(const iterator &i)const { return &m_parent == &i.m_parent&&m_pos == i.m_pos; }
            bool operator!=(const iterator &i)const { return !(operator==(i));}
            //---------------------------------------------
            iterator& operator=(iterator &i) {m_parent=i.m_parent; m_pos=i.m_pos; return *this;}
            //---------------------------------------------
            //*�������������,���ڻ�ȡ��ǰ�ڵ��valֵ
            const val_t& operator*() const
            {
                rx_assert(m_pos<m_parent.m_basetbl.capacity() &&m_parent.m_basetbl.node(m_pos)->state);
                return m_parent.m_basetbl.node(m_pos)->value.val;
            }
            //---------------------------------------------
            //()���������,���ڻ�ȡ��ǰ�ڵ��keyֵ
            const key_t& operator()() const
            {
                rx_assert(m_pos<m_parent.m_basetbl.capacity() &&m_parent.m_basetbl.node(m_pos)->state);
                return m_parent.m_basetbl.node(m_pos)->value.key;
            }
            //---------------------------------------------
            //�ڵ�ָ�����(ǰ�������ģʽ,δ�ṩ����ģʽ)
            iterator& operator++()
            {
                m_pos=m_parent.m_basetbl.node(m_pos)->value.next_pos;//ָ�������λ��
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
        //�ڹ�ϣ���в���Ԫ��
        node_t* insert_raw(const key_t &key)
        {
            uint32_t pos;
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
        bool insert(const key_t &key,const val_t &val)
        {
            node_t *node = insert_raw(key);
            if (!node)
                return false;
            ct::OC(&node->value.key,key);
            ct::OC(&node->value.val,val);
            return true;
        }
        //-------------------------------------------------
        //����Ԫ���Ƿ����
        iterator find(const key_t &key) const
        {
            uint32_t hash_code = cmp_t::hash(key);
            uint32_t pos;
            node_t *node = m_basetbl.find(hash_code, key,pos);
            if (!node)
                return end();
            return iterator(*this,pos);
        }
        //-------------------------------------------------
        //���ɾ��Ԫ��
        //����ֵ:��ɾ���Ľڵ�
        node_t* erase_raw(const key_t &key)
        {
            uint32_t hash_code = cmp_t::hash(key);
            uint32_t pos;
            node_t *node = m_basetbl.find(hash_code, key,pos);
            if (!node)
                return NULL;
            m_basetbl.remove(node);

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
        bool erase(const key_t &key)
        {
            node_t *node = erase_raw( key);
            if (!node)
                return false;
            ct::OD(&node->value.key);
            ct::OD(&node->value.val);
            return true;
        }

        //-------------------------------------------------
        //���ɾ��ָ��λ�õ�Ԫ��,������λ�ú���
        //����ֵ:��ɾ���Ľڵ�ָ��
        node_t* erase_raw(iterator &i)
        {
            rx_assert(i.m_pos<m_basetbl.capacity() && &i.m_parent==this);
            if (i.m_pos>= m_basetbl.capacity() || &i.m_parent!=this)
                return NULL;

            node_t &node = *m_basetbl.node(i.m_pos);
            m_basetbl.remove(&node);

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
    //����key����/value����/cmp�Ƚ�������װ��,�õ����ձ���ʹ�õĹ�ϣ������
    //-----------------------------------------------------
    //�򵥹�ϣ��ʹ�õĽڵ�Ƚ���
    class hashset_cmp_t
    {
    public:
        template<class NVT, class KT>
        static bool equ(const NVT &n, const KT &k) { return n == k; }

        template<class KT>
        static uint32_t hash(const KT &k) { return rx_hash_murmur(&k, sizeof(k)); }
        static uint32_t hash(const char *k) { return rx_hash_murmur(k,st::strlen(k)); }
        static uint32_t hash(const wchar_t *k) { return rx_hash_murmur(k, st::strlen(k)); }
        static uint32_t hash(const uint32_t &k) { return rx_hash_skeeto_triple(k); }
    };
    //�򵥹�ϣ��ʹ�õĽڵ�Ƚ���
    class hashtbl_cmp_t:public hashset_cmp_t
    {
    public:
        //�Խڵ�ȽϺ������и���,��Ҫ����ʹ�ýڵ��е�key�ֶ��������kֵ���бȽ�
        template<class NVT, class KT>
        static bool equ(const NVT &n, const KT &k) { return n.key == k; }
    };


    //-----------------------------------------------------
    //�ڵ�Ϊuint32_t���͵�����������
    //-----------------------------------------------------
    template<uint32_t max_node_count>
    class tiny_hashset_uint32_t :public hashset_base_t<uint32_t, hashset_cmp_t >
    {
        typedef hashset_base_t<uint32_t, hashset_cmp_t > super_t;
        typename super_t::node_t    m_nodes[max_node_count];
    public:
        tiny_hashset_uint32_t():hashset_base_t(m_nodes, max_node_count){}
    };

    //-----------------------------------------------------
    //keyΪuint32_t���͵���������ϣ��(Ĭ��valҲΪuint32_t)
    //-----------------------------------------------------
    template<uint32_t max_node_count,class val_t=uint32_t>
    class tiny_hashtbl_uint32_t :public hashtbl_base_t<uint32_t,val_t, hashtbl_cmp_t >
    {
        typedef hashtbl_base_t<uint32_t, val_t, hashtbl_cmp_t > super_t;
        typename super_t::node_t    m_nodes[max_node_count];
    public:
        tiny_hashtbl_uint32_t():super_t(m_nodes, max_node_count) {}
    };

    //-----------------------------------------------------
    //keyΪuint32_t���͵���������ϣ����(Ĭ��valҲΪuint32_t)
    //-----------------------------------------------------
    template<uint32_t max_node_count,class val_t=uint32_t>
    class tiny_hashlist_uint32_t :public hashlist_base_t<uint32_t,val_t, hashtbl_cmp_t >
    {
        typedef hashlist_base_t<uint32_t, val_t, hashtbl_cmp_t > super_t;
        typename super_t::node_t    m_nodes[max_node_count];
    public:
        tiny_hashlist_uint32_t():super_t(m_nodes, max_node_count) {}
    };

}

#endif
