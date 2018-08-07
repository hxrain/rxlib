#ifndef _RX_UT_HASH_TLMAP_H_
#define _RX_UT_HASH_TLMAP_H_

#include "../rx_tdd.h"
#include "../rx_hash_tlmap.h"
namespace tn_tmp
{
    typedef struct tlmap_cfg_t
    {
        enum {
            MIN_ALIGN = 32,                             //��С����ߴ�
            FLI_MAX = 31,                               //һ�������������
            SLI_MAX = 8,                                //���������������

            SLI_SHIFT = rx::log2<SLI_MAX>::result,      //���������ı�������
            FLI_OFFSET = rx::log2<MIN_ALIGN>::result,   //һ��������ƫ����
        };
    }tlmap_cfg_t;
}

inline void rx_hash_tlmap_base_1(rx_tdd_base &rt)
{
    uint32_t fl, sl;
    rx_hash_tlmap<tn_tmp::tlmap_cfg_t>(0, fl, sl);
    rt.tdd_assert(fl == 5 && sl == 0);
    rx_hash_tlmap<tn_tmp::tlmap_cfg_t>(31, fl, sl);
    rt.tdd_assert(fl == 5 && sl == 0);
    rx_hash_tlmap<tn_tmp::tlmap_cfg_t>(32, fl, sl);
    rt.tdd_assert(fl == 5 && sl == 0);
    rx_hash_tlmap<tn_tmp::tlmap_cfg_t>(33, fl, sl);
    rt.tdd_assert(fl == 5 && sl == 0);
    rx_hash_tlmap<tn_tmp::tlmap_cfg_t>(36, fl, sl);
    rt.tdd_assert(fl == 5 && sl == 1);
    rx_hash_tlmap<tn_tmp::tlmap_cfg_t>(40, fl, sl);
    rt.tdd_assert(fl == 5 && sl == 2);
    rx_hash_tlmap<tn_tmp::tlmap_cfg_t>(47, fl, sl);
    rt.tdd_assert(fl == 5 && sl == 3);
    rx_hash_tlmap<tn_tmp::tlmap_cfg_t>(82, fl, sl);
    rt.tdd_assert(fl == 6 && sl == 2);

}

rx_tdd(rx_hash_tlmap_base)
{
    rx_hash_tlmap_base_1(*this);
}



#endif
