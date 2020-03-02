#ifndef _UT_NET_TCP_H_
#define _UT_NET_TCP_H_

#include "../rx_net_tcp_svrsock.h"
#include "../rx_net_tcp_client.h"
#include "../rx_os_thread.h"
#include "../rx_tdd.h"

namespace rx
{
    //-----------------------------------------------------
    //��echo���в��ԵĿͻ���
    inline void ut_tcp_echo_clt(rx_tdd_t& rt,uint32_t loop=100)
    {
        tcp_client_t    clt;
        rt.tdd_assert(clt.dst_addr("127.0.0.1",45601));
        rt.tdd_assert(clt.connect());
        
        uint8_t tmp[1024*16];
        uint8_t tmp2[sizeof(tmp)];
        for(uint32_t i=0;i<loop;++i)
        {
            error_t e=clt.write(tmp,sizeof(tmp),sec2us(5));
            if (!rt.tdd_assert(e==ec_ok))
                break;

            e=clt.read(tmp2,sizeof(tmp2),sec2us(5));
            if (!rt.tdd_assert(e==ec_ok))
                break;

            if (!rt.tdd_assert(0==memcmp(tmp,tmp2,sizeof(tmp))))
                break;
        }
        clt.disconnect(false);
    }

    //-----------------------------------------------------
    //��echo���в��Եķ����
    inline void ut_tcp_echo_svr(rx_tdd_t& rt,uint32_t loop=100000)
    {
        tcp_echo_svr_t svr;
        svr.logger().bind(make_logger_con());
        if (!rt.tdd_assert(svr.init()))
            return;

        for(uint32_t i=0;i<loop;++i)
        {
            uint32_t r=svr.step(ms2us(1));
            if (r)
                svr.logger().debug("ut_tcp_echo_svr step %d/%d",i+1,loop);
        }
    }
    //-----------------------------------------------------
    //����tcp echo server���Ե��߳�
    class ut_tcpsvr_thd_t:public thread_ex_t
    {
        uint32_t on_run(void *param)
        {
            rx_tdd_t &rt=*(rx_tdd_t*)param;

            tcp_echo_svr_t svr;
            svr.logger().bind(make_logger_con());

            if (!rt.tdd_assert(svr.init()))
                return 1;

            while(!need_break())
                svr.step(ms2us(1));

            return 0;
        }
    };

    inline void ut_tcp_echo_thd(rx_tdd_t& rt,uint32_t loop=100,uint32_t count=10)
    {
        ut_tcpsvr_thd_t thd;
        rt.tdd_assert(thd.start(&rt));                      //�������̲߳�����
        for(uint32_t i=0;i<count;++i)
            ut_tcp_echo_clt(rt,loop);                       //���ÿͻ��˲���ѭ��
        rx_sleep(10);                                       //�ȷ���˴������
        thd.stop(true);                                     //�����߳�
    }
}
rx_tdd(test_net_tcp_base)
{
    rx::ut_tcp_echo_thd(*this);
}





#endif // _UT_MEM_POOL_H_
