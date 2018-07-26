#ifndef _UT_MEM_POOL_CNTR_H_
#define _UT_MEM_POOL_CNTR_H_

#include "../rx_mempool_cntr.h"
#include "../rx_mempool_fx1.h"
#include "../rx_tdd.h"

template<class T>
void test_mem_pool_cntr_base(rx_tdd_base &rt)
{
	T mempool;
	uint32_t bsize1,bsize2;
	void *p1=mempool.do_alloc(bsize1,7);
	rt.assert(bsize1==T::mem_cfg_t::MinAlignSize);
	void *p2=mempool.do_alloc(bsize2,37);
	rt.assert(bsize2==size_align_to(37,T::mem_cfg_t::MinAlignSize));

	mempool.do_free(p1,bsize1);
	mempool.do_free(p1,bsize2);
}


rx_tdd(test_mem_pool_cntr_base)
{
	typedef rx::rx_mempool_cntr_lin<rx::mempool_fixed_t<> > rx_mempool_lin_t;

    test_mem_pool_cntr_base<rx_mempool_lin_t>(*this);
}





#endif // _UT_MEM_POOL_H_
