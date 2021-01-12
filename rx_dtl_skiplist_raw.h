#ifndef _RX_RAW_SKIPLIST_H__
#define _RX_RAW_SKIPLIST_H__

#include "rx_cc_macro.h"
#include "rx_mem_alloc.h"
#include "rx_hash_rand.h"
#include <time.h>

namespace rx
{
	//ԭʼ��������㼶����
	#define RX_SKLIST_MAXLEVEL 32

	//����Ķ�����ָ������,��װ������ڵ�β����,���ʹ�������.
	//β��ָ�����ƫ��sizeof(key_t)�ֽڼ��ɷ�������������ڵ�
	typedef struct sk_block_t
	{
		sk_block_t *next[1];
	}sk_block_t;

	//ͳһָ��һ������ʹ�õ����������������
	typedef rand_skl_t skiplist_rnd_t;

	//-----------------------------------------------------
	//��������㼶������������ķ�װ
	template<class rnd_t, uint32_t MAX_LEVEL>
	class skiplist_rnd_level
	{
		rnd_t   m_rnd;
	public:
		static const uint32_t max_level() { return MAX_LEVEL; }
		skiplist_rnd_level(uint32_t seed)
		{
			if (seed < 1)
				seed = (uint32_t)time(NULL);
			m_rnd.seed(seed);
		}
		//-------------------------------------------------
		//����һ������Ĳ���:>=1;<=������
		uint32_t make()
		{
			uint32_t rc = 1;
			while (rc < MAX_LEVEL && (m_rnd.get() & 0xFFFFFF) < (0xFFFFFF >> 2))   //�����������С��25%��������(�൱��4����)
				++rc;
			rx_assert(rc <= MAX_LEVEL);
			return rc;
		}
	};

