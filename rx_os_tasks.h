#ifndef _HX_TASK_DISP_H_
#define _HX_TASK_DISP_H_

#include "rx_cc_macro.h"
#include "rx_os_sem.h"
#include "rx_os_thread.h"
#include <queue>

/*
//---------------------------------------------------------
//对任务调度器进行测试
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
		printf("task_disp进行测试,请选择以下操作:\n"
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
	//同步任务调度器(没有空闲消费者的时候,生产者也会被阻塞,避免任务队列过量堆积)
	//配置好任务类型,推送任务数据后,内部池就会唤醒一个线程取出数据并处理
	template<class task_class, uint32_t thread_count>
	class task_disp_t
	{
		//-------------------------------------------------
		//外部任务代理封装,用转发任务动作给外部指定的类
		class task_proxy_t :public task_class
		{
		protected:
			//---------------------------------------------
			//线程任务启动了
			virtual void on_begin(void* param)
			{//param来自thread_t.start(param),本例程为task_disp_t的实例this指针
				m_parent = static_cast<task_disp_t*>(param);
				task_class::on_begin(m_parent->m_task_init_param);
			}
			//---------------------------------------------
			//线程任务主函数结束了
			virtual void on_end(void* param)
			{ //param来自thread_t.start(param),本例程为task_disp_t的实例this指针
				m_parent = static_cast<task_disp_t*>(param);
				task_class::on_end(m_parent->m_task_init_param);
			}
			//---------------------------------------------
			//从任务数据队列中提取一个数据,准备调度处理
			void* m_get_data()
			{
				GUARD(m_parent->m_data_locker);
				//锁定队列同步锁,得到队首数据
				rx_assert(m_parent->m_data_queue.size() != 0);
				void* data = m_parent->m_data_queue.front();
				m_parent->m_data_queue.pop();
				return data;
			}
			//---------------------------------------------
			//进入线程任务主函数
			virtual uint32_t on_run(void* param)
			{
				while (m_parent->m_sem_work.take())
				{//任务被唤醒,进行一次数据处理
					if (task_class::need_break())
						break;                              //判断是否要求调度结束
					task_class::on_run(m_get_data());       //进行真正的数据处理任务
					m_parent->m_sem_idle.post();            //任务处理完成后,通知空闲计数器
				}
				return 0;
			}
			//---------------------------------------------
			task_disp_t    *m_parent;                       //指向调度器,访问公共对象
		public:
			task_proxy_t() : m_parent(NULL) {}
		};

		//-------------------------------------------------
		//定义任务与线程对象
		typedef struct worker_t
		{
			task_proxy_t    task_proxy;                     //真正待运行的任务的外包装代理实例
			thread_t        task_thread;                    //运行任务的独立线程
			worker_t() : task_thread(task_proxy) {}
		} worker_t;

		//-------------------------------------------------
		typedef std::queue<void*> data_queue_t;

		//-------------------------------------------------
		worker_t        m_workers[thread_count];            //定义任务worker数组
		semp_t          m_sem_idle;                         //空闲任务计数器
		semp_t          m_sem_work;                         //工作任务计数器
		data_queue_t    m_data_queue;                       //传递工作数据的队列
		locker_t        m_data_locker;                      //传递队列数据使用的同步锁
		void           *m_task_init_param;                  //任务线程启动时的初始化参数,来自init(init_param).
	public:
		//-------------------------------------------------
		task_disp_t() :m_task_init_param(NULL) {}
		~task_disp_t() { uninit(); }
		//-------------------------------------------------
		//任务调度器进行初始化,可告知任务线程初始启动使用的init_param参数
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
		//任务调度器解除
		void uninit()
		{
			for (uint32_t i = 0; i < thread_count; ++i)
				m_workers[i].task_proxy.halt();             //给所有的任务标记,即将结束

			for (uint32_t i = 0; i < thread_count; ++i)
				m_sem_work.post();                          //对工作计数器进行提交,唤醒所有的任务

			for (uint32_t i = 0; i < thread_count; ++i)
				m_workers[i].task_thread.stop(true);        //等待所有的工作线程结束

			m_sem_work.uninit();                            //解除工作计数器
			m_sem_idle.uninit();                            //解除空闲计数器
		}
		//-------------------------------------------------
		//推送任务数据,等待调度处理
		bool push(void* data, int wait_ms = -1)
		{
			//先等待空闲任务数量,超时没有就退出
			if (!m_sem_idle.take(wait_ms))
				return false;

			//工作参数进入待处理队列,进行锁同步
			{
				GUARD(m_data_locker);
				m_data_queue.push(data);
			}

			//唤醒一个工作任务
			return m_sem_work.post();
		}
		//-------------------------------------------------
		//尝试在uninit之后获取没来得及被处理的任务数据,便于销毁处理
		//返回值:NULL没有了.否则为之前push的data.
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
