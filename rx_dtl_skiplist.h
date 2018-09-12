#ifndef _RX_DTL_SKIPLIST_H_
#define _RX_DTL_SKIPLIST_H_

#include "rx_cc_macro.h"
#include "rx_mem_alloc.h"
#include "rx_mem_alloc_cntr.h"
#include "rx_hash_rand.h"
#include "rx_raw_skiplist.h"
#include <time.h>

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
    //��������㼶������������ķ�װ
    template<class rnd_t,uint32_t MAX_LEVEL>
    class tiny_skiplist_rnd_level
    {
        rnd_t   m_rnd;
    public:
        void seed(uint32_t s)
        {
            if (s<=1) s=(uint32_t)time(NULL);
            m_rnd.seed(s);
        }
        //-----------------------------------------------------
        //����һ������Ĳ���:>=1;<=������
        uint32_t make()
        {
            uint32_t rc=1;
            while(rc<MAX_LEVEL&&(m_rnd.get()&0xFFFFFF)<(0xFFFFFF>>2))   //�����������С��25%��������(�൱��4����)
                ++rc;
            rx_assert(rc<=MAX_LEVEL);
            return rc;
        }
    };

    //ͳһָ��һ������ʹ�õ����������������
    typedef rand_skiplist_t tiny_skiplist_rnd_t;


    //-----------------------------------------------------
    //��װ��������Ļ�������������
    //-----------------------------------------------------
    template<class key_t,uint32_t MAX_LEVEL,class rnd_t=tiny_skiplist_rnd_t>
    class tiny_skipset_t
    {
        typedef tiny_skipset_node_t<key_t>          sk_node_t;
        typedef raw_skiplist_t<sk_node_t,MAX_LEVEL> skiplist_t;
        tiny_skiplist_rnd_level<rnd_t,MAX_LEVEL>    m_rnd_level;
        skiplist_t                                  m_list;
    public:
        typedef sk_node_t node_t;
        //-------------------------------------------------
        tiny_skipset_t(mem_allotter_i &ma,uint32_t seed=1):m_list(ma){m_rnd_level.seed(seed);}
        tiny_skipset_t(uint32_t seed=1):m_list(global_mem_allotter()){m_rnd_level.seed(seed);}
        virtual ~tiny_skipset_t() {clear();}
        //-------------------------------------------------
        //����򵥵�ֻ��������
        class iterator
        {
            const node_t  *m_node;
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
        //����ֵ:<0-�ڴ治��;0-ֵ�ظ�;>0-�ɹ�
        template<class val_t>
        int insert(const val_t &val)
        {
            uint32_t level=m_rnd_level.make();
            bool duplication=false;
            node_t *node=m_list.insert_raw(val,duplication,node_t::ext_size(val),level);
            if (duplication)
                return 0;
            if (!node)
                return -1;
            node->OC(level,val);
            return 1;
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
        iterator rbegin() const { return iterator(m_list.tail()); }
        //-------------------------------------------------
        //ɾ��ָ��λ�õ�Ԫ��
        //����ֵ:�Ƿ�ɾ���˵�ǰֵ
        bool erase(iterator &i)
        {
            rx_assert(i.m_node!=NULL);
            if (i.m_node==NULL)
                return false;

            bool rc=m_list.earse(i.m_node->key);
            ++i;
            return rc;
        }
        //-------------------------------------------------
        #if RX_RAW_SKIPLIST_DEBUG_PRINT
        void print(){m_list.print();}
        #endif
        //-------------------------------------------------
        //���ȫ����Ԫ��
        void clear(){m_list.clear();}
    };
    //�﷨��,ֱ�Ӷ���һ������ʹ�õ�С�������������
    typedef tiny_skipset_t<uint32_t,32> tiny_skipset_uint32_t;

    //---------------------------------------------------------
    //��װ�������������
    //---------------------------------------------------------
    template<class key_t,class val_t,uint32_t MAX_LEVEL,class rnd_t=tiny_skiplist_rnd_t>
    class tiny_skiplist_t
    {
        typedef tiny_skiplist_node_t<key_t,val_t>   sk_node_t;
        typedef raw_skiplist_t<sk_node_t,MAX_LEVEL> skiplist_t;
        tiny_skiplist_rnd_level<rnd_t,MAX_LEVEL>    m_rnd_level;
        skiplist_t                                  m_list;
    public:
        typedef sk_node_t node_t;
        //-------------------------------------------------
        //�����ʱ��󶨽ڵ�ռ�
        tiny_skiplist_t(mem_allotter_i &ma,uint32_t seed=1):m_list(ma){m_rnd_level.seed(seed);}
        tiny_skiplist_t(uint32_t seed=1):m_list(global_mem_allotter()){m_rnd_level.seed(seed);}
        virtual ~tiny_skiplist_t() {clear();}
        //-------------------------------------------------
        //�Ѿ�ʹ�õĽڵ�����
        uint32_t size() const { return m_list.size(); }

        //-------------------------------------------------
        //����򵥵�ֻ��������
        class iterator
        {
            const node_t  *m_node;
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
        //����ֵ:<0-�ڴ治��;0-ֵ�ظ�;>0-�ɹ�
        template<class KT>
        int insert(const KT &key,const val_t &val)
        {
            uint32_t level=m_rnd_level.make();
            bool duplication=false;
            node_t *node=m_list.insert_raw(key,duplication,node_t::ext_size(key,val),level);
            if (duplication)
                return 0;
            if (!node)
                return -1;
            node->OC(level,key,val);
            return 1;
        }
        //-------------------------------------------------
        //����Ԫ���Ƿ����
        template<class KT>
        node_t* find(const KT &key) const {return m_list.find(key);}
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
        iterator rbegin() const { return iterator(m_list.tail()); }
        //-------------------------------------------------
        //ɾ��ָ��λ�õ�Ԫ��
        //����ֵ:�Ƿ�ɾ���˵�ǰֵ
        bool erase(iterator &i)
        {
            rx_assert(i.m_node!=NULL);
            if (i.m_node==NULL)
                return false;

            bool rc=m_list.earse(i.m_node.key);
            ++i;
            return rc;
        }
        //-------------------------------------------------
        #if RX_RAW_SKIPLIST_DEBUG_PRINT
        void print(){m_list.print();}
        #endif
        //-------------------------------------------------
        //���ȫ����Ԫ��
        void clear(){m_list.clear();}
    };

    //�﷨��,ֱ�Ӷ���һ������ʹ�õ�С�����������
    typedef tiny_skiplist_t<uint32_t,uint32_t,32> tiny_skiplist_uint32_t;
}

#endif
