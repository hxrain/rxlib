#ifndef _UT_BITS_OP_H_
#define _UT_BITS_OP_H_

#include "../rx_bits_op.h"
#include "../rx_tdd.h"

//---------------------------------------------------------
inline void ut_bits_op_base_1(rx_tdd_t &rt)
{
    uint32_t v;
    v = rx_byte_flip<uint32_t, 1>();
    rt.tdd_assert(v == 0x01010101);
    v = rx_byte_flip<uint32_t, 2>();
    rt.tdd_assert(v == 0x02020202);
    v = rx_byte_flip<uint32_t, 0x23>();
    rt.tdd_assert(v == 0x23232323);

    rt.tdd_assert(rx_popcnt((uint32_t)0) == 0);
    rt.tdd_assert(rx_popcnt((uint32_t)1) == 1);
    rt.tdd_assert(rx_popcnt((uint32_t)3) == 2);
    rt.tdd_assert(rx_popcnt((uint32_t)0x10101010) == 4);

    rt.tdd_assert(rx_popcnt((uint64_t)0) == 0);
    rt.tdd_assert(rx_popcnt((uint64_t)1) == 1);
    rt.tdd_assert(rx_popcnt((uint64_t)3) == 2);
    rt.tdd_assert(rx_popcnt((uint64_t)0x10101010) == 4);

    uint64_t ve;
    ve = rx_clz((uint64_t)0x7000000000000000);
    rt.tdd_assert(ve == 1);
    ve = rx_clz((uint32_t)0x70000000);
    rt.tdd_assert(ve == 1);
    ve = rx_clz((uint64_t)0);
    rt.tdd_assert(ve == 64);
    ve = rx_clz((uint64_t)1);
    rt.tdd_assert(ve == 63);
    ve = rx_clz((uint32_t)0);
    rt.tdd_assert(ve == 32);
    ve = rx_clz((uint32_t)1);
    rt.tdd_assert(ve == 31);

    ve = rx_ctz((uint64_t)0x7000000000000000);
    rt.tdd_assert(ve == 60);
    ve = rx_ctz((uint32_t)0x70000000);
    rt.tdd_assert(ve == 28);
    ve = rx_ctz((uint64_t)0);
    rt.tdd_assert(ve == 64);
    ve = rx_ctz((uint64_t)0);
    rt.tdd_assert(ve == 64);
    ve = rx_ctz((uint32_t)0);
    rt.tdd_assert(ve == 32);
    ve = rx_ctz((uint32_t)1);
    rt.tdd_assert(ve == 0);
    ve = rx_ctz((uint32_t)0);
    rt.tdd_assert(ve == 32);
    ve = rx_ctz((uint32_t)1);
    rt.tdd_assert(ve == 0);

    rt.tdd_assert(!rx_is_pow2(0));
    rt.tdd_assert(rx_is_pow2(1));
    rt.tdd_assert(rx_is_pow2(2));
    rt.tdd_assert(!rx_is_pow2(3));
    rt.tdd_assert(rx_is_pow2(4));
    rt.tdd_assert(!rx_is_pow2(5));
    rt.tdd_assert(!rx_is_pow2(6));
    rt.tdd_assert(!rx_is_pow2(7));
    rt.tdd_assert(rx_is_pow2(8));

    rt.tdd_assert(!rx_has_zero((uint32_t)0x11223344));
    rt.tdd_assert(rx_has_zero((uint32_t)0x11003344));
    rt.tdd_assert(rx_has_zero((uint32_t)0x11220044));

    rt.tdd_assert(!rx_has_byte((uint32_t)0x11223344, 0x23));
    rt.tdd_assert(rx_has_byte((uint32_t)0x11003344, 0x33));
    rt.tdd_assert(rx_has_byte((uint32_t)0x11220044, 0x00));

    rt.tdd_assert(!rx_has_zero((uint64_t)0x1122334455667788));
    rt.tdd_assert(rx_has_zero((uint64_t)0x1100334455667788));
    rt.tdd_assert(rx_has_zero((uint64_t)0x1122004455667788));

    rt.tdd_assert(!rx_has_byte((uint64_t)0x1122334455667788, 0x23));
    rt.tdd_assert(rx_has_byte((uint64_t)0x1100334455667788, 0x33));
    rt.tdd_assert(rx_has_byte((uint64_t)0x1122004455667788, 0x00));

    ve = rx_ffs((uint64_t)0);
    rt.tdd_assert(ve == 0);
    ve = rx_ffs((uint64_t)0x8000000000000000);
    rt.tdd_assert(ve == 64);
    ve = rx_ffs((uint32_t)0);
    rt.tdd_assert(ve == 0);
    ve = rx_ffs((uint32_t)0x80000000);
    rt.tdd_assert(ve == 32);
    ve = rx_ffs((uint32_t)0x70000000);
    rt.tdd_assert(ve == 29);
    ve = rx_ffs((uint32_t)0x20000000);
    rt.tdd_assert(ve == 30);

    ve = rx_fls((uint32_t)0x80000000);
    rt.tdd_assert(ve == 32);
    ve = rx_fls((uint32_t)0x70000000);
    rt.tdd_assert(ve == 31);
    ve = rx_fls((uint32_t)0x20000000);
    rt.tdd_assert(ve == 30);
    ve = rx_fls((uint32_t)1);
    rt.tdd_assert(ve == 1);
    ve = rx_fls((uint32_t)0);
    rt.tdd_assert(ve == 0);

    ve = rx_fls((uint32_t)0x30000000);
    rt.tdd_assert(ve == 30);
    ve = rx_ffs((uint32_t)0x30000000);
    rt.tdd_assert(ve == 29);

    ve = rx_fls((uint64_t)0x3000000000000000);
    rt.tdd_assert(ve == 30 + 32);
    ve = rx_ffs((uint64_t)0x3000000000000000);
    rt.tdd_assert(ve == 29 + 32);
}

