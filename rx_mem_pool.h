#ifndef _RX_MEM_H_
#define _RX_MEM_H_

#include <memory.h>
#include <stdlib.h>
#include "rx_cc_macro.h"
#include "rx_ct_util.h"

namespace rx
{
    //------------------------------------------------------
    //�ڴ�ط���ӿ�
    class mempool_t
    {
    public:
        virtual void *do_alloc(uint32_t &blocksize,uint32_t size)=0;
        virtual void do_free(void* ptr, uint32_t blocksize=0)=0;
        virtual void *do_realloc(uint32_t &blocksize, void* ptr, uint32_t newsize) { return NULL; }
    protected:
        virtual ~mempool_t() {}
    };

    //------------------------------------------------------
    //���ڼ�¼�ڴ�������Ĺ������
    typedef struct alloc_stat_t
    {
        uint32_t c_alloced;                                 //�����ѷ���
        uint32_t c_realsize;                                //���������
    } alloc_stat_t;

    //------------------------------------------------------
    //���ڼ�¼�ڴ�صĹ������
    typedef struct mempool_stat_t
    {
        uint32_t pool_size;                                 //�ڴ������
        uint32_t pool_free;                                 //�ڴ������
        uint32_t nc_alloced;                                //�ǻ������
    } mempool_stat_t;

    //------------------------------------------------------
    //����C��׼����ڴ��
    template<class dummy=void>
    class mempool_std_t:public mempool_t
    {
    public:
        static void *alloc(uint32_t size) { return ::malloc(size); }
        static void *realloc(void* ptr, uint32_t size) { return ::realloc(ptr, size); }
        static void free(void* ptr, uint32_t size = 0) { ::free(ptr); }
        
        virtual void *do_alloc(uint32_t &blocksize, uint32_t size) { blocksize = size; return ::malloc(size); }
        virtual void *do_realloc(uint32_t &blocksize, void* ptr, uint32_t newsize) { blocksize = newsize; return ::realloc(ptr,newsize); }
        virtual void do_free(void* ptr, uint32_t blocksize = 0) { ::free(ptr); }
        virtual bool do_init(uint32_t size = 0) { return true; }
        virtual void do_uninit(bool force = false) {}
    };

    //------------------------------------------------------
    //�ڴ�ع��ܻ���
    class mempool_i:public mempool_t
    {
    protected:
        mempool_stat_t	m_stat;
        mempool_t        *m_base;
        //--------------------------------------------------
        mempool_i():m_base(0) {memset(&m_stat,0,sizeof(m_stat));}
        //--------------------------------------------------
        void *base_alloc(uint32_t &blocksize,uint32_t size)
        {
            if (m_base)
                return m_base->do_alloc(blocksize,size);
            else
            {
                blocksize=size;
                return mempool_std_t<>::alloc(size);
            }
        }
        //--------------------------------------------------
        void *base_realloc(uint32_t &blocksize,void* ptr,uint32_t newsize)
        {
            if (m_base)
                return m_base->do_realloc(blocksize,ptr,newsize);
            else
            {
                blocksize=newsize;
                return mempool_std_t<>::realloc(ptr,newsize);
            }
        }
        //--------------------------------------------------
        void base_free(void* ptr, uint32_t blocksize=0)
        {
            if (m_base)
                return m_base->do_free(ptr,blocksize);
            else
                return mempool_std_t<>::free(ptr,blocksize);
        }

    public:
        void bind(mempool_t& mp) {m_base=&mp;}
        const mempool_stat_t& stat() const {return m_stat;}

        virtual bool do_init(uint32_t size = 0) { return true; }
        virtual void do_uninit(bool force = false) {}
    };

    //======================================================
    //������ò���:��黺��,Ĭ�Ͽ��Ի���64K���ڴ��
#ifdef RX_MEMCFG_BIG_BLOCK
    //�ڴ����С����ڵ�ߴ�
    #ifndef RX_MEMCFG_MinNode
        #define RX_MEMCFG_MinNode 128
    #endif

    //�ڴ��������ڵ�ߴ�
    #ifndef RX_MEMCFG_MaxNode
        #define RX_MEMCFG_MaxNode (1024*256)
    #endif

    //�ڴ����ÿ���ڴ��������ߴ�
    #ifndef RX_MEMCFG_StripeAlign
        #define RX_MEMCFG_StripeAlign (RX_MEMCFG_MaxNode*16)
    #endif
#endif

    //������ò���:С�黺��,Ĭ�Ͽ��Ի���4K���ڴ��
#ifdef RX_MEMCFG_SMALL_BLOCK
    //�ڴ����С����ߴ����С����ڵ�ߴ�
    #ifndef RX_MEMCFG_MinNode
        #define RX_MEMCFG_MinNode 32
    #endif

    //�ڴ��������ڵ�ߴ�
    #ifndef RX_MEMCFG_MaxNode
        #define RX_MEMCFG_MaxNode (1024*4)
    #endif

    //�ڴ����ÿ���ڴ��������ߴ�
    #ifndef RX_MEMCFG_StripeAlign
        #define RX_MEMCFG_StripeAlign (RX_MEMCFG_MaxNode*2)
    #endif
#endif

    //Ĭ������
    //�ڴ����С����ߴ����С����ڵ�ߴ�
#ifndef RX_MEMCFG_MinNode
    #define RX_MEMCFG_MinNode 32
#endif

    //�ڴ��������ڵ�ߴ�
#ifndef RX_MEMCFG_MaxNode
    #define RX_MEMCFG_MaxNode (1024*16)
#endif

    //�ڴ����ÿ���ڴ��������ߴ�
#ifndef RX_MEMCFG_StripeAlign
    #define RX_MEMCFG_StripeAlign (RX_MEMCFG_MaxNode*8)
#endif

    //======================================================
    //Ĭ���ڴ�ص����ò���(ÿ��ֵ������Ϊ2����������)
    typedef struct mempool_cfg_t
    {
        enum
        {
            //�ɻ������С�ڵ�ߴ�
            MinNodeSize = RX_MEMCFG_MinNode,
            //�ɻ�������ڵ�ߴ�
            MaxNodeSize = RX_MEMCFG_MaxNode,
            //ÿ���ڴ������������ߴ�
            StripeAlignSize = RX_MEMCFG_StripeAlign
        };

        //Ĭ�ϲ���Ķ�:log2(��С����ߴ�),ȷ��MinNodeSize��2����������
        enum {MinNodeShiftBit=LOG2<MinNodeSize>::result};
        //Ĭ�ϲ���Ķ�:log2(���ڵ�ߴ�),ȷ��MaxNodeSize��2����������
        enum {MaxNodeShiftBit=LOG2<MaxNodeSize>::result};
    } mempool_cfg_t;
}


#endif
