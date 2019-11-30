#ifndef _RX_MEMPOOL_CNTR_H_
#define _RX_MEMPOOL_CNTR_H_

#include "rx_cc_macro.h"
#include "rx_mem_pool.h"
#include "rx_ct_util.h"
#include "rx_ct_bitop.h"
#include "rx_hash_tlmap.h"
#include "rx_assert.h"
#include <math.h>
#include <new>

namespace rx
{
    //---------------------------------------------------------
    //内存池容器基类,可以放置多个固定尺寸内存池,形成动态尺寸的分配能力
    template<class pool_t,class cfg_t= mempool_cfg_t>
    class mempool_cntr_t:public mempool_i
    {
    protected:
        pool_t     *m_pool_array;							//固定尺寸内存块数组指针
        //-----------------------------------------------------
        //待分配的尺寸向上对齐后再计算在定长内存池数组中的偏移位置
        virtual uint32_t on_array_idx(uint32_t Size,uint32_t &blocksize)=0;
        virtual bool on_init()=0;
        virtual void on_uninit()=0;
        //-----------------------------------------------------
        mempool_cntr_t &operator =(const mempool_cntr_t &);
        mempool_cntr_t(const mempool_cntr_t&);
    public:
        typedef cfg_t mem_cfg_t;
        //-----------------------------------------------------
        mempool_cntr_t():m_pool_array(NULL) {}
        //-----------------------------------------------------
        //初始化内存池:固定池中每块包含的节点数量,清空池定时时间
        virtual bool do_init(uint32_t size=0)
        {
            if (m_pool_array)
            {
                rx_alert("The memory pool has been initialized!");
                return false;
            }
            if(cfg_t::MinNodeSize<8||cfg_t::MinNodeSize%4||cfg_t::MaxNodeSize%4||cfg_t::MaxNodeSize%cfg_t::MinNodeSize)
            {
                rx_alert("Initialize with the wrong parameters!");
                return false;
            }
            return on_init();
        }
        //-----------------------------------------------------
        virtual void do_uninit(bool force=false)
        {
            if (!m_pool_array)
                return;
            on_uninit();
            m_pool_array=NULL;
        }

        //-----------------------------------------------------
        //分配尺寸为Size的内存块
        void* do_alloc(uint32_t &blocksize,uint32_t Size)
        {
            if (Size<=cfg_t::MaxNodeSize)
            {
                //从内部固定池里面分配
                uint32_t idx = on_array_idx(Size, blocksize);
                return m_pool_array[idx].do_alloc(blocksize, blocksize);
            }
            else
            {
                //使用物理池进行分配
                return base_alloc(blocksize, Size);
            }
        }
        //-----------------------------------------------------
        //释放尺寸为Size的内存块
        void do_free(void* P,uint32_t Size)
        {
            if (Size<=cfg_t::MaxNodeSize)
            {
                //归还到内部固定池里面
                uint32_t blocksize;
                m_pool_array[on_array_idx(Size,blocksize)].do_free(P,blocksize);
            }
            else
            {
                //归还到物理池
                base_free(P,Size);
            }
        }
    };


