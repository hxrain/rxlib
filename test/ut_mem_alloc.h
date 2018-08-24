#ifndef _UT_MEM_ALLOC_H_
#define _UT_MEM_ALLOC_H_

#include "../rx_memalloc_cntr.h"
#include "../rx_tdd.h"

void test_mem_alloc_base(rx_tdd_base &rt)
{
    uint32_t *dp1=rx_new(uint32_t);
    rt.tdd_assert(dp1!=NULL);
    rx_delete(dp1);

    void *p1=rx_alloc(5);
    rt.tdd_assert(p1!=NULL);
    void *p2=rx_realloc(p1,6);
    rt.tdd_assert(p1==p2);
    rx_free(p2);

}

template<class T>
void test_mem_alloc_a(rx_tdd_base &rt)
{
}


rx_tdd(test_mem_alloc_base)
{
    test_mem_alloc_base(*this);
}




#endif // _UT_MEM_POOL_H_
