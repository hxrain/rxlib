#ifndef _RX_DTL_HASHTBL_EX_H_
#define _RX_DTL_HASHTBL_EX_H_

#include "rx_dtl_hashtbl.h"
#include "rx_mem_alloc_cntr.h"
/*
	定长动态分配的哈希表
*/
namespace rx
{
	//-----------------------------------------------------
	//key为uint32_t类型的动态哈希表(默认val也为uint32_t)
	//-----------------------------------------------------
	template<class val_t = uint32_t, class key_t = uint32_t, bool correct = false, class cmp_t = hashtbl_cmp_t>
	class hashtbl_t :public hashtbl_base_t<key_t, val_t, cmp_t, correct>
	{
		typedef hashtbl_base_t<key_t, val_t, cmp_t, correct> super_t;
		typename super_t::node_t    *m_nodes;
		raw_hashtbl_stat_t          m_stat;
		mem_allotter_i              &m_mem;
	public:
		hashtbl_t() :m_nodes(NULL), m_stat(0), m_mem(rx_global_mem_allotter()) {}
		hashtbl_t(mem_allotter_i &ma) :m_nodes(NULL), m_mem(ma) {}
		~hashtbl_t() { clear(); }
		//-------------------------------------------------
		bool make(uint32_t max_size, float space_factor = 0.3)
		{
			clear();
			max_size = (uint32_t)(max_size*(1 + space_factor));
			m_nodes = m_mem.make<typename super_t::node_t>(max_size);
			if (!m_nodes) return false;
			m_stat.max_nodes = max_size;
			super_t::m_basetbl.bind(m_nodes, &m_stat);
			return true;
		}
		//-------------------------------------------------
		void clear()
		{
			if (!m_nodes) return;

			super_t::clear();
			m_mem.unmake(m_nodes);
			m_nodes = NULL;
			super_t::m_basetbl.bind(NULL, NULL, false);
		}
	};

	//-----------------------------------------------------
	//key为tiny_string类型的动态哈希表(默认val为uint32_t)
	//-----------------------------------------------------
	template<class val_t = uint32_t, uint16_t max_str_size = 12, bool correct = false, class CT = char, class cmp_t = hashtbl_cmp_t>
	class hashtbl_st :public hashtbl_base_t<tiny_string_t<CT, max_str_size>, val_t, cmp_t, correct>
	{
		typedef hashtbl_base_t<tiny_string_t<CT, max_str_size>, val_t, cmp_t, correct> super_t;
		typename super_t::node_t    *m_nodes;
		raw_hashtbl_stat_t          m_stat;
		mem_allotter_i              &m_mem;
	public:
		hashtbl_st() :m_nodes(NULL), m_stat(0), m_mem(rx_global_mem_allotter()) {}
		hashtbl_st(mem_allotter_i &ma) :m_nodes(NULL), m_mem(ma) {}
		~hashtbl_st() { clear(); }
		//-------------------------------------------------
		bool make(uint32_t max_size, float space_factor = 0.3)
		{
			clear();
			max_size = (uint32_t)(max_size*(1 + space_factor));
			m_nodes = m_mem.make<typename super_t::node_t>(max_size);
			if (!m_nodes) return false;
			m_stat.max_nodes = max_size;
			super_t::m_basetbl.bind(m_nodes, &m_stat);
			return true;
		}
		//-------------------------------------------------
		void clear()
		{
			if (!m_nodes) return;

			super_t::clear();
			m_mem.unmake(m_nodes);
			m_nodes = NULL;
			super_t::m_basetbl.bind(NULL, NULL, false);
		}
	};
}

#endif
