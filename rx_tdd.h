#ifndef _RX_TDD_H_
#define _RX_TDD_H_

/*rx_tdd unit test framework,usage:

rx_tdd(test_name1)
{
    assert(1);
}

void test_1(rx_tdd_base &tdd)
{
    tdd.assert(1);
}

rx_tdd(test_name2)
{
    assert(1);
    test_1(*this);
}

void main()
{
    rx_tdd_run();
}
*/

#include <stdarg.h>
#include <stdio.h>
//---------------------------------------------------------
//调试输出函数类型与默认实现（控制台打印）
typedef void (*rx_tdd_out_fun_t)(const char* msg,va_list vl);
inline void rx_tdd_out_fun(const char* msg,va_list vl)
{
    vprintf(msg,vl);
}

class rx_tdd_base;

//---------------------------------------------------------
//定义TDD用例的运行状态
typedef struct rx_tdd_stat
{
    unsigned int _total;                                    //记录TDD用例总数
    unsigned int _perform;                                  //记录TDD用例已执行数量
    unsigned int _failed;                                   //记录断言失败数量
    unsigned int _assert;                                   //记录断言运行的次数

    rx_tdd_base *head;                                      //指向TDD用例的链表头结点
    rx_tdd_base *tail;                                      //指向TDD用例的链表尾结点
    rx_tdd_base *runed;                                     //指向TDD用例的链表已执行结点

    rx_tdd_out_fun_t out_fun;                               //记录输出内容的函数指针，可调整。
    void out(const char* msg,...)
    {
        va_list v;
        va_start(v,msg);
        out_fun(msg,v);
        va_end(v);
    }

    rx_tdd_stat():_total(0),_perform(0),_failed(0),_assert(0),head(0),tail(0),runed(0),out_fun(rx_tdd_out_fun){}
}rx_tdd_stat;

//使用简化的单件模式，获取TDD的运行状态
inline rx_tdd_stat& _rx_tdd_stat()
{
    static rx_tdd_stat stat;
    return stat;
}

//---------------------------------------------------------
//实现TDD用例的基类
class rx_tdd_base
{
    const char* m_tdd_name;
    const char* m_file_name;
    int         m_line_no;
    rx_tdd_base *m_next;
    void m_bind(rx_tdd_stat &root)
    {
        if (root.head==0)
        {//初始的时候，让TDD用例链表的头和尾都指向当前对象
            root.head=root.tail=this;
        }
        else
        {//添加后续TDD用例对象的时候
            root.tail->m_next=this;                         //用例链表尾节点的后趋指向当前节点
            root.tail=this;                                 //用例链表的尾节点指向当前节点
        }
        m_next=0;                                           //当前节点的后趋置空,代表TDD用例链表结束
        ++root._total;                                      //用例总数增加
    }

public:
    //-----------------------------------------------------
    rx_tdd_base(const char* tdd_name,const char* _file_name,const int _line_no)
    {
        m_tdd_name=tdd_name;
        m_file_name=_file_name;
        m_line_no=_line_no;
        m_bind(_rx_tdd_stat());
    }
    //-----------------------------------------------------
    //执行本用例
    void exec()
    {
        try{on_exec();}
        catch(...)
        {
            _rx_tdd_stat().out("RX TDD <%d> at <%s : %d> => throw exception!\r\n",m_tdd_name,m_file_name,m_line_no);
            ++_rx_tdd_stat()._failed;                   //捕捉异常时记录失败次数
        }
        ++_rx_tdd_stat()._perform;                      //执行过的用例数增加
    }
    //-----------------------------------------------------
    //内部断言，用于记录失败次数
    void tdd_assert(bool v,int _line_no)
    {
        rx_tdd_stat &s=_rx_tdd_stat();
        ++s._assert;
        if (v) return;
        _rx_tdd_stat().out("RX TDD <%s> at <%s : %d> => assert fail!\r\n",m_tdd_name,m_file_name,_line_no);
        ++_rx_tdd_stat()._failed;
    }
protected:
    friend void rx_tdd_run();
    //-----------------------------------------------------
    //子类用于执行具体的测试动作
    virtual void on_exec()=0;
};

//运行TDD用例,输出统计结果
inline void rx_tdd_run()
{
    rx_tdd_stat &s=_rx_tdd_stat();
    rx_tdd_base *node=s.head;
    if (!node)
    {
        s.out("\r\n===========================================\r\n");
        s.out("RX TDD object is empty!\r\n");
        s.out("\r\n===========================================\r\n");
        return;
    }

        s.out("RX TDD BEGIN===============================\r\n");
        while(node)
        {
            node->exec();
            node=node->m_next;
        }
        s.out("RX TDD END=================================\r\n");

        s.out(
              "RX TDD RESULT:\r\n"
              "       OBJECT total <%4d> : perform <%4d>\r\n"
              "       ASSERT total <%4d> : failed  <%4d>\r\n"
              "RX TDD COMPLETE============================\r\n",
              s._total,s._perform,s._assert,s._failed);
}

//---------------------------------------------------------
//语法糖，用于简化TDD断言的使用
#define assert(v) tdd_assert((v),__LINE__)

//定义rx_tdd宏,用于便捷的建立一个指定名字的测试用例
#define rx_tdd(name) class __RX_TDD_CLS__##name:public rx_tdd_base \
    {public:                                                    \
        __RX_TDD_CLS__##name():rx_tdd_base(#name,__FILE__,__LINE__){} \
        void on_exec();                                         \
    };                                                          \
    __RX_TDD_CLS__##name __RX_TDD_OBJ__##name;                  \
    inline void __RX_TDD_CLS__##name::on_exec()




#endif
