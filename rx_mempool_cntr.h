#ifndef _RX_MEMPOOL_CNTR_H_
#define _RX_MEMPOOL_CNTR_H_

#include "rx_cc_macro.h"
#include "rx_mempool.h"
#include "rx_ct_util.h"
#include "rx_assert.h"
#include <math.h>
namespace rx
{
    //---------------------------------------------------------
    //内存池容器,可以线性递增放置多个固定尺寸内存池,形成动态尺寸的分配能力
	template<class pool_t,class cfg_t= mempool_cfg_t>
	class mempool_cntr_lin:public mempool_i
	{
    private:
		pool_t     *m_pool_array;							//固定尺寸内存块数组指针
		enum{FM_PoolCount=cfg_t::MaxNodeSize/cfg_t::MinAlignSize};//定长内存池线性递增所需数组尺寸
        //-----------------------------------------------------
		//待分配的尺寸向上对齐后再计算在定长内存池数组中的偏移位置
        static uint32_t on_array_idx(uint32_t Size,uint32_t &blocksize)
        {
			blocksize=size_align_to(Size,cfg_t::MinAlignSize);
            return ((Size + (cfg_t::MinAlignSize - 1)) >> cfg_t::MinSizeShiftBit) - 1;
        }
		//-----------------------------------------------------
		//初始化内存池:固定池中每块包含的节点数量,清空池定时时间
		bool m_init()
		{
			if (m_pool_array)
			{
				rx_alert("该内存池已经初始化过了!");
				return false;
			}
			if(cfg_t::MinAlignSize<8||cfg_t::MinAlignSize%4||cfg_t::MaxNodeSize%4||cfg_t::MaxNodeSize%cfg_t::MinAlignSize)
			{
				rx_alert("使用错误的参数进行初始化!");
				return false;
			}
			m_pool_array=new pool_t[FM_PoolCount];
			for(uint32_t i=1;i<=FM_PoolCount;i++)
				m_pool_array[i-1].init(cfg_t::MinAlignSize*i);

			return true;
		}
		//-----------------------------------------------------
		bool m_uninit()
		{
			if (!m_pool_array) return false;
			for(uint32_t i=0;i<FM_PoolCount;i++)
				m_pool_array[i].uninit();
			delete [] m_pool_array;
			m_pool_array=NULL;
			return true;
		}

		//-----------------------------------------------------
		mempool_cntr_lin &operator =(const mempool_cntr_lin &);
		mempool_cntr_lin(const mempool_cntr_lin&);
	public:
		typedef cfg_t mem_cfg_t;
		//-----------------------------------------------------
		mempool_cntr_lin():m_pool_array(NULL){m_init();}
        virtual ~mempool_cntr_lin() { m_uninit(); }
		//-----------------------------------------------------
        //分配尺寸为Size的内存块
		void* do_alloc(uint32_t &blocksize,uint32_t Size)
		{
			if (Size<=cfg_t::MaxNodeSize)
			{//从内部固定池里面分配
                uint32_t idx = on_array_idx(Size, blocksize);
                rx_assert(idx<FM_PoolCount);
                return m_pool_array[idx].do_alloc(blocksize, blocksize);
			}
			else
			{//使用物理池进行分配
				blocksize=Size;
				return mempool_std::do_alloc(Size);
			}
		}
		//-----------------------------------------------------
        //释放尺寸为Size的内存块
		void do_free(void* P,uint32_t Size)
		{
			if (Size<=cfg_t::MaxNodeSize)
			{//归还到内部固定池里面
				uint32_t blocksize;
				rx_assert(on_array_idx(Size,blocksize)<FM_PoolCount);
				m_pool_array[on_array_idx(Size,blocksize)].do_free(P,blocksize);
			}
			else
			{//归还到物理池
				mempool_std::do_free(P,Size);
			}
		}
	};


    //---------------------------------------------------------
    //内存池容器,可以指数递增放置多个固定尺寸内存池,形成动态尺寸的分配能力
	template<class pool_t,class cfg_t= mempool_cfg_t>
	class mempool_cntr_pow2:public mempool_i
	{
    private:
		pool_t     *m_pool_array;							//固定尺寸内存块数组指针
		enum{FM_PoolCount=cfg_t::MaxNodeSizeShiftBit-cfg_t::MinSizeShiftBit+1};//定长内存池线性递增所需数组尺寸
        //-----------------------------------------------------
		//待分配的尺寸向上对齐后再计算在定长内存池数组中的偏移位置
        static uint32_t on_array_idx(uint32_t Size,uint32_t &blocksize)
        {
            if (Size <= cfg_t::MinAlignSize)
            {
                blocksize = cfg_t::MinAlignSize;
                return 0;
            }
            else
            {
                uint32_t offset = round_up(::log2(Size));
                blocksize = 1 << offset;
                return (offset -cfg_t::MinSizeShiftBit);
            }
        }
		//-----------------------------------------------------
		//初始化内存池:固定池中每块包含的节点数量,清空池定时时间
		bool m_init()
		{
			if (m_pool_array)
			{
				rx_alert("该内存池已经初始化过了!");
				return false;
			}
			if(cfg_t::MinAlignSize<8||cfg_t::MinAlignSize%4||cfg_t::MaxNodeSize%4||cfg_t::MaxNodeSize%cfg_t::MinAlignSize)
			{
				rx_alert("使用错误的参数进行初始化!");
				return false;
			}
			m_pool_array=new pool_t[FM_PoolCount];
			for(uint32_t i=0;i<FM_PoolCount;i++)
				m_pool_array[i].init(cfg_t::MinAlignSize<<i);

			return true;
		}
		//-----------------------------------------------------
		bool m_uninit()
		{
			if (!m_pool_array) return false;
			for(uint32_t i=0;i<FM_PoolCount;i++)
				m_pool_array[i].uninit();
			delete [] m_pool_array;
			m_pool_array=NULL;
			return true;
		}

		//-----------------------------------------------------
		mempool_cntr_pow2 &operator =(const mempool_cntr_pow2 &);
		mempool_cntr_pow2(const mempool_cntr_pow2&);
	public:
		typedef cfg_t mem_cfg_t;
		//-----------------------------------------------------
		mempool_cntr_pow2():m_pool_array(NULL){m_init();}
        virtual ~mempool_cntr_pow2() { m_uninit(); }
		//-----------------------------------------------------
        //分配尺寸为Size的内存块
		void* do_alloc(uint32_t &blocksize,uint32_t Size)
		{
			if (Size<=cfg_t::MaxNodeSize)
			{//从内部固定池里面分配
                uint32_t idx = on_array_idx(Size, blocksize);
				rx_assert(idx<FM_PoolCount);
				return m_pool_array[idx].do_alloc(blocksize,blocksize);
			}
			else
			{//使用物理池进行分配
				blocksize=Size;
				return mempool_std::do_alloc(Size);
			}
		}
		//-----------------------------------------------------
        //释放尺寸为Size的内存块
		void do_free(void* P,uint32_t Size)
		{
			if (Size<=cfg_t::MaxNodeSize)
			{//归还到内部固定池里面
				uint32_t blocksize;
				rx_assert(on_array_idx(Size,blocksize)<FM_PoolCount);
				m_pool_array[on_array_idx(Size,blocksize)].do_free(P,blocksize);
			}
			else
			{//归还到物理池
				mempool_std::do_free(P,Size);
			}
		}
	};
}
#endif