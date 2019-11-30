#ifndef _RX_UT_STR_UNI2GBK_H_
#define _RX_UT_STR_UNI2GBK_H_

#include "../rx_tdd.h"
#include "../rx_str_cs_utf8.h"
#include "../rx_str_cs_rawgbk.h"
#include "../rx_str_cs_uni2gbk.h"
#include "../rx_str_cs_gbk2uni.h"
#include "../rx_ct_util.h"
#include "../rx_assert.h"
#include "../rx_tdd_tick.h"
#include "../rx_str_util_std.h"

#if RX_CC==RX_CC_CLANG
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Winvalid-source-encoding"
#endif


namespace rx_ut
{
    //经典utf8编码算法
    inline uint8_t utf8_encode(uint32_t ch, char *s) {
        if (ch < 0x80) {
            s[0] = (char)ch;
            return 1;
        }
        if (ch <= 0x7FF) {
            s[1] = (char)((ch | 0x80) & 0xBF);
            s[0] = (char)((ch >> 6) | 0xC0);
            return 2;
        }
        if (ch <= 0xFFFF) {
        three:
            s[2] = (char)((ch | 0x80) & 0xBF);
            s[1] = (char)(((ch >> 6) | 0x80) & 0xBF);
            s[0] = (char)((ch >> 12) | 0xE0);
            return 3;
        }
        if (ch <= 0x1FFFFF) {
            s[3] = (char)((ch | 0x80) & 0xBF);
            s[2] = (char)(((ch >> 6) | 0x80) & 0xBF);
            s[1] = (char)(((ch >> 12) | 0x80) & 0xBF);
            s[0] = (char)((ch >> 18) | 0xF0);
            return 4;
        }
        if (ch <= 0x3FFFFFF) {
            s[4] = (char)((ch | 0x80) & 0xBF);
            s[3] = (char)(((ch >> 6) | 0x80) & 0xBF);
            s[2] = (char)(((ch >> 12) | 0x80) & 0xBF);
            s[1] = (char)(((ch >> 18) | 0x80) & 0xBF);
            s[0] = (char)((ch >> 24) | 0xF8);
            return 5;
        }
        if (ch <= 0x7FFFFFFF) {
            s[5] = (char)((ch | 0x80) & 0xBF);
            s[4] = (char)(((ch >> 6) | 0x80) & 0xBF);
            s[3] = (char)(((ch >> 12) | 0x80) & 0xBF);
            s[2] = (char)(((ch >> 18) | 0x80) & 0xBF);
            s[1] = (char)(((ch >> 24) | 0x80) & 0xBF);
            s[0] = (char)((ch >> 30) | 0xFC);
            return 6;
        }

        /* fallback */
        ch = 0xFFFD;
        goto three;
    }
    //经典utf8解码算法
    inline uint8_t utf8_decode(const char *s, const char *e, uint32_t &uc) {
        uint32_t ch;

        if (s >= e) {
            uc = 0;
            return 0;
        }

        ch = (unsigned char)s[0];
        if (ch < 0xC0) goto fallback;
        if (ch < 0xE0) {
            if (s + 1 >= e || (s[1] & 0xC0) != 0x80)
                goto fallback;
            uc = ((ch & 0x1F) << 6) |
                (s[1] & 0x3F);
            return 2;
        }
        if (ch < 0xF0) {
            if (s + 2 >= e || (s[1] & 0xC0) != 0x80
                || (s[2] & 0xC0) != 0x80)
                goto fallback;
            uc = ((ch & 0x0F) << 12) |
                ((s[1] & 0x3F) << 6) |
                (s[2] & 0x3F);
            return 3;
        }
        {
            int count = 0; /* to count number of continuation bytes */
            uc = 0;
            while ((ch & 0x40) != 0) { /* still have continuation bytes? */
                int cc = (unsigned char)s[++count];
                if ((cc & 0xC0) != 0x80) /* not a continuation byte? */
                    goto fallback; /* invalid byte sequence, fallback */
                uc = (uc << 6) | (cc & 0x3F); /* add lower 6 bits from cont. byte */
                ch <<= 1; /* to test next bit */
            }
            if (count > 5)
                goto fallback; /* invalid byte sequence */
            uc |= ((ch & 0x7F) << (count * 5)); /* add first byte */
            return count + 1;
        }

    fallback:
        uc = ch;
        return 1;
    }
    //-----------------------------------------------------
    //进行utf8字符编解码对比验证测试
    inline void ut_str_cs_utf8_base_1(rx_tdd_t &rt, uint32_t max_uni_code = 0x03FFFFFF)
    {
        uint8_t buff1[10] = "";
        uint8_t buff2[10] = "";
        uint32_t enc_bad_count = 0;
        uint32_t dec_bad_count = 0;
        for (uint32_t i = 0; i <= max_uni_code; ++i)
        {
            uint8_t r1 = rx_utf8_encode(i, buff1); buff1[r1] = 0;
            uint8_t r2 = utf8_encode(i, (char*)buff2); buff2[r2] = 0;
            if (r1 != r2 || r1 != rx_utf8_encode_size(i))
                ++enc_bad_count;
            else if (strcmp((char*)buff1, (char*)buff2))
                ++enc_bad_count;
            else
            {
                uint32_t u1, u2;
                uint8_t d1 = rx_utf8_decode(buff1, u1);
                uint8_t d2 = utf8_decode((char*)buff1, (char*)buff1 + r2, u2);
                if (d1 != d2 || d1 != rx_utf8_decode_size(buff1))
                    ++dec_bad_count;
                else if (u1 != u2)
                    ++dec_bad_count;
            }
        }
        rt.tdd_assert(enc_bad_count == 0);
        rt.tdd_assert(dec_bad_count == 0);
    }
    //-----------------------------------------------------
    //进行utf8字符串编解码对比性能测试
    inline void ut_str_cs_utf8_base_2(rx_tdd_t &rt, uint32_t max_uni_code = 0x13FFFFFF)
    {
        uint8_t u8str[10] = "";
        uint32_t ucode;
        uint8_t u8len;
        uint32_t bad = 0;
        tdd_tt(t, "ut_str_cs", "utf8_base_2");

        for (uint32_t i = 0; i <= max_uni_code; ++i)
        {
            u8len = utf8_encode(i, (char*)u8str); u8str[u8len] = 0;
            utf8_decode((char*)u8str, (char*)u8str + u8len, ucode);
            if (ucode != i)
                ++bad;
        }
        tdd_tt_hit(t, "classic utf8_encode/decode");

        for (uint32_t i = 0; i <= max_uni_code; ++i)
        {
            u8len = rx_utf8_encode(i, u8str); u8str[u8len] = 0;
            rx_utf8_decode(u8str, ucode);
            if (ucode != i)
                ++bad;
        }
        tdd_tt_hit(t, "rx utf8_encode/decode");

        rt.tdd_assert(bad == 0);
    }
    //-----------------------------------------------------
    //进行unicode/utf8字符串编解码测试
    inline void ut_str_cs_utf8_base_3(rx_tdd_t &rt)
    {
        const char *gstr= "今天天气不错~挺风和日丽的!我们下午没有课~想也挺好的?";
        wchar_t ustr[128];
        uint32_t gsc = rx_str_gbk2uni(gstr, ustr, sizeof(ustr));
        rt.tdd_assert(24 + 4 == gsc);

        uint8_t utf8[256];
        uint32_t u8sc = rx_utf8_encode(ustr, utf8);
        rt.tdd_assert(24 * 3 + 4 * 1 == u8sc);

        wchar_t ubuf[128];
        uint32_t usc = rx_utf8_decode(utf8, ubuf);

        rt.tdd_assert(gsc == usc);
        rt.tdd_assert(rx::st::strcmp(ustr,ubuf)==0);
        rt.tdd_assert(rx::st::strlen(ustr) == usc);

    }
    //-----------------------------------------------------
    inline void ut_str_uni2gbk_raw_1(rx_tdd_t &rt)
    {
        uint16_t rc = 0;
        for (uint16_t i = 1; i < rx_uni_gbk_base_table_items; ++i)
        {//检验原始的uni2gbk码表,要求按照uni进行升序排列
            if (rx_uni_gbk_base_table[i].uni < rx_uni_gbk_base_table[i - 1].uni)
                ++rc;
        }
        rt.tdd_assert(rc == 0);

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
        rt.tdd_assert(rx_str_gbk2uni("a啊", ubuff, sizeof(ubuff) / sizeof(wchar_t)) == 2);
        //string:uni2gbk
        char gbuff[4];
        rt.tdd_assert(rx_str_uni2gbk(ubuff, gbuff, sizeof(gbuff)) == 3);
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
    //生成uni2gbk的两级hashmap表
    inline void ut_str_uni2gbk_raw_u2g(rx_tdd_t &rt)
    {
        const uint32_t highs = 256 * 4;
        const uint32_t lows = 256 / 4;
        const uint32_t shift = rx::LOG2<lows>::result;

        uint16_t map[highs*lows];
        for (uint32_t i = 0; i < highs*lows; ++i) map[i] = RX_CS_BAD_CHAR;

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
                if (lmap[l] == RX_CS_BAD_CHAR)
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
    //生成gbk2uni的两级hashmap表
    inline void ut_str_uni2gbk_raw_g2u(rx_tdd_t &rt)
    {
        const uint32_t highs = 256 * 4;
        const uint32_t lows = 256 / 4;
        const uint32_t shift = rx::LOG2<lows>::result;
        uint16_t map[highs*lows];
        for (uint32_t i = 0; i < highs*lows; ++i) map[i] = RX_CS_BAD_CHAR;

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
                if (lmap[l] == RX_CS_BAD_CHAR)
                    ++emp;
            }
            if (emp == lows)
                continue;

            list[h] = h;
            printf("\nstatic const uint16_t rx_gbk2uni_0x%03x[%d]={\n", h, lows);
            for (uint16_t i = 0; i < lows; ++i)
            {
                if (i&&i % 16 == 0) printf("\n");
                printf("0x%04x%s", lmap[i], i == lows - 1 ? "};" : ",");
            }
        }
        printf("\nstatic const uint16_t* rx_gbk2uni_tbl[%d]={", highs);
        for (uint32_t i = 0; i < highs; ++i)
        {
            if (i&&i % 8 == 0) printf("\n");
            if (list[i] != i)
                printf("NULL%s", i == highs - 1 ? "};" : ",");
            else
                printf("rx_gbk2uni_0x%03x%s", list[i], i == highs - 1 ? "};" : ",");
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

#define RX_UT_MAKE_CS_GBK 0

rx_tdd(str_uni2gbk_raw)
{
#if RX_UT_MAKE_CS_GBK
    rx_show_msg("u2g=%d,g2u=%d,base=%d inc=%d\n", rx_ut::tbl_uni2gbk_size(), rx_ut::tbl_gbk2uni_size(), sizeof(rx_uni_gbk_base_table),
        (rx_ut::tbl_uni2gbk_size() + rx_ut::tbl_gbk2uni_size() - sizeof(rx_uni_gbk_base_table)));
    rx_ut::ut_str_uni2gbk_raw_g2u(*this);
    rx_ut::ut_str_uni2gbk_raw_u2g(*this);
#endif

    rx_ut::ut_str_cs_utf8_base_3(*this);
    rx_ut::ut_str_cs_utf8_base_1(*this);
    rx_ut::ut_str_cs_utf8_base_2(*this);
    rx_ut::ut_str_uni2gbk_raw_1(*this);
}

#if RX_CC==RX_CC_CLANG
    #pragma GCC diagnostic pop
#endif


#endif
