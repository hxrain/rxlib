#ifndef _RX_TDD_H_
#define _RX_TDD_H_

/*rx_tdd unit test framework,usage:

#include "rx_tdd.h"

void ut_xx_base(rx_tdd_t &tdd)
{
    tdd.assert(1);
}

rx_tdd(ut_xx)
{
    assert(1);
    ut_xx_base(*this);
}

rx_tdd_rtl(ut_xx,tdd_level_slow)
{
    assert(1);
    ut_xx_base(*this);
}

void main()
{
    rx_tdd_run();
}
*/
#include "rx_cc_macro.h"
#include <stdarg.h>
#include <stdio.h>
//---------------------------------------------------------
//�����������������Ĭ��ʵ�֣�����̨��ӡ��
typedef void (*rx_tdd_out_fun_t)(const char* msg,va_list vl);
inline void rx_tdd_out_fun(const char* msg,va_list vl)
{
    vprintf(msg,vl);
}

class rx_tdd_t;

//---------------------------------------------------------
//����TDD����������״̬
typedef struct rx_tdd_stat_t
{
    unsigned int _total;                                    //��¼TDD��������
    unsigned int _perform;                                  //��¼TDD������ִ������
    unsigned int _failed;                                   //��¼����ʧ������
    unsigned int _assert;                                   //��¼�������еĴ���

    rx_tdd_t *head;                                      //ָ��TDD����������ͷ���
    rx_tdd_t *tail;                                      //ָ��TDD����������β���
    rx_tdd_t *runed;                                     //ָ��TDD������������ִ�н��

    rx_tdd_out_fun_t out_fun;                               //��¼������ݵĺ���ָ�룬�ɵ�����
    void out(const char* msg,...)
    {
        va_list v;
        va_start(v,msg);
        out_fun(msg,v);
        va_end(v);
    }

    rx_tdd_stat_t():_total(0),_perform(0),_failed(0),_assert(0),head(0),tail(0),runed(0),out_fun(rx_tdd_out_fun) {}
    static rx_tdd_stat_t& get() {static rx_tdd_stat_t st; return st;}
} rx_tdd_stat_t;

//---------------------------------------------------------
//TDD�ļ���,����ʱָ��,����ʱ��ѡ������ĳ������֮�µ�TDD
typedef enum rx_tdd_level
{
    tdd_level_base      = 0,                                //�������tdd����,������ʱ��,�ܻ���������
    tdd_level_std       = 1,                                //��׼��tdd����,����ռ��һЩʱ��,����ִ�б�Ҫ������
    tdd_level_slow      = 2,                                //������tdd����,�ǳ�ռ��ʱ��,�ڽ������������ܲ���
    tdd_level_ui        = 3,                                //��ҪUI�����ļ���,��Ա�뿪����������
} rx_tdd_level;

//---------------------------------------------------------
//ʵ��TDD�����Ļ���
class rx_tdd_t
{
    friend void rx_tdd_run(rx_tdd_level rtl,bool only_curr_level);

    const char* m_tdd_name;
    const char* m_file_name;
    int         m_line_no;
    rx_tdd_t *m_next;
    unsigned char m_wait_key;
    unsigned char m_level;

