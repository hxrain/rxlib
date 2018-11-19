#define RX_USE_TDD_TICK 1

#include "../../rx_dtl_array.h"

//紧凑高效的gbk/unicode字符集转换功能
#include "../ut_str_uni2gbk.h"
#include "../../rx_str_cs_gbk2uni.h"
#include "../../rx_str_cs_uni2gbk.h"
//---------------------------------------------------------
//key基于bit分段的基数树
#include "../ut_dtl_radix_bit.h"
#include "../../rx_dtl_raxbit_map.h"
#include "../../rx_dtl_raxbit_set.h"
#include "../../rx_dtl_raxbit_raw.h"
//---------------------------------------------------------
//基于跳表的哈希映射容器,不限容量
#include "../ut_dtl_hashmap.h"
#include "../../rx_dtl_hashmap.h"
//---------------------------------------------------------
//基于内存文件映射的定长哈希表,可作为轻量级持久化k/v数据库
#include "../ut_ext_hashtbl_mm.h"
#include "../../rx_ext_hashtbl_mm.h"
//---------------------------------------------------------
//统一的错误码定义
#include "../../rx_ct_error.h"
//---------------------------------------------------------
//系统文件操作功能封装
#include "../ut_os_file.h"
#include "../../rx_os_file.h"
//---------------------------------------------------------
//文件内存映射功能封装
#include "../ut_os_mmap.h"
#include "../../rx_os_mmap.h"
//---------------------------------------------------------
//系统杂项功能
#include "../../rx_os_misc.h"
//---------------------------------------------------------
//模拟c++11的typeof与auto的功能封装
#include "../../rx_ct_typeof.h"
//---------------------------------------------------------
//基于时间轮的定时器功能框架
#include "../ut_timer_tw.h"
#include "../../rx_timer_tw.h"
//---------------------------------------------------------
//层次状态机功能封装
#include "../ut_hsm_core.h"
#include "../../rx_hsm_core.h"
//---------------------------------------------------------
//链表/栈/队列的基础容器功能封装
#include "../ut_dtl_list.h"
#include "../../rx_dtl_list.h"
#include "../../rx_dtl_stack.h"
#include "../../rx_dtl_queue.h"
//---------------------------------------------------------
//哈希表与跳表以及std::map容器的对比测试
#include "../ut_dtl_htbl_sklist_map.h"
//---------------------------------------------------------
//跳表与相关容器
#include "../ut_dtl_skiplist.h"
#include "../../rx_dtl_skipset.h"
#include "../../rx_dtl_skiplist.h"
#include "../../rx_dtl_skiplist_raw.h"
//---------------------------------------------------------
//哈希表与哈希容器相关功能
#include "../ut_dtl_hashtbl_tiny.h"
#include "../../rx_dtl_hashtbl_ex.h"
#include "../../rx_dtl_hashset.h"
#include "../../rx_dtl_hashlink.h"
#include "../../rx_dtl_hashtbl.h"
#include "../../rx_dtl_hashtbl_raw.h"
//---------------------------------------------------------
//字符串相关功能与极小字符串功能封装,便于容器内置使用
#include "../ut_str_util.h"
#include "../../rx_str_tiny.h"
#include "../../rx_str_util.h"
//---------------------------------------------------------
//测试用例与运行计时功能
#include "../../rx_tdd_tick.h"
//---------------------------------------------------------
//基于bipbuffer算法的环形缓冲区
#include "../ut_dtl_ringbuff.h"
#include "../../rx_dtl_ringbuff.h"
//---------------------------------------------------------
//环形队列
#include "../ut_dtl_ringqueue.h"
#include "../../rx_dtl_ringqueue.h"
//---------------------------------------------------------
//基于线程与同步原语的任务调度器功能封装
#include "../ut_os_tasks.h"
#include "../../rx_os_tasks.h"
//---------------------------------------------------------
//系统线程功能封装
#include "../../rx_os_thread.h"
//---------------------------------------------------------
//系统条件变量同步原语
#include "../../rx_os_cond.h"
//---------------------------------------------------------
//系统信号量/旗标同步原语
#include "../../rx_os_sem.h"
//---------------------------------------------------------
//系统同步线程锁定功能
#include "../ut_os_lock.h"
#include "../../rx_os_lock.h"
#include "../../rx_os_spinlock.h"
//---------------------------------------------------------
//two level bitmap
#include "../ut_hash_tlmap.h"
#include "../../rx_hash_tlmap.h"
//---------------------------------------------------------
//整数哈希与数据块哈希
#include "../ut_hash.h"
#include "../../rx_hash_rand.h"
#include "../../rx_hash_data.h"
#include "../../rx_hash_int.h"
//---------------------------------------------------------
//bit位操作
#include "../ut_bits_op.h"
#include "../../rx_ct_bitop.h"
#include "../../rx_ct_bitarr.h"
//---------------------------------------------------------
//内存分配器接口与内存分配容器
#include "../ut_mem_alloc.h"
#include "../../rx_mem_alloc_cntr.h"
#include "../../rx_mem_alloc.h"
//---------------------------------------------------------
//固定内存块范围的内存分配器
#include "../ut_mem_pool_h4.h"
#include "../../rx_mem_pool_h4.h"
//---------------------------------------------------------
//内存池容器
#include "../ut_mem_pool_cntr.h"
#include "../../rx_mem_pool_cntr.h"
//---------------------------------------------------------
//内存池基础功能
#include "../ut_mem_pool.h"
#include "../../rx_mem_pool_fx1.h"
#include "../../rx_dtl_list_raw.h"
#include "../../rx_mem_pool.h"
//---------------------------------------------------------
//编译期功能:类型萃取/编译期计算/简单委托
#include "../ut_ct_obj.h"
#include "../../rx_ct_delegate.h"
#include "../../rx_ct_util.h"
#include "../../rx_ct_traits.h"
//---------------------------------------------------------
//系统时间相关
#include "../ut_datetime.h"
#include "../../rx_datetime.h"
#include "../../rx_datetime_ex.h"
//---------------------------------------------------------
//原子变量
#include "../ut_atomic.h"
#include "../../rx_ct_atomic.h"
//---------------------------------------------------------
//原子操作
#include "../ut_cc_atomic.h"
#include "../../rx_cc_atomic.h"
//---------------------------------------------------------
//测试驱动开发/单元测试框架
#include "../../rx_tdd.h"
//---------------------------------------------------------
//断言处理
#include "../../rx_assert.h"
//---------------------------------------------------------
//编译器预定义宏功能封装
#include "../ut_cc_macro.h"
#include "../../rx_cc_macro.h"

//---------------------------------------------------------
#include <stdio.h>
#include <iostream>
//---------------------------------------------------------


int main()
{
    std::cout << "rxlib unit test!你好!Hello world!\n" << rx_cc_desc() << std::endl << std::endl;

    rx_static_assert(1);

    rx_tdd_run(tdd_level_std);

    getchar();
    return 0;
}
