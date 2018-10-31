#ifndef _RX_EXT_HASHTBL_MM_H_
#define _RX_EXT_HASHTBL_MM_H_

#include "rx_os_file.h"
#include "rx_os_mmap.h"
#include "rx_dtl_hashtbl_tiny.h"

namespace rx
{

    //-----------------------------------------------------
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
        error_code open(const char* file)
        {
            //先尝试打开已有文件,失败后再尝试创建文件
            error_code ec;
            if ((ec=m_file.open(file, "r+")) && (ec=m_file.open(file, "w+")))
                return ec;

            //计算最大所需空间
            const uint32_t memsize = sizeof(super_t::node_t)*max_node_count + sizeof(raw_hashtbl_stat_t);

            //映射文件到内存
            ec = m_mmap.open(m_file, "w+", memsize);
            if (ec)
                return ec;

            //获取状态区域指针
            raw_hashtbl_stat_t *stat = (raw_hashtbl_stat_t*)m_mmap.ptr();
            if (stat->max_nodes)
            {
                if (stat->max_nodes != max_node_count)
                    return ec_limit_data;
            }
            else
                stat->max_nodes = max_node_count;

            //获取数据区域指针
            void *ptr=(m_mmap.ptr() + sizeof(raw_hashtbl_stat_t));
            typename super_t::node_t *nodes = (typename super_t::node_t*)ptr;

            /*
            for (uint32_t i = 0; i < max_node_count; ++i)
            {//临时查看一下,冲突元素的步长是怎样分布的
                super_t::node_t &node = nodes[i];
                if (node.state > 4)
                    printf("%u ", node.step);
            }
            */


            //最后进行哈希容器的初始化
            super_t::m_basetbl.bind(nodes, stat,false);
            return ec_ok;
        }
        //-------------------------------------------------
        void close()
        {
            super_t::m_basetbl.bind(NULL,NULL);

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
