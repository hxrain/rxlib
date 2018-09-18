#ifndef _H_RX_RAW_STACK_H_
#define _H_RX_RAW_STACK_H_
#include "rx_cc_macro.h"
#include "rx_assert.h"

/**���ļ���װ��������������ĺ��Ĺ���

    //-----------------------------------------------------
    //ʹ�ô�ԭʼstack��ʱ��,Ӧ�ù������µĽڵ�����
    typedef struct raw_stack_node_t
    {
        struct raw_stack_node_t* volatile next;	            //�ڵ�ĺ���
    }raw_stack_node_t;

    //���������ջ����.
    template<class node_t>
    class raw_stack_t;

    //����������������,ά����β�ڵ�ָ��,����ֱ�ӷ������Ľڵ�.
    template<class node_t>
    class raw_queue_t;

    //-----------------------------------------------------
    //ʹ�ô�ԭʼlist��ʱ��,Ӧ�ù������µĽڵ�����
    typedef struct raw_list_node_t
    {
        struct raw_list_node_t* volatile prev;	            //�ڵ��ǰ��
        struct raw_list_node_t* volatile next;	            //�ڵ�ĺ���
    }raw_list_node_t;

    //������ʵ����˫������Ĺ���
    template<class node_t>
    class raw_list_t;

**/
namespace rx
{
    /*

    */
    //-----------------------------------------------------
    //Raw Stack:ԭʼ��ջ(����list),���������ʽ,�������ڴ�
    //Ҫ��node_t�ڵ����������ٺ���һ������ָ��next
    template<class node_t>
    class raw_stack_t
    {
        raw_stack_t& operator=(const raw_stack_t&);
    private:
        node_t                 *m_head;	            //ջ��ָ��
        uint32_t	            m_count;            //ջ�ڳ���
    public:
        //-------------------------------------------------
        //���캯��
        raw_stack_t():m_head(NULL),m_count(0) {}
        //-------------------------------------------------
        //ջ��Ԫ������
        uint32_t size() const {return m_count;}
        //-------------------------------------------------
        //��ջ�е�Ԫ��ȫ��ժ��,�õ�һ����������
        node_t* pick()
        {
            node_t *now_top=m_head;
            m_head=NULL;
            m_count=0;
            return now_top;
        }
        //-------------------------------------------------
        //�鿴ջ��(����ͷ)
        node_t* head() const {return m_head;}
        //-------------------------------------------------
        //�ڵ���ջ(��ͷ�ڵ�ǰ����,����µ�ͷ)
        void push_front(node_t *new_node)
        {
            new_node->next = m_head;
            m_head=new_node;
            ++m_count;
        }
        void push_front(node_t &new_node) {push_front(&new_node);}
        //-------------------------------------------------
        //����(ͷ���ժ��,������Ϊ�½ڵ�)
        //(�����п�ջ���,����ʹ��ǰ��Ҫ���м��)
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
    //Raw List:ԭʼ�ĵ���list(��queue),���������ʽ,���ù����ڴ�
    //Ҫ��node_t�ڵ��������躬��һ������ָ��next
    template<class node_t>
    class raw_queue_t
    {
        raw_queue_t& operator=(const raw_queue_t&);
    private:
        node_t                 *m_head;	            //����ͷָ��
        node_t                 *m_tail;             //����βָ��
        uint32_t	            m_count;            //ջ�ڳ���
    public:
        //-------------------------------------------------
        //���캯��
        raw_queue_t():m_head(NULL),m_tail(NULL),m_count(0) {}
        //-------------------------------------------------
        //Ԫ������
        uint32_t size() const {return m_count;}
        //-------------------------------------------------
        //��Ԫ��ȫ��ժ��,�õ�һ����������
        node_t* pick()
        {
            node_t *now_top=m_head;
            m_head=NULL;
            m_tail=NULL;
            m_count=0;
            return now_top;
        }
        //-------------------------------------------------
        //�鿴����ͷ��β
        node_t* head() const {return m_head;}
        node_t* tail() const {return m_tail;}
        //-------------------------------------------------
        //�ڵ����β��(����µ�β�ڵ�)
        void push_back(node_t *new_node)
        {
            new_node->next = NULL;                          //�½ڵ����Ϊ��
            if (m_tail)
                m_tail->next=new_node;                      //���½ڵ�ҽӵ�β�ڵ�ĺ���;
            else
                m_head=new_node;                            //������ʱ����ͷ���
            m_tail=new_node;                                //β�ڵ�ָ���½ڵ�
            ++m_count;
        }
        void push_back(node_t &new_node) {push_back(&new_node);}
        //-------------------------------------------------
        //�ڵ����ͷ��(����µ�ͷ�ڵ�)
        void push_front(node_t *new_node)
        {
            new_node->next=m_head;
            m_head?0:m_tail=new_node;
            m_head=new_node;
            ++m_count;
        }
        void push_front(node_t &new_node) {push_front(&new_node);}
        //-------------------------------------------------
        //�ڵ㵯��(ͷ���ժ��,������Ϊ��ͷ)
        //(�����п�ջ���,����ʹ��ǰ��Ҫ���м��)
        node_t* pop_front()
        {
            rx_assert(m_count!=0);
            node_t *now_top=m_head;                         //��¼ԭ����ͷ���,׼������
            m_head=m_head->next;                            //ͷ���ָ�������
            --m_count==0?m_tail=NULL:0;                     //����Ϊ�յ�ʱ��,β�ڵ�Ҳ�ÿ�
            rx_assert_if(m_count==0,m_tail==NULL&&m_head==NULL);
            return now_top;
        }
    };

