
#include "../../rx_tdd.h"
#include "../../rx_assert.h"
#include "../../rx_cc_atomic.h"
#include "../../rx_cc_macro.h"

#include <stdio.h>
#include <iostream>

using namespace std;

template<class T>
void test_atomic_base(rx_tdd_base &tdd)
{
    rx_st_assert(sizeof(T)==4||sizeof(T)==8,"sizeof()==4|8");

    T val = 0;
    rx_atomic_store(&val, (T)1);
    tdd.assert(rx_atomic_load(&val) == 1);
    tdd.assert(rx_atomic_add(&val, (T)1) == 1);
    tdd.assert(rx_atomic_load(&val) == 2);
    tdd.assert(rx_atomic_sub(&val, (T)1) == 2);
    tdd.assert(rx_atomic_load(&val) == 1);
    tdd.assert(rx_atomic_or(&val, (T)3) == 1);
    tdd.assert(rx_atomic_load(&val) == 3);
    tdd.assert(rx_atomic_and(&val, (T)1) == 3);
    tdd.assert(rx_atomic_load(&val) == 1);
    tdd.assert(rx_atomic_xor(&val, (T)1) == 1);
    tdd.assert(rx_atomic_load(&val) == 0);
    tdd.assert(rx_atomic_swap(&val, (T)100) == 0);
    tdd.assert(rx_atomic_load(&val) == 100);
    tdd.assert(rx_atomic_cas(&val, (T)100,(T)10));
    tdd.assert(rx_atomic_load(&val) == 10);

    T tmp = 100;
    tdd.assert(!rx_atomic_cas(&val, &tmp, (T)1));
    tdd.assert(tmp==10);
    tdd.assert(rx_atomic_load(&val) == 10);

    rx_atomic_store(&val, (T)-1);
    tdd.assert(rx_atomic_load(&val) == -1);
    tdd.assert(rx_atomic_sub(&val, (T)1) == -1);
    tdd.assert(rx_atomic_load(&val) == -2);
    tdd.assert(rx_atomic_add(&val, (T)1) == -2);
    tdd.assert(rx_atomic_load(&val) == -1);
    tdd.assert(rx_atomic_add(&val, (T)-1) == -1);
    tdd.assert(rx_atomic_load(&val) == -2);
    tdd.assert(rx_atomic_sub(&val, (T)-1) == -2);
    tdd.assert(rx_atomic_load(&val) == -1);
    tdd.assert(rx_atomic_add(&val, (T)1) == -1);
    tdd.assert(rx_atomic_load(&val) == 0);
}

rx_tdd(rx_atomic_base)
{
    test_atomic_base<int32_t>(*this);
    test_atomic_base<int64_t>(*this);
}

rx_tdd(temp)
{
    assert(1);
    assert(0);
}

int main()
{
    cout << "Hello world!\n" << rx_cc_desc() << endl << endl;

    rx_tdd_run();

	getchar();
    return 0;
}
