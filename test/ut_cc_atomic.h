#ifndef _UT_CC_ATOMIC_H_
#define _UT_CC_ATOMIC_H_

#include "../rx_tdd.h"
#include "../rx_assert.h"



template<class T>
void test_cc_atomic_base(rx_tdd_base &tdd)
{
    rx_st_assert(sizeof(T) == 4 || sizeof(T) == 8, "sizeof()==4|8");

    T val = 0;
    rx_atomic_store(&val, (T)1);
    tdd.tdd_assert(rx_atomic_load(&val) == 1);
    tdd.tdd_assert(rx_atomic_add(&val, (T)1) == 1);
    tdd.tdd_assert(rx_atomic_load(&val) == 2);
    tdd.tdd_assert(rx_atomic_sub(&val, (T)1) == 2);
    tdd.tdd_assert(rx_atomic_load(&val) == 1);
    tdd.tdd_assert(rx_atomic_or(&val, (T)3) == 1);
    tdd.tdd_assert(rx_atomic_load(&val) == 3);
    tdd.tdd_assert(rx_atomic_and(&val, (T)1) == 3);
    tdd.tdd_assert(rx_atomic_load(&val) == 1);
    tdd.tdd_assert(rx_atomic_xor(&val, (T)1) == 1);
    tdd.tdd_assert(rx_atomic_load(&val) == 0);
    tdd.tdd_assert(rx_atomic_swap(&val, (T)100) == 0);
    tdd.tdd_assert(rx_atomic_load(&val) == 100);
    tdd.tdd_assert(rx_atomic_cas(&val, (T)100, (T)10));
    tdd.tdd_assert(rx_atomic_load(&val) == 10);

    T tmp = 100;
    tdd.tdd_assert(!rx_atomic_cas(&val, &tmp, (T)1));
    tdd.tdd_assert(tmp == 10);
    tdd.tdd_assert(rx_atomic_load(&val) == 10);

    rx_atomic_store(&val, (T)-1);
    tdd.tdd_assert(rx_atomic_load(&val) == -1);
    tdd.tdd_assert(rx_atomic_sub(&val, (T)1) == -1);
    tdd.tdd_assert(rx_atomic_load(&val) == -2);
    tdd.tdd_assert(rx_atomic_add(&val, (T)1) == -2);
    tdd.tdd_assert(rx_atomic_load(&val) == -1);
    tdd.tdd_assert(rx_atomic_add(&val, (T)-1) == -1);
    tdd.tdd_assert(rx_atomic_load(&val) == -2);
    tdd.tdd_assert(rx_atomic_sub(&val, (T)-1) == -2);
    tdd.tdd_assert(rx_atomic_load(&val) == -1);
    tdd.tdd_assert(rx_atomic_add(&val, (T)1) == -1);
    tdd.tdd_assert(rx_atomic_load(&val) == 0);
}

rx_tdd(rx_cc_atomic)
{
    test_cc_atomic_base<int32_t>(*this);
#if RX_ATOMIC64
    test_cc_atomic_base<int64_t>(*this);
#endif
}







#endif
