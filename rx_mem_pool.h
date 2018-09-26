#ifndef _RX_MEM_H_
#define _RX_MEM_H_

#include <memory.h>
#include <stdlib.h>
#include "rx_cc_macro.h"
#include "rx_ct_util.h"

namespace rx
{
    //------------------------------------------------------
    //内存池分配接口
    class mempool_t
    {
    public:
        virtual void *do_alloc(uint32_t &blocksize,uint32_t size)=0;
        virtual void do_free(void* ptr, uint32_t blocksize=0)=0;
        virtual void *do_realloc(uint32_t &blocksize, void* ptr, uint32_t newsize) { return NULL; }
    protected:
        virtual ~mempool_t() {}
    };

    //------------------------------------------------------
    //用于记录内存分配器的工作情况
    typedef struct alloc_stat_t
    {
        uint32_t c_alloced;                                 //缓存已分配
        uint32_t c_realsize;                                //缓存刚需量
    } alloc_stat_t;

    //------------------------------------------------------
    //用于记录内存池的工作情况
    typedef struct mempool_stat_t
    {
        uint32_t pool_size;                                 //内存池总量
        uint32_t pool_free;                                 //内存池余量
        uint32_t nc_alloced;                                //非缓存分配
    } mempool_stat_t;

    //------------------------------------------------------
    //基于C标准库的内存池
    template<class dummy=void>
    class mempool_std_t:public mempool_t
    {
    public:
        static void *alloc(uint32_t size) { return ::malloc(size); }
        static void *realloc(void* ptr, uint32_t size) { return ::realloc(ptr, size); }
        static void free(void* ptr, uint32_t size = 0) { ::free(ptr); }
        
        virtual void *do_alloc(uint32_t &blocksize, uint32_t size) { blocksize = size; return ::malloc(size); }
        virtual void *do_realloc(uint32_t &blocksize, void* ptr, uint32_t newsize) { blocksize = newsize; return ::realloc(ptr,newsize); }
        virtual void do_free(void* ptr, uint32_t blocksize = 0) { ::free(ptr); }
        virtual bool do_init(uint32_t size = 0) { return true; }
        virtual void do_uninit(bool force = false) {}
    };

    //------------------------------------------------------
    //内存池功能基类
    class mempool_i:public mempool_t
    {
    protected:
        mempool_stat_t	m_stat;
        mempool_t        *m_base;
        //--------------------------------------------------
        mempool_i():m_base(0) {memset(&m_stat,0,sizeof(m_stat));}
        //--------------------------------------------------
        void *base_alloc(uint32_t &blocksize,uint32_t size)
        {
            if (m_base)
                return m_base->do_alloc(blocksize,size);
            else
            {
                blocksize=size;
                return mempool_std_t<>::alloc(size);
            }
        }
        //--------------------------------------------------
        void *base_realloc(uint32_t &blocksize,void* ptr,uint32_t newsize)
        {
            if (m_base)
                return m_base->do_realloc(blocksize,ptr,newsize);
            else
            {
                blocksize=newsize;
                return mempool_std_t<>::realloc(ptr,newsize);
            }
        }
        //--------------------------------------------------
        void base_free(void* ptr, uint32_t blocksize=0)
        {
            if (m_base)
                return m_base->do_free(ptr,blocksize);
            else
                return mempool_std_t<>::free(ptr,blocksize);
        }

    public:
        void bind(mempool_t& mp) {m_base=&mp;}
        const mempool_stat_t& stat() const {return m_stat;}

        virtual bool do_init(uint32_t size = 0) { return true; }
        virtual void do_uninit(bool force = false) {}
    };

    //======================================================
    //便捷配置参数:大块缓存,默认可以缓存64K的内存块
#ifdef RX_MEMCFG_BIG_BLOCK
    //内存池最小分配节点尺寸
    #ifndef RX_MEMCFG_MinNode
        #define RX_MEMCFG_MinNode 128
    #endif

    //内存池最大分配节点尺寸
    #ifndef RX_MEMCFG_MaxNode
        #define RX_MEMCFG_MaxNode (1024*256)
    #endif

    //内存池中每个内存条的最大尺寸
    #ifndef RX_MEMCFG_StripeAlign
        #define RX_MEMCFG_StripeAlign (RX_MEMCFG_MaxNode*16)
    #endif
#endif

    //便捷配置参数:小块缓存,默认可以缓存4K的内存块
#ifdef RX_MEMCFG_SMALL_BLOCK
    //内存池最小对齐尺寸和最小分配节点尺寸
    #ifndef RX_MEMCFG_MinNode
        #define RX_MEMCFG_MinNode 32
    #endif

    //内存池最大分配节点尺寸
    #ifndef RX_MEMCFG_MaxNode
        #define RX_MEMCFG_MaxNode (1024*4)
    #endif

    //内存池中每个内存条的最大尺寸
    #ifndef RX_MEMCFG_StripeAlign
        #define RX_MEMCFG_StripeAlign (RX_MEMCFG_MaxNode*2)
    #endif
#endif

    //默认配置
    //内存池最小对齐尺寸和最小分配节点尺寸
#ifndef RX_MEMCFG_MinNode
    #define RX_MEMCFG_MinNode 32
#endif

    //内存池最大分配节点尺寸
#ifndef RX_MEMCFG_MaxNode
    #define RX_MEMCFG_MaxNode (1024*16)
#endif

    //内存池中每个内存条的最大尺寸
#ifndef RX_MEMCFG_StripeAlign
    #define RX_MEMCFG_StripeAlign (RX_MEMCFG_MaxNode*8)
#endif

    //======================================================
    //默认内存池的配置参数(每个值都必须为2的整数次幂)
    typedef struct mempool_cfg_t
    {
        enum
        {
            //可缓存的最小节点尺寸
            MinNodeSize = RX_MEMCFG_MinNode,
            //可缓存的最大节点尺寸
            MaxNodeSize = RX_MEMCFG_MaxNode,
            //每个内存条的最大或对齐尺寸
            StripeAlignSize = RX_MEMCFG_StripeAlign
        };

        //默认不需改动:log2(最小对齐尺寸),确定MinNodeSize是2的整数次幂
        enum {MinNodeShiftBit=LOG2<MinNodeSize>::result};
        //默认不需改动:log2(最大节点尺寸),确定MaxNodeSize是2的整数次幂
        enum {MaxNodeShiftBit=LOG2<MaxNodeSize>::result};
    } mempool_cfg_t;
}


#endif
