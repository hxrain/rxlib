﻿#ifndef _H_RX_BITS_OP_H_
#define _H_RX_BITS_OP_H_
#include "rx_cc_macro.h"

    //---------------------------------------------------------
    //对变量b进行值v操作
    #define bits_tst(b,v)       ((b)&(v))                         //获取b&v的结果
    #define bits_set(b,v)       ((b)|=(v))                        //给b设置v的值
    #define bits_clr(b,v)       ((b)&=~(v))                       //对b清理v的值

    //---------------------------------------------------------
    //对变量b进行位序操作(位序i从0开始)
    #define bit_tst(b,i)        bits_tst(b,(1<<(i)))
    #define bit_set(b,i)        bits_set(b,(1<<(i)))
    #define bit_clr(b,i)        bits_clr(b,(1<<(i)))

    //---------------------------------------------------------
    //获取变量b的指定位序i与位数的值(位序i从0开始,位数方向从右向左计算)
    #define bits_get(b,i)       (!!((b)&(1<<i)))
    #define bits_get2(b,i)      (((b)&(0x03<<(i)))>>(i))
    #define bits_get3(b,i)      (((b)&(0x07<<(i)))>>(i))
    #define bits_get4(b,i)      (((b)&(0x0f<<(i)))>>(i))
    #define bits_get5(b,i)      (((b)&(0x1f<<(i)))>>(i))
    #define bits_get6(b,i)      (((b)&(0x3f<<(i)))>>(i))
    #define bits_get7(b,i)      (((b)&(0x7f<<(i)))>>(i))
    #define bits_get8(b,i)      (((b)&(0xff<<(i)))>>(i))

    //---------------------------------------------------------
    //设置变量b的指定位序i与位数的值为v(位序i从0开始,位数方向从右向左计算)
    #define bits_put(b,i,v)     (((b)&=~(0x01<<(i))),(b|=(((v)&0x01)<<(i))))
    #define bits_put2(b,i,v)    (((b)&=~(0x03<<(i))),(b|=(((v)&0x03)<<(i))))
    #define bits_put3(b,i,v)    (((b)&=~(0x07<<(i))),(b|=(((v)&0x07)<<(i))))
    #define bits_put4(b,i,v)    (((b)&=~(0x0f<<(i))),(b|=(((v)&0x0f)<<(i))))
    #define bits_put5(b,i,v)    (((b)&=~(0x1f<<(i))),(b|=(((v)&0x1f)<<(i))))
    #define bits_put6(b,i,v)    (((b)&=~(0x3f<<(i))),(b|=(((v)&0x3f)<<(i))))
    #define bits_put7(b,i,v)    (((b)&=~(0x7f<<(i))),(b|=(((v)&0x7f)<<(i))))
    #define bits_put8(b,i,v)    (((b)&=~(0xff<<(i))),(b|=(((v)&0xff)<<(i))))

    //-----------------------------------------------------
    //计算字节翻转 0x0101..01 * x
    template<class T,uint8_t x>
    T rx_byte_flip()
    {
        return ((~T(0))/0xff) * x;
    }
    //计算字节翻转 0x0101..01 * x
    template<class T>
    T rx_byte_flip(uint8_t x)
    {
        return ((~T(0))/0xff) * x;
    }


#if ( RX_CC==RX_CC_GCC||RX_CC==RX_CC_MINGW32||RX_CC==RX_CC_MINGW64)
    //-----------------------------------------------------
    //计算x中被置位的数量
    inline uint8_t rx_popcnt    (uint32_t x){return __builtin_popcount(x);}
    inline uint8_t rx_popcnt    (uint64_t x){return __builtin_popcountll(x);}

    //-----------------------------------------------------
    //计算x中前导0(高位)的数量
    inline uint8_t rx_clz       (uint32_t x){return x?__builtin_clz(x):32;}
    inline uint8_t rx_clz       (uint64_t x){return x?__builtin_clzll(x):64;}

    //-----------------------------------------------------
    //计算x中尾随0(低位)的数量
    inline uint8_t rx_ctz       (uint32_t x){return x?__builtin_ctz(x):32;}
    inline uint8_t rx_ctz       (uint64_t x){return x?__builtin_ctzll(x):64;}
#else
    //-----------------------------------------------------
    //计算x中被置位的数量
    template<class T>
    unsigned rx_popcnt(T x)
    {
        T b_0x01 = rx_byte_flip<T,0x01>();
        T b_0x55 = rx_byte_flip<T,0x55>();
        T b_0x33 = rx_byte_flip<T,0x33>();
        T b_0x0f = rx_byte_flip<T,0x0f>();

        // sum adjacent bits
        x -= (x >> 1) & b_0x55;
        // sum adjacent pairs of bits
        x = (x & b_0x33) + ((x >> 2) & b_0x33);
        // sum adjacent quartets of bits
        x = (x + (x >> 4)) & b_0x0f;

        // sum all bytes
        return (x * b_0x01) >> (sizeof(T)-1)*8;
    }
    //-----------------------------------------------------
    //计算x中前导0的数量
    inline uint8_t rx_clz(uint32_t x)
    {
        x |= (x >> 1);
        x |= (x >> 2);
        x |= (x >> 4);
        x |= (x >> 8);
        x |= (x >> 16);
        return(32 - rx_popcnt(x));
    }
    //-----------------------------------------------------
    inline uint8_t rx_clz(uint64_t x)
    {
        x |= (x >> 1);
        x |= (x >> 2);
        x |= (x >> 4);
        x |= (x >> 8);
        x |= (x >> 16);
        x |= (x >> 32);
        return(64 - rx_popcnt(x));
    }
    //-----------------------------------------------------
    //计算x中尾随0(低位)的数量
    template<class T>
    inline unsigned rx_ctz(T x){return rx_popcnt((x & -x) - 1);}
#endif

    //-----------------------------------------------------
    //判断x是否为2的指数倍
    template<class T>
    inline bool rx_is_pow2(T x){return x && !(x & (x - 1));}

    //-----------------------------------------------------
    //判断x中是否有一个字节为0
    template<class T>
    inline bool rx_has_zero(T x)
    {
        T b_1 = rx_byte_flip<T,1>();
        T b_0x80 = rx_byte_flip<T,0x80>();

        return (x - b_1) & ~x & b_0x80;
    }

    //-----------------------------------------------------
    //判断x中是否有一个字节为a
    template<class T>
    inline bool rx_has_byte(T x, uint8_t a)
    {
        T b_a = rx_byte_flip<T>(a);
        return rx_has_zero(x ^ b_a);
    }


#endif
