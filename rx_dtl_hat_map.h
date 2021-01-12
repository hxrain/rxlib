#ifndef _RX_DTL_HAT_MAP_H_
#define _RX_DTL_HAT_MAP_H_

#include "rx_cc_macro.h"
#include "rx_dtl_hat_raw.h"
#include "rx_str_util_std.h"

/*
	<�䳤��̬��ϣmap����,���ڽ��չ�ϣ��>

	//hat����map��������
	template<class key_t, class val_t, class hat_op = hat_op_t>
	class hatmap_base_t;

	//ʹ�ù̶��ڴ�Ľ��չ�ϣmap(int:int)
	template<uint32_t max_slot_count, class key_t = uint32_t, class val_t = uint32_t, class cmp_t = hat_op_t>
	class hatmap_nft;

	//ʹ�ö�̬�ڴ�����ݵĽ��չ�ϣmap(int:int)
	template<class key_t = uint32_t, class val_t = uint32_t, class cmp_t = hat_op_t>
	class hatmap_nt;

	//ʹ�ù̶��ڴ�Ľ��չ�ϣmap(char*:int)
	template<uint32_t max_slot_count, class key_t = char, uint16_t mean_cnt = 12, class val_t = uint32_t, class cmp_t = hat_op_t>
	class hatmap_sft;

	//ʹ�ö�̬�ڴ�����ݵĽ��չ�ϣmap(char*:int)
	template<class key_t = char, class val_t = uint32_t, class cmp_t = hat_op_t>
	class hatmap_st;

	//ʹ�ù̶��ڴ�Ľ��չ�ϣset(int)
	template<uint32_t max_slot_count, class key_t = uint32_t, class cmp_t = hat_op_t>
	class hatset_nft;

	//ʹ�ö�̬�ڴ�����ݵĽ��չ�ϣset(int)
	template<class key_t = uint32_t, class cmp_t = hat_op_t>
	class hatset_nt;

	//ʹ�ù̶��ڴ�Ľ��չ�ϣset(char*)
	template<uint32_t max_slot_count, class key_t = char, uint16_t mean_cnt = 12, class cmp_t = hat_op_t>
	class hatset_sft;

	//ʹ�ö�̬�ڴ�����ݵĽ��չ�ϣset(char*)
	template<class key_t = char, class cmp_t = hat_op_t>
	class hatset_st;

*/

namespace rx
{
	//-----------------------------------------------------
	//hat����map��������
	template<class key_t, class val_t, class hat_op = hat_op_t>
	class hatmap_base_t
	{
	public:
		typedef hat_raw_t<key_t, val_t, hat_op>	cntr_t;		//����ײ���������

