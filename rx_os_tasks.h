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
        //外部任务代理封装,用于处理数据队列与阻塞调度
        class task_proxy_t :public task_class
        {
        protected:
            //---------------------------------------------
            //线程任务启动了
            virtual void on_begin(void* param)
            {
                m_parent = static_cast<task_disp_t*>(param);
                task_class::on_begin(m_parent->m_task_init_param);
            }
            //---------------------------------------------
            //线程任务主函数结束了
            virtual void on_end(void* param) { task_class::on_end(param); }
            //---------------------------------------------
            //进入线程任务主函数
            virtual uint32_t on_run(void* param)
            {
                while (m_parent->m_sem_work.take())
                {//本任务被正确唤醒,可以进行一次处理了
                    if (task_class::need_break())
                        break;

                    void *data = NULL;
                    {GUARD(m_parent->m_data_locker);
                    //锁定队列同步锁,得到队首数据
                    rx_assert(m_parent->m_data_queue.size() != 0);
                    if (m_parent->m_data_queue.size() == 0)
                        continue;
                    data = m_parent->m_data_queue.front();
                    m_parent->m_data_queue.pop();
                    }

                    //进行真正的数据处理任务
                    task_class::on_run(data);

                    //任务处理完成后,通知空闲计数器
                    m_parent->m_sem_idle.post();
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
            task_proxy_t    task_proxy;
            thread_t        task_thread;
            worker_t() : task_thread(task_proxy) {}
        }worker_t;

        //-------------------------------------------------
        typedef std::queue<void*> data_queue_t;

        //-------------------------------------------------
        worker_t        m_workers[thread_count];            //定义任务worker数组
        semp_t          m_sem_idle;                         //空闲任务计数器
        semp_t          m_sem_work;                         //工作任务计数器
        data_queue_t    m_data_queue;                       //传递工作数据的队列
        locker_t        m_data_locker;                      //传递队列数据使用的同步锁
        void           *m_task_init_param;                  //任务初始化参数
    public:
        //-------------------------------------------------
        task_disp_t() :m_task_init_param(NULL) {}
        ~task_disp_t() { uninit(); }
        //-------------------------------------------------
        //任务调度器进行初始化
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
                m_workers[i].task_proxy.stop();             //给所有的任务标记,即将结束

            for (uint32_t i = 0; i < thread_count; ++i)
                m_sem_work.post();                          //对工作计数器进行提交,唤醒所有的任务

            for (uint32_t i = 0; i < thread_count; ++i)
                m_workers[i].task_thread.stop();            //等待所有的工作线程结束

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
            {GUARD(m_data_locker);
                m_data_queue.push(data);
            }

            //唤醒一个工作任务
            return m_sem_work.post();
        }
        //-------------------------------------------------
        //尝试在uninit之后获取未来得及被处理的任务数据
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
