#ifndef _RX_HASH_BITMAP_TL_H_
#define _RX_HASH_BITMAP_TL_H_

#include "rx_cc_macro.h"
#include "rx_ct_util.h"
#include "rx_bits_op.h"

//---------------------------------------------------------
//����bitmap����������޶�����
typedef struct tlmap_cfg_t
{
    enum {
        MIN_ALIGN  = 32,                            //��С����ߴ�
        FLI_MAX    = 30,                            //һ�������������
        SLI_MAX    = 8,                             //���������������

        SLI_SHIFT  = rx::log2<SLI_MAX>::result,     //���������ı�������
        FLI_OFFSET = rx::log2<MIN_ALIGN>::result,   //һ��������ƫ����
    };
}tlmap_cfg_t;

//---------------------------------------------------------
//���ݸ�����size,�������������ж�Ӧ������λ��:fl�׼�(FLI_OFFSET~FLI_MAX-1);sl�μ�(0~SLI_MAX-1)
template<class cfg_t>
inline void rx_hash_tlmap(uint32_t size, uint32_t& fl, uint32_t& sl)
{
    /*
    https://blog.csdn.net/sunao2002002/article/details/50611838
    https://blog.csdn.net/william198757/article/details/51452514
    ��������ӳ���ԭ��:
    1 ���ݸ�����size����pow2(FL)��ָ������,ֱ�Ӷ�λ����һ��FL.
    2 ʣ���size���еڶ����ڵ����Ի���,��λ���ڶ���SL.

    ����ӳ��ı�׼�㷨:
    * FL = log2(size)   �Խ��ȡ����FL=fls(size)
    * SL = (size - pow2(FL)) * (pow2(SLI) / pow2(FL))

    SL�ı任����(����subsizeΪsize�ڵ�ǰһ��������Ӧ��ʣ��ߴ�):
    *	subsize = (size - pow2(FL))
    *           = (size ^ (1U << FL))
    *
    *	SL  = (subsize) * (SLI_MAX / 2^FL)
    *       = (subsize * SLI_MAX) / 2^FL
    *	    = subsize << SLI_SHIFT) >> FL
    *	    = subsize >> (FL - SLI_SHIFT)
    *       = (size ^ (1U << FL)) >> (FL - SLI_SHIFT)
    */

    //����ߴ���е�ֵ�޶�
    size = rx::Max(size, (uint32_t)cfg_t::MIN_ALIGN);

    //�׼��������и�ֵ�޶�
    fl = rx_fls(size) - 1;
    fl = rx::Min(fl, (uint32_t)cfg_t::FLI_MAX);

    //����μ�����
    sl = (size ^ (1UL << fl)) >> (fl - cfg_t::SLI_SHIFT);
}
//---------------------------------------------------------
//���ݸ�����size,�������������ж�Ӧ������λ��,ͬʱ���гߴ�����϶������
//����ֵ:���϶����ĳߴ�
template<class cfg_t>
inline uint32_t rx_hash_tlmap_ex(uint32_t size, uint32_t& fl, uint32_t& sl)
{
    //����ߴ���е�ֵ�޶�
    size = rx::Max(size, (uint32_t)cfg_t::MIN_ALIGN);

    //�׼��������и�ֵ�޶�
    fl = rx_fls(size) - 1;
    fl = rx::Min(fl, (uint32_t)cfg_t::FLI_MAX);

    //����μ�����
    uint32_t upsize=(1UL << fl);
    uint32_t subsize=size ^ upsize;
    uint32_t offset=fl - cfg_t::SLI_SHIFT;
    sl = subsize >> offset;

    //���гߴ�����϶������
    subsize&&(rx_ffs(subsize)-1<offset)?sl+=1:0;
    sl==cfg_t::SLI_MAX?sl=0,fl+=1:0;
    upsize=(1UL << fl);
    uint32_t sl_blk_size=(upsize>>cfg_t::SLI_SHIFT);
    upsize+=sl_blk_size*sl;
    return upsize;
}
//---------------------------------------------------------
//���ݸ�����size,�������������ϲ��������λ��,ͬʱ���гߴ�����϶������
//����ֵ:�ϲ��������.
template<class cfg_t>
inline uint32_t rx_hash_tlmap_ex(uint32_t size, uint32_t& upsize)
{
    uint32_t fl,sl;
    upsize=rx_hash_tlmap_ex<cfg_t>(size,fl,sl);
    return (fl-cfg_t::FLI_OFFSET)*cfg_t::SLI_MAX+sl;
}
#endif