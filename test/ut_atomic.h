#ifndef _UT_ATOMIC_H_
#define _UT_ATOMIC_H_
#include "../rx_tdd.h"
#include "../rx_atomic.h"

template<class T>
void test_atomic_base(rx_tdd_base &tdd)
{
    rx_st_assert(sizeof(T) == 4 || sizeof(T) == 8,"sizeof()==4|8");

    rx::atomic<T> atomic;

    atomic.store((T)1);
    tdd.assert(atomic.load() == 1);
    tdd.assert(atomic.add((T)1) == 1);
    tdd.assert(atomic.load() == 2);
    tdd.assert(atomic.sub((T)1) == 2);
    tdd.assert(atomic.load() == 1);
    tdd.assert(atomic.or((T)3) == 1);
    tdd.assert(atomic.load() == 3);
    tdd.assert(atomic.and((T)1) == 3);
    tdd.assert(atomic.load() == 1);
    tdd.assert(atomic.xor((T)1) == 1);
    tdd.assert(atomic.load() == 0);
    tdd.assert(atomic.swap((T)100) == 0);
    tdd.assert(atomic.load() == 100);
    tdd.assert(atomic.cas((T)100,(T)10));
    tdd.assert(atomic.load() == 10);

    T tmp = 100;
    tdd.assert(!atomic.cas(&tmp,(T)1));
    tdd.assert(tmp == 10);
    tdd.assert(atomic.load() == 10);

    atomic.store((T)-1);
    tdd.assert(atomic.load() == -1);
    tdd.assert(atomic.sub((T)1) == -1);
    tdd.assert(atomic.load() == -2);
    tdd.assert(atomic.add((T)1) == -2);
    tdd.assert(atomic.load() == -1);
    tdd.assert(atomic.add((T)-1) == -1);
    tdd.assert(atomic.load() == -2);
    tdd.assert(atomic.sub((T)-1) == -2);
    tdd.assert(atomic.load() == -1);
    tdd.assert(atomic.add((T)1) == -1);
    tdd.assert(atomic.load() == 0);

    atomic += 100;
    tdd.assert(atomic == 100);
    tmp=atomic ++ ;
    tdd.assert(atomic == 101&&tmp==100);
    tmp = ++atomic;
    tdd.assert(atomic == 102 && tmp == 102);

    tmp = atomic--;
    tdd.assert(atomic == 101 && tmp == 102);
    tmp = --atomic;
    tdd.assert(atomic == 100 && tmp == 100);
    atomic -= 100;
    tdd.assert(atomic == 0);
}

rx_tdd(rx_atomic)
{
    test_atomic_base<int32_t>(*this);
    test_atomic_base<int64_t>(*this);
}


#endif