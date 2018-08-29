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
    class raw_stack_t
    {
        raw_stack_t& operator=(const raw_stack_t&);
    private:
        node_t                 *m_head;	            //栈顶指针
        uint32_t	            m_count;            //栈内长度
    public:
        //-------------------------------------------------
        //构造函数
        raw_stack_t():m_head(NULL),m_count(0){}
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
        void push(node_t &new_node){push(&new_node);}
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

    //-----------------------------------------------------
    //Raw List:原始的单向list(或queue),保持最简形式,不用管理内存
    //要求node_t节点类型中至少含有一个后趋指针next
    template<class node_t>
    class raw_list_t
    {
        raw_list_t& operator=(const raw_list_t&);
    private:
        node_t                 *m_head;	            //链表头指针
        node_t                 *m_tail;             //链表尾指针
        uint32_t	            m_count;            //栈内长度
    public:
        //-------------------------------------------------
        //构造函数
        raw_list_t():m_head(NULL),m_tail(NULL),m_count(0){}
        //-------------------------------------------------
        //元素数量
        uint32_t size() const {return m_count;}
        //-------------------------------------------------
        //将元素全部摘除,得到一个单向链表
        node_t* pick()
        {
            node_t *now_top=m_head;
            m_head=NULL;
            m_tail=NULL;
            m_count=0;
            return now_top;
        }
        //-------------------------------------------------
        //查看链表头或尾
        node_t* peek(bool is_tail=false) const {return is_tail?m_tail:(node_t*)m_head;}
        //-------------------------------------------------
        //节点挂入尾部(变成新的尾节点)
        void push_back(node_t *new_node)
        {
            new_node->next = NULL;                          //新节点后趋为空
            if (m_tail)
                m_tail->next=new_node;                      //将新节点挂接到尾节点的后趋;
            else
                m_head=new_node;                            //空链表时关联头结点
            m_tail=new_node;                                //尾节点指向新节点
            ++m_count;
        }
        void push_back(node_t &new_node){push_back(&new_node);}
        //-------------------------------------------------
        //节点挂入头部(变成新的头节点)
        void push_front(node_t *new_node)
        {
            new_node->next=m_head;
            m_head?0:m_tail=new_node;
            m_head=new_node;
            ++m_count;
        }
        void push_front(node_t &new_node){push_front(&new_node);}
        //-------------------------------------------------
        //节点弹出(头结点摘除,后趋变为新头)
        //(不进行空栈检测,外面使用前需要进行检查)
        node_t* pop_front()
        {
            rx_assert(m_count!=0);
            node_t *now_top=m_head;                         //记录原来的头结点,准备返回
            m_head=m_head->next;                            //头结点指向其后趋
            --m_count==0?m_tail=NULL:0;                     //链表为空的时候,尾节点也置空
            rx_assert_if(m_count==0,m_tail==NULL&&m_head==NULL);
            return now_top;
        }
    };
}

#endif
