#ifndef _RX_STR_CS_UTF8_H_
#define _RX_STR_CS_UTF8_H_

#include "rx_ct_bitop.h"

    //进行unicode字符ch的utf8编码,结果放入s中
    //返回值:ch对应的utf8串长度
    inline size_t rx_utf8_char_encode(uint32_t ch,char *s)
    {
        if (ch <= 0x7F) {
            s[0] = (uint8_t) ch;
            return 1;
        }
        if (ch <= 0x7FF) {
            s[1] = (uint8_t) ((ch | 0x80) & 0xBF);
            s[0] = (uint8_t) ((ch >> 6) | 0xC0);
            return 2;
        }
        if (ch <= 0xFFFF) {
        three:
            s[2] = (uint8_t) ((ch | 0x80) & 0xBF);
            s[1] = (uint8_t) (((ch >> 6) | 0x80) & 0xBF);
            s[0] = (uint8_t) ((ch >> 12) | 0xE0);
            return 3;
        }
        if (ch <= 0x1FFFFF) {
            s[3] = (uint8_t) ((ch | 0x80) & 0xBF);
            s[2] = (uint8_t) (((ch >> 6) | 0x80) & 0xBF);
            s[1] = (uint8_t) (((ch >> 12) | 0x80) & 0xBF);
            s[0] = (uint8_t) ((ch >> 18) | 0xF0);
            return 4;
        }
        if (ch <= 0x3FFFFFF) {
            s[4] = (uint8_t) ((ch | 0x80) & 0xBF);
            s[3] = (uint8_t) (((ch >> 6) | 0x80) & 0xBF);
            s[2] = (uint8_t) (((ch >> 12) | 0x80) & 0xBF);
            s[1] = (uint8_t) (((ch >> 18) | 0x80) & 0xBF);
            s[0] = (uint8_t) ((ch >> 24) | 0xF8);
            return 5;
        }
        if (ch <= 0x7FFFFFFF) {
            s[5] = (uint8_t) ((ch | 0x80) & 0xBF);
            s[4] = (uint8_t) (((ch >> 6) | 0x80) & 0xBF);
            s[3] = (uint8_t) (((ch >> 12) | 0x80) & 0xBF);
            s[2] = (uint8_t) (((ch >> 18) | 0x80) & 0xBF);
            s[1] = (uint8_t) (((ch >> 24) | 0x80) & 0xBF);
            s[0] = (uint8_t) ((ch >> 30) | 0xFC);
            return 6;
        }
        
        /* fallback */
        ch = 0xFFFD;
        goto three;
    }

#endif