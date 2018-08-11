#ifndef _RX_UT_HASH_H_
#define _RX_UT_HASH_H_

#include "../rx_tdd.h"
#include "../rx_hash_int.h"


inline void rx_hash_int_base_1(uint32_t seed,const char* hash_name,rx_hash32_func_t hash,rx_tdd_base &rt)
{
    const uint32_t array_size = 100;
    const uint32_t loop_count = 50000000;

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
        //printf("hash func <%20s> num<%3u> count = %8u\n",hash_name,i,c);
        variance += (want_count - c)*(want_count - c);
    }
    printf("hash func <%20s> var = %.4f\n", hash_name, (double)variance / loop_count);
}

inline uint32_t tmp_test_stdrand(uint32_t x)
{
    return rand();
}

rx_tdd_rtl(rx_hash_int_base,rtl_3)
{
    uint32_t seed = (uint32_t)time(NULL);
    for(int i=0;i<IHT_Count;++i)
        rx_hash_int_base_1(seed,rx_int_hash_name((rx_int_hash_type)i),rx_int_hash((rx_int_hash_type)i),*this);
    srand(seed);
    rx_hash_int_base_1(seed,"stdrand", tmp_test_stdrand,*this);
}



#endif
