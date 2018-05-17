

#include "../../rx_assert.h"
#include "../../rx_cc_atomic.h"
#include "../../rx_cc_macro.h"

#include <stdio.h>
#include <iostream>

using namespace std;

template<class T>
void test_atomic_base()
{
    rx_st_assert(sizeof(T)==4||sizeof(T)==8,"sizeof()==4|8");

    T val = 0;
    rx_atomic_store(&val, (T)1);
    rx_assert(rx_atomic_load(&val) == 1);
    rx_assert(rx_atomic_add(&val, (T)1) == 1);
    rx_assert(rx_atomic_load(&val) == 2);
    rx_assert(rx_atomic_sub(&val, (T)1) == 2);
    rx_assert(rx_atomic_load(&val) == 1);
    rx_assert(rx_atomic_or(&val, (T)3) == 1);
    rx_assert(rx_atomic_load(&val) == 3);
    rx_assert(rx_atomic_and(&val, (T)1) == 3);
    rx_assert(rx_atomic_load(&val) == 1);
    rx_assert(rx_atomic_xor(&val, (T)1) == 1);
    rx_assert(rx_atomic_load(&val) == 0);
    rx_assert(rx_atomic_swap(&val, (T)100) == 0);
    rx_assert(rx_atomic_load(&val) == 100);
    rx_assert(rx_atomic_cas(&val, (T)100,(T)10));
    rx_assert(rx_atomic_load(&val) == 10);

    T tmp = 100;
    rx_assert(!rx_atomic_cas(&val, &tmp, (T)1));
    rx_assert(tmp==10);
    rx_assert(rx_atomic_load(&val) == 10);

    rx_atomic_store(&val, (T)-1);
    rx_assert(rx_atomic_load(&val) == -1);
    rx_assert(rx_atomic_sub(&val, (T)1) == -1);
    rx_assert(rx_atomic_load(&val) == -2);
    rx_assert(rx_atomic_add(&val, (T)1) == -2);
    rx_assert(rx_atomic_load(&val) == -1);
    rx_assert(rx_atomic_add(&val, (T)-1) == -1);
    rx_assert(rx_atomic_load(&val) == -2);
    rx_assert(rx_atomic_sub(&val, (T)-1) == -2);
    rx_assert(rx_atomic_load(&val) == -1);
    rx_assert(rx_atomic_add(&val, (T)1) == -1);
    rx_assert(rx_atomic_load(&val) == 0);
}

int main()
{
    test_atomic_base<int32_t>();
    test_atomic_base<int64_t>();

    cout << "Hello world!\n" << rx_cc_desc() << endl;
	getchar();
    return 0;
}
