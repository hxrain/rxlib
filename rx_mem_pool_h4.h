#ifndef _RX_MEM_POOL_H4_H_
#define _RX_MEM_POOL_H4_H_

#include "rx_cc_macro.h"
#include "rx_mem_pool.h"
#include "rx_ct_util.h"
#include "rx_assert.h"

namespace rx
{
	#define RX_USE_MEMPOOL_H4_CHECK 0
	//-------------------------------------------------------------------
	//基于内存区域的分配器.h4_mem_align_size告知最小内存对齐尺寸
	template<size_t h4_mem_align_size>
	class mempool_h4_raw
	{
		//记录内存块节点头
		typedef struct h4_block_head
		{
			struct h4_block_head *next;	                    //指向下一个可用块地址
			size_t size;						            //可用块的尺寸
		} h4_block_head;

		//内存块节点占用的空间(对齐后的)尺寸
		static const size_t h4_block_head_size = size_align<sizeof(h4_block_head), h4_mem_align_size>::result;
		//制作"使用中"掩码标记,在size_t类型变量的最高位
		static const size_t h4_node_using_mask = ((size_t)1) << ((sizeof(size_t) * 8) - 1);

		size_t              m_remain_size;                  //当前区域的剩余内存
		size_t              m_remain_size_low;              //剩余内存的最低水位

		h4_block_head       m_last_pos;                     //后趋记录当前区域剩余内存的起始位置
		h4_block_head       *m_end_ptr;                     //指向当前区域的最后结尾处

		void                *m_head_addr;                   //记录当前内存区域的开始地址
		void                *m_tail_addr;                   //记录当前内存区域的结束地址

