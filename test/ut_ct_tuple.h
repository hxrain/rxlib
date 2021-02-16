#ifndef _RX_UT_CT_TUPLE_H_
#define _RX_UT_CT_TUPLE_H_

#include "../rx_tdd.h"
#include "../rx_ct_tuple.h"
#include "../rx_ct_typeof.h"

namespace rx
{
	void test_ct_tuple_base(rx_tdd_t &rt)
	{
		tuple_t<int> var1(1);
		rt.assert(get<0>(var1) == 1);

		const tuple_t<int> var1c(1);
		rt.assert(get<0>(var1c) == 1);

		tuple_t<int, char> var2(1, 127);
		rt.assert(get<0>(var2) == 1);
		rt.assert(get<1>(var2) == 127);

		const tuple_t<int, char> var2c(1, 127);
		rt.assert(get<0>(var2c) == 1);
		rt.assert(get<1>(var2c) == 127);

		tuple_t<int, char, short> var3(1, 127, 32767);
		rt.assert(get<0>(var3) == 1);
		rt.assert(get<1>(var3) == 127);
		rt.assert(get<2>(var3) == 32767);

		const tuple_t<int, char, short> var3c(1, 127, 32767);
		rt.assert(get<0>(var3c) == 1);
		rt.assert(get<1>(var3c) == 127);
		rt.assert(get<2>(var3c) == 32767);

		tuple_t<int, char, short, char> var4(1, 127, 32767, 127);
		rt.assert(get<0>(var4) == 1);
		rt.assert(get<1>(var4) == 127);
		rt.assert(get<2>(var4) == 32767);
		rt.assert(get<3>(var4) == 127);

		const tuple_t<int, char, short, char> var4c(1, 127, 32767, 127);
		rt.assert(get<0>(var4c) == 1);
		rt.assert(get<1>(var4c) == 127);
		rt.assert(get<2>(var4c) == 32767);
		rt.assert(get<3>(var4c) == 127);

		tuple_t<int, char, short, char, int> var5(1, 127, 32767, 127, -1);
		rt.assert(get<0>(var5) == 1);
		rt.assert(get<1>(var5) == 127);
		rt.assert(get<2>(var5) == 32767);
		rt.assert(get<3>(var5) == 127);
		rt.assert(get<4>(var5) == -1);

		const tuple_t<int, char, short, char, int> var5c(1, 127, 32767, 127, -1);
		rt.assert(get<0>(var5c) == 1);
		rt.assert(get<1>(var5c) == 127);
		rt.assert(get<2>(var5c) == 32767);
		rt.assert(get<3>(var5c) == 127);
		rt.assert(get<4>(var5c) == -1);
	}

	void test_ct_tuple_make(rx_tdd_t &rt)
	{
		tuple_t<int> var1 = make_tuple((int)1);
		rt.assert(get<0>(var1) == 1);

		tuple_t<int, char> var2 = make_tuple((int)1, (char)127);
		rt.assert(get<0>(var2) == 1);
		rt.assert(get<1>(var2) == 127);

		tuple_t<int, char, short> var3 = make_tuple((int)1, (char)127, (short)32767);
		rt.assert(get<0>(var3) == 1);
		rt.assert(get<1>(var3) == 127);
		rt.assert(get<2>(var3) == 32767);

		tuple_t<int, char, short, char> var4 = make_tuple((int)1, (char)127, (short)32767, (char)127);
		rt.assert(get<0>(var4) == 1);
		rt.assert(get<1>(var4) == 127);
		rt.assert(get<2>(var4) == 32767);
		rt.assert(get<3>(var4) == 127);

		tuple_t<int, char, short, char, int> var5 = make_tuple((int)1, (char)127, (short)32767, (char)127, (int)-1);
		rt.assert(get<0>(var5) == 1);
		rt.assert(get<1>(var5) == 127);
		rt.assert(get<2>(var5) == 32767);
		rt.assert(get<3>(var5) == 127);
		rt.assert(get<4>(var5) == -1);
	}

	void test_ct_tuple_auto(rx_tdd_t &rt)
	{
		auto_of(var4, make_tuple(1, 127, 32767, 127));
		rt.assert(get<0>(var4) == 1);
		rt.assert(get<1>(var4) == 127);
		rt.assert(get<2>(var4) == 32767);
		rt.assert(get<3>(var4) == 127);
	}
}

rx_tdd(ct_tuple_base)
{
	rx::test_ct_tuple_base(*this);
	rx::test_ct_tuple_make(*this);
	rx::test_ct_tuple_auto(*this);
}
#endif // _RX_UT_CT_VARIANT_LITE_H_
