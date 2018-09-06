#ifndef _RX_MEM_ALLOC_H_
#define _RX_MEM_ALLOC_H_

#include "rx_cc_macro.h"
#include "rx_assert.h"
#include "rx_ct_util.h"


namespace rx
{
    //======================================================
    //内存分配器接口类
    class mem_allotter_i
    {
        //标准内存分配器的存根
        typedef struct alloc_cookie_t
        {
            //内存分配存根
            typedef struct cookie_t
            {
                uint32_t item_count;                        //记录此内存块是否为数组,数组元素的数量
                uint32_t node_mem_size;                     //此内存块的尺寸
            } cookie_t;
            //存根结构的大小
            enum {cookie_size=sizeof(cookie_t)};

            //---------------------------------------------
            //将附加信息放入cookie_t:Count=0代表alloc;1代表new对象;其他为new数组;node_mem_size代表本内存块的实际大小(>=4)
            static void set(void* P,uint32_t Count,uint32_t node_mem_size)
            {
                ((cookie_t*)P)->item_count=Count;
                ((cookie_t*)P)->node_mem_size=node_mem_size;
            }
            //---------------------------------------------
            //从cookie_t中得到附加信息
            static cookie_t& get(void* P)
            {
                return *(cookie_t*)P;
            }
        } alloc_cookie_t;

        //-------------------------------------------------
        //对象与数组的分配构造基础语句,可复用.
        #define __DESC_NEW_BASE__   \
            uint32_t MemSize=0;     \
            uint8_t *R=(uint8_t*)base_alloc(MemSize,sizeof(VT)*Count+alloc_cookie_t::cookie_size);\
            if (R==NULL) return NULL;                                       \
            alloc_cookie_t::set(R,Count,MemSize);                           \
            VT* Ret=(VT*)(R+alloc_cookie_t::cookie_size);                   \

        mem_allotter_i& operator=(const mem_allotter_i&);
    public:
        //-------------------------------------------------
        //根据原始分配接口,得到内存块
        void* alloc(uint32_t Size)
        {
            uint32_t MemSize=0;
            uint8_t *R=(uint8_t*)base_alloc(MemSize,Size+alloc_cookie_t::cookie_size);
            if (R==NULL)
                return NULL;
            alloc_cookie_t::set(R,0,MemSize);
            return (R+alloc_cookie_t::cookie_size);
        }
        //-------------------------------------------------
        //释放内存
        void free(void* P)
        {
            rx_assert_msg(P!=NULL,"待释放内存错误!指针为空");
            uint8_t* R=(uint8_t*)P-alloc_cookie_t::cookie_size;
            alloc_cookie_t::cookie_t &ck=alloc_cookie_t::get(R);
            rx_assert_msg(!ck.item_count,"错误的使用了alloc/free函数匹配.");
            base_free(R,ck.node_mem_size);
        }
        //-------------------------------------------------
        //尝试基于现有内存分配扩展内存(暂时先使用数据拷贝的方式实现,等待进行优化)
        virtual void* realloc(void* P,uint32_t NewSize,uint32_t reserved=64)
        {
            if (P==NULL)                                    //没有原指针,直接分配(并进行预留)
                return alloc(NewSize+reserved);

            if (NewSize==0)                                 //有原指针,但要求新尺寸为0时,直接释放
            {
                free(P);
                return NULL;
            }

            //检查原指针的内存节点尺寸,够长的时候直接返回
            uint8_t* R=(uint8_t*)P-alloc_cookie_t::cookie_size;
            alloc_cookie_t::cookie_t &ck=alloc_cookie_t::get(R);
            if (ck.node_mem_size-alloc_cookie_t::cookie_size>=NewSize)
                return P;

            //现在发现,原指针确实不够长了,需要申请新空间
            uint32_t MemSize=0;
            R=(uint8_t*)base_alloc(MemSize, NewSize+reserved);
            if (R==NULL)
                return NULL;
            alloc_cookie_t::set(R,0,MemSize);

            //拷贝原数据
            void *Ret=(R+alloc_cookie_t::cookie_size);
            memcpy(Ret,P,ck.node_mem_size-alloc_cookie_t::cookie_size);

            //最后,释放原指针
            free(P);
            return Ret;
        }

        //-------------------------------------------------
        //分配一个指定类型的对象或数组,使用默认构造函数进行初始化
        template<class VT>
        VT* new0(uint32_t Count=1)
        {
            __DESC_NEW_BASE__
            return Count==1? ct::OC(Ret) : ct::AC(Ret,Count);
        }
        //-------------------------------------------------
        //分配一个指定类型的对象或数组,并使用参数进行构造初始化
        template<class VT,class PT1>
        VT* new1(PT1& P1,uint32_t Count=1)
        {
            __DESC_NEW_BASE__
            return Count==1? ct::OC(Ret,P1) : ct::AC(Ret,Count,P1);
        }
        //-------------------------------------------------
        //分配一个指定类型的对象或数组,并使用参数进行构造初始化
        template<class VT,class PT1,class PT2>
        VT* new2(PT1& P1,PT2 &P2,uint32_t Count=1)
        {
            __DESC_NEW_BASE__
            return Count==1? ct::OC(Ret,P1,P2) : ct::AC(Ret,Count,P1,P2);
        }
        //-------------------------------------------------
        //分配一个指定类型的对象或数组,并使用参数进行构造初始化
        template<class VT,class PT1,class PT2,class PT3>
        VT* new3(PT1& P1,PT2 &P2,PT3 &P3,uint32_t Count=1)
        {
            __DESC_NEW_BASE__
            return Count==1? ct::OC(Ret,P1,P2,P3) : ct::AC(Ret,Count,P1,P2,P3);
        }
        //-------------------------------------------------
        //将new系列函数分配的对象或数组进行析构并收回内存
        template<class VT> bool del(VT* P)
        {
            rx_assert_msg(P!=NULL,"待释放内存错误!指针为空");

            uint8_t* R=(uint8_t*)P-alloc_cookie_t::cookie_size;
            alloc_cookie_t::cookie_t &ck=alloc_cookie_t::get(R);
            if (ck.item_count==1)
                ct::OD(P);                              //单独的对象
            else
                ct::AD(P,ck.item_count);                //是数组类型
            base_free(R,ck.node_mem_size);
            return true;
        }
        //-------------------------------------------------
        template<class VT> bool _del(void* P) {try {throw 1;} catch(...) {} rx_show_msg("此函数专为型别匹配使用!不应该被调用!请检查代码错误");}
        //-------------------------------------------------
        virtual ~mem_allotter_i() {}
    protected:
#undef __DESC_NEW_BASE__
        //-------------------------------------------------
        //子类需给出最原始的分配接口
        virtual void* base_alloc(uint32_t &bksize, uint32_t size)=0;
        virtual void base_free(void* ptr,uint32_t size)=0;
    };

}


#endif
