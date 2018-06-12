#ifndef _UT_BITS_OP_H_
#define _UT_BITS_OP_H_

#include "../rx_bits_op.h"
#include "../rx_tdd.h"

rx_tdd(bits_op_base)
{
    uint32_t v;
    v=rx_byte_flip<uint32_t,1>();
    tdd_assert(v==0x01010101);
    v=rx_byte_flip<uint32_t,2>();
    tdd_assert(v==0x02020202);
    v=rx_byte_flip<uint32_t,0x23>();
    tdd_assert(v==0x23232323);

    tdd_assert(rx_popcnt((uint32_t)0)==0);
    tdd_assert(rx_popcnt((uint32_t)1)==1);
    tdd_assert(rx_popcnt((uint32_t)3)==2);
    tdd_assert(rx_popcnt((uint32_t)0x10101010)==4);

    tdd_assert(rx_popcnt((uint64_t)0)==0);
    tdd_assert(rx_popcnt((uint64_t)1)==1);
    tdd_assert(rx_popcnt((uint64_t)3)==2);
    tdd_assert(rx_popcnt((uint64_t)0x10101010)==4);

    uint64_t ve;
    ve=rx_clz((uint64_t)0x7000000000000000);
    tdd_assert(ve==1);
    ve=rx_clz((uint32_t)0x70000000);
    tdd_assert(ve==1);
    ve=rx_clz((uint64_t)0);
    tdd_assert(ve==64);
    ve=rx_clz((uint64_t)1);
    tdd_assert(ve==63);
    ve=rx_clz((uint32_t)0);
    tdd_assert(ve==32);
    ve=rx_clz((uint32_t)1);
    tdd_assert(ve==31);

    ve=rx_ctz((uint64_t)0x7000000000000000);
    tdd_assert(ve==60);
    ve=rx_ctz(0x70000000);
    tdd_assert(ve==28);
    ve=rx_ctz((uint64_t)0);
    tdd_assert(ve==64);
    ve = rx_ctz((int64_t)0);
    tdd_assert(ve == 64);
    ve = rx_ctz((int32_t)0);
    tdd_assert(ve == 32);
    ve=rx_ctz(1);
    tdd_assert(ve==0);
    ve=rx_ctz(0);
    tdd_assert(ve==32);
    ve=rx_ctz(1);
    tdd_assert(ve==0);

    tdd_assert(!rx_is_pow2(0));
    tdd_assert( rx_is_pow2(1));
    tdd_assert( rx_is_pow2(2));
    tdd_assert(!rx_is_pow2(3));
    tdd_assert( rx_is_pow2(4));
    tdd_assert(!rx_is_pow2(5));
    tdd_assert(!rx_is_pow2(6));
    tdd_assert(!rx_is_pow2(7));
    tdd_assert( rx_is_pow2(8));

    tdd_assert(!rx_has_zero((uint32_t)0x11223344));
    tdd_assert( rx_has_zero((uint32_t)0x11003344));
    tdd_assert( rx_has_zero((uint32_t)0x11220044));

    tdd_assert(!rx_has_byte((uint32_t)0x11223344,0x23));
    tdd_assert( rx_has_byte((uint32_t)0x11003344,0x33));
    tdd_assert( rx_has_byte((uint32_t)0x11220044,0x00));

    tdd_assert(!rx_has_zero((uint64_t)0x1122334455667788));
    tdd_assert( rx_has_zero((uint64_t)0x1100334455667788));
    tdd_assert( rx_has_zero((uint64_t)0x1122004455667788));

    tdd_assert(!rx_has_byte((uint64_t)0x1122334455667788,0x23));
    tdd_assert( rx_has_byte((uint64_t)0x1100334455667788,0x33));
    tdd_assert( rx_has_byte((uint64_t)0x1122004455667788,0x00));

    ve=rx_ffs((uint64_t)0);
    tdd_assert(ve==0);
    ve=rx_ffs((uint64_t)0x8000000000000000);
    tdd_assert(ve==64);
    ve=rx_ffs((uint32_t)0);
    tdd_assert(ve==0);
    ve=rx_ffs((uint32_t)0x80000000);
    tdd_assert(ve==32);
    ve=rx_ffs((uint32_t)0x70000000);
    tdd_assert(ve==29);
    ve=rx_ffs((uint32_t)0x20000000);
    tdd_assert(ve==30);

    ve=rx_fls((uint32_t)0x80000000);
    tdd_assert(ve==32);
    ve=rx_fls((uint32_t)0x70000000);
    tdd_assert(ve==31);
    ve=rx_fls((uint32_t)0x20000000);
    tdd_assert(ve==30);
    ve=rx_fls((uint32_t)1);
    tdd_assert(ve==1);
    ve=rx_fls((uint32_t)0);
    tdd_assert(ve==0);

    ve=rx_fls((uint32_t)0x30000000);
    tdd_assert(ve==30);
    ve=rx_ffs((uint32_t)0x30000000);
    tdd_assert(ve==29);

    ve=rx_fls((uint64_t)0x3000000000000000);
    tdd_assert(ve==30+32);
    ve=rx_ffs((uint64_t)0x3000000000000000);
    tdd_assert(ve==29+32);

}

#endif