    //---------------------------------------------------------
    //内存池容器,线性递增
    //---------------------------------------------------------
    template<class pool_t,class cfg_t= mempool_cfg_t>
    class mempool_cntr_lin_t:public mempool_cntr_t<pool_t,cfg_t>
    {
        uint32_t m_pool_array_size;
    private:
        typedef mempool_cntr_t<pool_t,cfg_t> parent_t;
        enum {FM_PoolCount=cfg_t::MaxNodeSize/cfg_t::MinNodeSize}; //定长内存池线性递增所需数组尺寸
        //-----------------------------------------------------
        //待分配的尺寸向上对齐后再计算在定长内存池数组中的偏移位置
        virtual uint32_t on_array_idx(uint32_t Size,uint32_t &blocksize)
        {
            blocksize=size_align_to(Size,cfg_t::MinNodeSize);
            uint32_t idx=((Size + (cfg_t::MinNodeSize - 1)) >> cfg_t::MinNodeShiftBit) - 1;
            rx_assert(idx<FM_PoolCount);
            return idx;
        }
        //-----------------------------------------------------
        virtual bool on_init()
        {
            parent_t::m_pool_array = (pool_t*)parent_t::base_alloc(m_pool_array_size,sizeof(pool_t)*FM_PoolCount);
            if (!parent_t::m_pool_array)
                return false;
            ct::AC(parent_t::m_pool_array, FM_PoolCount);

            for(uint32_t i=1; i<=FM_PoolCount; i++)
                if (!parent_t::m_pool_array[i-1].do_init(cfg_t::MinNodeSize*i))
                    return false;
            return true;
        }
        //-----------------------------------------------------
        virtual void on_uninit()
        {
            for(uint32_t i=0; i<FM_PoolCount; i++)
                parent_t::m_pool_array[i].do_uninit();
            ct::AD(parent_t::m_pool_array, FM_PoolCount);
            parent_t::base_free(parent_t::m_pool_array, m_pool_array_size);
        }
        //-----------------------------------------------------
        mempool_cntr_lin_t &operator =(const mempool_cntr_lin_t &);
        mempool_cntr_lin_t(const mempool_cntr_lin_t&);
    public:
        //-----------------------------------------------------
        mempool_cntr_lin_t(bool init=true) {if (init) parent_t::do_init();}
        virtual ~mempool_cntr_lin_t() { parent_t::do_uninit(); }
    };


    //---------------------------------------------------------
    //内存池容器,指数递增
    //---------------------------------------------------------
    template<class pool_t,class cfg_t= mempool_cfg_t>
    class mempool_cntr_pow2_t:public mempool_cntr_t<pool_t,cfg_t>
    {
        uint32_t m_pool_array_size;
    private:
        typedef mempool_cntr_t<pool_t,cfg_t> parent_t;
        enum {FM_PoolCount=cfg_t::MaxNodeShiftBit-cfg_t::MinNodeShiftBit+1}; //定长内存池线性递增所需数组尺寸
        //-----------------------------------------------------
        //待分配的尺寸向上对齐后再计算在定长内存池数组中的偏移位置
        virtual uint32_t on_array_idx(uint32_t Size,uint32_t &blocksize)
        {
            if (Size <= cfg_t::MinNodeSize)
            {
                blocksize = cfg_t::MinNodeSize;
                return 0;
            }
            else
            {
                rx_static_assert((!!0)==0);
                rx_static_assert((!!3) == 1);                   //要求逻辑取反后的值为0或1

                uint32_t offset = rx_fls(Size)-1;               //快速查找Size的高比特位置,可以当作是LOG2(Size)
                blocksize = 1 << offset;                        //先计算一下Size对应的pow2(offset)整数
                uint32_t bit_adj = (uint32_t)!!(blocksize^Size);//异或后的值进行逻辑化处理,就是为了判断是否Size大于pow2(offset)
                blocksize <<= bit_adj;                          //进行最终的尺寸校正
                offset += bit_adj;                              //进行最终的偏移校正

                uint32_t idx=(offset -cfg_t::MinNodeShiftBit);
                rx_assert(idx<FM_PoolCount);
                return idx;
            }
        }
        //-----------------------------------------------------
        virtual bool on_init()
        {
            parent_t::m_pool_array = (pool_t*)parent_t::base_alloc(m_pool_array_size, sizeof(pool_t)*FM_PoolCount);
            if (!parent_t::m_pool_array)
                return false;
            ct::AC(parent_t::m_pool_array, FM_PoolCount);

            for(uint32_t i=0; i<FM_PoolCount; i++)
                if (!parent_t::m_pool_array[i].do_init(cfg_t::MinNodeSize<<i))
                    return false;
            return true;
        }
        //-----------------------------------------------------
        virtual void on_uninit()
        {
            for(uint32_t i=0; i<FM_PoolCount; i++)
                parent_t::m_pool_array[i].do_uninit();

            ct::AD(parent_t::m_pool_array, FM_PoolCount);
            parent_t::base_free(parent_t::m_pool_array, m_pool_array_size);
        }
        //-----------------------------------------------------
        mempool_cntr_pow2_t &operator =(const mempool_cntr_pow2_t &);
        mempool_cntr_pow2_t(const mempool_cntr_pow2_t&);
    public:
        typedef cfg_t mem_cfg_t;
        //-----------------------------------------------------
        mempool_cntr_pow2_t(bool init=true) {if (init) parent_t::do_init();}
        virtual ~mempool_cntr_pow2_t() { parent_t::do_uninit(); }
    };


