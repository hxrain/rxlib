#ifndef _RX_CC_ATOMIC_H_
#define _RX_CC_ATOMIC_H_

#include "rx_cc_macro.h"
/*
    ���ڱ�������"Compiler Intrinsics"����,��װһ�׼�ʵ�õ�ǿһ��ԭ�Ӳ���API.
*/

    //��*ptr����ֵv
    template<class T>  void rx_atomic_store(T *ptr, const T v);
    //װ��*ptrԭ�ӱ�������������ֵ
    template<class T>  T    rx_atomic_load(const T *ptr);
    //��*ptr��v����ֵ����,���ؾ�ֵ
    template<class T>  T    rx_atomic_swap(T *ptr, const T v);
    //��*ptr���мӷ�,���ؾ�ֵ
    template<class T>  T    rx_atomic_add(T *ptr, const T v);
    //��*ptr���м���,���ؾ�ֵ
    template<class T>  T    rx_atomic_sub(T *ptr, const T v);
    //��*ptr����λ��,���ؾ�ֵ
    template<class T>  T    rx_atomic_and(T *ptr, const T v);
    //��*ptr����λ��,���ؾ�ֵ
    template<class T>  T    rx_atomic_or(T *ptr, const T v);
    //��*ptr�������,���ؾ�ֵ
    template<class T>  T    rx_atomic_xor(T *ptr, const T v);
    //�Ƚϲ�����:*ptr��*expected�Ƚ�,�����ͬ��*ptr����Ϊdesired,����true;������*expected�з����ֵ,����false.
    template<class T>  bool rx_atomic_cas(T* ptr, T* expected, const T desired);
    //�Ƚϲ�����:*ptr��expected�Ƚ�,�����ͬ��*ptr����Ϊdesired,����true;���򷵻�false.
    template<class T>  bool rx_atomic_cas(T* ptr, const T expected, const T desired);

#if RX_CC==RX_CC_VC
    #include <intrin.h>     //ʹ��VC���ڽ�ָ��

	//�ڴ���դָ��
	#define mem_barrier()	_ReadWriteBarrier()

    //32bit-----------------------------------------------------
	template<>  void        rx_atomic_store(int32_t *ptr, const int32_t v)  { _InterlockedExchange((volatile long *)ptr,v); }
	template<>  int32_t		rx_atomic_load(const int32_t *ptr)              { const int32_t v = *ptr; mem_barrier(); return v; }
	template<>  int32_t		rx_atomic_swap(int32_t *ptr, const int32_t v)   { return _InterlockedExchange((volatile long *)ptr, v); }
    template<>  int32_t		rx_atomic_add(int32_t *ptr, const int32_t v)    { return _InterlockedExchangeAdd((volatile long *)ptr, v); }
	template<>  int32_t		rx_atomic_sub(int32_t *ptr, const int32_t v)    { return _InterlockedExchangeAdd((volatile long *)ptr,0-v); }
	template<>  int32_t		rx_atomic_and(int32_t *ptr, const int32_t v)    { return _InterlockedAnd((volatile long *)ptr, v); }
	template<>  int32_t		rx_atomic_or(int32_t *ptr, const int32_t v)     { return _InterlockedOr((volatile long *)ptr, v); }
	template<>  int32_t		rx_atomic_xor(int32_t *ptr, const int32_t v)    { return _InterlockedXor((volatile long *)ptr, v); }
    template<>  bool		rx_atomic_cas(int32_t* ptr, int32_t* expected, const int32_t desired)
    {
        int32_t _Prev = _InterlockedCompareExchange((volatile long *)ptr, desired, *expected);
        if (_Prev == *expected) return true;
        else
        {
            *expected = _Prev;
            return false;
        }
    }
    template<>  bool		rx_atomic_cas(int32_t* ptr, const int32_t expected, const int32_t desired)
    {
        return _InterlockedCompareExchange((volatile long *)ptr, desired, expected)==expected;
    }

    //64bit-----------------------------------------------------
	template<>  void		rx_atomic_store(int64_t *ptr, const int64_t v)  { _InterlockedExchange64((volatile long long *)ptr, v); }
	template<>  int64_t		rx_atomic_load(const int64_t *ptr)              { const int64_t v = *ptr; mem_barrier(); return v; }
	template<>  int64_t		rx_atomic_swap(int64_t *ptr, const int64_t v)   { return _InterlockedExchange64((volatile long long *)ptr, v); }
	template<>  int64_t		rx_atomic_add(int64_t *ptr, const int64_t v)    { return _InterlockedExchangeAdd64((volatile long long*)ptr, v); }
	template<>  int64_t		rx_atomic_sub(int64_t *ptr, const int64_t v)    { return _InterlockedExchangeAdd64((volatile long long*)ptr, 0-v); }
	template<>  int64_t		rx_atomic_and(int64_t *ptr, const int64_t v)    { return _InterlockedAnd64((volatile long long*)ptr, v); }
    template<>  int64_t		rx_atomic_or(int64_t *ptr, const int64_t v)     { return _InterlockedOr64((volatile long long*)ptr, v); }
    template<>  int64_t		rx_atomic_xor(int64_t *ptr, const int64_t v)    { return _InterlockedXor64((volatile long long*)ptr, v); }
	template<>  bool		rx_atomic_cas(int64_t* ptr, int64_t *expected, const int64_t desired)
    {
        int64_t _Prev = _InterlockedCompareExchange64((volatile long long*)ptr, desired, *expected);
        if (_Prev == *expected) return true;
        else
        {
            *expected = _Prev;
            return false;
        }
    }
    template<>  bool		rx_atomic_cas(int64_t* ptr, const int64_t expected, const int64_t desired)
    {
        return _InterlockedCompareExchange64((volatile long long*)ptr, desired, expected) == expected;
    }
    //-----------------------------------------------------

