#ifndef _UT_NET_SOCK_STD_H_
#define _UT_NET_SOCK_STD_H_

#include "../rx_cc_macro.h"
#include "../rx_tdd.h"
#include "../rx_net_sock_std.h"

inline void test_sock_base_1(rx_tdd_t &rt)
{
	uint32_t addr1 = rx::sock_addr_t::lookup("127.0.0.1");
	uint32_t addr2 = rx::sock_addr_t::to_ip("127.0.0.1");
	rt.assert(addr1 == addr2);
}


rx_tdd(rx_net_sock_std_base)
{
	test_sock_base_1(*this);
}


#endif
