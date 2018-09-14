#ifndef _RX_DTL_HASHTBL_TINY_H_
#define _RX_DTL_HASHTBL_TINY_H_

#include "rx_cc_macro.h"
#include "rx_raw_hashtbl.h"
#include "rx_hash_data.h"
#include "rx_hash_int.h"
#include "rx_ct_util.h"

namespace rx
{
    //-----------------------------------------------------
    //�����Ĺ�ϣ�������ܷ�װ
    class tiny_hash_fun
    {
    public:
        template<class KT>
        static uint32_t hash(const KT &k){return rx_hash_murmur(&k,sizeof(k));}

        static uint32_t hash(const uint32_t &k){return rx_hash_skeeto_triple(k);}
    };

    //-----------------------------------------------------
    //�򵥹�ϣ��ʹ�õĽڵ�Ƚ���
    template<class hash_t=tiny_hash_fun>
    class tiny_hashset_cmp:public hash_t
    {
    public:
        template<class NVT,class KT>
        static bool equ(const NVT &n,const KT &k){return n==k;}
    };

    //-----------------------------------------------------
    //�򵥹�ϣ��ʹ�õĽڵ�Ƚ���
    template<class hash_t=tiny_hash_fun>
    class tiny_hashtbl_cmp:public hash_t
    {
    public:
        template<class NVT,class KT>
        static bool equ(const NVT &n,const KT &k){return n.key==k;}
    };

    //-----------------------------------------------------
    //����ԭʼ��ϣ���װ����������������
    //-----------------------------------------------------
    template<class val_t,uint32_t max_set_size,class vkcmp>
    class tiny_hashset_t
    {
        typedef raw_hashtbl_t<val_t,vkcmp> baseset_t;
        typedef typename baseset_t::node_t node_t;

        baseset_t   m_basetbl;                              //�ײ��ϣ���ܷ�װ
        node_t      m_nodes[max_set_size];                  //��ʵ�Ĺ�ϣ��ڵ�����ռ�

        //-------------------------------------------------
        //ɾ��Ԫ��(���ڵ�ɾ�����,û�д���������ֵ)
        node_t* erase_raw(const val_t &val)
        {
            uint32_t hash_code = vkcmp::hash(val);
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
            uint32_t hash_code = vkcmp::hash(val);
            node_t *node = m_basetbl.push(hash_code, val ,pos);
            if (!node)
                return NULL;
            return node;
        }
    public:
        //-------------------------------------------------
        //�����ʱ��󶨽ڵ�ռ�
        tiny_hashset_t() {m_basetbl.bind(m_nodes,max_set_size);}
        virtual ~tiny_hashset_t() {clear();}
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
            uint32_t hash_code = vkcmp::hash(val);
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
            const tiny_hashset_t   &m_parent;
            uint32_t                m_pos;
            friend class tiny_hashset_t;

