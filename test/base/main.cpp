

#include "../../rx_cc_atomic.h"
#include "../../rx_cc_macro.h"

#include <stdio.h>
#include <iostream>

using namespace std;



int main()
{
	int32_t val = 0;
	int64_t val64 = 0;
	rx_atomic_store(&val, 1);
	rx_atomic_store(&val64, (int64_t)1);
	cout << "Hello world!\n" << rx_cc_desc();
	getchar();
    return 0;
}
