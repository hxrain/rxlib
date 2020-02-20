#ifndef _RX_UT_LOGGER_STD_H_
#define _RX_UT_LOGGER_STD_H_

#include "../rx_tdd.h"
#include "../rx_logger_std.h"
#include "../rx_cc_base.h"

inline void rx_test_logger_stdout_1(rx_tdd_t &rt)
{
    rx::logger_wrcon_t<> wrcon;
    rx::logger_wrfile_t<> wrfle;
    rt.tdd_assert(wrfle.open("./tmp.log"));
    rx::logger_t<> logger;
    logger_modname(logger);
    logger.bind(wrcon);
    logger.bind(wrfle);

    logger.info("this's logger stdout unit test case. INFO!");
    logger.debug("this's logger stdout unit test case. DEBUG!");
    logger.warn("this's logger stdout unit test case. WARNING!");
    logger.cycle("this's logger stdout unit test case. CYCLE!");
    logger.error("this's logger stdout unit test case. ERROR!");

    uint8_t tmp[129];
    for(uint32_t i=0;i<sizeof(tmp);++i)
        tmp[i]=i;
    logger.writer.begin().fmt("recv stream hex:\n").hex(tmp,sizeof(tmp)).end();
    logger.writer.begin().fmt("recv bin:").bin(tmp,8).end();
}


inline void rx_test_logger_stdout_2(rx_tdd_t &rt,rx::logger_i *out=NULL)
{
    //ʹ����־��¼���ӿڽ�����־������ܵĲ���
    rx::logger_i logger;
    logger_modname(logger);
    if (out)
        logger.bind(*out);
    logger.info("sizeof(rx::logger_i)=%u",sizeof(rx::logger_i));
    logger.info("this's logger stdout unit test case. INFO!");
    logger.debug("this's logger stdout unit test case. DEBUG!");
    logger.warn("this's logger stdout unit test case. WARNING!");
    logger.cycle("this's logger stdout unit test case. CYCLE!");
    logger.error("this's logger stdout unit test case. ERROR!");

    uint8_t tmp[63];
    for(uint32_t i=0;i<sizeof(tmp);++i)
        tmp[i]=i;
    logger.writer.begin().fmt("recv stream hex:\n").hex(tmp,sizeof(tmp)).end();
    logger.writer.begin().fmt("recv bin:").bin(tmp,8).end();
}


inline void rx_test_logger_stdout_2A(rx_tdd_t &rt)
{
    //������־�����ʵ��
    rx::logger_wrcon_t<> wrcon;
    rx::logger_wrfile_t<> wrfle;
    rt.tdd_assert(wrfle.open("./tmp.log"));

    //������־��¼��ʵ��,�������
    rx::logger_t<> logger;
    logger_modname(logger);
    logger.bind(wrcon);
    logger.bind(wrfle);

    //������־��¼���ӿڲ��Ժ���
    rx_test_logger_stdout_2(rt);
    rx_test_logger_stdout_2(rt,&logger);

    logger.info("end");
}
rx_tdd(test_logger_std_base)
{
    rx_test_logger_stdout_1(*this);
    rx_test_logger_stdout_2A(*this);
}
#endif // _RX_UT_LOGGER_STD_H_
