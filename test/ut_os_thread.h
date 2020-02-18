#ifndef _UT_OS_THREAD_H_
#define _UT_OS_THREAD_H_

#include "../rx_cc_macro.h"
#include "../rx_tdd.h"
#include "../rx_os_thread.h"
#include "../rx_os_lock.h"
#include "../rx_os_misc.h"

namespace rx_ut
{
    rx::thread_tls_t G_tst_tls;

    //可进行循环并逻辑终止的任务
    class my_task_t:public rx::task_t
    {
        uint32_t on_run(void* param)
        {
            bool ret=G_tst_tls.setn(rx::get_tid());
            rx_assert(ret);
            if(!need_break()&&ret)
            {
                size_t tid=G_tst_tls.getn();
                rx_assert(tid!=0);
                printf("\t task thread id = %u\n",(uint32_t)tid);
            }
            return 0;
        }
    };
}

rx_tdd(rx_os_thread_base)
{
    assert(thread_tls_alloc(rx_ut::G_tst_tls));
    printf("\t curr process id = %u\n",(uint32_t)rx::get_pid());
    assert(rx_ut::G_tst_tls.setn(rx::get_tid()));
    size_t tid=rx_ut::G_tst_tls.getn();
    assert(tid!=0);
    printf("\t main thread id = %u\n",(uint32_t)tid);

    //定义任务对象与线程对象并绑定
    rx_ut::my_task_t task;
    rx::thread_t thread(task);

    //启动线程,等待线程停止
    if (thread.start())
        thread.stop(false);
}


#endif
