#ifndef _H_RX_RAW_STACK_H_
#define _H_RX_RAW_STACK_H_
#include "rx_cc_macro.h"
#include "rx_assert.h"

namespace rx
{
/*
    //使用此原始stack(或list)的时候,应该构造如下的节点类型
    typedef struct raw_stack_node_t
	{
		struct raw_stack_node_t* volatile next;	            //节点的后趋
    }raw_stack_node_t;
*/    
    //-----------------------------------------------------
    //Raw Stack:原始的栈(或单向list),保持最简形式,不用管理内存
    //要求node_t节点类型中至少含有一个后趋指针next
    template<class node_t>
    class raw_stack
    {
        raw_stack& operator=(const raw_stack&);
    private:
        node_t                 *m_head;	            //栈顶指针
        uint32_t	            m_count;            //栈内长度
    public:
        //-------------------------------------------------
        //构造函数
        raw_stack():m_head(NULL),m_count(0){}
        //-------------------------------------------------
        //栈内元素数量
        uint32_t size() const {return m_count;}
        //-------------------------------------------------
        //将栈中的元素全部摘除,得到一个单向链表
        node_t* pick()
        {
            node_t *now_top=m_head;
            m_head=NULL;
            m_count=0;
            return now_top;
        }
        //-------------------------------------------------
        //查看栈顶(链表头)
        node_t* peek() const {return (node_t*)m_head;}
        //-------------------------------------------------
        //节点入栈(在头节点前插入,变成新的头)
        void push(node_t *new_node)
        {
            new_node->next = m_head;
            m_head=new_node;
            ++m_count;
        }
        //-------------------------------------------------
        //弹出(头结点摘除,后趋变为新节点)
        //(不进行空栈检测,外面使用前需要进行检查)
        node_t* pop()
        {
            rx_assert(m_count!=0);
            node_t *now_top=m_head;
            m_head=m_head->next;
            --m_count;
            return now_top;
        }
        //-------------------------------------------------
    };
}

#endif
