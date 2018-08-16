#ifndef _RX_DTL_RINGQUEUE_H_
#define _RX_DTL_RINGQUEUE_H_

#include "rx_cc_macro.h"
#include "rx_os_spinlock.h"
#include "rx_bits_op.h"


namespace rx
{
    //-----------------------------------------------------
    //封装一个通用的ring queue的基类,自己不进行存储空间的管理,需要子类集成后提供
    //因为无符号整数的回绕问题,因此ST类型最大值的一半就是环形队列的最大容量
    template<class DT, class LT,class ST=uint32_t>
    class ringqueue_base
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
            char padding2[CPU_CACHELINE_SIZE - sizeof(size_type)];
        }pointing_t;
        //-------------------------------------------------
        pointing_t          m_pointing;                     //队列的头尾指示

        locker_t            m_locker;                       //并发控制锁

        item_type          *m_array;                        //存储队列元素的资源数组
        size_type           m_capacity;                     //数组的能力上限(要求必须是2的指数倍)
        size_type           m_mask;                         //快速计算环形缓冲区的回绕掩码

    protected:
        //-------------------------------------------------
        //绑定空间资源,进行内部初始化
        bool m_init(item_type* array, size_type capacity)
        {
            if (m_capacity)
                return false;

            m_pointing.head = 0;
            m_pointing.tail = 0;
            m_mask = 0;

            if (!rx_is_pow2(capacity))
                return false;

            if (capacity > (uint64_t(1) << (sizeof(size_type)*8-1)))
                return false;                               //因为无符号整数的回绕问题,因此ST类型最大值的一半就是环形队列的最大容量

            m_array = array;
            m_capacity = capacity;
            m_mask = m_capacity - 1;
            return true;
        }
        virtual ~ringqueue_base() {}
    public:
        ringqueue_base():m_array(NULL), m_capacity(0), m_mask(0){}
        
        //-------------------------------------------------
        //获取队列长度,返回值
        size_type   size()
        {
            mem_barrier();

            size_type head = m_pointing.head;
            size_type tail = m_pointing.tail;
            size_type rc = (size_type)(head - tail);
            rx_assert(rc<= m_capacity);
            return (rc <= m_mask) ? rc : m_capacity;
        }
        //-------------------------------------------------
        //获取队列能力值
        size_type   capacity() { return m_capacity; }
        //-------------------------------------------------
        //数据入队,返回值告知是否成功
        bool push(item_type data)
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
        item_type *pop(bool is_peek=false)
        {
            GUARD_T(m_locker, locker_t);                    //进行锁定

            size_type head = m_pointing.head;               //获取头尾位置
            size_type tail = m_pointing.tail;

            if (tail == head)                               //头尾相同说明队列是空的
                return NULL;

            if (tail > head && (head - tail) > m_mask)
            {
                rx_show_msg("(tail > head && (head - tail) > m_mask) == true");
                return NULL;
            }

            if (!is_peek)
                m_pointing.tail = tail + 1;                 //不是查看模式,则尾位置前移

            return &m_array[tail & m_mask];                 //获取之前尾位置处数据
        }
    };

    //-----------------------------------------------------
    //静态空间的环形队列,数据类型DT;容量(1<<CP);数字类型NT;锁类型LT.
    template<class DT,uint32_t CP=8, class LT = null_lock_t,class ST=uint32_t>
    class ringqueue_fixed :public ringqueue_base<DT, LT, ST>
    {
        DT  m_items[1<<CP];                                 //定义真正的队列空间
    public:
        ringqueue_fixed() { rx_check(m_init(m_items,uint32_t(1<<CP))); }        //构造时进行初始化
    };
}







#endif