		//----------------------------------------------------------------
		//将给定的自由块节点放入剩余空间,需要进行自由块节点的前向合并与后项合并处理.
		void m_merge_free_block(h4_block_head *free_block)
		{
			h4_block_head *curr_block = &m_last_pos;
			//从剩余空间的头部开始遍历查找,一直找到后趋在自由块之后的那个挂靠节点.
			while (curr_block->next < free_block)
				curr_block = curr_block->next;

			if (((uint8_t*)curr_block + curr_block->size) == (uint8_t*)free_block)
			{
				//如果挂靠节点与自由块是紧密相邻的那么就直接合并,自由块与挂靠节点相同了.
				curr_block->size += free_block->size;
				free_block = curr_block;
			}

			if (((uint8_t*)free_block + free_block->size) == (uint8_t*)curr_block->next)
			{
				//挂靠节点的后趋与自由块的后趋相同
				if (curr_block->next != m_end_ptr)
				{
					//挂靠节点的后趋不是结尾,并且和自由块的后趋相同了,那么就进行自由块与挂靠节点后趋的合并.
					free_block->size += curr_block->next->size;
					free_block->next = curr_block->next->next;
				}
				else
				{
					//挂靠节点的后趋就是结尾,那么让自由块的后趋也指向结尾.
					free_block->next = m_end_ptr;
				}
			}
			else
			{
				//挂靠节点的后趋和自由块的后趋不同,那么就让自由块的后趋指向挂靠节点的后趋,进行链表的插入动作.
				free_block->next = curr_block->next;
			}

			if (curr_block != free_block)
			{
				//当前节挂靠节点与自由块不同的时候,需要让挂靠节点的后趋指向自由块,与上面配合,最终完成自由块的插入操作.
				curr_block->next = free_block;
			}
		}
		//----------------------------------------------------------------
		//进行真正的内存分配操作
		void *m_alloc(size_t want_size)
		{
			if ((int)want_size<0 || want_size > m_remain_size)
				return NULL;

			//自由链表的头节点
			h4_block_head *free_head = &m_last_pos;
			//当前节点从头节点的后趋开始
			h4_block_head *curr_block = m_last_pos.next;
			//对自由链表进行遍历,查找足够大的那个块作为当前块
			while ((curr_block->size < want_size))
			{
				rx_assert(curr_block->next != curr_block);

				if (curr_block->next != NULL)
				{
					free_head = curr_block;                 //头节点指向当前节点
					curr_block = curr_block->next;          //当前节点移向后趋
				}
				else
					rx_assert(0);
			}

			//没有可用空间了,直接返回NULL
			if (curr_block == m_end_ptr)
				return NULL;

			rx_assert(free_head->next == curr_block);
			//真正的返回值,需要在当前块的基础上偏移让出节点尺寸
			void *ret = (void*)(((uint8_t*)free_head->next) + h4_block_head_size);

			//头节点的后趋指向当前节点的后趋,当前节点准备从自由链表中摘除
			free_head->next = curr_block->next;

			//如果剩余空间仍然足够大,那么就需要进行拆分处理,让剩余部分进入自由空间链表
			if ((curr_block->size - want_size) > (h4_block_head_size << 1))
			{
				//剩余块指向当前块跳过分配字节的空余处
				h4_block_head *remain_block = (h4_block_head *)(((uint8_t*)curr_block) + want_size);
				rx_assert((((size_t)remain_block) & (h4_mem_align_size - 1)) == 0);

				//设置剩余块尺寸,而不需要设置剩余块的后趋
				remain_block->size = curr_block->size - want_size;
				//调整当前块的真实尺寸
				curr_block->size = want_size;

				//对剩余块进行剩余空间处理
				m_merge_free_block(remain_block);
			}

			//剩余空间减量处理
			m_remain_size -= curr_block->size;

			//始终记录最低水位
			if (m_remain_size < m_remain_size_low)
				m_remain_size_low = m_remain_size;

			//处理当前块的尺寸标记
			curr_block->size |= h4_node_using_mask;
			//清空当前节点的后趋标记,完成摘除操作
			curr_block->next = NULL;

			return ret;
		}
	public:
		//---------------------------------------------------------------
		mempool_h4_raw() { memset(this, 0, sizeof(mempool_h4_raw)); } //构造函数进行内部状态清零
		//---------------------------------------------------------------
		//分配指定尺寸的内存
		void *malloc(uint32_t &block_size, uint32_t want_size)
		{
			if (want_size == 0 ||
				m_end_ptr == NULL ||
				want_size & h4_node_using_mask)
				return NULL;

			want_size += h4_block_head_size;                //期待的尺寸需要加上节点结构自身的尺寸

			//对待分配的尺寸数进行字节边界对齐
			want_size = size_alignx(want_size, h4_mem_align_size);
			block_size = want_size - h4_block_head_size;

			void *ret = m_alloc(want_size);
			rx_assert((((size_t)ret) & (h4_mem_align_size - 1)) == 0);
			check();
			return ret;
		}
		//----------------------------------------------------------------
		//释放指定的指针
		//返回值:0成功;其他错误
		int free(void *ptr)
		{
			if (!ptr)
				return 0;
			//从待释放的指针地址之前,得到该内存块的节点信息
			h4_block_head *block_head = (h4_block_head *)((char*)ptr - h4_block_head_size);

			//检查节点信息在正确性
			rx_assert((block_head->size & h4_node_using_mask) != 0);
			rx_assert(block_head->next == NULL);

			if ((block_head->size & h4_node_using_mask) == 0 ||
				block_head->next != NULL)
				return -1;

			//撤销节点信息中的块尺寸使用中标记位
			block_head->size &= ~h4_node_using_mask;

			//可用尺寸增量调整
			m_remain_size += block_head->size;
			//将当前节点放回可用空间
			m_merge_free_block(block_head);
			return check();
		}
		//----------------------------------------------------------------
		//判断是否被有效初始化过
		bool is_valid() { return m_end_ptr != NULL; }
		//----------------------------------------------------------------
		//判断给定的指针是否在本区域中.
		bool is_contain(void* ptr) { return (ptr >= m_head_addr) && (ptr <= m_tail_addr); }
		//----------------------------------------------------------------
		//获取剩余空间尺寸
		size_t remain(void) { return m_remain_size; }
		//----------------------------------------------------------------
		//获取剩余空间最低水位
		size_t remain_low(void) { return m_remain_size_low; }
		//----------------------------------------------------------------
		//进行堆区域初始化
		bool init(void* mem_addr, size_t mem_size)
		{
			if (!mem_addr ||
				mem_size < h4_mem_align_size * 64 ||
				m_tail_addr != NULL)
				return false;

			//进行工作区域首地址和可用尺寸的对齐处理
			size_t addr = (size_t)mem_addr;
			if ((addr & (h4_mem_align_size - 1)) != 0)
			{
				//向高址偏移对齐
				addr = size_alignx((size_t)mem_addr, h4_mem_align_size);
				//可用尺寸相应减少
				mem_size -= addr - (size_t)mem_addr;
			}

			//记录内存块的头地址
			m_head_addr = (void*)addr;

			//进行区域尾地址的对齐处理
			addr += mem_size - h4_block_head_size;
			addr &= ~((size_t)(h4_mem_align_size - 1));

			//让区域尾节点指针指向区域尾
			m_end_ptr = (h4_block_head *)addr;
			m_end_ptr->size = 0;
			m_end_ptr->next = NULL;

			//记录内存块尾地址
			m_tail_addr = m_end_ptr;

			//定义首块可用空间指针,指向区域首部,并填充为全部可用尺寸
			h4_block_head *first_block = (h4_block_head *)m_head_addr;
			first_block->size = addr - (size_t)first_block;
			//自由块链表的下一块指向区域尾节点;初始的时候,整个区域就只有一个可用块和一个尾节点.
			first_block->next = m_end_ptr;

			//m_last_pos节点利用后趋指向自由内存块链表的首项
			m_last_pos.next = first_block;
			m_last_pos.size = (size_t)0;

			//初始化记录剩余空间和最低水位
			m_remain_size_low = first_block->size;
			m_remain_size = first_block->size;

			return check() == 0;
		}
		//----------------------------------------------------------------
		//对整块区域进行扫描检查
		//返回值:0成功;其他失败
		int check()
		{
			#if !RX_USE_MEMPOOL_H4_CHECK
			return 0;
			#endif
			//当前节点从头节点的后趋开始
			h4_block_head *curr_block = (h4_block_head *)m_head_addr;
			//对自由链表进行遍历,查找足够大的那个块作为当前块
			while (curr_block < m_tail_addr)
			{
				//对整个区域进行扫描
				if (curr_block->next != NULL)
				{
					//碰到有后趋的节点头了,应该是未被使用过的块
					if (!is_contain(curr_block->next))
						return 1;                           //后趋指向的地址错误
					if (curr_block->next == curr_block)
						return 2;                           //后趋指向自身
					if (curr_block->size&h4_node_using_mask)
						return 3;                           //自由块的尺寸不能有使用中标识
					if (((size_t)curr_block->next - (size_t)curr_block) != curr_block->size)
					{
						//后趋指向的位置与长度不符,说明后面有一个被使用的块
						curr_block = (h4_block_head*)((uint8_t*)curr_block + curr_block->size);
						if ((curr_block->size&h4_node_using_mask) == 0)
							return 4;                       //被使用的块标志错误
						if (curr_block->next)
							return 5;                       //被使用的块还有后趋节点.
					}
					else
						curr_block = curr_block->next;      //移动到后趋节点
				}
				else
				{
					//后趋为空,说明是被使用中的块
					if ((curr_block->size&h4_node_using_mask) == 0)
						return 10;                          //使用中的块没有被正确标记
					//计算块尺寸,移动到后趋节点
					size_t size = curr_block->size&~h4_node_using_mask;
					curr_block = (h4_block_head*)((uint8_t*)curr_block + size);
					if (!is_contain(curr_block))
						return 11;                          //后趋范围错误
				}
			}
			return 0;
		}
	};

