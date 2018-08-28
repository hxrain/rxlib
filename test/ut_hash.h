#ifndef _RX_UT_HASH_H_
#define _RX_UT_HASH_H_

#include "../rx_cc_macro.h"
#include "../rx_tdd.h"
#include "../rx_hash_int.h"
#include "../rx_hash_rand.h"
#include <time.h>
#include <math.h>

//---------------------------------------------------------
inline void rx_hash_int_base_1(uint32_t seed,const char* hash_name,rx_hash32_func_t hash,rx_tdd_base &rt)
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
inline void rx_hash_int_base_2(uint32_t seed,const char* hash_name,rx_tdd_base &rt)
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

    rx_hash_int_base_2<rx::rand_std>(seed,"rand_std",*this);
    rx_hash_int_base_2<rx::rand_hge>(seed,"rand_hge",*this);
    rx_hash_int_base_2<rx::rand_skeeto_b>(seed,"rand_skeeto_b",*this);
    rx_hash_int_base_2<rx::rand_skeeto_triple>(seed,"rand_skeeto_triple",*this);

    for(int i=0;i<IHT_Count;++i)
        rx_hash_int_base_1(seed,rx_int_hash_name((rx_int_hash_type)i),rx_int_hash((rx_int_hash_type)i),*this);
    srand(seed);
    rx_hash_int_base_1(seed,"std::rand", tmp_test_stdrand,*this);

    rx_rnd_b32().seed(seed);
    rx_rnd_b32().get();

}



#endif
