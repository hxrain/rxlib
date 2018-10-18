#ifndef _RX_DTL_STACK_H_
#define _RX_DTL_STACK_H_

#include "rx_cc_macro.h"
#include "rx_dtl_list_raw.h"
#include "rx_mem_alloc.h"
#include "rx_mem_alloc_cntr.h"
#include "rx_str_tiny.h"

/*
    //-----------------------------------------------------
    //�򵥵�ջ����(��������,ͷ������)����
    template<class DT>
    class stack_t;

    //�﷨��,����һ������ʹ�õ�����ջ
    typedef stack_t<uint32_t>        stack_uint32_t;
    typedef stack_t<int32_t>         stack_int32_t;
    //�﷨��,����һ������ʹ�õ�const char*���ݳ���ջ
    typedef stack_t<const char*>     stack_cstr_t;
    //�﷨��,����һ������ʹ�õ�const wchar_t*���ݳ���ջ
    typedef stack_t<const wchar_t*>  stack_wstr_t;

*/

namespace rx
{
    //-----------------------------------------------------
    //�򵥵�ջ����(��������,ͷ������)����
    //-----------------------------------------------------
    template<class DT>
    class stack_t
    {
        //-------------------------------------------------
        template<class dt,typename dummy_t>
        struct raw_node_t
        {
            typedef DT node_data_t;
            struct raw_node_t* volatile next;	            //�ڵ�ĺ���
            node_data_t  data;
            //---------------------------------------------
            //����data��Ҫ����չ�ߴ�
            template<class KT> static int ext_size(const KT &data){return 0;}
            //---------------------------------------------
            //���ж����첢��ʼ��
            template<class val_t> void OC(val_t &val,void* ext_buff,uint32_t ext_size){ct::OC(&data,val);}
        };

        //-------------------------------------------------
        template<typename dummy_t>
        struct raw_node_t<const char*,dummy_t>
        {
            typedef tiny_string_head_ct node_data_t;
            struct raw_node_t* volatile next;	            //�ڵ�ĺ���
            node_data_t  data;
            //---------------------------------------------
            //����data��Ҫ����չ�ߴ�
            template<class KT> static int ext_size(const KT &data){return st::strlen(data)+1;}
            //---------------------------------------------
            //���ж����첢��ʼ��
            template<class val_t> void OC(val_t &val,void* ext_buff,uint32_t ext_size)
            {
                char *buf=(char*)ext_buff;
                ct::OC(&data,ext_size,buf);
                data.set(val,ext_size-1);
            }
        };

        //-------------------------------------------------
        template<typename dummy_t>
        struct raw_node_t<const wchar_t*,dummy_t>
        {
            typedef tiny_string_head_wt node_data_t;
            struct raw_node_t* volatile next;	            //�ڵ�ĺ���
            node_data_t  data;
            //---------------------------------------------
            //����data��Ҫ����չ�ߴ�(�Ǵ�����+1��2��)
            template<class KT> static int ext_size(const KT &data){return (st::strlen(data)+1) * sc<wchar_t>::char_size();}
            //---------------------------------------------
            //���ж����첢��ʼ��
            template<class val_t> void OC(val_t &val,void* ext_buff,uint32_t ext_size)
            {
                wchar_t *buf=(wchar_t*)ext_buff;
                uint32_t cap=ext_size / sc<wchar_t>::char_size();
                ct::OC(&data,cap,buf);
                data.set(val,cap-1);
            }
        };
    public:
        //-------------------------------------------------
        typedef raw_node_t<DT,void>     node_t;             //����ʹ�õ�ԭʼջ�ڵ�����
    protected:
        typedef raw_stack_t<node_t>     cntr_t;             //����ʹ�õ�ԭʼջ��������
        //-------------------------------------------------
        mem_allotter_i  &m_mem;                             //�ڴ������,����
        cntr_t          m_cntr;                             //�ײ�ջ����
    public:
        //-------------------------------------------------
        stack_t():m_mem(rx_global_mem_allotter()){}
        stack_t(mem_allotter_i& ma):m_mem(ma){}
        virtual ~stack_t(){clear();}
        mem_allotter_i& mem(){return m_mem;}
        //-------------------------------------------------
        //����򵥵�ֻ��������
        class iterator
        {
            const node_t  *m_node;
            friend class stack_t;
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
            //* ���������,���ڻ�ȡ��ǰ�ڵ��data
            const typename node_t::node_data_t& operator* () const {return m_node->data;}
            typename node_t::node_data_t* operator-> () const { return &m_node->data; }
            //---------------------------------------------
            iterator& operator++()
            {
                m_node=m_node->next;
                return reinterpret_cast<iterator&>(*this);
            }
        };
        //-------------------------------------------------
        //�鿴ջ��;���ر����ĳ�ʼλ��
        iterator begin() const {return iterator(m_cntr.head());}
        //-------------------------------------------------
        //���ر����Ľ���λ��
        iterator end() const { return iterator(NULL); }
        //-------------------------------------------------
        //�ͷ�ȫ���ڵ�
        void clear() {while(pop_front());}
        //-------------------------------------------------
        //ջ��Ԫ������
        uint32_t size() const {return m_cntr.size();}
        //-------------------------------------------------
        //������ջ,�������½ڵ�.
        template<class dt>
        iterator push_front(const dt &data)
        {
            uint32_t es=node_t::ext_size(data);
            node_t *node=(node_t *)m_mem.alloc(es+sizeof(node_t));
            if (!node) return iterator(NULL);
            m_cntr.push_front(node);
            node->OC(data,((uint8_t*)node+sizeof(node_t)),es);
            return iterator(node);
        }
        //-------------------------------------------------
        //����ջ��Ԫ�ز��ͷ�.
        //����ֵ:��ջʧ��,����ɹ�.
        bool pop_front()
        {
            if (!size()) return false;
            node_t *node=m_cntr.pop_front();
            ct::OD(node);                               //�ڵ�����
            m_mem.free(node);                           //�ڵ��ڴ��ͷ�
            return true;
        }
    };

    //�﷨��,����һ������ʹ�õ�����ջ
    typedef stack_t<uint32_t>        stack_uint32_t;
    typedef stack_t<int32_t>         stack_int32_t;
    //�﷨��,����һ������ʹ�õ�const char*���ݳ���ջ
    typedef stack_t<const char*>     stack_cstr_t;
    //�﷨��,����һ������ʹ�õ�const wchar_t*���ݳ���ջ
    typedef stack_t<const wchar_t*>  stack_wstr_t;

    //-----------------------------------------------------
    //����ջ������װһ���򵥵Ķ��󻺴��
    template<class T>
    class stack_cache_t
    {
        typedef stack_t<T*> obj_cache_t;
        obj_cache_t     m_cache;
    public:
        //-------------------------------------------------
        stack_cache_t():m_cache(rx_global_mem_allotter()){}
        stack_cache_t(mem_allotter_i& ma):m_cache(ma){}
        ~stack_cache_t(){clear();}
        //-------------------------------------------------
        T* get()
        {
            if (m_cache.size())
            {
                T *ret=*m_cache.begin();
                m_cache.pop_front();
                return ret;
            }
            else
                return m_cache.mem().new0();
        }
        //-------------------------------------------------
        void put(T* obj)
        {
            m_cache.push_front(obj);
        }
        //-------------------------------------------------
        void clear()
        {
            if (!m_cache.size()) return;
            do{
                m_cache.mem().del(*m_cache.begin());
            }while(m_cache.pop_front());
        }
    };
}

#endif