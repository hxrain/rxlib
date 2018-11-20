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

    //��������
    template<class key_t,class val_t,uint32_t MAX_LEVEL=RX_SKLIST_MAXLEVEL,class rnd_t=skiplist_rnd_t>
    class skiplist_t;

    //����ʹ�õ���������
    typedef skiplist_t<uint32_t,uint32_t>       skiplist_ut;
    typedef skiplist_t<int32_t,int32_t>         skiplist_it;
    //����ʹ�õ�const char*Ϊkey����������
    typedef skiplist_t<const char*,uint32_t>    skiplist_ct;
    //����ʹ�õ�const wchar_t*Ϊkey����������
    typedef skiplist_t<const wchar_t*,uint32_t> skiplist_wt;
*/

namespace rx
{
    //-----------------------------------------------------
    //��װ�������������
    //-----------------------------------------------------
    template<class key_t,class val_t,uint32_t MAX_LEVEL = RX_SKLIST_MAXLEVEL,class rnd_t=skiplist_rnd_t>
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
            static int cmp(const void *ptr, const KT &key)
            {
                const skiplist_node_t &n = *(skiplist_node_t*)ptr;
                return n.key == key ? 0 : (n.key < key ? -1 : 1);
            }
            //---------------------------------------------
            //����key��val��Ҫ����չ�ߴ�
            template<class KT,class VT>
            static uint32_t ext_size(const KT &k, const VT& v, uint32_t &es1, uint32_t &es2) { return 0; }
            //����key��Ҫ����չ�ߴ�
            template<class KT>
            static uint32_t ext_size(const KT &k, uint32_t &es1) { return 0; }

