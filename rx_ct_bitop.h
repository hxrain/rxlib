#ifndef _H_RX_BITS_OP_H_
#define _H_RX_BITS_OP_H_
#include "rx_cc_macro.h"

    //-----------------------------------------------------
    //静态计算字节翻转 0x0101..01 * x
    template<class T,uint8_t x>
    T rx_byte_flip()
    {
        return ((~T(0))/0xff) * x;
    }
    //动态计算字节翻转 0x0101..01 * x
    template<class T>
    T rx_byte_flip(uint8_t x)
    {
        return ((~T(0))/0xff) * x;
    }

    //-----------------------------------------------------
    //判断x是否为2的指数倍
    template<class T>
    inline bool rx_is_pow2(T x) { return x && !(x & (x - 1)); }

    //-----------------------------------------------------
    //判断是否有一个字节为0(包含错位匹配)
    template<class T>
    inline bool rx_has_zero(T x)
    {
        T b_1 = rx_byte_flip<T, 1>();
        T b_0x80 = rx_byte_flip<T, 0x80>();

        return !!((x - b_1) & ~x & b_0x80);
    }

    //-----------------------------------------------------
    //判断是否有一个字节为a(包含错位匹配)
    template<class T>
    inline bool rx_has_byte(T x, uint8_t a)
    {
        T b_a = rx_byte_flip<T>(a);
        return rx_has_zero(x ^ b_a);
    }

    //-----------------------------------------------------
    //循环左移
    inline uint32_t rx_rol(uint32_t value, int count)
    {
        int r = count % 32;
        return (value << r) | (value >> (32 - r));
    }

    inline uint64_t rx_rol(uint64_t value, int count)
    {
        int r = count % 64;
        return (value << r) | (value >> (64 - r));
    }
    //-----------------------------------------------------
    //循环右移
    inline uint32_t rx_ror(uint32_t value, int count)
    {
        int r = count % 32;
        return (value >> r) | (value << (32 - r));
    }

    inline uint64_t rx_ror(uint64_t value, int count)
    {
        int r = count % 64;
        return (value >> r) | (value << (64 - r));
    }

#if ( RX_CC==RX_CC_GCC||RX_CC==RX_CC_CLANG)
    //GCC系列编译器有完整的内建函数
    //-----------------------------------------------------
    //计算前导1(高位)的位置,返回值(0-没有置位;1~n为比特序号)
    inline uint8_t rx_fls(uint32_t x) { return x ? 32 - __builtin_clz(x) : 0; }
    inline uint8_t rx_fls(uint64_t x) { return x ? 64 - __builtin_clzll(x) : 0; }

    //-----------------------------------------------------
    //计算尾随1(低位)的位置,返回值(0-没有置位;1~n为比特序号)
    inline uint8_t rx_ffs(uint32_t x) { return x ? __builtin_ffs(x) : 0; }
    inline uint8_t rx_ffs(uint64_t x) { return x ? __builtin_ffsll(x) : 0; }

    //-----------------------------------------------------
    //计算被置位的数量
    inline uint8_t rx_popcnt    (uint32_t x){return __builtin_popcount(x);}
    inline uint8_t rx_popcnt    (uint64_t x){return __builtin_popcountll(x);}

    //-----------------------------------------------------
    //计算前导0(高位)的数量
    inline uint8_t rx_clz       (uint32_t x){return x?__builtin_clz(x):32;}
    inline uint8_t rx_clz       (uint64_t x){return x?__builtin_clzll(x):64;}

    //-----------------------------------------------------
    //计算尾随0(低位)的数量
    inline uint8_t rx_ctz       (uint32_t x){return x?__builtin_ctz(x):32;}
    inline uint8_t rx_ctz       (uint64_t x){return x?__builtin_ctzll(x):64;}

