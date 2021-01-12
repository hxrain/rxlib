#ifndef _RX_DTL_SKIPSET_H_
#define _RX_DTL_SKIPSET_H_

#include "rx_cc_macro.h"
#include "rx_mem_alloc.h"
#include "rx_mem_alloc_cntr.h"
#include "rx_hash_rand.h"
#include "rx_dtl_skiplist_raw.h"
#include <time.h>
#include "rx_str_tiny.h"
#include "rx_ct_util.h"

/*����Ԫ��װʵ���˻���ԭʼ���������
	//-----------------------------------------------------
	//��������ļ�������
	template<class key_t,uint32_t MAX_LEVEL,class rnd_t=skiplist_rnd_t>
	class skipset_t;

	//����ʹ�õ�����������
	typedef skipset_t<uint32_t>          skipset_ut;
	typedef skipset_t<int32_t>           skipset_it;
	//����ʹ�õ�const char*��������
	typedef skipset_t<const char*>       skipset_ct;
	//����ʹ�õ�const wchar_t*��������
	typedef skipset_t<const wchar_t*>    skipset_wt;
*/

namespace rx
{
	//-----------------------------------------------------
	//��װ��������Ļ�������������
	//-----------------------------------------------------
	template<class key_t, uint32_t MAX_LEVEL = RX_SKLIST_MAXLEVEL, class rnd_t = skiplist_rnd_t>
	class skipset_t
	{
		#pragma pack(push,1)
		//-------------------------------------------------
		//skiplist���������ڵ�����,Ϊ�˱���gcc��Ƕ��ľ�ȷƫ�ػ�����,����һ������ռλ��dummy_t.
		template<class kt, typename dummy_t>
		struct skipset_node_t
		{
			//---------------------------------------------
			//�ȽϽڵ������key�Ĵ�С
			//����ֵ:n<keyΪ<0;n==keyΪ0;n>keyΪ>0
			template<class KT>
			static int cmp(const void *ptr, const KT &key)
			{
				const skipset_node_t &n = *(skipset_node_t*)ptr;
				return n.key == key ? 0 : (n.key < key ? -1 : 1);
			}
			//---------------------------------------------
			//����key��Ҫ����չ�ߴ�
			template<class KT>
			static int ext_size(const KT &k) { return 0; }

			//---------------------------------------------
			//���ж����첢��ʼ��
			template<class val_t>
			void OC(uint32_t level, val_t &val, uint32_t es) { ct::OC(&key, val); }
			//---------------------------------------------
			typedef kt node_key_t;
			node_key_t  key;
			sk_block_t  levels;                             //����ʵ�ֵĹؼ�:�ֲ�ĺ����ڵ�ָ��,������ڽڵ�����,���ڵ�����չ����
		};
		//-------------------------------------------------
		//����skipset��const char*/const wchar_t*���͵�key����ƫ�ػ�,�ڲ�ͳһ�����ڴ������key���ݼ�¼
		//����char*/wchar_t*��key����,�ӱ���ԭʼkeyָ������
		template<typename dummy_t>
		struct skipset_node_t<const char*, dummy_t >
		{
			//---------------------------------------------
			//�ȽϽڵ������key�Ĵ�С
			//����ֵ:n<keyΪ<0;n==keyΪ0;n>keyΪ>0
			template<class KT>
			static int cmp(const void *ptr, const KT &key)
			{
				const skipset_node_t &n = *(skipset_node_t*)ptr;
				return st::strcmp(n.key.c_str(), key);
			}
			//---------------------------------------------
			//����key��Ҫ����չ�ߴ�
			static int ext_size(const char* k) { return st::strlen(k) + 1; }

			//---------------------------------------------
			//���ж����첢��ʼ��
			template<class val_t>
			void OC(uint32_t level, val_t &val, uint32_t es)
			{
				ct::OC(&key);
				key.bind((char*)&levels.next[level], es, val, es - 1);
			}
			//---------------------------------------------
			typedef tiny_string_t<char> node_key_t;
			node_key_t  key;
			sk_block_t  levels;                             //����ʵ�ֵĹؼ�:�ֲ�ĺ����ڵ�ָ��,������ڽڵ�����,���ڵ�����չ����
		};
		//-------------------------------------------------
		template<typename dummy_t>
		struct skipset_node_t<const wchar_t*, dummy_t >
		{
			//---------------------------------------------
			//�ȽϽڵ������key�Ĵ�С
			//����ֵ:n<keyΪ<0;n==keyΪ0;n>keyΪ>0
			template<class KT>
			static int cmp(const void *ptr, const KT &key)
			{
				const skipset_node_t &n = *(skipset_node_t*)ptr;
				return st::strcmp(n.key.c_str(), key);
			}
			//---------------------------------------------
			//����key��Ҫ����չ�ߴ�(w��ռ�ÿռ��c����һ��)
			static int ext_size(const wchar_t* k) { return (st::strlen(k) + 1) * sc<wchar_t>::char_size(); }