	protected:
		cntr_t	&m_cntr;
		//-------------------------------------------------
		hatmap_base_t(cntr_t &cntr) :m_cntr(cntr) {}
		virtual ~hatmap_base_t() {}
	public:
		//-------------------------------------------------
		uint32_t size() { return m_cntr.size(); }
		//-------------------------------------------------
		//������
		class iterator
		{
			const cntr_t		*m_parent;
			uint16_t			m_idx;
			friend class hatmap_base_t;
		public:
			//---------------------------------------------
			iterator() :m_parent(NULL), m_idx(-1) {}
			iterator(const iterator &i) :m_parent(i.m_parent), m_idx(i.m_idx) {}
			iterator(const cntr_t &p, uint16_t idx) :m_parent(&p), m_idx(idx) {}
			//---------------------------------------------
			bool operator==(const iterator &i)const { return m_parent == i.m_parent && m_idx == i.m_idx; }
			bool operator!=(const iterator &i)const { return !(operator==(i)); }
			//---------------------------------------------
			iterator& operator=(const iterator &i) { m_parent = i.m_parent; m_idx = i.m_idx; return *this; }
			//---------------------------------------------
			//*�������������,���ڻ�ȡ��ǰ�ڵ��valֵ
			const val_t* operator*() const
			{
				rx_assert(m_parent != NULL&&m_idx != -1);
				typename cntr_t::keyoff_t &ko = m_parent->offset(m_idx);
				return m_parent->value(ko);
			}
			const val_t& value() const
			{
				rx_assert(m_parent != NULL&&m_idx != -1);
				typename cntr_t::keyoff_t &ko = m_parent->offset(m_idx);
				return *m_parent->value(ko);
			}
			//---------------------------------------------
			//()���������,���ڻ�ȡ��ǰ�ڵ��keyֵ
			const key_t* operator()() const
			{
				rx_assert(m_parent != NULL&&m_idx != -1);
				typename cntr_t::keyoff_t &ko = m_parent->offset(m_idx);
				return m_parent->key(ko);
			}
			//---------------------------------------------
			//�ڵ�ָ�����(ǰ�������ģʽ,δ�ṩ����ģʽ)
			iterator& operator++()
			{
				rx_assert(m_parent != NULL&&m_idx != -1);
				m_idx = m_parent->next(m_idx);
				return reinterpret_cast<iterator&>(*this);
			}
		};
		//-------------------------------------------------
		//׼��������ϣ��,���ر����ĳ�ʼλ��
		iterator begin() const
		{
			if (m_cntr.offset(0).offset)
				return iterator(m_cntr, 0);
			else
				return iterator(m_cntr, m_cntr.next(0));
		}
		//-------------------------------------------------
		//���ر����Ľ���λ��
		iterator end() const { return iterator(m_cntr, m_cntr.capacity()); }
		//-------------------------------------------------
		//����Ԫ����ֵ,������dup�����ֶ�������ظ���ʱ��,val���ᱻʹ��
		template<class KT>
		iterator insert(const KT *key, uint16_t key_cnt, const val_t *val, bool *dup = NULL)
		{
			bool exist = false;
			if (dup == NULL)
				dup = &exist;

			uint16_t idx = m_cntr.push(key, key_cnt, dup);
			if (idx == m_cntr.capacity())
				return end();
			if (*dup)
				return iterator(m_cntr, idx);

			if (val)
			{
				uint16_t val_cnt = m_cntr.value_cnt();
				rx_assert(val_cnt != 0);

				val_t *vp = m_cntr.value(m_cntr.offset(idx));
				rx_assert(vp != NULL);
				for (uint16_t i = 0; i < val_cnt; ++i)
					vp[i] = val[i];
			}

			return iterator(m_cntr, idx);
		}
		template<class KT>
		iterator insert(const KT *key, const val_t *val = NULL, bool *dup = NULL)
		{
			return insert(key, st::strlen(key), val, dup);
		}
		template<class KT>
		iterator insert(const KT *key, const val_t &val, bool *dup = NULL)
		{
			rx_assert(m_cntr.value_cnt() == 1);
			return insert(key, st::strlen(key), &val, dup);
		}
		template<class KT>
		iterator insert(const KT &key, const val_t *val = NULL, bool *dup = NULL)
		{
			return insert(&key, 1, val, dup);
		}
		template<class KT>
		hatmap_base_t& operator()(const KT *key, const val_t &val)
		{
			rx_assert(m_cntr.value_cnt() == 1);
			if (insert(key, val) == end())
				rx_alert("hatmap space is not enough.");
			return *this;
		}
		template<class KT>
		hatmap_base_t& operator()(const KT &key, const val_t *val = NULL)
		{
			if (insert(key, val) == end())
				rx_alert("hatmap space is not enough.");
			return *this;
		}
		//-------------------------------------------------
		//����Ԫ��,ͨ�����ص������Ƿ���end()��ͬ�ж��Ƿ����
		template<class KT>
		iterator find(const KT *key, uint16_t key_cnt) const
		{
			uint16_t idx = m_cntr.find(key, key_cnt);
			return iterator(m_cntr, idx);
		}
		template<class KT>
		iterator find(const KT *key) const
		{
			uint16_t idx = m_cntr.find(key, st::strlen(key));
			return iterator(m_cntr, idx);
		}
		template<class KT>
		iterator find(const KT &key) const
		{
			uint16_t idx = m_cntr.find(key, 1);
			return iterator(m_cntr, idx);
		}
		template<class KT>
		iterator operator[](const KT *key) const { return find(key); }
		template<class KT>
		iterator operator[](const KT &key) const { return find(key); }
		//-------------------------------------------------
		//��ָ������ǰ׺���в���(��Ҫ����sort()����֮��)
		template<class KT>
		iterator prefix(const KT *key, uint16_t pre_cnt) const
		{
			uint16_t idx = m_cntr.prefix(key, pre_cnt);
			return iterator(m_cntr, idx);
		}
		//-------------------------------------------------
		//��������������
		void sort() { m_cntr.sort(); }
		//-------------------------------------------------
		//���ȫ����Ԫ��
		void clear() { m_cntr.clear(); }
	};



