#ifndef _RX_MEMPOOL_CNTR_H_
#define _RX_MEMPOOL_CNTR_H_

#include "rx_cc_macro.h"
#include "rx_mempool.h"
#include "rx_ct_util.h"
#include "rx_bits_op.h"
#include "rx_hash_tlmap.h"
#include "rx_assert.h"
#include <math.h>
#include <new>

//---------------------------------------------------------
#if RX_CC==RX_CC_VC && RX_CC_VER_MAJOR<=16
	inline double log2(double v){return log(v)/0.693147180559945309417;}
#endif

namespace rx
{
    //---------------------------------------------------------
    //内存池容器基类,可以放置多个固定尺寸内存池,形成动态尺寸的分配能力
	template<class pool_t,class cfg_t= mempool_cfg_t>
	class mempool_cntr_base:public mempool_i
	{
    protected:
		pool_t     *m_pool_array;							//固定尺寸内存块数组指针
        //-----------------------------------------------------
		//待分配的尺寸向上对齐后再计算在定长内存池数组中的偏移位置
        virtual uint32_t on_array_idx(uint32_t Size,uint32_t &blocksize)=0;
        virtual bool on_init()=0;
        virtual void on_uninit()=0;
		//-----------------------------------------------------
		mempool_cntr_base &operator =(const mempool_cntr_base &);
		mempool_cntr_base(const mempool_cntr_base&);
	public:
		typedef cfg_t mem_cfg_t;
		//-----------------------------------------------------
        mempool_cntr_base():m_pool_array(NULL){}
		//-----------------------------------------------------
		//初始化内存池:固定池中每块包含的节点数量,清空池定时时间
		virtual bool do_init(uint32_t size=0)
		{
			if (m_pool_array)
			{
				rx_alert("该内存池已经初始化过了!");
				return false;
			}
			if(cfg_t::MinNodeSize<8||cfg_t::MinNodeSize%4||cfg_t::MaxNodeSize%4||cfg_t::MaxNodeSize%cfg_t::MinNodeSize)
			{
				rx_alert("使用错误的参数进行初始化!");
				return false;
			}
			return on_init();
		}
		//-----------------------------------------------------
		virtual void do_uninit(bool force=false)
		{
			if (!m_pool_array) return;
            on_uninit();
            m_pool_array=NULL;
		}

		//-----------------------------------------------------
        //分配尺寸为Size的内存块
		void* do_alloc(uint32_t &blocksize,uint32_t Size)
		{
			if (Size<=cfg_t::MaxNodeSize)
			{//从内部固定池里面分配
                uint32_t idx = on_array_idx(Size, blocksize);
                return m_pool_array[idx].do_alloc(blocksize, blocksize);
			}
			else
			{//使用物理池进行分配
				return base_alloc(Size,blocksize);
			}
		}
		//-----------------------------------------------------
        //释放尺寸为Size的内存块
		void do_free(void* P,uint32_t Size)
		{
			if (Size<=cfg_t::MaxNodeSize)
			{//归还到内部固定池里面
				uint32_t blocksize;
				m_pool_array[on_array_idx(Size,blocksize)].do_free(P,blocksize);
			}
			else
			{//归还到物理池
				base_free(P,Size);
			}
		}
	};


