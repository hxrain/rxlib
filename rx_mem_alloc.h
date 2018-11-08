#ifndef _RX_MEM_ALLOC_H_
#define _RX_MEM_ALLOC_H_

#include "rx_cc_macro.h"
#include "rx_assert.h"
#include "rx_ct_util.h"
#include <memory.h>

//是否开启内存分配器的边界检查
#define RX_MEM_ALLOC_USE_CHECKING 1

namespace rx
{
    //======================================================
    //内存分配器接口类
    class mem_allotter_i
    {
        //---------------------------------------------
        //分配器的内存块结构:|info_t|userdata|pad_data|
        #pragma pack(push,1)
        //标准内存分配器的存根
        typedef struct cookie_t
        {
            //内存分配存根
            typedef struct info_t
            {
                uint32_t item_count;                        //记录此内存块是否为数组,数组元素的数量
                uint32_t mem_size;                          //此内存块的尺寸
            } info_t;
        private:
            //---------------------------------------------
        #if RX_MEM_ALLOC_USE_CHECKING
            typedef uint32_t pad_data_t;
            static const pad_data_t pad_data=0x12345678;    //填充数据,进行内存越界访问的检查
            enum { cookie_size = sizeof(info_t) + sizeof(pad_data_t) };//全部存根的尺寸(存根信息+填充尾)
            //从长度为mem_size的内存块R的尾部获取填充数据
            #define RX_MEM_BLOCK_TAIL_PAD_GET(R,mem_size) *rx_ptr_tail_cast(R, mem_size, sizeof(pad_data), pad_data_t)
            //对长度为mem_size的内存块R的尾部设置填充数据
            #define RX_MEM_BLOCK_TAIL_PAD_SET(R,mem_size) RX_MEM_BLOCK_TAIL_PAD_GET(R, mem_size) = pad_data
            //校验检查长度为mem_size的内存块R的尾部填充数据
            #define RX_MEM_BLOCK_CHECK_PAD(R,mem_size) rx_fail_msg(RX_MEM_BLOCK_TAIL_PAD_GET(R,mem_size)==pad_data,"!!allotter memory block overflow!!")
        #else
            enum { cookie_size = sizeof(info_t) };          //全部存根的尺寸(存根信息)
            #define RX_MEM_BLOCK_TAIL_PAD_GET(R,mem_size)
            #define RX_MEM_BLOCK_TAIL_PAD_SET(R,mem_size)
            #define RX_MEM_BLOCK_CHECK_PAD(R,mem_size)
        #endif
        public:
            //---------------------------------------------
            //获知包含存根和用户块的总空间尺寸
            static uint32_t memsize(uint32_t datsize){return datsize+cookie_size;}
            //根据信息头计算实际可用的用户块尺寸
            static uint32_t usrsize(info_t &info){return info.mem_size-cookie_size;}
            //---------------------------------------------
            //将附加信息放入R所指向的cookie_t中:Count=0代表alloc;1代表new对象;其他为new数组;mem_size代表本内存块的实际大小(>=4)
            //返回值:用户可用的指针部分
            static void* set(void* R,uint32_t Count,uint32_t mem_size)
            {
                info_t &ck = *rx_ptr_head_cast(R, 0, info_t);//指向info对象
                ck.item_count = Count;                      //进行info存根信息的记录
                ck.mem_size = mem_size;
                RX_MEM_BLOCK_TAIL_PAD_SET(R, mem_size);     //在内存块的尾部记录填充标记
                return rx_ptr_head_cast(R, sizeof(info_t), void);
            }
            //---------------------------------------------
            //从R所持有的cookie_t中得到附加信息
            static info_t& get(void* R)
            {
                info_t &ck = *rx_ptr_head_cast(R, 0, info_t);//指向info对象
                RX_MEM_BLOCK_CHECK_PAD(R, ck.mem_size);
                return ck;
            }
            //---------------------------------------------
            //根据R得到最终用户可用的指针部分
            static void* usrptr(void *R)
            {
                info_t &ck = *rx_ptr_head_cast(R, 0, info_t);
                RX_MEM_BLOCK_CHECK_PAD(R, ck.mem_size);
                return rx_ptr_head_cast(R, sizeof(info_t), void);
            }
            //---------------------------------------------
            //根据用户指针获取原始指针
            static void* rawptr(void *P)
            {
                void *R = rx_ptr_head_cast(P, -(int)sizeof(info_t), void);
                info_t &ck = *rx_ptr_head_cast(R, 0, info_t);
                RX_MEM_BLOCK_CHECK_PAD(R, ck.mem_size);
                return R;
            }
        }cookie_t;
        #pragma pack(pop)

