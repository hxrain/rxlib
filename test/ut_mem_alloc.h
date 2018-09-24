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

template<class ma_t,uint32_t array_size=5000,uint32_t loop_test=5000,uint32_t max_mem_size=1024*16>
class test_mem_alloc_a
{
public:
    static void test(rx_tdd_t &rt,const char* ms_type,uint32_t seed)
    {
        tdd_tt(tt,"mem_alloc_test",ms_type);
        tdd_tt_hit(tt,"array_size=%u,loop_test=%u,max_mem_size=%u",array_size,loop_test,max_mem_size);
        tdd_tt_hit(tt,"tmp1");
        tdd_tt_hit(tt,"tmp2");
        ptr_t   ptr_array[array_size];
        rx::rand_skeeto_bsa_t rnd;
        ma_t ma;
        rnd.seed(seed);

        for(uint32_t wl=0;wl<loop_test;++wl)
        {
            for(uint32_t l=0;l<array_size;++l)
            {
                ptr_array[l]=ma.alloc(rnd.get(max_mem_size));
            }
            for(uint32_t l=0;l<array_size;++l)
            {
                ma.free(ptr_array[l]);
            }
        }

    }

};


rx_tdd(test_mem_alloc_base)
{
    test_mem_alloc_base(*this);
    uint32_t seed=(uint32_t)time(NULL);
    test_mem_alloc_a<rx::mem_allotter_lin_slt,100,200>::test(*this,"mem_allotter_lin_slt",seed);
    test_mem_alloc_a<rx::mem_allotter_pow2_slt,100,200>::test(*this,"mem_allotter_pow2_slt",seed);
    test_mem_alloc_a<rx::mem_allotter_tlmap_slt,100,200>::test(*this,"mem_allotter_tlmap_slt",seed);
    test_mem_alloc_a<rx::mem_allotter_std_t,100,200>::test(*this,"mem_allotter_std",seed);

}


rx_tdd_rtl(test_mem_alloc_base,tdd_level_slow)
{
    uint32_t seed=(uint32_t)time(NULL);
    test_mem_alloc_a<rx::mem_allotter_lin_slt>::test(*this,"mem_allotter_lin_slt",seed);
    test_mem_alloc_a<rx::mem_allotter_pow2_slt>::test(*this,"mem_allotter_pow2_slt",seed);
    test_mem_alloc_a<rx::mem_allotter_tlmap_slt>::test(*this,"mem_allotter_tlmap_slt",seed);
    test_mem_alloc_a<rx::mem_allotter_std_t>::test(*this,"mem_allotter_std",seed);

}


#endif // _UT_MEM_POOL_H_