	//-----------------------------------------------------
	//ʹ�ù̶��ڴ�Ľ��չ�ϣmap(int:int)
	//-----------------------------------------------------
	template<uint32_t max_slot_count, class key_t = uint32_t, class val_t = uint32_t, class cmp_t = hat_op_t>
	class hatmap_nft :public hatmap_base_t<key_t, val_t, cmp_t>
	{
		typedef hatmap_base_t<key_t, val_t, cmp_t> super_t;
		typedef hat_ft<max_slot_count, key_t, 1, val_t, 1, cmp_t> cntr_t;
		cntr_t	m_cnt;
	public:
		~hatmap_nft() { super_t::clear(); }
		hatmap_nft() :super_t(m_cnt) {}
	};
	//-----------------------------------------------------
	//ʹ�ö�̬�ڴ�����ݵĽ��չ�ϣmap(int:int)
	//-----------------------------------------------------
	template<class key_t = uint32_t, class val_t = uint32_t, class cmp_t = hat_op_t>
	class hatmap_nt :public hatmap_base_t<key_t, val_t, cmp_t>
	{
		typedef hatmap_base_t<key_t, val_t, cmp_t> super_t;
		typedef hat_t<key_t, val_t, cmp_t> cntr_t;
		cntr_t	m_cnt;
	public:
		~hatmap_nt() { m_cnt.uninit(); }
		hatmap_nt(mem_allotter_i &mem) :m_cnt(mem), super_t(m_cnt) {}
		hatmap_nt() :super_t(m_cnt) {}
		bool init(uint16_t caps) { return m_cnt.init(caps, 1, 1); }
		void uninit() { m_cnt.uninit(); }
	};
	//-----------------------------------------------------
	//ʹ�ù̶��ڴ�Ľ��չ�ϣmap(char*:int)
	//-----------------------------------------------------
	template<uint32_t max_slot_count, class key_t = char, uint16_t mean_cnt = 12, class val_t = uint32_t, class cmp_t = hat_op_t>
	class hatmap_sft :public hatmap_base_t<key_t, val_t, cmp_t>
	{
		typedef hatmap_base_t<key_t, val_t, cmp_t> super_t;
		typedef hat_ft<max_slot_count, key_t, mean_cnt, val_t, 1, cmp_t> cntr_t;
		cntr_t	m_cnt;
	public:
		~hatmap_sft() { super_t::clear(); }
		hatmap_sft() :super_t(m_cnt) {}
	};
	//-----------------------------------------------------
	//ʹ�ö�̬�ڴ�����ݵĽ��չ�ϣmap(char*:int)
	//-----------------------------------------------------
	template<class key_t = char, class val_t = uint32_t, class cmp_t = hat_op_t>
	class hatmap_st :public hatmap_base_t<key_t, val_t, cmp_t>
	{
		typedef hatmap_base_t<key_t, val_t, cmp_t> super_t;
		typedef hat_t<key_t, val_t, cmp_t> cntr_t;
		cntr_t	m_cnt;
	public:
		~hatmap_st() { m_cnt.uninit(); }
		hatmap_st(mem_allotter_i &mem) :m_cnt(mem), super_t(m_cnt) {}
		hatmap_st() :super_t(m_cnt) {}
		bool init(uint16_t caps, uint16_t mean_cnt = 12) { return m_cnt.init(caps, mean_cnt, 1); }
		void uninit() { m_cnt.uninit(); }
	};



