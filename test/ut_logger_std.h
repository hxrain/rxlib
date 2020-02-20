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
    //使用日志记录器接口进行日志输出功能的测试
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
    //构造日志输出器实体
    rx::logger_wrcon_t<> wrcon;
    rx::logger_wrfile_t<> wrfle;
    rt.tdd_assert(wrfle.open("./tmp.log"));

    //构造日志记录器实体,绑定输出器
    rx::logger_t<> logger;
    logger_modname(logger);
    logger.bind(wrcon);
    logger.bind(wrfle);

    //调用日志记录器接口测试函数
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
