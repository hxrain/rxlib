#ifndef _H_RX_RAW_STACK_H_
#define _H_RX_RAW_STACK_H_
#include "rx_cc_macro.h"
#include "rx_assert.h"

/**本文件封装了链表基础容器的核心功能

	//-----------------------------------------------------
	//使用此原始stack的时候,应该构造如下的节点类型(或以此节点为基类)
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
	//-----------------------------------------------------
	//原始的栈(或单向list,仅可访问头),保持最简形式,不管理内存
	//要求node_t节点类型中至少含有一个后趋指针next
	template<class node_t>
	class raw_stack_t
	{
	private:
		raw_stack_t& operator=(const raw_stack_t&);
		node_t                 *m_head;	            //栈顶指针
		uint32_t	            m_count;            //栈内长度
	public:
		//-------------------------------------------------
		//构造函数
		raw_stack_t() :m_head(NULL), m_count(0) {}
		//-------------------------------------------------
		//栈内元素数量
		uint32_t size() const { return m_count; }
		//-------------------------------------------------
		//将栈中的元素全部摘除,得到一个单向链表
		node_t* pick()
		{
			node_t *now_top = m_head;
			m_head = NULL;
			m_count = 0;
			return now_top;
		}
		//-------------------------------------------------
		//查看栈顶(链表头)
		node_t* head() const { return m_head; }
		//-------------------------------------------------
		//节点入栈(在头节点前插入,变成新的头)
		void push_front(node_t *new_node)
		{
			new_node->next = m_head;
			m_head = new_node;
			++m_count;
		}
		void push_front(node_t &new_node) { push_front(&new_node); }
		//-------------------------------------------------
		//弹出(头结点摘除,后趋变为新节点)
		//(不进行空栈检测,外面使用前需要进行检查)
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
	//原始的queue(或单向list,可访问头尾;也可以当作stack使用),保持最简形式,不用管理内存
	//要求node_t节点类型中需含有一个后趋指针next
	template<class node_t>
	class raw_queue_t
	{
	private:
		raw_queue_t& operator=(const raw_queue_t&);

		//定义内部使用的原点节点
		typedef struct origin_t
		{
			node_t             *tail;	                    //指向尾节点
			node_t             *next;                       //指向头结点
			void reset() { tail = (node_t*)this; next = (node_t*)this; }
		}origin_t;

		uint32_t	            m_count;                    //栈内长度
		origin_t                m_origin;                   //内部的原点对象
	public:
		//-------------------------------------------------
		//构造函数
		raw_queue_t() :m_count(0) { m_origin.reset(); }
		//-------------------------------------------------
		//元素数量
		uint32_t size() const { return m_count; }
		//-------------------------------------------------
		//将元素全部摘除,得到一个单向链表
		node_t* pick()
		{
			node_t *now_top = m_origin.next;
			m_origin.reset();
			m_count = 0;
			return now_top;
		}
		//-------------------------------------------------
		//查看头结点和尾节点(不应直接使用,需判断是否为空容器,是否与origin()相同)
		node_t* head() const { return m_origin.next; }
		node_t* tail() const { return m_origin.tail; }
		node_t* origin() const { return (node_t*)&m_origin; }
		//-------------------------------------------------
		//节点挂入尾部(变成新的尾节点)
		void push_back(node_t *new_node)
		{
			m_origin.tail->next = new_node;                 //尾节点的后趋指向新节点
			new_node->next = (node_t*)&m_origin;            //新节点后趋指向原点
			m_origin.tail = new_node;                       //尾指针指向新节点
			if (++m_count == 1)
				m_origin.next = new_node;                   //最初容器为空时则头指针指向最新节点
		}
		void push_back(node_t &new_node) { push_back(&new_node); }
		//-------------------------------------------------
		//节点挂入头部(变成新的头节点)
		void push_front(node_t *new_node)
		{
			new_node->next = m_origin.next;                 //新节点的后趋指向原点的后趋
			m_origin.next = new_node;                       //原点的后趋指向新节点
			if (++m_count == 1)
				m_origin.tail = new_node;                   //最初容器为空时则尾指针指向最新节点
		}
		void push_front(node_t &new_node) { push_front(&new_node); }
		//-------------------------------------------------
		//节点弹出(头结点摘除,后趋变为新头)
		//(不进行空栈检测,外面使用前需要进行检查)
		node_t* pop_front()
		{
			rx_assert(m_count != 0);
			node_t *node = head();                          //记录原来的头结点,准备返回
			m_origin.next = node->next;                     //头指针后移
			if (--m_count == 0)
				m_origin.tail = (node_t*)&m_origin;         //容器变空时则尾指针指向自身
			rx_assert_if(m_count == 0, m_origin.next == (node_t*)&m_origin && m_origin.tail == (node_t*)&m_origin);
			return node;
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
		//定义内部使用的原点节点
		typedef struct origin_t
		{
			node_t             *prev;	                    //原点的前趋,指向尾节点
			node_t             *next;                       //原点的后趋,指向头结点
			void reset()
			{
				prev = (node_t*)this;
				next = (node_t*)this;
			}
		}origin_t;

		//-------------------------------------------------
		uint32_t	            m_count;                    //容器长度
		origin_t                m_origin;                   //内部的原点对象

		//-------------------------------------------------
		//将节点node挂接到prev与next之间
		template<class na_t, class nb_t, class nc_t>
		static void join(na_t *node, nb_t *prev, nc_t *next)
		{
			next->prev = (node_t*)node;
			node->next = (node_t*)next;
			node->prev = (node_t*)prev;
			prev->next = (node_t*)node;
		}

		//-------------------------------------------------
		//摘除prev与next之间的那个节点
		template<class na_t, class nb_t>
		static void pick(na_t *prev, nb_t *next)
		{
			next->prev = prev;
			prev->next = next;
		}
	public:
		//-------------------------------------------------
		//构造函数
		raw_list_t() :m_count(0) { m_origin.reset(); }
		//-------------------------------------------------
		//元素数量
		uint32_t size() const { return m_count; }
		//-------------------------------------------------
		//查看头结点和尾节点(不应直接使用,需判断是否为空容器,是否与origin()相同)
		node_t* head() const { return m_origin.next; }
		node_t* tail() const { return m_origin.prev; }
		node_t* origin() const { return (node_t*)&m_origin; }
		//-------------------------------------------------
		//节点挂入尾部(变成新的尾节点)
		void push_back(node_t *new_node)
		{
			rx_assert(new_node != NULL);
			join(new_node, tail(), &m_origin);              //在尾节点后插入新节点
			++m_count;
		}
		void push_back(node_t &new_node) { push_back(&new_node); }
		//-------------------------------------------------
		//将新节点new_node挂接到指定节点prev的后面
		void push_back(const node_t *prev, node_t *new_node)
		{
			rx_assert(prev != NULL && new_node != NULL);
			join(new_node, prev, prev->next);               //在给定节点后插入新节点
			++m_count;
		}
		//-------------------------------------------------
		//节点挂入头部(变成新的头节点)
		void push_front(node_t *new_node)
		{
			rx_assert(new_node != NULL);
			join(new_node, &m_origin, m_origin.next);       //在原点与头节点之间插入新节点
			++m_count;
		}
		void push_front(node_t &new_node) { push_front(&new_node); }
		//-------------------------------------------------
		//将新节点new_node挂接到指定节点next的前面
		void push_front(const node_t *next, node_t *new_node)
		{
			rx_assert(new_node != NULL&&next != NULL);
			join(new_node, next->prev, next);               //在给定节点前插入新节点
			++m_count;
		}
		//-------------------------------------------------
		//摘除尾部的节点
		//返回值:摘除的节点
		node_t* pop_back()
		{
			rx_assert(m_count != 0);
			node_t *node = m_origin.prev;                   //记录原来的尾结点,准备返回
			pick(node->prev, node->next);                   //摘除当前节点
			--m_count;
			rx_assert_if(m_count == 0, m_origin.prev == &m_origin && m_origin.next == &m_origin);
			return node;
		}
		//-------------------------------------------------
		//摘除头部的节点
		//返回值:摘除的节点
		node_t* pop_front()
		{
			rx_assert(m_count != 0);
			node_t *node = m_origin.next;                   //记录原来的头结点,准备返回
			pick(node->prev, node->next);                   //摘除当前节点
			--m_count;
			rx_assert_if(m_count == 0, m_origin.prev == (node_t*)&m_origin && m_origin.next == (node_t*)&m_origin);
			return node;
		}
		//-------------------------------------------------
		//将指定的节点从链表中摘除
		//返回指定节点的后趋
		node_t *pick(node_t *node)
		{
			rx_assert(m_count != 0 && node != NULL);
			--m_count;
			pick(node->prev, node->next);                   //摘除给定节点
			return node->next;
		}
		//-------------------------------------------------
		//将元素全部摘除
		//返回链表头节点
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
