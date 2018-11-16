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
    for(uint32_t i=0; i<Len; i++)
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
    for(uint32_t i=0; i<Len; i++)
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
    for(uint32_t i=0; i<Len; i++)
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
    for(uint32_t i=0; i<Len; i++)
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
    for(uint32_t i=0; i<Len; i++)
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
    for(uint32_t i=0; i<Len; i++)
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
    for(uint32_t i=0; i<Len; i++)
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
    for(uint32_t i=0; i<Len; i++)
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
    for (uint32_t i = 0 ; *str; i ++ )
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
    for(uint32_t i = 0; i < Len; i++)
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
        case 3:
            h ^= ((uint8_t*)data)[2] << 16;
        case 2:
            h ^= ((uint8_t*)data)[1] << 8;
        case 1:
            h ^= ((uint8_t*)data)[0];
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
    for (uint32_t i = 0; i < len; i++)
    {
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
    for (; *buf; ++buf)
    {
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
inline uint64_t rx_hash_fast64(const void *buf, uint32_t len, uint64_t seed=0)
{
    typedef struct util
    {
        static inline uint64_t mix(uint64_t h)
        {
            h ^= h >> 23;
            h *= 0x2127599bf4325c37ULL;
            h ^= h >> 47;
            return h;
        }
    } util;

    const uint64_t    m = 0x880355f21e6d1965ULL;
    const uint64_t *pos = (const uint64_t *)buf;
    const uint64_t *end = pos + (len / 8);
    uint64_t h = seed ^ (len * m);
    uint64_t v;

    while (pos != end)
    {
        v = *pos++;
        h ^= util::mix(v);
        h *= m;
    }

    const uint8_t *pos2 = (const uint8_t*)pos;
    v = 0;

    switch (len & 7)
    {
        case 7:
            v ^= (uint64_t)pos2[6] << 48;
        case 6:
            v ^= (uint64_t)pos2[5] << 40;
        case 5:
            v ^= (uint64_t)pos2[4] << 32;
        case 4:
            v ^= (uint64_t)pos2[3] << 24;
        case 3:
            v ^= (uint64_t)pos2[2] << 16;
        case 2:
            v ^= (uint64_t)pos2[1] << 8;
        case 1:
            v ^= (uint64_t)pos2[0];
            h ^= util::mix(v);
            h *= m;
    }

    return util::mix(h);
}

inline uint32_t rx_hash_fast32(const void *buf, uint32_t len, uint32_t seed=0)
{
    // the following trick converts the 64-bit hashcode to Fermat
    // residue, which shall retain information from both the higher
    // and lower parts of hashcode.
    uint64_t h = rx_hash_fast64(buf, len, seed);
    return uint32_t(h - (h >> 32));
}

//-----------------------------------------------------
//Zobrist Algorithm 转换为查表法后的哈希算法,对于字符串效果极好
//https://www.codeproject.com/Articles/1225252/A-new-Hash-Function-ZobHash
static const uint32_t zob_map_table[] = {
 0x5D60D139, 0x464D952B, 0x59D0ABFD, 0x7552995C, 0x4403A21F, 0x5F2D890C, 0x2E52372B, 0x292614BF, 0x650A2FAC, 0x2AE32EC8, 0x10B46261, 0x3173E4B7, 0x157675BA, 0x29BC961C, 0x5D2E9DB6, 0x368F7F24,
 0x398340A8, 0x57F22CAE, 0x74F547CE, 0x3CDD3EB4, 0x2235E2BF, 0x61F6D2C1, 0x1B2921BF, 0x3C981448, 0x177EFA9B, 0x14029CA8, 0x2F5E4B1A, 0x56846712, 0x66C34128, 0x19B60BB1, 0x43EC9A97, 0x463915C7,
 0x7C12A8B4, 0x2D3441C0, 0x0ECF4E49, 0x28B6A932, 0x34954E39, 0x095AF2BD, 0x40DEDFD9, 0x38DAA522, 0x5C98E835, 0x31BF45F7, 0x20ED4335, 0x447ACDED, 0x78A5958D, 0x7A3E0D2F, 0x0E384DFE, 0x5D8E6F28,
 0x2EC9FBFF, 0x49510F4B, 0x0D6B1EED, 0x23C20008, 0x36896ABC, 0x00A0225E, 0x14C77C0B, 0x7019FC4C, 0x42EFCAA4, 0x4FEF8081, 0x5DD39EC1, 0x30010577, 0x2FCF3DF2, 0x57CDD018, 0x4262D396, 0x4B5423FB,
 0x66F69430, 0x4A7B4C99, 0x35613C02, 0x684233F9, 0x1AED47D3, 0x3477F484, 0x01FA30EB, 0x30284DEB, 0x17134CD5, 0x3C1AA2AC, 0x6044567E, 0x70769CC7, 0x41098F8C, 0x56AE53D1, 0x43F27EBA, 0x1D40ED6B,
 0x05CA4EAA, 0x51CFDBF1, 0x594D720E, 0x21AF5318, 0x502E19D8, 0x137D2F1C, 0x6A5F0B60, 0x659414A3, 0x2133A705, 0x13654FD1, 0x7D9235C5, 0x175CB684, 0x2B8753FB, 0x10DDDA62, 0x090B6730, 0x04CB181D,
 0x6F5C7260, 0x55991F39, 0x5D8439BC, 0x2E2A48F4, 0x44DCD12D, 0x25F61A04, 0x42A12755, 0x48BF007F, 0x4319BD25, 0x195FA3ED, 0x195C1EA6, 0x1FBF67BA, 0x363A3D92, 0x1B89FDF3, 0x3A606EEB, 0x54761F9A,
 0x594618FA, 0x4092B156, 0x2A36B6CD, 0x5DFF6200, 0x7E805E09, 0x20B3807E, 0x7B260A22, 0x53796514, 0x601C4138, 0x4FCD275E, 0x470E8CF4, 0x0787F802, 0x36E5BEBE, 0x6A9D7A66, 0x04A0F9F0, 0x06357273,
 0x330F3B7C, 0x5B793E61, 0x011A2A67, 0x6B707052, 0x792A1A14, 0x15C835C6, 0x58641C3D, 0x5FD434A5, 0x0F2EB49C, 0x108E718F, 0x5E9595F2, 0x36CE75EB, 0x7A211075, 0x4A9FA3A6, 0x2F59D711, 0x05A9A912,
 0x5904E0E5, 0x291C162B, 0x3E169D89, 0x6AF4A2A4, 0x66A72394, 0x2B0C052F, 0x064ABA13, 0x4EA8F1E2, 0x5A731516, 0x0A0AD4A8, 0x4E0E07BB, 0x750FA9CE, 0x5EE78D0F, 0x3B192F53, 0x11D941C1, 0x587C42BE,
 0x14BEA9FD, 0x1326AC33, 0x6CB02C3D, 0x5AC0FF30, 0x1A6FD38C, 0x4EEFFE98, 0x5B4C0585, 0x437DE334, 0x682E9518, 0x4A628227, 0x4ED0AD64, 0x6DF1EC7A, 0x421DEA8B, 0x44579437, 0x5958549E, 0x157C9D92,
 0x2073504D, 0x4164E3E2, 0x2E83171C, 0x0DC9A893, 0x2C86DCDD, 0x19AC574B, 0x1F8E4EDE, 0x0803364D, 0x552E844E, 0x32713884, 0x10FD5B3C, 0x6F1F456C, 0x47BA2E0A, 0x6354726E, 0x00ECA796, 0x54158548,
 0x7EB52FCD, 0x06195334, 0x220FCBEE, 0x66FA6442, 0x5DEF7768, 0x5498D7E0, 0x6B39D585, 0x0A14E24D, 0x4DD010A5, 0x7A98BA54, 0x02C60D8C, 0x1C44A16D, 0x5C3B57CA, 0x1858CD98, 0x0C8B0757, 0x161B80DF,
 0x30B28009, 0x38916A29, 0x549C5981, 0x1D82A92D, 0x0C961837, 0x040F992E, 0x2BF565E1, 0x7B1252B1, 0x73985D54, 0x64ACF5F0, 0x05927AE2, 0x67FE9B07, 0x3DF2A35C, 0x6C2CC018, 0x7BC3B529, 0x04DA22AA,
 0x4975DA91, 0x7ABB281B, 0x6F3CBCAC, 0x3C049757, 0x2247C849, 0x725DF05B, 0x378D2629, 0x4BDA786C, 0x562B0E5C, 0x246E34CF, 0x3FF997ED, 0x31EE2288, 0x16E649BF, 0x0349AD71, 0x2BC7DE82, 0x3CD5B6B6,
 0x25E6312D, 0x7AE1DA5C, 0x3E6CC563, 0x0F28C3E1, 0x0EB818ED, 0x6369FE68, 0x477F6D11, 0x7AA5969F, 0x5A23ED52, 0x26FD793C, 0x736206ED, 0x79428177, 0x4F065CFE, 0x033B3A7E, 0x4C223EF0, 0x61A3508C};

inline uint32_t rx_hash_zob32(const void* data, uint32_t length,uint32_t seed= 0x7ED5052A)
{
    uint32_t hash = seed;
    for (uint32_t i = 0; i < length; i++)
    {
        int r = (i + 1) % 32;
        uint32_t value = zob_map_table[((uint8_t*)data)[i]];
        hash ^= (value << r) | (value >> (32 - r));
    }
    return hash;
}
inline uint64_t rx_hash_zob64(const void* data, uint32_t length, uint64_t seed = 0xFCDD00A8D5D2FC6E)
{
    uint64_t hash = seed;
    for (uint32_t i = 0; i < length; i++)
    {
        int r = (i + 1) % 64;
        uint32_t value = zob_map_table[((uint8_t*)data)[i]];
        hash ^= (value << r) | (value >> (64 - r));
    }
    return hash;
}
inline uint32_t rx_hash_zob(const char *buf, uint32_t seed = 0x7ED5052A)
{
    uint32_t hash = seed;
    for (uint32_t i = 0; buf[i]; i++)
    {
        int r = (i + 1) % 32;
        uint32_t value = zob_map_table[ buf[i] ];
        hash ^= (value << r) | (value >> (32 - r));
    }
    return hash;
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
    DHT_ZOB,
    DHT_Count,                                       //当作类型的数量
} rx_data_hash32_type;

//-----------------------------------------------------
//根据哈希函数类型获取其对应的算法名称
inline const char* rx_data_hash32_name(rx_data_hash32_type Type)
{
    switch(Type)
    {
        case DHT_RS:
            return "DataHash::RS";
        case DHT_JS:
            return "DataHash::JS";
        case DHT_PJW:
            return "DataHash::PJW";
        case DHT_ELF:
            return "DataHash::ELF";
        case DHT_BKDR:
            return "DataHash::BKDR";
        case DHT_SDBM:
            return "DataHash::SDBM";
        case DHT_DJB:
            return "DataHash::DJB";
        case DHT_AP:
            return "DataHash::AP";
        case DHT_DEK:
            return "DataHash::DEK";
        case DHT_BP:
            return "DataHash::BP";
        case DHT_FNV:
            return "DataHash::FNV";
        case DHT_MURMUR:
            return "DataHash::murmur";
        case DHT_MOSQUITO:
            return "DataHash::mosquito";
        case DHT_FAST:
            return "DataHash::fasthash";
        case DHT_ZOB:
            return "DataHash::Zobrist";
        default:
            return "Hash::Unknown";
    }
}

//-----------------------------------------------------
//已经实现的数据哈希算法
static const rx_data_hash32_t rx_data_hash32_funcs[]=
{
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
    rx_hash_fast32,
    rx_hash_zob32
};
const uint32_t rx_data_hash32_funcs_count = sizeof(rx_data_hash32_funcs) / sizeof(rx_data_hash32_funcs[0]);
//根据哈希函数类型获取对应的哈希函数
inline rx_data_hash32_t rx_data_hash32(const rx_data_hash32_type type=rx_data_hash32_type(rx_data_hash32_funcs_count-1))
{
    rx_static_assert(DHT_Count == rx_data_hash32_funcs_count);
    return rx_data_hash32_funcs[(uint32_t)type>=rx_data_hash32_funcs_count?rx_data_hash32_funcs_count-1:type];
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
    uint32_t lc=len>>2;
    uint32_t hash = seed+len;
    for (uint32_t i = 0; i < lc; i++)
        hash ^= hf(((uint32_t*)data)[i]);

    uint32_t v=0;
    uint8_t *pos2=(uint8_t*)((uint32_t*)data+lc);
    switch (len & 3)
    {
        case 3:
            v ^= (uint32_t)pos2[2] << 16;
        case 2:
            v ^= (uint32_t)pos2[1] << 8;
        case 1:
            v ^= (uint32_t)pos2[0];
            hash ^= hf(v);
    }
    return hf(hash);
}

//-----------------------------------------------------
//利用不同的整数哈希函数类型,构造数据哈希函数族,可通过序号获取
static const rx_data_hash32_t rx_data_hash32s_funcs[]=
{
    rx_data_hash32< rx_hash_tomas      >,
    rx_data_hash32< rx_hash_bobj       >,
    rx_data_hash32< rx_hash_murmur3    >,
    rx_data_hash32< rx_hash_mosquito   >,
    rx_data_hash32< rx_hash_skl        >,
    rx_data_hash32< rx_hash_skeeto_bsa >,
    rx_data_hash32< rx_hash_skeeto_2sa >,
    rx_data_hash32< rx_hash_skeeto_3sa >,
    rx_data_hash32< rx_hash_skeeto_3sb >,
    rx_data_hash32< rx_hash_skeeto_3s  >,
    rx_data_hash32< rx_hash_skeeto_3sr >
};
const uint32_t rx_data_hash32s_count = sizeof(rx_data_hash32s_funcs) / sizeof(rx_data_hash32s_funcs[0]);
inline rx_data_hash32_t rx_data_hash32s(const uint32_t idx=rx_data_hash32s_count-1)
{
    rx_static_assert(rx_data_hash32s_count == IHT_Count);
    return rx_data_hash32s_funcs[idx>=rx_data_hash32s_count?rx_data_hash32s_count-1:idx];
}

#endif
