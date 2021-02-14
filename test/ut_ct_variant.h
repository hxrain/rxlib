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
		//��׼���ַ���
		std::string hello = "hello, world";

		//�ɱ���(�ַ�/����/������/��׼��)
		variant_lite_t< char, int, long, std::string > var;
		rt.assert(!var.is_valid());

		//�ͱ�0,�ַ�
		var = 'v';
		rt.assert(var.is_valid());
		rt.assert(get<  0 >(var) == 'v');
		rt.assert(get<char>(var) == 'v');
		rt.assert(can<char>(var));
		rt.assert(!can<int>(var));
		rt.assert(!can<short>(var));
		rt.assert(!has<short>(var));
		rt.assert(var.index_of<short>()==bad_type_index);

		//�ͱ�1,����
		var = 7;
		rt.assert(get<int >(var) == 7);
		rt.assert(get<1>(var) == 7);
		rt.assert(var.index() == 1);
		rt.assert(var.index_of<char>() == 0);

		//�ͱ�2,������
		var = 42L;
		rt.assert(get<long>(var) == 42L);
		rt.assert(get<2>(var) == 42L);
		rt.assert(!can<int>(var));

		//�ͱ�3,��׼��
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
