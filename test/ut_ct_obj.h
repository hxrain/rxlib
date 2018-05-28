#ifndef _RX_UT_CTOBJ_H_
#define _RX_UT_CTOBJ_H_

#include "../rx_ct_util.h"
#include "../rx_tdd.h"

class tmp_tst_obj_1
{
public:
    int tmp_v1;
    tmp_tst_obj_1(int v):tmp_v1(v){}
};

class tmp_tst_obj_2:public tmp_tst_obj_1
{
public:
    tmp_tst_obj_2(int v):tmp_tst_obj_1(v){}
};

rx_tdd(tmp_obj)
{
    uint8_t buff_obj[sizeof(tmp_tst_obj_2)*5];
    tmp_tst_obj_2 *o2=(tmp_tst_obj_2*)buff_obj;
    rx::ct::OC(o2,2);
    assert(o2->tmp_v1==2);
    rx::ct::OD(o2);

    rx::ct::AC(o2,5,3);
    assert(o2->tmp_v1==3);
    assert((++o2)->tmp_v1==3);
    rx::ct::AD(--o2,5);

    assert(rx::Min(5,8,3)==3);
    assert(rx::Max(5,8,3)==8);
    //assert(0);
}



#endif // _RX_UT_CTOBJ_H_
