#ifndef _RX_HASH_H_
#define _RX_HASH_H_

#include "rx_cc_macro.h"

    //-----------------------------------------------------
    //data hash function
    //-----------------------------------------------------
    // RS Hash Function
    inline uint32_t rx_hash_rs(const uint8_t* Data,uint32_t Len)
    {
        uint32_t b = 378551 ;
        uint32_t a = 63689 ;
        uint32_t hash = 0 ;
        for(uint32_t i=0;i<Len;i++)
        {
            hash = hash * a + Data[i];
            a *= b;
        }
        return hash;
    }
    template<class CT>
    inline uint32_t rx_hash_rs(const CT* str)
    {
        uint32_t b = 378551 ;
        uint32_t a = 63689 ;
        uint32_t hash = 0 ;
        while (*str)
        {
            hash = hash * a + (*str ++ );
            a *= b;
        }
        return hash;
    }

    //-----------------------------------------------------
    // JS Hash Function
    inline uint32_t rx_hash_js(const uint8_t* Data,uint32_t Len)
    {
        uint32_t hash = 1315423911 ;
        for(uint32_t i=0;i<Len;i++)
            hash ^= ((hash << 5 ) + Data[i] + (hash >> 2 ));
        return hash;
    }
    template<class CT>
    inline uint32_t rx_hash_js(const CT* str)
    {
        uint32_t hash = 1315423911 ;
        while (*str)
            hash ^= ((hash << 5 ) + (*str ++ ) + (hash >> 2 ));
        return hash;
    }

    //-----------------------------------------------------
    // P. J. Weinberger Hash Function
    inline uint32_t rx_hash_pjw(const uint8_t* Data,uint32_t Len)
    {
        uint32_t BitsInUnignedInt = (uint32_t )( sizeof (uint32_t)*8 );
        uint32_t ThreeQuarters = (uint32_t )((BitsInUnignedInt*3 ) / 4 );
        uint32_t OneEighth = (uint32_t )(BitsInUnignedInt / 8 );
        uint32_t HighBits = (uint32_t )( 0xFFFFFFFF ) << (BitsInUnignedInt - OneEighth);
        uint32_t hash = 0 ;
        uint32_t test = 0 ;
        for(uint32_t i=0;i<Len;i++)
        {
            hash = (hash << OneEighth) + Data[i];
            if ((test = hash & HighBits) != 0 )
                hash = ((hash ^ (test >> ThreeQuarters)) & ( ~ HighBits));
        }
        return hash;
    }
    template<class CT>
    inline uint32_t rx_hash_pjw(const CT* str)
    {
        uint32_t BitsInUnignedInt = (uint32_t )( sizeof (uint32_t)*8 );
        uint32_t ThreeQuarters = (uint32_t )((BitsInUnignedInt*3 ) / 4 );
        uint32_t OneEighth = (uint32_t )(BitsInUnignedInt / 8 );
        uint32_t HighBits = (uint32_t )( 0xFFFFFFFF ) << (BitsInUnignedInt - OneEighth);
        uint32_t hash = 0 ;
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
    inline uint32_t rx_hash_elf(const uint8_t* Data,uint32_t Len)
    {
        uint32_t hash = 0 ;
        uint32_t x = 0 ;
        for(uint32_t i=0;i<Len;i++)
        {
            hash = (hash << 4 ) + Data[i];
            if ((x = hash & 0xF0000000L ) != 0 )
            {
                hash ^= (x >> 24 );
                hash &= ~ x;
            }
        }
        return hash;
    }
    template<class CT>
    inline uint32_t rx_hash_elf(const CT* str)
    {
        uint32_t hash = 0 ;
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
    inline uint32_t rx_hash_bkdr(const uint8_t* Data,uint32_t Len)
    {
        uint32_t seed = 131 ; // 31 131 1313 13131 131313 etc..
        uint32_t hash = 0 ;
        for(uint32_t i=0;i<Len;i++)
            hash = hash*seed + Data[i];
        return hash;
    }
    template<class CT>
    inline uint32_t rx_hash_bkdr(const CT* str)
    {
        uint32_t seed = 131 ; // 31 131 1313 13131 131313 etc..
        uint32_t hash = 0 ;
        while (*str)
            hash = hash*seed + (*str ++ );
        return hash;
    }

    //-----------------------------------------------------
    // SDBM Hash Function
    inline uint32_t rx_hash_sdbm(const uint8_t* Data,uint32_t Len)
    {
        uint32_t hash = 0 ;
        for(uint32_t i=0;i<Len;i++)
            hash = Data[i] + (hash << 6 ) + (hash << 16 ) - hash;
        return hash;
    }
    template<class CT>
    inline uint32_t rx_hash_sdbm(const CT* str)
    {
        uint32_t hash = 0 ;
        while (*str)
        {
            hash = (*str ++ ) + (hash << 6 ) + (hash << 16 ) - hash;
        }
        return hash;
    }

    //-----------------------------------------------------
    // DJB Hash Function
    inline uint32_t rx_hash_djb(const uint8_t* Data,uint32_t Len)
    {
        uint32_t hash = 5381 ;
        for(uint32_t i=0;i<Len;i++)
            hash += (hash << 5 ) + Data[i];
        return hash;
    }
    template<class CT>
    inline uint32_t rx_hash_djb(const CT* str)
    {
        uint32_t hash = 5381 ;
        while (*str)
            hash += (hash << 5 ) + (*str ++ );
        return hash;
    }

    //-----------------------------------------------------
    // AP Hash Function
    inline uint32_t rx_hash_ap(const uint8_t* Data,uint32_t Len)
    {
        uint32_t hash = 0 ;
        for(uint32_t i=0;i<Len;i++)
        {
            if ((i & 1 ) == 0 )
                hash ^= ((hash << 7 ) ^ Data[i] ^ (hash >> 3 ));
            else
                hash ^= ( ~ ((hash << 11 ) ^ Data[i] ^ (hash >> 5 )));
        }
        return hash;
    }
    template<class CT>
    inline uint32_t rx_hash_ap(const CT* str)
    {
        uint32_t hash = 0 ;
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
    inline uint32_t rx_hash_dek(const uint8_t* Data,uint32_t Len)
    {
        uint32_t hash = Len;
        for(uint32_t i = 0; i < Len; i++)
            hash = ((hash << 5) ^ (hash >> 27)) ^ Data[i];
        return hash;
    }
    template<class CT>
    inline uint32_t rx_hash_dek(const CT* Str)
    {
        uint32_t Len=HSU::strlen(Str);
        uint32_t hash = Len;
        for(uint32_t i = 0; i < Len; i++)
            hash = ((hash << 5) ^ (hash >> 27)) ^ Str[i];
        return hash;
    }
    //-----------------------------------------------------
    //BP Hash*
    inline uint32_t rx_hash_bp(const uint8_t* Data,uint32_t Len)
    {
        uint32_t hash=0;
        for(uint32_t i = 0;i < Len; i++)
            hash = (hash << 7) ^ Data[i];
        return hash;
    }
    template<class CT>
    inline uint32_t rx_hash_bp(const CT* Str)
    {
        uint32_t hash=0;
        while(*Str)
            hash = (hash << 7) ^ (*Str++);
        return hash;
    }
    //-----------------------------------------------------
    //FNV Hash
    inline uint32_t rx_hash_fnv(const uint8_t* Data,uint32_t Len)
    {
        uint32_t fnv_prime = 0x811C9DC5;
        uint32_t hash = 0;
        for(uint32_t i = 0; i < Len; i++)
        {
            hash *= fnv_prime;
            hash ^= Data[i];
        }
        return hash;
    }
    template<class CT>
    inline uint32_t rx_hash_fnv(const CT* Str)
    {
        uint32_t fnv_prime = 0x811C9DC5;
        uint32_t hash = 0;
        while(*Str)
        {
            hash *= fnv_prime;
            hash ^= (*Str++);
        }
        return hash;
    }

    //-----------------------------------------------------
    //MurmurHash
    inline uint32_t rx_hash_murmur(const uint8_t* data, uint32_t len,const uint32_t seed = 97)
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
            data += 4;
            len -= 4;
        }
        // Handle the last few bytes of the input array
        switch(len)
        {
            case 3: h ^= data[2] << 16;
            case 2: h ^= data[1] << 8;
            case 1: h ^= data[0];
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
        DHT_Count,                                       //当作类型的数量
    }rx_data_hash_type;

    //-----------------------------------------------------
    //根据哈希函数类型获取其对应的算法名称
    inline const char* rx_data_hash_name(rx_data_hash_type Type)
    {
        switch(Type)
        {
            case DHT_RS:     return "Hash::RS";
            case DHT_JS:     return "Hash::JS";
            case DHT_PJW:    return "Hash::PJW";
            case DHT_ELF:    return "Hash::ELF";
            case DHT_BKDR:   return "Hash::BKDR";
            case DHT_SDBM:   return "Hash::SDBM";
            case DHT_DJB:    return "Hash::DJB";
            case DHT_AP:     return "Hash::AP";
            case DHT_DEK:    return "Hash::DEK";
            case DHT_BP:     return "Hash::BP";
            case DHT_FNV:    return "Hash::FNV";
            case DHT_MURMUR: return "Hash::MURMUR";
        }
        return "Hash::Unknown";
    }

    //-----------------------------------------------------
    //根据哈希函数类型计算给定数据的哈希码
    inline uint32_t rx_data_hash(rx_data_hash_type Type,const uint8_t* Data,uint32_t Len)
    {
        switch(Type)
        {
            case DHT_RS:     return rx_hash_rs(Data,Len);
            case DHT_JS:     return rx_hash_js(Data,Len);
            case DHT_PJW:    return rx_hash_pjw(Data,Len);
            case DHT_ELF:    return rx_hash_elf(Data,Len);
            case DHT_BKDR:   return rx_hash_bkdr(Data,Len);
            case DHT_SDBM:   return rx_hash_sdbm(Data,Len);
            case DHT_DJB:    return rx_hash_djb(Data,Len);
            case DHT_AP:     return rx_hash_ap(Data,Len);
            case DHT_DEK:    return rx_hash_dek(Data,Len);
            case DHT_BP:     return rx_hash_bp(Data,Len);
            case DHT_FNV:    return rx_hash_fnv(Data,Len);
            case DHT_MURMUR: return rx_hash_murmur(Data,Len);
        }
        return rx_hash_rs(Data,Len);
    }


    //-----------------------------------------------------
    //integer hash function
    //-----------------------------------------------------
    // Tomas Wang
    template<class DT=uint32_t>
    inline DT hash_tomas(DT key)
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
    template<class DT=uint64_t>
    inline DT hash_tomas(DT key)
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
    template<class DT=uint32_t>
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
    template<class DT=uint32_t>
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
    template<class DT=uint32_t>
    inline DT hash_gold_a(DT addr)
    {
        return addr * 21911;
    }
    //黄金分隔哈希算法,对于内存地址效果较好(四字节对齐)
    template<class DT=uint32_t>
    inline DT hash_gold_a2(DT addr)
    {
        return (addr>>2) * 21911;
    }
    //黄金分隔哈希算法,对于内存地址效果较好(八字节对齐)
    template<class DT=uint32_t>
    inline DT hash_gold_a3(DT addr)
    {
        return (addr>>3) * 21911;
    }
    //-----------------------------------------------------
    //黄金分隔哈希算法,对于自然数效果较好
    template<class DT=uint32_t>
    inline DT hash_gold_b(DT addr)
    {
        return addr * 1403641;
    }
    //黄金分隔哈希算法,对于内存地址效果较好(四字节对齐)
    template<class DT=uint32_t>
    inline DT hash_gold_b2(DT addr)
    {
        return (addr>>2) * 1403641;
    }
    //黄金分隔哈希算法,对于内存地址效果较好(八字节对齐)
    template<class DT=uint32_t>
    inline DT hash_gold_b3(DT addr)
    {
        return (addr>>3) * 1403641;
    }
    //-----------------------------------------------------
    //黄金分隔哈希算法,对于自然数效果较好
    template<class DT=uint32_t>
    inline DT hash_gold_c(DT addr)
    {
        return addr * 11229331;
    }
    //黄金分隔哈希算法,对于内存地址效果较好(四字节对齐)
    template<class DT=uint32_t>
    inline DT hash_gold_c2(DT addr)
    {
        return (addr>>2) * 11229331;
    }
    //黄金分隔哈希算法,对于内存地址效果较好(八字节对齐)
    template<class DT=uint32_t>
    inline DT hash_gold_c3(DT addr)
    {
        return (addr>>3) * 11229331;
    }




#endif