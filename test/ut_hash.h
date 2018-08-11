#ifndef _RX_UT_HASH_H_
#define _RX_UT_HASH_H_

#include "../rx_tdd.h"
#include "../rx_hash_int.h"

inline void rx_hash_int_base_1(rx_tdd_base &rt)
{
    uint32_t h = 1;
    for (uint32_t i = 0; i < 10; ++i)
    {
        h = rx_hash_skeeto_f(i+h);
        printf("hash(%4u)=0x%08x; mod 10=%4u \n", i, h,h%10);
    }
    for (uint32_t i = 0; i < 10; ++i)
    {
        srand(i);
        printf("rand %4u\n",rand()%10);
    }
}

rx_tdd(rx_hash_int_base)
{
    rx_hash_int_base_1(*this);
}



#endif
