
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
	rt.tdd_assert(hat.value(hat.offset(0)) == NULL);
	rt.tdd_assert(hat.offset(0).offset == 0);
	rt.tdd_assert(hat.offset(0).key_cnt == 0);
	rt.tdd_assert(hat.offset(0).val_idx == 0);

	uint16_t kl;
	rt.tdd_assert(hat.key(0, &kl) == NULL);
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
	char* k = hat.key(idx, &kl);
	rt.tdd_assert(kl == 1);
	rt.tdd_assert(k[1] == 0);
	rt.tdd_assert(strcmp("a", k) == 0);
	rt.tdd_assert(k[2] == 'b');

	idx = hat.find("b", 1);
	rt.tdd_assert(idx != hat.capacity());
	k = hat.key(idx, &kl);
	rt.tdd_assert(kl == 1);
	rt.tdd_assert(k[1] == 0);
	rt.tdd_assert(strcmp("b", k) == 0);
	rt.tdd_assert(k[2] == 'c');

	idx = hat.find("c", 1);
	rt.tdd_assert(idx != hat.capacity());
	k = hat.key(idx, &kl);
	rt.tdd_assert(kl == 1);
	rt.tdd_assert(k[1] == 0);
	rt.tdd_assert(strcmp("c", k) == 0);
	rt.tdd_assert(k[2] == 'd');

	idx = hat.find("h", 1);
	rt.tdd_assert(idx != hat.capacity());
	k = hat.key(idx, &kl);
	rt.tdd_assert(kl == 1);
	rt.tdd_assert(k[1] == 0);
	rt.tdd_assert(strcmp("h", k) == 0);
	rt.tdd_assert(k[2] == 0);
}

