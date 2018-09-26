#ifndef _RX_MEM_ALLOC_CNTR_H_
#define _RX_MEM_ALLOC_CNTR_H_

#include "rx_cc_macro.h"
#include "rx_mem_pool.h"
#include "rx_mem_pool_cntr.h"
#include "rx_mem_pool_fx1.h"
#include "rx_mem_pool_h4.h"

#include "rx_mem_alloc.h"

#include "rx_assert.h"
#include "rx_ct_util.h"
#include "rx_os_lock.h"
#include "rx_os_spinlock.h"

//����һ��ͨ�õ��ڴ����������ƴװ��
//tnameΪ���յ�������;cfg_tΪ�ڴ�ز�������;locl_tΪ������;cntr_tΪ�ڴ����������;pool_tΪ�ڴ������
#define desc_mem_allotter(tname,cfg_t,lock_t,mp_cntr_t,pool_t) \
        typedef rx::mem_allotter_pool_t<mp_cntr_t<pool_t<cfg_t>,cfg_t> ,lock_t>  tname

//����һ��ͨ�õ�H4�ڴ����������ƴװ��
//tnameΪ���յ�������;cfg_tΪ�ڴ�ز�������;locl_tΪ������;
#define desc_mem_allotter_h4(tname,cfg_t,h4_pool) \
        typedef rx::mem_allotter_pool_t<h4_pool<cfg_t> ,rx::null_lock_t>  tname
#define desc_mem_allotter_h4slt(tname,cfg_t,h4_pool) \
        typedef rx::mem_allotter_pool_t<h4_pool<cfg_t> ,rx::spin_lock_t>  tname

//------------------------------------------------------
//�򻯵��ڴ������(���Ե���)��������:tname����ķ�������������;cfg_tΪ�ڴ�����ò���;lock_tΪ������
#define desc_mem_allotter_lin(tname,cfg_t,lock_t,pool_t) \
        desc_mem_allotter(tname,cfg_t,lock_t,rx::mempool_cntr_lin_t,pool_t)

//�򻯵��ڴ������(ָ������)��������:tname����ķ�������������;cfg_tΪ�ڴ�����ò���;lock_tΪ������
#define desc_mem_allotter_pow2(tname,cfg_t,lock_t,pool_t) \
        desc_mem_allotter(tname,cfg_t,lock_t,rx::mempool_cntr_pow2_t,pool_t)

//�򻯵��ڴ������(����ӳ��)��������:tname����ķ�������������;cfg_tΪ�ڴ�����ò���;lock_tΪ������
#define desc_mem_allotter_tlmap(tname,cfg_t,lock_t,pool_t) \
        desc_mem_allotter(tname,cfg_t,lock_t,rx::mempool_cntr_tlmap_t,pool_t)

namespace rx
{
    //------------------------------------------------------
    //��׼��,�����ڴ�ؽӿڵ��ڴ������
    template<class lock_t>
    class mem_allotter_base_t :public mem_allotter_i
    {
    protected:
        mempool_t  &m_base_pool;
        lock_t     m_lock;
        //-------------------------------------------------
        virtual void* base_alloc(uint32_t &bksize,uint32_t size){GUARD_T(m_lock, lock_t);return m_base_pool.do_alloc(bksize,size);}
        virtual void base_free(void* ptr, uint32_t size){GUARD_T(m_lock, lock_t);m_base_pool.do_free(ptr, size);}
        mem_allotter_base_t(mempool_t& pool):m_base_pool(pool) {}
    };

    //------------------------------------------------------
    //ʹ��C��׼����ڴ���������ܷ�װ
    class mem_allotter_std_t :public mem_allotter_i
    {
    protected:
        //-------------------------------------------------
        virtual void* base_alloc(uint32_t &bksize,uint32_t size){bksize=size;return ::malloc(size);}
        virtual void base_free(void* ptr, uint32_t size){::free(ptr);}
    };

    //------------------------------------------------------
    //�����ڴ�ص��ڴ����������
    template<class pool_t,class lock_t=null_lock_t>
    class mem_allotter_pool_t:public mem_allotter_base_t<lock_t>
    {
        //����Ĭ�ϵĸ����������͵��ڴ������,��������,���̰߳�ȫ.
        pool_t      m_pool;
    public:
        mem_allotter_pool_t():mem_allotter_base_t<lock_t>(m_pool) {}
    };

    //------------------------------------------------------
    //����Ĭ�ϵĸ����������͵��ڴ������,��������,���̰߳�ȫ.
    desc_mem_allotter_lin   (mem_allotter_lin_t,        mempool_cfg_t, null_lock_t, mempool_fixed_t);
    desc_mem_allotter_pow2  (mem_allotter_pow2_t,       mempool_cfg_t, null_lock_t, mempool_fixed_t);
    desc_mem_allotter_tlmap (mem_allotter_tlmap_t,      mempool_cfg_t, null_lock_t, mempool_fixed_t);
    //------------------------------------------------------
    //����Ĭ�ϵĸ����������͵��ڴ������,����������,���̰߳�ȫ.
    desc_mem_allotter_lin   (mem_allotter_lin_slt,      mempool_cfg_t, spin_lock_t, mempool_fixed_t);
    desc_mem_allotter_pow2  (mem_allotter_pow2_slt,     mempool_cfg_t, spin_lock_t, mempool_fixed_t);
    desc_mem_allotter_tlmap (mem_allotter_tlmap_slt,    mempool_cfg_t, spin_lock_t, mempool_fixed_t);
    desc_mem_allotter_tlmap (mem_allotter_tlmap_slt_std,mempool_cfg_t, spin_lock_t, mempool_std_t);
}

//------------------------------------------------------
//����Ĭ�ϵ�ȫ��ʹ�õ��ڴ������
inline rx::mem_allotter_i& rx_global_mem_allotter()
{
#if RX_DEF_ALLOC_USE_STD
    static rx::mem_allotter_std_t allotter;
#else
    static rx::mem_allotter_tlmap_slt allotter;
#endif
    return allotter;
}
//------------------------------------------------------
//����ȫ��ʹ�õ�Ĭ���ڴ�������﷨��
#define rx_mem()                rx_global_mem_allotter()
#define rx_alloc(size)          rx_global_mem_allotter().alloc((size))
#define rx_realloc(p,newsize)   rx_global_mem_allotter().realloc((p),(newsize))
#define rx_free(p)              rx_global_mem_allotter().free((p))
#define rx_new(T)               rx_global_mem_allotter().new0<T>()
#define rx_new1(T,P1)           rx_global_mem_allotter().new1<T>(P1)
#define rx_new2(T,P1,P2)        rx_global_mem_allotter().new2<T>(P1,P2)
#define rx_new3(T,P1,P2,P3)     rx_global_mem_allotter().new3<T>(P1,P2,P3)
#define rx_array(T,C)           rx_global_mem_allotter().new0<T>(C)
#define rx_array1(T,P1,C)       rx_global_mem_allotter().new1<T>(P1,C)
#define rx_array2(T,P1,P2,C)    rx_global_mem_allotter().new2<T>(P1,P2,C)
#define rx_array3(T,P1,P2,P3,C) rx_global_mem_allotter().new3<T>(P1,P2,P3,C)
#define rx_delete(P)            rx_global_mem_allotter().del((P))

#endif
