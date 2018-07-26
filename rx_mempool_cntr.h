#ifndef _RX_MEMPOOL_CNTR_H_
#define _RX_MEMPOOL_CNTR_H_

#include "rx_cc_macro.h"
#include "rx_mempool.h"

namespace rx
{
    //---------------------------------------------------------
    //�ڴ������,���Է��ö���̶��ߴ��ڴ��,�γɶ�̬�ߴ�ķ�������
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
		pool_t     *m_pool_array;                       //�̶��ߴ��ڴ������ָ��
		//-----------------------------------------------------
		//��ʼ���ڴ��:�̶�����ÿ������Ľڵ�����,��ճض�ʱʱ��
		bool m_init()
		{
			if (m_pool_array)
			{
				HAlert("���ڴ���Ѿ���ʼ������!");
				return false;
			}
			if(CT::MinAlignSize<8||CT::MinAlignSize%4||CT::MaxNodeSize%4||CT::MaxNodeSize%CT::MinAlignSize)
			{
				HAlert("ʹ�ô���Ĳ������г�ʼ��!");
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
        //����ߴ�ΪSize���ڴ��
		void* do_alloc(uint32_t Size)
		{
			if (Size<=CT::MaxNodeSize)
			{//���ڲ��̶����������
				rx_assert(on_array_idx(Size)<CT::PoolCount);
				//����ʹ���˶�̬����ƫ��,���Դ˹��̱Ⱦ�̬����ƫ��Ҫ��һ������
				//���·ǹ̶��رȹ̶��ص�Ч��Ҫ��һ������
				return m_pool_array[on_array_idx(Size)].do_alloc(Size);
			}
			else
			{//ʹ������ؽ��з���
				return rx_mem_pool_std::do_alloc(Size);
			}
		}
		//-----------------------------------------------------
        //�ͷųߴ�ΪSize���ڴ��
		void do_free(void* P,uint32_t Size)
		{
			if (Size<=CT::MaxNodeSize)
			{//�黹���ڲ��̶�������
				rx_assert(on_array_idx(Size)<CT::PoolCount);
				//����ʹ���˶�̬����ƫ��,���Դ˹��̱Ⱦ�̬����ƫ��Ҫ��һ������
				//���·ǹ̶��رȹ̶��ص�Ч��Ҫ��һ������
				m_pool_array[on_array_idx(Size)].do_free(P,Size);
			}
			else
			{//�黹�������
				rx_mem_pool_std::do_free(P,Size);
			}
		}
	};

}
#endif