#ifndef _RX_DTL_SKIPLIST_H_
#define _RX_DTL_SKIPLIST_H_

#include "rx_cc_macro.h"
#include "rx_mem_alloc.h"
#include "rx_mem_alloc_cntr.h"
#include "rx_hash_rand.h"
#include "rx_dtl_skiplist_raw.h"
#include <time.h>
#include "rx_str_tiny.h"

/*����Ԫ��װʵ���˻���ԭʼ�������������������.
    //-----------------------------------------------------
    //��������
    template<class key_t,uint32_t MAX_LEVEL,class rnd_t=skiplist_rnd_t>
    class skipset_t;

    //����ʹ�õ�����������
    typedef skipset_t<uint32_t,32> skipset_uint32_t;
    //����ʹ�õ�const char*��������
    typedef skipset_t<const char*,32> skipset_cstr_t;
    //����ʹ�õ�const wchar_t*��������
    typedef skipset_t<const wchar_t*,32> skipset_wstr_t;

    //-----------------------------------------------------
    //��������
    template<class key_t,class val_t,uint32_t MAX_LEVEL,class rnd_t=skiplist_rnd_t>
    class skiplist_t;

    //����ʹ�õ���������
    typedef skiplist_t<uint32_t,uint32_t,32> skiplist_uint32_t;
    //����ʹ�õ�const char*Ϊkey����������
    typedef skiplist_t<const char*,uint32_t,32> skiplist_cstr_uint32_t;
    //����ʹ�õ�const wchar_t*Ϊkey����������
    typedef skiplist_t<const wchar_t*,uint32_t,32> skiplist_wstr_uint32_t;

*/

namespace rx
{
    //ͳһָ��һ������ʹ�õ����������������
    typedef rand_skiplist_t skiplist_rnd_t;

    //-----------------------------------------------------
    //��������㼶������������ķ�װ
    template<class rnd_t=skiplist_rnd_t,uint32_t MAX_LEVEL=32>
    class skiplist_rnd_level
    {
        rnd_t   m_rnd;
    public:
        void seed(uint32_t s)
        {
            if (s<=1) s=(uint32_t)time(NULL);
            m_rnd.seed(s);
        }
        //-------------------------------------------------
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

    //-----------------------------------------------------
    //��װ��������Ļ�������������
    //-----------------------------------------------------
    template<class key_t,uint32_t MAX_LEVEL=32,class rnd_t=skiplist_rnd_t>
    class skipset_t
    {
        //-------------------------------------------------
        //skiplist���������ڵ�����,Ϊ�˱���gcc��Ƕ��ľ�ȷƫ�ػ�����,����һ������ռλ��dummy_t.
        template<class kt,typename dummy_t>
        struct skipset_node_t
        {
            //---------------------------------------------
            //�ȽϽڵ������key�Ĵ�С
            //����ֵ:n<keyΪ<0;n==keyΪ0;n>keyΪ>0
            template<class KT>
            static int cmp(const skipset_node_t &n,const KT &key){return n.key==key?0:(n.key<key?-1:1);}
            //---------------------------------------------
            //����key��Ҫ����չ�ߴ�
            template<class KT>
            static int ext_size(const KT &k){return 0;}

            //---------------------------------------------
            //���ж����첢��ʼ��
            template<class val_t>
            void OC(uint32_t level,val_t &val,uint32_t es){ct::OC(&key,val);}
            //---------------------------------------------
            typedef kt node_key_t;
            node_key_t  key;
            struct skipset_node_t *next[1];                //����ʵ�ֵĹؼ�:�ֲ�ĺ����ڵ�ָ��,������ڽڵ�����,���ڵ�����չ����
        };

        //����skipset��const char*/const wchar_t*���͵�key����ƫ�ػ�,�ڲ�ͳһ�����ڴ������key���ݼ�¼
        //����char*/wchar_t*��key����,�ӱ���ԭʼkeyָ������
        template<typename dummy_t>
        struct skipset_node_t<const char*,dummy_t >
        {
            //---------------------------------------------
            //�ȽϽڵ������key�Ĵ�С
            //����ֵ:n<keyΪ<0;n==keyΪ0;n>keyΪ>0
            template<class KT>
            static int cmp(const skipset_node_t &n,const KT &key){return st::strcmp(n.key.c_str(),key);}
            //---------------------------------------------
            //����key��Ҫ����չ�ߴ�
            static int ext_size(const char* k){return st::strlen(k)+1;}

            //---------------------------------------------
            //���ж����첢��ʼ��
            template<class val_t>
            void OC(uint32_t level,val_t &val,uint32_t es)
            {
                ct::OC(&key);
                key.bind((char*)&next[level],es,val,es-1);
            }
            //---------------------------------------------
            typedef tiny_string_head_t<char> node_key_t;
            node_key_t  key;
            struct skipset_node_t *next[1];                //����ʵ�ֵĹؼ�:�ֲ�Ľڵ��������,������ڽڵ�����,���ڵ�����չ����
        };

