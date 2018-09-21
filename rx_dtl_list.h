#ifndef _RX_DTL_LIST_H_
#define _RX_DTL_LIST_H_

#include "rx_cc_macro.h"
#include "rx_dtl_list_raw.h"
#include "rx_mem_alloc.h"
#include "rx_mem_alloc_cntr.h"
#include "rx_str_tiny.h"

namespace rx
{
    //-----------------------------------------------------
    //�򵥵�ջ����(��������)����
    template<class DT>
    class stack_t
    {
        //-------------------------------------------------
        template<class dt,typename dummy_t>
        struct raw_stack_node_t
        {
            typedef DT node_data_t;
            node_data_t  data;
            struct raw_stack_node_t* volatile next;	            //�ڵ�ĺ���
            //---------------------------------------------
            //����data��Ҫ����չ�ߴ�
            template<class KT> static int ext_size(const KT &data){return 0;}
            //---------------------------------------------
            //���ж����첢��ʼ��
            template<class val_t> void OC(val_t &val,void* ext_buff,uint32_t ext_size){ct::OC(&data,val);}
        };

        //-------------------------------------------------
        template<typename dummy_t>
        struct raw_stack_node_t<const char*,dummy_t>
        {
            typedef tiny_string_head_ct node_data_t;
            node_data_t  data;
            struct raw_stack_node_t* volatile next;	            //�ڵ�ĺ���
            //---------------------------------------------
            //����data��Ҫ����չ�ߴ�
            template<class KT> static int ext_size(const KT &data){return st::strlen(data)+1;}
            //---------------------------------------------
            //���ж����첢��ʼ��
            template<class val_t> void OC(val_t &val,void* ext_buff,uint32_t ext_size){ct::OC(&data,ext_size,ext_buff);data.set(val);}
        };

        //-------------------------------------------------
        template<typename dummy_t>
        struct raw_stack_node_t<const wchar_t*,dummy_t>
        {
            typedef tiny_string_head_wt node_data_t;
            node_data_t  data;
            struct raw_stack_node_t* volatile next;	            //�ڵ�ĺ���
            //---------------------------------------------
            //����data��Ҫ����չ�ߴ�
            template<class KT> static int ext_size(const KT &data){return st::strlen(data)+1;}
            //---------------------------------------------
            //���ж����첢��ʼ��
            template<class val_t> void OC(val_t &val,void* ext_buff,uint32_t ext_size){ct::OC(&data,ext_size,ext_buff);data.set(val);}
        };

        //-------------------------------------------------
        typedef raw_stack_node_t<DT,void>   raw_node_t;         //����ʹ�õ�ԭʼջ�ڵ�����
        typedef raw_stack_t<raw_node_t>     raw_stack_t;        //����ʹ�õ�ԭʼջ��������
        //-------------------------------------------------
        mem_allotter_i  &m_mem;                                  //�ڴ������,����
        raw_stack_t     m_stack;                                //�ײ�ջ����
    public:
        typedef raw_node_t node_t;
        //-------------------------------------------------
        stack_t():m_mem(global_mem_allotter()){}
        stack_t(mem_allotter_i& ma):m_mem(ma){}
        virtual ~stack_t(){clear();}
        //-------------------------------------------------
        //����򵥵�ֻ��������
        class iterator
        {
            const node_t  *m_node;
            friend class stack_t;
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
            //* ���������,���ڻ�ȡ��ǰ�ڵ��data
            const typename node_t::node_data_t& operator* () const {return m_node->data;}
            //---------------------------------------------
            iterator& operator++()
            {
                m_node=m_node->next;
                return reinterpret_cast<iterator&>(*this);
            }
        };
        //-------------------------------------------------
        //�鿴ջ��;���ر����ĳ�ʼλ��
        iterator begin() const {return iterator(m_stack.head());}
        //-------------------------------------------------
        //���ر����Ľ���λ��
        iterator end() const { return iterator(NULL); }
        //-------------------------------------------------
        //�ͷ�ȫ���ڵ�
        void clear() {while(pop_front());}
        //-------------------------------------------------
        //ջ��Ԫ������
        uint32_t size() const {return m_stack.size();}
        //-------------------------------------------------
        //������ջ,�������½ڵ�.
        template<class dt>
        iterator push_front(const dt &data)
        {
            uint32_t es=node_t::ext_size(data);
            node_t *node=m_mem.alloc(es+sizeof(node_t));
            if (!node) return iterator(NULL);
            node->OC(data,(uint8_t*)node+sizeof(node_t),es);
            return iterator(node);
        }
        //-------------------------------------------------
        //����ջ��Ԫ�ز��ͷ�.
        //����ֵ:��ջʧ��,����ɹ�.
        bool pop_front()
        {
            if (!size()) return false;
            node_t *node=m_stack.pop_front();
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


}

#endif