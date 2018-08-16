#ifndef _RX_UT_OS_TASKS_H_
#define _RX_UT_OS_TASKS_H_

#include "../rx_os_tasks.h"
#include "../rx_tdd.h"

//---------------------------------------------------------
//对任务调度器进行测试
class my_test_disp_task:public rx::task_t
{
protected:
    uint32_t on_run(void* param)
    {
        uint64_t data=(uint64_t)param;
        printf("task_disp [%u] [%d]\n",(uint32_t)rx_thread_id(),(uint32_t)data);
        Sleep(500);
        return 0;
    }
};

inline void test_my_disp_task()
{
    rx::task_disp_t<my_test_disp_task,4> my_disp;
    rx_check(my_disp.init());

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

rx_tdd_rtl(rx_os_tasks_base,tdd_level_ui)
{
    test_my_disp_task();
}

#endif