void ut_dtl_hat_base_2(rx_tdd_t &rt)
{
	typedef rx::hat_ft<8, char, 6> hat_t;
	hat_t hat;
	rt.tdd_assert(hat.is_valid());
	rt.tdd_assert(hat.size() == 0);
	rt.tdd_assert(hat.capacity() == 8);
	rt.tdd_assert(hat.remain() == (sizeof(char)*(6 + 1)*hat.capacity()));
	rt.tdd_assert(!hat.sorted());
	rt.tdd_assert(hat.value(hat.offset(0)) == NULL);
	rt.tdd_assert(hat.offset(0).offset == 0);
	rt.tdd_assert(hat.offset(0).key_cnt == 0);
	rt.tdd_assert(hat.offset(0).val_idx == 0);

	uint16_t kl;
	rt.tdd_assert(hat.key(0, &kl) == NULL);
	rt.tdd_assert(hat.next(0) == hat.capacity());
	rt.tdd_assert(hat.push("a", 1) != hat.capacity());
	rt.tdd_assert(hat.push("c", 1) != hat.capacity());
	rt.tdd_assert(hat.push("e", 1) != hat.capacity());
	rt.tdd_assert(hat.push("d", 1) != hat.capacity());
	rt.tdd_assert(hat.push("b", 1) != hat.capacity());

	//排序,之前的key序号是按hash计算的,现在就是按key序升序排列的.
	rt.tdd_assert(!hat.sort());

	//按序号升序访问元素,key应该是升序的
	char *k = hat.key(0);
	rt.tdd_assert(strcmp("a", k) == 0);
	rt.tdd_assert(k[1] == 0);
	rt.tdd_assert(k[2] == 'c');

	k = hat.key(1);
	rt.tdd_assert(strcmp("b", k) == 0);
	rt.tdd_assert(k[1] == 0);
	rt.tdd_assert(k[2] == 0);

	k = hat.key(2);
	rt.tdd_assert(strcmp("c", k) == 0);
	rt.tdd_assert(k[1] == 0);
	rt.tdd_assert(k[2] == 'e');

	k = hat.key(3);
	rt.tdd_assert(strcmp("d", k) == 0);
	rt.tdd_assert(k[1] == 0);
	rt.tdd_assert(k[2] == 'b');

	k = hat.key(4);
	rt.tdd_assert(strcmp("e", k) == 0);
	rt.tdd_assert(k[1] == 0);
	rt.tdd_assert(k[2] == 'd');

	//按key查找元素,序号应该是升序的
	uint16_t ki = hat.find("a", 1);
	rt.tdd_assert(ki != hat.capacity());
	k = hat.key(ki);
	rt.tdd_assert(strcmp("a", k) == 0);
	rt.tdd_assert(k[1] == 0);
	rt.tdd_assert(k[2] == 'c');
	rt.tdd_assert(ki == 0);

	ki = hat.find("b", 1);
	rt.tdd_assert(ki != hat.capacity());
	k = hat.key(ki);
	rt.tdd_assert(strcmp("b", k) == 0);
	rt.tdd_assert(k[1] == 0);
	rt.tdd_assert(k[2] == 0);
	rt.tdd_assert(ki == 1);

	ki = hat.find("c", 1);
	rt.tdd_assert(ki != hat.capacity());
	k = hat.key(ki);
	rt.tdd_assert(strcmp("c", k) == 0);
	rt.tdd_assert(k[1] == 0);
	rt.tdd_assert(k[2] == 'e');
	rt.tdd_assert(ki == 2);

	ki = hat.find("d", 1);
	rt.tdd_assert(ki != hat.capacity());
	k = hat.key(ki);
	rt.tdd_assert(strcmp("d", k) == 0);
	rt.tdd_assert(k[1] == 0);
	rt.tdd_assert(k[2] == 'b');
	rt.tdd_assert(ki == 3);

	ki = hat.find("e", 1);
	rt.tdd_assert(ki != hat.capacity());
	k = hat.key(ki);
	rt.tdd_assert(strcmp("e", k) == 0);
	rt.tdd_assert(k[1] == 0);
	rt.tdd_assert(k[2] == 'd');
	rt.tdd_assert(ki == 4);

	ki = hat.find("f", 1);
	rt.tdd_assert(ki == hat.capacity());

}
void ut_dtl_hat_base_3(rx_tdd_t &rt)
{
	typedef rx::hat_ft<8, char, 6> hat_t;
	hat_t hat;
	rt.tdd_assert(hat.push("a1a2a3", 6) != hat.capacity());
	rt.tdd_assert(hat.push("c1c2c3", 6) != hat.capacity());
	rt.tdd_assert(hat.push("a1abc", 5) != hat.capacity());
	rt.tdd_assert(hat.push("c12c3", 5) != hat.capacity());
	rt.tdd_assert(hat.push("b123", 4) != hat.capacity());

	uint16_t idx = hat.find("a1abc", 5);
	rt.tdd_assert(idx != hat.capacity());
	char* k = hat.key(idx);
	rt.tdd_assert(strcmp("a1abc", k) == 0);
	rt.tdd_assert(k[5] == 0);
	rt.tdd_assert(k[6] == 'c');

	//排序,之前的key序号是按hash计算的,现在就是按key序升序排列的.
	rt.tdd_assert(!hat.sort());
	idx = hat.prefix("a1", 2);
	rt.tdd_assert(idx != hat.capacity());
	k = hat.key(idx);
	rt.tdd_assert(strcmp("a1a2a3", k) == 0);
	rt.tdd_assert(k[6] == 0);
	rt.tdd_assert(k[7] == 'c');
	rt.tdd_assert(hat.prefix_left(idx, 2) == 0);
	rt.tdd_assert(hat.prefix_right(idx, 2) == 1);

	k = hat.key(idx + 1);
	rt.tdd_assert(strcmp("a1abc", k) == 0);
	rt.tdd_assert(k[5] == 0);
	rt.tdd_assert(k[6] == 'c');
}

rx_tdd(dtl_hat)
{
	ut_dtl_hat_base_3(*this);
	ut_dtl_hat_base_2(*this);
	ut_dtl_hat_base_1(*this);
}





#endif // _UT_MEM_POOL_H_