        template<typename dummy_t>
        struct skipset_node_t<const wchar_t*,dummy_t >
        {
            //---------------------------------------------
            //�ȽϽڵ������key�Ĵ�С
            //����ֵ:n<keyΪ<0;n==keyΪ0;n>keyΪ>0
            template<class KT>
            static int cmp(const skipset_node_t &n,const KT &key){return st::strcmp(n.key.c_str(),key);}
            //---------------------------------------------
            //����key��Ҫ����չ�ߴ�
            static int ext_size(const wchar_t* k){return st::strlen(k)+1;}

            //---------------------------------------------
            //���ж����첢��ʼ��
            template<class val_t>
            void OC(uint32_t level,val_t &val,uint32_t es)
            {
                ct::OC(&key);
                key.bind((wchar_t*)&next[level],es,val,es-1);
            }
            //---------------------------------------------
            typedef tiny_string_head_t<wchar_t> node_key_t;
            node_key_t  key;
            struct skipset_node_t *next[1];                //����ʵ�ֵĹؼ�:�ֲ�Ľڵ��������,������ڽڵ�����,���ڵ�����չ����
        };

        //-------------------------------------------------
        typedef skipset_node_t<key_t,void>          sk_node_t;
        typedef raw_skiplist_t<sk_node_t,MAX_LEVEL> sk_list_t;
        skiplist_rnd_level<rnd_t,MAX_LEVEL>         m_rnd_level;
        sk_list_t                                   m_list;
    public:
        typedef sk_node_t node_t;
        //-------------------------------------------------
        skipset_t(mem_allotter_i &ma,uint32_t seed=1):m_list(ma){m_rnd_level.seed(seed);}
        skipset_t(uint32_t seed=1):m_list(global_mem_allotter()){m_rnd_level.seed(seed);}
        virtual ~skipset_t() {clear();}
        //-------------------------------------------------
        //����򵥵�ֻ��������
        class iterator
        {
            const node_t  *m_node;
            friend class skipset_t;
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
            const typename node_t::node_key_t& operator*() const {return m_node->key;}
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
            uint32_t es=node_t::ext_size(val);
            node_t *node=m_list.insert_raw(val,duplication,es,level);
            if (duplication)
                return 0;
            if (!node)
                return -1;
            node->OC(level,val,es);
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

    //�﷨��,����һ������ʹ�õ�����������
    typedef skipset_t<uint32_t,32>          skipset_uint32_t;
    typedef skipset_t<int32_t,32>           skipset_int32_t;
    //�﷨��,����һ������ʹ�õ�const char*��������
    typedef skipset_t<const char*,32>       skipset_cstr_t;
    //�﷨��,����һ������ʹ�õ�const wchar_t*��������
    typedef skipset_t<const wchar_t*,32>    skipset_wstr_t;





    //-----------------------------------------------------
    //��װ�������������
    //-----------------------------------------------------
    template<class key_t,class val_t,uint32_t MAX_LEVEL=32,class rnd_t=skiplist_rnd_t>
    class skiplist_t
    {
        //-------------------------------------------------
        //skiplist���������ڵ�����
        template<class kt,class vt>
        struct skiplist_node_t
        {
            //---------------------------------------------
            //�ȽϽڵ������key�Ĵ�С
            //����ֵ:n<keyΪ<0;n==keyΪ0;n>keyΪ>0
            template<class KT>
            static int cmp(const skiplist_node_t &n,const KT &key){return n.key==key?0:(n.key<key?-1:1);}

            //---------------------------------------------
            //����key��Ҫ����չ�ߴ�
            template<class KT,class VT>
            static uint32_t ext_size(const KT &k,const VT& v,uint32_t &es1,uint32_t &es2){return 0;}

            //---------------------------------------------
            //���ж����첢��ʼ��
            template<class KT,class VT>
            void OC(uint32_t level,KT &k,VT &v,uint32_t es1,uint32_t es2){ct::OC(&key,k);ct::OC(&val,v);}
            //---------------------------------------------
            typedef kt node_key_t;                           //�Խڵ��ڲ�������key��val�����ͽ��ж���
            typedef vt node_val_t;
            node_key_t  key;
            node_val_t  val;
            struct skiplist_node_t *next[1];                    //����ʵ�ֵĹؼ�:�ֲ�ĺ����ڵ�ָ��,������ڽڵ�����,���ڵ�����չ����
        };

