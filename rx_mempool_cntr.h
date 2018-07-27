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
    //�ڴ������,�������Ե������ö���̶��ߴ��ڴ��,�γɶ�̬�ߴ�ķ�������
	template<class pool_t,class cfg_t= mempool_cfg_t>
	class mempool_cntr_lin:public mempool_i
	{
    private:
		pool_t     *m_pool_array;							//�̶��ߴ��ڴ������ָ��
		enum{FM_PoolCount=cfg_t::MaxNodeSize/cfg_t::MinAlignSize};//�����ڴ�����Ե�����������ߴ�
        //-----------------------------------------------------
		//������ĳߴ����϶�����ټ����ڶ����ڴ�������е�ƫ��λ��
        static uint32_t on_array_idx(uint32_t Size,uint32_t &blocksize)
        {
			blocksize=size_align_to(Size,cfg_t::MinAlignSize);
            return ((Size + (cfg_t::MinAlignSize - 1)) >> cfg_t::MinSizeShiftBit) - 1;
        }
		//-----------------------------------------------------
		//��ʼ���ڴ��:�̶�����ÿ������Ľڵ�����,��ճض�ʱʱ��
		bool m_init()
		{
			if (m_pool_array)
			{
				rx_alert("���ڴ���Ѿ���ʼ������!");
				return false;
			}
			if(cfg_t::MinAlignSize<8||cfg_t::MinAlignSize%4||cfg_t::MaxNodeSize%4||cfg_t::MaxNodeSize%cfg_t::MinAlignSize)
			{
				rx_alert("ʹ�ô���Ĳ������г�ʼ��!");
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
        //����ߴ�ΪSize���ڴ��
		void* do_alloc(uint32_t &blocksize,uint32_t Size)
		{
			if (Size<=cfg_t::MaxNodeSize)
			{//���ڲ��̶����������
                uint32_t idx = on_array_idx(Size, blocksize);
                rx_assert(idx<FM_PoolCount);
                return m_pool_array[idx].do_alloc(blocksize, blocksize);
			}
			else
			{//ʹ������ؽ��з���
				blocksize=Size;
				return mempool_std::do_alloc(Size);
			}
		}
		//-----------------------------------------------------
        //�ͷųߴ�ΪSize���ڴ��
		void do_free(void* P,uint32_t Size)
		{
			if (Size<=cfg_t::MaxNodeSize)
			{//�黹���ڲ��̶�������
				uint32_t blocksize;
				rx_assert(on_array_idx(Size,blocksize)<FM_PoolCount);
				m_pool_array[on_array_idx(Size,blocksize)].do_free(P,blocksize);
			}
			else
			{//�黹�������
				mempool_std::do_free(P,Size);
			}
		}
	};


    //---------------------------------------------------------
    //�ڴ������,����ָ���������ö���̶��ߴ��ڴ��,�γɶ�̬�ߴ�ķ�������
	template<class pool_t,class cfg_t= mempool_cfg_t>
	class mempool_cntr_pow2:public mempool_i
	{
    private:
		pool_t     *m_pool_array;							//�̶��ߴ��ڴ������ָ��
		enum{FM_PoolCount=cfg_t::MaxNodeSizeShiftBit-cfg_t::MinSizeShiftBit+1};//�����ڴ�����Ե�����������ߴ�
        //-----------------------------------------------------
		//������ĳߴ����϶�����ټ����ڶ����ڴ�������е�ƫ��λ��
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
		//��ʼ���ڴ��:�̶�����ÿ������Ľڵ�����,��ճض�ʱʱ��
		bool m_init()
		{
			if (m_pool_array)
			{
				rx_alert("���ڴ���Ѿ���ʼ������!");
				return false;
			}
			if(cfg_t::MinAlignSize<8||cfg_t::MinAlignSize%4||cfg_t::MaxNodeSize%4||cfg_t::MaxNodeSize%cfg_t::MinAlignSize)
			{
				rx_alert("ʹ�ô���Ĳ������г�ʼ��!");
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
        //����ߴ�ΪSize���ڴ��
		void* do_alloc(uint32_t &blocksize,uint32_t Size)
		{
			if (Size<=cfg_t::MaxNodeSize)
			{//���ڲ��̶����������
                uint32_t idx = on_array_idx(Size, blocksize);
				rx_assert(idx<FM_PoolCount);
				return m_pool_array[idx].do_alloc(blocksize,blocksize);
			}
			else
			{//ʹ������ؽ��з���
				blocksize=Size;
				return mempool_std::do_alloc(Size);
			}
		}
		//-----------------------------------------------------
        //�ͷųߴ�ΪSize���ڴ��
		void do_free(void* P,uint32_t Size)
		{
			if (Size<=cfg_t::MaxNodeSize)
			{//�黹���ڲ��̶�������
				uint32_t blocksize;
				rx_assert(on_array_idx(Size,blocksize)<FM_PoolCount);
				m_pool_array[on_array_idx(Size,blocksize)].do_free(P,blocksize);
			}
			else
			{//�黹�������
				mempool_std::do_free(P,Size);
			}
		}
	};
}
#endif