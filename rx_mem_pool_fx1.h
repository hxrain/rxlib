#ifndef _H_rx_mempool_H_
#define _H_rx_mempool_H_


#include "rx_cc_macro.h"
#include "rx_mem_pool.h"
#include "rx_raw_list.h"
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
        } mp_block_t;
        //-------------------------------------------------
        //ʹ��������ڴ���¼ÿ���ڴ�����ָ��
        typedef struct mp_stripes_t
        {
            enum {  total=(CT::MaxNodeSize-sizeof(void*)-sizeof(uint32_t))/sizeof(void*) };
            struct mp_stripes_t* volatile next;	            //�ڵ�ĺ���
            uint32_t  count;
            void *    ptrs[total];
        } mp_stripes_t;

        //-------------------------------------------------
        raw_stack_t<mp_block_t>   m_free_blocks;            //�����ڴ������
        raw_stack_t<mp_stripes_t> m_used_stripes;           //�Ѿ���������ڴ�������

        uint32_t         m_block_size;                      //ÿ���ڴ����õĿռ�ߴ�,��ʼȷ��,�����ٴθı�
        uint32_t         m_per_stripe_blocks;               //ÿ���ڴ����е��ڴ������,��ʼȷ��,�����ٴθı�
        //-------------------------------------------------
        //���ݳ�ʼ����,����һ���ڴ���
        bool m_alloc_stripe()
        {
            rx_assert(m_block_size&&m_per_stripe_blocks);

            uint32_t unset=0;
            uint8_t* new_stripe=(uint8_t*)base_alloc(unset,CT::StripeAlignSize);//���ô���������ڴ��,����һ���ڴ���
            rx_assert(new_stripe!=NULL);
            rx_static_assert(sizeof(mp_stripes_t)<=CT::MaxNodeSize);

            mp_stripes_t *sp=m_used_stripes.head();
            if (!sp||sp->count==sp->total)
            {
                //������ڵ��ڴ�����¼�鲻�ڻ�����,��������ڴ����м�¼
                sp=(mp_stripes_t*)base_alloc(unset,CT::MaxNodeSize);
                rx_assert(sp!=NULL);
                memset(sp,0,sizeof(mp_stripes_t));
                m_used_stripes.push_front(sp);
            }
            sp->ptrs[sp->count++]=new_stripe;

            for(uint32_t i=0; i<m_per_stripe_blocks; i++)   //�������еĸ����¿�������ɿ���
                m_free_blocks.push_front((mp_block_t*)&new_stripe[i*m_block_size]);
            return true;
        }
        //-------------------------------------------------
        //����һ��̶���С(NS)���ڴ��
        void* m_alloc_block()
        {
            if (m_free_blocks.head())
                return m_free_blocks.pop_front();           //ֱ�Ӵӿ��ÿ����л�ȡһ����
            if (!m_alloc_stripe())
                return NULL;                                //������·���һ��.����ʧ��˵�������ڴ治��
            return (void*)m_free_blocks.pop_front();        //�ٴγ��Դӿ��ÿ����л�ȡһ���鲢����
        }
        //-------------------------------------------------
        //������,�ͷ��ڲ����е��ڴ����������,���ڲ���Ϊ���������,�������¿�ʼ
        bool m_clear(bool Force)
        {
            m_free_blocks.pick();                           //������ɿ���

            while(m_used_stripes.size())
            {
                mp_stripes_t *sp=m_used_stripes.pop_front();
                for(uint32_t i=0; i<sp->count; ++i)
                    base_free(sp->ptrs[i]);
                base_free(sp);
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
        ~mempool_fixed_t() {do_uninit();}
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

            //��¼�ڴ��ߴ�
            m_block_size=BlockSize;
            //�������еĿ�����
            m_per_stripe_blocks=Max(CT::StripeAlignSize/m_block_size,(uint32_t)1);
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
            rx_assert(unused == 0 || unused == m_block_size);
            m_free_blocks.push_front((mp_block_t*)p);
        }
    };
}

#endif

