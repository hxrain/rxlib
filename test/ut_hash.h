#ifndef _RX_UT_HASH_H_
#define _RX_UT_HASH_H_

#include "../rx_cc_macro.h"
#include "../rx_tdd.h"
#include "../rx_hash_int.h"
#include "../rx_hash_rand.h"
#include <time.h>
#include <math.h>

//---------------------------------------------------------
inline void rx_hash_int_base_1(uint32_t seed,const char* hash_name,rx_int_hash32_t hash,rx_tdd_t &rt)
{
    const uint32_t array_size = 1000;
    const uint32_t loop_count = array_size*100000;

    uint32_t count_array[array_size];
    memset(count_array,0,sizeof(count_array));

    uint32_t h = seed;
    for (uint32_t i = 0; i < loop_count; ++i)
    {
        h = hash(h+i);
        uint32_t idx = h%array_size;
        ++count_array[idx];
    }

    uint32_t want_count = loop_count/ array_size;
    uint64_t variance = 0;
    for (uint32_t i = 0; i < array_size; ++i)
    {
        uint32_t c= count_array[i];
        variance += (want_count - c)*(want_count - c);
    }
    printf("hash  <%30s> sdv = %.4f\n", hash_name, sqrt((double)variance / array_size));
}
//---------------------------------------------------------
template<class rnd_t>
inline void rx_hash_int_base_2(uint32_t seed,const char* hash_name,rx_tdd_t &rt)
{
    const uint32_t array_size = 1000;
    const uint32_t loop_count = array_size*100000;
    rnd_t rnd(seed);

    uint32_t count_array[array_size];
    memset(count_array,0,sizeof(count_array));

    for (uint32_t i = 0; i < loop_count; ++i)
    {
        uint32_t h = rnd.get();
        uint32_t idx = h%array_size;
        ++count_array[idx];
    }

    uint32_t want_count = loop_count/ array_size;
    uint64_t variance = 0;
    for (uint32_t i = 0; i < array_size; ++i)
    {
        uint32_t c= count_array[i];
        variance += (want_count - c)*(want_count - c);
    }
    printf("rnd_t <%30s> sdv = %.4f\n", hash_name, sqrt((double)variance / array_size));
}
//---------------------------------------------------------
inline uint32_t tmp_test_stdrand(uint32_t x)
{
    return rand();
}
//---------------------------------------------------------
rx_tdd_rtl(rx_hash_int_base,tdd_level_slow)
{
    uint32_t seed = (uint32_t)time(NULL);

    rx_hash_int_base_2<rx::rand_std_t>(seed,"rand_std",*this);
    rx_hash_int_base_2<rx::rand_hge_t>(seed,"rand_hge",*this);
    rx_hash_int_base_2<rx::rand_skeeto_b32_t>(seed,"rand_skeeto_b",*this);
    rx_hash_int_base_2<rx::rand_skeeto_triple_t>(seed,"rand_skeeto_triple",*this);
    rx_hash_int_base_2<rx::rand_skiplist_t>(seed,"rand_skiplist_t",*this);

    for(int i=0;i<IHT_Count;++i)
        rx_hash_int_base_1(seed,rx_int_hash32_name((rx_int_hash32_type)i),rx_int_hash32((rx_int_hash32_type)i),*this);
    srand(seed);
    rx_hash_int_base_1(seed,"std::rand", tmp_test_stdrand,*this);

    rx_rnd_b32().seed(seed);
    rx_rnd_b32().get();

}

//---------------------------------------------------------
inline void test_poisson_rnd(rx_tdd_t &rt,uint32_t seed,double lambda=100,int pn_size = 1280,bool is_print=false)
{
    rx::rand_poisson_skt prnd(seed,lambda);
    uint32_t sum=0;
    for (int i = 0; i < pn_size; ++i)
    {
        uint32_t p = prnd.get();
        sum += p;
        if (is_print)
        {
            if (i && i % 16 == 0) 
                printf("\n");
            printf("%3u ", p);
        }
    }
    double avg=sum/(double)pn_size;
    double err=abs(lambda-avg)/pn_size;
    printf("TEST :: rand_poisson_skt(lambda=%.1f),avg=%.4f;loop=%u;e=%.5f\n",lambda,avg,pn_size,err);
    rt.tdd_assert(err<0.0007);          //验证过的均值,与实际期待均值间的偏差,小于此比例(万分之七).

}
//---------------------------------------------------------
rx_tdd_rtl(rx_hash_int_base,tdd_level_std)
{
    rx_int_hash32_t hf=rx_int_hash32_skeeto3s(0);
    hf(0);
    rx_tiny_prime(0);
    rx_tiny_fibonacci(0);

    double tmp = rx::p_poisson_t::pobability(0,3,2);  //= 0.0024787521766663594
    assert(abs(tmp - 0.0024787521766663594)<0.0001);

    for(int i=0;i<10;++i)
        test_poisson_rnd(*this,(uint32_t)time(NULL)+i);
}

#endif
