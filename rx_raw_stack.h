#ifndef _H_RX_RAW_STACK_H_
#define _H_RX_RAW_STACK_H_
#include "rx_cc_macro.h"
#include "rx_assert.h"

namespace rx
{
    //-----------------------------------------------------
    //Raw Stack:原始的栈,保持最简形式,不用管理内存
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
        //查看栈顶
        node_t* peek() const {return (node_t*)m_head;}
        //-------------------------------------------------
        //节点入栈
        void push(node_t *new_node)
        {
            new_node->next = m_head;
            m_head=new_node;
            ++m_count;
        }
        //-------------------------------------------------
        //弹出(不进行空栈检测,外面使用前需要进行检查)
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
