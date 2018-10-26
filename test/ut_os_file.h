#ifndef _RX_UT_OS_FILE_H_
#define _RX_UT_OS_FILE_H_

#include "../rx_os_file.h"
#include "../rx_tdd.h"

//---------------------------------------------------------
inline void ut_os_file_base_1(rx_tdd_t &rt)
{
    printf("11");
    const char* filename = "ut_tmp_file.txt";
    remove(filename);

    std::string str;
    rt.tdd_assert(rx::load_from_file(filename, str) < 0);

    str = "123456789012345678901234567890";
    rt.tdd_assert(rx::save_to_file(filename, str.c_str(), str.size()) == 30);

    std::string str1;
    rt.tdd_assert(rx::load_from_file(filename, str1) == 30);
    rt.tdd_assert(str == str1);

    remove(filename);
}


rx_tdd(ut_os_file_base)
{
    ut_os_file_base_1(*this);
}


#endif
