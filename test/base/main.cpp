
#include "../ut_atomic.h"
#include "../../rx_atomic.h"
#include "../ut_cc_atomic.h"
#include "../../rx_tdd.h"
#include "../../rx_assert.h"
#include "../../rx_cc_atomic.h"
#include "../../rx_cc_macro.h"

#include <stdio.h>
#include <iostream>

using namespace std;

rx_tdd(temp)
{
    assert(0);
}

int main()
{
    cout << "Hello world!\n" << rx_cc_desc() << endl << endl;

    rx_tdd_run();

	getchar();
    return 0;
}
