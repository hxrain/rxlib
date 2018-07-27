#ifndef _UT_MEM_POOL_CNTR_H_
#define _UT_MEM_POOL_CNTR_H_

#include "../rx_mempool_cntr.h"
#include "../rx_mempool_fx1.h"
#include "../rx_tdd.h"

//---------------------------------------------------------
template<class T>
void test_mem_pool_cntr_lin(rx_tdd_base &rt)
{
	T mempool;
	uint32_t bsize1,bsize2;
	void *p1=mempool.do_alloc(bsize1,7);
	rt.assert(bsize1==T::mem_cfg_t::MinAlignSize);
	void *p2=mempool.do_alloc(bsize2,37);
	rt.assert(bsize2==size_align_to(37,T::mem_cfg_t::MinAlignSize));

	mempool.do_free(p1,bsize1);
	mempool.do_free(p2,bsize2);
}

//---------------------------------------------------------
template<class T>
void test_mem_pool_cntr_pow2(rx_tdd_base &rt)
{
    T mempool;
    uint32_t bsize1, bsize2;
    void *p1 = mempool.do_alloc(bsize1, 7);
    rt.assert(bsize1 == T::mem_cfg_t::MinAlignSize);
    void *p2 = mempool.do_alloc(bsize2, 64);
    rt.assert(bsize2 == T::mem_cfg_t::MinAlignSize);

    mempool.do_free(p1, bsize1);
    mempool.do_free(p2, bsize2);

    p1 = mempool.do_alloc(bsize1, 128);
    rt.assert(bsize1 == T::mem_cfg_t::MinAlignSize<<1);
    p2 = mempool.do_alloc(bsize2, 129);
    rt.assert(bsize2 == T::mem_cfg_t::MinAlignSize<<2);

    mempool.do_free(p1, bsize1);
    mempool.do_free(p2, bsize2);
}

//---------------------------------------------------------
rx_tdd(test_mem_pool_cntr_base)
{
    typedef struct cfg_t
    {
        //可配置(每个值都必须为2的整数次幂):最小节点与增量尺寸;最大节点尺寸;每个内存条的最大尺寸
        //这些参数的配置决定了物理内存占用率和缓存池利用率.
        //条尺寸大一些时适合等长度内存块密集分配,但在随机长度块分配时会降低内存利用率.
        enum {
            MinAlignSize = 64,
            MaxNodeSize = 1024 * 16,
            MaxStripeSize = MaxNodeSize * 8
        };

        //校验最小对齐尺寸合法性,确定MinAlignSize是2的整数次幂
        enum { MinSizeShiftBit = rx::log2<MinAlignSize>::result };
        //校验最大节点尺寸合法性,确定MaxNodeSize是2的整数次幂
        enum { MaxNodeSizeShiftBit = rx::log2<MaxNodeSize>::result };
    }cfg_t;

	typedef rx::mempool_cntr_lin<rx::mempool_fixed_t<cfg_t> > rx_mempool_lin_t;
    typedef rx::mempool_cntr_pow2<rx::mempool_fixed_t<cfg_t> > rx_mempool_pow2_t;

    test_mem_pool_cntr_lin<rx_mempool_lin_t>(*this);
    test_mem_pool_cntr_pow2<rx_mempool_pow2_t>(*this);
}





#endif // _UT_MEM_POOL_H_