	//-----------------------------------------------------
	//总容量固定的h4内存池(静态占用缓冲区)
	template<class CT = mempool_cfg_t>
	class mempool_h4f_t :public mempool_i
	{
		mempool_h4_raw<CT::MinNodeSize>     m_pool;
		uint8_t     m_mem_buff[CT::StripeAlignSize];
	public:
		//-------------------------------------------------
		mempool_h4f_t() { do_init(); }
		//-------------------------------------------------
		virtual void *do_alloc(uint32_t &blocksize, uint32_t size) { return m_pool.malloc(blocksize, size); }
		virtual void do_free(void* ptr, uint32_t blocksize = 0) { m_pool.free(ptr); }
		virtual bool do_init(uint32_t size = 0) { return m_pool.init(m_mem_buff, sizeof(m_mem_buff)); }
		virtual void do_uninit(bool force = false) {}
	};

	//-----------------------------------------------------
	//总容量固定的h4内存池(动态分配缓冲区)
	template<class CT = mempool_cfg_t>
	class mempool_h4fx_t :public mempool_i
	{
		mempool_h4_raw<CT::MinNodeSize>     m_pool;
		uint8_t    *m_mem_buff;
	public:
		//-------------------------------------------------
		mempool_h4fx_t() :m_mem_buff(NULL) { do_init(CT::StripeAlignSize); }
		mempool_h4fx_t(uint32_t max_size) :m_mem_buff(NULL) { do_init(max_size); }
		~mempool_h4fx_t() { do_uninit(); }
		//-------------------------------------------------
		virtual void *do_alloc(uint32_t &blocksize, uint32_t size) { return m_pool.malloc(blocksize, size); }
		virtual void do_free(void* ptr, uint32_t blocksize = 0) { m_pool.free(ptr); }
		//-------------------------------------------------
		virtual bool do_init(uint32_t size = 0)
		{
			if (m_mem_buff) return true;
			if (!size) size = CT::StripeAlignSize;
			m_mem_buff = (uint8_t*)mempool_std_t<>::alloc(size);
			return m_pool.init(m_mem_buff, size);
		}
		virtual void do_uninit(bool force = false)
		{
			if (!m_mem_buff) return;
			mempool_std_t<>::free(m_mem_buff);
			m_mem_buff = NULL;
		}
	};
}

#endif
