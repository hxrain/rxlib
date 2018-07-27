#ifndef _H_rx_mempool_H_
#define _H_rx_mempool_H_


#include "rx_cc_macro.h"
#include "rx_mempool.h"
#include "rx_raw_stack.h"
#include "rx_assert.h"


namespace rx
{
    //-----------------------------------------------------
    //�����ڴ�ػ���:CT�ڴ����������
    template<class CT=mempool_cfg_t>
    class mempool_fixed_t:public mempool_i
    {
    protected:
        //-------------------------------------------------
        //�ڴ��ڵ�ͷ�ṹ����,����ֱ��ʹ��
        typedef struct mp_block_t
		{
			struct mp_block_t* volatile next;	            //�ڵ�ĺ���,���ڴ�����ڴ����ʱ,��m_free_blocksʹ��
        }mp_block_t;
        //-------------------------------------------------
        //�ڴ����ڵ�ͷ�ṹ����,����ֱ��ʹ��
        typedef struct mp_stripe_t
        {
            struct mp_stripe_t* volatile next;	            //�ڵ�ĺ���
            mp_block_t  *BlockHead;                         //ƫ��ռλ
        }mp_stripe_t;
        //-------------------------------------------------
		raw_stack<mp_block_t>   m_free_blocks;              //�����ڴ������
        raw_stack<mp_stripe_t>  m_used_stripes;             //�Ѿ���������ڴ�������

        uint32_t         m_block_size;                      //ÿ���ڴ����õĿռ�ߴ�,��ʼȷ��,�����ٴθı�
        uint32_t         m_per_stripe_blocks;               //ÿ���ڴ����е��ڴ������,��ʼȷ��,�����ٴθı�
        uint32_t         m_stripe_size;                     //ÿ���ڴ�������ռ�õĿռ�,��ʼȷ��,�����ٴθı�
        //-------------------------------------------------
        //���ݳ�ʼ����,����һ���ڴ���
        bool m_alloc_stripe()
        {
            rx_assert(m_block_size&&m_per_stripe_blocks);
            uint32_t unset=0;
            mp_stripe_t* new_stripe=(mp_stripe_t*)base_alloc(unset,m_stripe_size);//���ô���������ڴ��,����һ���ڴ���

            if (new_stripe==NULL){rx_alert("out of memroy!");return false;}

            m_used_stripes.push(new_stripe);                //��¼���ڴ���ָ��,��Clearʹ��
            for(uint32_t i=0;i<m_per_stripe_blocks;i++)     //�������еĸ����¿�������ɿ���
                m_free_blocks.push((mp_block_t*)&((uint8_t*)&(new_stripe->BlockHead))[i*m_block_size]);
            return true;
        }
        //-------------------------------------------------
        //����һ��̶���С(NS)���ڴ��
        void* m_alloc_block()
        {
            if (m_free_blocks.peek())
                return m_free_blocks.pop();                 //ֱ�Ӵӿ��ÿ����л�ȡһ����
            if (!m_alloc_stripe()) return NULL;             //������·���һ��,����ʧ��˵�������ڴ治��
            return (void*)m_free_blocks.pop();              //�ٴγ��Դӿ��ÿ����л�ȡһ���鲢����
        }
        //-------------------------------------------------
        //������,�ͷ��ڲ����е��ڴ����������,���ڲ���Ϊ���������,�������¿�ʼ
        bool m_clear(bool Force)
        {
            //��ǿ��,�ڴ��δ��,��ô�Ͳ������,ֱ�ӷ���,��֪δ���
            if (!Force&&!is_full())
                return false;

            m_free_blocks.pick();                           //������ɿ���

            while(m_used_stripes.size())
            {
                mp_stripe_t *Stripe=m_used_stripes.pop();
                base_free(Stripe);
            }
            return true;
        }
        //-------------------------------------------------

		mempool_fixed_t &operator =(const mempool_fixed_t&);
		mempool_fixed_t(const mempool_fixed_t&);
	public:
		typedef CT mem_cfg_t;
        //-------------------------------------------------
        //���캯��:�ڴ��ߴ�
        mempool_fixed_t(uint32_t BlockSize=0):m_block_size(0),m_per_stripe_blocks(0)
        {
            if (BlockSize)
                do_init(BlockSize);
        }
        ~mempool_fixed_t(){do_uninit();}
        //-------------------------------------------------
        //���ڴ�ؽ��г�ʼ��:�ڴ��ߴ�
        virtual bool do_init(uint32_t BlockSize)
        {
            if (m_block_size||m_per_stripe_blocks)
            {
                rx_alert("repeat init!");
                return false;
            }
            if (BlockSize<sizeof(struct mp_block_t))
                BlockSize=sizeof(struct mp_block_t);        //�̶�����ÿ���ڵ����С�ߴ�Ϊ��Ҫ������ͷ

            m_block_size=BlockSize;                         //��¼��ߴ�
            //�������еĿ�����
            m_per_stripe_blocks=Max(CT::MaxStripeSize/m_block_size,(uint32_t)1);
            //������ʵ�����ߴ�:ʵ�ʵĿ���Ҫ�ĳߴ�*ÿ���еĿ���+��������ӵ�ͷ��Ҫ�ĳߴ�
            m_stripe_size=m_block_size*m_per_stripe_blocks+sizeof(mp_stripe_t*);
            return true;
        }
        //-------------------------------------------------
        //�����ʼ��
        virtual void do_uninit(bool Force=false)
        {
            if (!m_block_size)
                return;
            m_clear(Force);
            m_block_size=0;
            m_per_stripe_blocks=0;
        }
        //-------------------------------------------------
        //�ж�,���ڴ���Ƿ�������(���еĿ�ڵ㶼���ڲ�)
        //-------------------------------------------------
        bool is_full(){return m_used_stripes.size()*m_per_stripe_blocks==m_free_blocks.size();}
        //-------------------------------------------------
        //��ѯ�Ѿ���ʹ���˶��ٸ��ڴ��.
        uint32_t using_blocks()
        {
            return m_used_stripes.size()*m_per_stripe_blocks-m_free_blocks.size();
        }
		//-------------------------------------------------
		//����̶��ߴ���ڴ��
		virtual void* do_alloc(uint32_t &blocksize,uint32_t unused=0)
		{
			rx_assert(unused ==0|| unused ==m_block_size);
			blocksize=m_block_size;
            return m_alloc_block();
		}
		//-------------------------------------------------
		//�黹�ڴ�
		virtual void do_free(void* p, uint32_t unused = 0)
		{
			rx_assert(p!=NULL);
            rx_assert(!is_full());
            rx_assert(unused == 0 || unused == m_block_size);
            m_free_blocks.push((mp_block_t*)p);
		}
    };
}

#endif

