#ifndef _RX_MEMPOOL_CNTR_H_
#define _RX_MEMPOOL_CNTR_H_

#include "rx_cc_macro.h"
#include "rx_mempool.h"

namespace rx
{
    //---------------------------------------------------------
    //内存池容器,可以放置多个固定尺寸内存池,形成动态尺寸的分配能力
	template<class pool_t,class CT= rx_mem_pool_cfg_t>
	class rx_mempool_cntr_lin:public rx_mem_pool_i
	{
    protected:
        //-----------------------------------------------------
        virtual uint32_t on_array_idx(uint32_t Size)
        {
            return ((Size + (CT::MinAlignSize - 1)) >> CT::ArrayIndexShiftBit) - 1;
        }
        virtual uint32_t on_array_count(uint32_t Size)
        {

        }
    private:
		pool_t     *m_pool_array;                       //固定尺寸内存块数组指针
		//-----------------------------------------------------
		//初始化内存池:固定池中每块包含的节点数量,清空池定时时间
		bool m_init()
		{
			if (m_pool_array)
			{
				HAlert("该内存池已经初始化过了!");
				return false;
			}
			if(CT::MinAlignSize<8||CT::MinAlignSize%4||CT::MaxNodeSize%4||CT::MaxNodeSize%CT::MinAlignSize)
			{
				HAlert("使用错误的参数进行初始化!");
				return false;
			}
			m_pool_array=new pool_t[CT::PoolCount];
			for(uint32_t i=0;i<CT::PoolCount;i++)
			{
				uint32_t AlignSize=CT::MinAlignSize*(i+1);
				m_pool_array[i].Init(AlignSize);
			}

			return true;
		}
		//-----------------------------------------------------
		bool m_uninit()
		{
			if (!m_pool_array) return false;
			delete [] m_pool_array;
			m_pool_array=NULL;
			return true;
		}

		//-----------------------------------------------------
		rx_mempool_cntr_lin &operator =(const rx_mempool_cntr_lin &);
		rx_mempool_cntr_lin(const rx_mempool_cntr_lin&);
	public:
		//-----------------------------------------------------
		rx_mempool_cntr_lin():m_pool_array(NULL){m_init();}
        virtual ~rx_mempool_cntr_lin() { m_uninit(); }
		//-----------------------------------------------------
        //分配尺寸为Size的内存块
		void* do_alloc(uint32_t Size)
		{
			if (Size<=CT::MaxNodeSize)
			{//从内部固定池里面分配
				rx_assert(on_array_idx(Size)<CT::PoolCount);
				//由于使用了动态数组偏移,所以此过程比静态数组偏移要慢一倍左右
				//导致非固定池比固定池的效率要慢一倍左右
				return m_pool_array[on_array_idx(Size)].do_alloc(Size);
			}
			else
			{//使用物理池进行分配
				return rx_mem_pool_std::do_alloc(Size);
			}
		}
		//-----------------------------------------------------
        //释放尺寸为Size的内存块
		void do_free(void* P,uint32_t Size)
		{
			if (Size<=CT::MaxNodeSize)
			{//归还到内部固定池里面
				rx_assert(on_array_idx(Size)<CT::PoolCount);
				//由于使用了动态数组偏移,所以此过程比静态数组偏移要慢一倍左右
				//导致非固定池比固定池的效率要慢一倍左右
				m_pool_array[on_array_idx(Size)].do_free(P,Size);
			}
			else
			{//归还到物理池
				rx_mem_pool_std::do_free(P,Size);
			}
		}
	};

}
#endif