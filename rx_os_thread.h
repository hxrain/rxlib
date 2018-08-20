#ifndef _HX_THREAD_H_
#define _HX_THREAD_H_

#include "rx_cc_macro.h"
#include "rx_atomic.h"
#include "rx_os_lock.h"
#include "rx_os_cond.h"

namespace rx
{
    /*任务与线程的使用样例

        //可进行循环并逻辑终止的任务
        class my_task_t:public task_t
        {
            uint32_t on_run(void* param)
            {
                while(!need_break())
                {
                }
                return 0;
            }
        };


        //定义任务对象与线程对象并绑定
        my_task_t task;
        thread_t thread(task);

        //启动线程,等待线程停止
        if (thread.start())
            thread.stop();
    */

    //-----------------------------------------------------
    //线程任务接口,内置循环处理与停止逻辑
    class task_t
    {
        atomic_int   m_stop_flag;
        friend class thread_t;
    protected:
        //-------------------------------------------------
        virtual bool need_break() { return !!m_stop_flag; }
        //-------------------------------------------------
        //将一个长休眠变为多个短休眠,便于检查是否要求停止
        void sleep(uint32_t ms)
        {
            uint32_t sc = ms / 100;
            if (!sc) rx_thread_yield(ms);
            else
            {
                for (uint32_t i = 0; i < sc; ++i)
                {
                    rx_thread_yield(100);
                    if (need_break())
                        break;
                }
            }
        }
    public:
        virtual ~task_t() {}
        //-------------------------------------------------
        //让任务循环逻辑终止
        virtual void stop() { m_stop_flag.inc(); }
    protected:
        //-------------------------------------------------
        //任务被启动了
        virtual void on_begin(void* param) {}
        //-------------------------------------------------
        //默认任务处理过程,持续循环,直到出现中断标记
        virtual uint32_t on_run(void* param)
        {
            while (!need_break())
                on_step(param);
            return 0;
        }
        //-------------------------------------------------
        //单次执行
        virtual void on_step(void* param) {}
        //-------------------------------------------------
        //任务主函数结束了
        virtual void on_end(void* param) {}
    };

