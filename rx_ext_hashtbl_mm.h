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
        uint32_t open(const char* file)
        {
            if (!m_file.open(file, "w+"))
                return m_file.error();
            
            const uint32_t memsize = sizeof(super_t::node_t)*max_node_count + sizeof(raw_hashtbl_stat_t);
            int ret = m_mmap.open(m_file, "w+", memsize);
            if (ret <= 0)
                return ret;
            
            super_t::m_basetbl.bind((super_t::node_t*)(m_mmap.ptr() + sizeof(raw_hashtbl_stat_t)), max_node_count, (raw_hashtbl_stat_t*)m_mmap.ptr(),false);
            return 0;
        }
        //-------------------------------------------------
        void close()
        {
            raw_hashtbl_stat_t dummy;
            super_t::m_basetbl.bind(NULL,0,&dummy);

            m_mmap.flush();
            m_mmap.close();
            m_file.close();
        }
        //-------------------------------------------------
        bool flush() { m_mmap.flush(); }
        //-------------------------------------------------
    };





}

#endif