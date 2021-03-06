#ifndef _RX_EXT_HASHTBL_MM_H_
#define _RX_EXT_HASHTBL_MM_H_

#include "rx_os_file.h"
#include "rx_os_mmap.h"
#include "rx_dtl_hashtbl.h"

namespace rx
{

	//-----------------------------------------------------
	//基于文件内存映射的持久化定长哈希表
	template<uint32_t max_node_count, class val_t = uint32_t, class key_t = uint32_t, class cmp_t = hashtbl_cmp_t>
	class tiny_mm_hashtbl_t :public hashtbl_base_t<key_t, val_t, cmp_t >
	{
		typedef hashtbl_base_t<key_t, val_t, cmp_t > super_t;
		os_file_t   m_file;
		os_mmap_t   m_mmap;
	public:
		//-------------------------------------------------
		tiny_mm_hashtbl_t() {}
		~tiny_mm_hashtbl_t() { close(); }
		//-------------------------------------------------
		//创建或打开文件映射哈希表
		error_t open(const char* file)
		{
			//先尝试打开已有文件,失败后再尝试创建文件
			error_t ec;
			if ((ec = m_file.open(file, "r+")) && (ec = m_file.open(file, "w+")))
				return ec;

			//计算最大所需空间
			const uint32_t memsize = sizeof(typename super_t::node_t)*max_node_count + sizeof(raw_hashtbl_stat_t);

			//映射文件到内存
			ec = m_mmap.open(m_file, "w+", memsize);
			if (ec)
				return ec;

			//获取状态区域指针
			raw_hashtbl_stat_t *stat = (raw_hashtbl_stat_t*)m_mmap.ptr();
			if (stat->max_nodes)
			{//文件被初始化过,则需要检查之前初始化和现在初始化给定的最大容量是否相同.
				if (stat->max_nodes != max_node_count)
					return ec_limit_data;
			}
			else
				stat->max_nodes = max_node_count;			//文件未被初始化过,则记录最大容量

			//获取数据区域指针
			void *ptr = (m_mmap.ptr() + sizeof(raw_hashtbl_stat_t));
			typename super_t::node_t *nodes = (typename super_t::node_t*)ptr;

			//最后进行哈希容器的初始化
			super_t::m_basetbl.bind(nodes, stat, false);
			return ec_ok;
		}
		//-------------------------------------------------
		void close()
		{
			super_t::m_basetbl.bind(NULL, NULL);

			m_mmap.flush();
			m_mmap.close();
			m_file.close();
		}
		//-------------------------------------------------
		bool flush() { return m_mmap.flush(); }
		//-------------------------------------------------
	};
}

#endif