    //-----------------------------------------------------
    //带有暂停/恢复功能的任务处理功能对象
    class task_ex :public task_t
    {
        cond_t      m_cond;
        locker_t    m_lock;
        bool        m_pause_flag;
    protected:
        //-------------------------------------------------
        virtual bool need_break()
        {
            if (task_t::need_break())
                return true;

            if (m_pause_flag)
            {
                GUARD(m_lock);
                m_cond.wait(m_lock);
            }

            return task_t::need_break();
        }
    public:
        task_ex(bool is_pause = false) :m_pause_flag(is_pause) {}
        //-------------------------------------------------
        //标记,让线程进入逻辑暂停
        void pause()
        {
            if (!m_pause_flag)
            {
                GUARD(m_lock);
                m_pause_flag = true;
            }
        }
        //-------------------------------------------------
        //标记,让任务从逻辑暂停中恢复
        void resume()
        {
            if (m_pause_flag)
            {
                GUARD(m_lock);
                m_pause_flag = false;
                m_cond.post();
            }
        }
        //-------------------------------------------------
        //标记,让任务循环逻辑终止
        void stop()
        {
            task_t::stop();
            resume();
        }
    };




#if RX_OS_POSIX
    //-----------------------------------------------------
    //线程功能对象化封装
    class thread_t
    {
        task_t      &m_task;                                    //线程调用的任务对象.引用.
        void        *m_task_param;                              //线程任务的入口参数
        pthread_t    m_handle;                                  //线程句柄
        //-------------------------------------------------
        //真正的线程执行入口函数
        static void *thread_start_routine(void * THIS)
        {
            thread_t &thread = *static_cast<thread_t*>(THIS);
            //进行任务启动事件的调用
            try {
                thread.m_task.on_begin(thread.m_task_param);
            }
            catch (...) { rx_alert( "thread.m_task.on_begin() exception."); }

            //进行任务主体函数的调用
            int rc = 0;
            try {
                rc = thread.m_task.on_run(thread.m_task_param);
            }
            catch (...) { rx_alert( "thread.m_task.on_run() exception."); }

            //进行任务结束事件的调用
            try {
                thread.m_task.on_end(thread.m_task_param);
            }
            catch (...) { rx_alert( "thread.m_task.on_end() exception."); }

            return (void*)(intptr_t)rc;
        }
        //-------------------------------------------------
        class thread_attr
        {
            pthread_attr_t m_attr;
            bool m_is_valid;
        public:
            thread_attr() { m_is_valid = (pthread_attr_init(&m_attr) == 0); }
            bool operator()() { return m_is_valid; }
            bool set_stacksize(uint32_t size=0)
            {
                if (size&&pthread_attr_setstacksize(&m_attr, size))
                    return false;
                return true;
            }
            pthread_attr_t& attr() { return m_attr; }
            ~thread_attr()
            {
                if (!m_is_valid) return;
                m_is_valid = false;
                pthread_attr_destroy(&m_attr);
            }
        };
        //-------------------------------------------------
        //进行线程属性参数的设置
        bool m_init_attr(pthread_attr_t *attr, uint32_t stacksize = 0)
        {
            if (stacksize&&pthread_attr_setstacksize(attr, stacksize))
                return false;
            return true;
        }
    public:
        thread_t(task_t& task) :m_task(task), m_task_param(NULL), m_handle(-1) {}
        //-------------------------------------------------
        //创建线程,启动任务,并传递任务参数指针(需要注意任务参数的生存期与线程的生存期的关系)
        bool start(void* task_param = NULL, uint32_t stacksize = 0)
        {
            //进行线程属性的初始化
            thread_attr ta;
            if (!ta()) return false;
            if (!ta.set_stacksize(stacksize))
                return false;

            m_task_param = task_param;

            return 0 == pthread_create(&m_handle, &ta.attr(), thread_start_routine, this);
        }
        //-------------------------------------------------
        //等待线程结束并得到任务的退出码
        int stop()
        {
            if (m_handle == (pthread_t)-1)
                return -1;
            m_task.stop();
            void* rc;
            if (pthread_join(m_handle, &rc))
                return -2;
            m_handle = -1;
            return (uint32_t)(uintptr_t)rc;
        }
        //-------------------------------------------------
    };

#elif RX_OS_WIN
    //-----------------------------------------------------
    //线程功能对象化封装
    class thread_t
    {
        task_t      &m_task;                                    //线程调用的任务对象.引用.
        void        *m_task_param;                              //线程任务的入口参数
        HANDLE      m_handle;                                   //线程句柄
        int         m_rc;                                       //任务的返回值
        //-------------------------------------------------
        //真正的线程执行入口函数
        static unsigned int WINAPI thread_start_routine(void * THIS)
        {
            thread_t &thread = *static_cast<thread_t*>(THIS);
            //进行任务启动事件的调用
            try {
                thread.m_task.on_begin(thread.m_task_param);
            }
            catch (...) { rx_alert( "thread.m_task.on_begin() exception."); }

            //进行任务主体函数的调用
            thread.m_rc = 0;
            try {
                thread.m_rc = thread.m_task.on_run(thread.m_task_param);
            }
            catch (...) { rx_alert( "thread.m_task.on_run() exception."); }

            //进行任务结束事件的调用
            try {
                thread.m_task.on_end(thread.m_task_param);
            }
            catch (...) { rx_alert( "thread.m_task.on_end() exception."); }

            return thread.m_rc;
        }

    public:
        thread_t(task_t& task) :m_task(task), m_task_param(NULL), m_handle(NULL) {}
        //-------------------------------------------------
        //创建线程,启动任务,并传递任务参数指针(需要注意任务参数的生存期与线程的生存期的关系)
        bool start(void* task_param = NULL, uint32_t stacksize = 0)
        {
            //进行线程属性的初始化
            uint32_t CreateFlag = 0;
            if (stacksize)
                CreateFlag = STACK_SIZE_PARAM_IS_A_RESERVATION;
            m_task_param = task_param;
            m_handle=(HANDLE)_beginthreadex(NULL, stacksize, thread_start_routine, (void*)this, CreateFlag, NULL);

            return m_handle!=NULL;
        }
        //-------------------------------------------------
        //等待线程结束并得到任务的退出码
        int stop()
        {
            if (m_handle == NULL)
                return -1;

            m_task.stop();

            if (WaitForSingleObject(m_handle, INFINITE))
                return -2;

            CloseHandle(m_handle);
            m_handle = NULL;
            return m_rc;
        }
        //-------------------------------------------------
    };
#endif
}
#endif
