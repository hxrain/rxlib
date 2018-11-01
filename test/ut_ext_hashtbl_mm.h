#ifndef _RX_UT_EXT_HASHTBL_MM_H_
#define _RX_UT_EXT_HASHTBL_MM_H_

#include "../rx_tdd.h"
#include "../rx_tdd_tick.h"
#include "../rx_ext_hashtbl_mm.h"
#include "../rx_hash_int.h"
#include "../rx_ct_util.h"

namespace rx_ut
{
    const bool ut_show_mmhashtbl_debug = false;
    //-----------------------------------------------------
    class ut_mmhtcmp_t0
    {
    public:
        //对节点比较函数进行覆盖,需要真正使用节点中的key字段与给定的k值进行比较
        template<class NVT, class KT>
        static bool equ(const NVT &n, const KT &k) { return n.key == k; }
    };
    class ut_mmhtcmp_t1 :public ut_mmhtcmp_t0
    {
    public:
        static uint32_t hash(const uint32_t &k) { return (uint32_t)rx_hash_gold<0>(k, 514229); }
    };
    class ut_mmhtcmp_t2 :public ut_mmhtcmp_t0
    {
    public:
        static uint32_t hash(const uint32_t &k) { return rx_hash_tomas(k); }
    };
    class ut_mmhtcmp_t3 :public ut_mmhtcmp_t0
    {
    public:
        static uint32_t hash(const uint32_t &k) { return rx_hash_murmur3(k); }
    };
    class ut_mmhtcmp_t4 :public ut_mmhtcmp_t0
    {
    public:
        static uint32_t hash(const uint32_t &k) { return rx_hash_mosquito(k); }
    };

    //-----------------------------------------------------
    const uint32_t max_node_count = 200003;
    typedef rx::tiny_mm_hashtbl_t<max_node_count, uint32_t, uint32_t, ut_mmhtcmp_t1> mm_hashtbl_int;

    //-----------------------------------------------------
    inline uint32_t ut_mm_hashtbl_base_make(mm_hashtbl_int &tbl,uint32_t total)
    {
        tdd_print(ut_show_mmhashtbl_debug,"mm_hashtbl test1\r\n");
        tdd_tt(t, "mm_hashtbl_int", "make");
        uint32_t ec = 0;
        for (uint32_t i = 0; i < total; ++i)
        {
            if (tbl.insert(80021000 + i, i) == tbl.end())
                ++ec;
        }
        tdd_tt_hit(t, "capacity=%d items=%d collision=%d", tbl.capacity(), total, tbl.stat().collision_count);
        return ec;
    }

    //-----------------------------------------------------
    inline uint32_t ut_mm_hashtbl_base_check(mm_hashtbl_int &tbl)
    {
        tdd_tt(t, "mm_hashtbl_int", "check");
        uint16_t max_step = 0;
        uint32_t ec = 0;
        for (uint32_t i = 0; i < tbl.size(); ++i)
        {
            mm_hashtbl_int::iterator I = tbl.find(80021000 + i);

            if (I == tbl.end() || *I != i)
                ++ec;
            else
                max_step = rx::Max(tbl.at(I.pos())->step, max_step);
        }
        tdd_tt_hit(t, "capacity=%d items=%d collision=%d max_step=%d", tbl.capacity(), tbl.size(), tbl.stat().collision_count, max_step);
        return ec;
    }
    //-----------------------------------------------------
    inline void ut_mm_hashtbl_base(rx_tdd_t &rt)
    {
        const uint32_t total = 10000 * 16;
        mm_hashtbl_int mm_htbl;

        rx::error_code ec = mm_htbl.open("ut_tbl_int_base.dat");
        rt.tdd_assert(ec==rx::ec_ok);

        if (ec)
        {
            tdd_print(ut_show_mmhashtbl_debug, "mm_hashtbl open error = %d\r\n", ec.value);
            return;
        }

        uint32_t ic;
        if (mm_htbl.size() == 0)
        {
            ic = ut_mm_hashtbl_base_make(mm_htbl,total);
            rt.tdd_assert(ic==0);
        }

        rt.tdd_assert(mm_htbl.size() == total);

        ic = ut_mm_hashtbl_base_check(mm_htbl);
        rt.tdd_assert(ic == 0);
        rt.tdd_assert(mm_htbl.size() == total);

        ut_mm_hashtbl_base_check(mm_htbl);

        ut_mm_hashtbl_base_check(mm_htbl);
    }

}

rx_tdd(bits_op_base)
{
    rx_ut::ut_mm_hashtbl_base(*this);
}

#endif
