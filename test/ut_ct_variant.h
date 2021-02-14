#ifndef _RX_UT_CT_VARIANT_LITE_H_
#define _RX_UT_CT_VARIANT_LITE_H_

#include "../rx_tdd.h"
#include "../rx_cc_macro.h"
#include "../rx_ct_variant.h"
#include <string>

namespace rx
{
	void test_ct_variant_lite(rx_tdd_t &rt)
	{
		//标准库字符串
		std::string hello = "hello, world";

		//可变量(字符/整数/长整数/标准串)
		variant_lite_t< char, int, long, std::string > var;
		rt.assert(!var.is_valid());

		//型别0,字符
		var = 'v';
		rt.assert(var.is_valid());
		rt.assert(get<  0 >(var) == 'v');
		rt.assert(get<char>(var) == 'v');
		rt.assert(can<char>(var));
		rt.assert(!can<int>(var));
		rt.assert(!can<short>(var));
		rt.assert(!has<short>(var));
		rt.assert(var.index_of<short>()==bad_type_index);

		//型别1,整数
		var = 7;
		rt.assert(get<int >(var) == 7);
		rt.assert(get<1>(var) == 7);
		rt.assert(var.index() == 1);
		rt.assert(var.index_of<char>() == 0);

		//型别2,长整数
		var = 42L;
		rt.assert(get<long>(var) == 42L);
		rt.assert(get<2>(var) == 42L);
		rt.assert(!can<int>(var));

		//型别3,标准串
		var = hello;
		rt.assert(get<std::string>(var) == hello);
		rt.assert(get<3>(var) == hello);
	}
}

rx_tdd(ct_variant_lite_base)
{
	rx::test_ct_variant_lite(*this);
}
#endif // _RX_UT_CT_VARIANT_LITE_H_
