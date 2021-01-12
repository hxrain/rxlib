#ifndef _H_RX_RAW_STACK_H_
#define _H_RX_RAW_STACK_H_
#include "rx_cc_macro.h"
#include "rx_assert.h"

/**���ļ���װ��������������ĺ��Ĺ���

	//-----------------------------------------------------
	//ʹ�ô�ԭʼstack��ʱ��,Ӧ�ù������µĽڵ�����(���Դ˽ڵ�Ϊ����)
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
	//-----------------------------------------------------
	//ԭʼ��ջ(����list,���ɷ���ͷ),���������ʽ,�������ڴ�
	//Ҫ��node_t�ڵ����������ٺ���һ������ָ��next
	template<class node_t>
	class raw_stack_t
	{
	private:
		raw_stack_t& operator=(const raw_stack_t&);
		node_t                 *m_head;	            //ջ��ָ��
		uint32_t	            m_count;            //ջ�ڳ���
	public:
		//-------------------------------------------------
		//���캯��
		raw_stack_t() :m_head(NULL), m_count(0) {}
		//-------------------------------------------------
		//ջ��Ԫ������
		uint32_t size() const { return m_count; }
		//-------------------------------------------------
		//��ջ�е�Ԫ��ȫ��ժ��,�õ�һ����������
		node_t* pick()
		{
			node_t *now_top = m_head;
			m_head = NULL;
			m_count = 0;
			return now_top;
		}
		//-------------------------------------------------
		//�鿴ջ��(����ͷ)
		node_t* head() const { return m_head; }
		//-------------------------------------------------
		//�ڵ���ջ(��ͷ�ڵ�ǰ����,����µ�ͷ)
		void push_front(node_t *new_node)
		{
			new_node->next = m_head;
			m_head = new_node;
			++m_count;
		}
		void push_front(node_t &new_node) { push_front(&new_node); }
		//-------------------------------------------------
		//����(ͷ���ժ��,������Ϊ�½ڵ�)
		//(�����п�ջ���,����ʹ��ǰ��Ҫ���м��)
		node_t* pop_front()
		{
			rx_assert(m_count != 0);
			node_t *now_top = m_head;
			m_head = m_head->next;
			--m_count;
			return now_top;
		}
		//-------------------------------------------------
	};

	//-----------------------------------------------------
	//ԭʼ��queue(����list,�ɷ���ͷβ;Ҳ���Ե���stackʹ��),���������ʽ,���ù����ڴ�
	//Ҫ��node_t�ڵ��������躬��һ������ָ��next
	template<class node_t>
	class raw_queue_t
	{
	private:
		raw_queue_t& operator=(const raw_queue_t&);

		//�����ڲ�ʹ�õ�ԭ��ڵ�
		typedef struct origin_t
		{
			node_t             *tail;	                    //ָ��β�ڵ�
			node_t             *next;                       //ָ��ͷ���
			void reset() { tail = (node_t*)this; next = (node_t*)this; }
		}origin_t;

		uint32_t	            m_count;                    //ջ�ڳ���
		origin_t                m_origin;                   //�ڲ���ԭ�����
	public:
		//-------------------------------------------------
		//���캯��
		raw_queue_t() :m_count(0) { m_origin.reset(); }
		//-------------------------------------------------
		//Ԫ������
		uint32_t size() const { return m_count; }
		//-------------------------------------------------
		//��Ԫ��ȫ��ժ��,�õ�һ����������
		node_t* pick()
		{
			node_t *now_top = m_origin.next;
			m_origin.reset();
			m_count = 0;
			return now_top;
		}
		//-------------------------------------------------
		//�鿴ͷ����β�ڵ�(��Ӧֱ��ʹ��,���ж��Ƿ�Ϊ������,�Ƿ���origin()��ͬ)
		node_t* head() const { return m_origin.next; }
		node_t* tail() const { return m_origin.tail; }
		node_t* origin() const { return (node_t*)&m_origin; }
		//-------------------------------------------------
		//�ڵ����β��(����µ�β�ڵ�)
		void push_back(node_t *new_node)
		{
			m_origin.tail->next = new_node;                 //β�ڵ�ĺ���ָ���½ڵ�
			new_node->next = (node_t*)&m_origin;            //�½ڵ����ָ��ԭ��
			m_origin.tail = new_node;                       //βָ��ָ���½ڵ�
			if (++m_count == 1)
				m_origin.next = new_node;                   //�������Ϊ��ʱ��ͷָ��ָ�����½ڵ�
		}
		void push_back(node_t &new_node) { push_back(&new_node); }
		//-------------------------------------------------
		//�ڵ����ͷ��(����µ�ͷ�ڵ�)
		void push_front(node_t *new_node)
		{
			new_node->next = m_origin.next;                 //�½ڵ�ĺ���ָ��ԭ��ĺ���
			m_origin.next = new_node;                       //ԭ��ĺ���ָ���½ڵ�
			if (++m_count == 1)
				m_origin.tail = new_node;                   //�������Ϊ��ʱ��βָ��ָ�����½ڵ�
		}
		void push_front(node_t &new_node) { push_front(&new_node); }
		//-------------------------------------------------
		//�ڵ㵯��(ͷ���ժ��,������Ϊ��ͷ)
		//(�����п�ջ���,����ʹ��ǰ��Ҫ���м��)
		node_t* pop_front()
		{
			rx_assert(m_count != 0);
			node_t *node = head();                          //��¼ԭ����ͷ���,׼������
			m_origin.next = node->next;                     //ͷָ�����
			if (--m_count == 0)
				m_origin.tail = (node_t*)&m_origin;         //�������ʱ��βָ��ָ������
			rx_assert_if(m_count == 0, m_origin.next == (node_t*)&m_origin && m_origin.tail == (node_t*)&m_origin);
			return node;
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
		//�����ڲ�ʹ�õ�ԭ��ڵ�
		typedef struct origin_t
		{
			node_t             *prev;	                    //ԭ���ǰ��,ָ��β�ڵ�
			node_t             *next;                       //ԭ��ĺ���,ָ��ͷ���
			void reset()
			{
				prev = (node_t*)this;
				next = (node_t*)this;
			}
		}origin_t;

		//-------------------------------------------------
		uint32_t	            m_count;                    //��������
		origin_t                m_origin;                   //�ڲ���ԭ�����

		//-------------------------------------------------
		//���ڵ�node�ҽӵ�prev��next֮��
		template<class na_t, class nb_t, class nc_t>
		static void join(na_t *node, nb_t *prev, nc_t *next)
		{
			next->prev = (node_t*)node;
			node->next = (node_t*)next;
			node->prev = (node_t*)prev;
			prev->next = (node_t*)node;
		}

		//-------------------------------------------------
		//ժ��prev��next֮����Ǹ��ڵ�
		template<class na_t, class nb_t>
		static void pick(na_t *prev, nb_t *next)
		{
			next->prev = prev;
			prev->next = next;
		}
	public:
		//-------------------------------------------------
		//���캯��
		raw_list_t() :m_count(0) { m_origin.reset(); }
		//-------------------------------------------------
		//Ԫ������
		uint32_t size() const { return m_count; }
		//-------------------------------------------------
		//�鿴ͷ����β�ڵ�(��Ӧֱ��ʹ��,���ж��Ƿ�Ϊ������,�Ƿ���origin()��ͬ)
		node_t* head() const { return m_origin.next; }
		node_t* tail() const { return m_origin.prev; }
		node_t* origin() const { return (node_t*)&m_origin; }
		//-------------------------------------------------
		//�ڵ����β��(����µ�β�ڵ�)
		void push_back(node_t *new_node)
		{
			rx_assert(new_node != NULL);
			join(new_node, tail(), &m_origin);              //��β�ڵ������½ڵ�
			++m_count;
		}
		void push_back(node_t &new_node) { push_back(&new_node); }
		//-------------------------------------------------
		//���½ڵ�new_node�ҽӵ�ָ���ڵ�prev�ĺ���
		void push_back(const node_t *prev, node_t *new_node)
		{
			rx_assert(prev != NULL && new_node != NULL);
			join(new_node, prev, prev->next);               //�ڸ����ڵ������½ڵ�
			++m_count;
		}
		//-------------------------------------------------
		//�ڵ����ͷ��(����µ�ͷ�ڵ�)
		void push_front(node_t *new_node)
		{
			rx_assert(new_node != NULL);
			join(new_node, &m_origin, m_origin.next);       //��ԭ����ͷ�ڵ�֮������½ڵ�
			++m_count;
		}
		void push_front(node_t &new_node) { push_front(&new_node); }
		//-------------------------------------------------
		//���½ڵ�new_node�ҽӵ�ָ���ڵ�next��ǰ��
		void push_front(const node_t *next, node_t *new_node)
		{
			rx_assert(new_node != NULL&&next != NULL);
			join(new_node, next->prev, next);               //�ڸ����ڵ�ǰ�����½ڵ�
			++m_count;
		}
		//-------------------------------------------------
		//ժ��β���Ľڵ�
		//����ֵ:ժ���Ľڵ�
		node_t* pop_back()
		{
			rx_assert(m_count != 0);
			node_t *node = m_origin.prev;                   //��¼ԭ����β���,׼������
			pick(node->prev, node->next);                   //ժ����ǰ�ڵ�
			--m_count;
			rx_assert_if(m_count == 0, m_origin.prev == &m_origin && m_origin.next == &m_origin);
			return node;
		}
		//-------------------------------------------------
		//ժ��ͷ���Ľڵ�
		//����ֵ:ժ���Ľڵ�
		node_t* pop_front()
		{
			rx_assert(m_count != 0);
			node_t *node = m_origin.next;                   //��¼ԭ����ͷ���,׼������
			pick(node->prev, node->next);                   //ժ����ǰ�ڵ�
			--m_count;
			rx_assert_if(m_count == 0, m_origin.prev == (node_t*)&m_origin && m_origin.next == (node_t*)&m_origin);
			return node;
		}
		//-------------------------------------------------
		//��ָ���Ľڵ��������ժ��
		//����ָ���ڵ�ĺ���
		node_t *pick(node_t *node)
		{
			rx_assert(m_count != 0 && node != NULL);
			--m_count;
			pick(node->prev, node->next);                   //ժ�������ڵ�
			return node->next;
		}
		//-------------------------------------------------
		//��Ԫ��ȫ��ժ��
		//��������ͷ�ڵ�
		node_t* pick()
		{
			node_t *node = m_origin.next;
			m_origin.reset();
			m_count = 0;
			return node;
		}
	};
}

#endif
