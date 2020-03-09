#ifndef _UT_DTL_ALG_H_
#define _UT_DTL_ALG_H_

#include "../rx_dtl_alg.h"
#include "../rx_tdd.h"
#include "../rx_hash_rand.h"


//---------------------------------------------------------
//�̶������Ĳ���,��֤������������㷨���߼���ȷ��
inline void ut_dtl_alg_bisect_0(rx_tdd_t &rt)
{
	//���������������
	int arr[] = { 1,2,3,3,4,4,5,6,7,7,7,8 };				//����12,��Ҫ�Ķ���������
	const uint32_t length = rx_arrsize(arr);

	//����ֱ�Ӳ���
	uint32_t idx = rx::bisect(arr, length, 3);
	rt.tdd_assert(idx == 2);								//3 �Ĳ�ѯ���ǡ������߽�

	idx = rx::bisect(arr, length, 4);
	rt.tdd_assert(idx == 5);								//4 �Ĳ�ѯ���ǡ�����ұ߽�

	idx = rx::bisect(arr, length, 1);
	rt.tdd_assert(idx == 0);								//1 �Ĳ�ѯ���

	idx = rx::bisect(arr, length, 8);
	rt.tdd_assert(idx == 11);								//8 �Ĳ�ѯ���

	idx = rx::bisect(arr, length, 7);
	rt.tdd_assert(idx == 8);								//7 �Ĳ�ѯ���ǡ������߽�

	idx = rx::bisect_lte(arr, length, 3);
	rt.tdd_assert(idx == 3);
	idx = rx::bisect_lte(arr, length, 5);
	rt.tdd_assert(idx == 6);
	idx = rx::bisect_lte(arr, length, 6);
	rt.tdd_assert(idx == 7);

	idx = rx::bisect_first(arr, length, 3);
	rt.tdd_assert(idx == 2);								//����3,Ҫ��߽�,��һ��
	idx = rx::bisect_last(arr, length, 3);
	rt.tdd_assert(idx == 3);								//����3,Ҫ�ұ߽�,����
	idx = rx::bisect_first(arr, length, 4);
	rt.tdd_assert(idx == 4);								//����4,Ҫ��߽�,��һ��
	idx = rx::bisect_last(arr, length, 4);
	rt.tdd_assert(idx == 5);								//����4,Ҫ�ұ߽�,����
}
//---------------------------------------------------------
typedef uint32_t(*bs_fun_t)(const int* arr, const uint32_t length, const int& x);

//��̬�����Ĳ���,��֤���������㷨�Ŀɿ���
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
//��������ļ���֤
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
