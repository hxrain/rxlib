
#include "../ut_mem_pool.h"
#include "../../rx_mem_pool.h"
#include "../../rx_raw_stack.h"
#include "../../rx_mem_cfg.h"

#include "../ut_ct_obj.h"
#include "../../rx_ct_util.h"
#include "../../rx_ct_traits.h"
#include "../ut_datetime.h"
#include "../../rx_datetime.h"
#include "../ut_atomic.h"
#include "../../rx_atomic.h"
#include "../ut_cc_atomic.h"
#include "../../rx_tdd.h"
#include "../../rx_assert.h"
#include "../../rx_cc_atomic.h"
#include "../../rx_cc_macro.h"

#include <stdio.h>
#include <iostream>

#define TSN(n,msg_a) int _tmp_##n(msg_a)

using namespace std;

int main()
{
    cout << "Hello world!\n" << rx_cc_desc() << endl << endl;
    TSN(n,1);
    TSN(m, 1);
    rx_tdd_run();
    _tmp_n = 2;
    _tmp_m = 2;

	getchar();
    return 0;
}
