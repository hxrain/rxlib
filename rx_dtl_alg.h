#ifndef _RX_DTL_ALG_H_
#define _RX_DTL_ALG_H_

#include "rx_cc_macro.h"
#include "rx_assert.h"

namespace rx
{
	//-----------------------------------------------------
	//二分搜索算法,查找x(多值的时候不确定是哪个);数组arr的长度为length,升序排列
	//返回值:length,未找到;其他为x在数组中的索引
	template<class ET, class XT>
	inline uint32_t bisect(const ET* arr, const uint32_t length, const XT& x) {
		uint32_t left = 0, right = length - 1;
		while (left < right) {
			uint32_t mid = (left + right) / 2;
			const ET& ci = arr[mid];
			if (ci == x)
				return mid;
			else if (ci > x)
				right = mid - 1;
			else
				left = mid + 1;
		}
		if (left == right&&arr[left] == x)
			return left;
		return length;
	}

	//-----------------------------------------------------
	//二分搜索算法,查找x的左边界位置(首次出现);数组arr的长度为length,升序排列
	//返回值:length,未找到;其他为x在数组中的索引
	template<class ET, class XT>
	uint32_t bisect_first(const ET * arr, uint32_t length, const XT &x)
	{
		uint32_t left = 0, right = length - 1;
		while (left < right)
		{
			uint32_t mid = (left + right) / 2;
			if (arr[mid] < x)
				left = mid + 1;
			else
				right = mid;
		}

		if (arr[left] == x)
			return left;
		else
			return length;
	}

	//-----------------------------------------------------
	//二分搜索算法,查找x的右边界位置(最后出现);数组arr的长度为length,升序排列
	//返回值:length,未找到;其他为x在数组中的索引
	template<class ET, class XT>
	uint32_t bisect_last(const ET * arr, uint32_t length, const XT &x)
	{
		uint32_t left = 0, right = length - 1;
		while (left < right - 1)
		{
			uint32_t mid = left + (right - left) / 2;
			if (arr[mid] <= x)
				left = mid;
			else
				right = mid;
		}

		if (arr[right] == x)
			return right;
		else if (arr[left] == x)
			return left;
		else
			return length;
	}

	//-----------------------------------------------------
	//二分搜索算法,查找<=x的左边界位置;数组arr的长度为length,升序排列
	//返回值:length,未找到;其他为x在数组中的索引
	template<class ET, class XT>
	inline uint32_t bisect_lte(const ET* arr, const uint32_t length, const XT& x)
	{
		uint32_t left = 0, right = length - 1;
		while (left < right)
		{
			uint32_t mid = (left + right) / 2;
			if (arr[mid] <= x)
			{
				if (left == mid)
					break;
				left = mid;
			}
			else
				right = mid;
		}

		if (arr[right] < x)
			return right;
		else if (arr[left] <= x)
			return left;

		return length;
	}

	//-----------------------------------------------------
	//二分搜索算法,查找<x的左边界位置;数组arr的长度为length,升序排列
	//返回值:length,未找到;其他为x在数组中的索引
	template<class ET, class XT>
	inline uint32_t bisect_lt(const ET* arr, const uint32_t length, const XT& x)
	{
		uint32_t left = 0, right = length - 1;
		while (left < right)
		{
			uint32_t mid = (left + right) / 2;
			if (arr[mid] < x)
			{
				if (left == mid)
					break;
				left = mid;
			}
			else
				right = mid;
		}

		if (arr[right] < x)
			return right;
		else if (arr[left] < x)
			return left;

		return length;
	}
	//-----------------------------------------------------
	//二分搜索算法,查找>x的右边界位置;数组arr的长度为length,升序排列
	//返回值:length,未找到;其他为x在数组中的索引
	template<class ET, class XT>
	uint32_t bisect_gt(const ET* arr, const uint32_t length, const XT& x)
	{
		uint32_t left = 0, right = length - 1;
		while (left < right)
		{
			uint32_t mid = (left + right) / 2;
			if (arr[mid] <= x)
				left = mid + 1;
			else
				right = mid;
		}
		if (arr[right] > x)
			return right;
		return length;
	}


	//-----------------------------------------------------
	//排序需要的,小于比较器,如果a<b则返回真
	class qs_cmp_t
	{
	public:
		template<class DT>
		bool operator()(const DT &a, const DT &b) const { return a < b; }
	};

	//-----------------------------------------------------
	//快速排序算法的简单实现,CT 默认为 qs_cmp_t
	template<class DT, class IT, class CT >
	void quick_sort(DT *arr, IT left, IT right, const CT &lt = qs_cmp_t())
	{
		if (left >= right) return;

		IT i = left, j = right;
		DT x = arr[left];									// 以最左侧的数作为基准值,也是作为第一个"坑"的位置
		while (i < j)
		{
			while (i < j && !lt(arr[j], x))
				j--;										// 从右向左找第一个小于x的位置j
			if (i < j)
				arr[i++] = arr[j];							// 右侧值填入左侧的坑

			while (i < j && lt(arr[i], x))
				i++;										// 从左向右找第一个大于等于x的数
			if (i < j)
				arr[j--] = arr[i];							// 左侧的值填入右侧的坑
		}
		rx_assert(i == j);
		arr[i] = x;											// 现在i==j,把第一个坑的内容填回到最后的坑

		if (i)
			quick_sort(arr, left, i - 1, lt);				// 递归调用,处理左边
		quick_sort(arr, i + 1, right, lt);					// 递归调用,处理右边
	}

	//-----------------------------------------------------
	//对指定长度的数组进行快速排序,CT 默认为 qs_cmp_t
	template<class DT, class CT>
	void quick_sort(DT *arr, uint32_t size, const CT &lt = qs_cmp_t())
	{
		quick_sort<DT, uint32_t, CT>(arr, (uint32_t)0, (uint32_t)(size - 1), lt);
	}
	//-----------------------------------------------------
}


#endif
