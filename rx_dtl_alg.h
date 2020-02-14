#ifndef _RX_DTL_ALG_H_
#define _RX_DTL_ALG_H_

#include "rx_cc_macro.h"

namespace rx
{
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
}


#endif
