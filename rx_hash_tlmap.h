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
//�ߴ����϶���(������С������)
template<class cfg_t>
inline uint32_t rx_hash_tlmap_up(uint32_t size, uint32_t offset = 1)
{
    //����ߴ���е�ֵ�޶�
    size = size_align_to(size, cfg_t::MIN_ALIGN);
    return size + (1UL << (rx_fls(size) - 1 - cfg_t::SLI_SHIFT)) - offset;
}
//�ߴ����϶���(������ӽ���)
template<class cfg_t>
inline uint32_t rx_hash_tlmap_up2(uint32_t size, uint32_t offset = 1)
{
    //����ߴ���е�ֵ�޶�
    size = rx::Max(size, (uint32_t)cfg_t::MIN_ALIGN);
    return size + (1UL << (rx_fls(size) - 1 - cfg_t::SLI_SHIFT)) - offset;
}
//---------------------------------------------------------
//���ݸ�����size,�������������ϲ��������λ��,ͬʱ���гߴ�����϶������(�����С�����)
//����ֵ:�ϲ��������.
template<class cfg_t>
inline uint32_t rx_hash_tlmap_ex(uint32_t size, uint32_t& upsize)
{
    uint32_t fl,sl;
    rx_hash_tlmap<cfg_t>(rx_hash_tlmap_up<cfg_t>(size), fl, sl);

    upsize = (1 << fl);
    upsize += (upsize >> cfg_t::SLI_SHIFT)*sl;

    return (fl-cfg_t::FLI_OFFSET)*cfg_t::SLI_MAX+sl;
}
//---------------------------------------------------------
//���ݸ�����size,�������������ϲ��������λ��,ͬʱ���гߴ�����Ͻ��ն���(��ӽ���)
//����ֵ:�ϲ��������.
template<class cfg_t>
inline uint32_t rx_hash_tlmap_ex2(uint32_t size, uint32_t& upsize)
{
    uint32_t fl, sl;

    rx_hash_tlmap<cfg_t>(rx_hash_tlmap_up2<cfg_t>(size), fl, sl);
    upsize = (1<<fl);
    upsize += (upsize >> cfg_t::SLI_SHIFT)*sl;

    return (fl - cfg_t::FLI_OFFSET)*cfg_t::SLI_MAX + sl;
}

#endif
