#ifndef _UT_OS_LOCK_H_
#define _UT_OS_LOCK_H_

#include "../rx_cc_macro.h"
#include "../rx_tdd.h"
#include "../rx_lock_base.h"
#include "../rx_lock_os.h"
#include "../rx_lock_spin.h"

namespace rx
{
    inline void os_lock_base_1(rx_tdd_t &rt)
    {
        locker_t locker;
        {GUARD(locker); }

        GUARD(locker);
        GUARD(locker);
        guard(locker);
    }
}
inline void test_spinlock_base_1(rx_tdd_t &rt)
{
	rx::spin_lock_i lk;
	rt.tdd_assert(lk.lock());
	rt.tdd_assert(!lk.trylock());
	lk.unlock();

	rt.tdd_assert(lk.trylock());
	lk.unlock();
}

inline void test_spinlock_base_2(rx_tdd_t &rt)
{
    rx::spin_lock_t lk;
    rt.tdd_assert(lk.lock());
    rt.tdd_assert(!lk.trylock());
    lk.unlock();

    rt.tdd_assert(lk.trylock());
    lk.unlock();
}

rx_tdd(rx_os_lock_base)
{
    rx::os_lock_base_1(*this);
	test_spinlock_base_1(*this);
    test_spinlock_base_2(*this);
}


#endif