#elif RX_CC==RX_CC_VC
    //使用VC内建函数实现
    #include <intrin.h>
    //-----------------------------------------------------
    //计算前导1(高位)的位置,返回值(0-没有置位;1~n为比特序号)
    inline uint8_t rx_fls(uint32_t x) { DWORD index = 0; return _BitScanReverse(&index, x) ? (uint8_t)index + 1 : 0; }
    #if RX_CC_BIT==64
    inline uint8_t rx_fls(uint64_t x) { DWORD index = 0; return _BitScanReverse64(&index, x) ? (uint8_t)index + 1 : 0; }
    #else
    inline uint8_t rx_fls(uint64_t x)
    {
        uint32_t h = uint32_t(x >> 32);
        return h ? 32 + rx_fls(h) : rx_fls((uint32_t)x);
    }
    #endif
    //-----------------------------------------------------
    //计算尾随1(低位)的位置,返回值(0-没有置位;1~n为比特序号)
    inline uint8_t rx_ffs(uint32_t x) { DWORD index = 0; return _BitScanForward(&index, x) ? (uint8_t)index + 1 : 0; }
    #if RX_CC_BIT==64
    inline uint8_t rx_ffs(uint64_t x) { DWORD index = 0; return _BitScanForward64((DWORD*)&index, x) ? (uint8_t)index + 1 : 0; }
    #else
    inline uint8_t rx_ffs(uint64_t x)
    {
        if (!x) return 0;
        uint32_t l = uint32_t(x);
        return l ? rx_ffs(l):32+rx_ffs(uint32_t(x>>32));
    }
    #endif

    //-----------------------------------------------------
    //计算被置位的数量
    inline uint8_t rx_popcnt(uint32_t x) { return __popcnt(x); }
    #if RX_CC_BIT==64
    inline uint8_t rx_popcnt(uint64_t x) { return (uint8_t)__popcnt64(x); }
    #else
    inline uint8_t rx_popcnt(uint64_t x) { return (uint8_t)__popcnt(uint32_t(x))+ __popcnt(uint32_t(x>>32)); }
    #endif

    //-----------------------------------------------------
    //计算前导0(高位)的数量
    inline uint8_t rx_clz(uint32_t x) { return x ? 32 - rx_fls(x) : 32; }
    inline uint8_t rx_clz(uint64_t x) { return x ? 64 - rx_fls(x) : 64; }

    //-----------------------------------------------------
    //计算尾随0(低位)的数量
    inline uint8_t rx_ctz(uint32_t x) { return x ? rx_ffs(x)-1 : 32; }
    inline uint8_t rx_ctz(uint64_t x) { return x ? rx_ffs(x)-1 : 64; }


#else
    //没有编译器内建函数的C算法版本
    //-----------------------------------------------------
    //计算前导1(高位)的位置,返回值(0-没有置位;1~n为比特序号)
    inline uint8_t rx_fls(uint32_t x)
    {
        int bit = 32;

        if (!x) bit -= 1;
        if (!(x & 0xffff0000)) { x <<= 16; bit -= 16; }
        if (!(x & 0xff000000)) { x <<= 8; bit -= 8; }
        if (!(x & 0xf0000000)) { x <<= 4; bit -= 4; }
        if (!(x & 0xc0000000)) { x <<= 2; bit -= 2; }
        if (!(x & 0x80000000)) { x <<= 1; bit -= 1; }

        return bit;
    }
    inline uint8_t rx_fls(uint64_t x)
    {
        uint32_t high = (uint32_t)(x >> 32);
        if (high)
            return 32 + rx_fls(high);
        else
            return rx_fls((uint32_t)x & 0xffffffff);
    }

    //-----------------------------------------------------
    //计算尾随1(低位)的位置,返回值(0-没有置位;1~n为比特序号)
    inline uint8_t rx_ffs(uint32_t x) { return rx_fls(x & (~x + 1)); }
    inline uint8_t rx_ffs(uint64_t x) { return rx_fls(x & (~x + 1)); }

    //-----------------------------------------------------
    //计算被置位的数量
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
    template<int32_t> inline uint8_t rx_popcnt(int32_t x) { return rx_popcnt<uint32_t>(x); }
    template<int64_t> inline uint8_t rx_popcnt(int64_t x) { return rx_popcnt<uint64_t>(x); }

    //-----------------------------------------------------
    //计算前导0的数量
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
    //计算尾随0(低位)的数量
    template<class T> inline unsigned rx_ctz(T x) { return rx_popcnt((x & -x) - 1); }
    template<int32_t> inline uint8_t rx_ctz(int32_t  x) { return rx_ctz<uint32_t>(x); }
    template<int64_t> inline uint8_t rx_ctz(int64_t  x) { return rx_ctz<uint64_t>(x); }

#endif

#endif
