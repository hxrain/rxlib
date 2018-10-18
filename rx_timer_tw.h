#ifndef _RX_TIMER_TW_H_
#define _RX_TIMER_TW_H_
#include "list"
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

        //轮子指针转动事件委托
        typedef delegate1_rt<timer_entry_t&, void> step_delegate_t;

        //轮子转动一周事件委托
        typedef delegate1_rt<uint32_t, void> round_delegate_t;

        const uint32_t max_wheels  = 4;                     //最大的轮子数量
        const uint32_t wheel_slots = 256;                   //每个轮子上的槽位数量
        const uint32_t slot_mask   = 0xFF;                  //槽位索引掩码,用于取模计算
        const uint32_t max_rounds  = 65535;                 //最大允许的溢出轮数

        //-------------------------------------------------
        //内部定时器信息项
        typedef struct timer_entry_t
        {
            static const uint8_t size_mask = 0x1F;          //本条目的尺寸掩码(2的5次方,应<=sizeof(*this))
            //---------------------------------------------
            //外部用户提供的参数
            timer_delegate_t      u_cb_func;                //定时器委托回调
            uint32_t              u_interval;               //定时间隔时间,ms
            uint32_t              u_event_code;             //用户给定的事件码
            uint16_t              u_repeat;                 //重复执行次数,-1为永远执行

                                                            //---------------------------------------------
                                                            //内部工作需要的参数
            uint16_t              c_round;                  //触发本定时器需要的剩余轮数,为0时才能触发
            uint8_t               c_dummy;
            uint8_t               c_ttl;                    //本条目的生存计数
            uint8_t               w_wheel_idx;              //本条目所属的轮子编号
            uint8_t               w_slot_idx;               //本条目所属的槽位索引
            item_list_t::iterator w_slot_link;              //记录本节点在时间槽链表中的位置,结合slot_idx便于反向查找

            //---------------------------------------------
            //内部信息清除,准备再次复用
            void reset()
            {
                u_cb_func.reset();
                u_interval=0;
                u_event_code=0;
                u_repeat=0;

                c_round=0;
                c_ttl = (c_ttl + 1)&size_mask;              //生存计数不清零,递增取模

                w_wheel_idx = -1;
                w_slot_idx = 0;
                w_slot_link = NULL;
            }
            //---------------------------------------------
            //获取本条目的对外句柄
            size_t handle()
            {
                size_t ret=(size_t)this;
                rx_assert((ret&size_mask)==0);
                return ret | c_ttl;                         //借用指针低5bit存放ttl,等待访问时来验证
            }
            //---------------------------------------------
            //校验对外句柄的有效性
            bool check(size_t handle)
            {
                return (handle&size_mask)== c_ttl;
            }
            //---------------------------------------------
            //静态方法,根据定时器句柄计算内部信息对象指针
            static timer_entry_t* to_ptr(size_t handle)
            {
                timer_entry_t *ret=(timer_entry_t*)(handle&(~size_mask));
                if (ret->check(handle))
                    return ret;
                return NULL;
            }
            //---------------------------------------------
            timer_entry_t():c_ttl(0){reset();}
        }timer_entry_t;

        //-------------------------------------------------
        //基础的时间轮功能封装
        class wheel_t
        {
            item_list_t   m_slots[wheel_slots];             //时间轮中的槽位数组.每个槽位对应着一个定时器信息指针列表
            uint32_t      m_slot_ptr;                       //时间轮的槽指针,指向当前被处理过的时间槽
            uint32_t      m_wheel_idx;                      //轮子序号
        public:
            wheel_t():m_wheel_idx(-1){}
            step_delegate_t     cb_step;                    //指针转动事件,需要处理一个条目
            round_delegate_t    cb_round;                   //轮子转动一周事件,可以驱动上层轮子
            //---------------------------------------------
            //初始化
            bool init(uint32_t wheel_idx, mem_allotter_i& mem)
            {
                if (m_wheel_idx != (uint32_t)-1)
                    return false;
                m_slot_ptr = 0;
                m_wheel_idx = wheel_idx;
                ct::AC(m_slots, wheel_slots, mem);          //!!强制!!"重新构造初始化"槽位数组,绑定内存分配器
                return true;
            }
            //---------------------------------------------
            //解除
            void uninit()
            {
                for (uint32_t i = 0; i < wheel_slots; ++i)
                    m_slots[i].clear();
                m_wheel_idx = -1;
                m_slot_ptr = 0;
                cb_round.reset();
                cb_step.reset();
            }
            //---------------------------------------------
            //获取当前轮子的槽位指向位置
            uint32_t slot_ptr() { return m_slot_ptr; }
            //---------------------------------------------
            //将给定的item安装在给定的槽位中
            bool insert(timer_entry_t& item, uint8_t slot_idx)
            {
                rx_assert(slot_idx < wheel_slots);
                if (slot_idx >= wheel_slots)
                    return false;

                //在指定的槽位上插入条目
                item_list_t &slot = m_slots[slot_idx];
                item.w_slot_link = slot.push_back(&item);
                if (item.w_slot_link == slot.end())
                {
                    item.w_slot_link = NULL;
                    return false;
                }

                //记录条目的轮槽信息
                item.w_wheel_idx = m_wheel_idx;
                item.w_slot_idx = slot_idx;
                return true;
            }
            //---------------------------------------------
            //将给定的item从槽位中摘除
            bool remove(timer_entry_t& item)
            {
                if (item.w_slot_idx >= wheel_slots || item.w_wheel_idx!=m_wheel_idx ||
                    item.w_slot_link == NULL)
                    return false;

                //将条目从旧槽中移除
                item_list_t &slot = m_slots[item.w_slot_idx];
                item_list_t::iterator I = item.w_slot_link;
                if (!slot.earse(I))
                    return false;

                //抹除条目的轮槽信息
                item.w_wheel_idx = -1;
                item.w_slot_idx = -1;
                item.w_slot_link = NULL;

                return true;
            }
            //---------------------------------------------
            //将指定的条目移动到新的槽位
            bool move(timer_entry_t& item,uint8_t slot_idx)
            {
                rx_assert(slot_idx < wheel_slots);
                if (slot_idx >= wheel_slots || item.w_slot_idx >= wheel_slots ||
                    item.w_wheel_idx != m_wheel_idx || item.w_slot_link == NULL)
                    return false;

                //将条目从旧槽移除
                {
                item_list_t &slot = m_slots[item.w_slot_idx];
                item_list_t::iterator I = item.w_slot_link;
                if (!slot.earse(I))
                    return false;
                }

                //条目放入新槽
                item_list_t &slot = m_slots[slot_idx];
                item.w_slot_link = slot.push_back(&item);
                if (item.w_slot_link == slot.end())
                {
                    item.w_slot_link = NULL;
                    return false;
                }
                item.w_slot_idx = slot_idx;
                return true;
            }
            //---------------------------------------------
            //驱动当前轮子中的槽位指针前进一步,遍历新槽位中的全部条目
            uint32_t step()
            {
                //槽位指针前移一步
                m_slot_ptr = (m_slot_ptr + 1) & slot_mask;
                if (m_slot_ptr == 0)
                    cb_round(m_wheel_idx);                  //上一轮结束了,新一轮开始给出事件,可用于驱动上层轮子的转动

                item_list_t &slot = m_slots[m_slot_ptr];
                uint32_t rc=0;
                for (item_list_t::iterator I = slot.begin(); I != slot.end();)
                {
                    timer_entry_t &item = *(*I);
                    ++I;                                    //迭代器预先后移,避免在cb_step中item被删除时产生干扰.

                    if (item.c_round)
                    {//仍有剩余轮数,继续等待下一次吧
                        --item.c_round;
                        continue;
                    }

                    ++rc;
                    cb_step(item);                          //给出外部事件动作
                }
                return rc;
            }
        };
        //-------------------------------------------------
    }

    //-----------------------------------------------------
    //时间轮定时器
    //-----------------------------------------------------
    class timing_wheel_t
    {
        tw::wheel_t     m_wheels[tw::max_wheels];           //定义多级(max_wheels)轮子数组,0为最低级;每级轮子间时间速率相差256(wheel_slots)倍
        uint64_t        m_last_step_time;                   //上一次的动作发生时间,us.
        uint32_t        m_slot_unit_us;                     //0级轮子时间槽的时间单位,用us表示.
        //-------------------------------------------------
        //处理一个定时器条目
        void on_step(tw::timer_entry_t& item)
        {

        }
        //-------------------------------------------------
        //处理一次轮子转动(级联触发时此方法可能会被递归调用max_wheels层)
        void on_round(uint32_t wi)
        {
            if (wi<tw::max_wheels-1)
                m_wheels[wi+1].step();                      //级联驱动上层时间轮
        }
    public:
        //-------------------------------------------------
        //默认构造不初始化(需明确给出初始时间才进行构造初始化).
        timing_wheel_t(uint64_t curr_time_us=0,uint32_t slot_unit_us=1000,mem_allotter_i& ma=rx_global_mem_allotter())
        {
            if (curr_time_us)
                wheels_init(curr_time_us,slot_unit_us,ma);
        }
        ~timing_wheel_t(){wheels_uninit();}
        //-------------------------------------------------
        //时间轮初始化:给出初始时间点;内部时间槽的时间单位(默认1ms);时间轮使用的内存分配器.
        bool wheels_init(uint64_t curr_time_us,uint32_t slot_unit_us=1000,mem_allotter_i& mem=rx_global_mem_allotter())
        {
            for(uint32_t i=0;i<tw::max_wheels;++i)
            {
                tw::wheel_t  &w=m_wheels[i];
                if (!w.init(i,mem))
                    return false;
                w.cb_round.bind(*this,&timing_wheel_t::on_round);
                w.cb_step.bind(*this,&timing_wheel_t::on_step);

            }

            m_last_step_time = curr_time_us;
            m_slot_unit_us = slot_unit_us;

            return true;
        }
        //-------------------------------------------------
        //解除
        void wheels_uninit()
        {
            for(uint32_t i=0;i<tw::max_wheels;++i)
            {
                tw::wheel_t  &w=m_wheels[i];
                w.uninit();
            }
        }
        //-------------------------------------------------
        //驱动当前时间轮:给出当前时间.
        //返回值:真正触发行走的步数
        uint32_t wheels_step(uint64_t curr_time_us)
        {
            //计算流逝时间
            uint64_t delta=curr_time_us-m_last_step_time;
            //计算应该走动的步数
            uint32_t step_count=uint32_t(delta/m_slot_unit_us);

            //驱动最低级时间轮行走(在round回调中会同步驱动上级时间轮)
            for(uint32_t i=0;i<step_count;++i)
                m_wheels[0].step();

            //调整最后行走的时间点
            m_last_step_time+=step_count*m_slot_unit_us;
            return step_count;
        }
        //-------------------------------------------------
        //创建一个新的定时器
        size_t timer_insert()
        {

        }
        //-------------------------------------------------
        //删除一个定时器
        bool timer_remove(size_t timer)
        {

        }
        //-------------------------------------------------
        //更新一个定时器(以当前时间点和触发间隔,更新下一次的触发时刻)
        bool timer_update(size_t timer)
        {

        }
    };




}

#endif
