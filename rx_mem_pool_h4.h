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
	//�����ڴ�����ķ�����.h4_mem_align_size��֪��С�ڴ����ߴ�
	template<size_t h4_mem_align_size>
	class mempool_h4_raw
	{
		//��¼�ڴ��ڵ�ͷ
		typedef struct h4_block_head
		{
			struct h4_block_head *next;	                    //ָ����һ�����ÿ��ַ
			size_t size;						            //���ÿ�ĳߴ�
		} h4_block_head;

		//�ڴ��ڵ�ռ�õĿռ�(������)�ߴ�
		static const size_t h4_block_head_size = size_align<sizeof(h4_block_head), h4_mem_align_size>::result;
		//����"ʹ����"������,��size_t���ͱ��������λ
		static const size_t h4_node_using_mask = ((size_t)1) << ((sizeof(size_t) * 8) - 1);

		size_t              m_remain_size;                  //��ǰ�����ʣ���ڴ�
		size_t              m_remain_size_low;              //ʣ���ڴ�����ˮλ

		h4_block_head       m_last_pos;                     //������¼��ǰ����ʣ���ڴ����ʼλ��
		h4_block_head       *m_end_ptr;                     //ָ��ǰ���������β��

		void                *m_head_addr;                   //��¼��ǰ�ڴ�����Ŀ�ʼ��ַ
		void                *m_tail_addr;                   //��¼��ǰ�ڴ�����Ľ�����ַ

		//----------------------------------------------------------------
		//�����������ɿ�ڵ����ʣ��ռ�,��Ҫ�������ɿ�ڵ��ǰ��ϲ������ϲ�����.
		void m_merge_free_block(h4_block_head *free_block)
		{
			h4_block_head *curr_block = &m_last_pos;
			//��ʣ��ռ��ͷ����ʼ��������,һֱ�ҵ����������ɿ�֮����Ǹ��ҿ��ڵ�.
			while (curr_block->next < free_block)
				curr_block = curr_block->next;

			if (((uint8_t*)curr_block + curr_block->size) == (uint8_t*)free_block)
			{
				//����ҿ��ڵ������ɿ��ǽ������ڵ���ô��ֱ�Ӻϲ�,���ɿ���ҿ��ڵ���ͬ��.
				curr_block->size += free_block->size;
				free_block = curr_block;
			}

			if (((uint8_t*)free_block + free_block->size) == (uint8_t*)curr_block->next)
			{
				//�ҿ��ڵ�ĺ��������ɿ�ĺ�����ͬ
				if (curr_block->next != m_end_ptr)
				{
					//�ҿ��ڵ�ĺ������ǽ�β,���Һ����ɿ�ĺ�����ͬ��,��ô�ͽ������ɿ���ҿ��ڵ�����ĺϲ�.
					free_block->size += curr_block->next->size;
					free_block->next = curr_block->next->next;
				}
				else
				{
					//�ҿ��ڵ�ĺ������ǽ�β,��ô�����ɿ�ĺ���Ҳָ���β.
					free_block->next = m_end_ptr;
				}
			}
			else
			{
				//�ҿ��ڵ�ĺ��������ɿ�ĺ�����ͬ,��ô�������ɿ�ĺ���ָ��ҿ��ڵ�ĺ���,��������Ĳ��붯��.
				free_block->next = curr_block->next;
			}

			if (curr_block != free_block)
			{
				//��ǰ�ڹҿ��ڵ������ɿ鲻ͬ��ʱ��,��Ҫ�ùҿ��ڵ�ĺ���ָ�����ɿ�,���������,����������ɿ�Ĳ������.
				curr_block->next = free_block;
			}
		}
		//----------------------------------------------------------------
		//�����������ڴ�������
		void *m_alloc(size_t want_size)
		{
			if ((int)want_size<0 || want_size > m_remain_size)
				return NULL;

			//���������ͷ�ڵ�
			h4_block_head *free_head = &m_last_pos;
			//��ǰ�ڵ��ͷ�ڵ�ĺ�����ʼ
			h4_block_head *curr_block = m_last_pos.next;
			//������������б���,�����㹻����Ǹ�����Ϊ��ǰ��
			while ((curr_block->size < want_size))
			{
				rx_assert(curr_block->next != curr_block);

				if (curr_block->next != NULL)
				{
					free_head = curr_block;                 //ͷ�ڵ�ָ��ǰ�ڵ�
					curr_block = curr_block->next;          //��ǰ�ڵ��������
				}
				else
					rx_assert(0);
			}

			//û�п��ÿռ���,ֱ�ӷ���NULL
			if (curr_block == m_end_ptr)
				return NULL;

			rx_assert(free_head->next == curr_block);
			//�����ķ���ֵ,��Ҫ�ڵ�ǰ��Ļ�����ƫ���ó��ڵ�ߴ�
			void *ret = (void*)(((uint8_t*)free_head->next) + h4_block_head_size);

			//ͷ�ڵ�ĺ���ָ��ǰ�ڵ�ĺ���,��ǰ�ڵ�׼��������������ժ��
			free_head->next = curr_block->next;

			//���ʣ��ռ���Ȼ�㹻��,��ô����Ҫ���в�ִ���,��ʣ�ಿ�ֽ������ɿռ�����
			if ((curr_block->size - want_size) > (h4_block_head_size << 1))
			{
				//ʣ���ָ��ǰ�����������ֽڵĿ��ദ
				h4_block_head *remain_block = (h4_block_head *)(((uint8_t*)curr_block) + want_size);
				rx_assert((((size_t)remain_block) & (h4_mem_align_size - 1)) == 0);

				//����ʣ���ߴ�,������Ҫ����ʣ���ĺ���
				remain_block->size = curr_block->size - want_size;
				//������ǰ�����ʵ�ߴ�
				curr_block->size = want_size;

				//��ʣ������ʣ��ռ䴦��
				m_merge_free_block(remain_block);
			}

			//ʣ��ռ��������
			m_remain_size -= curr_block->size;

			//ʼ�ռ�¼���ˮλ
			if (m_remain_size < m_remain_size_low)
				m_remain_size_low = m_remain_size;

			//����ǰ��ĳߴ���
			curr_block->size |= h4_node_using_mask;
			//��յ�ǰ�ڵ�ĺ������,���ժ������
			curr_block->next = NULL;

			return ret;
		}
	public:
		//---------------------------------------------------------------
		mempool_h4_raw() { memset(this, 0, sizeof(mempool_h4_raw)); } //���캯�������ڲ�״̬����
		//---------------------------------------------------------------
		//����ָ���ߴ���ڴ�
		void *malloc(uint32_t &block_size, uint32_t want_size)
		{
			if (want_size == 0 ||
				m_end_ptr == NULL ||
				want_size & h4_node_using_mask)
				return NULL;

			want_size += h4_block_head_size;                //�ڴ��ĳߴ���Ҫ���Ͻڵ�ṹ����ĳߴ�

			//�Դ�����ĳߴ��������ֽڱ߽����
			want_size = size_alignx(want_size, h4_mem_align_size);
			block_size = want_size - h4_block_head_size;

			void *ret = m_alloc(want_size);
			rx_assert((((size_t)ret) & (h4_mem_align_size - 1)) == 0);
			check();
			return ret;
		}
		//----------------------------------------------------------------
		//�ͷ�ָ����ָ��
		//����ֵ:0�ɹ�;��������
		int free(void *ptr)
		{
			if (!ptr)
				return 0;
			//�Ӵ��ͷŵ�ָ���ַ֮ǰ,�õ����ڴ��Ľڵ���Ϣ
			h4_block_head *block_head = (h4_block_head *)((char*)ptr - h4_block_head_size);

			//���ڵ���Ϣ����ȷ��
			rx_assert((block_head->size & h4_node_using_mask) != 0);
			rx_assert(block_head->next == NULL);

			if ((block_head->size & h4_node_using_mask) == 0 ||
				block_head->next != NULL)
				return -1;

			//�����ڵ���Ϣ�еĿ�ߴ�ʹ���б��λ
			block_head->size &= ~h4_node_using_mask;

			//���óߴ���������
			m_remain_size += block_head->size;
			//����ǰ�ڵ�Żؿ��ÿռ�
			m_merge_free_block(block_head);
			return check();
		}
		//----------------------------------------------------------------
		//�ж��Ƿ���Ч��ʼ����
		bool is_valid() { return m_end_ptr != NULL; }
		//----------------------------------------------------------------
		//�жϸ�����ָ���Ƿ��ڱ�������.
		bool is_contain(void* ptr) { return (ptr >= m_head_addr) && (ptr <= m_tail_addr); }
		//----------------------------------------------------------------
		//��ȡʣ��ռ�ߴ�
		size_t remain(void) { return m_remain_size; }
		//----------------------------------------------------------------
		//��ȡʣ��ռ����ˮλ
		size_t remain_low(void) { return m_remain_size_low; }
		//----------------------------------------------------------------
		//���ж������ʼ��
		bool init(void* mem_addr, size_t mem_size)
		{
			if (!mem_addr ||
				mem_size < h4_mem_align_size * 64 ||
				m_tail_addr != NULL)
				return false;

			//���й��������׵�ַ�Ϳ��óߴ�Ķ��봦��
			size_t addr = (size_t)mem_addr;
			if ((addr & (h4_mem_align_size - 1)) != 0)
			{
				//���ַƫ�ƶ���
				addr = size_alignx((size_t)mem_addr, h4_mem_align_size);
				//���óߴ���Ӧ����
				mem_size -= addr - (size_t)mem_addr;
			}

			//��¼�ڴ���ͷ��ַ
			m_head_addr = (void*)addr;

			//��������β��ַ�Ķ��봦��
			addr += mem_size - h4_block_head_size;
			addr &= ~((size_t)(h4_mem_align_size - 1));

			//������β�ڵ�ָ��ָ������β
			m_end_ptr = (h4_block_head *)addr;
			m_end_ptr->size = 0;
			m_end_ptr->next = NULL;

			//��¼�ڴ��β��ַ
			m_tail_addr = m_end_ptr;

			//�����׿���ÿռ�ָ��,ָ�������ײ�,�����Ϊȫ�����óߴ�
			h4_block_head *first_block = (h4_block_head *)m_head_addr;
			first_block->size = addr - (size_t)first_block;
			//���ɿ��������һ��ָ������β�ڵ�;��ʼ��ʱ��,���������ֻ��һ�����ÿ��һ��β�ڵ�.
			first_block->next = m_end_ptr;

			//m_last_pos�ڵ����ú���ָ�������ڴ�����������
			m_last_pos.next = first_block;
			m_last_pos.size = (size_t)0;

			//��ʼ����¼ʣ��ռ�����ˮλ
			m_remain_size_low = first_block->size;
			m_remain_size = first_block->size;

			return check() == 0;
		}
		//----------------------------------------------------------------
		//�������������ɨ����
		//����ֵ:0�ɹ�;����ʧ��
		int check()
		{
			#if !RX_USE_MEMPOOL_H4_CHECK
			return 0;
			#endif
			//��ǰ�ڵ��ͷ�ڵ�ĺ�����ʼ
			h4_block_head *curr_block = (h4_block_head *)m_head_addr;
			//������������б���,�����㹻����Ǹ�����Ϊ��ǰ��
			while (curr_block < m_tail_addr)
			{
				//�������������ɨ��
				if (curr_block->next != NULL)
				{
					//�����к����Ľڵ�ͷ��,Ӧ����δ��ʹ�ù��Ŀ�
					if (!is_contain(curr_block->next))
						return 1;                           //����ָ��ĵ�ַ����
					if (curr_block->next == curr_block)
						return 2;                           //����ָ������
					if (curr_block->size&h4_node_using_mask)
						return 3;                           //���ɿ�ĳߴ粻����ʹ���б�ʶ
					if (((size_t)curr_block->next - (size_t)curr_block) != curr_block->size)
					{
						//����ָ���λ���볤�Ȳ���,˵��������һ����ʹ�õĿ�
						curr_block = (h4_block_head*)((uint8_t*)curr_block + curr_block->size);
						if ((curr_block->size&h4_node_using_mask) == 0)
							return 4;                       //��ʹ�õĿ��־����
						if (curr_block->next)
							return 5;                       //��ʹ�õĿ黹�к����ڵ�.
					}
					else
						curr_block = curr_block->next;      //�ƶ��������ڵ�
				}
				else
				{
					//����Ϊ��,˵���Ǳ�ʹ���еĿ�
					if ((curr_block->size&h4_node_using_mask) == 0)
						return 10;                          //ʹ���еĿ�û�б���ȷ���
					//�����ߴ�,�ƶ��������ڵ�
					size_t size = curr_block->size&~h4_node_using_mask;
					curr_block = (h4_block_head*)((uint8_t*)curr_block + size);
					if (!is_contain(curr_block))
						return 11;                          //������Χ����
				}
			}
			return 0;
		}
	};

	//-----------------------------------------------------
	//�������̶���h4�ڴ��(��̬ռ�û�����)
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
	//�������̶���h4�ڴ��(��̬���仺����)
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
