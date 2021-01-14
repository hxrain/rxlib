#ifndef _RX_LOCK_BASE_H_
#define _RX_LOCK_BASE_H_

#include "rx_cc_macro.h"
#include "rx_assert.h"
#include "rx_cc_atomic.h"

namespace rx
{
	//------------------------------------------------------
	//ͬ�������ܶ����ͳһ�ӿ�
	class lock_i
	{
	protected:
		virtual ~lock_i() {}
	public:
		//--------------------------------------------------
		//����,Ĭ��д��
		virtual bool lock(bool is_wr_lock = true) = 0;
		//--------------------------------------------------
		//����
		virtual bool unlock() = 0;
		//--------------------------------------------------
		//��������,Ĭ��д��
		virtual bool trylock(bool is_wr_lock = true) { return false; }
	};

	//------------------------------------------------------
	//ռλ�õĿ���,ɶ������.
	class null_lock_t :public lock_i
	{
	public:
		//--------------------------------------------------
		//����
		virtual bool lock(bool is_wr_lock = true) { return true; }
		//--------------------------------------------------
		//����
		virtual bool unlock() { return true; }
		//--------------------------------------------------
		//��������
		virtual bool trylock(bool is_wr_lock = true) { return true; }
	};

	//-----------------------------------------------------
	//����ԭ�ӱ��,��װһ���򵥵�������
	class spin_lock_i :public lock_i
	{
		uint8_t     padding1[CPU_CACHELINE_SIZE / 2 - sizeof(void*)];
		int32_t     m_lock;
		uint32_t	m_max_retry;
		uint8_t     padding2[CPU_CACHELINE_SIZE / 2 - sizeof(uint32_t) * 2];
	public:
		spin_lock_i(uint32_t max_retry = 100) :m_lock(0), m_max_retry(max_retry) {}
		//--------------------------------------------------
		//����,Ĭ��д��
		bool lock(bool is_wr_lock = true)
		{
			//�������ñ��Ϊ��,������ؾ�ֵΪ��,����ζ����������.������ѭ��.
			mem_barrier();
			while (!rx_atomic_cas(m_lock, 0, 1));
			return true;
		}
		//--------------------------------------------------
		//����
		bool unlock()
		{
			mem_barrier();                                  // ��������д����
			rx_atomic_store(m_lock, 0);                     // �ͷ���
			return true;
		}
		//--------------------------------------------------
		//��������,Ĭ��д��
		bool trylock(bool is_wr_lock = true)
		{
			uint32_t lc = 0;
			while (lc < m_max_retry && !rx_atomic_cas(m_lock, 0, 1))
				++lc;
			return lc < m_max_retry;
		}
	};

	//------------------------------------------------------
	//��װһ�������������������,������������Ĺ����������Զ������������ڵ�����/����
	template<class lt, bool is_wr_lock = true>
	class guarded_t
	{
		lt  &m_locker;
		int m_flag;
	public:
		//�����ʱ�����
		guarded_t(lt &locker) :m_locker(locker), m_flag(0)
		{
			rx_check(m_locker.lock(is_wr_lock));
		}
		//�ñ�ǿ��ƽ�Ӧ��ѭ��һ��
		bool pass_one() { return 0 == m_flag++; }
		//������ʱ�����
		~guarded_t()
		{
			m_locker.unlock();
		}
	};

	//------------------------------------------------------
	//ʹ�ú궨��,����ʹ���������������ģʽ,���ڶ�д����˵��Ϊд��
	#define GUARD_T(Locker,LT) guarded_t<LT> RX_CT_SYM(_guard_)((Locker))
	#define GUARD(Locker) GUARD_T(Locker,lock_i)
	//ʹ��for���ṹ����������Χ�޶��ĺ궨���﷨��
	#define guard_t(Locker,LT) for(guarded_t<LT> RX_CT_SYM(_guard_for_)(Locker);RX_CT_SYM(_guard_for_).pass_one();)
	#define guard(Locker) guard_t(Locker,lock_i)
	//------------------------------------------------------
	//��д���У��������﷨�Ƕ���
	#define RGUARD_T(Locker,LT) guarded_t<LT,false> RX_CT_SYM(_guard_)((Locker))
	#define RGUARD(Locker) RGUARD_T(Locker,lock_i)
	//ʹ��for���ṹ����������Χ�޶��ĺ궨���﷨��
	#define rguard_t(Locker,LT) for(guarded_t<LT,false> RX_CT_SYM(_guard_)(Locker);RX_CT_SYM(_guard_).pass_one();)
	#define rguard(Locker) rguard_t(Locker,lock_i)
}


#endif