    //-----------------------------------------------------
    //ԭʼ��˫������ʵ��,�ڲ��������ڴ����
    //�ڵ����ͱ��뺬��prev��nextָ��.
    template<class node_t>
    class raw_list_t
    {
        raw_list_t& operator=(const raw_list_t&);
    private:
        node_t                 *m_head;	            //����ͷָ��
        node_t                 *m_tail;             //����βָ��
        uint32_t	            m_count;            //������
    public:
        //-------------------------------------------------
        //���캯��
        raw_list_t():m_head(NULL),m_tail(NULL),m_count(0) {}
        //-------------------------------------------------
        //Ԫ������
        uint32_t size() const {return m_count;}
        //-------------------------------------------------
        //�鿴����ͷ��β
        node_t* head() const {return m_head;}
        node_t* tail() const {return m_tail;}
        //-------------------------------------------------
        //�ڵ����β��(����µ�β�ڵ�)
        void push_back(node_t *new_node)
        {
            rx_assert(new_node!=NULL);
            new_node->next = NULL;                          //�½ڵ����Ϊ��
            if (m_tail)
                m_tail->next=new_node;                      //���½ڵ�ҽӵ�β�ڵ�ĺ���;
            else
                m_head=new_node;                            //������ʱ����ͷ���Ϊ�½ڵ�
            new_node->prev=m_tail;                          //�½ڵ��ǰ��ָ��ԭ����β�ڵ�
            m_tail=new_node;                                //β�ڵ�ָ���½ڵ�
            ++m_count;
        }
        void push_back(node_t &new_node) {push_back(&new_node);}
        //-------------------------------------------------
        //���½ڵ�new_node�ҽӵ�ָ���ڵ�prev�ĺ���
        void push_back(const node_t *prev,node_t *new_node)
        {
            rx_assert(new_node!=NULL&&prev!=NULL);
            new_node->next = prev->next;                    //�½ڵ����ָ��ǰ�ýڵ�ĺ���
            new_node->prev = prev;                          //�½ڵ��ǰ��ָ��ǰ�ýڵ�
            prev->next=new_node;                            //ǰ�ýڵ�ĺ���ָ���½ڵ�
            if (prev==m_tail)
                m_tail=new_node;                            //���ǰ�ýڵ����β�ڵ�,��β�ڵ�ָ���½ڵ�
            ++m_count;
        }
        //-------------------------------------------------
        //�ڵ����ͷ��(����µ�ͷ�ڵ�)
        void push_front(node_t *new_node)
        {
            rx_assert(new_node!=NULL);
            new_node->prev=NULL;                            //�½ڵ��ǰ��Ϊ��
            if (m_head)
                m_head->prev=new_node;                      //���½ڵ�ҽӵ�ͷ�ڵ��ǰ��
            else
                m_tail=new_node;                            //������ʱ����β�ڵ�Ϊ�½ڵ�
            new_node->next=m_head;                          //�½ڵ�ĺ���ָ��ԭ����ͷ���
            m_head=new_node;                                //ԭ����ͷ���ָ���½ڵ�
            ++m_count;
        }
        void push_front(node_t &new_node) {push_front(&new_node);}
        //-------------------------------------------------
        //���½ڵ�new_node�ҽӵ�ָ���ڵ�next��ǰ��
        void push_front(const node_t *next,node_t *new_node)
        {
            rx_assert(new_node!=NULL&&next!=NULL);
            new_node->next = next;                          //�½ڵ����ָ����ýڵ�
            new_node->prev = next->prev;                    //�½ڵ��ǰ��ָ����ýڵ��ǰ��
            next->prev=new_node;                            //���ýڵ��ǰ��ָ���½ڵ�
            if (next==m_head)
                m_head=new_node;                            //���ǰ�ýڵ����β�ڵ�,��β�ڵ�ָ���½ڵ�
            ++m_count;
        }
        //-------------------------------------------------
        //ժ��β���Ľڵ�
        //����ֵ:ժ���Ľڵ�
        node_t* pop_back()
        {
            rx_assert(m_count!=0);
            node_t *node=m_tail;                            //��¼ԭ����β���,׼������
            m_tail=m_tail->prev;                            //β���ָ����ǰ��
            if (--m_count==0)
                m_head=NULL;                                //����Ϊ�յ�ʱ��,ͷ�ڵ�Ҳ�ÿ�
            else
                m_tail->next=NULL;                          //����ǿյ�ʱ��,β�ڵ�ĺ����ÿ�
            rx_assert_if(m_count==0,m_tail==NULL&&m_head==NULL);
            return node;
        }
        //-------------------------------------------------
        //ժ��ͷ���Ľڵ�
        //����ֵ:ժ���Ľڵ�
        node_t* pop_front()
        {
            rx_assert(m_count!=0);
            node_t *node=m_head;                            //��¼ԭ����ͷ���,׼������
            m_head=m_head->next;                            //ͷ���ָ�������
            if (--m_count==0)
                m_tail=NULL;                                //����Ϊ�յ�ʱ��,β�ڵ�Ҳ�ÿ�
            else
                m_head->prev=NULL;                          //����ǿյ�ʱ��,ͷ�ڵ��ǰ���ÿ�
            rx_assert_if(m_count==0,m_tail==NULL&&m_head==NULL);
            return node;
        }
        //-------------------------------------------------
        //��ָ���Ľڵ��������ժ��
        //����ָ���ڵ�ĺ���
        node_t *pick(node_t *node)
        {
            rx_assert(m_count!=0&&node!=NULL);
            --m_count;
            node_t *prev=node->prev;                        //��ȡָ���ڵ��ǰ��
            node_t *next=node->next;                        //��ȡָ���ڵ�ĺ���
            if (prev)
                prev->next=next;                            //�����ǰ������,����ǰ���ĺ���ָ�������
            if (next)
                next->prev=prev;                            //������������,���������ǰ��ָ����ǰ��

            if (node==m_head)
                m_head=next;                                //���ָ���ڵ����ͷ���,��ͷ���ָ�������
            if (node==m_tail)
                m_tail=prev;                                //���ָ���ڵ����β�ڵ�,��β�ڵ�ָ����ǰ��
            return next;
        }
        //-------------------------------------------------
        //��Ԫ��ȫ��ժ��
        //��������ͷ�ڵ�
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
