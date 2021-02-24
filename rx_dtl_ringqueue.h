#ifndef _RX_DTL_RINGQUEUE_H_
#define _RX_DTL_RINGQUEUE_H_

#include "rx_cc_macro.h"
#include "rx_ct_util.h"
#include "rx_ct_bitop.h"
#include "rx_lock_base.h"

namespace rx
{
	//-----------------------------------------------------
	//封装一个通用的ring queue的基类,自己不进行存储空间的管理,需要子类集成后提供
	//队列算法参考Linux内核队列kfifo的实现,使用定长数组,且元素容量最大为2的指数幂
	//队列的头尾索引的管控,利用"能力掩码"的"与操作"(相当于求模),因此要求能力值为2的整数幂
	template<class DT, class LT, class ST = uint32_t>
	class ringqueue_t
	{
	public:
		typedef DT item_type;
		typedef LT locker_t;
		typedef ST size_type;
	private:
		//-------------------------------------------------
		//记录队列的头与尾的指向信息
		typedef struct pointing_t
		{
			volatile size_type head;                        //volatile告知编译器必须直取,不可优化
			char padding1[CPU_CACHELINE_SIZE - sizeof(size_type)];

			volatile size_type tail;
			char padding2[CPU_CACHELINE_SIZE - sizeof(size_type)]; //头尾占用两个缓存线,互不干扰
		} pointing_t;
		
		//-------------------------------------------------
		pointing_t          m_pointing;                     //队列的头尾指示
		locker_t            m_locker;                       //并发控制锁
		item_type          *m_array;                        //存储队列元素的资源数组
		size_type           m_mask;                         //快速计算环形缓冲区的回绕掩码

	protected:
		//-------------------------------------------------
		//绑定空间资源,进行内部初始化,数组的能力上限必须是2的指数幂
		bool bind(item_type* array, size_type capacity)
		{
			if (m_mask)
				return false;								//已经绑定了

			m_pointing.head = 0;
			m_pointing.tail = 0;

			if (!rx_is_pow2(capacity))
				return false;

			//指定了ST能力尺寸类型并限定了能力必须为2的指数幂的情况下,能力值仅能表达为ST最大值的一半
			rx_assert(capacity <= (uint64_t(1) << (sizeof(size_type) * 8 - 1)));

			m_array = array;
			m_mask = capacity - 1;
			return true;
		}
		virtual ~ringqueue_t() {}
	public:
		//-------------------------------------------------
		ringqueue_t() :m_array(NULL), m_mask(0) { rx_static_assert(sizeof(pointing_t) == CPU_CACHELINE_SIZE*2); }
		//-------------------------------------------------
		//获取队列长度,返回值与能力值相同则队列满了
		size_type   size() const
		{
			mem_barrier();									//内存栏栅指令,要求多核cpu同步缓存
			size_type head = m_pointing.head;
			size_type tail = m_pointing.tail;
			size_type rc = (size_type)(head - tail);
			rx_assert(rc <= capacity());
			return rc;
		}
		//-------------------------------------------------
		//获取队列能力值
		size_type   capacity() const { return m_mask + 1; }
		//-------------------------------------------------
		//剩余空间
		size_type   remain() const { return capacity() - size(); }
		//-------------------------------------------------
		//判断队列是否为空
		bool        empty() const { return m_pointing.head == m_pointing.tail; }
		//-------------------------------------------------
		//直接清空队列
		void        clear() { m_pointing.head = 0; m_pointing.tail = 0; }
		//-------------------------------------------------
		//数据入队,返回值告知是否成功
		bool push_back(item_type data)
		{
			GUARD_T(m_locker, locker_t);                    //进行锁定

			size_type head = m_pointing.head;               //获取头尾位置
			size_type tail = m_pointing.tail;

			if (size_type(head - tail) > m_mask)            //头尾距离超过限定值,说明队列满了
				return false;

			m_pointing.head = head + 1;                     //头位置后移,不处理回绕

			m_array[head & m_mask] = data;                  //在原来的头位置处进行回绕后,记录本次数据
			return true;
		}
		//-------------------------------------------------
		//数据出队,返回值NULL说明队列是空的
		item_type *pop_front(bool is_peek = false)
		{
			GUARD_T(m_locker, locker_t);                    //进行锁定

			size_type head = m_pointing.head;               //获取头尾位置
			size_type tail = m_pointing.tail;

			if (tail == head)                               //头尾相同说明队列是空的
				return NULL;

			if (!is_peek)
				m_pointing.tail = tail + 1;                 //不是查看模式,则尾位置前移

			return &m_array[tail & m_mask];                 //获取之前尾位置处数据
		}
	};

	//-----------------------------------------------------
	//静态空间的环形队列,数据类型DT;容量(为2的CP次方);数字类型NT;锁类型LT.
	template<class DT, uint32_t CP = LOG2<128>::result, class LT = null_lock_t, class ST = uint32_t>
	class ringqueue_ft :public ringqueue_t<DT, LT, ST>
	{
		DT  m_items[1 << CP];                               //定义真正的队列空间
		typedef ringqueue_t<DT, LT, ST> superclass;
	public:
		ringqueue_ft() { rx_check(superclass::bind(m_items, ST(1 << CP))); }        //构造时进行父类的空间绑定
	};
}
#endif