	//-----------------------------------------------------
	//����ԭʼ��������������.op��������Ҫ����cmp����
	//����ڵ���ڴ沼��Ϊ: rsv_size(key/val)|sizeof(sk_block_t)*level|ext_size
	template<class op>
	class raw_skiplist_t
	{
		uint32_t            m_count;                        //�ڲ��ڵ�����
		uint32_t            m_levels;                       //��ǰ�Ѿ�ʹ�õ���߲���
		sk_block_t         *m_head;                         //ָ��ͷ���
		sk_block_t         *m_tail;                         //ָ��β�ڵ�
		mem_allotter_i     &m_mem;                          //�ڴ�������ӿ�
		uint32_t            m_rsv_size;                     //�ڵ��ײ������ߴ�(��sk_block_t֮ǰ���ڼ�¼�ڵ�����)
		//-------------------------------------------------
		//���ݽڵ��β��ָ���ȡ�ڵ���ָ��
		void* node_by_block(const sk_block_t *block) const { return (uint8_t*)block - m_rsv_size; }
		//-------------------------------------------------
		//���ݸ����Ĳ�������չ�ߴ�,��̬�����ڵ�
		sk_block_t *m_make_node(uint32_t level, uint32_t rsv_size, uint32_t ext_size)
		{
			//����ڵ�β��ߴ�����Ҫ��չ�ĳߴ�
			uint32_t size = rsv_size + level * sizeof(sk_block_t*) + ext_size;
			//����ڵ��ڴ�,���ӽڵ�β�鿪ʼʹ��
			sk_block_t *block = (sk_block_t *)((uint8_t*)m_mem.alloc(size) + rsv_size);
			if (!block) return NULL;

			//�½ڵ����в�ĺ���,��ʼ��ΪNULL
			for (uint32_t lvl = 0; lvl < level; ++lvl)
				block->next[lvl] = NULL;

			return block;
		}
		//-------------------------------------------------
		//ժ��ָ���Ľڵ�,��������ǰ���ڵ�ĺ���ָ��
		void m_pick(sk_block_t *block, sk_block_t **update)
		{
			for (uint32_t lvl = 0; lvl < m_levels; ++lvl)
			{//�ӵ�������ժ������
				if (update[lvl]->next[lvl] == block)        //����ǰ�ڵ��ǰ��ָ�������
					update[lvl]->next[lvl] = block->next[lvl];
			}

			for (int32_t lvl = m_levels - 1; lvl >= 0; --lvl)
			{//�ٴӸ��������ж��Ƿ�Ӧ��������Ĳ���
				if (m_head->next[lvl] == NULL)
					--m_levels;                             //���ͷ���ĵ�ǰ�����ΪNULL,��˵��������������Ӧ�ý�����
				else
					break;
			}
		}
		//-------------------------------------------------
		//����ָ��key��ÿ���Ӧ��ǰ��,���ǵ�update��
		//����ֵ:key��Ӧ����ײ��ǰ���ڵ�
		template<class key_t>
		sk_block_t *m_prev(const key_t &key, sk_block_t **update)
		{
			sk_block_t  *block = m_head;                    //��ͷ�ڵ㿪ʼ������
			for (int32_t lvl = m_levels - 1; lvl >= 0; --lvl)
			{//����߲���㽵������,����skiplist���㷨�������
				while (block->next[lvl] &&                  //�����ǰ�ڵ��к���
					op::cmp(node_by_block(block->next[lvl]), key) < 0)     //���ҵ�ǰ�ڵ�ĺ���С��key(˵��key��Ӧ�������)
					block = block->next[lvl];				//��ǰ�ڵ����,׼����������
				update[lvl] = block;                        //��ǰ����ҽ�����,��¼��ǰ�ڵ�Ϊָ��keyλ�õ�ǰ��
			}
			return block;
		}
	public:
		//-------------------------------------------------
		raw_skiplist_t(mem_allotter_i &ma, uint32_t rsv_size, uint32_t head_levels = RX_SKLIST_MAXLEVEL) :m_mem(ma), m_rsv_size(rsv_size)
		{
			m_levels = 1;                                   //����ĳ�ʼ����ֻ��1��(����Ϊ0�Ļ�����)
			m_head = m_make_node(head_levels, 0, 0);        //ͷ���ֻ�����ڴ�,������
			rx_fail(m_head != NULL);
			m_tail = NULL;
			m_count = 0;
		}
		virtual ~raw_skiplist_t()
		{
			clear();
			m_mem.free(m_head);                             //�ͷ�ͷ����ڴ�,������
		}
		//-------------------------------------------------
		//��ǰ�����еĽڵ�����
		uint32_t size() const { return m_count; }
		//��ǰ�����������
		uint32_t levels() const { return m_levels; }
		//��ȡ��ǰ�����ͷ��β�ڵ�
		void* head() const { return m_head->next[0] == NULL ? NULL : node_by_block(m_head->next[0]); }
		void* tail() const { return m_tail == NULL ? NULL : node_by_block(m_tail); }
		//-------------------------------------------------
		//����ָ����key��Ӧ�Ľڵ�(key�����ظ�,����᷵��֮ǰ���ڵĽڵ�ָ��);��������ڵ�Ĳ���level;������չ���ڴ�ߴ�ext_size;
		//���صĽڵ�ָ�������Ϊ��,��duplication�ظ�ָʾΪ��,����Խ��нڵ�Ĺ����ʼ��(key��val��������ʼ��)
		template<class key_t>
		void *insert_raw(const key_t &key, bool &duplication, uint32_t ext_size, const uint32_t level)
		{
			sk_block_t *update[RX_SKLIST_MAXLEVEL];         //������ʱ��¼��ǰ�ڵ������,��Ӧ�ĸ���ǰ���ڵ�
			sk_block_t *prv = m_prev(key, update)->next[0]; //����ָ��key��Ӧ�ĸ���ǰ��,�����Եõ��Ѿ����ڵ�key�ڵ�

			if (prv&&op::cmp(node_by_block(prv), key) == 0)
			{
				duplication = true;                         //��֪key�Ѵ���
				return prv;                                 //ֱ�ӷ���
			}

			duplication = false;
			sk_block_t *block = m_make_node(level, m_rsv_size, ext_size);//��������ָ���������½ڵ�
			if (!block)
				return NULL;

			if (level > m_levels)
			{//����½ڵ�Ĳ�������ԭ�в���
				for (uint32_t lvl = m_levels; lvl < level; ++lvl)
					update[lvl] = m_head;                   //������������ǰ��Ϊͷ���
				m_levels = level;                           //���������
			}

			for (uint32_t lvl = 0; lvl < level; ++lvl)
			{//��ʼ���ҽ��½ڵ�
				block->next[lvl] = update[lvl]->next[lvl];  //�½ڵ�ĺ���ָ��ǰ���ڵ�ĺ���
				update[lvl]->next[lvl] = block;             //ǰ���ڵ�ĺ���ָ���½ڵ�
			}

			if (m_count++ == 0 || block->next[0] == NULL)   //��������ǿյ�,�������²���Ľڵ����β�ڵ�(û�к���)
				m_tail = block;                             //��β�ڵ�ָ���Ӧ��ָ�����½ڵ�

			return node_by_block(block);                    //���ؽڵ���ָ��
		}
		//-------------------------------------------------
		//ժ������key��Ӧ��һ���ڵ�,������������б�Ҫ����������.֮��������(remove_free�ͷ�)
		template<class key_t>
		void* remove_find(const key_t &key)
		{
			sk_block_t *update[RX_SKLIST_MAXLEVEL];         //������ʱ��¼��ǰ�ڵ����ʱ,��Ӧ�ĸ���ǰ���ڵ�
			sk_block_t *block = m_prev(key, update)->next[0];//����ָ��key�ڵ��Ӧ�ĸ���ǰ��,�����Եõ���Ӧ�Ľڵ�
			void *node = node_by_block(block);
			if (block && op::cmp(node, key) == 0)
			{//�����Ӧ�ڵ������key��ͬ,��˵���ҵ��˴˽ڵ�
				m_pick(block, update);                      //ժ��ָ���Ľڵ�(���нڵ����ǰ���ĵ���)
				if (block == m_tail)
					m_tail = update[0];                     //���ɾ���Ľڵ����β�ڵ�,��β�ڵ�ָ����ǰ��
				--m_count;                                  //�����������
				rx_assert_if(m_count == 0, m_tail == NULL);
				rx_assert_if(m_count != 0, m_tail != NULL);
				return node;
			}
			return NULL;
		}
		//-------------------------------------------------
		//�ͷŽڵ��ڴ�
		void remove_free(void* node)
		{
			rx_assert(node != NULL);
			m_mem.free(node);
		}
		//-------------------------------------------------
		//����ָ��key��Ӧ�Ľڵ�
		template<class key_t>
		void *find(const key_t &key) const
		{
			sk_block_t *block = m_head;                     //���׽ڵ㿪ʼ����
			for (int32_t lvl = m_levels - 1; lvl >= 0; --lvl)
			{//�Ӹ߲���ײ�������
				while (block->next[lvl] != NULL)
				{//���нڵ�ıȽ�
					void *node = node_by_block(block->next[lvl]);
					int cv = op::cmp(node, key);
					if (cv < 0)
						block = block->next[lvl];           //�ڵ�С��key,��Ҫ����������
					else if (cv == 0)
						return node;                        //key��ͬ,�ҵ���.
					else
						break;                              //�ڵ����key˵������鲻��,׼�������
				}
			}
			return NULL;                                    //ȫ���㼶�������,ȷʵû�ҵ�
		}
		//-------------------------------------------------
		//�������,��������ʹ��(�ϲ�Ӧ���Ƚ��нڵ�����������ٵ��ô˷���)
		void clear()
		{
			sk_block_t *block = m_head->next[0];            //��ͷ���ĺ���(�׸��ڵ�)��ʼ����
			while (block)
			{
				sk_block_t *next = block->next[0];          //��¼��ǰ�ڵ�ĺ���ָ��
				m_mem.free(node_by_block(block));           //�ڵ㲻����,ֱ���ͷ�
				block = next;                               //��ǰ�ڵ�ָ��ָ�������
			}

			for (uint32_t lvl = 0; lvl < m_levels; ++lvl)
				m_head->next[lvl] = NULL;                   //ͷ���ĺ���ȫ�����

			m_levels = 1;
			m_count = 0;
			m_tail = NULL;
		}
	};
}

#endif
