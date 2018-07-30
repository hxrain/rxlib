#ifndef _RX_MEMPOOL_CNTR_H_
#define _RX_MEMPOOL_CNTR_H_

#include "rx_cc_macro.h"
#include "rx_mempool.h"
#include "rx_ct_util.h"
#include "rx_assert.h"
#include <math.h>

//---------------------------------------------------------
#if RX_CC==RX_CC_VC && RX_CC_VER_MAJOR<=16
	inline double log2(double v){return log(v)/0.693147180559945309417;}
#endif

namespace rx
{
    //---------------------------------------------------------
    //�ڴ����������,���Է��ö���̶��ߴ��ڴ��,�γɶ�̬�ߴ�ķ�������
	template<class pool_t,class cfg_t= mempool_cfg_t>
	class mempool_cntr_base:public mempool_i
	{
    protected:
		pool_t     *m_pool_array;							//�̶��ߴ��ڴ������ָ��
        //-----------------------------------------------------
		//������ĳߴ����϶�����ټ����ڶ����ڴ�������е�ƫ��λ��
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
		//��ʼ���ڴ��:�̶�����ÿ������Ľڵ�����,��ճض�ʱʱ��
		virtual bool do_init()
		{
			if (m_pool_array)
			{
				rx_alert("���ڴ���Ѿ���ʼ������!");
				return false;
			}
			if(cfg_t::MinNodeSize<8||cfg_t::MinNodeSize%4||cfg_t::MaxNodeSize%4||cfg_t::MaxNodeSize%cfg_t::MinNodeSize)
			{
				rx_alert("ʹ�ô���Ĳ������г�ʼ��!");
				return false;
			}
			return on_init();
		}
		//-----------------------------------------------------
		virtual bool do_uninit()
		{
			if (!m_pool_array) return false;
            on_uninit();
            m_pool_array=NULL;
			return true;
		}

		//-----------------------------------------------------
        //����ߴ�ΪSize���ڴ��
		void* do_alloc(uint32_t &blocksize,uint32_t Size)
		{
			if (Size<=cfg_t::MaxNodeSize)
			{//���ڲ��̶����������
                uint32_t idx = on_array_idx(Size, blocksize);
                return m_pool_array[idx].do_alloc(blocksize, blocksize);
			}
			else
			{//ʹ������ؽ��з���
				return base_alloc(Size,blocksize);
			}
		}
		//-----------------------------------------------------
        //�ͷųߴ�ΪSize���ڴ��
		void do_free(void* P,uint32_t Size)
		{
			if (Size<=cfg_t::MaxNodeSize)
			{//�黹���ڲ��̶�������
				uint32_t blocksize;
				m_pool_array[on_array_idx(Size,blocksize)].do_free(P,blocksize);
			}
			else
			{//�黹�������
				base_free(P,Size);
			}
		}
	};


    //---------------------------------------------------------
    //�ڴ������,���Ե���
	template<class pool_t,class cfg_t= mempool_cfg_t>
	class mempool_cntr_lin:public mempool_cntr_base<pool_t,cfg_t>
	{
    protected:
		enum{FM_PoolCount=cfg_t::MaxNodeSize/cfg_t::MinNodeSize};//�����ڴ�����Ե�����������ߴ�
        //-----------------------------------------------------
		//������ĳߴ����϶�����ټ����ڶ����ڴ�������е�ƫ��λ��
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
			m_pool_array=new pool_t[FM_PoolCount];
			for(uint32_t i=1;i<=FM_PoolCount;i++)
				if (!m_pool_array[i-1].do_init(cfg_t::MinNodeSize*i))
                    return false;
            return true;
        }
		//-----------------------------------------------------
        virtual void on_uninit()
        {
			for(uint32_t i=0;i<FM_PoolCount;i++)
				m_pool_array[i].do_uninit();
			delete [] m_pool_array;
        }
		//-----------------------------------------------------
		mempool_cntr_lin &operator =(const mempool_cntr_lin &);
		mempool_cntr_lin(const mempool_cntr_lin&);
	public:
		//-----------------------------------------------------
        mempool_cntr_lin(bool init=true){if (init) do_init();}
        virtual ~mempool_cntr_lin() { do_uninit(); }
	};


    //---------------------------------------------------------
    //�ڴ������,ָ������
	template<class pool_t,class cfg_t= mempool_cfg_t>
	class mempool_cntr_pow2:public mempool_cntr_base<pool_t,cfg_t>
	{
    private:
		enum{FM_PoolCount=cfg_t::MaxNodeShiftBit-cfg_t::MinNodeShiftBit+1};//�����ڴ�����Ե�����������ߴ�
        //-----------------------------------------------------
		//������ĳߴ����϶�����ټ����ڶ����ڴ�������е�ƫ��λ��
        virtual uint32_t on_array_idx(uint32_t Size,uint32_t &blocksize)
        {
            if (Size <= cfg_t::MinNodeSize)
            {
                blocksize = cfg_t::MinNodeSize;
                return 0;
            }
            else
            {
                uint32_t offset = round_up(::log2(Size));
                blocksize = 1 << offset;
                uint32_t idx=(offset -cfg_t::MinNodeShiftBit);
                rx_assert(idx<FM_PoolCount);
                return idx;
            }
        }
		//-----------------------------------------------------
        virtual bool on_init()
        {
			m_pool_array=new pool_t[FM_PoolCount];
			for(uint32_t i=0;i<FM_PoolCount;i++)
        		if (!m_pool_array[i].do_init(cfg_t::MinNodeSize<<i))
                    return false;  
            return true;
        }
		//-----------------------------------------------------
        virtual void on_uninit()
        {
			for(uint32_t i=0;i<FM_PoolCount;i++)
				m_pool_array[i].do_uninit();
			delete [] m_pool_array;
        }

		//-----------------------------------------------------
		mempool_cntr_pow2 &operator =(const mempool_cntr_pow2 &);
		mempool_cntr_pow2(const mempool_cntr_pow2&);
	public:
		typedef cfg_t mem_cfg_t;
		//-----------------------------------------------------
		mempool_cntr_pow2(bool init=true){if (init) do_init();}
        virtual ~mempool_cntr_pow2() { do_uninit(); }
	};
}
#endif