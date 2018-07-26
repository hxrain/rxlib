#ifndef _RX_MEM_H_
#define _RX_MEM_H_

#include <memory.h>
#include <stdlib.h>
#include "rx_cc_macro.h"
#include "rx_ct_util.h"


namespace rx
{
    //------------------------------------------------------
    //内存池抽象接口
    class rx_mem_pool_i
    {
    public:
        virtual void *do_alloc(uint32_t size)=0;
        virtual void *do_realloc(void* ptr,uint32_t newsize){return NULL;}
        virtual void do_free(void* ptr, uint32_t size=0)=0;
    protected:
        virtual ~rx_mem_pool_i(){}
    };

    //------------------------------------------------------
    //基于C标准库的内存池
    class rx_mem_pool_c:public rx_mem_pool_i
    {
    public:
        virtual void *do_alloc(uint32_t size){return malloc(size);}
        virtual void *do_realloc(void* ptr,uint32_t size){return realloc(ptr,size);}
        virtual void do_free(void* ptr, uint32_t size = 0){free(ptr);}
    };


    //======================================================
    //便捷配置参数:大块缓存,默认可以缓存64K的内存块
    #ifdef RX_MEMCFG_BIG_BLOCK
        //内存池最小对齐尺寸和最小分配节点尺寸
        #ifndef RX_MEMCFG_MinAlign
        #define RX_MEMCFG_MinAlign 32
        #endif

        //内存池最大分配节点尺寸
        #ifndef RX_MEMCFG_MaxNode
        #define RX_MEMCFG_MaxNode (1024*256)
        #endif

        //内存池中每个内存条的最大尺寸
        #ifndef RX_MEMCFG_MaxStripe
        #define RX_MEMCFG_MaxStripe (RX_MEMCFG_MaxNode)
        #endif
    #endif

    //便捷配置参数:小块缓存,默认可以缓存4K的内存块
    #ifdef RX_MEMCFG_SMALL_BLOCK
        //内存池最小对齐尺寸和最小分配节点尺寸
        #ifndef RX_MEMCFG_MinAlign
        #define RX_MEMCFG_MinAlign 8
        #endif

        //内存池最大分配节点尺寸
        #ifndef RX_MEMCFG_MaxNode
        #define RX_MEMCFG_MaxNode (1024*4)
        #endif

        //内存池中每个内存条的最大尺寸
        #ifndef RX_MEMCFG_MaxStripe
        #define RX_MEMCFG_MaxStripe (RX_MEMCFG_MaxNode*2)
        #endif
    #endif

    //默认配置
    //内存池最小对齐尺寸和最小分配节点尺寸
    #ifndef RX_MEMCFG_MinAlign
        #define RX_MEMCFG_MinAlign 32
    #endif

    //内存池最大分配节点尺寸
    #ifndef RX_MEMCFG_MaxNode
        #define RX_MEMCFG_MaxNode (1024*8)
    #endif

    //内存池中每个内存条的最大尺寸
    #ifndef RX_MEMCFG_MaxStripe
        #define RX_MEMCFG_MaxStripe (RX_MEMCFG_MaxNode*2)
    #endif

    //======================================================
    //默认内存池的配置参数
    typedef struct rx_mem_pool_cfg_t
    {
        //可配置(每个值都必须为2的整数次幂):最小节点与增量尺寸;最大节点尺寸;每个内存条的最大尺寸
        //这些参数的配置决定了物理内存占用率和缓存池利用率.
        //条尺寸大一些时适合等长度内存块密集分配,但在随机长度块分配时会降低内存利用率.
        enum{MinAlignSize=RX_MEMCFG_MinAlign,MaxNodeSize=RX_MEMCFG_MaxNode,MaxStripeSize=RX_MEMCFG_MaxStripe};

        //固定内存池的最大数量
        enum{PoolCount=MaxNodeSize/MinAlignSize};

        //计算索引偏移使用,右移位数
        enum{ArrayIndexShiftBit=log2<MinAlignSize>::result};

        //校验最大节点尺寸合法性,确定MaxNodeSize是2的整数次幂
        enum{MaxNodeSizeShiftBitCheck=log2<MaxNodeSize>::result};
    }
    rx_mem_pool_cfg_t;

	//======================================================
	//标准内存分配器的存根
    typedef struct rx_alloc_cookie_t
    {
        //内存分配存根
        typedef struct HNodeCookie
        {
            uint32_t Cookie;                              //记录此内存块是否为数组,数组元素的数量
            uint32_t MemSize;                             //此内存块的尺寸
        }HNodeCookie;
        //存根结构的大小
        enum{HNodeCookieSize=sizeof(HNodeCookie)};
        //分配的类型:单独的对象,对象数组
        enum{AllocType_ObjectArray=0x80000000};
        //--------------------------------------------------
        //将附加信息放入Cookie:Count=0代表Alloc,1代表New,其他为New数组;MemSize代表本内存块的实际大小
        static inline void Record(void* P,uint32_t Count,uint32_t MemSize)
        {
            ((HNodeCookie*)P)->Cookie=Count;
            ((HNodeCookie*)P)->MemSize=MemSize;
        }
        //--------------------------------------------------
        //从Cookie中得到附加信息
        static inline bool Get(void* P,uint32_t &Count,uint32_t &MemSize)
        {
            Count=((HNodeCookie*)P)->Cookie&~AllocType_ObjectArray;
            MemSize=((HNodeCookie*)P)->MemSize;
            return !!(((HNodeCookie*)P)->Cookie&AllocType_ObjectArray);
        }
    }rx_alloc_cookie_t;

    //------------------------------------------------------
    //用于记录内存分配器工作情况
    typedef struct rx_mem_stat_t
    {
        uint32_t CacheAlloced;                                //可缓存已分配
        uint32_t RealNeedSize;                                //可缓存刚需量
        uint32_t InCacheSize;                                 //已缓存待复用
        uint32_t ExternAlloced;                               //非缓存已分配
        uint32_t CacheUsing()const{return CacheAlloced-InCacheSize;}//可缓存正使用
    }rx_mem_stat_t;

}


#endif