            //---------------------------------------------
            //���ж����첢��ʼ��
            template<class KT,class VT>
            void OC(uint32_t level,KT &k,VT &v,uint32_t es1,uint32_t es2){ct::OC(&key,k);ct::OC(&val,v);}
            //���ж����첢��ʼ��
            template<class KT>
            void OC(uint32_t level, KT &k, uint32_t es1) { ct::OC(&key, k); ct::OC(&val);}
            //---------------------------------------------
            typedef kt node_key_t;                          //�Խڵ��ڲ�������key��val�����ͽ��ж���
            typedef vt node_val_t;
            node_key_t  key;
            node_val_t  val;
            sk_block_t  levels;                             //����ʵ�ֵĹؼ�:�ֲ�ĺ����ڵ�ָ��,������ڽڵ�����,���ڵ�����չ����
        };
        //-------------------------------------------------
        //����skiplist�����������ڵ�����
        //������key���͵�const char*ƫ�ػ�����,���Խ����ڲ��־û�;
        template<class vt>
        struct skiplist_node_t<const char*, vt>
        {
            //---------------------------------------------
            //�ȽϽڵ������key�Ĵ�С
            //����ֵ:n<keyΪ<0;n==keyΪ0;n>keyΪ>0
            template<class KT>
            static int cmp(const void *ptr, const KT &key)
            {
                const skiplist_node_t &n = *(skiplist_node_t*)ptr;
                return st::strcmp(n.key.c_str(), key);
            }
            //---------------------------------------------
            //����key��Ҫ����չ�ߴ�
            template<class KT, class VT>
            static uint32_t ext_size(const KT &k, const VT& v, uint32_t &es1, uint32_t &es2) { es1 = st::strlen(k) + 1; return es1; }
            template<class KT>
            static uint32_t ext_size(const KT &k, uint32_t &es1) { es1 = st::strlen(k) + 1; return es1; }

            //---------------------------------------------
            //���ж����첢��ʼ��
            template<class KT, class VT>
            void OC(uint32_t level, KT &k, VT &v, uint32_t es1, uint32_t es2)
            {
                ct::OC(&key);
                ct::OC(&val, v);
                key.bind((char*)&levels.next[level], es1, k, es1 - 1);
            }
            template<class KT>
            void OC(uint32_t level, KT &k, uint32_t es1)
            {
                ct::OC(&key);
                ct::OC(&val);
                key.bind((char*)&levels.next[level], es1, k, es1 - 1);
            }
            //---------------------------------------------
            typedef tiny_string_t<char> node_key_t;         //�Խڵ��ڲ�������key��val�����ͽ��ж���
            typedef vt node_val_t;
            node_key_t  key;
            node_val_t  val;
            sk_block_t  levels;                             //����ʵ�ֵĹؼ�:�ֲ�ĺ����ڵ�ָ��,������ڽڵ�����,���ڵ�����չ����
        };
        //-------------------------------------------------
        template<class vt>
        struct skiplist_node_t<const wchar_t*,vt>
        {
            //---------------------------------------------
            //�ȽϽڵ������key�Ĵ�С
            //����ֵ:n<keyΪ<0;n==keyΪ0;n>keyΪ>0
            template<class KT>
            static int cmp(const void *ptr, const KT &key)
            {
                const skiplist_node_t &n = *(skiplist_node_t*)ptr;
                return st::strcmp(n.key.c_str(), key);
            }
            //---------------------------------------------
            //����key��val��Ҫ����չ�ߴ�
            template<class KT,class VT>
            static uint32_t ext_size(const KT &k,const VT& v,uint32_t &es1,uint32_t &es2){es1=(st::strlen(k)+1) * sc<wchar_t>::char_size();return es1;}
            template<class KT>
            static uint32_t ext_size(const KT &k, uint32_t &es1) { es1 = (st::strlen(k) + 1) * sc<wchar_t>::char_size(); return es1; }

            //---------------------------------------------
            //���ж����첢��ʼ��
            template<class KT,class VT>
            void OC(uint32_t level,KT &k,VT &v,uint32_t es1,uint32_t es2)
            {
                ct::OC(&key);
                ct::OC(&val,v);
                uint32_t cap = es1 / sc<wchar_t>::char_size();
                key.bind((wchar_t*)&levels.next[level],cap,k,cap-1);
            }
            template<class KT>
            void OC(uint32_t level, KT &k, uint32_t es1)
            {
                ct::OC(&key);
                ct::OC(&val);
                uint32_t cap = es1 / sc<wchar_t>::char_size();
                key.bind((wchar_t*)&levels.next[level], cap, k, cap - 1);
            }
            //---------------------------------------------
            typedef tiny_string_t<wchar_t> node_key_t;      //�Խڵ��ڲ�������key��val�����ͽ��ж���
            typedef vt node_val_t;
            node_key_t  key;
            node_val_t  val;
            sk_block_t  levels;                             //����ʵ�ֵĹؼ�:�ֲ�ĺ����ڵ�ָ��,������ڽڵ�����,���ڵ�����չ����
        };

        //-------------------------------------------------
        typedef skiplist_node_t<key_t,val_t>   sk_node_t;   //��������ʹ�õ�ԭʼ����ڵ�����
        typedef raw_skiplist_t<sk_node_t>      sk_list_t;   //��������ʹ�õ�ԭʼ������������
        //����ڵ�����β��֮ǰ��Ҫ�����ĳߴ�
        static const uint32_t node_rsv_size = sizeof(sk_node_t::node_key_t) + sizeof(sk_node_t::node_val_t);
        //-------------------------------------------------
        skiplist_rnd_level<rnd_t,MAX_LEVEL>    m_rnd_level; //�������������
        sk_list_t                              m_raw_list;  //����ʹ�õĵײ�ԭʼ��������
    public:
        typedef sk_node_t node_t;
        //-------------------------------------------------
        //�����ʱ��󶨽ڵ�ռ�
        skiplist_t(mem_allotter_i &ma,uint32_t seed=1):m_rnd_level(seed),m_raw_list(ma, node_rsv_size){}
        skiplist_t(uint32_t seed=1):m_rnd_level(seed),m_raw_list(rx_global_mem_allotter(), node_rsv_size){}
        virtual ~skiplist_t() {clear();}
        //-------------------------------------------------
        //����򵥵�ֻ��������
        class iterator
        {
            const node_t  *m_node;
            friend class skiplist_t;
        public:
            //---------------------------------------------
            iterator() :m_node(NULL) {}
            iterator(const node_t *node) :m_node(node) {}
            iterator(const iterator &i):m_node(i.m_node){}
            //---------------------------------------------
            bool operator==(const iterator &i)const { return m_node == i.m_node; }
            bool operator!=(const iterator &i)const { return !(operator==(i));}
            //---------------------------------------------
            iterator& operator=(const iterator &i) {m_node=i.m_node; return *this;}
            //---------------------------------------------
            //* ���������,���ڻ�ȡ��ǰ�ڵ��valֵ
            const typename node_t::node_val_t& operator* () const {return m_node->val;}
            //()���������,���ڻ�ȡ��ǰ�ڵ��keyֵ
            const typename node_t::node_key_t& operator()() const {return m_node->key;}
            //---------------------------------------------
            iterator& operator++()
            {
                m_node = next();
                return reinterpret_cast<iterator&>(*this);
            }
            //---------------------------------------------
            //�жϵ�ǰ�������Ƿ���Ч
            bool is_end() { return m_node == NULL; }
            //---------------------------------------------
            //���Ի�ȡ��ǰ�������ĺ����ڵ�ָ��
            const node_t* next() 
            { 
                uint8_t *ptr = (uint8_t*)m_node->levels.next[0];
                return (node_t*)(ptr ? ptr - node_rsv_size : NULL);
            }
        };
        //-------------------------------------------------
        //�Ѿ�ʹ�õĽڵ�����
        uint32_t size() const { return m_raw_list.size(); }
        //-------------------------------------------------
        //׼����������,���ر����ĳ�ʼλ��
        iterator begin() const { return iterator((node_t*)m_raw_list.head()); }
        //-------------------------------------------------
        //���ر����Ľ���λ��
        iterator end() const { return iterator(NULL); }
        //-------------------------------------------------
        //��ȡ�����β�ڵ������(�����ڿ��ٻ�ȡβ�ڵ��ֵ)
        iterator rbegin() const { return iterator((node_t*)m_raw_list.tail()); }
        //-------------------------------------------------
        //ɾ��ָ��λ�õ�Ԫ��
        //����ֵ:�Ƿ�ɾ���˵�ǰֵ
        bool erase(iterator &i)
        {
            rx_assert(i.m_node != NULL);
            if (i.m_node == NULL)
                return false;
            const node_t::node_key_t &key = i.m_node->key;
            ++i;
            return erase(key);
        }
        //-------------------------------------------------
        //�������в���Ԫ�ز���ֵ����,dup�ɻ�֪�Ƿ��ظ�
        //����ֵ:end()-�ڴ治��;�����ɹ�
        template<class KT>
        iterator insert(const KT &key,const val_t &val, bool *dup = NULL)
        {
            uint32_t level=m_rnd_level.make();
            bool duplication=false;
            uint32_t es1=0,es2=0;
            node_t *node = (node_t*)m_raw_list.insert_raw(key, duplication, node_t::ext_size(key, val, es1, es2), level);
            if (!node)
                return end();

            if (dup) *dup = duplication;

            if (duplication)
                return iterator(node);

            node->OC(level,key,val,es1,es2);
            return iterator(node);
        }
        //-------------------------------------------------
        //�������в���Ԫ��key(�״�valĬ�Ϲ���);dup�ɻ�ȡkey�Ƿ��ظ�;
        //����ֵ:end()-�ڴ治��;�����ɹ�
        template<class KT>
        iterator insert(const KT &key, bool *dup=NULL)
        {
            uint32_t level = m_rnd_level.make();
            bool duplication = false;
            uint32_t es1 = 0;
            node_t *node = m_raw_list.insert_raw(key, duplication, node_t::ext_size(key, es1), level);
            if (!node)
                return end();

            if (dup) *dup = duplication;

            if (duplication)
            {//�ظ���ʱ��,ֱ�ӷ���valָ��
                return iterator(node);
            }

            //������ʱ����нڵ���key/val�ĳ�ʼ��
            node->OC(level, key, es1);
            return iterator(node);
        }
        //-------------------------------------------------
        //����Ԫ���Ƿ����
        template<class KT>
        iterator find(const KT &key) const {return iterator((node_t*)m_raw_list.find(key));}
        //-------------------------------------------------
        //ɾ��Ԫ�ز�Ĭ������
        //ɾ��Ԫ�ز�Ĭ������
        template<class KT>
        bool erase(const KT &key)
        {
            node_t *node = (node_t*)m_raw_list.remove_find(key);
            if (!node) return false;
            ct::OD(node);
            m_raw_list.remove_free(node);
            return true;
        }
        //-------------------------------------------------
        //���ȫ����Ԫ��
        void clear()
        {
            for (iterator I = begin(); I != end(); ++I)
                ct::OD(I.m_node);
            m_raw_list.clear();
        }
    };

    //-----------------------------------------------------
    //�﷨��,����һ������ʹ�õ���������
    typedef skiplist_t<uint32_t,uint32_t>        skiplist_ut;
    typedef skiplist_t<int32_t,int32_t>          skiplist_it;
    //�﷨��,����һ������ʹ�õ�const char*Ϊkey����������
    typedef skiplist_t<const char*,uint32_t>     skiplist_ct;
    //�﷨��,����һ������ʹ�õ�const wchar_t*Ϊkey����������
    typedef skiplist_t<const wchar_t*,uint32_t>  skiplist_wt;
}

#endif