			//---------------------------------------------
			//���ж����첢��ʼ��
			template<class val_t>
			void OC(uint32_t level, val_t &val, uint32_t es)
			{
				ct::OC(&key);
				uint32_t cap = es / sc<wchar_t>::char_size();
				key.bind((wchar_t*)&levels.next[level], cap, val, cap - 1);
			}
			//---------------------------------------------
			typedef tiny_string_t<wchar_t> node_key_t;
			node_key_t  key;
			sk_block_t  levels;                             //����ʵ�ֵĹؼ�:�ֲ�ĺ����ڵ�ָ��,������ڽڵ�����,���ڵ�����չ����
		};
		#pragma pack(pop)
		//-------------------------------------------------
		typedef skipset_node_t<key_t, void>  sk_node_t;     //��������ʹ�õĻ���ԭʼ���������ڵ�
		typedef raw_skiplist_t<sk_node_t>    sk_list_t;     //��������ʹ�õĻ���ԭʼ������������
		//-------------------------------------------------
		skiplist_rnd_level<rnd_t, MAX_LEVEL> m_rnd_level;   //������������
		sk_list_t                            m_raw_list;    //ԭʼ����������
	public:
		typedef sk_node_t node_t;
		//-------------------------------------------------
		skipset_t(mem_allotter_i &ma, uint32_t seed = 1) :m_rnd_level(seed), m_raw_list(ma, field_offset(sk_node_t, levels)) {}
		skipset_t(uint32_t seed = 1) :m_rnd_level(seed), m_raw_list(rx_global_mem_allotter(), field_offset(sk_node_t, levels)) {}
		virtual ~skipset_t() { clear(); }
		//-------------------------------------------------
		//����򵥵�ֻ��������
		class iterator
		{
			const node_t  *m_node;
			friend class skipset_t;
		public:
			//---------------------------------------------
			iterator() :m_node(NULL) {}
			iterator(const node_t *node) :m_node(node) {}
			iterator(const iterator &i) :m_node(i.m_node) {}
			//---------------------------------------------
			bool operator==(const iterator &i)const { return m_node == i.m_node; }
			bool operator!=(const iterator &i)const { return !(operator==(i)); }
			//---------------------------------------------
			iterator& operator=(const iterator &i) { m_node = i.m_node; return *this; }
			//---------------------------------------------
			const typename node_t::node_key_t& operator*() const { return m_node->key; }
			//---------------------------------------------
			iterator& operator++()
			{
				uint8_t *ptr = (uint8_t*)m_node->levels.next[0];
				m_node = (node_t*)(ptr ? ptr - field_offset(sk_node_t, levels) : NULL);
				return reinterpret_cast<iterator&>(*this);
			}
		};
		//-------------------------------------------------
		//�Ѿ�ʹ�õĽڵ�����
		uint32_t size() const { return m_raw_list.size(); }
		//-------------------------------------------------
		//׼����������,���ر����ĳ�ʼλ��
		iterator begin() const { return iterator((node_t*)m_raw_list.head()); }
		//-------------------------------------------------
		//���ر����Ľ���λ��
		iterator end() const { return iterator(NULL); }
		//-------------------------------------------------
		//��ȡ���ϵ�β�ڵ������(�����ڿ��ٻ�ȡβ�ڵ��ֵ)
		iterator rbegin() const { return iterator((node_t*)m_raw_list.tail()); }
		//-------------------------------------------------
		//ɾ��ָ��λ�õ�Ԫ��
		//����ֵ:�Ƿ�ɾ���˵�ǰֵ
		bool erase(iterator &i)
		{
			rx_assert(i.m_node != NULL);
			if (i.m_node == NULL)
				return false;
			const typename node_t::node_key_t &key = i.m_node->key;
			++i;
			return erase(key);
		}
		//-------------------------------------------------
		//�ڼ����в���Ԫ�ز���ֵ����
		//����ֵ:<0-�ڴ治��;0-ֵ�ظ�;>0-�ɹ�
		template<class val_t>
		int insert(const val_t &val)
		{
			uint32_t level = m_rnd_level.make();
			bool duplication = false;
			uint32_t es = node_t::ext_size(val);
			node_t *node = (node_t*)m_raw_list.insert_raw(val, duplication, es, level);
			if (duplication)
				return 0;
			if (!node)
				return -1;
			node->OC(level, val, es);
			return 1;
		}
		//-------------------------------------------------
		//����Ԫ���Ƿ����
		template<class val_t>
		bool find(const val_t &val) const { return m_raw_list.find(val) != NULL; }
		//-------------------------------------------------
		//ɾ��Ԫ�ز�Ĭ������
		template<class val_t>
		bool erase(const val_t &val)
		{
			node_t *node = (node_t*)m_raw_list.remove_find(val);
			if (!node) return false;
			ct::OD(node);
			m_raw_list.remove_free(node);
			return true;
		}
		//-------------------------------------------------
		//���ȫ����Ԫ��
		void clear()
		{
			for (iterator I = begin(); I != end(); ++I)
				ct::OD(I.m_node);
			m_raw_list.clear();
		}
	};

	//�﷨��,����һ������ʹ�õ�����������
	typedef skipset_t<uint32_t>          skipset_ut;
	typedef skipset_t<int32_t>           skipset_it;
	//�﷨��,����һ������ʹ�õ�const char*��������
	typedef skipset_t<const char*>       skipset_ct;
	//�﷨��,����һ������ʹ�õ�const wchar_t*��������
	typedef skipset_t<const wchar_t*>    skipset_wt;
}

#endif
