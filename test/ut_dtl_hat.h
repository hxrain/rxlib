
#ifndef _UT_DTL_HAT_H_
#define _UT_DTL_HAT_H_

#include "../rx_dtl_hat_raw.h"
#include "../rx_tdd.h"
#include "../rx_hash_rand.h"

//---------------------------------------------------------
void ut_dtl_hat_base_1(rx_tdd_t &rt)
{
	typedef rx::hat_ft<8, char, 6> hat_t;
	hat_t hat;
	rt.tdd_assert(hat.is_valid());
	rt.tdd_assert(hat.size() == 0);
	rt.tdd_assert(hat.capacity() == 8);
	rt.tdd_assert(hat.remain() == (sizeof(char)*(6 + 1)*hat.capacity()));
	rt.tdd_assert(!hat.sorted());
	rt.tdd_assert(hat.value(0) == NULL);
	rt.tdd_assert(hat.offset(0).offset == 0);
	rt.tdd_assert(hat.offset(0).key_cnt == 0);
	rt.tdd_assert(hat.offset(0).val_idx == 0);

	uint16_t kl;
	rt.tdd_assert(hat.key(0, kl) == NULL);
	rt.tdd_assert(hat.next(0) == hat.capacity());
	rt.tdd_assert(hat.push("a", 1) != hat.capacity());
	rt.tdd_assert(hat.push("b", 1) != hat.capacity());
	rt.tdd_assert(hat.push("c", 1) != hat.capacity());
	rt.tdd_assert(hat.push("d", 1) != hat.capacity());
	rt.tdd_assert(hat.push("e", 1) != hat.capacity());
	rt.tdd_assert(hat.push("f", 1) != hat.capacity());
	rt.tdd_assert(hat.push("g", 1) != hat.capacity());
	rt.tdd_assert(hat.push("h", 1) != hat.capacity());
	rt.tdd_assert(hat.push("i", 1) == hat.capacity());

	uint16_t idx = hat.find("a", 1);
	rt.tdd_assert(idx != hat.capacity());
	char* k = hat.key(idx, kl);
	rt.tdd_assert(kl == 1);
	rt.tdd_assert(k[1] == 0);
	rt.tdd_assert(strcmp("a", k) == 0);
	rt.tdd_assert(k[2] == 'b');

	idx = hat.find("b", 1);
	rt.tdd_assert(idx != hat.capacity());
	k = hat.key(idx, kl);
	rt.tdd_assert(kl == 1);
	rt.tdd_assert(k[1] == 0);
	rt.tdd_assert(strcmp("b", k) == 0);
	rt.tdd_assert(k[2] == 'c');

	idx = hat.find("c", 1);
	rt.tdd_assert(idx != hat.capacity());
	k = hat.key(idx, kl);
	rt.tdd_assert(kl == 1);
	rt.tdd_assert(k[1] == 0);
	rt.tdd_assert(strcmp("c", k) == 0);
	rt.tdd_assert(k[2] == 'd');

	idx = hat.find("h", 1);
	rt.tdd_assert(idx != hat.capacity());
	k = hat.key(idx, kl);
	rt.tdd_assert(kl == 1);
	rt.tdd_assert(k[1] == 0);
	rt.tdd_assert(strcmp("h", k) == 0);
	rt.tdd_assert(k[2] == 0);
}


rx_tdd(dtl_hat)
{
	ut_dtl_hat_base_1(*this);
}





#endif // _UT_MEM_POOL_H_
