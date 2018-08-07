#ifndef _UT_OS_LOCK_H_
#define _UT_OS_LOCK_H_

#include "../rx_tdd.h"
#include "../rx_os_lock.h"

rx_tdd(rx_os_lock_base)
{
    using namespace rx;
    locker_t locker;
    {GUARD(locker); }

    GUARD(locker);
    GUARD(locker);
    guard(locker);
}


#endif
