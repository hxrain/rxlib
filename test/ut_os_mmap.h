#ifndef _RX_UT_OS_MMAP_H_
#define _RX_UT_OS_MMAP_H_

#include "../rx_tdd.h"
#include "../rx_os_mmap.h"
#include "../rx_os_file.h"

//---------------------------------------------------------
inline void ut_os_mmap_base_1(rx_tdd_t &rt)
{
    const char* filename = "ut_tmp_map.txt";
    remove(filename);

    rx::os_file_t file;
    rt.tdd_assert(file.open(filename,"w+")== rx::ec_ok);

    rx::os_mmap_t mmap;
    rt.tdd_assert(mmap.open(file, "w+",20) == rx::ec_ok);
    if (!mmap.is_valid())
        return;

    rt.tdd_assert(mmap.write("1234567890", 10) == 10);
    rt.tdd_assert(mmap.flush());

    rx::os_file_t file2;
    rt.tdd_assert(file2.open(filename, "r+")== rx::ec_ok);

    rx::os_mmap_t mmap2;
    rt.tdd_assert(mmap2.open(file2, "w+", 20) == rx::ec_ok);
    rt.tdd_assert(strcmp((char*)mmap2.ptr(), "1234567890") == 0);

    rt.tdd_assert(mmap2.write("1234567891", 10) == 10);
    rt.tdd_assert(strcmp((char*)mmap.ptr(), "1234567891") == 0);

    mmap.close();
    file.close();

    mmap2.close();
    file2.close();

    remove(filename);
}


rx_tdd(ut_os_mmap_base)
{
    ut_os_mmap_base_1(*this);
}

#endif
