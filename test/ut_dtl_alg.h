#ifndef _UT_DTL_ALG_H_
#define _UT_DTL_ALG_H_

#include "../rx_dtl_alg.h"
#include "../rx_tdd.h"
#include "../rx_hash_rand.h"


template<int max_size>
void ut_dtl_alg_qsort_1(rx_tdd_t &rt, int s = 0)
{
	int arr[max_size];
	if (s)
		rx_rnd_t32().seed(s);

	for (int i = 0;i < max_size;++i)
	{
		arr[i] = (uint16_t)rx_rnd_t32().get();
		if (max_size < 30)
			printf("%u ", arr[i]);
	}
	if (max_size < 30)
		printf("\n");

	chkbad_t bad;
	rx::quick_sort(arr, max_size, rx::qs_cmp_t());

	for (int i = 0;i < max_size - 1;++i)
	{
		bad.assert(arr[i] <= arr[i + 1]);
		if (max_size < 30)
			printf("%u ", arr[i]);
	}
	if (max_size < 30)
		printf("%u \n", arr[max_size - 1]);

	rt.tdd_assert(bad.error() == 0);
}


rx_tdd(dtl_alg)
{
	ut_dtl_alg_qsort_1<10>(*this, 1);
	ut_dtl_alg_qsort_1<20>(*this, 2);

	for (int i = 0;i < 100;++i)
	{
		ut_dtl_alg_qsort_1<100>(*this, i);
		ut_dtl_alg_qsort_1<1000>(*this, i);
		ut_dtl_alg_qsort_1<10000>(*this, i);
		ut_dtl_alg_qsort_1<100000>(*this, i);
	}
}





#endif // _UT_MEM_POOL_H_
