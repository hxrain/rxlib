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

/*本单元封装实现了基于原始跳表的跳表集
	//-----------------------------------------------------
	//基于跳表的集合容器
	template<class key_t,uint32_t MAX_LEVEL,class rnd_t=skiplist_rnd_t>
	class skipset_t;

	//便于使用的整数跳表集合
	typedef skipset_t<uint32_t>          skipset_ut;
	typedef skipset_t<int32_t>           skipset_it;
	//便于使用的const char*串跳表集合
	typedef skipset_t<const char*>       skipset_ct;
	//便于使用的const wchar_t*串跳表集合
	typedef skipset_t<const wchar_t*>    skipset_wt;
*/

namespace rx
{
	//-----------------------------------------------------
	//封装基于跳表的基础集合容器类
	//-----------------------------------------------------
	template<class key_t, uint32_t MAX_LEVEL = RX_SKLIST_MAXLEVEL, class rnd_t = skiplist_rnd_t>
	class skipset_t
	{
		#pragma pack(push,1)
		//-------------------------------------------------
		//skiplist集合容器节点类型,为了避免gcc内嵌类的精确偏特化错误,增加一个类型占位符dummy_t.
		template<class kt, typename dummy_t>
		struct skipset_node_t
		{
			//---------------------------------------------
			//比较节点与给定key的大小
			//返回值:n<key为<0;n==key为0;n>key为>0
			template<class KT>
			static int cmp(const void *ptr, const KT &key)
			{
				const skipset_node_t &n = *(skipset_node_t*)ptr;
				return n.key == key ? 0 : (n.key < key ? -1 : 1);
			}
			//---------------------------------------------
			//计算key需要的扩展尺寸
			template<class KT>
			static int ext_size(const KT &k) { return 0; }

			//---------------------------------------------
			//进行定向构造并初始化
			template<class val_t>
			void OC(uint32_t level, val_t &val, uint32_t es) { ct::OC(&key, val); }
			//---------------------------------------------
			typedef kt node_key_t;
			node_key_t  key;
			sk_block_t  levels;                             //跳表实现的关键:分层的后趋节点指针,必须放在节点的最后,用于弹性扩展访问
		};
		//-------------------------------------------------
		//对于skipset的const char*/const wchar_t*类型的key进行偏特化,内部统一进行内存分配与key内容记录
		//对于char*/wchar_t*的key类型,扔保留原始key指针类型
		template<typename dummy_t>
		struct skipset_node_t<const char*, dummy_t >
		{
			//---------------------------------------------
			//比较节点与给定key的大小
			//返回值:n<key为<0;n==key为0;n>key为>0
			template<class KT>
			static int cmp(const void *ptr, const KT &key)
			{
				const skipset_node_t &n = *(skipset_node_t*)ptr;
				return st::strcmp(n.key.c_str(), key);
			}
			//---------------------------------------------
			//计算key需要的扩展尺寸
			static int ext_size(const char* k) { return st::strlen(k) + 1; }

			//---------------------------------------------
			//进行定向构造并初始化
			template<class val_t>
			void OC(uint32_t level, val_t &val, uint32_t es)
			{
				ct::OC(&key);
				key.bind((char*)&levels.next[level], es, val, es - 1);
			}
			//---------------------------------------------
			typedef tiny_string_t<char> node_key_t;
			node_key_t  key;
			sk_block_t  levels;                             //跳表实现的关键:分层的后趋节点指针,必须放在节点的最后,用于弹性扩展访问
		};
		//-------------------------------------------------
		template<typename dummy_t>
		struct skipset_node_t<const wchar_t*, dummy_t >
		{
			//---------------------------------------------
			//比较节点与给定key的大小
			//返回值:n<key为<0;n==key为0;n>key为>0
			template<class KT>
			static int cmp(const void *ptr, const KT &key)
			{
				const skipset_node_t &n = *(skipset_node_t*)ptr;
				return st::strcmp(n.key.c_str(), key);
			}
			//---------------------------------------------
			//计算key需要的扩展尺寸(w串占用空间比c串大一倍)
			static int ext_size(const wchar_t* k) { return (st::strlen(k) + 1) * sc<wchar_t>::char_size(); }

			//---------------------------------------------
			//进行定向构造并初始化
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
			sk_block_t  levels;                             //跳表实现的关键:分层的后趋节点指针,必须放在节点的最后,用于弹性扩展访问
		};
		#pragma pack(pop)
		//-------------------------------------------------
		typedef skipset_node_t<key_t, void>  sk_node_t;     //定义最终使用的基础原始调整容器节点
		typedef raw_skiplist_t<sk_node_t>    sk_list_t;     //定义最终使用的基础原始跳表容器类型
		//-------------------------------------------------
		skiplist_rnd_level<rnd_t, MAX_LEVEL> m_rnd_level;   //随机层高生成器
		sk_list_t                            m_raw_list;    //原始的跳表容器
	public:
		typedef sk_node_t node_t;
		//-------------------------------------------------
		skipset_t(mem_allotter_i &ma, uint32_t seed = 1) :m_rnd_level(seed), m_raw_list(ma, field_offset(sk_node_t, levels)) {}
		skipset_t(uint32_t seed = 1) :m_rnd_level(seed), m_raw_list(rx_global_mem_allotter(), field_offset(sk_node_t, levels)) {}
		virtual ~skipset_t() { clear(); }
		//-------------------------------------------------
		//定义简单的只读迭代器
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
		//已经使用的节点数量
		uint32_t size() const { return m_raw_list.size(); }
		//-------------------------------------------------
		//准备遍历集合,返回遍历的初始位置
		iterator begin() const { return iterator((node_t*)m_raw_list.head()); }
		//-------------------------------------------------
		//返回遍历的结束位置
		iterator end() const { return iterator(NULL); }
		//-------------------------------------------------
		//获取集合的尾节点迭代器(可用于快速获取尾节点的值)
		iterator rbegin() const { return iterator((node_t*)m_raw_list.tail()); }
		//-------------------------------------------------
		//删除指定位置的元素
		//返回值:是否删除了当前值
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
		//在集合中插入元素并赋值构造
		//返回值:<0-内存不足;0-值重复;>0-成功
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
		//查找元素是否存在
		template<class val_t>
		bool find(const val_t &val) const { return m_raw_list.find(val) != NULL; }
		//-------------------------------------------------
		//删除元素并默认析构
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
		//清空全部的元素
		void clear()
		{
			for (iterator I = begin(); I != end(); ++I)
				ct::OD(I.m_node);
			m_raw_list.clear();
		}
	};

	//语法糖,定义一个便于使用的整数跳表集合
	typedef skipset_t<uint32_t>          skipset_ut;
	typedef skipset_t<int32_t>           skipset_it;
	//语法糖,定义一个便于使用的const char*串跳表集合
	typedef skipset_t<const char*>       skipset_ct;
	//语法糖,定义一个便于使用的const wchar_t*串跳表集合
	typedef skipset_t<const wchar_t*>    skipset_wt;
}

#endif
