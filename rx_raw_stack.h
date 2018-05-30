#ifndef _H_RX_RAW_STACK_H_
#define _H_RX_RAW_STACK_H_
#include "rx_cc_macro.h"
#include "rx_assert.h"

namespace rx
{
    //-----------------------------------------------------
    //Raw Stack:ԭʼ��ջ,���������ʽ,���ù����ڴ�
    //Ҫ��node_t�ڵ����������ٺ���һ������ָ��next
    template<class node_t>
    class raw_stack
    {
        raw_stack& operator=(const raw_stack&);
    private:
        node_t                 *m_head;	            //ջ��ָ��
        uint32_t	            m_count;            //ջ�ڳ���
    public:
        //-------------------------------------------------
        //���캯��
        raw_stack():m_head(NULL),m_count(0){}
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
        //�鿴ջ��
        node_t* peek() const {return (node_t*)m_head;}
        //-------------------------------------------------
        //�ڵ���ջ
        void push(node_t *new_node)
        {
            new_node->next = m_head;
            m_head=new_node;
            ++m_count;
        }
        //-------------------------------------------------
        //����(�����п�ջ���,����ʹ��ǰ��Ҫ���м��)
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
