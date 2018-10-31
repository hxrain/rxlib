#ifndef _RX_UT_OS_FILE_H_
#define _RX_UT_OS_FILE_H_

#include "../rx_os_file.h"
#include "../rx_tdd.h"

//---------------------------------------------------------
inline void ut_os_file_base_1(rx_tdd_t &rt)
{
    const char* filename = "ut_tmp_file.txt";
    remove(filename);

    std::string str;
    rt.tdd_assert(rx::load_from_file(filename, str));

    str = "123456789012345678901234567890";
    rt.tdd_assert(rx::save_to_file(filename, str.c_str(), str.size()) == rx::ec_ok);

    std::string str1;
    rt.tdd_assert(rx::load_from_file(filename, str1) == rx::ec_ok);
    rt.tdd_assert(str == str1);

    remove(filename);
}

//---------------------------------------------------------
inline void ut_os_file_base_2(rx_tdd_t &rt)
{
    const char* filename = "ut_tmp_file.txt";
    remove(filename);

    rx::os_file_t file;
    rt.tdd_assert(file.open(filename, "a+")==rx::ec_ok);

    rt.tdd_assert(file.write("123", 3));
    rt.tdd_assert(file.seek(0, 0));
    rt.tdd_assert(file.write("456", 3));
    file.close();

    std::string str1;
    rt.tdd_assert(rx::load_from_file(filename, str1) == rx::ec_ok);
    rt.tdd_assert(str1 == "123456");

    remove(filename);
}

//---------------------------------------------------------
rx_tdd(ut_os_file_base)
{
    ut_os_file_base_1(*this);
    ut_os_file_base_2(*this);
}


#endif
