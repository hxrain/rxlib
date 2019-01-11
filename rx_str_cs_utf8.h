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

/*
    //-----------------------------------------------------
    //单个unicode字符ch的utf8编码,结果放入s
    //返回值:ch对应的utf8串长度
    inline uint8_t rx_utf8_encode(uint32_t ch, uint8_t *s);

    //-----------------------------------------------------
    //unicode字符串str编码为utf8串(外部需确保缓冲区足够长,必要时可调用rx_utf8_encode_size获取)
    //返回值:0错误;其他为utf8串长度
    inline uint32_t rx_utf8_encode(const wchar_t *str,uint8_t *utf8);

    //-----------------------------------------------------
    //计算unicode字符ch编码后占用的utf8串长度
    //返回值:0错误;其他为串长度
    inline uint8_t rx_utf8_encode_size(uint32_t ch);

    //-----------------------------------------------------
    //计算unicode字符串str编码后的utf8串长度
    //返回值:0错误;其他为串长度
    inline uint32_t rx_utf8_encode_size(const wchar_t *str);

    //-----------------------------------------------------
    //对utf8字符串s解码,得到单独的一个unicode字符uc;(错误处理简化,仅依赖前导1的比特数)
    //返回值:消耗的字节数(1~6)
    inline uint8_t rx_utf8_decode(const uint8_t *s, uint32_t &uc);

    //-----------------------------------------------------
    //对utf8字符串s解码得到unicode字符串ustr(外部需确保缓冲器足够长,必要时可使用rx_utf8_decode_size计算所需长度)
    //返回值:0出错;其他为unicode串的字符数量
    inline uint32_t rx_utf8_decode(const uint8_t *s,wchar_t* ustr);

    //-----------------------------------------------------
    //计算utf8字符串s解码后消耗的字节数量
    //返回值:消耗的字节数;0错误(s不是一个正确的utf8开始字节).
    inline uint8_t rx_utf8_decode_size(const uint8_t *s);

    //-----------------------------------------------------
    //计算utf8字符串s解码后的unicode字符数量
    //返回值:s的长度
    inline uint32_t rx_utf8_decode_size(const uint8_t *s,uint32_t &uni_chars);
*/

    //-----------------------------------------------------
    //单个unicode字符ch的utf8编码,结果放入s
    //返回值:ch对应的utf8串长度
    inline uint8_t rx_utf8_encode(uint32_t ch, uint8_t *s)
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
    //计算unicode字符ch编码后占用的utf8串长度
    //返回值:0错误;其他为串长度
    inline uint8_t rx_utf8_encode_size(uint32_t ch)
    {
        //获取前导比特0的数量
        uint8_t zbit_count = rx_clz(ch);
        const static uint8_t chars_size[] = {0,6,6,6,6,6,5,5,5,5,5,4,4,4,4,4,3,3,3,3,3,2,2,2,2,1,1,1,1,1,1,1,1};
        rx_assert(zbit_count<=32);
        return chars_size[zbit_count];
    }

    //-----------------------------------------------------
    //计算unicode字符串str编码后的utf8串长度
    //返回值:0错误;其他为串长度
    inline uint32_t rx_utf8_encode_size(const wchar_t *str)
    {
        if (!str) return 0;
        wchar_t ch;
        uint32_t rc=0;
        while((ch=*str))
        {
            uint8_t sl = rx_utf8_encode_size(ch);
            rc += (sl==0?3:sl);
        }
        return rc;
    }

    //-----------------------------------------------------
    //unicode字符串str编码为utf8串(外部需确保缓冲区足够长,必要时可调用rx_utf8_encode_size获取)
    //返回值:0错误;其他为utf8串长度
    inline uint32_t rx_utf8_encode(const wchar_t *str,uint8_t *utf8)
    {
    }

    //-----------------------------------------------------
    //对utf8字符串s解码,得到单独的一个unicode字符放入uc;(错误处理简化,仅依赖前导1的比特数)
    //返回值:消耗的字节数(1~6)
    inline uint8_t rx_utf8_decode(const uint8_t *s, uint32_t &uc)
    {
        rx_assert(s!=NULL);

        uc = s[0];
        switch (rx_clz((~uc)&0xFF)-24)                      //获取首字节前导bit置位数,进行分类处理
        {
        case 2: uc = ((s[0] & 0x1F) << 6) | (s[1] & 0x3F);
            return 2;
        case 3: uc = ((s[0] & 0x0F) << 12) | (s[1] & 0x3F) << 6 | (s[2] & 0x3F);
            return 3;
        case 4: uc = ((s[0] & 0x07) << 18) | (s[1] & 0x3F) << 12 | (s[2] & 0x3F) << 6 | (s[3] & 0x3F);
            return 4;
        case 5: uc = ((s[0] & 0x03) << 24) | (s[1] & 0x3F) << 18 | (s[2] & 0x3F) << 12 | (s[3] & 0x3F) << 6 | (s[4] & 0x3F);
            return 5;
        case 6: uc = ((s[0] & 0x03) << 30) | (s[1] & 0x3F) << 24 | (s[2] & 0x3F) << 18 | (s[3] & 0x3F) << 12 | (s[4] & 0x3F) << 6 | (s[5] & 0x3F);
            return 6;
        default:
            return 1;                                       //格式错误或单一字节,都告知消耗1字节
        }
    }

    //-----------------------------------------------------
    //对utf8字符串s解码得到unicode字符串ustr(外部需确保缓冲器足够长,必要时可使用rx_utf8_decode_size计算所需长度)
    //返回值:0出错;其他为unicode串的字符数量
    inline uint32_t rx_utf8_decode(const uint8_t *s,wchar_t* ustr)
    {
    }

    //-----------------------------------------------------
    //计算utf8字符串s解码后消耗的字节数量
    //返回值:消耗的字节数;0错误(s不是一个正确的utf8开始字节).
    inline uint8_t rx_utf8_decode_size(const uint8_t *s)
    {
        rx_assert(s!=NULL);
        const static uint8_t chars_size[]={1,0,2,3,4,5,6};
        uint8_t bc = rx_clz((~(uint32_t)*s)&0xFF)-24;       //获取首字节前导bit置位数
        return (bc<=6?chars_size[bc]:0);
    }

    //-----------------------------------------------------
    //计算utf8字符串s解码后的unicode字符数量
    //返回值:s的长度
    inline uint32_t rx_utf8_decode_size(const uint8_t *s,uint32_t &uni_chars)
    {
        uni_chars=0;
        if (s==NULL) 
            return 0;

        const uint8_t *o=s;
        while(*s)
        {
            uint8_t sc=rx_utf8_decode_size(s);
            s+=(sc==0?1:sc);
            ++uni_chars;
        }
        return s-o;
    }

#endif
