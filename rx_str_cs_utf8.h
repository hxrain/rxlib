#ifndef _RX_STR_CS_UTF8_H_
#define _RX_STR_CS_UTF8_H_

#include "rx_ct_bitop.h"
/*
        UNICODE          =>                 UTF8
   0x00000000-0x0000007F | 0xxxxxxx
   0x00000080-0x000007FF | 110xxxxx 10xxxxxx
   0x00000800-0x0000FFFF | 1110xxxx 10xxxxxx 10xxxxxx
   0x00010000-0x001FFFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
   0x00200000-0x03FFFFFF | 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
   0x04000000-0x7FFFFFFF | 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
*/

    //-----------------------------------------------------
    //进行unicode字符ch的utf8编码,结果放入s中
    //返回值:ch对应的utf8串长度
    inline uint8_t rx_utf8_char_encode(uint32_t ch, uint8_t *s)
    {
        //获取前导比特0的数量
        uint8_t zbit_count = rx_clz(ch);
        switch (zbit_count)
        {
        case 32:case 31:case 30:case 29:case 28:case 27:case 26:case 25:
            s[0] = (uint8_t)ch;
            return 1;
        case 24:case 23:case 22:case 21:
            s[1] = (uint8_t)((ch | 0x80) & 0xBF);
            s[0] = (uint8_t)((ch >> 6) | 0xC0);
            return 2;
        case 20:case 19:case 18:case 17:case 16:
            s[2] = (uint8_t)((ch | 0x80) & 0xBF);
            s[1] = (uint8_t)(((ch >> 6) | 0x80) & 0xBF);
            s[0] = (uint8_t)((ch >> 12) | 0xE0);
            return 3;
        case 15:case 14:case 13:case 12:case 11:
            s[3] = (uint8_t)((ch | 0x80) & 0xBF);
            s[2] = (uint8_t)(((ch >> 6) | 0x80) & 0xBF);
            s[1] = (uint8_t)(((ch >> 12) | 0x80) & 0xBF);
            s[0] = (uint8_t)((ch >> 18) | 0xF0);
            return 4;
        case 10:case 9:case 8:case 7:case 6:
            s[4] = (uint8_t)((ch | 0x80) & 0xBF);
            s[3] = (uint8_t)(((ch >> 6) | 0x80) & 0xBF);
            s[2] = (uint8_t)(((ch >> 12) | 0x80) & 0xBF);
            s[1] = (uint8_t)(((ch >> 18) | 0x80) & 0xBF);
            s[0] = (uint8_t)((ch >> 24) | 0xF8);
            return 5;
        case 5:case 4:case 3:case 2:case 1:
            s[5] = (uint8_t)((ch | 0x80) & 0xBF);
            s[4] = (uint8_t)(((ch >> 6) | 0x80) & 0xBF);
            s[3] = (uint8_t)(((ch >> 12) | 0x80) & 0xBF);
            s[2] = (uint8_t)(((ch >> 18) | 0x80) & 0xBF);
            s[1] = (uint8_t)(((ch >> 24) | 0x80) & 0xBF);
            s[0] = (uint8_t)((ch >> 30) | 0xFC);
            return 6;
        default://0xFFFD
            s[2] = 0xbd;
            s[1] = 0xbf;
            s[0] = 0xef;
            return 3;
        }
    }
    //-----------------------------------------------------
    //计算unicode字符ch占用的utf8串长度
    //返回值:0错误;其他为串长度
    inline uint8_t rx_utf8_chars(uint32_t ch)
    {
        //获取前导比特0的数量
        uint8_t zbit_count = rx_clz(ch);
        static uint8_t chars_size[] = {0,6,6,6,6,6,5,5,5,5,5,4,4,4,4,4,3,3,3,3,3,2,2,2,2,1,1,1,1,1,1,1,1};
        rx_assert(zbit_count<=32);
        return chars_size[zbit_count];
    }
    //-----------------------------------------------------
    //对utf8字符串s解码,内容放入uc;(错误处理简化,仅依赖前导1的比特数)
    //返回值:消耗的字节数
    inline uint8_t rx_utf8_char_decode(const uint8_t *s, uint32_t &uc)
    {
        rx_assert(s!=NULL);

        //获取首字节前导bit置位数
        uc = s[0];
        uint8_t sbit_count = rx_clz((~uc)&0xFF)-24;

        switch (sbit_count)
        {
        case 2:
            uc = ((s[0] & 0x1F) << 6) | (s[1] & 0x3F);
            return 2;
        case 3:
            uc = ((s[0] & 0x0F) << 12) | (s[1] & 0x3F) << 6 | (s[2] & 0x3F);
            return 3;
        case 4:
            uc = ((s[0] & 0x07) << 18) | (s[1] & 0x3F) << 12 | (s[2] & 0x3F) << 6 | (s[3] & 0x3F);
            return 4;
        case 5:
            uc = ((s[0] & 0x03) << 24) | (s[1] & 0x3F) << 18 | (s[2] & 0x3F) << 12 | (s[3] & 0x3F) << 6 | (s[4] & 0x3F);
            return 5;
        case 6:
            uc = ((s[0] & 0x03) << 30) | (s[1] & 0x3F) << 24 | (s[2] & 0x3F) << 18 | (s[3] & 0x3F) << 12 | (s[4] & 0x3F) << 6 | (s[5] & 0x3F);
            return 6;
        default:
            return 1;
        }
    }

#endif
