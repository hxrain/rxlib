#ifndef _RX_DTL_QUEUE_H_
#define _RX_DTL_QUEUE_H_

#include "rx_cc_macro.h"
#include "rx_dtl_list_raw.h"
#include "rx_dtl_stack.h"
#include "rx_mem_alloc.h"
#include "rx_mem_alloc_cntr.h"
#include "rx_str_tiny.h"

/*
    //-----------------------------------------------------
    //�򵥵Ķ�������(��������,ͷβ���ٷ���)����
    template<class DT>
    class queue_t;

    //�﷨��,����һ������ʹ�õ���������
    typedef queue_t<uint32_t>        queue_uint32_t;
    typedef queue_t<int32_t>         queue_int32_t;
    //�﷨��,����һ������ʹ�õ�const char*���ݳ��ж���
    typedef queue_t<const char*>     queue_cstr_t;
    //�﷨��,����һ������ʹ�õ�const wchar_t*���ݳ��ж���
    typedef queue_t<const wchar_t*>  queue_wstr_t;
*/

namespace rx
{
    //-----------------------------------------------------
    //�򵥵Ķ�������(��������,ͷβ���ٷ���)����
    //-----------------------------------------------------
    template<class DT>
    class queue_t
    {
    public:
        typedef typename stack_t<DT>::node_t node_t;
    protected:
        //-------------------------------------------------
        typedef raw_queue_t<node_t>         cntr_t;         //����ʹ�õ�ԭʼջ��������
        //-------------------------------------------------
        mem_allotter_i  &m_mem;                             //�ڴ������,����
        cntr_t           m_cntr;                            //�ײ�ջ����
    public:
        //-------------------------------------------------
        queue_t():m_mem(rx_global_mem_allotter()){}
        queue_t(mem_allotter_i& ma):m_mem(ma){}
        virtual ~queue_t(){clear();}
        mem_allotter_i& mem(){return m_mem;}
        //-------------------------------------------------
        //����򵥵�ֻ��������
        class iterator
        {
            const node_t  *m_node;
            friend class queue_t;
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
        //�鿴����;���ر����ĳ�ʼλ��
        iterator begin() const {return iterator(m_cntr.head());}
        //-------------------------------------------------
        //���ر����Ľ���λ��
        iterator end() const { return iterator(m_cntr.origin()); }
        //-------------------------------------------------
        //�鿴��β
        iterator rbegin() const {return iterator(m_cntr.tail());}
        //-------------------------------------------------
        //�ͷ�ȫ���ڵ�
        void clear() {while(pop_front());}
        //-------------------------------------------------
        //�ڲ�Ԫ������
        uint32_t size() const {return m_cntr.size();}
        //-------------------------------------------------
        //�������(��β),�������½ڵ�.
        template<class dt>
        iterator push_back(const dt &data)
        {
            uint32_t es=node_t::ext_size(data);
            node_t *node=(node_t *)m_mem.alloc(es+sizeof(node_t));
            if (!node) return end();
            m_cntr.push_back(node);
            node->OC(data,((uint8_t*)node+sizeof(node_t)),es);
            return iterator(node);
        }
        //-------------------------------------------------
        //������ջ(����),�������½ڵ�.
        template<class dt>
        iterator push_front(const dt &data)
        {
            uint32_t es=node_t::ext_size(data);
            node_t *node=(node_t *)m_mem.alloc(es+sizeof(node_t));
            if (!node) return end();
            m_cntr.push_front(node);
            node->OC(data,((uint8_t*)node+sizeof(node_t)),es);
            return iterator(node);
        }
        //-------------------------------------------------
        //��������Ԫ�ز��ͷ�.
        //����ֵ:��ջʧ��,����ɹ�.
        bool pop_front()
        {
            if (!size()) return false;
            node_t *node=m_cntr.pop_front();
            ct::OD(node);                                   //�ڵ�����
            m_mem.free(node);                               //�ڵ��ڴ��ͷ�
            return true;
        }
    };

    //-----------------------------------------------------
    //�﷨��,����һ������ʹ�õ���������
    typedef queue_t<uint32_t>        queue_uint32_t;
    typedef queue_t<int32_t>         queue_int32_t;
    //�﷨��,����һ������ʹ�õ�const char*���ݳ��ж���
    typedef queue_t<const char*>     queue_cstr_t;
    //�﷨��,����һ������ʹ�õ�const wchar_t*���ݳ��ж���
    typedef queue_t<const wchar_t*>  queue_wstr_t;
    
    //-----------------------------------------------------
    //���ö���������װһ���򵥵Ķ��󻺴��
    template<class T>
    class queue_cache_t
    {
        typedef queue_t<T*> obj_cache_t;
        obj_cache_t     m_cache;
    public:
        //-------------------------------------------------
        queue_cache_t():m_cache(rx_global_mem_allotter()){}
        queue_cache_t(mem_allotter_i& ma):m_cache(ma){}
        ~queue_cache_t(){clear();}
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
            m_cache.push_back(obj);
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