        public:
            //---------------------------------------------
            iterator(const tiny_hashset_t &s, uint32_t pos) :m_parent(s), m_pos(pos) {}
            iterator(const iterator &i):m_parent(i.m_parent),m_pos(i.m_pos) {}
            //---------------------------------------------
            bool operator==(const iterator &i)const { return &m_parent == &i.m_parent&&m_pos == i.m_pos; }
            bool operator!=(const iterator &i)const { return !(operator==(i));}
            //---------------------------------------------
            iterator& operator=(iterator &i) {m_parent=i.m_parent; m_pos=i.m_pos; return *this;}
            //---------------------------------------------
            const val_t& operator*() const
            {
                rx_assert(m_pos<max_set_size&&m_parent.m_nodes[m_pos].state);
                return m_parent.m_nodes[m_pos].value;
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
        iterator end() const { return iterator(*this, max_set_size); }
        //-------------------------------------------------
        //ɾ��ָ��λ�õ�Ԫ��
        //����ֵ:�Ƿ�ɾ���˵�ǰֵ
        bool erase(iterator &i)
        {
            rx_assert(i.m_pos<max_set_size && &i.m_parent==this);
            if (i.m_pos>=max_set_size || &i.m_parent!=this)
                return false;

            node_t &node = m_nodes[i.m_pos];
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
    template<class key_t,class val_t,uint32_t max_set_size,class vkcmp>
    class tiny_hashtbl_t
    {
        //-------------------------------------------------
        //��ϣ��ڵ���ڲ��ṹ,��������key��value�ĳ���
        typedef struct node_val_t
        {
            key_t   key;
            val_t   val;
        } node_val_t;

        //-------------------------------------------------
        typedef raw_hashtbl_t<node_val_t,vkcmp> raw_tbl_t;
        typedef typename raw_tbl_t::node_t node_t;

        //-------------------------------------------------
        raw_tbl_t   m_basetbl;                              //�ײ��ϣ���ܷ�װ
        node_t      m_nodes[max_set_size];                  //��ʵ�Ĺ�ϣ��ڵ�����ռ�
    public:
        //-------------------------------------------------
        //�����ʱ��󶨽ڵ�ռ�
        tiny_hashtbl_t() {m_basetbl.bind(m_nodes,max_set_size);}
        virtual ~tiny_hashtbl_t() {clear();}
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
            const tiny_hashtbl_t   &m_parent;
            uint32_t                m_pos;
            friend class tiny_hashtbl_t;

        public:
            //---------------------------------------------
            iterator(const tiny_hashtbl_t &s, uint32_t pos) :m_parent(s), m_pos(pos) {}
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
                rx_assert(m_pos<max_set_size&&m_parent.m_nodes[m_pos].state);
                return m_parent.m_nodes[m_pos].value.val;
            }
            //---------------------------------------------
            //()���������,���ڻ�ȡ��ǰ�ڵ��keyֵ
            const key_t& operator()() const
            {
                rx_assert(m_pos<max_set_size&&m_parent.m_nodes[m_pos].state);
                return m_parent.m_nodes[m_pos].value.key;
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
        iterator end() const { return iterator(*this, max_set_size); }
        //-------------------------------------------------
        //�ڹ�ϣ���в���Ԫ��
        node_t* insert_raw(const key_t &key)
        {
            uint32_t pos;
            uint32_t hash_code = vkcmp::hash(key);
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
            uint32_t hash_code = vkcmp::hash(key);
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
            uint32_t hash_code = vkcmp::hash(key);
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
            rx_assert(i.m_pos<max_set_size && &i.m_parent==this);
            if (i.m_pos>=max_set_size || &i.m_parent!=this)
                return NULL;

            node_t &node = m_nodes[i.m_pos];
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
    template<class key_t,class val_t,uint32_t max_set_size,class vkcmp>
    class tiny_hashlist_t
    {
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
        typedef raw_hashtbl_t<node_val_t,vkcmp> raw_tbl_t;
        typedef typename raw_tbl_t::node_t node_t;
        static const uint32_t nil_pos=-1;                   //������Ч�ڵ�λ��
        uint32_t    m_head;                                 //��¼ͷ���λ��
        uint32_t    m_last;                                 //��¼���ڵ��λ��
        //-------------------------------------------------
        raw_tbl_t   m_basetbl;                              //�ײ��ϣ���ܷ�װ
        node_t      m_nodes[max_set_size];                  //��ʵ�Ĺ�ϣ��ڵ�����ռ�
    public:
        //-------------------------------------------------
        //�����ʱ��󶨽ڵ�ռ�
        tiny_hashlist_t():m_head(nil_pos),m_last(nil_pos) {m_basetbl.bind(m_nodes,max_set_size);}
        virtual ~tiny_hashlist_t() {clear();}
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
            const tiny_hashlist_t   &m_parent;
            uint32_t                m_pos;
            friend class tiny_hashlist_t;
        public:
            //---------------------------------------------
            iterator(const tiny_hashlist_t &s, uint32_t pos) :m_parent(s), m_pos(pos) {}
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
                rx_assert(m_pos<max_set_size&&m_parent.m_nodes[m_pos].state);
                return m_parent.m_nodes[m_pos].value.val;
            }
            //---------------------------------------------
            //()���������,���ڻ�ȡ��ǰ�ڵ��keyֵ
            const key_t& operator()() const
            {
                rx_assert(m_pos<max_set_size&&m_parent.m_nodes[m_pos].state);
                return m_parent.m_nodes[m_pos].value.key;
            }
            //---------------------------------------------
            //�ڵ�ָ�����(ǰ�������ģʽ,δ�ṩ����ģʽ)
            iterator& operator++()
            {
                m_pos=m_parent.m_nodes[m_pos].value.next_pos;//ָ�������λ��
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
            uint32_t hash_code = vkcmp::hash(key);
            node_t *node = m_basetbl.push(hash_code, key,pos);
            if (!node)
                return NULL;

            if (m_head==nil_pos)
                m_head=pos;                                 //��¼ͷ���λ��
            if (m_last==nil_pos)
                m_last=m_head;                              //��ʼʱ��¼β�ڵ�λ��

            m_nodes[m_last].value.next_pos=pos;             //ǰһ�ڵ�ĺ���ָ��ǰ�ڵ�

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
            uint32_t hash_code = vkcmp::hash(key);
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
            uint32_t hash_code = vkcmp::hash(key);
            uint32_t pos;
            node_t *node = m_basetbl.find(hash_code, key,pos);
            if (!node)
                return NULL;
            m_basetbl.remove(node);

            if (m_last==pos)
                m_last=node->value.prv_pos;                 //�����ǰ�ڵ�ǡ����β�ڵ�,��β�ڵ�λ�ñ�Ϊ��ǰ��
            if (m_head==pos)
                m_head=node->value.next_pos;                //�����ǰ�ڵ�ǡ����ͷ�ڵ�,��ͷ���λ�ñ�Ϊ�����
            node_t &prv_node=m_nodes[node->value.prv_pos];  //��ȡ��ǰ�ڵ��ǰ���ڵ�
            prv_node.value.next_pos=node->value.next_pos;   //ǰ���ڵ�ĺ���ָ��ǰ�ڵ�ĺ���
            if (node->value.next_pos!=nil_pos)
            {//�����ǰ�ڵ�ĺ�������
                node_t &next_node=m_nodes[node->value.next_pos];//��ȡ��ǰ�ڵ�ĺ����ڵ�
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
            rx_assert(i.m_pos<max_set_size && &i.m_parent==this);
            if (i.m_pos>=max_set_size || &i.m_parent!=this)
                return NULL;

            node_t &node = m_nodes[i.m_pos];
            m_basetbl.remove(&node);

            if (m_last==i.m_pos)
                m_last=node.value.prv_pos;                 //�����ǰ�ڵ�ǡ����β�ڵ�,��β�ڵ�λ�ñ�Ϊ��ǰ��
            if (m_head==i.m_pos)
                m_head=node.value.next_pos;                //�����ǰ�ڵ�ǡ����ͷ�ڵ�,��ͷ���λ�ñ�Ϊ�����

            node_t &prv_node=m_nodes[node.value.prv_pos];  //��ȡ��ǰ�ڵ��ǰ���ڵ�
            prv_node.value.next_pos=node.value.next_pos;   //ǰ���ڵ�ĺ���ָ��ǰ�ڵ�ĺ���
            if (node.value.next_pos!=nil_pos)
            {//�����ǰ�ڵ�ĺ�������
                node_t &next_node=m_nodes[node.value.next_pos];//��ȡ��ǰ�ڵ�ĺ����ڵ�
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
    //����key����/value����/cmp�Ƚ�������װ��,�õ����ձ���ʹ�õĹ�ϣ������.uint32_t��������ʾ�����Ԥ����
    //-----------------------------------------------------
    //uint32_t���͵�����������
    template<uint32_t max_set_size,class hash_t=tiny_hash_fun>
    class tiny_hashset_uint32_t :public tiny_hashset_t<uint32_t, max_set_size, tiny_hashset_cmp<hash_t> > {};

    //uint32_t(key/value)���͵���������ϣ��
    template<uint32_t max_set_size,class val_t=uint32_t,class hash_t=tiny_hash_fun>
    class tiny_hashtbl_uint32_t :public tiny_hashtbl_t<uint32_t,val_t, max_set_size, tiny_hashtbl_cmp<hash_t> > {};

    //uint32_t(key/value)���͵���������ϣ����
    template<uint32_t max_set_size,class val_t=uint32_t,class hash_t=tiny_hash_fun>
    class tiny_hashlist_uint32_t :public tiny_hashlist_t<uint32_t,val_t, max_set_size, tiny_hashtbl_cmp<hash_t> > {};

}

#endif