    //---------------------------------------------------------
    //内存池容器,两级bit映射
    //---------------------------------------------------------
    template<class pool_t,class cfg_t= mempool_cfg_t,uint32_t SLI_COUNT=8,bool USE_MIN_ALIGN=true>
    class mempool_cntr_tlmap_t:public mempool_cntr_t<pool_t,cfg_t>
    {
        uint32_t m_pool_array_size;
    private:
        typedef mempool_cntr_t<pool_t,cfg_t> parent_t;
        //---------------------------------------------------------
        //两级bitmap索引计算的限定条件
        typedef struct tlmap_cfg_t
        {
            enum
            {
                MIN_ALIGN  = cfg_t::MinNodeSize,            //最小对齐尺寸
                FLI_MAX    = rx::LOG2<cfg_t::MaxNodeSize>::result,//一级索引最大数量
                SLI_MAX    = SLI_COUNT,                     //二级索引最大数量

                SLI_SHIFT  = rx::LOG2<SLI_MAX>::result,     //二级索引的比特数量
                FLI_OFFSET = rx::LOG2<MIN_ALIGN>::result,   //一级索引的偏移量
            };
        } tlmap_cfg_t;
        enum {FM_PoolCount=tlmap_cfg_t::FLI_MAX*tlmap_cfg_t::SLI_MAX}; //定长内存池数组尺寸
        //-----------------------------------------------------
        //待分配的尺寸向上对齐后再计算在定长内存池数组中的偏移位置
        virtual uint32_t on_array_idx(uint32_t Size,uint32_t &blocksize)
        {
            //根据给的的Size进行映射,得到两级索引的位置(已经向上调整了)
            if (USE_MIN_ALIGN)
                return rx_hash_tlmap_ex2<tlmap_cfg_t>(Size,blocksize);
            else
                return rx_hash_tlmap_ex<tlmap_cfg_t>(Size, blocksize);
        }
        //-----------------------------------------------------
        virtual bool on_init()
        {
            parent_t::m_pool_array = (pool_t*)parent_t::base_alloc(m_pool_array_size, sizeof(pool_t)*FM_PoolCount);
            if (!parent_t::m_pool_array)
                return false;
            ct::AC(parent_t::m_pool_array, FM_PoolCount);

            for(uint32_t f=0; f<tlmap_cfg_t::FLI_MAX; ++f)
                for(uint32_t s=0; s<tlmap_cfg_t::SLI_MAX; ++s)
                {
                    //数组偏移计算
                    uint32_t idx=f*tlmap_cfg_t::SLI_MAX+s;
                    //对应内存池的尺寸计算
                    uint32_t size=1<<(f+tlmap_cfg_t::FLI_OFFSET);
                    size+=(size>>tlmap_cfg_t::SLI_SHIFT)*s;
                    //初始化对应的内存池
                    if (!parent_t::m_pool_array[idx].do_init(size))
                        return false;
                }
            return true;
        }
        //-----------------------------------------------------
        virtual void on_uninit()
        {
            for(uint32_t i=0; i<FM_PoolCount; i++)
                parent_t::m_pool_array[i].do_uninit();

            ct::AD(parent_t::m_pool_array, FM_PoolCount);
            parent_t::base_free(parent_t::m_pool_array, m_pool_array_size);
        }
        //-----------------------------------------------------
        mempool_cntr_tlmap_t &operator =(const mempool_cntr_tlmap_t &);
        mempool_cntr_tlmap_t(const mempool_cntr_tlmap_t&);
    public:
        typedef cfg_t mem_cfg_t;
        //-----------------------------------------------------
        mempool_cntr_tlmap_t(bool init=true) {if (init) parent_t::do_init();}
        virtual ~mempool_cntr_tlmap_t() { parent_t::do_uninit(); }
    };

}
#endif
