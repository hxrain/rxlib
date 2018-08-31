#ifndef _RX_HASH_DATA_H_
#define _RX_HASH_DATA_H_

#include "rx_cc_macro.h"

    //-----------------------------------------------------
    //data hash function
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
        {
            hash = (*str ++ ) + (hash << 6 ) + (hash << 16 ) - hash;
        }
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
    inline uint32_t rx_hash_dek(const void* data,uint32_t Len)
    {
        uint32_t hash = Len;
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
    static inline uint64_t rx_hash_fast_mix(uint64_t h) {
        h ^= h >> 23;
        h *= 0x2127599bf4325c37ULL;
        h ^= h >> 47;
        return h;
    }

    uint64_t rx_hash_fast64(const void *buf, size_t len, uint64_t seed=0)
    {
        const uint64_t    m = 0x880355f21e6d1965ULL;
        const uint64_t *pos = (const uint64_t *)buf;
        const uint64_t *end = pos + (len / 8);
        const unsigned char *pos2;
        uint64_t h = seed ^ (len * m);
        uint64_t v;

        while (pos != end) {
            v = *pos++;
            h ^= rx_hash_fast_mix(v);
            h *= m;
        }

        pos2 = (const unsigned char*)pos;
        v = 0;

        switch (len & 7) {
        case 7: v ^= (uint64_t)pos2[6] << 48;
        case 6: v ^= (uint64_t)pos2[5] << 40;
        case 5: v ^= (uint64_t)pos2[4] << 32;
        case 4: v ^= (uint64_t)pos2[3] << 24;
        case 3: v ^= (uint64_t)pos2[2] << 16;
        case 2: v ^= (uint64_t)pos2[1] << 8;
        case 1: v ^= (uint64_t)pos2[0];
            h ^= rx_hash_fast_mix(v);
            h *= m;
        }

        return rx_hash_fast_mix(h);
    }

    uint32_t rx_hash_fast(const void *buf, size_t len, uint32_t seed=0)
    {
        // the following trick converts the 64-bit hashcode to Fermat
        // residue, which shall retain information from both the higher
        // and lower parts of hashcode.
        uint64_t h = rx_hash_fast64(buf, len, seed);
        return uint32_t(h - (h >> 32));
    }

    //-----------------------------------------------------
    //可用的数据哈希函数类型
    typedef enum rx_data_hash_type
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
    }rx_data_hash_type;

    //-----------------------------------------------------
    //根据哈希函数类型获取其对应的算法名称
    inline const char* rx_data_hash_name(rx_data_hash_type Type)
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
    //根据哈希函数类型计算给定数据的哈希码
    inline uint32_t rx_data_hash(rx_data_hash_type Type,const void* data,uint32_t Len)
    {
        switch(Type)
        {
            case DHT_RS:     return rx_hash_rs(data,Len);
            case DHT_JS:     return rx_hash_js(data,Len);
            case DHT_PJW:    return rx_hash_pjw(data,Len);
            case DHT_ELF:    return rx_hash_elf(data,Len);
            case DHT_BKDR:   return rx_hash_bkdr(data,Len);
            case DHT_SDBM:   return rx_hash_sdbm(data,Len);
            case DHT_DJB:    return rx_hash_djb(data,Len);
            case DHT_AP:     return rx_hash_ap(data,Len);
            case DHT_DEK:    return rx_hash_dek(data,Len);
            case DHT_BP:     return rx_hash_bp(data,Len);
            case DHT_FNV:    return rx_hash_fnv(data,Len);
            case DHT_MURMUR: return rx_hash_murmur(data,Len);
            case DHT_MOSQUITO:return rx_hash_mosquito(data, Len);
            case DHT_FAST:   return rx_hash_fast(data, Len);

            default:         return rx_hash_mosquito(data,Len);
        }
    }

#endif
