#ifndef _HX_TASK_DISP_H_
#define _HX_TASK_DISP_H_

#include "rx_cc_macro.h"
#include "rx_os_sem.h"
#include "rx_os_thread.h"
#include <queue>

/*
//---------------------------------------------------------
//��������������в���
class my_test_disp_task:public task_t
{
protected:
	uint32_t on_run(void* param)
	{
		uint64_t data=(uint64_t)param;
		printf("task_disp [%ld] [%ld]\n",threadId(),data);
		Sleep(500);
		return 0;
	}
};

inline void test_my_disp_task()
{
	task_disp_t<my_test_disp_task,2> my_disp;
	hx_assert(my_disp.init(),"task_disp_t<my_task,2>.init()");

	int ipt=1;
	while(ipt)
	{
		printf("task_disp���в���,��ѡ�����²���:\n"
			   "1 - push ;\n"
			   "2 - stop ;\n");
		ipt=getchar();
		switch(ipt)
		{
		case '1':
			my_disp.push((void*)time(NULL));
			break;
		case '2':
			my_disp.uninit();
			ipt=0;
			break;
		}
	}
}
*/

namespace rx
{
	//-----------------------------------------------------
	//ͬ�����������(û�п��������ߵ�ʱ��,������Ҳ�ᱻ����,����������й����ѻ�)
	//���ú���������,�����������ݺ�,�ڲ��ؾͻỽ��һ���߳�ȡ�����ݲ�����
	template<class task_class, uint32_t thread_count>
	class task_disp_t
	{
		//-------------------------------------------------
		//�ⲿ��������װ,��ת�����������ⲿָ������
		class task_proxy_t :public task_class
		{
		protected:
			//---------------------------------------------
			//�߳�����������
			virtual void on_begin(void* param)
			{//param����thread_t.start(param),������Ϊtask_disp_t��ʵ��thisָ��
				m_parent = static_cast<task_disp_t*>(param);
				task_class::on_begin(m_parent->m_task_init_param);
			}
			//---------------------------------------------
			//�߳�����������������
			virtual void on_end(void* param)
			{ //param����thread_t.start(param),������Ϊtask_disp_t��ʵ��thisָ��
				m_parent = static_cast<task_disp_t*>(param);
				task_class::on_end(m_parent->m_task_init_param);
			}
			//---------------------------------------------
			//���������ݶ�������ȡһ������,׼�����ȴ���
			void* m_get_data()
			{
				GUARD(m_parent->m_data_locker);
				//��������ͬ����,�õ���������
				rx_assert(m_parent->m_data_queue.size() != 0);
				void* data = m_parent->m_data_queue.front();
				m_parent->m_data_queue.pop();
				return data;
			}
			//---------------------------------------------
			//�����߳�����������
			virtual uint32_t on_run(void* param)
			{
				while (m_parent->m_sem_work.take())
				{//���񱻻���,����һ�����ݴ���
					if (task_class::need_break())
						break;                              //�ж��Ƿ�Ҫ����Ƚ���
					task_class::on_run(m_get_data());       //�������������ݴ�������
					m_parent->m_sem_idle.post();            //��������ɺ�,֪ͨ���м�����
				}
				return 0;
			}
			//---------------------------------------------
			task_disp_t    *m_parent;                       //ָ�������,���ʹ�������
		public:
			task_proxy_t() : m_parent(NULL) {}
		};

		//-------------------------------------------------
		//�����������̶߳���
		typedef struct worker_t
		{
			task_proxy_t    task_proxy;                     //���������е���������װ����ʵ��
			thread_t        task_thread;                    //��������Ķ����߳�
			worker_t() : task_thread(task_proxy) {}
		} worker_t;

		//-------------------------------------------------
		typedef std::queue<void*> data_queue_t;

		//-------------------------------------------------
		worker_t        m_workers[thread_count];            //��������worker����
		semp_t          m_sem_idle;                         //�������������
		semp_t          m_sem_work;                         //�������������
		data_queue_t    m_data_queue;                       //���ݹ������ݵĶ���
		locker_t        m_data_locker;                      //���ݶ�������ʹ�õ�ͬ����
		void           *m_task_init_param;                  //�����߳�����ʱ�ĳ�ʼ������,����init(init_param).
	public:
		//-------------------------------------------------
		task_disp_t() :m_task_init_param(NULL) {}
		~task_disp_t() { uninit(); }
		//-------------------------------------------------
		//������������г�ʼ��,�ɸ�֪�����̳߳�ʼ����ʹ�õ�init_param����
		bool init(void* init_param = NULL)
		{
			m_task_init_param = init_param;

			if (!m_sem_idle.init(thread_count))
				return false;

			if (!m_sem_work.init(0))
				return false;

			for (uint32_t i = 0; i < thread_count; ++i)
			{
				if (!m_workers[i].task_thread.start(this))
					return false;
			}
			return true;
		}
		//-------------------------------------------------
		//������������
		void uninit()
		{
			for (uint32_t i = 0; i < thread_count; ++i)
				m_workers[i].task_proxy.halt();             //�����е�������,��������

			for (uint32_t i = 0; i < thread_count; ++i)
				m_sem_work.post();                          //�Թ��������������ύ,�������е�����

			for (uint32_t i = 0; i < thread_count; ++i)
				m_workers[i].task_thread.stop(true);        //�ȴ����еĹ����߳̽���

			m_sem_work.uninit();                            //�������������
			m_sem_idle.uninit();                            //������м�����
		}
		//-------------------------------------------------
		//������������,�ȴ����ȴ���
		bool push(void* data, int wait_ms = -1)
		{
			//�ȵȴ�������������,��ʱû�о��˳�
			if (!m_sem_idle.take(wait_ms))
				return false;

			//��������������������,������ͬ��
			{
				GUARD(m_data_locker);
				m_data_queue.push(data);
			}

			//����һ����������
			return m_sem_work.post();
		}
		//-------------------------------------------------
		//������uninit֮���ȡû���ü����������������,�������ٴ���
		//����ֵ:NULLû����.����Ϊ֮ǰpush��data.
		void *remain()
		{
			if (m_data_queue.size() == 0)
				return NULL;
			void *data = m_data_queue.front();
			m_data_queue.pop();
			return data;
		}
	};
}


#endif // _HX_TASK_DISP_H_
