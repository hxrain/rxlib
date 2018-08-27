﻿#ifndef _RX_TDD_TICK_H_
#define _RX_TDD_TICK_H_

#include "rx_cc_macro.h"
#include "rx_datetime_ex.h"

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <memory.h>

namespace rx
{
    //-----------------------------------------------------
    //滴答增量计时器,用于记录代码段执行用时
    template<class tick_type=rx_tick_us>
    class rx_tdd_tick
    {
        tick_type    m_tick_meter;                          //滴答计数器
        uint64_t     m_begin_tick;                          //计时开始的最初时间
        uint32_t     m_expend_limit;                        //显示输出的用时下限
        bool         m_enable;                              //是否开启计时功能
        const char*  m_msg_a;                               //打印输出中,告知功能大类
        const char*  m_msg_b;                               //打印输出中,告知功能小类
        uint32_t     m_tab_deep;                            //打印输出时,进行hit层次化缩进深度处理
        const char*  m_file;
        uint32_t     m_lineno;
    public:
        //-------------------------------------------------
        //构造函数,传入时间戳的限定值和消息提示
        rx_tdd_tick(bool enable=true,uint32_t expend_limit=0,const char* msg_a=NULL,const char* msg_b=NULL,const char* file=NULL,int lineno=0)
        {
            begin(enable,expend_limit,msg_a,msg_b,file,lineno);
        }
        //-------------------------------------------------
        //执行开始,记录开始时间与相关内容项
        void begin(bool enable=true,uint32_t expend_limit=0,const char* msg_a=NULL,const char* msg_b=NULL,const char* file=NULL,int lineno=0)
        {
            m_enable=enable;
            if (!m_enable) return;
            m_begin_tick=m_tick_meter.update();
            m_expend_limit=expend_limit;
            m_msg_a=msg_a;if (!m_msg_a) m_msg_a="";
            m_msg_b=msg_b;if (!m_msg_b) m_msg_b="";
            m_file=file;if (!m_file) m_file="";
            m_lineno=lineno;
        }
        uint32_t &tab_deep(){return m_tab_deep;}
        //-------------------------------------------------
        //进行计时的中间动作
        void hit(const char* msg_c=NULL,const char* file=NULL,int lineno=0)
        {
            if (!m_enable) return;
            uint64_t prv_tick=m_tick_meter.count();
            uint32_t dt=uint32_t(m_tick_meter.update()-prv_tick);
            if (dt<m_expend_limit)
                return;

            char tab_str[32];
            uint32_t tab_deep=m_tab_deep;
            if (tab_deep>sizeof(tab_str)-1)
                tab_deep=sizeof(tab_str)-1;
            memset(tab_str,' ',tab_deep);
            tab_str[tab_deep]=0;

            char tmp[512];
            snprintf(tmp,sizeof(tmp),"<tdd_ts>[%s]%s using [%3uus] at {%s}:(%s:%u).\n",m_msg_a,tab_str,dt,msg_c,file,lineno);
            on_output(tmp);
        }
        //-------------------------------------------------
        //执行结束,自动判断用时,并打印输出内容
        void end()
        {
            if (!m_enable) return;
            uint32_t dt=uint32_t(m_tick_meter.update()-m_begin_tick);
            if (dt<m_expend_limit)
                return;

            char tmp[512];
            snprintf(tmp,sizeof(tmp),"<tdd_ts>[%s] using [%3uus] at {%s}:(%s:%u).\n",m_msg_a,dt,m_msg_b,m_file,m_lineno);
            on_output(tmp);
            m_enable=false;
        }
        //-------------------------------------------------
        //析构函数,计算前后时间是否超过了限定值
        virtual ~rx_tdd_tick()
        {
            end();
        }
    protected:
        virtual void on_output(const char* str){printf(str);}
    };

    //-----------------------------------------------------
    //进行滴答计时的tab深度管理,同时简化输出时的参数数量
    template<class tdd_tick_t=rx_tdd_tick<> >
    class tdd_tick_guard
    {
        tdd_tick_t  &m_tdd_tick;
        const char* m_file;
        uint32_t    m_lineno;
    public:
        //-------------------------------------------------
        tdd_tick_guard(tdd_tick_t& tc,const char* file,uint32_t lineno,char* fmt,...):m_tdd_tick(tc)
        {
            va_list ap;
            va_start(fmt,ap);
            char msg_c[256];
            vsnprintf(msg_c,sizeof(msg_c),fmt,ap);
            m_tdd_tick.hit(msg_c,m_file,m_lineno);
            va_end(ap);

            ++m_tdd_tick.tab_deep();
        }
        ~tdd_tick_guard(){--m_tdd_tick.tab_deep();}
    };
}

    //-----------------------------------------------------
    #define rx_tt_enable true
    #define rx_tt_limit 0

    //用来定义一个tt滴答计时对象,可以给出完整的参数
    #define rx_tt_desc(sym,msg_a,msg_b,limit) rx::rx_tdd_tick<> __rx_tt_obj_##sym(rx_tt_enable,limit,msg_a,msg_b,__FILE__,__LINE__)
    //用于简化定义一个tt滴答计时对象,用时限制使用默认值
    #define rx_tt(sym,msg_a,msg_b) rx_tt_desc(sym,msg_a,msg_b,rx_tt_limit)
    //用于简化定义一个tt滴答计时对象的中间可嵌套计时动作
    #define rx_tt_hit(sym,fmt,...) rx::tdd_tick_guard<> __rx_tt_hit_obj_##sym(__rx_tt_obj_##sym,__FILE__,__LINE__,fmt,__VA_ARGS__)

#endif