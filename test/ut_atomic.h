#ifndef _UT_ATOMIC_H_
#define _UT_ATOMIC_H_
#include "../rx_tdd.h"
#include "../rx_atomic.h"

template<class T>
void test_atomic_base(rx_tdd_t &rt)
{
    rx_static_assert(sizeof(T) == 4 || sizeof(T) == 8);

    rx::atomic_t<T> atomic;

    atomic.store((T)1);
    rt.tdd_assert(atomic.load() == 1);
    rt.tdd_assert(atomic.add((T)1) == 1);
    rt.tdd_assert(atomic.load() == 2);
    rt.tdd_assert(atomic.sub((T)1) == 2);
    rt.tdd_assert(atomic.load() == 1);
    rt.tdd_assert(atomic.or_op((T)3) == 1);
    rt.tdd_assert(atomic.load() == 3);
    rt.tdd_assert(atomic.and_op((T)1) == 3);
    rt.tdd_assert(atomic.load() == 1);
    rt.tdd_assert(atomic.xor_op((T)1) == 1);
    rt.tdd_assert(atomic.load() == 0);
    rt.tdd_assert(atomic.swap((T)100) == 0);
    rt.tdd_assert(atomic.load() == 100);
    rt.tdd_assert(atomic.cas((T)100,(T)10));
    rt.tdd_assert(atomic.load() == 10);

    T tmp = 100;
    rt.tdd_assert(!atomic.cas(&tmp,(T)1));
    rt.tdd_assert(tmp == 10);
    rt.tdd_assert(atomic.load() == 10);

    atomic.store((T)-1);
    rt.tdd_assert(atomic.load() == -1);
    rt.tdd_assert(atomic.sub((T)1) == -1);
    rt.tdd_assert(atomic.load() == -2);
    rt.tdd_assert(atomic.add((T)1) == -2);
    rt.tdd_assert(atomic.load() == -1);
    rt.tdd_assert(atomic.add((T)-1) == -1);
    rt.tdd_assert(atomic.load() == -2);
    rt.tdd_assert(atomic.sub((T)-1) == -2);
    rt.tdd_assert(atomic.load() == -1);
    rt.tdd_assert(atomic.add((T)1) == -1);
    rt.tdd_assert(atomic.load() == 0);

    atomic += 100;
    rt.tdd_assert(atomic == 100);
    tmp=atomic ++ ;
    rt.tdd_assert(atomic == 101&&tmp==100);
    tmp = ++atomic;
    rt.tdd_assert(atomic == 102 && tmp == 102);

    tmp = atomic--;
    rt.tdd_assert(atomic == 101 && tmp == 102);
    tmp = --atomic;
    rt.tdd_assert(atomic == 100 && tmp == 100);
    atomic -= 100;
    rt.tdd_assert(atomic == 0);
}

inline void test_spinlock_base_1(rx_tdd_t &rt)
{
    rx::spin_lock lk;
    rt.tdd_assert(lk.lock());
    rt.tdd_assert(!lk.trylock());
    lk.unlock();

    rt.tdd_assert(lk.trylock());
    lk.unlock();
}

rx_tdd(rx_atomic)
{
    test_spinlock_base_1(*this);
    test_atomic_base<int32_t>(*this);
#if RX_ATOMIC64
    test_atomic_base<int64_t>(*this);
#endif
}


#endif
