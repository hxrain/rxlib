#ifndef _RX_UT_STR_UNI2GBK_H_
#define _RX_UT_STR_UNI2GBK_H_

#include "../rx_tdd.h"
#include "../rx_str_cs_utf8.h"
#include "../rx_str_cs_rawgbk.h"
#include "../rx_str_cs_uni2gbk.h"
#include "../rx_str_cs_gbk2uni.h"
#include "../rx_ct_util.h"
#include "../rx_assert.h"

namespace rx_ut
{
    //-----------------------------------------------------
    inline void ut_str_uni2gbk_raw_1(rx_tdd_t &rt)
    {
        uint16_t rc = 0;
        for (uint16_t i = 1; i < rx_uni_gbk_base_table_items; ++i)
        {//检验原始的uni2gbk码表,要求按照uni进行升序排列
            if (rx_uni_gbk_base_table[i].uni < rx_uni_gbk_base_table[i - 1].uni)
                ++rc;
        }
        rt.tdd_assert(rc==0);

        //单独校验原始查找算法
        rt.tdd_assert(rx_raw_uni2gbk(0x554a) == 0xb0a1);
        rt.tdd_assert(rx_char_uni2gbk(0x554a) == 0xb0a1);

        rt.tdd_assert(rx_raw_gbk2uni(0xb0a1) == 0x554a);

        //校验全部uni代码的查找
        rc = 0;
        for (uint16_t i = 1; i < rx_uni_gbk_base_table_items; ++i)
            if (rx_raw_uni2gbk(rx_uni_gbk_base_table[i].uni) != rx_uni_gbk_base_table[i].gbk)
                ++rc;
        rt.tdd_assert(rc == 0);

        rc = 0;
        for (uint16_t i = 1; i < rx_uni_gbk_base_table_items; ++i)
            if (rx_char_uni2gbk(rx_uni_gbk_base_table[i].uni) != rx_uni_gbk_base_table[i].gbk)
                ++rc;
        rt.tdd_assert(rc == 0);

        //校验全部gbk代码的查找
        rc = 0;
        for (uint16_t i = 1; i < rx_uni_gbk_base_table_items; ++i)
            if (rx_char_gbk2uni(rx_uni_gbk_base_table[i].gbk) != rx_uni_gbk_base_table[i].uni)
                ++rc;
        rt.tdd_assert(rc == 0);

        //进行gbk字符内存字节序的测试
        uint16_t gbk = str2code("啊");
        rt.tdd_assert(gbk == 0xb0a1);

        //string:gbk2uni
        wchar_t ubuff[4];
        rt.tdd_assert(rx_str_gbk2uni("a啊", ubuff, sizeof(ubuff)/sizeof(wchar_t)) == 2);
        //string:uni2gbk
        char gbuff[4];
        rt.tdd_assert(rx_str_uni2gbk(ubuff,gbuff, sizeof(gbuff)) == 3);
        rt.tdd_assert(strcmp(gbuff, "a啊") == 0);
        //string:chars
        rt.tdd_assert(rx_chars_gbk2uni(gbuff) == 2);
        rt.tdd_assert(rx_chars_uni2gbk(ubuff) == 3);

        //string:临界测试
        rt.tdd_assert(rx_str_uni2gbk(ubuff, gbuff, 3) == 3);
        rt.tdd_assert(rx_str_gbk2uni("a啊", ubuff, 3) == 2);
        //缓冲区不足,返回值与目标字符数相等了
        rt.tdd_assert(rx_str_gbk2uni("a啊", ubuff, 2) == 2);
    }

