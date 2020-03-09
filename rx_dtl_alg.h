#ifndef _RX_DTL_ALG_H_
#define _RX_DTL_ALG_H_

#include "rx_cc_macro.h"
#include "rx_assert.h"

namespace rx
{
	//-----------------------------------------------------
	//���������㷨,����x(��ֵ��ʱ��ȷ�����ĸ�);����arr�ĳ���Ϊlength,��������
	//����ֵ:length,δ�ҵ�;����Ϊx�������е�����
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
	//���������㷨,����x����߽�λ��(�״γ���);����arr�ĳ���Ϊlength,��������
	//����ֵ:length,δ�ҵ�;����Ϊx�������е�����
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
	//���������㷨,����x���ұ߽�λ��(������);����arr�ĳ���Ϊlength,��������
	//����ֵ:length,δ�ҵ�;����Ϊx�������е�����
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
	//���������㷨,����<=x����߽�λ��;����arr�ĳ���Ϊlength,��������
	//����ֵ:length,δ�ҵ�;����Ϊx�������е�����
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
	//���������㷨,����<x����߽�λ��;����arr�ĳ���Ϊlength,��������
	//����ֵ:length,δ�ҵ�;����Ϊx�������е�����
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
	//���������㷨,����>x���ұ߽�λ��;����arr�ĳ���Ϊlength,��������
	//����ֵ:length,δ�ҵ�;����Ϊx�������е�����
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
	//������Ҫ��,С�ڱȽ���,���a<b�򷵻���
	class qs_cmp_t
	{
	public:
		template<class DT>
		bool operator()(const DT &a, const DT &b) const { return a < b; }
	};

	//-----------------------------------------------------
	//���������㷨�ļ�ʵ��,CT Ĭ��Ϊ qs_cmp_t
	template<class DT, class IT, class CT >
	void quick_sort(DT *arr, IT left, IT right, const CT &lt = qs_cmp_t())
	{
		if (left >= right) return;

		IT i = left, j = right;
		DT x = arr[left];									// ������������Ϊ��׼ֵ,Ҳ����Ϊ��һ��"��"��λ��
		while (i < j)
		{
			while (i < j && !lt(arr[j], x))
				j--;										// ���������ҵ�һ��С��x��λ��j
			if (i < j)
				arr[i++] = arr[j];							// �Ҳ�ֵ�������Ŀ�

			while (i < j && lt(arr[i], x))
				i++;										// ���������ҵ�һ�����ڵ���x����
			if (i < j)
				arr[j--] = arr[i];							// ����ֵ�����Ҳ�Ŀ�
		}
		rx_assert(i == j);
		arr[i] = x;											// ����i==j,�ѵ�һ���ӵ�������ص����Ŀ�

		if (i)
			quick_sort(arr, left, i - 1, lt);				// �ݹ����,�������
		quick_sort(arr, i + 1, right, lt);					// �ݹ����,�����ұ�
	}

	//-----------------------------------------------------
	//��ָ�����ȵ�������п�������,CT Ĭ��Ϊ qs_cmp_t
	template<class DT, class CT>
	void quick_sort(DT *arr, uint32_t size, const CT &lt = qs_cmp_t())
	{
		quick_sort<DT, uint32_t, CT>(arr, (uint32_t)0, (uint32_t)(size - 1), lt);
	}
	//-----------------------------------------------------
}


#endif
