#ifndef _RX_DTL_LIST_H_
#define _RX_DTL_LIST_H_

#include "rx_cc_macro.h"
#include "rx_dtl_list_raw.h"
#include "rx_mem_alloc.h"
#include "rx_mem_alloc_cntr.h"
#include "rx_str_tiny.h"

/*
    //-----------------------------------------------------
    //�򵥵�list����(˫������,ͷβ���ɿ��ٷ���)
    template<class DT>
    class list_t;

    //�﷨��,����һ������ʹ�õ���������
    typedef list_t<uint32_t>        list_uint32_t;
    typedef list_t<int32_t>         list_int32_t;
    //�﷨��,����һ������ʹ�õ�const char*���ݳ��ж���
    typedef list_t<const char*>     list_cstr_t;
    //�﷨��,����һ������ʹ�õ�const wchar_t*���ݳ��ж���
    typedef list_t<const wchar_t*>  list_wstr_t;
*/

namespace rx
{
    //-----------------------------------------------------
    //�򵥵�list����(˫������,ͷβ���ɿ��ٷ���)
    //-----------------------------------------------------
    template<class DT>
    class list_t
    {
        //-------------------------------------------------
        template<class dt,typename dummy_t>
        struct raw_node_t
        {
            typedef DT node_data_t;
            struct raw_node_t* volatile prev;	            //�ڵ��ǰ��
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
            struct raw_node_t* volatile prev;	            //�ڵ��ǰ��
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
            struct raw_node_t* volatile prev;	            //�ڵ��ǰ��
            struct raw_node_t* volatile next;	            //�ڵ�ĺ���
            node_data_t  data;
            //---------------------------------------------
            //����data��Ҫ����չ�ߴ�
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
        //-------------------------------------------------
        typedef raw_list_t<node_t>      cntr_t;             //����ʹ�õ�ԭʼջ��������
        //-------------------------------------------------
        mem_allotter_i  &m_mem;                             //�ڴ������,����
        cntr_t           m_cntr;                            //�ײ�ջ����
    public:
        //-------------------------------------------------
        list_t():m_mem(rx_global_mem_allotter()){}
        list_t(mem_allotter_i& ma):m_mem(ma){}
        virtual ~list_t(){clear();}
        mem_allotter_i& mem(){return m_mem;}
        //-------------------------------------------------
        //����򵥵�ֻ��������
        class iterator
        {
            const node_t  *m_node;
            friend class list_t;
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
            //---------------------------------------------
            iterator& operator++()
            {
                m_node=m_node->next;
                return reinterpret_cast<iterator&>(*this);
            }
            iterator& operator--()
            {
                m_node=m_node->prev;
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
        //�鿴��β,������������
        iterator rbegin() const {return iterator(m_cntr.tail());}
        //-------------------------------------------------
        //��������Ľ���λ��
        iterator rend() const { return iterator(m_cntr.origin()); }
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
        //�������(��ָ���ڵ�it�ĺ���),�������½ڵ�.
        template<class dt>
        iterator push_back(const dt &data,iterator it)
        {
            uint32_t es=node_t::ext_size(data);
            node_t *node=(node_t *)m_mem.alloc(es+sizeof(node_t));
            if (!node) return end();
            m_cntr.push_back(it.m_node,node);
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
        //������ջ(��ָ���ڵ�it��ǰ��),�������½ڵ�.
        template<class dt>
        iterator push_front(const dt &data,iterator it)
        {
            uint32_t es=node_t::ext_size(data);
            node_t *node=(node_t *)m_mem.alloc(es+sizeof(node_t));
            if (!node) return end();
            m_cntr.push_front(it.m_node,node);
            node->OC(data,((uint8_t*)node+sizeof(node_t)),es);
            return iterator(node);
        }
        //-------------------------------------------------
        //������βԪ�ز��ͷ�.
        //����ֵ:��ջʧ��,����ɹ�.
        bool pop_back()
        {
            if (!size()) return false;
            node_t *node=m_cntr.pop_back();
            ct::OD(node);                                   //�ڵ�����
            m_mem.free(node);                               //�ڵ��ڴ��ͷ�
            return true;
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
        //-------------------------------------------------
        //ɾ��ָ���ڵ�,itָ��������ڵ�
        //����ֵ:�Ƿ�ɾ���ɹ�
        bool earse(iterator &it)
        {
            if (!size()) return false;
            node_t *node=m_cntr.pick(it.m_node)
            ct::OD(it.m_node);                              //�ڵ�����
            m_mem.free(it.m_node);                          //�ڵ��ڴ��ͷ�

            it.m_node=node;
            return true;
        }
    };

    //�﷨��,����һ������ʹ�õ���������
    typedef list_t<uint32_t>        list_uint32_t;
    typedef list_t<int32_t>         list_int32_t;
    //�﷨��,����һ������ʹ�õ�const char*���ݳ��ж���
    typedef list_t<const char*>     list_cstr_t;
    //�﷨��,����һ������ʹ�õ�const wchar_t*���ݳ��ж���
    typedef list_t<const wchar_t*>  list_wstr_t;
}

#endif