#ifndef _UT_MEM_POOL_CNTR_H_
#define _UT_MEM_POOL_CNTR_H_

#include "../rx_mem_pool_cntr.h"
#include "../rx_mem_pool_fx1.h"
#include "../rx_tdd.h"

//---------------------------------------------------------
template<class T>
void test_mem_pool_cntr_lin(rx_tdd_t &rt)
{
	T mempool;
	uint32_t bsize1,bsize2;
	void *p1=mempool.do_alloc(bsize1,7);
	rt.tdd_assert(bsize1==T::mem_cfg_t::MinNodeSize);
	void *p2=mempool.do_alloc(bsize2,37);
	rt.tdd_assert(bsize2==rx::size_align_to(37,T::mem_cfg_t::MinNodeSize));

	mempool.do_free(p1,bsize1);
	mempool.do_free(p2,bsize2);
}

//---------------------------------------------------------
template<class T>
void test_mem_pool_cntr_pow2(rx_tdd_t &rt)
{
    T mempool;
    uint32_t bsize1, bsize2;
    void *p1 = mempool.do_alloc(bsize1, 7);
    rt.tdd_assert(bsize1 == T::mem_cfg_t::MinNodeSize);
    void *p2 = mempool.do_alloc(bsize2, 64);
    rt.tdd_assert(bsize2 == T::mem_cfg_t::MinNodeSize);

    mempool.do_free(p1, bsize1);
    mempool.do_free(p2, bsize2);

    p1 = mempool.do_alloc(bsize1, 128);
    rt.tdd_assert(bsize1 == T::mem_cfg_t::MinNodeSize<<1);
    p2 = mempool.do_alloc(bsize2, 129);
    rt.tdd_assert(bsize2 == T::mem_cfg_t::MinNodeSize<<2);

    mempool.do_free(p1, bsize1);
    mempool.do_free(p2, bsize2);
}
//---------------------------------------------------------
template<class T>
void test_mem_pool_cntr_tlmap(rx_tdd_t &rt)
{
    T mempool;
    uint32_t bsize1, bsize2;
    void *p1 = mempool.do_alloc(bsize1, 7);
    rt.tdd_assert(bsize1 == 64);
    void *p2 = mempool.do_alloc(bsize2, 64);
    rt.tdd_assert(bsize2 == 64);

    mempool.do_free(p1, bsize1);
    mempool.do_free(p2, bsize2);

    p1 = mempool.do_alloc(bsize1, 128);
    rt.tdd_assert(bsize1 == 128);
    p2 = mempool.do_alloc(bsize2, 129);
    rt.tdd_assert(bsize2 == 192);

    mempool.do_free(p1, bsize1);
    mempool.do_free(p2, bsize2);

    p1 = mempool.do_alloc(bsize1, 127);
    rt.tdd_assert(bsize1 == 128);
    p2 = mempool.do_alloc(bsize2, 143);
    rt.tdd_assert(bsize2 == 192);
    mempool.do_free(p1, bsize1);
    mempool.do_free(p2, bsize2);

    p1 = mempool.do_alloc(bsize1, 144);
    rt.tdd_assert(bsize1 == 192);
    p2 = mempool.do_alloc(bsize2, 145);
    rt.tdd_assert(bsize2 == 192);
    mempool.do_free(p1, bsize1);
    mempool.do_free(p2, bsize2);
}
template<class T>
void test_mem_pool_cntr_tlmap2(rx_tdd_t &rt)
{
    T mempool;
    uint32_t bsize1, bsize2;
    void *p1 = mempool.do_alloc(bsize1, 7);
    rt.tdd_assert(bsize1 == 64);
    void *p2 = mempool.do_alloc(bsize2, 64);
    rt.tdd_assert(bsize2 == 64);

    mempool.do_free(p1, bsize1);
    mempool.do_free(p2, bsize2);

    p1 = mempool.do_alloc(bsize1, 128);
    rt.tdd_assert(bsize1 == 128);
    p2 = mempool.do_alloc(bsize2, 129);
    rt.tdd_assert(bsize2 == 144);

    mempool.do_free(p1, bsize1);
    mempool.do_free(p2, bsize2);

    p1 = mempool.do_alloc(bsize1, 127);
    rt.tdd_assert(bsize1 == 128);
    p2 = mempool.do_alloc(bsize2, 143);
    rt.tdd_assert(bsize2 == 144);
    mempool.do_free(p1, bsize1);
    mempool.do_free(p2, bsize2);

    p1 = mempool.do_alloc(bsize1, 144);
    rt.tdd_assert(bsize1 == 144);
    p2 = mempool.do_alloc(bsize2, 145);
    rt.tdd_assert(bsize2 == 160);
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
    enum { MinNodeShiftBit = rx::LOG2<MinNodeSize>::result };
    //У�����ڵ�ߴ�Ϸ���,ȷ��MaxNodeSize��2����������
    enum { MaxNodeShiftBit = rx::LOG2<MaxNodeSize>::result };
}test_mp_cfg_t;

//---------------------------------------------------------
rx_tdd(test_mem_pool_cntr_base)
{
	typedef rx::mempool_cntr_lin_t<rx::mempool_fixed_t<test_mp_cfg_t>,test_mp_cfg_t> rx_mempool_lin_t;
    typedef rx::mempool_cntr_pow2_t<rx::mempool_fixed_t<test_mp_cfg_t>,test_mp_cfg_t> rx_mempool_pow2_t;
    typedef rx::mempool_cntr_tlmap_t<rx::mempool_fixed_t<test_mp_cfg_t>,test_mp_cfg_t> rx_mempool_tlmap_t;

    test_mem_pool_cntr_lin<rx_mempool_lin_t>(*this);
    test_mem_pool_cntr_pow2<rx_mempool_pow2_t>(*this);
    test_mem_pool_cntr_tlmap2<rx_mempool_tlmap_t>(*this);
}





#endif // _UT_MEM_POOL_H_