#elif (RX_CC==RX_CC_GCC||RX_CC==RX_CC_CLANG||RX_CC==RX_CC_CYGWIN||RX_CC==RX_CC_MINGW32||RX_CC==RX_CC_MINGW64)
    //�ڴ���դָ��
    #define mem_barrier()	__sync_synchronize()

    //32bit-----------------------------------------------------
    template<>  void        rx_atomic_store (int32_t *ptr, const int32_t v)   { __atomic_store_4(ptr,v,__ATOMIC_SEQ_CST); }
    template<>  int32_t		rx_atomic_load  (const int32_t *ptr)              { return __atomic_load_4(ptr,__ATOMIC_SEQ_CST); }
    template<>  int32_t		rx_atomic_swap  (int32_t *ptr, const int32_t v)   { return __atomic_exchange_4(ptr, v,__ATOMIC_SEQ_CST); }
    template<>  int32_t		rx_atomic_add   (int32_t *ptr, const int32_t v)   { return __sync_fetch_and_add (ptr, v); }
    template<>  int32_t		rx_atomic_sub   (int32_t *ptr, const int32_t v)   { return __sync_fetch_and_sub (ptr, v); }
    template<>  int32_t		rx_atomic_and   (int32_t *ptr, const int32_t v)   { return __sync_fetch_and_and (ptr, v); }
    template<>  int32_t		rx_atomic_or    (int32_t *ptr, const int32_t v)   { return __sync_fetch_and_or(ptr, v); }
    template<>  int32_t		rx_atomic_xor   (int32_t *ptr, const int32_t v)   { return __sync_fetch_and_xor (ptr, v); }
    template<>  bool		rx_atomic_cas   (int32_t* ptr, int32_t* expected, const int32_t desired)
    {
        return __atomic_compare_exchange_4(ptr, expected,desired, 0,__ATOMIC_SEQ_CST,__ATOMIC_SEQ_CST);
    }
    template<>  bool		rx_atomic_cas(int32_t* ptr, const int32_t expected, const int32_t desired)
    {
        return __atomic_compare_exchange_4(ptr, (void*)&expected,desired,0,__ATOMIC_SEQ_CST,__ATOMIC_SEQ_CST);
    }

    //64bit-----------------------------------------------------
    template<>  void		rx_atomic_store (int64_t *ptr, const int64_t v)   { __atomic_store_8(ptr,v,__ATOMIC_SEQ_CST); }
    template<>  int64_t		rx_atomic_load  (const int64_t *ptr)              { return __atomic_load_8(ptr,__ATOMIC_SEQ_CST); }
    template<>  int64_t		rx_atomic_swap  (int64_t *ptr, const int64_t v)   { return __atomic_exchange_8(ptr, v,__ATOMIC_SEQ_CST); }
    template<>  int64_t		rx_atomic_add   (int64_t *ptr, const int64_t v)   { return __sync_fetch_and_add ((volatile long long*)ptr, v); }
    template<>  int64_t		rx_atomic_sub   (int64_t *ptr, const int64_t v)   { return __sync_fetch_and_sub ((volatile long long*)ptr, v); }
    template<>  int64_t		rx_atomic_and   (int64_t *ptr, const int64_t v)   { return __sync_fetch_and_and ((volatile long long*)ptr, v); }
    template<>  int64_t		rx_atomic_or    (int64_t *ptr, const int64_t v)   { return __sync_fetch_and_or ((volatile long long*)ptr, v); }
    template<>  int64_t		rx_atomic_xor   (int64_t *ptr, const int64_t v)   { return __sync_fetch_and_xor ((volatile long long*)ptr, v); }
    template<>  bool		rx_atomic_cas   (int64_t* ptr, int64_t* expected, const int64_t desired)
    {
        return __atomic_compare_exchange_8(ptr, expected,desired, 0,__ATOMIC_SEQ_CST,__ATOMIC_SEQ_CST);
    }
    template<>  bool		rx_atomic_cas(int64_t* ptr, const int64_t expected, const int64_t desired)
    {
        return __atomic_compare_exchange_8(ptr, (void*)&expected,desired,0,__ATOMIC_SEQ_CST,__ATOMIC_SEQ_CST);
    }
    //-----------------------------------------------------
#endif




#endif
