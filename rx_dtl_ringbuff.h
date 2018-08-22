#ifndef _RX_DTL_RINGBUFF_H_
#define _RX_DTL_RINGBUFF_H_

#include "rx_cc_macro.h"
#include "rx_cc_atomic.h"
#include "rx_os_lock.h"
#include "rx_os_spinlock.h"


namespace rx
{
    //-----------------------------------------------------
    //https://github.com/willemt/bipbuffer/blob/master/bipbuffer.c
/*
    bipbuffer通过将定长数组动态划分为A/B两段区域,赋予了环形缓冲区的直接数据读取并能力,而不必进行尾段回绕导致的缓冲区取数据拷贝动作.
    从应用的角度看,数据的存入仍然需要memcpy,但取出操作不需要,代价是整体空间的利用率比真正的ringbuffer稍低,需更合理的预估数据长度.
    要求:每次放入与取出数据的动作,都需要避免回绕并避免过量.

    |{    B region      }     {     A region    }    |
    |                   |     |                 |    |
 m_data               b_end a_start           a_end  m_capacity
*/
    //-----------------------------------------------------
    //基于bipbuffer原理封装一个循环缓冲区功能对象
    class bipbuff_base
    {
    protected:
        uint8_t     b_inuse;                                //标记B区域是否被使用了
        uint32_t    b_end;                                  //记录B区域的结束点位置
        uint32_t    a_start;                                //记录A区域的开始点位置
        uint32_t    a_end;                                  //记录A区域的结束点位置
        uint32_t    m_capacity;                             //记录缓冲区的总容量
        uint8_t    *m_data;                                 //缓冲区指针
        //-------------------------------------------------
        //内部初始化,记录必要的信息
        void m_init(const uint32_t capacity,uint8_t *buff)
        {
            m_capacity = capacity;
            m_data = buff;
            clear();
        }
    private:
        //判断是否应该切换到B区
        void try_switch_to_b()
        {
            if (m_capacity - a_end < a_start - b_end)
                b_inuse = 1;  //如果A区的后部剩余空间小于A区前的B区可用空间,则切换到B区
        }

    public:
        //-------------------------------------------------
        //获取可用剩余空间尺寸(不是真正的整体剩余空间)
        uint32_t remain()
        {
            if (b_inuse)
                return a_start - b_end;                     //B区被使用的时候,可用的剩余空间就是B区的剩余空间
            else
                return m_capacity - a_end;                  //B区未被使用的时候,可用的剩余空间就是A区的剩余空间
        }
        //-------------------------------------------------
        //获取缓冲区的总体能力
        uint32_t capacity() { return m_capacity; }
        //-------------------------------------------------
        //获取可直接读取的数据长度(或数据总长度)
        uint32_t size(bool is_total=false) 
        { 
            if (is_total)
                return (a_end - a_start) + b_end;           //A区和B区的数据总长
            else
                return (a_end - a_start);                   //当前可读点总是在A区的
        }       
        //-------------------------------------------------
        //清空当前缓冲区
        void clear() { a_start = a_end = b_end = 0; b_inuse = 0; }
        //-------------------------------------------------
        //判断缓冲区是否为空
        bool empty() { return a_start == a_end; }
        //-------------------------------------------------
        //数据放入缓冲区(并进行B区的切换尝试)
        //返回值:是否放入成功(当前区剩余空间是否足够)
        bool push(const unsigned char *data, const uint32_t size)
        {
            if (remain() < size)
                return false;                               //必须进行可用剩余空间的判断

            if (b_inuse)
            {//B区被使用中,则直接将数据放在B区的尾部,B区增长
                memcpy(m_data + b_end, data, size);
                b_end += size;
            }
            else
            {//B区没有被使用,则将数据放在A区的尾部,A区增长
                memcpy(m_data + a_end, data, size);
                a_end += size;
                try_switch_to_b();                          //判断是否应该切换到B区
            }
            return true;
        }
        //-------------------------------------------------
        //从缓冲区中取出数据(或仅仅查看本区的剩余数据)
        uint8_t *pop(uint32_t size = 0, bool is_peek = false)
        {
            if (empty())
                return NULL;                                //没有数据直接返回空

            if (!size)
                size = this->size();                        //没有指定待获取尺寸的时候,就是访问当前的全部可用数据

            if (m_capacity < a_start + size)                //访问数据总是从A区进行
                return NULL;                                //A区没有可用的那么多数据时,也返回空.

            uint8_t *pos = m_data + a_start;                //记录数据访问点,必须从A区开始

            if (is_peek)
                return pos;                                 //查看模式直接返回

            a_start += size;                                //提取模式,则移动A区的开始点

            if (a_start == a_end)
            {//如果A区的数据被取空了
                if (b_inuse)
                {//如果B区被使用中,则处理B区转A区的动作
                    a_start = 0;
                    a_end = b_end;
                    b_end = b_inuse = 0;                    //关闭B区
                }
                else //如果B区没有被使用,则将A区移动到缓冲区的开始.
                    a_start = a_end = 0;
            }
            else //如果A区还有剩余数据,则尝试进行B区的切换
                try_switch_to_b();

            return pos;
        }
    };

    //-----------------------------------------------------
    //静态空间的环形缓冲区;容量CP.
    template<uint32_t CP = 256>
    class ringbuff_fixed :public bipbuff_base
    {
        uint8_t  m_buff[CP];                                //定义真正的缓冲区空间
    public:
        ringbuff_fixed() { bipbuff_base::m_init(CP,m_buff); }//构造时进行初始化
    };
}



#endif