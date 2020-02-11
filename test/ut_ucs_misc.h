#ifndef _UT_UCS2_MISC_H_
#define _UT_UCS2_MISC_H_

#include "../rx_cc_macro.h"
#include "../rx_tdd.h"
#include "../rx_ucs2_misc.h"

namespace rx{

    inline void test_ucs2_misc_base_1(rx_tdd_t &rt)
    {
        uint32_t idx=query_ucs_block(1);
        const ucs2_block_item_t &it=ucs2_blocks_range[idx];
        rt.tdd_assert(idx!=ucs2_blocks_range_size);
        rt.tdd_assert(it.code_begin==0);

        //�����ַ�����unicode��
        for(uint32_t c=0;c<0xFFFF;++c)
        {
            uint32_t idx=query_ucs_block(c);
            const ucs2_block_item_t &it=ucs2_blocks_range[idx];
            rt.tdd_assert(idx!=ucs2_blocks_range_size);
            rt.tdd_assert(it.code_begin<=c);
            const ucs2_block_item_t &itn=ucs2_blocks_range[idx+1];
            rt.tdd_assert(itn.code_begin>c);
        }

        //���Կ��Ե����
        for(uint32_t i=0;i<ucs2_blocks_range_size;++i)
        {
            uint32_t c=ucs2_blocks_range[i].code_begin;
            uint32_t idx=query_ucs_block_edge(c);
            const ucs2_block_item_t &it=ucs2_blocks_range[idx];
            rt.tdd_assert(idx!=ucs2_blocks_range_size);
            rt.tdd_assert(it.code_begin==c);
            const ucs2_block_item_t &itn=ucs2_blocks_range[idx+1];
            rt.tdd_assert(itn.code_begin>c);
        }
    }

}
rx_tdd(ucs2_misc_base)
{
    rx::test_ucs2_misc_base_1(*this);
}


#endif
