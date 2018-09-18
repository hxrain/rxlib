#ifndef _H_RX_RAW_STACK_H_
#define _H_RX_RAW_STACK_H_
#include "rx_cc_macro.h"
#include "rx_assert.h"

/**本文件封装了链表基础容器的核心功能

    //-----------------------------------------------------
    //使用此原始stack的时候,应该构造如下的节点类型
    typedef struct raw_stack_node_t
    {
        struct raw_stack_node_t* volatile next;	            //节点的后趋
    }raw_stack_node_t;

    //单向链表或栈容器.
    template<class node_t>
    class raw_stack_t;

    //单向链表或队列容器,维护了尾节点指针,可以直接访问最后的节点.
    template<class node_t>
    class raw_queue_t;

    //-----------------------------------------------------
    //使用此原始list的时候,应该构造如下的节点类型
    typedef struct raw_list_node_t
    {
        struct raw_list_node_t* volatile prev;	            //节点的前趋
        struct raw_list_node_t* volatile next;	            //节点的后趋
    }raw_list_node_t;

    //完整的实现了双向链表的功能
    template<class node_t>
    class raw_list_t;

**/
namespace rx
{
    /*

    */
    //-----------------------------------------------------
    //Raw Stack:原始的栈(或单向list),保持最简形式,不管理内存
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
        raw_stack_t():m_head(NULL),m_count(0) {}
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
        node_t* head() const {return m_head;}
        //-------------------------------------------------
        //节点入栈(在头节点前插入,变成新的头)
        void push_front(node_t *new_node)
        {
            new_node->next = m_head;
            m_head=new_node;
            ++m_count;
        }
        void push_front(node_t &new_node) {push_front(&new_node);}
        //-------------------------------------------------
        //弹出(头结点摘除,后趋变为新节点)
        //(不进行空栈检测,外面使用前需要进行检查)
        node_t* pop_front()
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
    //要求node_t节点类型中需含有一个后趋指针next
    template<class node_t>
    class raw_queue_t
    {
        raw_queue_t& operator=(const raw_queue_t&);
    private:
        node_t                 *m_head;	            //链表头指针
        node_t                 *m_tail;             //链表尾指针
        uint32_t	            m_count;            //栈内长度
    public:
        //-------------------------------------------------
        //构造函数
        raw_queue_t():m_head(NULL),m_tail(NULL),m_count(0) {}
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
        node_t* head() const {return m_head;}
        node_t* tail() const {return m_tail;}
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
        void push_back(node_t &new_node) {push_back(&new_node);}
        //-------------------------------------------------
        //节点挂入头部(变成新的头节点)
        void push_front(node_t *new_node)
        {
            new_node->next=m_head;
            m_head?0:m_tail=new_node;
            m_head=new_node;
            ++m_count;
        }
        void push_front(node_t &new_node) {push_front(&new_node);}
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

