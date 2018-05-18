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
//�����������������Ĭ��ʵ�֣�����̨��ӡ��
typedef void (*rx_tdd_out_fun_t)(const char* msg,va_list vl);
inline void rx_tdd_out_fun(const char* msg,va_list vl)
{
    vprintf(msg,vl);
}

class rx_tdd_base;

//---------------------------------------------------------
//����TDD����������״̬
typedef struct rx_tdd_stat
{
    unsigned int _total;                                    //��¼TDD��������
    unsigned int _perform;                                  //��¼TDD������ִ������
    unsigned int _failed;                                   //��¼����ʧ������
    unsigned int _assert;                                   //��¼�������еĴ���

    rx_tdd_base *head;                                      //ָ��TDD����������ͷ���
    rx_tdd_base *tail;                                      //ָ��TDD����������β���
    rx_tdd_base *runed;                                     //ָ��TDD������������ִ�н��

    rx_tdd_out_fun_t out_fun;                               //��¼������ݵĺ���ָ�룬�ɵ�����
    void out(const char* msg,...)
    {
        va_list v;
        va_start(v,msg);
        out_fun(msg,v);
        va_end(v);
    }

    rx_tdd_stat():_total(0),_perform(0),_failed(0),_assert(0),head(0),tail(0),runed(0),out_fun(rx_tdd_out_fun){}
}rx_tdd_stat;

//ʹ�ü򻯵ĵ���ģʽ����ȡTDD������״̬
inline rx_tdd_stat& _rx_tdd_stat()
{
    static rx_tdd_stat stat;
    return stat;
}

//---------------------------------------------------------
//ʵ��TDD�����Ļ���
class rx_tdd_base
{
    const char* m_tdd_name;
    const char* m_file_name;
    int         m_line_no;
    rx_tdd_base *m_next;
    void m_bind(rx_tdd_stat &root)
    {
        if (root.head==0)
        {//��ʼ��ʱ����TDD���������ͷ��β��ָ��ǰ����
            root.head=root.tail=this;
        }
        else
        {//��Ӻ���TDD���������ʱ��
            root.tail->m_next=this;                         //��������β�ڵ�ĺ���ָ��ǰ�ڵ�
            root.tail=this;                                 //���������β�ڵ�ָ��ǰ�ڵ�
        }
        m_next=0;                                           //��ǰ�ڵ�ĺ����ÿ�,����TDD�����������
        ++root._total;                                      //������������
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
    //ִ�б�����
    void exec()
    {
        try{on_exec();}
        catch(...)
        {
            _rx_tdd_stat().out("RX TDD <%d> at <%s : %d> => throw exception!\r\n",m_tdd_name,m_file_name,m_line_no);
            ++_rx_tdd_stat()._failed;                   //��׽�쳣ʱ��¼ʧ�ܴ���
        }
        ++_rx_tdd_stat()._perform;                      //ִ�й�������������
    }
    //-----------------------------------------------------
    //�ڲ����ԣ����ڼ�¼ʧ�ܴ���
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
    //��������ִ�о���Ĳ��Զ���
    virtual void on_exec()=0;
};

//����TDD����,���ͳ�ƽ��
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
//�﷨�ǣ����ڼ�TDD���Ե�ʹ��
#define assert(v) tdd_assert((v),__LINE__)

//����rx_tdd��,���ڱ�ݵĽ���һ��ָ�����ֵĲ�������
#define rx_tdd(name) class __RX_TDD_CLS__##name:public rx_tdd_base \
    {public:                                                    \
        __RX_TDD_CLS__##name():rx_tdd_base(#name,__FILE__,__LINE__){} \
        void on_exec();                                         \
    };                                                          \
    __RX_TDD_CLS__##name __RX_TDD_OBJ__##name;                  \
    inline void __RX_TDD_CLS__##name::on_exec()




#endif
