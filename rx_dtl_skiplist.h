#ifndef _RX_DTL_SKIPLIST_H_
#define _RX_DTL_SKIPLIST_H_

#include "rx_cc_macro.h"
#include "rx_mem_alloc.h"
#include "rx_hash_rand.h"
#include "rx_raw_skiplist.h"

namespace rx
{
    //-----------------------------------------------------
    //����skiplist�ļ��������ڵ�����
    template<class key_t>
    struct tiny_skipset_node_t
    {
        //-------------------------------------------------
        //�ȽϽڵ������key�Ĵ�С
        //����ֵ:n<keyΪ<0;n==keyΪ0;n>keyΪ>0
        template<class KT>
        static int cmp(const tiny_skipset_node_t &n,const KT &key){return n.key-key;}
        //-------------------------------------------------
        //����key��Ҫ����չ�ߴ�
        template<class KT>
        static int ext_size(const KT &k){return 0;}

        //-------------------------------------------------
        //���ж����첢��ʼ��
        template<class val_t>
        void OC(uint32_t level,val_t &val){ct::OC(&key,val);}
        //-------------------------------------------------
        key_t  key;
        struct tiny_skipset_node_t *next[1];                //����ʵ�ֵĹؼ�:�ֲ�ĺ����ڵ�ָ��,������ڽڵ�����,���ڵ�����չ����
    };

    //-----------------------------------------------------
    //����skiplist�����������ڵ�����
    template<class key_t,class val_t>
    struct tiny_skiplist_node_t
    {
        //-------------------------------------------------
        //�ȽϽڵ������key�Ĵ�С
        //����ֵ:n<keyΪ<0;n==keyΪ0;n>keyΪ>0
        template<class KT>
        static int cmp(const tiny_skiplist_node_t &n,const KT &key){return n.key-key;}
        //-------------------------------------------------
        //����key��Ҫ����չ�ߴ�
        template<class KT,class VT>
        static int ext_size(const KT &k,const VT& v){return 0;}

        //-------------------------------------------------
        //���ж����첢��ʼ��
        template<class KT,class VT>
        void OC(uint32_t level,KT &k,VT &v){ct::OC(&key,k);ct::OC(&val,v);}
        //-------------------------------------------------
        key_t  key;
        val_t  val;
        struct tiny_skiplist_node_t *next[1];               //����ʵ�ֵĹؼ�:�ֲ�ĺ����ڵ�ָ��,������ڽڵ�����,���ڵ�����չ����
    };

    //-----------------------------------------------------
    //��װ��������Ļ�������������
    //-----------------------------------------------------
    template<class key_t,uint32_t MAX_LEVEL=LOG2<256>::result>
    class tiny_skipset_t
    {
        typedef tiny_skipset_node_t<key_t>          node_t;
        typedef raw_skiplist_t<node_t,MAX_LEVEL>    skiplist_t;
        rand_skeeto_triple_t    m_rnd;
        skiplist_t              m_list;
    public:
        //-------------------------------------------------
        tiny_skipset_t(mem_allotter_i &ma):m_list(ma,m_rnd){}
        virtual ~tiny_skipset_t() {clear();}
        //-------------------------------------------------
        //����򵥵�ֻ��������
        class iterator
        {
            node_t  *m_node;
            friend class tiny_skipset_t;
        public:
            //---------------------------------------------
            iterator(const node_t *node) :m_node(node) {}
            iterator(const iterator &i):m_node(i.m_node){}
            //---------------------------------------------
            bool operator==(const iterator &i)const { return m_node == i.m_node; }
            bool operator!=(const iterator &i)const { return !(operator==(i));}
            //---------------------------------------------
            iterator& operator=(iterator &i) {m_node=i.m_node; return *this;}
            //---------------------------------------------
            const key_t& operator*() const {return m_node->key;}
            //---------------------------------------------
            iterator& operator++()
            {
                m_node=m_node->next[0];
                return reinterpret_cast<iterator&>(*this);
            }
        };
        //-------------------------------------------------
        //�ڼ����в���Ԫ�ز���ֵ����
        template<class val_t>
        bool insert(const val_t &val,bool duplication=false)
        {
            uint32_t level=0;
            node_t *node=m_list.insert_raw(val,node_t::ext_size(val),level,duplication);
            if (!node)
                return false;
            node->OC(level,val);
            return true;
        }
        //-------------------------------------------------
        //����Ԫ���Ƿ����
        template<class val_t>
        bool find(const val_t &val) const {return m_list.find(val)!=NULL;}
        //-------------------------------------------------
        //ɾ��Ԫ�ز�Ĭ������
        template<class val_t>
        bool erase(const val_t &val) {return m_list.earse(val);}
        //�Ѿ�ʹ�õĽڵ�����
        uint32_t size() const { return m_list.size(); }
        //-------------------------------------------------
        //׼����������,���ر����ĳ�ʼλ��
        iterator begin() const{return iterator(m_list.head());}
        //-------------------------------------------------
        //���ر����Ľ���λ��
        iterator end() const { return iterator(NULL); }
        //-------------------------------------------------
        //��ȡ���ϵ�β�ڵ������(�����ڿ��ٻ�ȡβ�ڵ��ֵ)
        iterator rend() const { return iterator(m_list.tail()); }
        //-------------------------------------------------
        //ɾ��ָ��λ�õ�Ԫ��
        //����ֵ:�Ƿ�ɾ���˵�ǰֵ
        bool erase(iterator &i)
        {
            rx_assert(i.m_node!=NULL);
            if (i.m_node==NULL)
                return false;

            bool rc=m_list.earse(i.m_node);
            ++i;
            return rc;
        }
        //-------------------------------------------------
        //���ȫ����Ԫ��
        void clear(){m_list.clear();}
    };

