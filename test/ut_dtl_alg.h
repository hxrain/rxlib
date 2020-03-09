#ifndef _UT_DTL_ALG_H_
#define _UT_DTL_ALG_H_

#include "../rx_dtl_alg.h"
#include "../rx_tdd.h"
#include "../rx_hash_rand.h"


//---------------------------------------------------------
//固定样本的测试,验证各类二分搜索算法的逻辑正确性
inline void ut_dtl_alg_bisect_0(rx_tdd_t &rt)
{
	//定义测试样本数组
	int arr[] = { 1,2,3,3,4,4,5,6,7,7,7,8 };				//长度12,不要改动数组内容
	const uint32_t length = rx_arrsize(arr);

	//进行直接查找
	uint32_t idx = rx::bisect(arr, length, 3);
	rt.tdd_assert(idx == 2);								//3 的查询结果恰好在左边界

	idx = rx::bisect(arr, length, 4);
	rt.tdd_assert(idx == 5);								//4 的查询结果恰好在右边界

	idx = rx::bisect(arr, length, 1);
	rt.tdd_assert(idx == 0);								//1 的查询结果

	idx = rx::bisect(arr, length, 8);
	rt.tdd_assert(idx == 11);								//8 的查询结果

	idx = rx::bisect(arr, length, 7);
	rt.tdd_assert(idx == 8);								//7 的查询结果恰好在左边界

	idx = rx::bisect_lte(arr, length, 3);
	rt.tdd_assert(idx == 3);
	idx = rx::bisect_lte(arr, length, 5);
	rt.tdd_assert(idx == 6);
	idx = rx::bisect_lte(arr, length, 6);
	rt.tdd_assert(idx == 7);

	idx = rx::bisect_first(arr, length, 3);
	rt.tdd_assert(idx == 2);								//搜索3,要左边界,第一个
	idx = rx::bisect_last(arr, length, 3);
	rt.tdd_assert(idx == 3);								//搜索3,要右边界,最后的
	idx = rx::bisect_first(arr, length, 4);
	rt.tdd_assert(idx == 4);								//搜索4,要左边界,第一个
	idx = rx::bisect_last(arr, length, 4);
	rt.tdd_assert(idx == 5);								//搜索4,要右边界,最后的
}
//---------------------------------------------------------
typedef uint32_t(*bs_fun_t)(const int* arr, const uint32_t length, const int& x);

//动态样本的测试,验证二分搜索算法的可靠性
template<int max_size>
void ut_dtl_alg_bisect_1(rx_tdd_t &rt, bs_fun_t fun, int s = 0)
{
	int arr[max_size];
	if (s)
		rx_rnd_t32().seed(s);

	int xp = rx_rnd_t32().get(max_size - 1);
	int x;

	for (int i = 0;i < max_size;++i)
	{
		arr[i] = rx_rnd_t32().get(max_size);
		if (i == xp)
			x = arr[i];
	}

	rx::quick_sort(arr, max_size, rx::qs_cmp_t());

	rt.tdd_assert(max_size != fun(arr, max_size, x));
}

//---------------------------------------------------------
//快速排序的简单验证
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


//---------------------------------------------------------

rx_tdd(dtl_alg)
{
	ut_dtl_alg_bisect_0(*this);

	for (int i = 0;i < 100;++i)
	{
		ut_dtl_alg_bisect_1<10000>(*this, rx::bisect, i);
		ut_dtl_alg_bisect_1<10000>(*this, rx::bisect_lte, i);
		ut_dtl_alg_bisect_1<10000>(*this, rx::bisect_first, i);
		ut_dtl_alg_bisect_1<10000>(*this, rx::bisect_last, i);
	}

	for (int i = 0;i < 100;++i)
	{
		ut_dtl_alg_qsort_1<100>(*this, i);
		ut_dtl_alg_qsort_1<1000>(*this, i);
		ut_dtl_alg_qsort_1<10000>(*this, i);
		ut_dtl_alg_qsort_1<100000>(*this, i);
	}
}





#endif // _UT_MEM_POOL_H_
