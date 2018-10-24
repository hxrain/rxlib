#ifndef _UT_MEM_ALLOC_H_
#define _UT_MEM_ALLOC_H_

#include "../rx_mem_alloc_cntr.h"
#include "../rx_tdd.h"
#include "../rx_hash_rand.h"

#define RX_UT_TEST_MEM_BAD 0

void test_mem_alloc_base(rx_tdd_t &rt)
{
    uint32_t *dp1=rx_new(uint32_t);
    rt.tdd_assert(dp1!=NULL);
    rt.tdd_assert(rx_mem().memsize(dp1)>sizeof(uint32_t));
    rt.tdd_assert(rx_mem().usrsize(dp1)>=sizeof(uint32_t));
    rx_delete(dp1);

    void *p1=rx_alloc(5);
    rt.tdd_assert(p1!=NULL);
    rt.tdd_assert(rx_mem().memsize(p1)>5);
    rt.tdd_assert(rx_mem().usrsize(p1)>=5);

    void *p2=rx_realloc(p1,6);
    rt.tdd_assert(p2!=NULL);

    uint32_t us=rx_mem().usrsize(p2);
    rt.tdd_assert(rx_mem().memsize(p2)>6);
    rt.tdd_assert(us>=6);

#if RX_UT_TEST_MEM_BAD
    //进行内存溢出覆盖的测试
    memset(p2,0,us+1);
#endif
    rx_free(p2);

#if RX_UT_TEST_MEM_BAD
    //进行double free测试
    rx_free(p2);
#endif
}

template<class ma_t,uint32_t array_size,uint32_t loop_test,uint32_t max_mem_size>
class test_mem_alloc_a
{
public:
    static void test(rx_tdd_t &rt,const char* ms_type,uint32_t seed)
    {
        tdd_tt(tt,ms_type, "mem_alloc_test");
        ptr_t   ptr_array[array_size];
        rx::rand_skeeto_bsa_t rnd;
        ma_t ma;
        rnd.seed(seed);

        for(uint32_t wl=0;wl<loop_test;++wl)
        {
            for(uint32_t l=0;l<array_size;++l)
            {
                uint32_t s = rnd.get(max_mem_size);
                ptr_array[l]=ma.alloc(s);
                rx_assert(ptr_array[l]!=NULL);
            }
            for(uint32_t l=0;l<array_size;++l)
            {
                ma.free(ptr_array[l]);
            }
        }
        tdd_tt_tab(tt, "array_size=%u,loop_test=%u,max_mem_size=%u", array_size, loop_test, max_mem_size);
    }

};

//默认内存池的配置参数(每个值都必须为2的整数次幂)
typedef struct ut_mempool_cfg_h4t
{
    enum
    {
        //可缓存的最小节点尺寸
        MinNodeSize = 32,
        //可缓存的最大节点尺寸
        MaxNodeSize = 1024,
        //每个内存条的最大或对齐尺寸
        StripeAlignSize = MaxNodeSize * 128
    };

    //默认不需改动:log2(最小对齐尺寸),确定MinNodeSize是2的整数次幂
    enum { MinNodeShiftBit = rx::LOG2<MinNodeSize>::result };
    //默认不需改动:log2(最大节点尺寸),确定MaxNodeSize是2的整数次幂
    enum { MaxNodeShiftBit = rx::LOG2<MaxNodeSize>::result };
} ut_mempool_cfg_h4t;

desc_mem_allotter_h4slt(ut_mem_allotter_h4_slt, ut_mempool_cfg_h4t, rx::mempool_h4f_t);
desc_mem_allotter_h4(ut_mem_allotter_h4, ut_mempool_cfg_h4t, rx::mempool_h4f_t);
desc_mem_allotter_h4slt(ut_mem_allotter_h4x_slt, ut_mempool_cfg_h4t, rx::mempool_h4fx_t);
desc_mem_allotter_h4(ut_mem_allotter_h4x, ut_mempool_cfg_h4t, rx::mempool_h4fx_t);

//---------------------------------------------------------
rx_tdd(test_mem_alloc_base)
{
    const uint32_t arr_count = 100;
    const uint32_t loop_count = 5000;
    const uint32_t max_size = ut_mempool_cfg_h4t::MaxNodeSize;

    test_mem_alloc_base(*this);
    uint32_t seed=(uint32_t)time(NULL);
    test_mem_alloc_a<rx::mem_allotter_lin_slt,arr_count,loop_count,max_size>::      test(*this, "   mem_allotter_lin_slt", seed);
    test_mem_alloc_a<rx::mem_allotter_lin_t, arr_count, loop_count, max_size>::     test(*this, "   mem_allotter_lin    ", seed);
    test_mem_alloc_a<rx::mem_allotter_pow2_slt,arr_count,loop_count,max_size>::     test(*this, "  mem_allotter_pow2_slt", seed);
    test_mem_alloc_a<rx::mem_allotter_pow2_t, arr_count, loop_count, max_size>::    test(*this, "  mem_allotter_pow2    ", seed);
    test_mem_alloc_a<rx::mem_allotter_tlmap_slt,arr_count,loop_count,max_size>::    test(*this, " mem_allotter_tlmap_slt", seed);
    test_mem_alloc_a<rx::mem_allotter_tlmap_t, arr_count, loop_count, max_size>::   test(*this, " mem_allotter_tlmap    ", seed);
    test_mem_alloc_a<rx::mem_allotter_std_t,arr_count,loop_count,max_size>::        test(*this, "       mem_allotter_std", seed);

    test_mem_alloc_a<ut_mem_allotter_h4_slt, arr_count, loop_count,max_size>::      test(*this, "ut_mem_allotter_h4_slt ", seed);
    test_mem_alloc_a<ut_mem_allotter_h4, arr_count, loop_count,max_size>::          test(*this, "ut_mem_allotter_h4     ", seed);
    test_mem_alloc_a<ut_mem_allotter_h4x_slt, arr_count, loop_count, max_size>::    test(*this, "ut_mem_allotter_h4x_slt", seed);
    test_mem_alloc_a<ut_mem_allotter_h4x, arr_count, loop_count, max_size>::        test(*this, "ut_mem_allotter_h4x    ", seed);
}

//---------------------------------------------------------
rx_tdd_rtl(test_mem_alloc_base,tdd_level_slow)
{
    const uint32_t arr_count = 5000;
    const uint32_t loop_count = 5000;
    const uint32_t max_size = 1024*16;

    uint32_t seed=(uint32_t)time(NULL);
    test_mem_alloc_a<rx::mem_allotter_lin_slt,arr_count,loop_count,max_size>::    test(*this,"  mem_allotter_lin_slt", seed);
    test_mem_alloc_a<rx::mem_allotter_pow2_slt,arr_count,loop_count,max_size>::   test(*this," mem_allotter_pow2_slt", seed);
    test_mem_alloc_a<rx::mem_allotter_tlmap_slt,arr_count,loop_count,max_size>::  test(*this,"mem_allotter_tlmap_slt", seed);
    test_mem_alloc_a<rx::mem_allotter_std_t,arr_count,loop_count,max_size>::      test(*this,"      mem_allotter_std", seed);
}


#endif // _UT_MEM_POOL_H_
