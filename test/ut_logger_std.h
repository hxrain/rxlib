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

    uint8_t tmp[1024];
    for(uint32_t i=0;i<sizeof(tmp);++i)
        tmp[i]=i;
    logger.writer.begin().fmt("recv stream hex:\n").hex(tmp,sizeof(tmp)).end();
    logger.writer.begin().fmt("recv bin:").bin(tmp,8).end();
}

rx_tdd(test_logger_std_base)
{
    rx_test_logger_stdout_1(*this);
}
#endif // _RX_UT_LOGGER_STD_H_
