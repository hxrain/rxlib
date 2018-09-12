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
    //�δ�������ʱ��,���ڼ�¼�����ִ����ʱ
    template<class tick_type=rx_tick_us_t>
    class tdd_tick_t
    {
        tick_type    m_tick_meter;                          //�δ������
        uint64_t     m_begin_tick;                          //��ʱ��ʼ�����ʱ��
        uint32_t     m_expend_limit;                        //��ʾ�������ʱ����
        bool         m_enable;                              //�Ƿ�����ʱ����
        const char*  m_msg_a;                               //��ӡ�����,��֪���ܴ���
        const char*  m_msg_b;                               //��ӡ�����,��֪����С��
        uint32_t     m_tab_deep;                            //��ӡ���ʱ,����hit��λ�������ȴ���
        const char*  m_file;
        uint32_t     m_lineno;
    public:
        //-------------------------------------------------
        //���캯��,����ʱ������޶�ֵ����Ϣ��ʾ
        tdd_tick_t(bool enable=true,uint32_t expend_limit=0,const char* msg_a=NULL,const char* msg_b=NULL,const char* file=NULL,int lineno=0)
        {
            begin(enable,expend_limit,msg_a,msg_b,file,lineno);
        }
        //-------------------------------------------------
        //ִ�п�ʼ,��¼��ʼʱ�������������
        void begin(bool enable=true,uint32_t expend_limit=0,const char* msg_a=NULL,const char* msg_b=NULL,const char* file=NULL,int lineno=0)
        {
            m_enable=enable;
            if (!m_enable)
                return;
            m_begin_tick=m_tick_meter.update();
            m_expend_limit=expend_limit;
            m_msg_a=msg_a;
            if (!m_msg_a)
                m_msg_a="";
            m_msg_b=msg_b;
            if (!m_msg_b)
                m_msg_b="";
            m_file=file;
            if (!m_file)
                m_file="";
            m_lineno=lineno;
            m_tab_deep=0;
        }
        uint32_t &tab_deep() {return m_tab_deep;}
        //-------------------------------------------------
        //���м�ʱ���м䶯��
        void hit(const char* msg_c=NULL,const char* file=NULL,int lineno=0)
        {
            if (!m_enable)
                return;
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
            snprintf(tmp,sizeof(tmp),"<tdd_tt>{%s}:hit:%s (%.1f)ms {%s} : (%s:%u).\n",m_msg_a,tab_str,dt / 1000.0,msg_c,file,lineno);
            on_output(tmp);
        }
        //-------------------------------------------------
        //ִ�н���,�Զ��ж���ʱ,����ӡ�������
        void end()
        {
            if (!m_enable)
                return;
            uint32_t dt=uint32_t(m_tick_meter.update()-m_begin_tick);
            if (dt<m_expend_limit)
                return;

            char tmp[512];
            snprintf(tmp,sizeof(tmp),"<tdd_tt>{%s}:all: (%.1f)ms {%s} : (%s:%u).\n",m_msg_a,dt/1000.0,m_msg_b,m_file,m_lineno);
            on_output(tmp);
            m_enable=false;
        }
        //-------------------------------------------------
        //��������,����ǰ��ʱ���Ƿ񳬹����޶�ֵ
        virtual ~tdd_tick_t()
        {
            end();
        }
    protected:
        virtual void on_output(const char* str) {printf(str);}
    };

    //-----------------------------------------------------
    //���еδ��ʱ��tab��ȹ���,ͬʱ�����ʱ�Ĳ�������
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
        ~tdd_tick_guard_t() {--m_tdd_tick.tab_deep();}
    };
}

//-----------------------------------------------------
#define tdd_tt_enable true                              //Ĭ��tdd��ʱ�Ƿ���
#define tdd_tt_limit 0                                  //Ĭ��tdd��ʱ�ĳ�ʱ�޶�ֵ

//��������һ��tt�δ��ʱ����,���Ը��������Ĳ���
#define tdd_tt_desc(sym,msg_a,msg_b,limit) rx::tdd_tick_t<> __tdd_tt_obj_##sym(tdd_tt_enable,limit,msg_a,msg_b,__FILE__,__LINE__)
//���ڼ򻯶���һ��tt�δ��ʱ����,��ʱ����ʹ��Ĭ��ֵ
#define tdd_tt(sym,msg_a,msg_b) tdd_tt_desc(sym,msg_a,msg_b,tdd_tt_limit)
#define tt(msg_a) tdd_tt(tt,msg_a,"")

//���ڼ򻯶���һ��tt�δ��ʱ������м��Ƕ�׼�ʱ����
#define _tdd_tt_hit(sym,dis,fmt,...) rx::tdd_tick_guard_t<> __rx_tdd_tick_hit_obj_##dis(__tdd_tt_obj_##sym,__FILE__,__LINE__,fmt,##__VA_ARGS__)
//�м�궨��,Ϊ�˽���dis��չ��ת��
#define _tdd_tt_hit_(sym,dis,fmt,...) _tdd_tt_hit(sym,dis,fmt,##__VA_ARGS__)
//����rx_tdd_rtl��,���ڱ�ݵĽ���һ��ָ�����ֺ����м��Ĳ�������
#define tdd_tt_hit(sym,fmt,...) _tdd_tt_hit_(sym,RX_CT_SYM(sym),fmt,##__VA_ARGS__)

#endif