    //---------------------------------------------------------
    //��װ�������������
    //---------------------------------------------------------
    template<class key_t,class val_t,uint32_t MAX_LEVEL=LOG2<256>::result>
    class tiny_skiplist_t
    {
        typedef tiny_skiplist_node_t<key_t,val_t>   node_t;
        typedef raw_skiplist_t<node_t,MAX_LEVEL>    skiplist_t;
        rand_skeeto_triple_t    m_rnd;
        skiplist_t              m_list;
    public:
        //-------------------------------------------------
        //�����ʱ��󶨽ڵ�ռ�
        tiny_skiplist_t(mem_allotter_i &ma):m_list(ma,m_rnd){}
        virtual ~tiny_skiplist_t() {clear();}
        //-------------------------------------------------
        //�Ѿ�ʹ�õĽڵ�����
        uint32_t size() const { return m_list.size(); }

        //-------------------------------------------------
        //����򵥵�ֻ��������
        class iterator
        {
            node_t  *m_node;
            friend class tiny_skiplist_t;
        public:
            //---------------------------------------------
            iterator(const node_t *node) :m_node(node) {}
            iterator(const iterator &i):m_node(i.m_node){}
            //---------------------------------------------
            bool operator==(const iterator &i)const { return m_node == i.m_node; }
            bool operator!=(const iterator &i)const { return !(operator==(i));}
            //---------------------------------------------
            iterator& operator=(iterator &i) {m_node=i.m_node; return *this;}
            //---------------------------------------------
            //* ���������,���ڻ�ȡ��ǰ�ڵ��valֵ
            const val_t& operator* () const {return m_node->val;}
            //()���������,���ڻ�ȡ��ǰ�ڵ��keyֵ
            const key_t& operator()() const {return m_node->key;}
            //---------------------------------------------
            iterator& operator++()
            {
                m_node=m_node->next[0];
                return reinterpret_cast<iterator&>(*this);
            }
        };
        //-------------------------------------------------
        //�������в���Ԫ�ز���ֵ����
        template<class KT>
        bool insert(const KT &key,const val_t &val,bool duplication=false)
        {
            uint32_t level=0;
            node_t *node=m_list.insert_raw(key,node_t::ext_size(key,val),level,duplication);
            if (!node)
                return false;
            node->OC(level,key,val);
            return true;
        }
        //-------------------------------------------------
        //����Ԫ���Ƿ����
        template<class KT>
        bool find(const KT &key) const {return m_list.find(key)!=NULL;}
        //-------------------------------------------------
        //ɾ��Ԫ�ز�Ĭ������
        template<class KT>
        bool erase(const KT &key) {return m_list.earse(key);}
        //-------------------------------------------------
        //׼����������,���ر����ĳ�ʼλ��
        iterator begin() const{return iterator(m_list.head());}
        //-------------------------------------------------
        //���ر����Ľ���λ��
        iterator end() const { return iterator(NULL); }
        //-------------------------------------------------
        //��ȡ�����β�ڵ������(�����ڿ��ٻ�ȡβ�ڵ��ֵ)
        iterator rend() const { return iterator(m_list.tail()); }
        //-------------------------------------------------
        //ɾ��ָ��λ�õ�Ԫ��
        //����ֵ:�Ƿ�ɾ���˵�ǰֵ
        bool erase(iterator &i)
        {
            rx_assert(i.m_node!=NULL);
            if (i.m_node==NULL)
                return false;

            bool rc=m_list.earse(i.m_node);
            ++i;
            return rc;
        }
        //-------------------------------------------------
        //���ȫ����Ԫ��
        void clear(){m_list.clear();}
    };

}

#endif
