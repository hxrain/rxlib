#ifndef _RX_TIMER_TW_H_
#define _RX_TIMER_TW_H_

#include "rx_cc_macro.h"
#include "rx_dtl_list.h"
#include "rx_dtl_queue.h"
#include "rx_mem_alloc_cntr.h"
#include "rx_ct_util.h"
#include "rx_ct_delegate.h"

namespace rx
{
    //-------------------------------------------------
    //标准定时器回调函数类型
    typedef void (*on_timer_t)(const size_t handle, uint32_t event_code, void *usrdat);

    //成员函数回调类型
    //void xx::on_timer(const size_t handle, uint32_t event_code)

    //定时器事件回调委托类型
    typedef delegate2_rt<const size_t, uint32_t, void> timer_delegate_t;

    namespace tw
    {
        //-------------------------------------------------
        //定义内部定时器信息列表类型
        struct timer_entry_t;
        typedef list_t<timer_entry_t*>  item_list_t;

        const uint32_t wheel_slots = 256;                   //每个轮子上的槽位数量
        const uint32_t max_rounds  = 65535;                 //最大允许的溢出轮数

        //-------------------------------------------------
        //内部定时器信息项
        typedef struct timer_entry_t
        {
            static const uint8_t size_mask = 0x1F;          //本条目的尺寸掩码(2的5次方,应<=sizeof(*this))

            item_list_t::iterator   slot_link;              //记录本节点在时间槽链表中的位置,便于反向查找
            timer_delegate_t        cb;                     //定时器委托回调
            uint32_t                interval;               //定时间隔时间,ms
            uint32_t                event_code;             //用户给定的事件码
            uint32_t                repeat;                 //重复执行次数,-1为永远执行
            uint16_t                round;                  //触发本定时器需要的剩余轮数,为0时才能触发
            uint16_t                ttl;                    //本条目的生存计数

            //内部信息清除,准备再次复用
            void reset()
            {
                slot_link=NULL;
                cb.reset();
                interval=0;
                event_code=0;
                repeat=0;
                round=0;
                ttl=(ttl+1)&size_mask;                      //生存计数不清零,递增取模
            }
            //获取本条目的对外句柄
            size_t handle()
            {
                size_t ret=(size_t)this;
                rx_assert((ret&size_mask)==0);
                return ret|ttl;                             //借用指针低5bit存放ttl,等待访问时来验证
            }
            //校验对外句柄的有效性
            bool check(size_t handle)
            {
                return (handle&size_mask)==ttl&&((handle&(~size_mask))==(size_t)this);
            }
            //静态方法,根据定时器句柄计算内部信息对象指针
            static timer_entry_t* ptr(size_t handle)
            {
                timer_entry_t *ret=(timer_entry_t*)(handle&(~size_mask));
                if (ret->check(handle))
                    return ret;
                return NULL;
            }
            timer_entry_t():ttl(0){reset();}
        }timer_entry_t;

        //-------------------------------------------------
        //定义基础的时间轮功能封装
        class timing_wheel_t
        {
            item_list_t     slots[wheel_slots];             //时间轮中的槽位数组.每个槽位对应着一个定时器信息指针列表
            uint32_t        slot_ptr;                       //时间轮的槽指针,指向当前被处理过的时间槽
        public:
        };
        //-------------------------------------------------
    }






}

#endif
