#ifndef _RX_DTL_ALG_H_
#define _RX_DTL_ALG_H_

#include "rx_cc_macro.h"
#include "rx_assert.h"

namespace rx
{
	//-----------------------------------------------------
    //���Ԫ������ET������Ķ��������㷨,�ڳ���Ϊlength��arr�����в���x(Ҫ��arrΪ����)
    //����ֵ:arrsize,δ�ҵ�;����Ϊx�������е�����
    template<class ET>
    inline uint32_t bisect(const ET* arr,const uint32_t length,const ET& x) {
        uint32_t left=0;
        uint32_t right=length-1;
        while(left<right) {
            uint32_t mid=(left+right)/2;
            const ET& ci=arr[mid];
            if(ci==x)
                return mid;
            else if(ci>x)
                right=mid-1;
            else
                left=mid+1;
        }
        if (left==right&&arr[left]==x)
            return left;
        return length;
    }

    //���Ԫ������ET������Ķ��������㷨,�ڳ���Ϊlength��arr�����в��������ӽ�x��ֵ����(Ҫ��arrΪ����)
    //����ֵ:arrsize,δ�ҵ�;����Ϊx�������е�����
    template<class ET>
    inline uint32_t bisect_ll(const ET* arr,const uint32_t length,const ET& x)
    {
        uint32_t left = 0, right = length-1;
        while(left < right)
        {
            uint32_t mid = (left+right)/2;
            if(arr[mid] <= x)
            {
                if (left == mid)
                    break;
                left = mid;
            }
            else
                right = mid;
        }

        if(arr[right] < x)
            return right;
        else if(arr[left] <= x)
            return left;

        return length;
    }


	//-----------------------------------------------------
	//������Ҫ��,С�ڱȽ���,���a<b�򷵻���
	template<class DT>
	class qs_cmp_t
	{
	public:
		bool operator()(const DT &a, const DT &b) const { return a < b; }
	};

	//-----------------------------------------------------
	//���������㷨�ļ�ʵ��
	template<class DT, class IT, class CT = qs_cmp_t<DT> >
	void quick_sort(DT *arr, IT left, IT right, const CT &lt = CT())
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
	//��ָ�����ȵ�������п�������
	template<class DT, class CT = qs_cmp_t<DT> >
	void quick_sort(DT *arr, uint32_t size,const CT &lt = CT())
	{
		quick_sort<DT, uint32_t, CT>(arr, (uint32_t)0, (uint32_t)(size - 1), lt);
	}
	//-----------------------------------------------------
}


#endif
