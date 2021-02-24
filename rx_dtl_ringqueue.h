#ifndef _RX_DTL_RINGQUEUE_H_
#define _RX_DTL_RINGQUEUE_H_

#include "rx_cc_macro.h"
#include "rx_ct_util.h"
#include "rx_ct_bitop.h"
#include "rx_lock_base.h"

namespace rx
{
	//-----------------------------------------------------
	//��װһ��ͨ�õ�ring queue�Ļ���,�Լ������д洢�ռ�Ĺ���,��Ҫ���༯�ɺ��ṩ
	//�����㷨�ο�Linux�ں˶���kfifo��ʵ��,ʹ�ö�������,��Ԫ���������Ϊ2��ָ����
	//���е�ͷβ�����Ĺܿ�,����"��������"��"�����"(�൱����ģ),���Ҫ������ֵΪ2��������
	template<class DT, class LT, class ST = uint32_t>
	class ringqueue_t
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
			char padding2[CPU_CACHELINE_SIZE - sizeof(size_type)]; //ͷβռ������������,��������
		} pointing_t;
		
		//-------------------------------------------------
		pointing_t          m_pointing;                     //���е�ͷβָʾ
		locker_t            m_locker;                       //����������
		item_type          *m_array;                        //�洢����Ԫ�ص���Դ����
		size_type           m_mask;                         //���ټ��㻷�λ������Ļ�������

	protected:
		//-------------------------------------------------
		//�󶨿ռ���Դ,�����ڲ���ʼ��,������������ޱ�����2��ָ����
		bool bind(item_type* array, size_type capacity)
		{
			if (m_mask)
				return false;								//�Ѿ�����

			m_pointing.head = 0;
			m_pointing.tail = 0;

			if (!rx_is_pow2(capacity))
				return false;

			//ָ����ST�����ߴ����Ͳ��޶�����������Ϊ2��ָ���ݵ������,����ֵ���ܱ��ΪST���ֵ��һ��
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
		//��ȡ���г���,����ֵ������ֵ��ͬ���������
		size_type   size() const
		{
			mem_barrier();									//�ڴ���դָ��,Ҫ����cpuͬ������
			size_type head = m_pointing.head;
			size_type tail = m_pointing.tail;
			size_type rc = (size_type)(head - tail);
			rx_assert(rc <= capacity());
			return rc;
		}
		//-------------------------------------------------
		//��ȡ��������ֵ
		size_type   capacity() const { return m_mask + 1; }
		//-------------------------------------------------
		//ʣ��ռ�
		size_type   remain() const { return capacity() - size(); }
		//-------------------------------------------------
		//�ж϶����Ƿ�Ϊ��
		bool        empty() const { return m_pointing.head == m_pointing.tail; }
		//-------------------------------------------------
		//ֱ����ն���
		void        clear() { m_pointing.head = 0; m_pointing.tail = 0; }
		//-------------------------------------------------
		//�������,����ֵ��֪�Ƿ�ɹ�
		bool push_back(item_type data)
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
		item_type *pop_front(bool is_peek = false)
		{
			GUARD_T(m_locker, locker_t);                    //��������

			size_type head = m_pointing.head;               //��ȡͷβλ��
			size_type tail = m_pointing.tail;

			if (tail == head)                               //ͷβ��ͬ˵�������ǿյ�
				return NULL;

			if (!is_peek)
				m_pointing.tail = tail + 1;                 //���ǲ鿴ģʽ,��βλ��ǰ��

			return &m_array[tail & m_mask];                 //��ȡ֮ǰβλ�ô�����
		}
	};

	//-----------------------------------------------------
	//��̬�ռ�Ļ��ζ���,��������DT;����(Ϊ2��CP�η�);��������NT;������LT.
	template<class DT, uint32_t CP = LOG2<128>::result, class LT = null_lock_t, class ST = uint32_t>
	class ringqueue_ft :public ringqueue_t<DT, LT, ST>
	{
		DT  m_items[1 << CP];                               //���������Ķ��пռ�
		typedef ringqueue_t<DT, LT, ST> superclass;
	public:
		ringqueue_ft() { rx_check(superclass::bind(m_items, ST(1 << CP))); }        //����ʱ���и���Ŀռ��
	};
}
#endif
