#ifndef _UT_MEM_POOL_H_
#define _UT_MEM_POOL_H_

#include "../rx_mem_pool.h"
#include "../rx_tdd.h"

template<class T>
void test_mem_pool_base(rx_tdd_base &rt)
{
    T mp4(4);
    rt.assert(mp4.is_full());
    uint8_t *p1=(uint8_t *)mp4.do_alloc();
    rt.assert(p1!=NULL);
    rt.assert(mp4.using_blocks()==1);
    rt.assert(!mp4.is_full());
    uint8_t *p2=(uint8_t *)mp4.do_alloc();
    rt.assert(p2!=NULL);
    rt.assert(mp4.using_blocks()==2);
    rt.assert(!mp4.is_full());
    mp4.do_free(p1);
    rt.assert(!mp4.is_full());
    rt.assert(mp4.using_blocks()==1);
    mp4.do_free(p2);
    rt.assert(mp4.using_blocks()==0);
    rt.assert(mp4.is_full());
    rt.assert(mp4.uninit());
}


rx_tdd(test_mem_pool_base)
{
    test_mem_pool_base<rx::mempool_fixed_t<> >(*this);
}





#endif // _UT_MEM_POOL_H_
