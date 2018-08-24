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
    class mem_alloc_std :public mem_alloc_i
    {
    protected:
        mempool   &m_pool;
        lock_t     m_lock;
        //-------------------------------------------------
        virtual void* base_alloc(uint32_t &bksize,uint32_t size) 
        { 
            GUARD_T(m_lock, lock_t);
            return m_pool.do_alloc(bksize,size); 
        }
        virtual void base_free(void* ptr, uint32_t size) 
        { 
            GUARD_T(m_lock, lock_t);
            m_pool.do_free(ptr, size); 
        }
    };
}


#endif