        //����skiplist�����������ڵ�����
        //������key���͵�const char*ƫ�ػ�����,���Խ����ڲ��־û�;
        template<class vt>
        struct skiplist_node_t<const wchar_t*,vt>
        {
            //---------------------------------------------
            //�ȽϽڵ������key�Ĵ�С
            //����ֵ:n<keyΪ<0;n==keyΪ0;n>keyΪ>0
            template<class KT>
            static int cmp(const skiplist_node_t &n,const KT &key){return st::strcmp(n.key.c_str(),key);}
            //---------------------------------------------
            //����key��Ҫ����չ�ߴ�
            template<class KT,class VT>
            static uint32_t ext_size(const KT &k,const VT& v,uint32_t &es1,uint32_t &es2){es1=st::strlen(k)+1;return es1;}

            //---------------------------------------------
            //���ж����첢��ʼ��
            template<class KT,class VT>
            void OC(uint32_t level,KT &k,VT &v,uint32_t es1,uint32_t es2)
            {
                ct::OC(&key);
                ct::OC(&val,v);
                key.bind((wchar_t*)&next[level],es1,k,es1-1);
            }
            //---------------------------------------------
            typedef tiny_string_head_t<wchar_t> node_key_t;          //�Խڵ��ڲ�������key��val�����ͽ��ж���
            typedef vt node_val_t;
            node_key_t  key;
            node_val_t  val;
            struct skiplist_node_t *next[1];                    //����ʵ�ֵĹؼ�:�ֲ�ĺ����ڵ�ָ��,������ڽڵ�����,���ڵ�����չ����
        };

        template<class vt>
        struct skiplist_node_t<const char*,vt>
        {
            //---------------------------------------------
            //�ȽϽڵ������key�Ĵ�С
            //����ֵ:n<keyΪ<0;n==keyΪ0;n>keyΪ>0
            template<class KT>
            static int cmp(const skiplist_node_t &n,const KT &key){return st::strcmp(n.key.c_str(),key);}
            //---------------------------------------------
            //����key��Ҫ����չ�ߴ�
            template<class KT,class VT>
            static uint32_t ext_size(const KT &k,const VT& v,uint32_t &es1,uint32_t &es2){es1=st::strlen(k)+1;return es1;}

            //---------------------------------------------
            //���ж����첢��ʼ��
            template<class KT,class VT>
            void OC(uint32_t level,KT &k,VT &v,uint32_t es1,uint32_t es2)
            {
                ct::OC(&key);
                ct::OC(&val,v);
                key.bind((char*)&next[level],es1,k,es1-1);
            }
            //---------------------------------------------
            typedef tiny_string_head_t<char> node_key_t;          //�Խڵ��ڲ�������key��val�����ͽ��ж���
            typedef vt node_val_t;
            node_key_t  key;
            node_val_t  val;
            struct skiplist_node_t *next[1];                    //����ʵ�ֵĹؼ�:�ֲ�ĺ����ڵ�ָ��,������ڽڵ�����,���ڵ�����չ����
        };

        //-------------------------------------------------
        typedef skiplist_node_t<key_t,val_t>        sk_node_t;
        typedef raw_skiplist_t<sk_node_t,MAX_LEVEL> sk_list_t;
        skiplist_rnd_level<rnd_t,MAX_LEVEL>         m_rnd_level;
        sk_list_t                                   m_list;
    public:
        typedef sk_node_t node_t;
        //-------------------------------------------------
        //�����ʱ��󶨽ڵ�ռ�
        skiplist_t(mem_allotter_i &ma,uint32_t seed=1):m_list(ma){m_rnd_level.seed(seed);}
        skiplist_t(uint32_t seed=1):m_list(global_mem_allotter()){m_rnd_level.seed(seed);}
        virtual ~skiplist_t() {clear();}
        //-------------------------------------------------
        //�Ѿ�ʹ�õĽڵ�����
        uint32_t size() const { return m_list.size(); }

        //-------------------------------------------------
        //����򵥵�ֻ��������
        class iterator
        {
            const node_t  *m_node;
            friend class skiplist_t;
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
            const typename node_t::node_val_t& operator* () const {return m_node->val;}
            //()���������,���ڻ�ȡ��ǰ�ڵ��keyֵ
            const typename node_t::node_key_t& operator()() const {return m_node->key;}
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
            uint32_t es1=0,es2=0;
            node_t *node=m_list.insert_raw(key,duplication,node_t::ext_size(key,val,es1,es2),level);
            if (duplication)
                return 0;
            if (!node)
                return -1;
            node->OC(level,key,val,es1,es2);
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

    //�﷨��,����һ������ʹ�õ���������
    typedef skiplist_t<uint32_t,uint32_t,32>        skiplist_uint32_t;
    typedef skiplist_t<int32_t,uint32_t,32>         skiplist_int32_t;
    //�﷨��,����һ������ʹ�õ�const char*Ϊkey����������
    typedef skiplist_t<const char*,uint32_t,32>     skiplist_cstr_uint32_t;
    //�﷨��,����һ������ʹ�õ�const wchar_t*Ϊkey����������
    typedef skiplist_t<const wchar_t*,uint32_t,32>  skiplist_wstr_uint32_t;
}

#endif
