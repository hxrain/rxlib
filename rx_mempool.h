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
    class mempool
    {
    public:
        virtual void *do_alloc(uint32_t &blocksize,uint32_t size)=0;
        virtual void *do_realloc(uint32_t &blocksize,void* ptr,uint32_t newsize){return NULL;}
        virtual void do_free(void* ptr, uint32_t blocksize=0)=0;
    protected:
        virtual ~mempool(){}
    };

	//------------------------------------------------------
    //���ڼ�¼�ڴ�ع������
    typedef struct mempool_stat_t
    {
        uint32_t alloced;                                   //�����ѷ���
        uint32_t real_size;                                 //���������
        uint32_t can_reuse;                                 //���������
        uint32_t nc_alloced;                                //�ǻ������
        uint32_t cache_using()const { return alloced - can_reuse; }//����ʹ����
    }mempool_stat_t;

    //------------------------------------------------------
    //����C��׼����ڴ��
    class mempool_std 
    {
    public:
        static void *do_alloc(uint32_t size) { return malloc(size); }
        static void *do_realloc(void* ptr, uint32_t size) { return realloc(ptr, size); }
        static void do_free(void* ptr, uint32_t size = 0) { free(ptr); }
    };

    //------------------------------------------------------
    //�ڴ�ع��ܻ���
    class mempool_i:public mempool
    {
    protected:
		mempool_stat_t	m_stat;
		mempool        *m_base;
        //--------------------------------------------------
        mempool_i():m_base(0){memset(&m_stat,0,sizeof(m_stat));}
        //--------------------------------------------------
        void *base_alloc(uint32_t &blocksize,uint32_t size)
        {
            if (m_base)
                return m_base->do_alloc(blocksize,size);
            else
            {
                blocksize=size;
                return mempool_std::do_alloc(size);
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
                return mempool_std::do_realloc(ptr,newsize);
            }
        }
        //--------------------------------------------------
        void base_free(void* ptr, uint32_t blocksize=0)
        {
            if (m_base)
                return m_base->do_free(ptr,blocksize);
            else
                return mempool_std::do_free(ptr,blocksize);
        }

	public:
        void bind(mempool& mp){m_base=&mp;}
		virtual bool do_init(uint32_t size){return true;}
		virtual void do_uninit(bool force=false){}
		const mempool_stat_t& stat() const {return m_stat;}
    };

    //======================================================
    //������ò���:��黺��,Ĭ�Ͽ��Ի���64K���ڴ��
    #ifdef RX_MEMCFG_BIG_BLOCK
        //�ڴ����С����ߴ����С����ڵ�ߴ�
        #ifndef RX_MEMCFG_MinAlign
        #define RX_MEMCFG_MinAlign 128
        #endif

        //�ڴ��������ڵ�ߴ�
        #ifndef RX_MEMCFG_MaxNode
        #define RX_MEMCFG_MaxNode (1024*256)
        #endif

        //�ڴ����ÿ���ڴ��������ߴ�
        #ifndef RX_MEMCFG_MaxStripe
        #define RX_MEMCFG_MaxStripe (RX_MEMCFG_MaxNode*4)
        #endif
    #endif

    //������ò���:С�黺��,Ĭ�Ͽ��Ի���4K���ڴ��
    #ifdef RX_MEMCFG_SMALL_BLOCK
        //�ڴ����С����ߴ����С����ڵ�ߴ�
        #ifndef RX_MEMCFG_MinAlign
        #define RX_MEMCFG_MinAlign 32
        #endif

        //�ڴ��������ڵ�ߴ�
        #ifndef RX_MEMCFG_MaxNode
        #define RX_MEMCFG_MaxNode (1024*4)
        #endif

        //�ڴ����ÿ���ڴ��������ߴ�
        #ifndef RX_MEMCFG_MaxStripe
        #define RX_MEMCFG_MaxStripe (RX_MEMCFG_MaxNode*2)
        #endif
    #endif

    //Ĭ������
    //�ڴ����С����ߴ����С����ڵ�ߴ�
    #ifndef RX_MEMCFG_MinAlign
        #define RX_MEMCFG_MinAlign 64
    #endif

    //�ڴ��������ڵ�ߴ�
    #ifndef RX_MEMCFG_MaxNode
        #define RX_MEMCFG_MaxNode (1024*16)
    #endif

    //�ڴ����ÿ���ڴ��������ߴ�
    #ifndef RX_MEMCFG_MaxStripe
        #define RX_MEMCFG_MaxStripe (RX_MEMCFG_MaxNode*8)
    #endif

    //======================================================
    //Ĭ���ڴ�ص����ò���(ÿ��ֵ������Ϊ2����������)
    typedef struct mempool_cfg_t
    {
        enum {
            MinAlignSize = RX_MEMCFG_MinAlign,              //��С�ڵ�����ߴ�
            MaxNodeSize = RX_MEMCFG_MaxNode,                //�ɻ�������ڵ�ߴ�
            MaxStripeSize = RX_MEMCFG_MaxStripe             //ÿ���ڴ��������ߴ�
        };

        //У����С����ߴ�Ϸ���,ȷ��MinAlignSize��2����������
        enum{MinSizeShiftBit=log2<MinAlignSize>::result};
        //У�����ڵ�ߴ�Ϸ���,ȷ��MaxNodeSize��2����������
        enum{MaxNodeSizeShiftBit=log2<MaxNodeSize>::result};
    }mempool_cfg_t;

	//======================================================
	//��׼�ڴ�������Ĵ��
    typedef struct alloc_cookie_t
    {
        //�ڴ������
        typedef struct HNodeCookie
        {
            uint32_t Cookie;                              //��¼���ڴ���Ƿ�Ϊ����,����Ԫ�ص�����
            uint32_t MemSize;                             //���ڴ��ĳߴ�
        }HNodeCookie;
        //����ṹ�Ĵ�С
        enum{HNodeCookieSize=sizeof(HNodeCookie)};
        //���������:�����Ķ���,��������
        enum{AllocType_ObjectArray=0x80000000};
        //--------------------------------------------------
        //��������Ϣ����Cookie:Count=0����Alloc,1����New,����ΪNew����;MemSize�����ڴ���ʵ�ʴ�С
        static inline void Record(void* P,uint32_t Count,uint32_t MemSize)
        {
            ((HNodeCookie*)P)->Cookie=Count;
            ((HNodeCookie*)P)->MemSize=MemSize;
        }
        //--------------------------------------------------
        //��Cookie�еõ�������Ϣ
        static inline bool Get(void* P,uint32_t &Count,uint32_t &MemSize)
        {
            Count=((HNodeCookie*)P)->Cookie&~AllocType_ObjectArray;
            MemSize=((HNodeCookie*)P)->MemSize;
            return !!(((HNodeCookie*)P)->Cookie&AllocType_ObjectArray);
        }
    }alloc_cookie_t;
}


#endif