        //-------------------------------------------------
        //对象与数组的分配构造基础语句,可复用.
        #define __DESC_NEW_BASE__                                   \
            uint32_t MemSize=0;                                     \
            uint8_t *R=(uint8_t*)base_alloc(MemSize,cookie_t::memsize(sizeof(VT)*Count));\
            if (R==NULL) return NULL;                               \
            VT* Ret=(VT*)cookie_t::set(R,Count,MemSize);            \

        mem_allotter_i& operator=(const mem_allotter_i&);
    public:
        //-------------------------------------------------
        //根据原始分配接口,得到内存块
        void* alloc(uint32_t Size)
        {
            uint32_t MemSize=0;
            uint8_t *R=(uint8_t*)base_alloc(MemSize,cookie_t::memsize(Size));
            if (R==NULL)
                return NULL;
            return cookie_t::set(R,0,MemSize);
        }
        //-------------------------------------------------
        //释放内存
        void free(void* P)
        {
            rx_assert_msg(P!=NULL,"待释放内存错误!指针为空");
            uint8_t* R=(uint8_t*)cookie_t::rawptr(P);
            cookie_t::info_t &ck=cookie_t::get(R);
            rx_assert_msg(!ck.item_count,"错误的使用了alloc/free函数匹配.");
            uint32_t memsize=ck.mem_size;
            base_free(R,memsize);
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
            uint8_t* R=(uint8_t*)cookie_t::rawptr(P);
            cookie_t::info_t &ck=cookie_t::get(R);
            if (cookie_t::usrsize(ck)>=NewSize+reserved)
                return P;

            //现在发现,原指针确实不够长了,需要申请新空间
            uint32_t MemSize=0;
            R=(uint8_t*)base_alloc(MemSize, cookie_t::memsize(NewSize+reserved));
            if (R==NULL)
                return NULL;
            void *Ret=cookie_t::set(R,0,MemSize);           //得到最终可返回的用户指针

            //拷贝原数据到新空间
            memcpy(Ret,P,cookie_t::usrsize(ck));

            //最后,释放原指针
            free(P);
            return Ret;
        }
        //-------------------------------------------------
        //获知指定内存块的用户可用尺寸
        uint32_t usrsize(void* P){return cookie_t::usrsize(cookie_t::get(cookie_t::rawptr(P)));}
        //获知指定内存块的实际占用尺寸
        uint32_t memsize(void* P){return cookie_t::get(cookie_t::rawptr(P)).mem_size;}
        //-------------------------------------------------
        //分配一个指定类型的对象或数组,使用默认构造函数进行初始化,没有额外参数
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

            uint8_t* R=(uint8_t*)cookie_t::rawptr(P);
            cookie_t::info_t &ck=cookie_t::get(R);
            if (ck.item_count==1)
                ct::OD(P);                              //单独的对象
            else
                ct::AD(P,ck.item_count);                //是数组类型
            uint32_t memsize=ck.mem_size;
            base_free(R,memsize);
            return true;
        }
        //-------------------------------------------------
        template<class VT> bool del(void* P) {try {throw 1;} catch(...) {} rx_show_msg("此函数专为型别匹配使用!不应该被调用!请检查代码错误");}
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
