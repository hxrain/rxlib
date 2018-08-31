#ifndef _RX_HASH_INT_H_
#define _RX_HASH_INT_H_

#include "rx_cc_macro.h"

    //-----------------------------------------------------
    //倾向于小范围高密度的素数表,便于构建轻量级哈希表
    static const uint32_t rx_tiny_prime_array[] = {
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
    const uint32_t rx_tiny_prime_count=338;
    inline uint32_t rx_tiny_prime(const uint32_t idx)
    {
        rx_static_assert(rx_tiny_prime_count==sizeof(rx_tiny_prime_array) / sizeof(rx_tiny_prime_array[0]));
        return rx_tiny_prime_array[idx>=rx_tiny_prime_count?rx_tiny_prime_count-1:idx];
    }

    //-----------------------------------------------------
    //轻量级斐波那契序数
    static const uint32_t rx_tiny_fibonacci_seqs[] = {
        1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597, 2584, 4181, 6765, 10946, 17711, 28657, 46368, 75025,
        121393, 196418, 317811, 514229, 832040, 1346269,2178309, 3524578, 5702887, 9227465, 14930352, 24157817, 39088169, 63245986,
        102334155, 165580141, 267914296, 433494437, 701408733, 1134903170, 1836311903
    };
    const uint32_t rx_tiny_fibonacci_count=45;
    inline uint32_t rx_tiny_fibonacci(const uint32_t idx)
    {
        rx_static_assert(rx_tiny_fibonacci_count== sizeof(rx_tiny_fibonacci_seqs) / sizeof(rx_tiny_fibonacci_seqs[0]));
        return rx_tiny_fibonacci_seqs[idx>=rx_tiny_fibonacci_count?rx_tiny_fibonacci_count-1:idx];
    }
    //-----------------------------------------------------
    //黄金分隔哈希算法,极其简单.因子可选斐波那契序数.使用此函数给定不同的参数即可造就一系列的哈希函数组
    inline uint32_t rx_hash_gold(uint32_t x, uint32_t factor = 17711, uint32_t r_shift = 0)
    {
        return (x >> r_shift)* factor;
    }

    //-----------------------------------------------------
    //integer hash function
    //-----------------------------------------------------
    typedef uint32_t(*rx_int_hash32_t)(uint32_t x);
    //-----------------------------------------------------
    // Tomas Wang
    inline uint32_t rx_hash_tomas(uint32_t key)
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
    inline uint64_t rx_hash_tomas(uint64_t key)
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
    inline uint32_t rx_hash_tomas64to32(uint64_t key)
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
    inline uint32_t rx_hash_bobj(uint32_t a)
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
    //对于自然数效果较好(exact bias: 0.26398543281818287)
    inline uint32_t rx_hash_murmur3(uint32_t h)
    {
        h ^= h >> 16;
        h *= 0x85ebca6b;
        h ^= h >> 13;
        h *= 0xc2b2ae35;
        h ^= h >> 16;
        return h;
    }

    //-----------------------------------------------------
    //https://github.com/skeeto/hash-prospector
    //进行过雪崩系数验证的哈希函数(Avalanche score = 1.67)
    inline uint32_t rx_hash_mosquito(uint32_t x)
    {
        x = ~x;
        x ^= x >> 16;
        x *= uint32_t(0xb03a22b3);
        x ^= x >> 10;
        return x;
    }

    //-----------------------------------------------------
    //https://github.com/skeeto/hash-prospector
    //-----------------------------------------------------
    //进行过雪崩系数验证的哈希函数(Avalanche score = 1.51)
    inline uint32_t rx_hash_skeeto_a(uint32_t x)
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
    inline uint32_t rx_hash_skeeto_b(uint32_t x)
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
    //进行过雪崩系数验证的哈希函数(Avalanche score = 1.03 / exact bias: 0.34968228323361017)
    inline uint32_t rx_hash_skeeto_c(uint32_t x)
    {
        x ^= x >> 15;
        x *= uint32_t(0x2c1b3c6d);
        x ^= x >> 12;
        x *= uint32_t(0x297a2d39);
        x ^= x >> 15;
        return x;
    }
    //进行过统计验证的哈希函数(exact bias: 0.20207553121367283)
    inline uint32_t rx_hash_skeeto_d(uint32_t x)
    {
        x ^= x >> 16;
        x *= uint32_t(0xe2d0d4cb);
        x ^= x >> 15;
        x *= uint32_t(0x3c6ad939);
        x ^= x >> 15;
        return x;
    }

    //进行过统计验证的哈希函数(exact bias: 0.19768193144773874)
    inline uint32_t rx_hash_skeeto_e(uint32_t x)
    {
        x ^= x >> 18;
        x *= uint32_t(0xa136aaad);
        x ^= x >> 16;
        x *= uint32_t(0x9f6d62d7);
        x ^= x >> 17;
        return x;
    }

    //进行过统计验证的哈希函数(exact bias: 0.19768193144773874)
    inline uint32_t rx_hash_skeeto_e_r(uint32_t x)
    {
        x ^= x >> 17;
        x *= uint32_t(0x5f68b0e7);
        x ^= x >> 16;
        x *= uint32_t(0x79e64925);
        x ^= x >> 18;
        return x;
    }

    //进行过统计验证的哈希函数(exact bias: 0.022829781930394154)
    inline uint32_t rx_hash_skeeto_f(uint32_t x)
    {
        x ^= x >> 18;
        x *= uint32_t(0xed5ad4bb);
        x ^= x >> 12;
        x *= uint32_t(0xac4c1b51);
        x ^= x >> 17;
        x *= uint32_t(0xc0a8e5d7);
        x ^= x >> 12;
        return x;
    }

    //进行过统计验证的哈希函数(exact bias: 0.021334944237993255)
    inline uint32_t rx_hash_skeeto_g(uint32_t x)
    {
        x ^= x >> 18;
        x *= uint32_t(0xed5ad4bb);
        x ^= x >> 11;
        x *= uint32_t(0xac4c1b51);
        x ^= x >> 15;
        x *= uint32_t(0x31848bab);
        x ^= x >> 14;
        return x;
    }

    //进行过统计验证的哈希函数(exact bias: 0.020829410544597495)
    inline uint32_t rx_hash_skeeto_triple(uint32_t x)
    {
        ++x;
        x ^= x >> 17;
        x *= uint32_t(0xed5ad4bb);
        x ^= x >> 11;
        x *= uint32_t(0xac4c1b51);
        x ^= x >> 15;
        x *= uint32_t(0x31848bab);
        x ^= x >> 14;
        return x;
    }
    inline uint32_t rx_hash_skeeto_triple_r(uint32_t x)
    {
        x ^= x >> 14 ^ x >> 28;
        x *= uint32_t(0x32b21703);
        x ^= x >> 15 ^ x >> 30;
        x *= uint32_t(0x469e0db1);
        x ^= x >> 11 ^ x >> 22;
        x *= uint32_t(0x79a85073);
        x ^= x >> 17;
        --x;
        return x;
    }

    //-----------------------------------------------------
    //整数哈希函数类型
    typedef enum rx_int_hash32_type
    {
        IHT_tomas = 0,
        IHT_bobj,
        IHT_murmur3,
        IHT_mosquito,
        IHT_skeeto_a,
        IHT_skeeto_b,
        IHT_skeeto_c,
        IHT_skeeto_d,
        IHT_skeeto_e,
        IHT_skeeto_e_r,
        IHT_skeeto_f,
        IHT_skeeto_g,
        IHT_skeeto_triple_r,
        IHT_skeeto_triple,

        IHT_Count                                        //当作类型的数量
    }rx_int_hash32_type;

    //-----------------------------------------------------
    //根据哈希函数类型获取其对应的算法名称
    inline const char* rx_int_hash32_name(rx_int_hash32_type Type)
    {
        switch (Type)
        {
        case IHT_tomas:             return "IntHash::tomas";
        case IHT_bobj:              return "IntHash::bobj";
        case IHT_murmur3:           return "IntHash::murmur3";
        case IHT_mosquito:          return "IntHash::mosquito";
        case IHT_skeeto_a:          return "IntHash::skeeto_a";
        case IHT_skeeto_b:          return "IntHash::skeeto_b";
        case IHT_skeeto_c:          return "IntHash::skeeto_c";
        case IHT_skeeto_d:          return "IntHash::skeeto_d";
        case IHT_skeeto_e:          return "IntHash::skeeto_e";
        case IHT_skeeto_e_r:        return "IntHash::skeeto_e_r";
        case IHT_skeeto_f:          return "IntHash::skeeto_f";
        case IHT_skeeto_g:          return "IntHash::skeeto_g";
        case IHT_skeeto_triple_r:   return "IntHash::skeeto_triple_r";
        case IHT_skeeto_triple:     return "IntHash::skeeto_triple";

        default:            return "Hash::Unknown";
        }
    }

    //-----------------------------------------------------
    //已实现的整数哈希函数指针列表
    static const rx_int_hash32_t rx_int_hash32_funcs[] = {
        rx_hash_tomas,
        rx_hash_bobj,
        rx_hash_murmur3,
        rx_hash_mosquito,
        rx_hash_skeeto_a,
        rx_hash_skeeto_b,
        rx_hash_skeeto_c,
        rx_hash_skeeto_d,
        rx_hash_skeeto_e,
        rx_hash_skeeto_e_r,
        rx_hash_skeeto_f,
        rx_hash_skeeto_g,
        rx_hash_skeeto_triple_r,
        rx_hash_skeeto_triple,
    };
    const uint32_t rx_int_hash32_funcs_count=IHT_Count;
    //根据给的类型,获取指定哈希函数
    inline rx_int_hash32_t rx_int_hash32(const rx_int_hash32_type Type)
    {
        rx_static_assert(rx_int_hash32_funcs_count==sizeof(rx_int_hash32_funcs)/sizeof(rx_int_hash32_funcs[0]));
        return rx_int_hash32_funcs[Type>=rx_int_hash32_funcs_count?rx_int_hash32_funcs_count-1:Type];
    }

    //-----------------------------------------------------
    //根据哈希函数类型计算结果的整数哈希函数族
    inline uint32_t rx_int_hash32(uint32_t Key, rx_int_hash32_type Type= IHT_skeeto_triple)
    {
        return rx_int_hash32(Type)(Key);
    }


    //-----------------------------------------------------
    //算法一致性的整数哈希函数族(三绕模式)
    //-----------------------------------------------------
    template<uint32_t shr_1,uint32_t mul_1,uint32_t shr_2,uint32_t mul_2,uint32_t shr_3,uint32_t mul_3,uint32_t shr_4>
    inline uint32_t rx_int_hash32_skeeto3(uint32_t x)
    {
        ++x;
        x ^= x >> shr_1;
        x *= mul_1;
        x ^= x >> shr_2;
        x *= mul_2;
        x ^= x >> shr_3;
        x *= mul_3;
        x ^= x >> shr_4;
        return x;
    }

    //根据给的序号,获取三绕整数哈希函数
    static const rx_int_hash32_t rx_int_hash32_skeeto3s_funcs[] = {
        rx_int_hash32_skeeto3< 17, 0xed5ad4bb, 11, 0xac4c1b51, 15, 0x31848bab, 14 >, //(exact bias:0.020829410544597495)
        rx_int_hash32_skeeto3< 18, 0xed5ad4bb, 11, 0xac4c1b51, 15, 0x31848bab, 14 >, //(exact bias:0.021334944237993255)
        rx_int_hash32_skeeto3< 16, 0xaeccedab, 14, 0xac613e37, 16, 0x19c89935, 17 >, //exact bias: 0.021246568167078764
        rx_int_hash32_skeeto3< 18, 0x4260bb47, 13, 0x27e8e1ed, 15, 0x9d48a33b, 15 >, //exact bias: 0.021576730651802156
        rx_int_hash32_skeeto3< 17, 0x3f6cde45, 12, 0x51d608ef, 16, 0x6e93639d, 17 >, //exact bias: 0.021772288363808408
        rx_int_hash32_skeeto3< 15, 0x5dfa224b, 14, 0x4bee7e4b, 17, 0x930ee371, 15 >, //exact bias: 0.021845216288848130
        rx_int_hash32_skeeto3< 16, 0x66046c65, 14, 0xd3f0865b, 16, 0xf9999193, 16 >, //exact bias: 0.021944606836500700
        rx_int_hash32_skeeto3< 16, 0xb1a89b33, 14, 0x09136aaf, 16, 0x5f2a44a7, 15 >, //exact bias: 0.021998624107282542
        rx_int_hash32_skeeto3< 15, 0x4df8395b, 15, 0x466b428b, 16, 0xb4b2868b, 16 >, //exact bias: 0.022140187420461286
        rx_int_hash32_skeeto3< 16, 0x2bbed51b, 14, 0xcd09896b, 16, 0x38d4c587, 15 >, //exact bias: 0.022159936298777144
        rx_int_hash32_skeeto3< 16, 0x0ab694cd, 14, 0x4c139e47, 16, 0x11a42c3b, 16 >, //exact bias: 0.022209281912203550
        rx_int_hash32_skeeto3< 17, 0x7f1e072b, 12, 0x8750a507, 16, 0xecbb5b5f, 16 >, //exact bias: 0.022283743052847804
        rx_int_hash32_skeeto3< 16, 0x66e756d5, 14, 0xb5f5a9cd, 16, 0x84e56b11, 16 >, //exact bias: 0.022372957847491555
        rx_int_hash32_skeeto3< 15, 0x233354bb, 15, 0xce1247bd, 16, 0x855089bb, 17 >, //exact bias: 0.022406591070966285
        rx_int_hash32_skeeto3< 16, 0x8288ab57, 14, 0x0d1bfe57, 16, 0x131631e5, 16 >, //exact bias: 0.022431656871313443
        rx_int_hash32_skeeto3< 16, 0x45109e55, 14, 0x3b94759d, 16, 0xadf31ea5, 17 >, //exact bias: 0.022436433678417977
        rx_int_hash32_skeeto3< 16, 0x7001e6eb, 14, 0xbb8e7313, 16, 0x3aa8c523, 15 >, //exact bias: 0.022491767264054854
        rx_int_hash32_skeeto3< 16, 0x49ed0a13, 14, 0x83588f29, 15, 0x658f258d, 15 >, //exact bias: 0.022500668856510898
        rx_int_hash32_skeeto3< 16, 0x6cdb9705, 14, 0x4d58d2ed, 14, 0xc8642b37, 16 >, //exact bias: 0.022504626537729222
        rx_int_hash32_skeeto3< 16, 0xa986846b, 14, 0xbdd5372d, 15, 0xad44de6b, 17 >, //exact bias: 0.022528238323120016
        rx_int_hash32_skeeto3< 16, 0xc9575725, 15, 0x9448f4c5, 16, 0x3b7a5443, 16 >, //exact bias: 0.022586511310042686
        rx_int_hash32_skeeto3< 15, 0xfc54c453, 13, 0x08213789, 15, 0x669f96eb, 16 >, //exact bias: 0.022591114646032095
        rx_int_hash32_skeeto3< 16, 0xd47ef17b, 14, 0x642fa58f, 16, 0xa8b65b9b, 16 >, //exact bias: 0.022600633971701509
        rx_int_hash32_skeeto3< 16, 0x953a55e9, 15, 0x8523822b, 17, 0x56e7aa63, 15 >, //exact bias: 0.022667180032713324
        rx_int_hash32_skeeto3< 16, 0xa3d7345b, 15, 0x7f41c9c7, 16, 0x308bd62d, 17 >, //exact bias: 0.022688845770122031
        rx_int_hash32_skeeto3< 16, 0x195565c7, 14, 0x16064d6f, 16, 0x0f9ec575, 15 >, //exact bias: 0.022697810688752193
        rx_int_hash32_skeeto3< 16, 0x13566dbb, 14, 0x59369a03, 15, 0x990f9d1b, 16 >, //exact bias: 0.022712430070797596
        rx_int_hash32_skeeto3< 16, 0x8430cc4b, 15, 0xa7831cbd, 15, 0xc6ccbd33, 15 >, //exact bias: 0.022734765033419774
        rx_int_hash32_skeeto3< 18, 0xed5ad4bb, 12, 0xac4c1b51, 17, 0xc0a8e5d7, 12 >, //(exact bias:0.022829781930394154)
        rx_int_hash32_skeeto3< 16, 0x699f272b, 14, 0x09c01023, 16, 0x39bd48c3, 15 >, //exact bias: 0.022854175321846512
        rx_int_hash32_skeeto3< 15, 0x336536c3, 13, 0x4f0e38b1, 16, 0x15d229f7, 16 >, //exact bias: 0.022884125170795171
        rx_int_hash32_skeeto3< 16, 0x221f686d, 12, 0xd8948a07, 16, 0xed8a8345, 16 >, //exact bias: 0.022902500408830236
        rx_int_hash32_skeeto3< 16, 0xd7ca8cbb, 13, 0xeb4e259f, 15, 0x34ab1143, 16 >, //exact bias: 0.022905955538176669
        rx_int_hash32_skeeto3< 16, 0x7cb04f65, 14, 0x9b96da73, 16, 0x83625687, 15 >, //exact bias: 0.022906573700088178
        rx_int_hash32_skeeto3< 15, 0x5156196b, 14, 0x940d8869, 15, 0x0086f473, 17 >, //exact bias: 0.022984943828687553
    };
    const uint32_t rx_int_hash32_skeeto3s_count=35;
    inline const rx_int_hash32_t rx_int_hash32_skeeto3s(const uint32_t idx=0)
    {
        rx_static_assert(sizeof(rx_int_hash32_skeeto3s_funcs)/sizeof(rx_int_hash32_skeeto3s_funcs[0])==rx_int_hash32_skeeto3s_count);
        return rx_int_hash32_skeeto3s_funcs[idx>=rx_int_hash32_skeeto3s_count?0:idx];
    }


#endif


