#ifndef _RX_UT_DTL_ARRAY_H_
#define _RX_UT_DTL_ARRAY_H_

#include "../rx_tdd.h"
#include "../rx_dtl_array.h"
#include "../rx_dtl_array_ex.h"

namespace rx_ut
{
    //-----------------------------------------------------
    inline void ut_dtl_array_1(rx_tdd_t &rt)
    {
        const uint32_t max_items = 10;
        typedef rx::alias_array_ft<max_items> array_t;
        array_t cntr;
        rt.tdd_assert(cntr.size() == 0);
        rt.tdd_assert(cntr.capacity() == max_items);
        rt.tdd_assert(cntr.alias(0) == NULL);
        rt.tdd_assert(cntr.index("") == cntr.capacity());
        rt.tdd_assert(cntr[1] != NULL);

        rt.tdd_assert(cntr.bind(0, "0"));
        rt.tdd_assert(cntr.size() == 1);
        rt.tdd_assert(rx::st::strcmp(cntr.alias(0),"0")==0);
        rt.tdd_assert(cntr.index("0") == 0);

        rt.tdd_assert(cntr.bind(1, "1"));
        rt.tdd_assert(cntr.size() == 2);
        rt.tdd_assert(rx::st::strcmp(cntr.alias(1), "1") == 0);
        rt.tdd_assert(cntr.index("1") == 1);

    }
}

rx_tdd(ut_dtl_array)
{
    rx_ut::ut_dtl_array_1(*this);
}

#endif
