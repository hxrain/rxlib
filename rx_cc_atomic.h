#ifndef _RX_CC_ATOMIC_H_
#define _RX_CC_ATOMIC_H_

    #include "rx_cc_macro.h"
/*
    ���ڱ�������"Compiler Intrinsics"����,��װһ�׼�ʵ�õ�ǿһ��ԭ�Ӳ���API.
*/

    //-----------------------------------------------------
    //ԭ�Ӳ���APIԭ��
    //��var����ֵv
    template<class T>  void rx_atomic_store(T &var, const T v);
    //װ��varԭ�ӱ�������������ֵ
    template<class T>  T    rx_atomic_load(const volatile T &var);
    //��var��v����ֵ����,���ؾ�ֵ
    template<class T>  T    rx_atomic_swap(T &var, const T v);
    //��var���мӷ�,���ؾ�ֵ
    template<class T>  T    rx_atomic_add(T &var, const T v);
    //��var���м���,���ؾ�ֵ
    template<class T>  T    rx_atomic_sub(T &var, const T v);
    //��var����λ��,���ؾ�ֵ
    template<class T>  T    rx_atomic_and(T &var, const T v);
    //��var����λ��,���ؾ�ֵ
    template<class T>  T    rx_atomic_or(T &var, const T v);
    //��var�������,���ؾ�ֵ
    template<class T>  T    rx_atomic_xor(T &var, const T v);
    //�Ƚϲ�����:var��*expected�Ƚ�,�����ͬ��var����Ϊdesired,����true;������*expected�з����ֵ,����false.
    template<class T>  bool rx_atomic_cas(T &var, T* expected, const T desired);
    //�Ƚϲ�����:var��expected�Ƚ�,�����ͬ��var����Ϊdesired,����true;���򷵻�false.
    template<class T>  bool rx_atomic_cas(T &var, const T expected, const T desired);

    #if RX_CC==RX_CC_VC
        #include <intrin.h>     //ʹ��VC���ڽ�ָ��

        //�ڴ���դָ��
        #define mem_barrier()    _ReadWriteBarrier()

        //32bit--------------------------------------------
        template<> inline void      rx_atomic_store(int32_t &var, const int32_t v)  { _InterlockedExchange((volatile long *)&var,v); }
        template<> inline int32_t   rx_atomic_load(const volatile int32_t &var)     { const int32_t v = var; mem_barrier(); return v; }
        template<> inline int32_t   rx_atomic_swap(int32_t &var, const int32_t v)   { return _InterlockedExchange((volatile long *)&var, v); }
        template<> inline int32_t   rx_atomic_add(int32_t &var, const int32_t v)    { return _InterlockedExchangeAdd((volatile long *)&var, v); }
        template<> inline int32_t   rx_atomic_sub(int32_t &var, const int32_t v)    { return _InterlockedExchangeAdd((volatile long *)&var,0-v); }
        template<> inline int32_t   rx_atomic_and(int32_t &var, const int32_t v)    { return _InterlockedAnd((volatile long *)&var, v); }
        template<> inline int32_t   rx_atomic_or(int32_t &var, const int32_t v)     { return _InterlockedOr((volatile long *)&var, v); }
        template<> inline int32_t   rx_atomic_xor(int32_t &var, const int32_t v)    { return _InterlockedXor((volatile long *)&var, v); }
        template<> inline bool      rx_atomic_cas(int32_t &var, int32_t* expected, const int32_t desired)
        {
            int32_t _Prev = _InterlockedCompareExchange((volatile long *)&var, desired, *expected);
            if (_Prev == *expected) return true;
            else
            {
                *expected = _Prev;
                return false;
            }
        }
        template<> inline bool        rx_atomic_cas(int32_t &var, const int32_t expected, const int32_t desired)
        {
            return _InterlockedCompareExchange((volatile long *)&var, desired, expected)==expected;
        }
        //64bit--------------------------------------------
        #if RX_CC_BIT==64
        #define RX_ATOMIC64     1                                //VCֻ����x64ģʽ���б������ڽ���64λ��ԭ�Ӳ���API

        template<> inline void      rx_atomic_store(int64_t &var, const int64_t v)  { _InterlockedExchange64((volatile long long *)&var, v); }
        template<> inline int64_t   rx_atomic_load(const volatile int64_t &var)     { const int64_t v = var; mem_barrier(); return v; }
        template<> inline int64_t   rx_atomic_swap(int64_t &var, const int64_t v)   { return _InterlockedExchange64((volatile long long *)&var, v); }
        template<> inline int64_t   rx_atomic_add(int64_t &var, const int64_t v)    { return _InterlockedExchangeAdd64((volatile long long*)&var, v); }
        template<> inline int64_t   rx_atomic_sub(int64_t &var, const int64_t v)    { return _InterlockedExchangeAdd64((volatile long long*)&var, 0-v); }
        template<> inline int64_t   rx_atomic_and(int64_t &var, const int64_t v)    { return _InterlockedAnd64((volatile long long*)&var, v); }
        template<> inline int64_t   rx_atomic_or(int64_t &var, const int64_t v)     { return _InterlockedOr64((volatile long long*)&var, v); }
        template<> inline int64_t   rx_atomic_xor(int64_t &var, const int64_t v)    { return _InterlockedXor64((volatile long long*)&var, v); }
        template<> inline bool      rx_atomic_cas(int64_t &var, int64_t *expected, const int64_t desired)
        {
            int64_t _Prev = _InterlockedCompareExchange64((volatile long long*)&var, desired, *expected);
            if (_Prev == *expected) return true;
            else
            {
                *expected = _Prev;
                return false;
            }
        }
        template<> inline bool      rx_atomic_cas(int64_t &var, const int64_t expected, const int64_t desired)
        {
            return _InterlockedCompareExchange64((volatile long long*)&var, desired, expected) == expected;
        }
        #endif
        //-------------------------------------------------

    #elif (RX_CC==RX_CC_GCC||RX_CC==RX_CC_CLANG)
        //�ڴ���դָ��
        #define mem_barrier()    __sync_synchronize()

        //32bit--------------------------------------------
        template<> inline void      rx_atomic_store (int32_t &var, const int32_t v)   { __atomic_store_n(&var,v,__ATOMIC_SEQ_CST); }
        template<> inline int32_t   rx_atomic_load  (const volatile int32_t &var)     { return __atomic_load_n(&var,__ATOMIC_SEQ_CST); }
        template<> inline int32_t   rx_atomic_swap  (int32_t &var, const int32_t v)   { return __atomic_exchange_n(&var, v,__ATOMIC_SEQ_CST); }
        template<> inline int32_t   rx_atomic_add   (int32_t &var, const int32_t v)   { return __sync_fetch_and_add (&var, v); }
        template<> inline int32_t   rx_atomic_sub   (int32_t &var, const int32_t v)   { return __sync_fetch_and_sub (&var, v); }
        template<> inline int32_t   rx_atomic_and   (int32_t &var, const int32_t v)   { return __sync_fetch_and_and (&var, v); }
        template<> inline int32_t   rx_atomic_or    (int32_t &var, const int32_t v)   { return __sync_fetch_and_or(&var, v); }
        template<> inline int32_t   rx_atomic_xor   (int32_t &var, const int32_t v)   { return __sync_fetch_and_xor (&var, v); }
        template<> inline bool      rx_atomic_cas   (int32_t &var, int32_t* expected, const int32_t desired)
        {
            return __atomic_compare_exchange_n(&var, expected,desired, 0,__ATOMIC_SEQ_CST,__ATOMIC_SEQ_CST);
        }
        template<> inline bool      rx_atomic_cas(int32_t &var, const int32_t expected, const int32_t desired)
        {
            return __atomic_compare_exchange_n(&var, (int32_t*)&expected,desired,0,__ATOMIC_SEQ_CST,__ATOMIC_SEQ_CST);
        }

        //64bit--------------------------------------------
        #define RX_ATOMIC64     1
        template<> inline void      rx_atomic_store (int64_t &var, const int64_t v)   { __atomic_store_n(&var,v,__ATOMIC_SEQ_CST); }
        template<> inline int64_t   rx_atomic_swap  (int64_t &var, const int64_t v)   { return __atomic_exchange_n(&var, v,__ATOMIC_SEQ_CST); }
        template<> inline int64_t   rx_atomic_load  (const volatile int64_t &var)     { return __atomic_load_n(&var,__ATOMIC_SEQ_CST); }
        template<> inline int64_t   rx_atomic_add   (int64_t &var, const int64_t v)   { return __sync_fetch_and_add ((volatile long long*)&var, v); }
        template<> inline int64_t   rx_atomic_sub   (int64_t &var, const int64_t v)   { return __sync_fetch_and_sub ((volatile long long*)&var, v); }
        template<> inline int64_t   rx_atomic_and   (int64_t &var, const int64_t v)   { return __sync_fetch_and_and ((volatile long long*)&var, v); }
        template<> inline int64_t   rx_atomic_or    (int64_t &var, const int64_t v)   { return __sync_fetch_and_or ((volatile long long*)&var, v); }
        template<> inline int64_t   rx_atomic_xor   (int64_t &var, const int64_t v)   { return __sync_fetch_and_xor ((volatile long long*)&var, v); }
        template<> inline bool      rx_atomic_cas   (int64_t &var, int64_t* expected, const int64_t desired)
        {
            return __atomic_compare_exchange_n(&var, expected,desired, 0,__ATOMIC_SEQ_CST,__ATOMIC_SEQ_CST);
        }
        template<> inline bool      rx_atomic_cas(int64_t &var, const int64_t expected, const int64_t desired)
        {
            return __atomic_compare_exchange_n(&var, (int64_t*)&expected,desired,0,__ATOMIC_SEQ_CST,__ATOMIC_SEQ_CST);
        }
        //-------------------------------------------------
    #endif
#endif
