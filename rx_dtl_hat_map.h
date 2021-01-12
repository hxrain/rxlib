#ifndef _RX_DTL_HAT_MAP_H_
#define _RX_DTL_HAT_MAP_H_

#include "rx_cc_macro.h"
#include "rx_dtl_hat_raw.h"
#include "rx_str_util_std.h"

/*
	<变长动态哈希map容器,基于紧凑哈希表>

	//hat紧凑map容器基类
	template<class key_t, class val_t, class hat_op = hat_op_t>
	class hatmap_base_t;

	//使用固定内存的紧凑哈希map(int:int)
	template<uint32_t max_slot_count, class key_t = uint32_t, class val_t = uint32_t, class cmp_t = hat_op_t>
	class hatmap_nft;

	//使用动态内存可扩容的紧凑哈希map(int:int)
	template<class key_t = uint32_t, class val_t = uint32_t, class cmp_t = hat_op_t>
	class hatmap_nt;

	//使用固定内存的紧凑哈希map(char*:int)
	template<uint32_t max_slot_count, class key_t = char, uint16_t mean_cnt = 12, class val_t = uint32_t, class cmp_t = hat_op_t>
	class hatmap_sft;

	//使用动态内存可扩容的紧凑哈希map(char*:int)
	template<class key_t = char, class val_t = uint32_t, class cmp_t = hat_op_t>
	class hatmap_st;

	//使用固定内存的紧凑哈希set(int)
	template<uint32_t max_slot_count, class key_t = uint32_t, class cmp_t = hat_op_t>
	class hatset_nft;

	//使用动态内存可扩容的紧凑哈希set(int)
	template<class key_t = uint32_t, class cmp_t = hat_op_t>
	class hatset_nt;

	//使用固定内存的紧凑哈希set(char*)
	template<uint32_t max_slot_count, class key_t = char, uint16_t mean_cnt = 12, class cmp_t = hat_op_t>
	class hatset_sft;

	//使用动态内存可扩容的紧凑哈希set(char*)
	template<class key_t = char, class cmp_t = hat_op_t>
	class hatset_st;

*/

namespace rx
{
	//-----------------------------------------------------
	//hat紧凑map容器基类
	template<class key_t, class val_t, class hat_op = hat_op_t>
	class hatmap_base_t
	{
	public:
		typedef hat_raw_t<key_t, val_t, hat_op>	cntr_t;		//定义底层容器类型

	protected:
		cntr_t	&m_cntr;
		//-------------------------------------------------
		hatmap_base_t(cntr_t &cntr) :m_cntr(cntr) {}
		virtual ~hatmap_base_t() {}
	public:
		//-------------------------------------------------
		uint32_t size() { return m_cntr.size(); }
		//-------------------------------------------------
		//迭代器
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
			//*提领运算符重载,用于获取当前节点的val值
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
			//()运算符重载,用于获取当前节点的key值
			const key_t* operator()() const
			{
				rx_assert(m_parent != NULL&&m_idx != -1);
				typename cntr_t::keyoff_t &ko = m_parent->offset(m_idx);
				return m_parent->key(ko);
			}
			//---------------------------------------------
			//节点指向后移(前置运算符模式,未提供后置模式)
			iterator& operator++()
			{
				rx_assert(m_parent != NULL&&m_idx != -1);
				m_idx = m_parent->next(m_idx);
				return reinterpret_cast<iterator&>(*this);
			}
		};
		//-------------------------------------------------
		//准备遍历哈希表,返回遍历的初始位置
		iterator begin() const
		{
			if (m_cntr.offset(0).offset)
				return iterator(m_cntr, 0);
			else
				return iterator(m_cntr, m_cntr.next(0));
		}
		//-------------------------------------------------
		//返回遍历的结束位置
		iterator end() const { return iterator(m_cntr, m_cntr.capacity()); }
		//-------------------------------------------------
		//插入元素与值,给定了dup判重字段且真的重复的时候,val不会被使用
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
		//查找元素,通过返回迭代器是否与end()相同判断是否存在
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
		//按指定长度前缀进行查找(需要进行sort()排序之后)
		template<class KT>
		iterator prefix(const KT *key, uint16_t pre_cnt) const
		{
			uint16_t idx = m_cntr.prefix(key, pre_cnt);
			return iterator(m_cntr, idx);
		}
		//-------------------------------------------------
		//对容器进行排序
		void sort() { m_cntr.sort(); }
		//-------------------------------------------------
		//清空全部的元素
		void clear() { m_cntr.clear(); }
	};



	//-----------------------------------------------------
	//使用固定内存的紧凑哈希map(int:int)
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
	//使用动态内存可扩容的紧凑哈希map(int:int)
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
	//使用固定内存的紧凑哈希map(char*:int)
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
	//使用动态内存可扩容的紧凑哈希map(char*:int)
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
	//使用固定内存的紧凑哈希set(int)
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
	//使用动态内存可扩容的紧凑哈希set(int)
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
	//使用固定内存的紧凑哈希set(char*)
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
	//使用动态内存可扩容的紧凑哈希set(char*)
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
