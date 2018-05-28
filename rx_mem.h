#ifndef _RX_MEM_H_
#define _RX_MEM_H_

#include "rx_cc_macro.h"
#include "memory.h"

namespace rx
{
    //------------------------------------------------------
    //�ڴ�س���ӿ�
    class rx_mem_pool_if
    {
    public:
        virtual void *do_alloc(uint32_t size)=0;
        virtual void *do_realloc(void* ptr,uint32_t size)=0;
        virtual void do_free(void* ptr)=0;
    protected:
        virtual ~rx_mem_pool_if(){}
    };

    //------------------------------------------------------
    //����C��׼����ڴ��
    class rx_mem_pool_c:public rx_mem_pool_if
    {
    public:
        virtual void *do_alloc(uint32_t size){return malloc(size);}
        virtual void *do_realloc(void* ptr,uint32_t size){return realloc(ptr,size);}
        virtual void do_free(void* ptr){free(ptr);}
    };
}


#endif