    //-----------------------------------------------------
    //原始的双向链表实现,内部不进行内存管理
    //节点类型必须含有prev和next指针.
    template<class node_t>
    class raw_list_t
    {
        raw_list_t& operator=(const raw_list_t&);
    private:
        node_t                 *m_head;	            //链表头指针
        node_t                 *m_tail;             //链表尾指针
        uint32_t	            m_count;            //链表长度
    public:
        //-------------------------------------------------
        //构造函数
        raw_list_t():m_head(NULL),m_tail(NULL),m_count(0) {}
        //-------------------------------------------------
        //元素数量
        uint32_t size() const {return m_count;}
        //-------------------------------------------------
        //查看链表头或尾
        node_t* head() const {return m_head;}
        node_t* tail() const {return m_tail;}
        //-------------------------------------------------
        //节点挂入尾部(变成新的尾节点)
        void push_back(node_t *new_node)
        {
            rx_assert(new_node!=NULL);
            new_node->next = NULL;                          //新节点后趋为空
            if (m_tail)
                m_tail->next=new_node;                      //将新节点挂接到尾节点的后趋;
            else
                m_head=new_node;                            //空链表时关联头结点为新节点
            new_node->prev=m_tail;                          //新节点的前趋指向原来的尾节点
            m_tail=new_node;                                //尾节点指向新节点
            ++m_count;
        }
        void push_back(node_t &new_node) {push_back(&new_node);}
        //-------------------------------------------------
        //将新节点new_node挂接到指定节点prev的后面
        void push_back(const node_t *prev,node_t *new_node)
        {
            rx_assert(new_node!=NULL&&prev!=NULL);
            new_node->next = prev->next;                    //新节点后趋指向前置节点的后趋
            new_node->prev = prev;                          //新节点的前趋指向前置节点
            prev->next=new_node;                            //前置节点的后趋指向新节点
            if (prev==m_tail)
                m_tail=new_node;                            //如果前置节点就是尾节点,则尾节点指向新节点
            ++m_count;
        }
        //-------------------------------------------------
        //节点挂入头部(变成新的头节点)
        void push_front(node_t *new_node)
        {
            rx_assert(new_node!=NULL);
            new_node->prev=NULL;                            //新节点的前趋为空
            if (m_head)
                m_head->prev=new_node;                      //将新节点挂接到头节点的前趋
            else
                m_tail=new_node;                            //空链表时关联尾节点为新节点
            new_node->next=m_head;                          //新节点的后趋指向原来的头结点
            m_head=new_node;                                //原来的头结点指向新节点
            ++m_count;
        }
        void push_front(node_t &new_node) {push_front(&new_node);}
        //-------------------------------------------------
        //将新节点new_node挂接到指定节点next的前面
        void push_front(const node_t *next,node_t *new_node)
        {
            rx_assert(new_node!=NULL&&next!=NULL);
            new_node->next = next;                          //新节点后趋指向后置节点
            new_node->prev = next->prev;                    //新节点的前趋指向后置节点的前趋
            next->prev=new_node;                            //后置节点的前趋指向新节点
            if (next==m_head)
                m_head=new_node;                            //如果前置节点就是尾节点,则尾节点指向新节点
            ++m_count;
        }
        //-------------------------------------------------
        //摘除尾部的节点
        //返回值:摘除的节点
        node_t* pop_back()
        {
            rx_assert(m_count!=0);
            node_t *node=m_tail;                            //记录原来的尾结点,准备返回
            m_tail=m_tail->prev;                            //尾结点指向其前趋
            if (--m_count==0)
                m_head=NULL;                                //链表为空的时候,头节点也置空
            else
                m_tail->next=NULL;                          //链表非空的时候,尾节点的后趋置空
            rx_assert_if(m_count==0,m_tail==NULL&&m_head==NULL);
            return node;
        }
        //-------------------------------------------------
        //摘除头部的节点
        //返回值:摘除的节点
        node_t* pop_front()
        {
            rx_assert(m_count!=0);
            node_t *node=m_head;                            //记录原来的头结点,准备返回
            m_head=m_head->next;                            //头结点指向其后趋
            if (--m_count==0)
                m_tail=NULL;                                //链表为空的时候,尾节点也置空
            else
                m_head->prev=NULL;                          //链表非空的时候,头节点的前趋置空
            rx_assert_if(m_count==0,m_tail==NULL&&m_head==NULL);
            return node;
        }
        //-------------------------------------------------
        //将指定的节点从链表中摘除
        //返回指定节点的后趋
        node_t *pick(node_t *node)
        {
            rx_assert(m_count!=0&&node!=NULL);
            --m_count;
            node_t *prev=node->prev;                        //获取指定节点的前趋
            node_t *next=node->next;                        //获取指定节点的后趋
            if (prev)
                prev->next=next;                            //如果其前趋存在,则其前趋的后趋指向其后趋
            if (next)
                next->prev=prev;                            //如果其后趋存在,则其后趋的前趋指向其前趋

            if (node==m_head)
                m_head=next;                                //如果指定节点就是头结点,则头结点指向其后趋
            if (node==m_tail)
                m_tail=prev;                                //如果指定节点就是尾节点,则尾节点指向其前趋
            return next;
        }
        //-------------------------------------------------
        //将元素全部摘除
        //返回链表头节点
        node_t* pick()
        {
            node_t *node=m_head;
            m_head=NULL;
            m_tail=NULL;
            m_count=0;
            return node;
        }
    };
}

#endif