    void m_bind(rx_tdd_stat_t &root)
    {
        if (root.head==0)
        {
            //��ʼ��ʱ����TDD���������ͷ��β��ָ��ǰ����
            root.head=root.tail=this;
        }
        else
        {
            //��Ӻ���TDD���������ʱ��
            root.tail->m_next=this;                         //��������β�ڵ�ĺ���ָ��ǰ�ڵ�
            root.tail=this;                                 //���������β�ڵ�ָ��ǰ�ڵ�
        }
        m_next=0;                                           //��ǰ�ڵ�ĺ����ÿ�,����TDD�����������
        ++root._total;                                      //������������
    }

public:
    //-----------------------------------------------------
    rx_tdd_t(const char* tdd_name,rx_tdd_level rtl,const char* _file_name,const int _line_no)
    {
        m_tdd_name=tdd_name;
        m_file_name=_file_name;
        m_line_no=_line_no;
        m_wait_key=false;
        m_level=rtl;
        m_bind(rx_tdd_stat_t::get());
    }
    //-----------------------------------------------------
    //ִ�б�����
    void exec(rx_tdd_level rtl,bool only_curr_level=false)
    {
        if (rtl<m_level||(only_curr_level&&rtl!=m_level))
        {
            rx_tdd_stat_t::get().out("TDD::<*SKIP* %s> at <%s:%d>\r\n",m_tdd_name,m_file_name,m_line_no);
            return;
        }

        rx_tdd_stat_t::get().out("TDD::<       %s> at <%s:%d>\r\n",m_tdd_name,m_file_name,m_line_no);
        try
        {
            on_exec();
        }
        catch(...)
        {
            rx_tdd_stat_t::get().out("TDD::<*EXCEPTION* %s> at <%s:%d> => throw exception!\r\n",m_tdd_name,m_file_name,m_line_no);
            ++rx_tdd_stat_t::get()._failed;                   //��׽�쳣ʱ��¼ʧ�ܴ���
        }
        ++rx_tdd_stat_t::get()._perform;                      //ִ�й�������������
    }
    //-----------------------------------------------------
    //�ڲ����ԣ����ڼ�¼ʧ�ܴ���
    void assert(bool v) {assert(v,m_line_no,NULL);}
    void assert(bool v,int _line_no) {assert(v,_line_no,NULL);}
    void assert(bool v,int _line_no,const char* msg,...)
    {
        rx_tdd_stat_t &s=rx_tdd_stat_t::get();
        ++s._assert;
        if (v)
            return;
        ++s._failed;

        s.out("TDD::<*FAIL*>         {%s} at <%s : %d>\r\n",m_tdd_name,m_file_name,_line_no);
        if (msg&&msg[0])
        {
            s.out("    ");
            va_list v;
            va_start(v,msg);
            s.out_fun(msg,v);
            va_end(v);
            s.out("\r\n");
        }

        if (m_wait_key)
        {
            s.out("press enter key to continue...\r\n");
            getchar();
        }
    }
    //-----------------------------------------------------
    //���ִ����ʱ��,�Ƿ���������UI�ȴ�ȷ��
    void enable_error_wait(bool v=true) {m_wait_key=v;}
protected:
    //-----------------------------------------------------
    //��������ִ�о���Ĳ��Զ���
    virtual void on_exec()=0;
};

//---------------------------------------------------------
//����TDD����,���ͳ�ƽ��(��ָ��Ҫ�����еĵȼ�,���г�ʱ�����в��ԵĹ���)
inline void rx_tdd_run(rx_tdd_level rtl=tdd_level_slow,bool only_curr_level=false)
{
    rx_tdd_stat_t &s=rx_tdd_stat_t::get();
    rx_tdd_t *node=s.head;

    s.out("TDD::******************************[BEGIN]******************************\r\n");
    while(node)
    {
        node->exec(rtl, only_curr_level);
        node=node->m_next;
    }

    s.out(
        "TDD::******************************[STATE]******************************\r\n"
        "                OBJECT total <%6u> : perform <%6u>\r\n"
        "                ASSERT total <%6u> : failed  <%6u>\r\n"
        "TDD::******************************[ END ]******************************\r\n",
        s._total,s._perform,s._assert,s._failed);
}

//---------------------------------------------------------
//�����ݺ�,������������ߵ��к�
#define tdd_assert(v) assert((v),__LINE__)
#define msg_assert(v,msg,...) assert((v),__LINE__,msg,##__VA_ARGS__)

//---------------------------------------------------------
#define tdd_print(en,...) if (en) printf(__VA_ARGS__)
//---------------------------------------------------------
//����rx_tdd��,��ָ�����м���,���ڱ�ݵĽ���һ��ָ�����ֵĲ�������
#define _tdd_rtl_desc(name,memo,rtl,lineno) class __RX_TDD_CLS__##name:public rx_tdd_t \
    {public:                                                    \
        __RX_TDD_CLS__##name():rx_tdd_t(memo,rtl,__FILE__,lineno){} \
        void on_exec();                                         \
    };                                                          \
    __RX_TDD_CLS__##name __RX_TDD_OBJ__##name;                  \
    inline void __RX_TDD_CLS__##name::on_exec()

//�м�궨��,Ϊ�˽���name��չ��ת��
#define _tdd_desc(name,memo,rtl) _tdd_rtl_desc(name,memo,rtl,__LINE__)

//����rx_tdd_rtl��,���ڱ�ݵĽ���һ��ָ�����ֺ����м��Ĳ�������
#define rx_tdd_rtl(name,rtl) _tdd_desc(RX_CT_SYM(name),RX_CT_STR(name),rtl)

//����rx_tdd��,���ڱ�ݵĽ���һ��ָ�����ֵĲ�������
#define rx_tdd(name) _tdd_desc(RX_CT_SYM(name),RX_CT_STR(name),tdd_level_base)
#define rx_tddm(name,memo) _tdd_desc(RX_CT_SYM(name),memo,tdd_level_base)


#endif