	//-----------------------------------------------------
	//ʹ�ù̶��ڴ�Ľ��չ�ϣset(int)
	//-----------------------------------------------------
	template<uint32_t max_slot_count, class key_t = uint32_t, class cmp_t = hat_op_t>
	class hatset_nft :public hatmap_base_t<key_t, void*, cmp_t>
	{
		typedef hatmap_base_t<key_t, void*, cmp_t> super_t;
		typedef hat_ft<max_slot_count, key_t, 1, void*, 0, cmp_t> cntr_t;
		cntr_t	m_cnt;
	public:
		~hatset_nft() { super_t::clear(); }
		hatset_nft() :super_t(m_cnt) {}
	};
	//-----------------------------------------------------
	//ʹ�ö�̬�ڴ�����ݵĽ��չ�ϣset(int)
	//-----------------------------------------------------
	template<class key_t = uint32_t, class cmp_t = hat_op_t>
	class hatset_nt :public hatmap_base_t<key_t, void*, cmp_t>
	{
		typedef hatmap_base_t<key_t, void*, cmp_t> super_t;
		typedef hat_t<key_t, void*, cmp_t> cntr_t;
		cntr_t	m_cnt;
	public:
		~hatset_nt() { m_cnt.uninit(); }
		hatset_nt(mem_allotter_i &mem) :m_cnt(mem), super_t(m_cnt) {}
		hatset_nt() :super_t(m_cnt) {}
		bool init(uint16_t caps) { return m_cnt.init(caps, 1, 0); }
		void uninit() { m_cnt.uninit(); }
	};
	//-----------------------------------------------------
	//ʹ�ù̶��ڴ�Ľ��չ�ϣset(char*)
	//-----------------------------------------------------
	template<uint32_t max_slot_count, class key_t = char, uint16_t mean_cnt = 12, class cmp_t = hat_op_t>
	class hatset_sft :public hatmap_base_t<key_t, void*, cmp_t>
	{
		typedef hatmap_base_t<key_t, void*, cmp_t> super_t;
		typedef hat_ft<max_slot_count, key_t, mean_cnt, void*, 0, cmp_t> cntr_t;
		cntr_t	m_cnt;
	public:
		~hatset_sft() { super_t::clear(); }
		hatset_sft() :super_t(m_cnt) {}
	};
	//-----------------------------------------------------
	//ʹ�ö�̬�ڴ�����ݵĽ��չ�ϣset(char*)
	//-----------------------------------------------------
	template<class key_t = char, class cmp_t = hat_op_t>
	class hatset_st :public hatmap_base_t<key_t, void*, cmp_t>
	{
		typedef hatmap_base_t<key_t, void*, cmp_t> super_t;
		typedef hat_t<key_t, void*, cmp_t> cntr_t;
		cntr_t	m_cnt;
	public:
		~hatset_st() { m_cnt.uninit(); }
		hatset_st(mem_allotter_i &mem) :m_cnt(mem), super_t(m_cnt) {}
		hatset_st() :super_t(m_cnt) {}
		bool init(uint16_t caps, uint16_t mean_cnt = 12) { return m_cnt.init(caps, mean_cnt, 0); }
		void uninit() { m_cnt.uninit(); }
	};
}


#endif