//---------------------------------------------------------
inline void ut_bits_op_base_2(rx_tdd_t &rt)
{
    uint32_t v = 0x80010422;//1000 0000 0000 0001 0000 0100 0010 0010b
    uint32_t r;

    rt.tdd_assert(bits_tst(v, 0x20)!=0);
    rt.tdd_assert(bits_set(v, 0x81) == 0x800104A3);
    rt.tdd_assert(bits_clr(v, 0x81) == 0x80010422);

    rt.tdd_assert(bit_tst(v, 1) != 0);
    rt.tdd_assert(bit_set(v, 2) == 0x80010426);
    rt.tdd_assert(bit_clr(v, 2) == 0x80010422);

    rt.tdd_assert(bits_get1(v, 1) == 1);
    rt.tdd_assert(bits_get1(v, 6) == 0);
    rt.tdd_assert(rx_bits_gets(v, 1, 1, r) && r == 1);
    rt.tdd_assert(rx_bits_gets(v, 1, 2, r) && r == 2);
    rt.tdd_assert(rx_bits_gets(v, 6, 1, r) && r == 0);
    rt.tdd_assert(rx_bits_gets(v, 6, 6, r) && r == 0x11);


    rt.tdd_assert(bits_get2(v, 1) == 2);
    rt.tdd_assert(bits_get2(v, 6) == 1);

    rt.tdd_assert(bits_get3(v, 2) == 2);
    rt.tdd_assert(bits_get3(v, 6) == 2);

    rt.tdd_assert(bits_get4(v, 3) == 2);
    rt.tdd_assert(bits_get4(v, 6) == 4);

    rt.tdd_assert(bits_get8(v, 7) == 0x22);
    rt.tdd_assert(bits_get8(v, 9) == 8);

    rt.tdd_assert(bits_put2(v, 1, 1) == 0x80010421);

    v = 0x80010422;
    rt.tdd_assert(bits_put3(v, 6, 4) == 0x80010442);

    v = 0x80010422;
    rt.tdd_assert(bits_put4(v, 6, 3) == 0x8001041A);

    v = 0x80010422;
    rt.tdd_assert(bits_put8(v, 7, 1) == 0x80010401);

    v = 0x80010422;
    rt.tdd_assert(rx_bits_puts(v, 1, 1, 0, r) && r == 0x80010420);
    rt.tdd_assert(rx_bits_puts(v, 1, 2, 0, r) && r == 0x80010420);
    rt.tdd_assert(rx_bits_puts(v, 6, 3, 1, r) && r == 0x80010412);
    rt.tdd_assert(rx_bits_puts(v, 6, 6, 2, r) && r == 0x80010404);

    uint8_t *bs = (uint8_t*)&v;
    rt.tdd_assert(rx_bits_set(2, bs, sizeof(v)) && v == 0x80010426);
    rt.tdd_assert(rx_bits_set(9, bs, sizeof(v)) && v == 0x80010626);

    rt.tdd_assert(rx_bits_tst(9, bs, sizeof(v)));
    rt.tdd_assert(rx_bits_set(2, bs, sizeof(v)));

    rt.tdd_assert(rx_bits_clr(9, bs, sizeof(v)) && v == 0x80010426);
    rt.tdd_assert(rx_bits_clr(2, bs, sizeof(v)) && v == 0x80010422);

    //ÉıĞò
    v = 0x80010422;
    rx_bits_array_t ba;
    rt.tdd_assert(ba.begin(bs, sizeof(v)));
    rt.tdd_assert(ba.bits() == sizeof(v) << 3);
    rt.tdd_assert(ba.pos() == 0);
    rt.tdd_assert(ba.remain() == ba.bits()-1);
    rt.tdd_assert(!ba.tst());
    ba.set();
    rt.tdd_assert(ba.tst());
    ba.clr();
    rt.tdd_assert(!ba.tst());
    rt.tdd_assert(ba.next());
    rt.tdd_assert(ba.tst());
    ba.clr();
    rt.tdd_assert(!ba.tst());
    ba.set();
    rt.tdd_assert(ba.tst());
    rt.tdd_assert(ba.pos() == 1);
    rt.tdd_assert(v == 0x80010422);
    rt.tdd_assert(ba.remain() == ba.bits()-2);
    rt.tdd_assert(ba.pos(30));
    rt.tdd_assert(ba.remain() == 1);
    rt.tdd_assert(!ba.tst());
    ba.set();
    rt.tdd_assert(v == 0xC0010422);
    ba.clr();
    rt.tdd_assert(v == 0x80010422);
    rt.tdd_assert(ba.next());
    rt.tdd_assert(ba.remain() == 0);
    rt.tdd_assert(ba.pos() == 31);
    rt.tdd_assert(!ba.next());
    rt.tdd_assert(ba.pos(3));
    rt.tdd_assert(ba.fetch(r, 3) && r == 0x4);
    rt.tdd_assert(ba.pos(3));
    rt.tdd_assert(ba.fill(0x01, 3) && v == 0x8001040A);

    //½µĞò
    v = 0x80010422;
    rt.tdd_assert(ba.begin(bs, sizeof(v),false));
    rt.tdd_assert(ba.bits() == sizeof(v) << 3);
    rt.tdd_assert(ba.pos() == 31);
    rt.tdd_assert(ba.remain() == ba.bits() - 1);
    rt.tdd_assert(ba.tst());
    ba.clr();
    rt.tdd_assert(!ba.tst());
    ba.set();
    rt.tdd_assert(ba.tst());
    rt.tdd_assert(ba.next());
    rt.tdd_assert(!ba.tst());
    ba.set();
    rt.tdd_assert(ba.tst());
    ba.clr();
    rt.tdd_assert(!ba.tst());
    rt.tdd_assert(ba.pos() == 30);
    rt.tdd_assert(v == 0x80010422);
    rt.tdd_assert(ba.remain() == ba.bits() - 2);
    rt.tdd_assert(ba.pos(1));
    rt.tdd_assert(ba.remain() == 1);
    rt.tdd_assert(ba.tst());
    ba.clr();
    rt.tdd_assert(v == 0x80010420);
    ba.set();
    rt.tdd_assert(v == 0x80010422);
    rt.tdd_assert(ba.next());
    rt.tdd_assert(ba.remain() == 0);
    rt.tdd_assert(ba.pos() == 0);
    rt.tdd_assert(!ba.next());
    rt.tdd_assert(ba.pos(6));
    rt.tdd_assert(ba.fetch(r, 3) && r == 0x40000000);
    rt.tdd_assert(ba.pos(6));
    rt.tdd_assert(ba.fill((uint8_t)0x20, 3) && v == 0x80010412);
}

//---------------------------------------------------------
rx_tdd(bits_op_base)
{
    ut_bits_op_base_1(*this);
    ut_bits_op_base_2(*this);

}

#endif
