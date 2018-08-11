#include "../ut_os_tasks.h"
#include "../../rx_os_tasks.h"
#include "../../rx_os_thread.h"

#include "../../rx_os_cond.h"
#include "../../rx_os_sem.h"

#include "../ut_os_lock.h"
#include "../../rx_os_lock.h"

#include "../ut_hash_tlmap.h"
#include "../../rx_hash_tlmap.h"

#include "../ut_mem_pool_cntr.h"
#include "../../rx_mempool_cntr.h"

#include "../ut_hash.h"
#include "../../rx_hash_data.h"
#include "../../rx_hash_int.h"

#include "../ut_bits_op.h"
#include "../../rx_bits_op.h"

#include "../ut_mem_pool.h"
#include "../../rx_mempool_fx1.h"
#include "../../rx_raw_stack.h"
#include "../../rx_mempool.h"


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

#include "../ut_cc_macro.h"
#include "../../rx_cc_macro.h"

#include <stdio.h>
#include <iostream>

using namespace std;

int main()
{
    cout << "ÄãºÃHello world!\n" << rx_cc_desc() << endl << endl;
    rx_static_assert(1);

    rx_tdd_run(rtl_2);

	getchar();
    return 0;
}
