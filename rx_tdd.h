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
//调试输出函数类型与默认实现（控制台打印）
typedef void (*rx_tdd_out_fun_t)(const char* msg,va_list vl);
inline void rx_tdd_out_fun(const char* msg,va_list vl)
{
    vprintf(msg,vl);
}

class rx_tdd_t;

//---------------------------------------------------------
//定义TDD用例的运行状态
typedef struct rx_tdd_stat_t
{
    unsigned int _total;                                    //记录TDD用例总数
    unsigned int _perform;                                  //记录TDD用例已执行数量
    unsigned int _failed;                                   //记录断言失败数量
    unsigned int _assert;                                   //记录断言运行的次数

    rx_tdd_t *head;                                      //指向TDD用例的链表头结点
    rx_tdd_t *tail;                                      //指向TDD用例的链表尾结点
    rx_tdd_t *runed;                                     //指向TDD用例的链表已执行结点

    rx_tdd_out_fun_t out_fun;                               //记录输出内容的函数指针，可调整。
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
//TDD的级别,定义时指定,运行时可选择运行某个级别之下的TDD
typedef enum rx_tdd_level
{
    tdd_level_base      = 0,                                //最基础的tdd级别,不耽误时间,很基础的用例
    tdd_level_std       = 1,                                //标准的tdd级别,稍稍占用一些时间,但会执行必要的用例
    tdd_level_slow      = 2,                                //很慢的tdd级别,非常占用时间,在进行完整的性能测试
    tdd_level_ui        = 3,                                //需要UI交互的级别,人员离开后会出现阻塞
} rx_tdd_level;

//---------------------------------------------------------
//实现TDD用例的基类
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
            //初始的时候，让TDD用例链表的头和尾都指向当前对象
            root.head=root.tail=this;
        }
        else
        {
            //添加后续TDD用例对象的时候
            root.tail->m_next=this;                         //用例链表尾节点的后趋指向当前节点
            root.tail=this;                                 //用例链表的尾节点指向当前节点
        }
        m_next=0;                                           //当前节点的后趋置空,代表TDD用例链表结束
        ++root._total;                                      //用例总数增加
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
    //执行本用例
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
            ++rx_tdd_stat_t::get()._failed;                   //捕捉异常时记录失败次数
        }
        ++rx_tdd_stat_t::get()._perform;                      //执行过的用例数增加
    }
    //-----------------------------------------------------
    //内部断言，用于记录失败次数
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
    //出现错误的时候,是否提升进行UI等待确认
    void enable_error_wait(bool v=true) {m_wait_key=v;}
protected:
    //-----------------------------------------------------
    //子类用于执行具体的测试动作
    virtual void on_exec()=0;
};

//---------------------------------------------------------
//运行TDD用例,输出统计结果(可指定要求运行的等级,进行长时间运行测试的过滤)
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
//定义便捷宏,方便输出调用者的行号
#define tdd_assert(v) assert((v),__LINE__)
#define msg_assert(v,msg,...) assert((v),__LINE__,msg,##__VA_ARGS__)

//---------------------------------------------------------
#define tdd_print(en,...) if (en) printf(__VA_ARGS__)
//---------------------------------------------------------
//定义rx_tdd宏,并指定运行级别,用于便捷的建立一个指定名字的测试用例
#define _tdd_rtl_desc(name,memo,rtl,lineno) class __RX_TDD_CLS__##name:public rx_tdd_t \
    {public:                                                    \
        __RX_TDD_CLS__##name():rx_tdd_t(memo,rtl,__FILE__,lineno){} \
        void on_exec();                                         \
    };                                                          \
    __RX_TDD_CLS__##name __RX_TDD_OBJ__##name;                  \
    inline void __RX_TDD_CLS__##name::on_exec()

//中间宏定义,为了进行name的展开转换
#define _tdd_desc(name,memo,rtl) _tdd_rtl_desc(name,memo,rtl,__LINE__)

//定义rx_tdd_rtl宏,用于便捷的建立一个指定名字和运行级的测试用例
#define rx_tdd_rtl(name,rtl) _tdd_desc(RX_CT_SYM(name),RX_CT_STR(name),rtl)

//定义rx_tdd宏,用于便捷的建立一个指定名字的测试用例
#define rx_tdd(name) _tdd_desc(RX_CT_SYM(name),RX_CT_STR(name),tdd_level_base)
#define rx_tddm(name,memo) _tdd_desc(RX_CT_SYM(name),memo,tdd_level_base)


#endif
