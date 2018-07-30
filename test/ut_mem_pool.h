#ifndef _UT_MEM_POOL_H_
#define _UT_MEM_POOL_H_

#include "../rx_mempool_fx1.h"
#include "../rx_tdd.h"

template<class T>
void test_mem_pool_base(rx_tdd_base &rt)
{
    uint32_t bsize;
    T mempool(4);
    
    uint8_t *p1=(uint8_t *)mempool.do_alloc(bsize);
    rt.assert(p1!=NULL);
    
    uint8_t *p2=(uint8_t *)mempool.do_alloc(bsize);
    rt.assert(p2!=NULL);
    
    mempool.do_free(p1);

    mempool.do_free(p2);

    mempool.do_uninit();
}


rx_tdd(test_mem_pool_base)
{
    test_mem_pool_base<rx::mempool_fixed_t<> >(*this);
}





#endif // _UT_MEM_POOL_H_
