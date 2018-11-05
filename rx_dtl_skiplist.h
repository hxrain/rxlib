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
            typedef kt node_key_t;                         //�Խڵ��ڲ�������key��val�����ͽ��ж���
            typedef vt node_val_t;
            node_key_t  key;
            node_val_t  val;
            struct skiplist_node_t *next[1];               //����ʵ�ֵĹؼ�:�ֲ�ĺ����ڵ�ָ��,������ڽڵ�����,���ڵ�����չ����
        };
        //-------------------------------------------------
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
            static uint32_t ext_size(const KT &k,const VT& v,uint32_t &es1,uint32_t &es2){es1=(st::strlen(k)+1) * sc<wchar_t>::char_size();return es1;}

            //---------------------------------------------
            //���ж����첢��ʼ��
            template<class KT,class VT>
            void OC(uint32_t level,KT &k,VT &v,uint32_t es1,uint32_t es2)
            {
                ct::OC(&key);
                ct::OC(&val,v);
                uint32_t cap = es1 / sc<wchar_t>::char_size();
                key.bind((wchar_t*)&next[level],cap,k,cap-1);
            }
            //---------------------------------------------
            typedef tiny_string_head_t<wchar_t> node_key_t;//�Խڵ��ڲ�������key��val�����ͽ��ж���
            typedef vt node_val_t;
            node_key_t  key;
            node_val_t  val;
            struct skiplist_node_t *next[1];               //����ʵ�ֵĹؼ�:�ֲ�ĺ����ڵ�ָ��,������ڽڵ�����,���ڵ�����չ����
        };
        //-------------------------------------------------
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
            typedef tiny_string_head_t<char> node_key_t;    //�Խڵ��ڲ�������key��val�����ͽ��ж���
            typedef vt node_val_t;
            node_key_t  key;
            node_val_t  val;
            struct skiplist_node_t *next[1];                //����ʵ�ֵĹؼ�:�ֲ�ĺ����ڵ�ָ��,������ڽڵ�����,���ڵ�����չ����
        };

        //-------------------------------------------------
        typedef skiplist_node_t<key_t,val_t>   sk_node_t;   //��������ʹ�õ�ԭʼ����ڵ�����
        typedef raw_skiplist_t<sk_node_t>      sk_list_t;   //��������ʹ�õ�ԭʼ������������
        //-------------------------------------------------
        skiplist_rnd_level<rnd_t,MAX_LEVEL>    m_rnd_level; //�������������
        sk_list_t                              m_raw_list;  //����ʹ�õĵײ�ԭʼ��������
    public:
        typedef sk_node_t node_t;
        //-------------------------------------------------
        //�����ʱ��󶨽ڵ�ռ�
        skiplist_t(mem_allotter_i &ma,uint32_t seed=1):m_rnd_level(seed),m_raw_list(ma){}
        skiplist_t(uint32_t seed=1):m_rnd_level(seed),m_raw_list(rx_global_mem_allotter()){}
        virtual ~skiplist_t() {clear();}
        //-------------------------------------------------
        //�Ѿ�ʹ�õĽڵ�����
        uint32_t size() const { return m_raw_list.size(); }

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
            node_t *node=m_raw_list.insert_raw(key,duplication,node_t::ext_size(key,val,es1,es2),level);
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
        node_t* find(const KT &key) const {return m_raw_list.find(key);}
        //-------------------------------------------------
        //ɾ��Ԫ�ز�Ĭ������
        template<class KT>
        bool erase(const KT &key) {return m_raw_list.earse(key);}
        //-------------------------------------------------
        //׼����������,���ر����ĳ�ʼλ��
        iterator begin() const{return iterator(m_raw_list.head());}
        //-------------------------------------------------
        //���ر����Ľ���λ��
        iterator end() const { return iterator(NULL); }
        //-------------------------------------------------
        //��ȡ�����β�ڵ������(�����ڿ��ٻ�ȡβ�ڵ��ֵ)
        iterator rbegin() const { return iterator(m_raw_list.tail()); }
        //-------------------------------------------------
        //ɾ��ָ��λ�õ�Ԫ��
        //����ֵ:�Ƿ�ɾ���˵�ǰֵ
        bool erase(iterator &i)
        {
            rx_assert(i.m_node!=NULL);
            if (i.m_node==NULL)
                return false;

            bool rc=m_raw_list.earse(i.m_node.key);
            ++i;
            return rc;
        }
        //-------------------------------------------------
        #if RX_RAW_SKIPLIST_DEBUG_PRINT
        void print(){m_raw_list.print();}
        #endif
        //-------------------------------------------------
        //���ȫ����Ԫ��
        void clear(){m_raw_list.clear();}
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