    //-----------------------------------------------------
    //生成uni2gbk的两级map表
    inline void ut_str_uni2gbk_raw_u2g(rx_tdd_t &rt)
    {
        const uint32_t highs = 256 * 4;
        const uint32_t lows = 256 / 4;
        const uint32_t shift = rx::LOG2<lows>::result;

        uint16_t map[highs*lows];
        memset(map, 0xff, sizeof(map));

        for (uint16_t i = 0; i < rx_uni_gbk_base_table_items; ++i)
            map[rx_uni_gbk_base_table[i].uni] = rx_uni_gbk_base_table[i].gbk;

        uint16_t list[highs] = { 0 };
        uint16_t lmap[lows];
        for (uint32_t h = 0; h < highs; ++h)
        {
            memset(lmap, 0xff, sizeof(lmap));
            uint16_t emp = 0;
            for (uint16_t l = 0; l < lows; ++l)
            {
                uint32_t idx = (h << shift) | l;
                lmap[l] = map[idx];
                if (lmap[l] == RX_BAD_UNI_CODE)
                    ++emp;
            }
            if (emp == lows)
                continue;

            list[h] = h;
            printf("\nstatic const uint16_t rx_uni2gbk_0x%03x[%d]={\n", h, lows);
            for (uint16_t i = 0; i < lows; ++i)
            {
                if (i&&i % 16 == 0) printf("\n");
                printf("0x%04x%s", lmap[i], i == lows - 1 ? "};" : ",");
            }
        }
        printf("\nstatic const uint16_t* rx_uni2gbk_tbl[%d]={", highs);
        for (uint32_t i = 0; i < highs; ++i)
        {
            if (i&&i % 8 == 0) printf("\n");
            if (list[i] != i)
                printf("NULL%s", i == highs - 1 ? "};" : ",");
            else
                printf("rx_uni2gbk_0x%03x%s", list[i], i == highs - 1 ? "};" : ",");
        }
        printf("\n");
    }
    //-----------------------------------------------------
    //生成gbk2uni的两级map表
    inline void ut_str_uni2gbk_raw_g2u(rx_tdd_t &rt)
    {
        const uint32_t highs = 256 * 4;
        const uint32_t lows = 256 / 4;
        const uint32_t shift = rx::LOG2<lows>::result;
        uint16_t map[highs*lows];
        memset(map, 0xff, sizeof(map));

        for (uint16_t i = 0; i < rx_uni_gbk_base_table_items; ++i)
            map[rx_uni_gbk_base_table[i].gbk] = rx_uni_gbk_base_table[i].uni;

        uint16_t list[highs] = { 0 };
        uint16_t lmap[lows];
        for (uint32_t h = 0; h < highs; ++h)
        {
            memset(lmap, 0xff, sizeof(lmap));
            uint16_t emp = 0;
            for (uint16_t l = 0; l < lows; ++l)
            {
                uint32_t idx = (h << shift) | l;
                lmap[l] = map[idx];
                if (lmap[l] == RX_BAD_UNI_CODE)
                    ++emp;
            }
            if (emp == lows)
                continue;

            list[h] = h;
            printf("\nstatic const uint16_t rx_gbk2uni_0x%03x[%d]={\n", h, lows);
            for (uint16_t i = 0; i < lows; ++i)
            {
                if (i&&i % 16 == 0) printf("\n");
                printf("0x%04x%s", lmap[i], i == lows-1 ? "};" : ",");
            }
        }
        printf("\nstatic const uint16_t* rx_gbk2uni_tbl[%d]={",highs);
        for (uint32_t i = 0; i < highs; ++i)
        {
            if (i&&i % 8 == 0) printf("\n");
            if (list[i] != i)
                printf("NULL%s", i == highs-1 ? "};" : ",");
            else
                printf("rx_gbk2uni_0x%03x%s", list[i], i == highs-1 ? "};" : ",");
        }
        printf("\n");
    }
    //-----------------------------------------------------
    uint32_t tbl_uni2gbk_size()
    {
        uint32_t rc = sizeof(rx_uni2gbk_tbl);
        uint16_t lc = rc / (uint16_t)sizeof(uint16_t*);
        for (uint32_t i = 0; i < lc; ++i)
            if (rx_uni2gbk_tbl[i])
                rc += sizeof(rx_uni2gbk_0x000);
        return rc;
    }
    //-----------------------------------------------------
    uint32_t tbl_gbk2uni_size()
    {
        uint32_t rc = sizeof(rx_gbk2uni_tbl);
        uint16_t lc = rc / (uint16_t)sizeof(uint16_t*);
        for (uint32_t i = 0; i < lc; ++i)
            if (rx_gbk2uni_tbl[i])
                rc += sizeof(rx_gbk2uni_0x000);
        return rc;
    }
}

rx_tdd(str_uni2gbk_raw)
{
#if RX_UT_MAKE_CS_GBK
    rx_show_msg("u2g=%d,g2u=%d,base=%d inc=%d\n", rx_ut::tbl_uni2gbk_size(), rx_ut::tbl_gbk2uni_size(), sizeof(rx_uni_gbk_base_table),
        (rx_ut::tbl_uni2gbk_size() + rx_ut::tbl_gbk2uni_size() - sizeof(rx_uni_gbk_base_table)));
    rx_ut::ut_str_uni2gbk_raw_g2u(*this);
    rx_ut::ut_str_uni2gbk_raw_u2g(*this);
#endif
    rx_ut::ut_str_uni2gbk_raw_1(*this);
}



#endif
