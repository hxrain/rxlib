#define RX_USE_TDD_TICK 1

#define UT_ALL 1
#define UT_SEL 42

//---------------------------------------------------------
#if UT_ALL||UT_SEL==42
    //轻量级紧凑查找表
	#include "../ut_dtl_hat.h"
	#include "../../rx_dtl_hat_map.h"
    #include "../../rx_dtl_hat_raw.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==41
    //TCP网络功能
    #include "../ut_net_tcp.h"
    #include "../../rx_net_tcp_svrsock.h"
    #include "../../rx_net_tcp_client.h"
    #include "../../rx_net_tcp_server.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==40
    //日志记录器与输出器
    #include "../ut_logger_std.h"
    #include "../../rx_logger_std.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==40
    //标准的同步socket功能封装
	#include "../ut_net_sock_std.h"
    #include "../../rx_net_sock_std.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==40||UT_SEL==41
    //使用网络功能,需要初始化socket环境
    rx::socket_env_t G_sock_env;
    #if RX_CC==RX_CC_VC
        #pragma comment(lib,"ws2_32.lib")
    #endif
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==39
	//简单的缓冲区功能封装
	#include "../../rx_dtl_buff.h"

	//常用数据容器算法
	#include "../ut_dtl_alg.h"
    #include "../../rx_dtl_alg.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==38
    //封装数组与别名数组
    #include "../ut_dtl_array.h"
    #include "../../rx_dtl_array.h"
    #include "../../rx_dtl_array_ex.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==37
    //紧凑高效的gbk/unicode字符集转换功能
    #include "../ut_str_uni2gbk.h"
    #include "../../rx_ucs2_gbk2uni.h"
    #include "../../rx_ucs2_uni2gbk.h"
    #include "../../rx_ucs2_rawgbk.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==36
    //ucs字符集杂项功能,判断字符类型,全半角转换,unicode分块等等.
    #include "../ut_ucs_misc.h"
    #include "../../rx_ucs2_misc.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==35
    //key基于bit分段的基数树
    #include "../ut_dtl_radix_bit.h"
    #include "../../rx_dtl_raxbit_map.h"
    #include "../../rx_dtl_raxbit_set.h"
    #include "../../rx_dtl_raxbit_raw.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==34
    //基于跳表的哈希映射容器,不限容量
    #include "../ut_dtl_hashmap.h"
    #include "../../rx_dtl_hashmap.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==33
    //基于内存文件映射的定长哈希表,可作为轻量级持久化k/v数据库
    #include "../ut_ext_hashtbl_mm.h"
    #include "../../rx_ext_hashtbl_mm.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==32
    //统一的错误码定义
    #include "../../rx_ct_error.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==31
    //系统文件操作功能封装
    #include "../ut_os_file.h"
    #include "../../rx_os_file.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==30
    //文件内存映射功能封装
    #include "../ut_os_mmap.h"
    #include "../../rx_os_mmap.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==29
    //系统杂项功能
    #include "../../rx_os_misc.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==28
    //模拟c++11的typeof与auto的功能封装
    #include "../../rx_ct_typeof.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==27
    //基于时间轮的定时器功能框架
    #include "../ut_timer_tw.h"
    #include "../../rx_timer_tw.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==26
    //层次状态机功能封装
    #include "../ut_hsm_core.h"
    #include "../../rx_hsm_core.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==25
    //链表/栈/队列的基础容器功能封装
    #include "../ut_dtl_list.h"
    #include "../../rx_dtl_list.h"
    #include "../../rx_dtl_stack.h"
    #include "../../rx_dtl_queue.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==24
    //哈希表与跳表以及std::map容器的对比测试
    #include "../ut_dtl_htbl_sklist_map.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==23
    //跳表与相关容器
    #include "../ut_dtl_skiplist.h"
    #include "../../rx_dtl_skipset.h"
    #include "../../rx_dtl_skiplist.h"
    #include "../../rx_dtl_skiplist_raw.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==22
    //哈希表与哈希容器相关功能
    #include "../ut_dtl_hashtbl_tiny.h"
    #include "../../rx_dtl_hashtbl_ex.h"
    #include "../../rx_dtl_hashset.h"
    #include "../../rx_dtl_hashlink.h"
    #include "../../rx_dtl_hashtbl.h"
    #include "../../rx_dtl_hashtbl_raw.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==21
    //字符串相关功能与极小字符串功能封装,便于容器内置使用
    #include "../ut_str_util.h"
    #include "../../rx_str_tiny.h"
    #include "../../rx_str_util_std.h"
    #include "../../rx_str_util_ext.h"
    #include "../../rx_str_util_bin.h"
    #include "../../rx_str_util_misc.h"
