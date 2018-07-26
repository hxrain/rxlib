#ifndef _RX_MEM_H_
#define _RX_MEM_H_

#include <memory.h>
#include <stdlib.h>
#include "rx_cc_macro.h"
#include "rx_ct_util.h"


namespace rx
{
    //------------------------------------------------------
    //�ڴ�س���ӿ�
    class rx_mem_pool_i
    {
    public:
        virtual void *do_alloc(uint32_t size)=0;
        virtual void *do_realloc(void* ptr,uint32_t newsize){return NULL;}
        virtual void do_free(void* ptr, uint32_t size=0)=0;
    protected:
        virtual ~rx_mem_pool_i(){}
    };

    //------------------------------------------------------
    //����C��׼����ڴ��
    class rx_mem_pool_c:public rx_mem_pool_i
    {
    public:
        virtual void *do_alloc(uint32_t size){return malloc(size);}
        virtual void *do_realloc(void* ptr,uint32_t size){return realloc(ptr,size);}
        virtual void do_free(void* ptr, uint32_t size = 0){free(ptr);}
    };


    //======================================================
    //������ò���:��黺��,Ĭ�Ͽ��Ի���64K���ڴ��
    #ifdef RX_MEMCFG_BIG_BLOCK
        //�ڴ����С����ߴ����С����ڵ�ߴ�
        #ifndef RX_MEMCFG_MinAlign
        #define RX_MEMCFG_MinAlign 32
        #endif

        //�ڴ��������ڵ�ߴ�
        #ifndef RX_MEMCFG_MaxNode
        #define RX_MEMCFG_MaxNode (1024*256)
        #endif

        //�ڴ����ÿ���ڴ��������ߴ�
        #ifndef RX_MEMCFG_MaxStripe
        #define RX_MEMCFG_MaxStripe (RX_MEMCFG_MaxNode)
        #endif
    #endif

    //������ò���:С�黺��,Ĭ�Ͽ��Ի���4K���ڴ��
    #ifdef RX_MEMCFG_SMALL_BLOCK
        //�ڴ����С����ߴ����С����ڵ�ߴ�
        #ifndef RX_MEMCFG_MinAlign
        #define RX_MEMCFG_MinAlign 8
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
        #define RX_MEMCFG_MinAlign 32
    #endif

    //�ڴ��������ڵ�ߴ�
    #ifndef RX_MEMCFG_MaxNode
        #define RX_MEMCFG_MaxNode (1024*8)
    #endif

    //�ڴ����ÿ���ڴ��������ߴ�
    #ifndef RX_MEMCFG_MaxStripe
        #define RX_MEMCFG_MaxStripe (RX_MEMCFG_MaxNode*2)
    #endif

    //======================================================
    //Ĭ���ڴ�ص����ò���
    typedef struct rx_mem_pool_cfg_t
    {
        //������(ÿ��ֵ������Ϊ2����������):��С�ڵ��������ߴ�;���ڵ�ߴ�;ÿ���ڴ��������ߴ�
        //��Щ���������þ����������ڴ�ռ���ʺͻ����������.
        //���ߴ��һЩʱ�ʺϵȳ����ڴ���ܼ�����,����������ȿ����ʱ�ή���ڴ�������.
        enum{MinAlignSize=RX_MEMCFG_MinAlign,MaxNodeSize=RX_MEMCFG_MaxNode,MaxStripeSize=RX_MEMCFG_MaxStripe};

        //�̶��ڴ�ص��������
        enum{PoolCount=MaxNodeSize/MinAlignSize};

        //��������ƫ��ʹ��,����λ��
        enum{ArrayIndexShiftBit=log2<MinAlignSize>::result};

        //У�����ڵ�ߴ�Ϸ���,ȷ��MaxNodeSize��2����������
        enum{MaxNodeSizeShiftBitCheck=log2<MaxNodeSize>::result};
    }
    rx_mem_pool_cfg_t;

	//======================================================
	//��׼�ڴ�������Ĵ��
    typedef struct rx_alloc_cookie_t
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
    }rx_alloc_cookie_t;

    //------------------------------------------------------
    //���ڼ�¼�ڴ�������������
    typedef struct rx_mem_stat_t
    {
        uint32_t CacheAlloced;                                //�ɻ����ѷ���
        uint32_t RealNeedSize;                                //�ɻ��������
        uint32_t InCacheSize;                                 //�ѻ��������
        uint32_t ExternAlloced;                               //�ǻ����ѷ���
        uint32_t CacheUsing()const{return CacheAlloced-InCacheSize;}//�ɻ�����ʹ��
    }rx_mem_stat_t;

}


#endif
