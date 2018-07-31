#ifndef _RX_HASH_INT_H_
#define _RX_HASH_INT_H_

#include "rx_cc_macro.h"

    //-----------------------------------------------------
    //倾向于小范围高密度的素数表,便于构建轻量级哈希表
    inline uint32_t rx_tiny_prime(uint32_t idx)
    {
        static const uint32_t primes[] = {
            2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,113,127,131,137,139,149,151,157,163,167,
            173,179,181,191,193,197,199,211,223,227,229,233,239,241,251,257,263,269,271,277,281,283,293,307,311,331,349,353,373,
            389,401,409,421,431,443,457,461,479,487,499,509,521,541,557,569,577,593,601,613,631,643,659,673,683,701,719,733,743,
            757,769,787,809,821,839,853,877,907,919,937,953,971,991,1009,1021,1051,1087,1103,1129,1151,1171,1193,1217,1237,1259,
            1277,1301,1321,1361,1381,1409,1433,1471,1499,1511,1549,1571,1601,1621,1657,1699,1723,1753,1777,1801,1831,1861,1879,
            1901,1931,1951,1973,1993,2003,2029,2063,2089,2111,2141,2179,2203,2237,2267,2297,2311,2341,2371,2399,2423,2459,2477,
            2503,2531,2557,2591,2617,2647,2677,2707,2731,2767,2791,2819,2843,2861,2897,2927,2963,3001,3109,3203,3301,3407,3511,
            3607,3701,3803,3907,4001,4111,4201,4297,4409,4507,4603,4703,4801,4903,5003,5101,5209,5303,5407,5501,5591,5623,5701,
            5801,5903,6007,6101,6203,6301,6397,6421,6521,6607,6701,6803,6907,7001,7103,7207,7307,7411,7507,7603,7703,7817,7901,
            8009,8101,8209,8311,8419,8501,8609,8707,8803,8923,9001,9103,9203,9311,9403,9511,9601,9719,9803,9901,10007,11003,12007,
            13001,14009,15013,16001,17011,18013,19001,20011,21001,22003,23003,24001,25013,26003,27011,28001,29009,30011,32003,34019,
            36007,38011,40009,42013,44017,46021,48017,50021,52009,54001,56003,58013,60013,62003,64007,66029,68023,70001,75011,80021,
            85009,90001,100003,110017,120011,130003,140009,150001,160001,170003,180001,190027,200003,210011,220009,230003,240007,
            250007,260003,270001,280001,290011,300007,310019,320009,330017,350899,701819,1403641,2807303,5614657,11229331,
            22458671,44917381,89834777,179669557,359339171,718678369,1437356741,2147483647
        };

        if (idx >= sizeof(primes) / sizeof(uint32_t))
            return 0;
        return primes[idx];
    }

    //-----------------------------------------------------
    //轻量级斐波那契序数
    inline uint32_t rx_tiny_fibonacci(uint32_t idx)
    {
        static const uint32_t seqs[] = {
            1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597, 2584, 4181, 6765, 10946, 17711, 28657, 46368, 75025,
            121393, 196418, 317811, 514229, 832040, 1346269,2178309, 3524578, 5702887, 9227465, 14930352, 24157817, 39088169, 63245986,
            102334155, 165580141, 267914296, 433494437, 701408733, 1134903170, 1836311903
        };
        if (idx >= sizeof(seqs) / sizeof(uint32_t))
            return 0;
        return seqs[idx];
    }

    //-----------------------------------------------------
    //integer hash function
    //-----------------------------------------------------
    // Tomas Wang
    template<class DT>
    inline DT rx_hash_tomas32(DT key)
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
    inline DT rx_hash_tomas64(DT key)
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
    inline uint32_t rx_hash_tomas2(uint64_t key)
    {
        key = (~key) + (key << 18); // key = (key << 18) - key - 1;
        key = key ^ (key >> 31);
        key = key * 21; // key = (key + (key << 2)) + (key << 4);
        key = key ^ (key >> 11);
        key = key + (key << 6);
        key = key ^ (key >> 22);
        return uint32_t(key);
    }
    //-----------------------------------------------------
    // Bob Jenkins' 32 bit integer hash function
    // 这六个数是随机数， 通过设置合理的6个数，你可以找到对应的perfect hash.
    template<class DT>
    inline DT rx_hash_bobj(DT a)
    {
        a = (a + 0x7ed55d16) + (a << 12);
        a = (a ^ 0xc761c23c) ^ (a >> 19);
        a = (a + 0x165667b1) + (a << 5);
        a = (a + 0xd3a2646c) ^ (a << 9);
        a = (a + 0xfd7046c5) + (a << 3); // <<和 +的组合是可逆的
        a = (a ^ 0xb55a4f09) ^ (a >> 16);
        return a;
    }

    //-----------------------------------------------------
    //32位整数的Murmur哈希码算法; from code.google.com/p/smhasher/wiki/MurmurHash3
    //对于自然数效果较好
    template<class DT>
    inline DT rx_hash_murmur3(DT h)
    {
        h ^= h >> 16;
        h *= 0x85ebca6b;
        h ^= h >> 13;
        h *= 0xc2b2ae35;
        h ^= h >> 16;
        return h;
    }
    //-----------------------------------------------------
    //黄金分隔哈希算法,极其简单.因子可选斐波那契序数.
    template<class DT>
    inline DT rx_hash_gold(DT x, DT factor = 17711, uint32_t x_shift = 0)
    {
        return (x >> x_shift)* factor;
    }

    //-----------------------------------------------------
    //https://github.com/skeeto/hash-prospector
    //进行过雪崩系数验证的哈希函数(Avalanche score = 1.67)
    template<class DT>
    inline DT rx_hash_mosquito32(DT x)
    {
        x = ~x;
        x ^= x >> 16;
        x *= DT(0xb03a22b3);
        x ^= x >> 10;
        return x;
    }
    //-----------------------------------------------------
    //https://github.com/skeeto/hash-prospector
    //进行过雪崩系数验证的哈希函数(Avalanche score = 1.51)
    template<class DT>
    inline DT rx_hash_skeeto32a(DT x)
    {
        x = ~x;
        x ^= x >> 2;
        x += x << 21;
        x ^= x >> 15;
        x ^= x << 5;
        x ^= x >> 9;
        x ^= x << 13;
        return x;
    }
    //-----------------------------------------------------
    //https://github.com/skeeto/hash-prospector
    //进行过雪崩系数验证的哈希函数(Avalanche score = 1.1875)
    template<class DT>
    inline DT rx_hash_skeeto32b(DT x)
    {
        x = ~x;
        x ^= x << 16;
        x ^= x >> 1;
        x ^= x << 13;
        x ^= x >> 4;
        x ^= x >> 12;
        x ^= x >> 2;
        return x;
    }
    //-----------------------------------------------------
    //https://github.com/skeeto/hash-prospector
    //进行过雪崩系数验证的哈希函数(Avalanche score = 1.03)
    template<class DT>
    inline DT rx_hash_skeeto32c(DT x)
    {
        x ^= x >> 15;
        x *= uint32_t(0x2c1b3c6d);
        x ^= x >> 12;
        x *= uint32_t(0x297a2d39);
        x ^= x >> 15;
        return x;
    }
    //-----------------------------------------------------
    //可用的整数哈希函数类型
    typedef enum rx_int_hash_type
    {
        IHT_tomas = 0,
        IHT_bobj,
        IHT_murmur3,
        IHT_gold,
        IHT_mosquito,
        IHT_skeeto32a,
        IHT_skeeto32b,
        IHT_skeeto32c,

        IHT_Count                                        //当作类型的数量
    }rx_int_hash_type;

    //-----------------------------------------------------
    //根据哈希函数类型获取其对应的算法名称
    inline const char* rx_int_hash_name(rx_data_hash_type Type)
    {
        switch (Type)
        {
        case IHT_tomas:     return "IntHash::tomas";
        case IHT_bobj:      return "IntHash::bobj";
        case IHT_murmur3:   return "IntHash::murmur3";
        case IHT_gold:      return "IntHash::gold";
        case IHT_mosquito:  return "IntHash::mosquito";
        case IHT_skeeto32a: return "IntHash::skeeto32a";
        case IHT_skeeto32b: return "IntHash::skeeto32b";
        case IHT_skeeto32c: return "IntHash::skeeto32c";

        default:            return "Hash::Unknown";
        }
    }

    //-----------------------------------------------------
    //根据哈希函数类型计算给定数据的哈希码
    inline uint32_t rx_int_hash(rx_data_hash_type Type, uint32_t Key)
    {
        switch (Type)
        {
        case IHT_tomas:     return rx_hash_tomas32(Key);
        case IHT_bobj:      return rx_hash_bobj(Key);
        case IHT_murmur3:   return rx_hash_murmur3(Key);
        case IHT_gold:      return rx_hash_gold(Key);
        case IHT_mosquito:  return rx_hash_mosquito32(Key);
        case IHT_skeeto32a: return rx_hash_skeeto32a(Key);
        case IHT_skeeto32b: return rx_hash_skeeto32b(Key);
        case IHT_skeeto32c: return rx_hash_skeeto32c(Key);

        default:            return rx_hash_skeeto32c(Key);
        }

    }

#endif
