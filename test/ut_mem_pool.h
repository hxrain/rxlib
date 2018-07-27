#ifndef _UT_MEM_POOL_H_
#define _UT_MEM_POOL_H_

#include "../rx_mempool_fx1.h"
#include "../rx_tdd.h"

template<class T>
void test_mem_pool_base(rx_tdd_base &rt)
{
    uint32_t bsize;
    T mempool(4);
    rt.assert(mempool.is_full());
    
    uint8_t *p1=(uint8_t *)mempool.do_alloc(bsize);
    rt.assert(p1!=NULL);
    rt.assert(mempool.using_blocks()==1);
    rt.assert(!mempool.is_full());
    
    uint8_t *p2=(uint8_t *)mempool.do_alloc(bsize);
    rt.assert(p2!=NULL);
    rt.assert(mempool.using_blocks()==2);
    rt.assert(!mempool.is_full());
    
    mempool.do_free(p1);
    rt.assert(!mempool.is_full());
    rt.assert(mempool.using_blocks()==1);

    mempool.do_free(p2);
    rt.assert(mempool.using_blocks()==0);
    rt.assert(mempool.is_full());
    mempool.do_uninit();
}


rx_tdd(test_mem_pool_base)
{
    test_mem_pool_base<rx::mempool_fixed_t<> >(*this);
}





#endif // _UT_MEM_POOL_H_