#endif
#if UT_ALL||UT_SEL==20
    //无外部依赖的字符串格式化功能封装
    #include "../ut_str_util_fmt.h"
    #include "../../rx_str_util_fmt.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==19
    //测试用例与运行计时功能
    #include "../../rx_tdd_tick.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==18
    //基于bipbuffer算法的环形缓冲区
    #include "../ut_dtl_ringbuff.h"
    #include "../../rx_dtl_ringbuff.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==17
    //环形队列
    #include "../ut_dtl_ringqueue.h"
    #include "../../rx_dtl_ringqueue.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==16
    //基于线程与同步原语的任务调度器功能封装
    #include "../ut_os_tasks.h"
    #include "../../rx_os_tasks.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==15
    //系统线程功能封装
    #include "../ut_os_thread.h"
    #include "../../rx_os_thread.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==14
    //系统条件变量同步原语
    #include "../../rx_os_cond.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==13
    //系统信号量/旗标同步原语
    #include "../../rx_os_sem.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==12
    //系统同步线程锁定功能
    #include "../ut_os_lock.h"
	#include "../../rx_lock_base.h"
	#include "../../rx_lock_os.h"
    #include "../../rx_lock_spin.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==11
    //two level bitmap
    #include "../ut_hash_tlmap.h"
    #include "../../rx_hash_tlmap.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==10
    //整数哈希与数据块哈希
    #include "../ut_hash.h"
    #include "../../rx_hash_rand.h"
    #include "../../rx_hash_data.h"
    #include "../../rx_hash_int.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==9
    //bit位操作
    #include "../ut_bits_op.h"
    #include "../../rx_ct_bitop.h"
    #include "../../rx_ct_bitarr.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==8
    //内存分配器接口与内存分配容器
    #include "../ut_mem_alloc.h"
    #include "../../rx_mem_alloc_base.h"
	#include "../../rx_mem_alloc.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==7
    //固定内存块范围的内存分配器
    #include "../ut_mem_pool_h4.h"
    #include "../../rx_mem_pool_h4.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==6
    //内存池容器
    #include "../ut_mem_pool_cntr.h"
    #include "../../rx_mem_pool_cntr.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==5
    //内存池基础功能
    #include "../ut_mem_pool.h"
    #include "../../rx_mem_pool_fx1.h"
    #include "../../rx_dtl_list_raw.h"
    #include "../../rx_mem_pool.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==4
    //编译期功能:类型萃取/编译期计算/简单委托
    #include "../ut_ct_obj.h"
    #include "../../rx_ct_delegate.h"
    #include "../../rx_ct_util.h"
    #include "../../rx_ct_traits.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==3
    //系统时间相关
    #include "../ut_datetime.h"
    #include "../../rx_datetime.h"
    #include "../../rx_datetime_ex.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==2
    //原子变量
    #include "../ut_atomic.h"
    #include "../../rx_ct_atomic.h"
#endif
//---------------------------------------------------------
#if UT_ALL||UT_SEL==1
    //原子操作
    #include "../ut_cc_atomic.h"
    #include "../../rx_cc_atomic.h"
#endif

//---------------------------------------------------------
//测试驱动开发/单元测试框架
#include "../../rx_tdd.h"
//---------------------------------------------------------
//常用基础功能
#include "../../rx_cc_base.h"
//---------------------------------------------------------
//断言功能
#include "../../rx_assert.h"
//---------------------------------------------------------
//编译器预定义宏
#include "../ut_cc_macro.h"
#include "../../rx_cc_macro.h"
//---------------------------------------------------------

int main()
{
    return rx_tdd_run(tdd_level_std);
}
