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
    class mempool
    {
    public:
        virtual void *do_alloc(uint32_t &blocksize,uint32_t size)=0;
        virtual void *do_realloc(uint32_t &blocksize,void* ptr,uint32_t newsize){return NULL;}
        virtual void do_free(void* ptr, uint32_t blocksize=0)=0;
    protected:
        virtual ~mempool(){}
    };

	//------------------------------------------------------
    //用于记录内存池工作情况
    typedef struct mempool_stat_t
    {
        uint32_t alloced;                                   //缓存已分配
        uint32_t real_size;                                 //缓存刚需量
        uint32_t can_reuse;                                 //缓存待复用
        uint32_t nc_alloced;                                //非缓存分配
        uint32_t cache_using()const { return alloced - can_reuse; }//缓存使用量
    }mempool_stat_t;

    //------------------------------------------------------
    //基于C标准库的内存池
    class mempool_std 
    {
    public:
        static void *do_alloc(uint32_t size) { return malloc(size); }
        static void *do_realloc(void* ptr, uint32_t size) { return realloc(ptr, size); }
        static void do_free(void* ptr, uint32_t size = 0) { free(ptr); }
    };

    //------------------------------------------------------
    //内存池功能基类
    class mempool_i:public mempool
    {
    protected:
		mempool_stat_t	m_stat;
		mempool        *m_base;
        //--------------------------------------------------
        mempool_i():m_base(0){memset(&m_stat,0,sizeof(m_stat));}
        //--------------------------------------------------
        void *base_alloc(uint32_t &blocksize,uint32_t size)
        {
            if (m_base)
                return m_base->do_alloc(blocksize,size);
            else
            {
                blocksize=size;
                return mempool_std::do_alloc(size);
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
                return mempool_std::do_realloc(ptr,newsize);
            }
        }
        //--------------------------------------------------
        void base_free(void* ptr, uint32_t blocksize=0)
        {
            if (m_base)
                return m_base->do_free(ptr,blocksize);
            else
                return mempool_std::do_free(ptr,blocksize);
        }

	public:
        void bind(mempool& mp){m_base=&mp;}
		virtual bool do_init(uint32_t size){return true;}
		virtual void do_uninit(bool force=false){}
		const mempool_stat_t& stat() const {return m_stat;}
    };

    //======================================================
    //便捷配置参数:大块缓存,默认可以缓存64K的内存块
    #ifdef RX_MEMCFG_BIG_BLOCK
        //内存池最小对齐尺寸和最小分配节点尺寸
        #ifndef RX_MEMCFG_MinAlign
        #define RX_MEMCFG_MinAlign 128
        #endif

        //内存池最大分配节点尺寸
        #ifndef RX_MEMCFG_MaxNode
        #define RX_MEMCFG_MaxNode (1024*256)
        #endif

        //内存池中每个内存条的最大尺寸
        #ifndef RX_MEMCFG_MaxStripe
        #define RX_MEMCFG_MaxStripe (RX_MEMCFG_MaxNode*4)
        #endif
    #endif

    //便捷配置参数:小块缓存,默认可以缓存4K的内存块
    #ifdef RX_MEMCFG_SMALL_BLOCK
        //内存池最小对齐尺寸和最小分配节点尺寸
        #ifndef RX_MEMCFG_MinAlign
        #define RX_MEMCFG_MinAlign 32
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
        #define RX_MEMCFG_MinAlign 64
    #endif

    //内存池最大分配节点尺寸
    #ifndef RX_MEMCFG_MaxNode
        #define RX_MEMCFG_MaxNode (1024*16)
    #endif

    //内存池中每个内存条的最大尺寸
    #ifndef RX_MEMCFG_MaxStripe
        #define RX_MEMCFG_MaxStripe (RX_MEMCFG_MaxNode*8)
    #endif

    //======================================================
    //默认内存池的配置参数(每个值都必须为2的整数次幂)
    typedef struct mempool_cfg_t
    {
        enum {
            MinAlignSize = RX_MEMCFG_MinAlign,              //最小节点或对齐尺寸
            MaxNodeSize = RX_MEMCFG_MaxNode,                //可缓存的最大节点尺寸
            MaxStripeSize = RX_MEMCFG_MaxStripe             //每个内存条的最大尺寸
        };

        //校验最小对齐尺寸合法性,确定MinAlignSize是2的整数次幂
        enum{MinSizeShiftBit=log2<MinAlignSize>::result};
        //校验最大节点尺寸合法性,确定MaxNodeSize是2的整数次幂
        enum{MaxNodeSizeShiftBit=log2<MaxNodeSize>::result};
    }mempool_cfg_t;

	//======================================================
	//标准内存分配器的存根
    typedef struct alloc_cookie_t
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
    }alloc_cookie_t;
}


#endif
