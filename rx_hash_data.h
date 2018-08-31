#ifndef _RX_HASH_DATA_H_
#define _RX_HASH_DATA_H_

#include "rx_cc_macro.h"
#include "rx_hash_int.h"

    //-----------------------------------------------------
    //data hash function
    typedef uint32_t (*rx_data_hash32_t)(const void *data, uint32_t len, uint32_t seed);
    //-----------------------------------------------------

    // RS Hash Function
    inline uint32_t rx_hash_rs(const void* data,uint32_t Len, uint32_t seed = 0)
    {
        const uint32_t b = 378551 ;
        uint32_t a = 63689 ;
        uint32_t hash = seed ;
        for(uint32_t i=0;i<Len;i++)
        {
            hash = hash * a + ((uint8_t*)data)[i];
            a *= b;
        }
        return hash;
    }
    template<class CT>
    inline uint32_t rx_hash_rs(const CT* str, uint32_t seed = 0)
    {
        const uint32_t b = 378551 ;
        uint32_t a = 63689 ;
        uint32_t hash = seed ;
        while (*str)
        {
            hash = hash * a + (*str ++ );
            a *= b;
        }
        return hash;
    }

    //-----------------------------------------------------
    // JS Hash Function
    inline uint32_t rx_hash_js(const void* data,uint32_t Len, uint32_t seed = 1315423911)
    {
        uint32_t hash = seed ;
        for(uint32_t i=0;i<Len;i++)
            hash ^= ((hash << 5 ) + ((uint8_t*)data)[i] + (hash >> 2 ));
        return hash;
    }
    template<class CT>
    inline uint32_t rx_hash_js(const CT* str, uint32_t seed = 1315423911)
    {
        uint32_t hash = seed ;
        while (*str)
            hash ^= ((hash << 5 ) + (*str ++ ) + (hash >> 2 ));
        return hash;
    }

    //-----------------------------------------------------
    // P. J. Weinberger Hash Function
    inline uint32_t rx_hash_pjw(const void* data,uint32_t Len, uint32_t seed = 0)
    {
        const uint32_t BitsInUnignedInt = (uint32_t)(sizeof(uint32_t) * 8);
        const uint32_t ThreeQuarters = (uint32_t)((BitsInUnignedInt * 3) / 4);
        const uint32_t OneEighth = (uint32_t)(BitsInUnignedInt / 8);
        const uint32_t HighBits = (uint32_t)(0xFFFFFFFF) << (BitsInUnignedInt - OneEighth);
        uint32_t hash = seed ;
        uint32_t test = 0 ;
        for(uint32_t i=0;i<Len;i++)
        {
            hash = (hash << OneEighth) + ((uint8_t*)data)[i];
            if ((test = hash & HighBits) != 0 )
                hash = ((hash ^ (test >> ThreeQuarters)) & ( ~ HighBits));
        }
        return hash;
    }
    template<class CT>
    inline uint32_t rx_hash_pjw(const CT* str, uint32_t seed = 0)
    {
        const uint32_t BitsInUnignedInt = (uint32_t )( sizeof (uint32_t)*8 );
        const uint32_t ThreeQuarters = (uint32_t )((BitsInUnignedInt*3 ) / 4 );
        const uint32_t OneEighth = (uint32_t )(BitsInUnignedInt / 8 );
        const uint32_t HighBits = (uint32_t )( 0xFFFFFFFF ) << (BitsInUnignedInt - OneEighth);
        uint32_t hash = seed ;
        uint32_t test;
        while (*str)
        {
            hash = (hash << OneEighth) + (*str ++ );
            if ((test = hash & HighBits) != 0 )
                hash = ((hash ^ (test >> ThreeQuarters)) & ( ~ HighBits));
        }
        return hash;
    }

    //-----------------------------------------------------
    // ELF Hash Function
    inline uint32_t rx_hash_elf(const void* data,uint32_t Len, uint32_t seed = 0)
    {
        uint32_t hash = seed ;
        uint32_t x = 0 ;
        for(uint32_t i=0;i<Len;i++)
        {
            hash = (hash << 4 ) + ((uint8_t*)data)[i];
            if ((x = hash & 0xF0000000L ) != 0 )
            {
                hash ^= (x >> 24 );
                hash &= ~ x;
            }
        }
        return hash;
    }
    template<class CT>
    inline uint32_t rx_hash_elf(const CT* str,uint32_t seed=0)
    {
        uint32_t hash = seed ;
        uint32_t x;
        while (*str)
        {
            hash = (hash << 4 ) + (*str ++ );
            if ((x = hash & 0xF0000000L ) != 0 )
            {
                hash ^= (x >> 24 );
                hash &= ~ x;
            }
        }
        return hash;
    }

    //-----------------------------------------------------
    // BKDR Hash Function
    inline uint32_t rx_hash_bkdr(const void* data,uint32_t Len, uint32_t seed = 131)
    {
        uint32_t hash = seed;// 31 131 1313 13131 131313 etc..
        for(uint32_t i=0;i<Len;i++)
            hash = hash*seed + ((uint8_t*)data)[i];
        return hash;
    }
    template<class CT>
    inline uint32_t rx_hash_bkdr(const CT* str, uint32_t seed = 131)
    {
        uint32_t hash = seed ;// 31 131 1313 13131 131313 etc..
        while (*str)
            hash = hash*seed + (*str ++ );
        return hash;
    }

    //-----------------------------------------------------
    // SDBM Hash Function
    inline uint32_t rx_hash_sdbm(const void* data,uint32_t Len, uint32_t seed = 0)
    {
        uint32_t hash = seed;
        for(uint32_t i=0;i<Len;i++)
            hash = ((uint8_t*)data)[i] + (hash << 6 ) + (hash << 16 ) - hash;
        return hash;
    }
    template<class CT>
    inline uint32_t rx_hash_sdbm(const CT* str, uint32_t seed = 0)
    {
        uint32_t hash = seed;
        while (*str)
            hash = (*str ++ ) + (hash << 6 ) + (hash << 16 ) - hash;
        return hash;
    }

    //-----------------------------------------------------
    // DJB Hash Function
    inline uint32_t rx_hash_djb(const void* data,uint32_t Len, uint32_t seed = 5381)
    {
        uint32_t hash = seed ;
        for(uint32_t i=0;i<Len;i++)
            hash += (hash << 5 ) + ((uint8_t*)data)[i];
        return hash;
    }
    template<class CT>
    inline uint32_t rx_hash_djb(const CT* str, uint32_t seed = 5381)
    {
        uint32_t hash = seed ;
        while (*str)
            hash += (hash << 5 ) + (*str ++ );
        return hash;
    }

    //-----------------------------------------------------
    // AP Hash Function
    inline uint32_t rx_hash_ap(const void* data,uint32_t Len, uint32_t seed = 0)
    {
        uint32_t hash = seed ;
        for(uint32_t i=0;i<Len;i++)
        {
            if ((i & 1 ) == 0 )
                hash ^= ((hash << 7 ) ^ ((uint8_t*)data)[i] ^ (hash >> 3 ));
            else
                hash ^= ( ~ ((hash << 11 ) ^ ((uint8_t*)data)[i] ^ (hash >> 5 )));
        }
        return hash;
    }
    template<class CT>
    inline uint32_t rx_hash_ap(const CT* str, uint32_t seed = 0)
    {
        uint32_t hash = seed ;
        for (uint32_t i = 0 ;*str; i ++ )
        {
            if ((i & 1 ) == 0 )
                hash ^= ((hash << 7 ) ^ (*str ++ ) ^ (hash >> 3 ));
            else
                hash ^= ( ~ ((hash << 11 ) ^ (*str ++ ) ^ (hash >> 5 )));
        }
        return hash;
    }
    //-----------------------------------------------------
    //DEK Hash
    inline uint32_t rx_hash_dek(const void* data,uint32_t Len,uint32_t seed=0)
    {
        uint32_t hash = Len+seed;
        for(uint32_t i = 0; i < Len; i++)
            hash = ((hash << 5) ^ (hash >> 27)) ^ ((uint8_t*)data)[i];
        return hash;
    }
    template<class CT>
    inline uint32_t rx_hash_dek(const CT* Str)
    {
        uint32_t Len=strlen(Str);
        uint32_t hash = Len;
        for(uint32_t i = 0; i < Len; i++)
            hash = ((hash << 5) ^ (hash >> 27)) ^ Str[i];
        return hash;
    }
    //-----------------------------------------------------
    //BP Hash*
    inline uint32_t rx_hash_bp(const void* data,uint32_t Len, uint32_t seed = 0)
    {
        uint32_t hash=seed;
        for(uint32_t i = 0;i < Len; i++)
            hash = (hash << 7) ^ ((uint8_t*)data)[i];
        return hash;
    }
    template<class CT>
    inline uint32_t rx_hash_bp(const CT* Str, uint32_t seed = 0)
    {
        uint32_t hash=seed;
        while(*Str)
            hash = (hash << 7) ^ (*Str++);
        return hash;
    }
    //-----------------------------------------------------
    //FNV Hash
    inline uint32_t rx_hash_fnv(const void* data,uint32_t Len, uint32_t seed = 0)
    {
        uint32_t fnv_prime = 0x811C9DC5;
        uint32_t hash = seed;
        for(uint32_t i = 0; i < Len; i++)
        {
            hash *= fnv_prime;
            hash ^= ((uint8_t*)data)[i];
        }
        return hash;
    }
    template<class CT>
    inline uint32_t rx_hash_fnv(const CT* Str, uint32_t seed = 0)
    {
        uint32_t fnv_prime = 0x811C9DC5;
        uint32_t hash = seed;
        while(*Str)
        {
            hash *= fnv_prime;
            hash ^= (*Str++);
        }
        return hash;
    }

    //-----------------------------------------------------
    //MurmurHash
    inline uint32_t rx_hash_murmur(const void* data, uint32_t len,const uint32_t seed = 97)
    {
        const uint32_t m = 0x5bd1e995;
        uint32_t h = seed ^ len;

        while(len >= 4)
        {
            uint32_t k = *(uint32_t *)data;
            k *= m;
            k ^= k >> 24;
            k *= m;
            h *= m;
            h ^= k;
            data = (uint8_t*)data+4;
            len -= 4;
        }
        // Handle the last few bytes of the input array
        switch(len)
        {
            case 3: h ^= ((uint8_t*)data)[2] << 16;
            case 2: h ^= ((uint8_t*)data)[1] << 8;
            case 1: h ^= ((uint8_t*)data)[0];
            h *= m;
        };
        // Do a few final mixes of the hash to ensure the last few
        // bytes are well-incorporated.
        h ^= h >> 13;
        h *= m;
        h ^= h >> 15;
        return h;
    }

    //-----------------------------------------------------
    //https://github.com/skeeto/hash-prospector
    inline uint32_t rx_hash_mosquito(const void* data, uint32_t len, uint32_t seed=0)
    {
        uint32_t hash = seed;
        for (uint32_t i = 0; i < len; i++) {
            hash += ((uint8_t*)data)[i];
            hash ^= hash >> 16;
            hash *= uint32_t(0xb03a22b3);
            hash ^= hash >> 10;
        }
        return hash;
    }
    template<class CT>
    inline uint32_t rx_hash_mosquito(const CT *buf, uint32_t seed=0)
    {
        uint32_t hash = seed;
        for (;*buf; ++buf) {
            hash += *buf;
            hash ^= hash >> 16;
            hash *= uint32_t(0xb03a22b3);
            hash ^= hash >> 10;
        }
        return hash;
    }

    //-----------------------------------------------------
    //https://github.com/rurban/smhasher/blob/master/fasthash.cpp
    //fasthash
    uint64_t rx_hash_fast64(const void *buf, uint32_t len, uint64_t seed=0)
    {
        typedef struct util{
            static inline uint64_t mix(uint64_t h) {
                h ^= h >> 23;
                h *= 0x2127599bf4325c37ULL;
                h ^= h >> 47;
                return h;
            }
        }util;

        const uint64_t    m = 0x880355f21e6d1965ULL;
        const uint64_t *pos = (const uint64_t *)buf;
        const uint64_t *end = pos + (len / 8);
        uint64_t h = seed ^ (len * m);
        uint64_t v;

        while (pos != end) {
            v = *pos++;
            h ^= util::mix(v);
            h *= m;
        }

        const uint8_t *pos2 = (const uint8_t*)pos;
        v = 0;

        switch (len & 7) {
        case 7: v ^= (uint64_t)pos2[6] << 48;
        case 6: v ^= (uint64_t)pos2[5] << 40;
        case 5: v ^= (uint64_t)pos2[4] << 32;
        case 4: v ^= (uint64_t)pos2[3] << 24;
        case 3: v ^= (uint64_t)pos2[2] << 16;
        case 2: v ^= (uint64_t)pos2[1] << 8;
        case 1: v ^= (uint64_t)pos2[0];
            h ^= util::mix(v);
            h *= m;
        }

        return util::mix(h);
    }

    uint32_t rx_hash_fast32(const void *buf, uint32_t len, uint32_t seed=0)
    {
        // the following trick converts the 64-bit hashcode to Fermat
        // residue, which shall retain information from both the higher
        // and lower parts of hashcode.
        uint64_t h = rx_hash_fast64(buf, len, seed);
        return uint32_t(h - (h >> 32));
    }

    //-----------------------------------------------------
    //可用的数据哈希函数类型
    typedef enum rx_data_hash32_type
    {
        DHT_RS=0,
        DHT_JS,
        DHT_PJW,
        DHT_ELF,
        DHT_BKDR,
        DHT_SDBM,
        DHT_DJB,
        DHT_AP,
        DHT_DEK,
        DHT_BP,
        DHT_FNV,
        DHT_MURMUR,
        DHT_MOSQUITO,
        DHT_FAST,
        DHT_Count,                                       //当作类型的数量
    }rx_data_hash32_type;

    //-----------------------------------------------------
    //根据哈希函数类型获取其对应的算法名称
    inline const char* rx_data_hash32_name(rx_data_hash32_type Type)
    {
        switch(Type)
        {
            case DHT_RS:     return "DataHash::RS";
            case DHT_JS:     return "DataHash::JS";
            case DHT_PJW:    return "DataHash::PJW";
            case DHT_ELF:    return "DataHash::ELF";
            case DHT_BKDR:   return "DataHash::BKDR";
            case DHT_SDBM:   return "DataHash::SDBM";
            case DHT_DJB:    return "DataHash::DJB";
            case DHT_AP:     return "DataHash::AP";
            case DHT_DEK:    return "DataHash::DEK";
            case DHT_BP:     return "DataHash::BP";
            case DHT_FNV:    return "DataHash::FNV";
            case DHT_MURMUR: return "DataHash::murmur";
            case DHT_MOSQUITO:return "DataHash::mosquito";
            case DHT_FAST:   return "DataHash::fasthash";

            default:         return "Hash::Unknown";
        }
    }

    //-----------------------------------------------------
    //已经实现的数据哈希算法
    static const rx_data_hash32_t rx_data_hash32_funcs[]={
        rx_hash_rs,
        rx_hash_js,
        rx_hash_pjw,
        rx_hash_elf,
        rx_hash_bkdr,
        rx_hash_sdbm,
        rx_hash_djb,
        rx_hash_ap,
        rx_hash_dek,
        rx_hash_bp,
        rx_hash_fnv,
        rx_hash_murmur,
        rx_hash_mosquito,
        rx_hash_fast32
    };
    const uint32_t rx_data_hash32_funcs_count=DHT_Count;
    //根据哈希函数类型获取对应的哈希函数
    inline rx_data_hash32_t rx_data_hash32(const rx_data_hash32_type type=rx_data_hash32_type(rx_data_hash32_funcs_count-1))
    {
        rx_static_assert(sizeof(rx_data_hash32_funcs)/sizeof(rx_data_hash32_funcs[0])==rx_data_hash32_funcs_count);
        return rx_data_hash32_funcs[type>=rx_data_hash32_funcs_count?rx_data_hash32_funcs_count-1:type];
    }
    //根据哈希函数类型计算给定数据的哈希码
    inline uint32_t rx_data_hash32(const void* data,uint32_t len,const rx_data_hash32_type Type=rx_data_hash32_type(rx_data_hash32_funcs_count-1),uint32_t seed=0)
    {
        return rx_data_hash32(Type)(data,len,seed);
    }

    //-----------------------------------------------------
    //利用不同的整数哈希函数类型,构造一致性数据哈希函数族(逐4字节整数遍历累计)
    //-----------------------------------------------------
    template<rx_int_hash32_t hf>
    inline uint32_t rx_data_hash32(const void* data, uint32_t len, uint32_t seed=1)
    {
        uint32_t hash = seed;
        uint32_t lc=len>>2;
        for (uint32_t i = 0; i < lc; i++)
            hash ^= hf(((uint32_t*)data)[i]);

        uint32_t v=0;
        uint8_t *pos2=(uint8_t*)((uint32_t*)data+lc);
        switch (len & 3)
        {
            case 3: v ^= (uint32_t)pos2[2] << 16;
            case 2: v ^= (uint32_t)pos2[1] << 8;
            case 1: v ^= (uint32_t)pos2[0];
                hash ^= hf(v);
        }
        return hf(hash);
    }

    //-----------------------------------------------------
    //利用不同的整数哈希函数类型,构造数据哈希函数族,可通过序号获取
    static const rx_data_hash32_t rx_data_hash32s_funcs[]={
        rx_data_hash32<rx_hash_tomas        >,
        rx_data_hash32<rx_hash_bobj         >,
        rx_data_hash32<rx_hash_murmur3      >,
        rx_data_hash32<rx_hash_mosquito     >,
        rx_data_hash32<rx_hash_skeeto_a     >,
        rx_data_hash32<rx_hash_skeeto_b     >,
        rx_data_hash32<rx_hash_skeeto_c     >,
        rx_data_hash32<rx_hash_skeeto_d     >,
        rx_data_hash32<rx_hash_skeeto_e     >,
        rx_data_hash32<rx_hash_skeeto_e_r   >,
        rx_data_hash32<rx_hash_skeeto_f     >,
        rx_data_hash32<rx_hash_skeeto_g     >,
        rx_data_hash32<rx_hash_skeeto_triple_r>,
        rx_data_hash32<rx_hash_skeeto_triple>,
    };
    const uint32_t rx_data_hash32s_count=IHT_Count;
    inline rx_data_hash32_t rx_data_hash32s(const uint32_t idx=rx_data_hash32s_count-1)
    {
        rx_static_assert(rx_data_hash32s_count==sizeof(rx_data_hash32s_funcs)/sizeof(rx_data_hash32s_funcs[0]));
        return rx_data_hash32s_funcs[idx>=rx_data_hash32s_count?rx_data_hash32s_count-1:idx];
    }

#endif
