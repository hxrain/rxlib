#ifndef _RX_MEM_ALLOC_CNTR_H_
#define _RX_MEM_ALLOC_CNTR_H_

#include "rx_cc_macro.h"
#include "rx_mempool.h"
#include "rx_mempool_cntr.h"
#include "rx_mempool_fx1.h"
#include "rx_assert.h"
#include "rx_ct_util.h"
#include "rx_memalloc.h"
#include "rx_os_lock.h"
#include "rx_os_spinlock.h"

namespace rx
{
    //------------------------------------------------------
    //标准的,基于内存池接口的内存分配器
    template<class lock_t>
    class mem_allotter_std :public mem_allotter_i
    {
    protected:
        mempool   &m_base_pool;
        lock_t     m_lock;
        //-------------------------------------------------
        virtual void* base_alloc(uint32_t &bksize,uint32_t size)
        {
            GUARD_T(m_lock, lock_t);
            return m_base_pool.do_alloc(bksize,size);
        }
        virtual void base_free(void* ptr, uint32_t size)
        {
            GUARD_T(m_lock, lock_t);
            m_base_pool.do_free(ptr, size);
        }
        mem_allotter_std(mempool& pool):m_base_pool(pool){}
    };

    //------------------------------------------------------
    //基于内存池的内存分配器容器
    template<class pool_t,class lock_t=null_lock_t>
    class mem_allotter_pool:public mem_allotter_std<lock_t>
    {//描述默认的各种容器类型的内存分配器,无锁保护,单线程安全.
        pool_t      m_pool;
    public:
        mem_allotter_pool():mem_allotter_std<lock_t>(m_pool){}
    };

    //------------------------------------------------------
    //描述一个通用的内存分配器类型拼装宏
    //tname为最终的类型名;cfg_t为内存池参数配置;locl_t为锁类型;cntr_t为内存池容器类型;pool_t为内存池类型
    #define desc_mem_allotter(tname,cfg_t,lock_t,cntr_t,pool_t) \
        typedef mem_allotter_pool<cntr_t<pool_t<cfg_t>,cfg_t> ,lock_t>  tname

    //简化的内存分配器(线性递增)类型描述:tname定义的分配器类型名称;cfg_t为内存池配置参数;lock_t为锁类型
    #define desc_mem_allotter_lin(tname,cfg_t,lock_t) \
        desc_mem_allotter(tname,cfg_t,lock_t,mempool_cntr_lin,mempool_fixed_t)

    //简化的内存分配器(指数倍增)类型描述:tname定义的分配器类型名称;cfg_t为内存池配置参数;lock_t为锁类型
    #define desc_mem_allotter_pow2(tname,cfg_t,lock_t) \
        desc_mem_allotter(tname,cfg_t,lock_t,mempool_cntr_pow2,mempool_fixed_t)

    //简化的内存分配器(两级映射)类型描述:tname定义的分配器类型名称;cfg_t为内存池配置参数;lock_t为锁类型
    #define desc_mem_allotter_tlmap(tname,cfg_t,lock_t) \
        desc_mem_allotter(tname,cfg_t,lock_t,mempool_cntr_tlmap,mempool_fixed_t)

    //------------------------------------------------------
    //描述默认的各种容器类型的内存分配器,无锁保护,单线程安全.
    desc_mem_allotter_lin  (mem_allotter_lin_t,mempool_cfg_t,null_lock_t);
    desc_mem_allotter_pow2 (mem_allotter_pow2_t,mempool_cfg_t,null_lock_t);
    desc_mem_allotter_tlmap(mem_allotter_tlmap_t,mempool_cfg_t,null_lock_t);
    //------------------------------------------------------
    //描述默认的各种容器类型的内存分配器,自旋锁保护,多线程安全.
    desc_mem_allotter_lin  (mem_allotter_lin_slt,mempool_cfg_t,spin_lock_t);
    desc_mem_allotter_pow2 (mem_allotter_pow2_slt,mempool_cfg_t,spin_lock_t);
    desc_mem_allotter_tlmap(mem_allotter_tlmap_slt,mempool_cfg_t,spin_lock_t);

    //------------------------------------------------------
    //定义默认的全局使用的内存分配器
    inline mem_allotter_i& global_mem_allotter()
    {
        static mem_allotter_tlmap_slt allotter;
        return allotter;
    }
}
    //------------------------------------------------------
    //描述全局使用的默认内存分配器语法糖
    #define rx_alloc(size)          rx::global_mem_allotter().alloc((size))
    #define rx_realloc(p,newsize)   rx::global_mem_allotter().realloc((p),(newsize))
    #define rx_free(p)              rx::global_mem_allotter().free((p))
    #define rx_new(T)               rx::global_mem_allotter().new0<T>()
    #define rx_new1(T,P1)           rx::global_mem_allotter().new1<T>(P1)
    #define rx_new2(T,P1,P2)        rx::global_mem_allotter().new2<T>(P1,P2)
    #define rx_new3(T,P1,P2,P3)     rx::global_mem_allotter().new3<T>(P1,P2,P3)
    #define rx_array(T,C)           rx::global_mem_allotter().new0<T>(C)
    #define rx_array1(T,P1,C)       rx::global_mem_allotter().new1<T>(P1,C)
    #define rx_array2(T,P1,P2,C)    rx::global_mem_allotter().new2<T>(P1,P2,C)
    #define rx_array3(T,P1,P2,P3,C) rx::global_mem_allotter().new3<T>(P1,P2,P3,C)
    #define rx_delete(P)            rx::global_mem_allotter().del((P))

#endif
