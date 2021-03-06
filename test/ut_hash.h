#ifndef _RX_UT_HASH_H_
#define _RX_UT_HASH_H_

#include "../rx_cc_macro.h"
#include "../rx_tdd.h"
#include "../rx_hash_int.h"
#include "../rx_hash_rand.h"
#include <time.h>
#include <math.h>

const bool ut_show_hash_debug = false;

//---------------------------------------------------------
//计算指定哈希函数hash的仿rnd生成器的均方差
template<uint32_t loop,uint32_t arrsize>
inline void rx_hash_int_base_1(uint32_t seed,const char* hash_name,rx_int_hash32_t hash,rx_tdd_t &rt)
{
    const uint32_t array_size = arrsize;
    const uint32_t loop_count = array_size*loop;

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
    tdd_print(ut_show_hash_debug,"hash  <%30s> sdv = %.4f\n", hash_name, sqrt((double)variance / array_size));
}
//---------------------------------------------------------
//计算指定rnd_t随机数生成器的均方差
template<class rnd_t, uint32_t loop,uint32_t arrsize>
inline void rx_hash_int_base_2(uint32_t seed,const char* hash_name,rx_tdd_t &rt)
{
    const uint32_t array_size = arrsize;
    const uint32_t loop_count = array_size*loop;
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
    tdd_print(ut_show_hash_debug, "rnd_t <%30s> sdv = %.4f\n", hash_name, sqrt((double)variance / array_size));
}
//---------------------------------------------------------
inline uint32_t tmp_test_stdrand(uint32_t x)
{
    return rand();
}

//---------------------------------------------------------
//计算泊松分布随机数发生器的偏差
inline void test_poisson_rnd(rx_tdd_t &rt, uint32_t seed, double lambda = 100, int pn_size = 1280, bool is_print = false)
{
    rx::rand_poisson_skt prnd(seed, lambda);
    uint32_t sum = 0;
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
    double avg = sum / (double)pn_size;
    double err = fabs(lambda - avg) / pn_size;
    tdd_print(ut_show_hash_debug, "TEST :: rand_poisson_skt(lambda=%.1f),avg=%.4f;loop=%u;err=%.4f%%\n", lambda, avg, pn_size, err * 100);
    rt.tdd_assert(err<0.00075);          //验证过的均值,与实际期待均值间的偏差,小于此比例(万分之七).
}

//---------------------------------------------------------
//对多种随机数发生器进行统一测试
template<uint32_t loop,uint32_t arrsize>
inline void test_rnd_sdv(rx_tdd_t &rt,uint32_t seed)
{
    rx_hash_int_base_2<rx::rand_std_t,loop,arrsize>(seed, "rand_std", rt);
    rx_hash_int_base_2<rx::rand_hge_t, loop,arrsize>(seed, "rand_hge", rt);
    rx_hash_int_base_2<rx::rand_skeeto_bsa_t, loop,arrsize>(seed, "rand_skeeto_bsa", rt);
    rx_hash_int_base_2<rx::rand_skeeto_triple_t, loop,arrsize>(seed, "rand_skeeto_triple", rt);
    rx_hash_int_base_2<rx::rand_skl_t, loop,arrsize>(seed, "rand_skl_t", rt);

    for (int i = 0; i<IHT_Count; ++i)
        rx_hash_int_base_1<loop,arrsize>(seed, rx_int_hash32_name((rx_int_hash32_type)i), rx_int_hash32((rx_int_hash32_type)i), rt);
    srand(seed);
    rx_hash_int_base_1<loop,arrsize>(seed, "std::rand", tmp_test_stdrand, rt);

    rx_rnd_b32().seed(seed);
    rx_rnd_b32().get();
    tdd_print(ut_show_hash_debug, "< rand/hash > LOOP:%u,ARRSize:%u\n",loop,arrsize);
}
//---------------------------------------------------------
//计算指定哈希函数hash值并打印显示.
template<uint32_t loop>
inline void rx_test_hash_int_show_1(uint32_t seed, const char* hash_name, rx_int_hash32_t hash, rx_tdd_t &rt)
{
    tdd_print(ut_show_hash_debug, "hash  <%30s> result (%u)\n", hash_name, loop);
    uint32_t h = seed;
    for (uint32_t i = 0; i < loop; ++i)
    {
        h = hash(h + i);
        if (i&&i % 8 == 0) printf("\n");
        printf("%6u : %08x",i,h);
    }
    tdd_print(ut_show_hash_debug, "\n");
}

//---------------------------------------------------------
//较慢的随机数均方差验证性测试
rx_tdd_rtl(rx_hash_int_base,tdd_level_slow)
{
    uint32_t seed = (uint32_t)time(NULL);
    test_rnd_sdv<10000,5000>(*this,seed);
}

//---------------------------------------------------------
//hash和rand的基础测试用例
rx_tdd_rtl(rx_hash_int_base,tdd_level_std)
{
    uint32_t seed = (uint32_t)time(NULL);
#if RX_CC_DEBUG
    test_rnd_sdv<1000, 400>(*this, seed);
#else
    test_rnd_sdv<1000, 4000>(*this, seed);
#endif

    //rx_test_hash_int_show_1<10000>(0, "rand_skeeto_triple", rx_int_hash32_skeeto3s(0),*this);

    rx_int_hash32_t hf=rx_int_hash32_skeeto3s(0);
    hf(0);
    rx_tiny_prime(0);
    rx_tiny_fibonacci(0);

    double tmp = rx::p_poisson_t::pobability(0,3,2);  //= 0.0024787521766663594
    assert(fabs(tmp - 0.0024787521766663594)<0.0001);

    for(int i=0;i<10;++i)
        test_poisson_rnd(*this,(uint32_t)time(NULL)+i);
}

#endif
