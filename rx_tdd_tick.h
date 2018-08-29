#ifndef _RX_TDD_TICK_H_
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
    template<class tick_type=rx_tick_us_t>
    class tdd_tick_t
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
        tdd_tick_t(bool enable=true,uint32_t expend_limit=0,const char* msg_a=NULL,const char* msg_b=NULL,const char* file=NULL,int lineno=0)
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
            m_tab_deep=0;
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
            uint32_t tab_deep=m_tab_deep+1;
            if (tab_deep>sizeof(tab_str)-1)
                tab_deep=sizeof(tab_str)-1;
            memset(tab_str,'.',tab_deep);
            tab_str[tab_deep]=0;

            char tmp[512];
            snprintf(tmp,sizeof(tmp),"<tdd_tt>{%s}:hit:%s (%6u)us {%s} : (%s:%u).\n",m_msg_a,tab_str,dt,msg_c,file,lineno);
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
            snprintf(tmp,sizeof(tmp),"<tdd_tt>{%s}:all: (%6u)us {%s} : (%s:%u).\n",m_msg_a,dt,m_msg_b,m_file,m_lineno);
            on_output(tmp);
            m_enable=false;
        }
        //-------------------------------------------------
        //析构函数,计算前后时间是否超过了限定值
        virtual ~tdd_tick_t()
        {
            end();
        }
    protected:
        virtual void on_output(const char* str){printf(str);}
    };

    //-----------------------------------------------------
    //进行滴答计时的tab深度管理,同时简化输出时的参数数量
    template<class tt=tdd_tick_t<> >
    class tdd_tick_guard_t
    {
        tt  &m_tdd_tick;
    public:
        //-------------------------------------------------
        tdd_tick_guard_t(tt& tc,const char* file,uint32_t lineno,const char* fmt,...):m_tdd_tick(tc)
        {
            va_list ap;
            va_start(ap,fmt);
            char msg_c[256];
            vsnprintf(msg_c,sizeof(msg_c),fmt,ap);
            m_tdd_tick.hit(msg_c,file,lineno);
            va_end(ap);

            ++m_tdd_tick.tab_deep();
        }
        ~tdd_tick_guard_t(){--m_tdd_tick.tab_deep();}
    };
}

    //-----------------------------------------------------
    #define tdd_tt_enable true                              //默认tdd计时是否开启
    #define tdd_tt_limit 0                                  //默认tdd计时的超时限定值

    //用来定义一个tt滴答计时对象,可以给出完整的参数
    #define tdd_tt_desc(sym,msg_a,msg_b,limit) rx::tdd_tick_t<> __tdd_tt_obj_##sym(tdd_tt_enable,limit,msg_a,msg_b,__FILE__,__LINE__)
    //用于简化定义一个tt滴答计时对象,用时限制使用默认值
    #define tdd_tt(sym,msg_a,msg_b) tdd_tt_desc(sym,msg_a,msg_b,tdd_tt_limit)
    //用于简化定义一个tt滴答计时对象的中间可嵌套计时动作
    #define _tdd_tt_hit(sym,dis,fmt,...) rx::tdd_tick_guard_t<> __rx_tdd_tick_hit_obj_##dis(__tdd_tt_obj_##sym,__FILE__,__LINE__,fmt,##__VA_ARGS__)
    //中间宏定义,为了进行dis的展开转换
    #define _tdd_tt_hit_(sym,dis,fmt,...) _tdd_tt_hit(sym,dis,fmt,##__VA_ARGS__)
    //定义rx_tdd_rtl宏,用于便捷的建立一个指定名字和运行级的测试用例
    #define tdd_tt_hit(sym,fmt,...) _tdd_tt_hit_(sym,RX_CT_SYM(sym),fmt,##__VA_ARGS__)

#endif
