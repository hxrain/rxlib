#ifndef _RX_HASH_INT_H_
#define _RX_HASH_INT_H_

#include "rx_cc_macro.h"

    //-----------------------------------------------------
    //integer hash function
    //-----------------------------------------------------
    // Tomas Wang
    template<class DT>
    inline DT hash_tomas32(DT key)
    {
        key = ~key + (key << 15); // key = (key << 15) - key - 1;
        key = key ^ (key >> 12);
        key = key + (key << 2);
        key = key ^ (key >> 4);
        key = key * 2057; // key = (key + (key << 3)) + (key << 11);
        key = key ^ (key >> 16);
        return key;
    }
    //-----------------------------------------------------
    // 64 bit Mix Functions
    template<class DT>
    inline DT hash_tomas64(DT key)
    {
        key = (~key) + (key << 21); // key = (key << 21) - key - 1;
        key = key ^ (key >> 24);
        key = (key + (key << 3)) + (key << 8); // key * 265
        key = key ^ (key >> 14);
        key = (key + (key << 2)) + (key << 4); // key * 21
        key = key ^ (key >> 28);
        key = key + (key << 31);
        return key;
    }

    //-----------------------------------------------------
    // 64 bit to 32 bit Mix Functions
    inline uint32_t hash_tomas2(uint64_t key)
    {
        key = (~key) + (key << 18); // key = (key << 18) - key - 1;
        key = key ^ (key >> 31);
        key = key * 21; // key = (key + (key << 2)) + (key << 4);
        key = key ^ (key >> 11);
        key = key + (key << 6);
        key = key ^ (key >> 22);
        return uint32_t( key );
    }
    //-----------------------------------------------------
    // Bob Jenkins' 32 bit integer hash function
    // 这六个数是随机数， 通过设置合理的6个数，你可以找到对应的perfect hash.
    template<class DT>
    inline DT hash_bobj(DT a)
    {
        a = (a+0x7ed55d16) + (a<<12);
        a = (a^0xc761c23c) ^ (a>>19);
        a = (a+0x165667b1) + (a<<5);
        a = (a+0xd3a2646c) ^ (a<<9);
        a =(a+0xfd7046c5) + (a<<3); // <<和 +的组合是可逆的
        a = (a^0xb55a4f09) ^ (a>>16);
        return a;
    }
    
    //-----------------------------------------------------
    //32位整数的Murmur哈希码算法; from code.google.com/p/smhasher/wiki/MurmurHash3
    //对于自然数效果较好
    template<class DT>
    inline DT hash_murmur3(DT h)
    {
        h ^= h >> 16;
        h *= 0x85ebca6b;
        h ^= h >> 13;
        h *= 0xc2b2ae35;
        h ^= h >> 16;
        return h;
    }

    //-----------------------------------------------------
    //黄金分隔哈希算法,对于自然数效果较好
    template<class DT>
    inline DT hash_gold_a(DT addr)
    {
        return addr * 21911;
    }
    //黄金分隔哈希算法,对于内存地址效果较好(四字节对齐)
    template<class DT>
    inline DT hash_gold_a2(DT addr)
    {
        return (addr>>2) * 21911;
    }
    //黄金分隔哈希算法,对于内存地址效果较好(八字节对齐)
    template<class DT>
    inline DT hash_gold_a3(DT addr)
    {
        return (addr>>3) * 21911;
    }
    //-----------------------------------------------------
    //黄金分隔哈希算法,对于自然数效果较好
    template<class DT>
    inline DT hash_gold_b(DT addr)
    {
        return addr * 1403641;
    }
    //黄金分隔哈希算法,对于内存地址效果较好(四字节对齐)
    template<class DT>
    inline DT hash_gold_b2(DT addr)
    {
        return (addr>>2) * 1403641;
    }
    //黄金分隔哈希算法,对于内存地址效果较好(八字节对齐)
    template<class DT>
    inline DT hash_gold_b3(DT addr)
    {
        return (addr>>3) * 1403641;
    }
    //-----------------------------------------------------
    //黄金分隔哈希算法,对于自然数效果较好
    template<class DT>
    inline DT hash_gold_c(DT addr)
    {
        return addr * 11229331;
    }
    //黄金分隔哈希算法,对于内存地址效果较好(四字节对齐)
    template<class DT>
    inline DT hash_gold_c2(DT addr)
    {
        return (addr>>2) * 11229331;
    }
    //黄金分隔哈希算法,对于内存地址效果较好(八字节对齐)
    template<class DT>
    inline DT hash_gold_c3(DT addr)
    {
        return (addr>>3) * 11229331;
    }
    //-----------------------------------------------------
    //https://github.com/skeeto/hash-prospector
    template<class DT>
    inline DT hash_mosquito32(DT x)
    {
        x  = ~x;
        x ^= x >> 16;
        x *= DT(0xb03a22b3);
        x ^= x >> 10;
        return x;
    }
    //-----------------------------------------------------
    //https://github.com/skeeto/hash-prospector
    template<class DT>
    inline DT hash_skeeto32(DT x)
    {
        x  = ~x;
        x ^= x >> 2;
        x += x << 21;
        x ^= x >> 15;
        x ^= x << 5;
        x ^= x >> 9;
        x ^= x << 13;
        return x;
    }

#endif