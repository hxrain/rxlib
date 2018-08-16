#ifndef _RX_DTL_RINGQUEUE_H_
#define _RX_DTL_RINGQUEUE_H_

#include "rx_cc_macro.h"
#include "rx_os_spinlock.h"
#include "rx_bits_op.h"


namespace rx
{
    //-----------------------------------------------------
    //��װһ��ͨ�õ�ring queue�Ļ���,�Լ������д洢�ռ�Ĺ���,��Ҫ���༯�ɺ��ṩ
    //��Ϊ�޷��������Ļ�������,���ST�������ֵ��һ����ǻ��ζ��е��������
    template<class DT, class LT,class ST=uint32_t>
    class ringqueue_base
    {
    public:
        typedef DT item_type;
        typedef LT locker_t;
        typedef ST size_type;
    private:
        //-------------------------------------------------
        //��¼���е�ͷ��β��ָ����Ϣ
        typedef struct pointing_t
        {
            volatile size_type head;                        //volatile��֪����������ֱȡ,�����Ż�
            char padding1[CPU_CACHELINE_SIZE - sizeof(size_type)];

            volatile size_type tail;
            char padding2[CPU_CACHELINE_SIZE - sizeof(size_type)];
        }pointing_t;
        //-------------------------------------------------
        pointing_t          m_pointing;                     //���е�ͷβָʾ

        locker_t            m_locker;                       //����������

        item_type          *m_array;                        //�洢����Ԫ�ص���Դ����
        size_type           m_capacity;                     //�������������(Ҫ�������2��ָ����)
        size_type           m_mask;                         //���ټ��㻷�λ������Ļ�������

    protected:
        //-------------------------------------------------
        //�󶨿ռ���Դ,�����ڲ���ʼ��
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
                return false;                               //��Ϊ�޷��������Ļ�������,���ST�������ֵ��һ����ǻ��ζ��е��������

            m_array = array;
            m_capacity = capacity;
            m_mask = m_capacity - 1;
            return true;
        }
        virtual ~ringqueue_base() {}
    public:
        ringqueue_base():m_array(NULL), m_capacity(0), m_mask(0){}
        
        //-------------------------------------------------
        //��ȡ���г���,����ֵ
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
        //��ȡ��������ֵ
        size_type   capacity() { return m_capacity; }
        //-------------------------------------------------
        //�������,����ֵ��֪�Ƿ�ɹ�
        bool push(item_type data)
        {
            GUARD_T(m_locker, locker_t);                    //��������

            size_type head = m_pointing.head;               //��ȡͷβλ��
            size_type tail = m_pointing.tail;

            if (size_type(head - tail) > m_mask)            //ͷβ���볬���޶�ֵ,˵����������
                return false;

            m_pointing.head = head + 1;                     //ͷλ�ú���,���������

            m_array[head & m_mask] = data;                  //��ԭ����ͷλ�ô����л��ƺ�,��¼��������
            return true;
        }
        //-------------------------------------------------
        //���ݳ���,����ֵNULL˵�������ǿյ�
        item_type *pop(bool is_peek=false)
        {
            GUARD_T(m_locker, locker_t);                    //��������

            size_type head = m_pointing.head;               //��ȡͷβλ��
            size_type tail = m_pointing.tail;

            if (tail == head)                               //ͷβ��ͬ˵�������ǿյ�
                return NULL;

            if (tail > head && (head - tail) > m_mask)
            {
                rx_show_msg("(tail > head && (head - tail) > m_mask) == true");
                return NULL;
            }

            if (!is_peek)
                m_pointing.tail = tail + 1;                 //���ǲ鿴ģʽ,��βλ��ǰ��

            return &m_array[tail & m_mask];                 //��ȡ֮ǰβλ�ô�����
        }
    };

    //-----------------------------------------------------
    //��̬�ռ�Ļ��ζ���,��������DT;����(1<<CP);��������NT;������LT.
    template<class DT,uint32_t CP=8, class LT = null_lock_t,class ST=uint32_t>
    class ringqueue_fixed :public ringqueue_base<DT, LT, ST>
    {
        DT  m_items[1<<CP];                                 //���������Ķ��пռ�
    public:
        ringqueue_fixed() { rx_check(m_init(m_items,uint32_t(1<<CP))); }        //����ʱ���г�ʼ��
    };
}







#endif