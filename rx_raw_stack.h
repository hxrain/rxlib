#ifndef _H_RX_RAW_STACK_H_
#define _H_RX_RAW_STACK_H_
#include "rx_cc_macro.h"
#include "rx_assert.h"

namespace rx
{
/*
    //ʹ�ô�ԭʼstack(��list)��ʱ��,Ӧ�ù������µĽڵ�����
    typedef struct raw_stack_node_t
	{
		struct raw_stack_node_t* volatile next;	            //�ڵ�ĺ���
    }raw_stack_node_t;
*/
    //-----------------------------------------------------
    //Raw Stack:ԭʼ��ջ(����list),���������ʽ,���ù����ڴ�
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
        raw_stack_t():m_head(NULL),m_count(0){}
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
        node_t* peek() const {return (node_t*)m_head;}
        //-------------------------------------------------
        //�ڵ���ջ(��ͷ�ڵ�ǰ����,����µ�ͷ)
        void push(node_t *new_node)
        {
            new_node->next = m_head;
            m_head=new_node;
            ++m_count;
        }
        void push(node_t &new_node){push(&new_node);}
        //-------------------------------------------------
        //����(ͷ���ժ��,������Ϊ�½ڵ�)
        //(�����п�ջ���,����ʹ��ǰ��Ҫ���м��)
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
    //Raw List:ԭʼ�ĵ���list(��queue),���������ʽ,���ù����ڴ�
    //Ҫ��node_t�ڵ����������ٺ���һ������ָ��next
    template<class node_t>
    class raw_list_t
    {
        raw_list_t& operator=(const raw_list_t&);
    private:
        node_t                 *m_head;	            //����ͷָ��
        node_t                 *m_tail;             //����βָ��
        uint32_t	            m_count;            //ջ�ڳ���
    public:
        //-------------------------------------------------
        //���캯��
        raw_list_t():m_head(NULL),m_tail(NULL),m_count(0){}
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
        node_t* peek(bool is_tail=false) const {return is_tail?m_tail:(node_t*)m_head;}
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
        void push_back(node_t &new_node){push_back(&new_node);}
        //-------------------------------------------------
        //�ڵ����ͷ��(����µ�ͷ�ڵ�)
        void push_front(node_t *new_node)
        {
            new_node->next=m_head;
            m_head?0:m_tail=new_node;
            m_head=new_node;
            ++m_count;
        }
        void push_front(node_t &new_node){push_front(&new_node);}
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
}

#endif
