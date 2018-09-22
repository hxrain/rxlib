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
    //简单的栈容器(或单向链表)定义
    template<class DT>
    class stack_t
    {
        //-------------------------------------------------
        template<class dt,typename dummy_t>
        struct raw_stack_node_t
        {
            typedef DT node_data_t;
            node_data_t  data;
            struct raw_stack_node_t* volatile next;	            //节点的后趋
            //---------------------------------------------
            //计算data需要的扩展尺寸
            template<class KT> static int ext_size(const KT &data){return 0;}
            //---------------------------------------------
            //进行定向构造并初始化
            template<class val_t> void OC(val_t &val,void* ext_buff,uint32_t ext_size){ct::OC(&data,val);}
        };

        //-------------------------------------------------
        template<typename dummy_t>
        struct raw_stack_node_t<const char*,dummy_t>
        {
            typedef tiny_string_head_ct node_data_t;
            node_data_t  data;
            struct raw_stack_node_t* volatile next;	            //节点的后趋
            //---------------------------------------------
            //计算data需要的扩展尺寸
            template<class KT> static int ext_size(const KT &data){return st::strlen(data)+1;}
            //---------------------------------------------
            //进行定向构造并初始化
            template<class val_t> void OC(val_t &val,void* ext_buff,uint32_t ext_size)
            {
                char *buf=(char*)ext_buff;
                ct::OC(&data,ext_size,buf);
                data.set(val);
            }
        };

        //-------------------------------------------------
        template<typename dummy_t>
        struct raw_stack_node_t<const wchar_t*,dummy_t>
        {
            typedef tiny_string_head_wt node_data_t;
            node_data_t  data;
            struct raw_stack_node_t* volatile next;	            //节点的后趋
            //---------------------------------------------
            //计算data需要的扩展尺寸
            template<class KT> static int ext_size(const KT &data){return st::strlen(data)+1;}
            //---------------------------------------------
            //进行定向构造并初始化
            template<class val_t> void OC(val_t &val,void* ext_buff,uint32_t ext_size)
            {
                wchar_t *buf=(wchar_t*)ext_buff;
                ct::OC(&data,ext_size,buf);
                data.set(val);
            }
        };

        //-------------------------------------------------
        typedef raw_stack_node_t<DT,void>   raw_node_t;         //最终使用的原始栈节点类型
        typedef raw_stack_t<raw_node_t>     raw_stack_t;        //最终使用的原始栈容器类型
        //-------------------------------------------------
        mem_allotter_i  &m_mem;                                  //内存分配器,引用
        raw_stack_t     m_stack;                                //底层栈容器
    public:
        typedef raw_node_t node_t;
        //-------------------------------------------------
        stack_t():m_mem(global_mem_allotter()){}
        stack_t(mem_allotter_i& ma):m_mem(ma){}
        virtual ~stack_t(){clear();}
        //-------------------------------------------------
        //定义简单的只读迭代器
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
            //* 运算符重载,用于获取当前节点的data
            const typename node_t::node_data_t& operator* () const {return m_node->data;}
            //---------------------------------------------
            iterator& operator++()
            {
                m_node=m_node->next;
                return reinterpret_cast<iterator&>(*this);
            }
        };
        //-------------------------------------------------
        //查看栈顶;返回遍历的初始位置
        iterator begin() const {return iterator(m_stack.head());}
        //-------------------------------------------------
        //返回遍历的结束位置
        iterator end() const { return iterator(NULL); }
        //-------------------------------------------------
        //释放全部节点
        void clear() {while(pop_front());}
        //-------------------------------------------------
        //栈内元素数量
        uint32_t size() const {return m_stack.size();}
        //-------------------------------------------------
        //数据入栈,返回最新节点.
        template<class dt>
        iterator push_front(const dt &data)
        {
            uint32_t es=node_t::ext_size(data);
            node_t *node=(node_t *)m_mem.alloc(es+sizeof(node_t));
            if (!node) return iterator(NULL);
            m_stack.push_front(node);
            node->OC(data,((uint8_t*)node+sizeof(node_t)),es);
            return iterator(node);
        }
        //-------------------------------------------------
        //弹出栈顶元素并释放.
        //返回值:空栈失败,否则成功.
        bool pop_front()
        {
            if (!size()) return false;
            node_t *node=m_stack.pop_front();
            ct::OD(node);                               //节点析构
            m_mem.free(node);                           //节点内存释放
            return true;
        }
    };

    //语法糖,定义一个便于使用的整数栈
    typedef stack_t<uint32_t>        stack_uint32_t;
    typedef stack_t<int32_t>         stack_int32_t;
    //语法糖,定义一个便于使用的const char*内容持有栈
    typedef stack_t<const char*>     stack_cstr_t;
    //语法糖,定义一个便于使用的const wchar_t*内容持有栈
    typedef stack_t<const wchar_t*>  stack_wstr_t;


}

#endif