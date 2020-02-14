#ifndef _RX_DTL_ALG_H_
#define _RX_DTL_ALG_H_

#include "rx_cc_macro.h"

namespace rx
{
    //针对元素类型ET的数组的二分搜索算法,在长度为length的arr数组中查找x(要求arr为正序)
    //返回值:arrsize,未找到;其他为x在数组中的索引
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

    //针对元素类型ET的数组的二分搜索算法,在长度为length的arr数组中查找最左侧接近x的值索引(要求arr为正序)
    //返回值:arrsize,未找到;其他为x在数组中的索引
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
