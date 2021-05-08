#ifndef _RX_UT_LOGGER_STD_H_
#define _RX_UT_LOGGER_STD_H_

#include "../rx_tdd.h"
#include "../rx_logger.h"
#include "../rx_logger_ex.h"
#include "../rx_cc_base.h"

inline void rx_test_logger_stdout_1(rx_tdd_t &rt)
{
    rx::logger_wrcon_t<> wrcon;
    rx::logger_wrfile_t<> wrfle;
    rt.tdd_assert(wrfle.open("./tmp.log"));
    rx::logger_master_t<> logger_master;
	logger_master.bind(wrcon);
	logger_master.bind(wrfle);

	rx::logger_t logger;
    logger_modname(logger);

    logger.info("this's logger stdout unit test case. INFO!");
    logger.debug("this's logger stdout unit test case. DEBUG!");
    logger.warn("this's logger stdout unit test case. WARNING!");
    logger.cycle("this's logger stdout unit test case. CYCLE!");
    logger.error("this's logger stdout unit test case. ERROR!");

    uint8_t tmp[129];
    for(uint32_t i=0;i<sizeof(tmp);++i)
        tmp[i]=i;
    logger.recoder.begin().fmt("recv stream hex:\n").hex(tmp,sizeof(tmp)).end();
    logger.recoder.begin().fmt("recv bin:").bin(tmp,8).end();
}

inline void rx_test_logger_stdout_2(rx_tdd_t &rt,rx::logger_master_i *out=NULL)
{
    //使用日志记录器接口进行日志输出功能的测试
    rx::logger_t logger;
    logger_modname(logger);
    if (out)
        logger.bind(*out);
    logger.info("sizeof(rx::logger_t)=%u",sizeof(rx::logger_t));
    logger.info("sizeof(rx::logger_master_t)=%u",sizeof(rx::logger_master_t<>));
    logger.info("this's logger stdout unit test case. INFO!");
    logger.debug("this's logger stdout unit test case. DEBUG!");
    logger.warn("this's logger stdout unit test case. WARNING!");
    logger.cycle("this's logger stdout unit test case. CYCLE!");
    logger.error("this's logger stdout unit test case. ERROR!");

    uint8_t tmp[63];
    for(uint32_t i=0;i<sizeof(tmp);++i)
        tmp[i]=i;
    logger.recoder.begin().fmt("recv stream hex:\n").hex(tmp,sizeof(tmp)).end();
    logger.recoder.begin().fmt("recv bin:").bin(tmp,8).end();
}

inline void rx_test_logger_stdout_2A(rx_tdd_t &rt)
{
    //构造日志输出器实体
    rx::logger_wrcon_t<> wrcon;
    rx::logger_wrfile_t<> wrfle;
    rt.tdd_assert(wrfle.open("./tmp.log"));

    //构造日志记录器实体,绑定输出器
    rx::logger_master_t<> logger_master;
	rx::logger_t logger(logger_master);
    logger_modname(logger);
	logger_master.bind(wrcon);
	logger_master.bind(wrfle);

    //调用日志记录器接口测试函数
    rx_test_logger_stdout_2(rt);
    rx_test_logger_stdout_2(rt,&logger_master);

    logger.info("end");
}

//测试便捷日志记录器生成函数的使用
inline void rx_test_logger_stdout_2B(rx_tdd_t &rt)
{
    {
        rx::logger_t &logger = rx::make_logger_con();
        logger_modname(logger);
        rx_test_logger_stdout_2(rt,logger.master());
        logger.debug("logger_con end");
    }

    {
        rx::logger_t &logger = rx::make_logger_file<0>("./log0.txt");
        logger_modname(logger);
        rx_test_logger_stdout_2(rt,logger.master());
        logger.debug("logger_file0 end");
    }

    {
        rx::logger_t &logger = rx::make_logger_file<1>("./log1.txt");
        logger_modname(logger);
        rx_test_logger_stdout_2(rt,logger.master());
        logger.debug("logger_file1 end");
    }

    {
        rx::logger_t &logger = rx::make_logger_confile();
        logger_modname(logger);
        rx_test_logger_stdout_2(rt,logger.master());
        logger.debug("logger_confile end");
    }
}

rx_tdd(test_logger_std_base)
{
    rx_test_logger_stdout_1(*this);
    rx_test_logger_stdout_2A(*this);
    rx_test_logger_stdout_2B(*this);
}
#endif // _RX_UT_LOGGER_STD_H_
