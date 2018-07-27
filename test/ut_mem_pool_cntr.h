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
        //������(ÿ��ֵ������Ϊ2����������):��С�ڵ��������ߴ�;���ڵ�ߴ�;ÿ���ڴ��������ߴ�
        //��Щ���������þ����������ڴ�ռ���ʺͻ����������.
        //���ߴ��һЩʱ�ʺϵȳ����ڴ���ܼ�����,����������ȿ����ʱ�ή���ڴ�������.
        enum {
            MinAlignSize = 64,
            MaxNodeSize = 1024 * 16,
            MaxStripeSize = MaxNodeSize * 8
        };

        //У����С����ߴ�Ϸ���,ȷ��MinAlignSize��2����������
        enum { MinSizeShiftBit = rx::log2<MinAlignSize>::result };
        //У�����ڵ�ߴ�Ϸ���,ȷ��MaxNodeSize��2����������
        enum { MaxNodeSizeShiftBit = rx::log2<MaxNodeSize>::result };
    }cfg_t;

	typedef rx::mempool_cntr_lin<rx::mempool_fixed_t<cfg_t> > rx_mempool_lin_t;
    typedef rx::mempool_cntr_pow2<rx::mempool_fixed_t<cfg_t> > rx_mempool_pow2_t;

    test_mem_pool_cntr_lin<rx_mempool_lin_t>(*this);
    test_mem_pool_cntr_pow2<rx_mempool_pow2_t>(*this);
}





#endif // _UT_MEM_POOL_H_
