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
		virtual bool do_init(uint32_t size=0)
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
		virtual void do_uninit(bool force=false)
		{
			if (!m_pool_array) return;
            on_uninit();
            m_pool_array=NULL;
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
    private:
        typedef mempool_cntr_base<pool_t,cfg_t> parent_t;
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
    //�ڴ������,ָ������
	template<class pool_t,class cfg_t= mempool_cfg_t>
	class mempool_cntr_pow2:public mempool_cntr_base<pool_t,cfg_t>
	{
    private:
        typedef mempool_cntr_base<pool_t,cfg_t> parent_t;
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
                rx_static_assert((!!0)==0);
                rx_static_assert((!!3) == 1);                   //Ҫ���߼�ȡ�����ֵΪ0��1

                uint32_t offset = rx_fls(Size)-1;               //���ٲ���Size�ĸ߱���λ��,���Ե�����log2(Size)
                blocksize = 1 << offset;                        //�ȼ���һ��Size��Ӧ��pow2(offset)����
                uint32_t bit_adj = (uint32_t)!!(blocksize^Size);//�����ֵ�����߼�������,����Ϊ���ж��Ƿ�Size����pow2(offset)
                blocksize <<= bit_adj;                          //�������յĳߴ�У��
                offset += bit_adj;                              //�������յ�ƫ��У��

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
    //�ڴ������,����bitӳ��
	template<class pool_t,class cfg_t= mempool_cfg_t,uint32_t SLI_COUNT=8>
	class mempool_cntr_tlmap:public mempool_cntr_base<pool_t,cfg_t>
	{
    private:
        typedef mempool_cntr_base<pool_t,cfg_t> parent_t;
        //---------------------------------------------------------
        //����bitmap����������޶�����
        typedef struct tlmap_cfg_t
        {
            enum {
                MIN_ALIGN  = cfg_t::MinNodeSize,            //��С����ߴ�
                FLI_MAX    = log2<cfg_t::MaxNodeSize>::result,//һ�������������
                SLI_MAX    = SLI_COUNT,                     //���������������

                SLI_SHIFT  = rx::log2<SLI_MAX>::result,     //���������ı�������
                FLI_OFFSET = rx::log2<MIN_ALIGN>::result,   //һ��������ƫ����
            };
        }tlmap_cfg_t;
        enum{FM_PoolCount=tlmap_cfg_t::FLI_MAX*tlmap_cfg_t::SLI_MAX};//�����ڴ������ߴ�
        //-----------------------------------------------------
		//������ĳߴ����϶�����ټ����ڶ����ڴ�������е�ƫ��λ��
        virtual uint32_t on_array_idx(uint32_t Size,uint32_t &blocksize)
        {
            //���ݸ��ĵ�Size����ӳ��,�õ�����������λ��(�Ѿ����ϵ�����)
            return rx_hash_tlmap_ex<tlmap_cfg_t>(Size,blocksize);         
        }
		//-----------------------------------------------------
        virtual bool on_init()
        {
			parent_t::m_pool_array=new pool_t[FM_PoolCount];
            for(uint32_t f=0;f<tlmap_cfg_t::FLI_MAX;++f)
                for(uint32_t s=0;s<tlmap_cfg_t::SLI_MAX;++s)
                {
                    //����ƫ�Ƽ���
                    uint32_t idx=f*tlmap_cfg_t::SLI_MAX+s;
                    //��Ӧ�ڴ�صĳߴ����
                    uint32_t size=1<<(f+tlmap_cfg_t::FLI_OFFSET);
                    size+=(size>>tlmap_cfg_t::SLI_SHIFT)*s;
                    //��ʼ����Ӧ���ڴ��
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
