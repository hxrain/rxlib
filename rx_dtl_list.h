#ifndef _RX_DTL_LIST_H_
#define _RX_DTL_LIST_H_

#include "rx_cc_macro.h"
#include "rx_dtl_list_raw.h"
#include "rx_mem_alloc.h"
#include "rx_mem_alloc_cntr.h"
#include "rx_str_tiny.h"

/*
    //-----------------------------------------------------
    //简单的list容器(双向链表,头尾均可快速访问)
    template<class DT>
    class list_t;

    //语法糖,定义一个便于使用的整数队列
    typedef list_t<uint32_t>        list_uint32_t;
    typedef list_t<int32_t>         list_int32_t;
    //语法糖,定义一个便于使用的const char*内容持有队列
    typedef list_t<const char*>     list_cstr_t;
    //语法糖,定义一个便于使用的const wchar_t*内容持有队列
    typedef list_t<const wchar_t*>  list_wstr_t;
*/

namespace rx
{
    //-----------------------------------------------------
    //简单的list容器(双向链表,头尾均可快速访问)
    //-----------------------------------------------------
    template<class DT>
    class list_t
    {
        //-------------------------------------------------
        template<class dt,typename dummy_t>
        struct raw_node_t
        {
            typedef DT node_data_t;
            struct raw_node_t* volatile prev;	            //节点的前趋
            struct raw_node_t* volatile next;	            //节点的后趋
            node_data_t  data;
            //---------------------------------------------
            //计算data需要的扩展尺寸
            template<class KT> static int ext_size(const KT &data){return 0;}
            //---------------------------------------------
            //进行定向构造并初始化
            template<class val_t> void OC(val_t &val,void* ext_buff,uint32_t ext_size){ct::OC(&data,val);}
        };

        //-------------------------------------------------
        template<typename dummy_t>
        struct raw_node_t<const char*,dummy_t>
        {
            typedef tiny_string_head_ct node_data_t;
            struct raw_node_t* volatile prev;	            //节点的前趋
            struct raw_node_t* volatile next;	            //节点的后趋
            node_data_t  data;
            //---------------------------------------------
            //计算data需要的扩展尺寸
            template<class KT> static int ext_size(const KT &data){return st::strlen(data)+1;}
            //---------------------------------------------
            //进行定向构造并初始化
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
            struct raw_node_t* volatile prev;	            //节点的前趋
            struct raw_node_t* volatile next;	            //节点的后趋
            node_data_t  data;
            //---------------------------------------------
            //计算data需要的扩展尺寸
            template<class KT> static int ext_size(const KT &data){return (st::strlen(data)+1) * sc<wchar_t>::char_size();}
            //---------------------------------------------
            //进行定向构造并初始化
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
        typedef raw_node_t<DT,void>     node_t;             //最终使用的原始栈节点类型    
    protected:
        //-------------------------------------------------
        typedef raw_list_t<node_t>      cntr_t;             //最终使用的原始栈容器类型
        //-------------------------------------------------
        mem_allotter_i  &m_mem;                             //内存分配器,引用
        cntr_t           m_cntr;                            //底层栈容器
    public:
        //-------------------------------------------------
        list_t():m_mem(rx_global_mem_allotter()){}
        list_t(mem_allotter_i& ma):m_mem(ma){}
        virtual ~list_t(){clear();}
        mem_allotter_i& mem(){return m_mem;}
        //-------------------------------------------------
        //定义简单的只读迭代器
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
            //* 运算符重载,用于获取当前节点的data
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
        //查看队首;返回遍历的初始位置
        iterator begin() const {return iterator(m_cntr.head());}
        //-------------------------------------------------
        //返回遍历的结束位置
        iterator end() const { return iterator(m_cntr.origin()); }
        //-------------------------------------------------
        //查看队尾,或反向遍历的起点
        iterator rbegin() const {return iterator(m_cntr.tail());}
        //-------------------------------------------------
        //反向遍历的结束位置
        iterator rend() const { return iterator(m_cntr.origin()); }
        //-------------------------------------------------
        //释放全部节点
        void clear() {while(pop_front());}
        //-------------------------------------------------
        //内部元素数量
        uint32_t size() const {return m_cntr.size();}
        //-------------------------------------------------
        //数据入队(队尾),返回最新节点.
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
        //数据入队(在指定节点it的后面),返回最新节点.
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
        //数据入栈(队首),返回最新节点.
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
        //数据入栈(在指定节点it的前面),返回最新节点.
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
        //弹出队尾元素并释放.
        //返回值:空栈失败,否则成功.
        bool pop_back()
        {
            if (!size()) return false;
            node_t *node=m_cntr.pop_back();
            ct::OD(node);                                   //节点析构
            m_mem.free(node);                               //节点内存释放
            return true;
        }
        //-------------------------------------------------
        //弹出队首元素并释放.
        //返回值:空栈失败,否则成功.
        bool pop_front()
        {
            if (!size()) return false;
            node_t *node=m_cntr.pop_front();
            ct::OD(node);                                   //节点析构
            m_mem.free(node);                               //节点内存释放
            return true;
        }
        //-------------------------------------------------
        //删除指定节点,it指向其后续节点
        //返回值:是否删除成功
        bool earse(iterator &it)
        {
            if (!size()) return false;
            node_t *node=m_cntr.pick(it.m_node)
            ct::OD(it.m_node);                              //节点析构
            m_mem.free(it.m_node);                          //节点内存释放

            it.m_node=node;
            return true;
        }
    };

    //语法糖,定义一个便于使用的整数队列
    typedef list_t<uint32_t>        list_uint32_t;
    typedef list_t<int32_t>         list_int32_t;
    //语法糖,定义一个便于使用的const char*内容持有队列
    typedef list_t<const char*>     list_cstr_t;
    //语法糖,定义一个便于使用的const wchar_t*内容持有队列
    typedef list_t<const wchar_t*>  list_wstr_t;
}

#endif