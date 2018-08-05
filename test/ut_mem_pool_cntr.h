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
	rt.assert(bsize1==T::mem_cfg_t::MinNodeSize);
	void *p2=mempool.do_alloc(bsize2,37);
	rt.assert(bsize2==rx::size_align_to(37,T::mem_cfg_t::MinNodeSize));

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
    rt.assert(bsize1 == T::mem_cfg_t::MinNodeSize);
    void *p2 = mempool.do_alloc(bsize2, 64);
    rt.assert(bsize2 == T::mem_cfg_t::MinNodeSize);

    mempool.do_free(p1, bsize1);
    mempool.do_free(p2, bsize2);

    p1 = mempool.do_alloc(bsize1, 128);
    rt.assert(bsize1 == T::mem_cfg_t::MinNodeSize<<1);
    p2 = mempool.do_alloc(bsize2, 129);
    rt.assert(bsize2 == T::mem_cfg_t::MinNodeSize<<2);

    mempool.do_free(p1, bsize1);
    mempool.do_free(p2, bsize2);
}
//---------------------------------------------------------
template<class T>
void test_mem_pool_cntr_tlmap(rx_tdd_base &rt)
{
    T mempool;
    uint32_t bsize1, bsize2;
    void *p1 = mempool.do_alloc(bsize1, 7);
    rt.assert(bsize1 == T::mem_cfg_t::MinNodeSize);
    void *p2 = mempool.do_alloc(bsize2, 64);
    rt.assert(bsize2 == T::mem_cfg_t::MinNodeSize);

    mempool.do_free(p1, bsize1);
    mempool.do_free(p2, bsize2);

    p1 = mempool.do_alloc(bsize1, 128);
    rt.assert(bsize1 == T::mem_cfg_t::MinNodeSize<<1);
    p2 = mempool.do_alloc(bsize2, 129);
    rt.assert(bsize2 == 144);

    mempool.do_free(p1, bsize1);
    mempool.do_free(p2, bsize2);

    p1 = mempool.do_alloc(bsize1, 127);
    rt.assert(bsize1 == T::mem_cfg_t::MinNodeSize<<1);
    p2 = mempool.do_alloc(bsize2, 143);
    rt.assert(bsize2 == 144);
    mempool.do_free(p1, bsize1);
    mempool.do_free(p2, bsize2);

    p1 = mempool.do_alloc(bsize1, 144);
    rt.assert(bsize1 == 144);
    p2 = mempool.do_alloc(bsize2, 145);
    rt.assert(bsize2 == 160);
    mempool.do_free(p1, bsize1);
    mempool.do_free(p2, bsize2);
}
//---------------------------------------------------------
typedef struct test_mp_cfg_t
{
    //������(ÿ��ֵ������Ϊ2����������):��С�ڵ��������ߴ�;���ڵ�ߴ�;ÿ���ڴ��������ߴ�
    //��Щ���������þ����������ڴ�ռ���ʺͻ����������.
    //���ߴ��һЩʱ�ʺϵȳ����ڴ���ܼ�����,����������ȿ����ʱ�ή���ڴ�������.
    enum {
        MinNodeSize = 64,
        MaxNodeSize = 1024 * 16,
        StripeAlignSize = MaxNodeSize * 8
    };

    //У����С����ߴ�Ϸ���,ȷ��MinNodeSize��2����������
    enum { MinNodeShiftBit = rx::log2<MinNodeSize>::result };
    //У�����ڵ�ߴ�Ϸ���,ȷ��MaxNodeSize��2����������
    enum { MaxNodeShiftBit = rx::log2<MaxNodeSize>::result };
}test_mp_cfg_t;

//---------------------------------------------------------
rx_tdd(test_mem_pool_cntr_base)
{
	typedef rx::mempool_cntr_lin<rx::mempool_fixed_t<test_mp_cfg_t>,test_mp_cfg_t> rx_mempool_lin_t;
    typedef rx::mempool_cntr_pow2<rx::mempool_fixed_t<test_mp_cfg_t>,test_mp_cfg_t> rx_mempool_pow2_t;
    typedef rx::mempool_cntr_tlmap<rx::mempool_fixed_t<test_mp_cfg_t>,test_mp_cfg_t> rx_mempool_tlmap_t;

    test_mem_pool_cntr_lin<rx_mempool_lin_t>(*this);
    test_mem_pool_cntr_pow2<rx_mempool_pow2_t>(*this);
    test_mem_pool_cntr_tlmap<rx_mempool_tlmap_t>(*this);
}





#endif // _UT_MEM_POOL_H_
