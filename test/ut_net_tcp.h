#ifndef _UT_NET_TCP_H_
#define _UT_NET_TCP_H_

#include "../rx_net_tcp_svrsock.h"
#include "../rx_net_tcp_client.h"
#include "../rx_tdd.h"

namespace rx
{
    //-----------------------------------------------------
    class ut_tcp_echo_client
    {
        tcp_client_t    m_clt;
    public:
        bool init(uint16_t port=45601,const char* host=NULL)
        {
            if (is_empty(host))
                host="127.0.0.1";
            return m_clt.dst_addr(host,port);
        }
        bool conn(){return m_clt.connect();}
        bool step()
        {
            uint8_t tmp[1024*16];
            error_t e=m_clt.write(tmp,sizeof(tmp),sec2us(5));
            if (e!=ec_ok)
                return false;

            uint8_t tmp2[sizeof(tmp)];
            e=m_clt.read(tmp2,sizeof(tmp2),sec2us(5));
            if (e!=ec_ok)
                return false;

            return 0==memcmp(tmp,tmp2,sizeof(tmp));
        }
    };

    //-----------------------------------------------------
    //对echo进行测试的客户端
    inline void ut_tcp_echo_clt(rx_tdd_t& rt,uint32_t loop=100)
    {
        ut_tcp_echo_client clt;
        rt.tdd_assert(clt.init(45601,"127.0.0.1"));
        rt.tdd_assert(clt.conn());
        for(uint32_t i=0;i<loop;++i)
        {
            bool r=clt.step();
            if (!rt.tdd_assert(r))
                break;
        }
    }

    //-----------------------------------------------------
    //对echo进行测试的服务端
    inline void ut_tcp_echo_svr(rx_tdd_t& rt,uint32_t loop=100000)
    {
        tcp_echo_svr_t svr;
        if (!rt.tdd_assert(svr.init()))
            return;

        for(uint32_t i=0;i<loop;++i)
        {
            uint32_t r=svr.step(ms2us(1));
            if (r)
                svr.logger().debug("ut_tcp_echo_svr step %d/%d",i+1,loop);
        }
    }
}
rx_tdd(test_net_tcp_base)
{
    //rx::ut_tcp_echo_clt(*this);
}





#endif // _UT_MEM_POOL_H_