    //---------------------------------------------------------
    //内存池容器,线性递增
	template<class pool_t,class cfg_t= mempool_cfg_t>
	class mempool_cntr_lin:public mempool_cntr_base<pool_t,cfg_t>
	{
    private:
        typedef mempool_cntr_base<pool_t,cfg_t> parent_t;
		enum{FM_PoolCount=cfg_t::MaxNodeSize/cfg_t::MinNodeSize};//定长内存池线性递增所需数组尺寸
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
			parent_t::m_pool_array=new pool_t[FM_PoolCount];
			for(uint32_t i=1;i<=FM_PoolCount;i++)
				if (!parent_t::m_pool_array[i-1].do_init(cfg_t::MinNodeSize*i))
                    return false;
            return true;
        }
		//-----------------------------------------------------
        virtual void on_uninit()
        {
			for(uint32_t i=0;i<FM_PoolCount;i++)
				parent_t::m_pool_array[i].do_uninit();
			delete [] parent_t::m_pool_array;
        }
		//-----------------------------------------------------
		mempool_cntr_lin &operator =(const mempool_cntr_lin &);
		mempool_cntr_lin(const mempool_cntr_lin&);
	public:
		//-----------------------------------------------------
        mempool_cntr_lin(bool init=true){if (init) parent_t::do_init();}
        virtual ~mempool_cntr_lin() { parent_t::do_uninit(); }
	};


    //---------------------------------------------------------
    //内存池容器,指数递增
	template<class pool_t,class cfg_t= mempool_cfg_t>
	class mempool_cntr_pow2:public mempool_cntr_base<pool_t,cfg_t>
	{
    private:
        typedef mempool_cntr_base<pool_t,cfg_t> parent_t;
		enum{FM_PoolCount=cfg_t::MaxNodeShiftBit-cfg_t::MinNodeShiftBit+1};//定长内存池线性递增所需数组尺寸
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

                uint32_t offset = rx_fls(Size)-1;               //快速查找Size的高比特位置,可以当作是log2(Size)
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
			parent_t::m_pool_array=new pool_t[FM_PoolCount];
			for(uint32_t i=0;i<FM_PoolCount;i++)
        		if (!parent_t::m_pool_array[i].do_init(cfg_t::MinNodeSize<<i))
                    return false;
            return true;
        }
		//-----------------------------------------------------
        virtual void on_uninit()
        {
			for(uint32_t i=0;i<FM_PoolCount;i++)
				parent_t::m_pool_array[i].do_uninit();
			delete [] parent_t::m_pool_array;
        }

		//-----------------------------------------------------
		mempool_cntr_pow2 &operator =(const mempool_cntr_pow2 &);
		mempool_cntr_pow2(const mempool_cntr_pow2&);
	public:
		typedef cfg_t mem_cfg_t;
		//-----------------------------------------------------
		mempool_cntr_pow2(bool init=true){if (init) parent_t::do_init();}
        virtual ~mempool_cntr_pow2() { parent_t::do_uninit(); }
	};


    //---------------------------------------------------------
    //内存池容器,两级bit映射
	template<class pool_t,class cfg_t= mempool_cfg_t,uint32_t SLI_COUNT=8>
	class mempool_cntr_tlmap:public mempool_cntr_base<pool_t,cfg_t>
	{
    private:
        typedef mempool_cntr_base<pool_t,cfg_t> parent_t;
        //---------------------------------------------------------
        //两级bitmap索引计算的限定条件
        typedef struct tlmap_cfg_t
        {
            enum {
                MIN_ALIGN  = cfg_t::MinNodeSize,            //最小对齐尺寸
                FLI_MAX    = log2<cfg_t::MaxNodeSize>::result,//一级索引最大数量
                SLI_MAX    = SLI_COUNT,                     //二级索引最大数量

                SLI_SHIFT  = rx::log2<SLI_MAX>::result,     //二级索引的比特数量
                FLI_OFFSET = rx::log2<MIN_ALIGN>::result,   //一级索引的偏移量
            };
        }tlmap_cfg_t;
        enum{FM_PoolCount=tlmap_cfg_t::FLI_MAX*tlmap_cfg_t::SLI_MAX};//定长内存池数组尺寸
        //-----------------------------------------------------
		//待分配的尺寸向上对齐后再计算在定长内存池数组中的偏移位置
        virtual uint32_t on_array_idx(uint32_t Size,uint32_t &blocksize)
        {
            //根据给的的Size进行映射,得到两级索引的位置(已经向上调整了)
            return rx_hash_tlmap_ex<tlmap_cfg_t>(Size,blocksize);         
        }
		//-----------------------------------------------------
        virtual bool on_init()
        {
			parent_t::m_pool_array=new pool_t[FM_PoolCount];
            for(uint32_t f=0;f<tlmap_cfg_t::FLI_MAX;++f)
                for(uint32_t s=0;s<tlmap_cfg_t::SLI_MAX;++s)
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
			for(uint32_t i=0;i<FM_PoolCount;i++)
				parent_t::m_pool_array[i].do_uninit();
			delete [] parent_t::m_pool_array;
        }
		//-----------------------------------------------------
		mempool_cntr_tlmap &operator =(const mempool_cntr_tlmap &);
		mempool_cntr_tlmap(const mempool_cntr_tlmap&);
	public:
		typedef cfg_t mem_cfg_t;
		//-----------------------------------------------------
		mempool_cntr_tlmap(bool init=true){if (init) parent_t::do_init();}
        virtual ~mempool_cntr_tlmap() { parent_t::do_uninit(); }
	};

}
#endif
