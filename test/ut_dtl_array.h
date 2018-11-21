#ifndef _RX_UT_DTL_ARRAY_H_
#define _RX_UT_DTL_ARRAY_H_

#include "../rx_tdd.h"
#include "../rx_dtl_array.h"
#include "../rx_dtl_array_ex.h"

namespace rx_ut
{
    //-----------------------------------------------------
    template<class array_t,uint32_t max_items>
    inline void ut_dtl_array_1(rx_tdd_t &rt, array_t &cntr)
    {
        rt.tdd_assert(cntr.size() == 0);
        rt.tdd_assert(cntr.capacity() == max_items);
        rt.tdd_assert(cntr.alias(0) == NULL);
        rt.tdd_assert(cntr.index("") == cntr.capacity());
        rt.tdd_assert(cntr[1] == 0);

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
    rx::alias_array_ft<10> farray;
    farray.set(0);
    rx_ut::ut_dtl_array_1<rx::alias_array_ft<10>,10>(*this, farray);

    rx::alias_array_t<> array;
    array.make(10);
    array.set(0);
    rx_ut::ut_dtl_array_1<rx::alias_array_t<>, 10>(*this, array);

}

#endif
