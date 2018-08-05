#ifndef _RX_HASH_BITMAP_TL_H_
#define _RX_HASH_BITMAP_TL_H_

#include "rx_cc_macro.h"
#include "rx_ct_util.h"
#include "rx_bits_op.h"

//---------------------------------------------------------
//两级bitmap索引计算的限定条件
typedef struct tlmap_cfg_t
{
    enum {
        MIN_ALIGN  = 32,                            //最小对齐尺寸
        FLI_MAX    = 30,                            //一级索引最大数量
        SLI_MAX    = 8,                             //二级索引最大数量

        SLI_SHIFT  = rx::log2<SLI_MAX>::result,     //二级索引的比特数量
        FLI_OFFSET = rx::log2<MIN_ALIGN>::result,   //一级索引的偏移量
    };
}tlmap_cfg_t;

//---------------------------------------------------------
//根据给定的size,计算两级索引中对应的索引位置:fl首级(FLI_OFFSET~FLI_MAX-1);sl次级(0~SLI_MAX-1)
template<class cfg_t>
inline void rx_hash_tlmap(uint32_t size, uint32_t& fl, uint32_t& sl)
{
    /*
    https://blog.csdn.net/sunao2002002/article/details/50611838
    https://blog.csdn.net/william198757/article/details/51452514
    两级索引映射的原理:
    1 根据给定的size进行pow2(FL)的指数划分,直接定位到第一级FL.
    2 剩余的size进行第二级内的线性划分,定位到第二级SL.

    两级映射的标准算法:
    * FL = log2(size)   对结果取整后FL=fls(size)
    * SL = (size - pow2(FL)) * (pow2(SLI) / pow2(FL))

    SL的变换计算(引入subsize为size在当前一级索引对应的剩余尺寸):
    *	subsize = (size - pow2(FL))
    *           = (size ^ (1U << FL))
    *
    *	SL  = (subsize) * (SLI_MAX / 2^FL)
    *       = (subsize * SLI_MAX) / 2^FL
    *	    = subsize << SLI_SHIFT) >> FL
    *	    = subsize >> (FL - SLI_SHIFT)
    *       = (size ^ (1U << FL)) >> (FL - SLI_SHIFT)
    */

    //输入尺寸进行低值限定
    size = rx::Max(size, (uint32_t)cfg_t::MIN_ALIGN);

    //首级索引进行高值限定
    fl = rx_fls(size) - 1;
    fl = rx::Min(fl, (uint32_t)cfg_t::FLI_MAX);

    //计算次级索引
    sl = (size ^ (1UL << fl)) >> (fl - cfg_t::SLI_SHIFT);
}
//---------------------------------------------------------
//根据给定的size,计算两级索引中对应的索引位置,同时进行尺寸的向上对齐调整
//返回值:向上对齐后的尺寸
template<class cfg_t>
inline uint32_t rx_hash_tlmap_ex(uint32_t size, uint32_t& fl, uint32_t& sl)
{
    //输入尺寸进行低值限定
    size = rx::Max(size, (uint32_t)cfg_t::MIN_ALIGN);

    //首级索引进行高值限定
    fl = rx_fls(size) - 1;
    fl = rx::Min(fl, (uint32_t)cfg_t::FLI_MAX);

    //计算次级索引
    uint32_t upsize=(1UL << fl);
    uint32_t subsize=size ^ upsize;
    uint32_t offset=fl - cfg_t::SLI_SHIFT;
    sl = subsize >> offset;

    //进行尺寸的向上对齐调整
    subsize&&(rx_ffs(subsize)-1<offset)?sl+=1:0;
    sl==cfg_t::SLI_MAX?sl=0,fl+=1:0;
    upsize=(1UL << fl);
    uint32_t sl_blk_size=(upsize>>cfg_t::SLI_SHIFT);
    upsize+=sl_blk_size*sl;
    return upsize;
}
//---------------------------------------------------------
//根据给定的size,计算两级索引合并后的索引位置,同时进行尺寸的向上对齐调整
//返回值:合并后的索引.
template<class cfg_t>
inline uint32_t rx_hash_tlmap_ex(uint32_t size, uint32_t& upsize)
{
    uint32_t fl,sl;
    upsize=rx_hash_tlmap_ex<cfg_t>(size,fl,sl);
    return (fl-cfg_t::FLI_OFFSET)*cfg_t::SLI_MAX+sl;
}
#endif