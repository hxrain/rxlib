#ifndef _H_rx_mempool_H_
#define _H_rx_mempool_H_


#include "rx_cc_macro.h"
#include "rx_mempool.h"
#include "rx_raw_stack.h"
#include "rx_assert.h"


namespace rx
{
    //-----------------------------------------------------
    //定长内存池基类:CT内存池配置类型
    template<class CT=mempool_cfg_t>
    class mempool_fixed_t:public mempool_i
    {
    protected:
        //-------------------------------------------------
        //内存块节点头结构描述,不可直接使用
        typedef struct mp_block_t
		{
			struct mp_block_t* volatile next;	            //节点的后趋,该内存块在内存池中时,被m_free_blocks使用
        }mp_block_t;
        //-------------------------------------------------
        //内存条节点头结构描述,不可直接使用
        typedef struct mp_stripe_t
        {
            struct mp_stripe_t* volatile next;	            //节点的后趋
            mp_block_t  *BlockHead;                         //偏移占位
        }mp_stripe_t;
        //-------------------------------------------------
		raw_stack<mp_block_t>   m_free_blocks;              //可用内存块链表
        raw_stack<mp_stripe_t>  m_used_stripes;             //已经分配出的内存条链表

        uint32_t         m_block_size;                      //每个内存块可用的空间尺寸,初始确定,不会再次改变
        uint32_t         m_per_stripe_blocks;               //每个内存条中的内存块数量,初始确定,不会再次改变
        uint32_t         m_stripe_size;                     //每个内存条真正占用的空间,初始确定,不会再次改变
        //-------------------------------------------------
        //根据初始参数,分配一个内存条
        bool m_alloc_stripe()
        {
            rx_assert(m_block_size&&m_per_stripe_blocks);
            uint32_t unset=0;
            mp_stripe_t* new_stripe=(mp_stripe_t*)base_alloc(unset,m_stripe_size);//利用传入的物理内存池,分配一个内存条

            if (new_stripe==NULL){rx_alert("out of memroy!");return false;}

            m_used_stripes.push(new_stripe);                //记录新内存条指针,给Clear使用
            for(uint32_t i=0;i<m_per_stripe_blocks;i++)     //将新条中的各个新块放入自由块链
                m_free_blocks.push((mp_block_t*)&((uint8_t*)&(new_stripe->BlockHead))[i*m_block_size]);
            return true;
        }
        //-------------------------------------------------
        //分配一块固定大小(NS)的内存块
        void* m_alloc_block()
        {
            if (m_free_blocks.peek())
                return m_free_blocks.pop();                 //直接从可用块链中获取一个块
            if (!m_alloc_stripe()) return NULL;             //否则就新分配一批,分配失败说明物理内存不足
            return (void*)m_free_blocks.pop();              //再次尝试从可用块链中获取一个块并返回
        }
        //-------------------------------------------------
        //清理函数,释放内部所有的内存条给物理池,将内部变为最初的样子,可以重新开始
        bool m_clear(bool Force)
        {
            //非强制,内存池未满,那么就不清除了,直接返回,告知未清除
            if (!Force&&!is_full())
                return false;

            m_free_blocks.pick();                           //清空自由块链

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
        //构造函数:内存块尺寸
        mempool_fixed_t(uint32_t BlockSize=0):m_block_size(0),m_per_stripe_blocks(0)
        {
            if (BlockSize)
                do_init(BlockSize);
        }
        ~mempool_fixed_t(){do_uninit();}
        //-------------------------------------------------
        //对内存池进行初始化:内存块尺寸
        virtual bool do_init(uint32_t BlockSize)
        {
            if (m_block_size||m_per_stripe_blocks)
            {
                rx_alert("repeat init!");
                return false;
            }
            if (BlockSize<sizeof(struct mp_block_t))
                BlockSize=sizeof(struct mp_block_t);        //固定池内每个节点的最小尺寸为需要包含块头

            m_block_size=BlockSize;                         //记录块尺寸
            //记算条中的块数量
            m_per_stripe_blocks=Max(CT::MaxStripeSize/m_block_size,(uint32_t)1);
            //计算真实的条尺寸:实际的块需要的尺寸*每条中的块数+额外的条接点头需要的尺寸
            m_stripe_size=m_block_size*m_per_stripe_blocks+sizeof(mp_stripe_t*);
            return true;
        }
        //-------------------------------------------------
        //解除初始化
        virtual void do_uninit(bool Force=false)
        {
            if (!m_block_size)
                return;
            m_clear(Force);
            m_block_size=0;
            m_per_stripe_blocks=0;
        }
        //-------------------------------------------------
        //判断,该内存池是否是满的(所有的块节点都在内部)
        //-------------------------------------------------
        bool is_full(){return m_used_stripes.size()*m_per_stripe_blocks==m_free_blocks.size();}
        //-------------------------------------------------
        //查询已经被使用了多少个内存块.
        uint32_t using_blocks()
        {
            return m_used_stripes.size()*m_per_stripe_blocks-m_free_blocks.size();
        }
		//-------------------------------------------------
		//分配固定尺寸的内存块
		virtual void* do_alloc(uint32_t &blocksize,uint32_t unused=0)
		{
			rx_assert(unused ==0|| unused ==m_block_size);
			blocksize=m_block_size;
            return m_alloc_block();
		}
		//-------------------------------------------------
		//归还内存
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

