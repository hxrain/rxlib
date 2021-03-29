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

	//在遍历过程中,处理每个元组的元素
	class looper
	{
	public:
		template<class T>
		void operator()(uint32_t idx, uint32_t size, T& v)
		{
			if (idx == 0)
				printf("tuple loop: ");
			printf("%d/%d=%d; ", idx + 1, size, v);;
			if (idx == size - 1)
				printf("\n");
		}
	};

	void test_ct_tuple_auto(rx_tdd_t &rt)
	{
		//使用语法糖快速定义元组
		auto_of(var4, make_tuple(1, 127, 32767, 126));
		rt.assert(get<0>(var4) == 1);
		rt.assert(get<1>(var4) == 127);
		rt.assert(get<2>(var4) == 32767);
		rt.assert(get<3>(var4) == 126);

		//定义元组遍历器,并调用处理器
		tuple_foreach(var4, looper());

		{auto_of(var, make_tuple(1));
		rt.assert(get<0>(var) == 1);
		}
		{auto_of(var, make_tuple(1, 2));
		rt.assert(get<0>(var) == 1);
		rt.assert(get<1>(var) == 2);
		}
		{auto_of(var, make_tuple(1, 2, 3));
		rt.assert(get<0>(var) == 1);
		rt.assert(get<1>(var) == 2);
		rt.assert(get<2>(var) == 3);
		}
		{auto_of(var, make_tuple(1, 2, 3, 4));
		rt.assert(get<0>(var) == 1);
		rt.assert(get<1>(var) == 2);
		rt.assert(get<2>(var) == 3);
		rt.assert(get<3>(var) == 4);
		}
		{auto_of(var, make_tuple(1, 2, 3, 4, 5));
		rt.assert(get<0>(var) == 1);
		rt.assert(get<1>(var) == 2);
		rt.assert(get<2>(var) == 3);
		rt.assert(get<3>(var) == 4);
		rt.assert(get<4>(var) == 5);
		}
		{auto_of(var, make_tuple(1, 2, 3, 4, 5, 6));
		rt.assert(get<0>(var) == 1);
		rt.assert(get<1>(var) == 2);
		rt.assert(get<2>(var) == 3);
		rt.assert(get<3>(var) == 4);
		rt.assert(get<4>(var) == 5);
		rt.assert(get<5>(var) == 6);
		}
		{auto_of(var, make_tuple(1, 2, 3, 4, 5, 6, 7));
		rt.assert(get<0>(var) == 1);
		rt.assert(get<1>(var) == 2);
		rt.assert(get<2>(var) == 3);
		rt.assert(get<3>(var) == 4);
		rt.assert(get<4>(var) == 5);
		rt.assert(get<5>(var) == 6);
		rt.assert(get<6>(var) == 7);
		}
		{auto_of(var, make_tuple(1, 2, 3, 4, 5, 6, 7, 8));
		rt.assert(get<0>(var) == 1);
		rt.assert(get<1>(var) == 2);
		rt.assert(get<2>(var) == 3);
		rt.assert(get<3>(var) == 4);
		rt.assert(get<4>(var) == 5);
		rt.assert(get<5>(var) == 6);
		rt.assert(get<6>(var) == 7);
		rt.assert(get<7>(var) == 8);
		}
		{auto_of(var, make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9));
		rt.assert(get<0>(var) == 1);
		rt.assert(get<1>(var) == 2);
		rt.assert(get<2>(var) == 3);
		rt.assert(get<3>(var) == 4);
		rt.assert(get<4>(var) == 5);
		rt.assert(get<5>(var) == 6);
		rt.assert(get<6>(var) == 7);
		rt.assert(get<7>(var) == 8);
		rt.assert(get<8>(var) == 9);
		}
		{auto_of(var, make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
		rt.assert(get<0>(var) == 1);
		rt.assert(get<1>(var) == 2);
		rt.assert(get<2>(var) == 3);
		rt.assert(get<3>(var) == 4);
		rt.assert(get<4>(var) == 5);
		rt.assert(get<5>(var) == 6);
		rt.assert(get<6>(var) == 7);
		rt.assert(get<7>(var) == 8);
		rt.assert(get<8>(var) == 9);
		rt.assert(get<9>(var) == 10);
		}
		{auto_of(var, make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11));
		rt.assert(get<0>(var) == 1);
		rt.assert(get<1>(var) == 2);
		rt.assert(get<2>(var) == 3);
		rt.assert(get<3>(var) == 4);
		rt.assert(get<4>(var) == 5);
		rt.assert(get<5>(var) == 6);
		rt.assert(get<6>(var) == 7);
		rt.assert(get<7>(var) == 8);
		rt.assert(get<8>(var) == 9);
		rt.assert(get<9>(var) == 10);
		rt.assert(get<10>(var) == 11);
		}
		{auto_of(var, make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12));
		rt.assert(get<0>(var) == 1);
		rt.assert(get<1>(var) == 2);
		rt.assert(get<2>(var) == 3);
		rt.assert(get<3>(var) == 4);
		rt.assert(get<4>(var) == 5);
		rt.assert(get<5>(var) == 6);
		rt.assert(get<6>(var) == 7);
		rt.assert(get<7>(var) == 8);
		rt.assert(get<8>(var) == 9);
		rt.assert(get<9>(var) == 10);
		rt.assert(get<10>(var) == 11);
		rt.assert(get<11>(var) == 12);
		}
		{auto_of(var, make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13));
		rt.assert(get<0>(var) == 1);
		rt.assert(get<1>(var) == 2);
		rt.assert(get<2>(var) == 3);
		rt.assert(get<3>(var) == 4);
		rt.assert(get<4>(var) == 5);
		rt.assert(get<5>(var) == 6);
		rt.assert(get<6>(var) == 7);
		rt.assert(get<7>(var) == 8);
		rt.assert(get<8>(var) == 9);
		rt.assert(get<9>(var) == 10);
		rt.assert(get<10>(var) == 11);
		rt.assert(get<11>(var) == 12);
		rt.assert(get<12>(var) == 13);
		}
		{auto_of(var, make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14));
		rt.assert(get<0>(var) == 1);
		rt.assert(get<1>(var) == 2);
		rt.assert(get<2>(var) == 3);
		rt.assert(get<3>(var) == 4);
		rt.assert(get<4>(var) == 5);
		rt.assert(get<5>(var) == 6);
		rt.assert(get<6>(var) == 7);
		rt.assert(get<7>(var) == 8);
		rt.assert(get<8>(var) == 9);
		rt.assert(get<9>(var) == 10);
		rt.assert(get<10>(var) == 11);
		rt.assert(get<11>(var) == 12);
		rt.assert(get<12>(var) == 13);
		rt.assert(get<13>(var) == 14);
		}
		{auto_of(var, make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15));
		rt.assert(get<0>(var) == 1);
		rt.assert(get<1>(var) == 2);
		rt.assert(get<2>(var) == 3);
		rt.assert(get<3>(var) == 4);
		rt.assert(get<4>(var) == 5);
		rt.assert(get<5>(var) == 6);
		rt.assert(get<6>(var) == 7);
		rt.assert(get<7>(var) == 8);
		rt.assert(get<8>(var) == 9);
		rt.assert(get<9>(var) == 10);
		rt.assert(get<10>(var) == 11);
		rt.assert(get<11>(var) == 12);
		rt.assert(get<12>(var) == 13);
		rt.assert(get<13>(var) == 14);
		rt.assert(get<14>(var) == 15);
		}
		{auto_of(var, make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16));
		rt.assert(get<0>(var) == 1);
		rt.assert(get<1>(var) == 2);
		rt.assert(get<2>(var) == 3);
		rt.assert(get<3>(var) == 4);
		rt.assert(get<4>(var) == 5);
		rt.assert(get<5>(var) == 6);
		rt.assert(get<6>(var) == 7);
		rt.assert(get<7>(var) == 8);
		rt.assert(get<8>(var) == 9);
		rt.assert(get<9>(var) == 10);
		rt.assert(get<10>(var) == 11);
		rt.assert(get<11>(var) == 12);
		rt.assert(get<12>(var) == 13);
		rt.assert(get<13>(var) == 14);
		rt.assert(get<14>(var) == 15);
		rt.assert(get<15>(var) == 16);
		tuple_foreach(var, looper());
		}
	}
}

rx_tdd(ct_tuple_base)
{
	rx::test_ct_tuple_base(*this);
	rx::test_ct_tuple_make(*this);
	rx::test_ct_tuple_auto(*this);
}
#endif // _RX_UT_CT_VARIANT_LITE_H_
