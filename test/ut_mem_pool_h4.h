
#ifndef _UT_MEM_POOL_H4_H_
#define _UT_MEM_POOL_H4_H_

#include "../rx_mem_pool_h4.h"
#include "../rx_tdd.h"

//---------------------------------------------------------
template<class T>
void test_mem_pool_h4_base(rx_tdd_t &rt)
{
    uint32_t bsize;
    T mempool;

    uint8_t *p1 = (uint8_t *)mempool.do_alloc(bsize,12);
    rt.assert(p1 != NULL);
    memset(p1,0xFE,bsize);

    uint8_t *p2 = (uint8_t *)mempool.do_alloc(bsize,63);
    rt.assert(p2 != NULL);
    memset(p2, 0xFE, bsize);

    mempool.do_free(p1);

    mempool.do_free(p2);

    mempool.do_uninit();
}

rx_tdd(test_mem_pool_h4_base)
{
    test_mem_pool_h4_base<rx::mempool_h4f_t<> >(*this);
}





#endif // _UT_MEM_POOL_H_
