#include <string.h>
#include <sstream>
#include <math.h>

#include "../rx_tdd.h"
#include "../rx_str_util_fmt.h"

/*
    代码参考 reference https://github.com/mpaland/printf/blob/master/test/test_suite.cpp
*/

const int ut_printf_buffer_size=100;
static char   ut_printf_buffer[ut_printf_buffer_size];

rx_tddm(fmt,"fmt :: ntoa") {
    memset(ut_printf_buffer, 0xCC, ut_printf_buffer_size);
    double value=-123.4232;
    tdd_assert(rx::st::ftoa(value,ut_printf_buffer,4) == ut_printf_buffer);
    tdd_assert(ut_printf_buffer[9] == 0);
    tdd_assert(!strcmp(ut_printf_buffer, "-123.4232"));

    tdd_assert(rx::st::ftoa(value,ut_printf_buffer,4,true) == ut_printf_buffer);
    tdd_assert(ut_printf_buffer[11] == 0);
    tdd_assert(!strcmp(ut_printf_buffer, "-1.2342e+02"));

    tdd_assert(rx::st::ftoa(value,ut_printf_buffer,6,true) == ut_printf_buffer);
    tdd_assert(ut_printf_buffer[13] == 0);
    tdd_assert(!strcmp(ut_printf_buffer, "-1.234232e+02"));
}

rx_tddm(fmt,"fmt :: ntoa") {
    memset(ut_printf_buffer, 0xCC, ut_printf_buffer_size);
    tdd_assert(rx::st::ntoa(4232,ut_printf_buffer) == ut_printf_buffer);
    tdd_assert(ut_printf_buffer[4] == 0);
    tdd_assert(!strcmp(ut_printf_buffer, "4232"));

    tdd_assert(rx::st::ntoa(-4232,ut_printf_buffer) == ut_printf_buffer);
    tdd_assert(!strcmp(ut_printf_buffer, "-4232"));

    tdd_assert(rx::st::ntoa(-4232,ut_printf_buffer,16) == ut_printf_buffer);
    tdd_assert(!strcmp(ut_printf_buffer, "ffffef78"));

    tdd_assert(rx::st::itoa(-4232,ut_printf_buffer) == ut_printf_buffer);
    tdd_assert(!strcmp(ut_printf_buffer, "-4232"));

    tdd_assert(rx::st::itoa(-4232,ut_printf_buffer,16) == ut_printf_buffer);
    tdd_assert(!strcmp(ut_printf_buffer, "ffffef78"));

    tdd_assert(rx::st::itox(-4232,ut_printf_buffer) == ut_printf_buffer);
    tdd_assert(!strcmp(ut_printf_buffer, "0xffffef78"));

    tdd_assert(rx::st::utox(-4232,ut_printf_buffer) == ut_printf_buffer);
    tdd_assert(!strcmp(ut_printf_buffer, "0xffffef78"));
}

rx_tdd(sprintf_base) {
  memset(ut_printf_buffer, 0xCC, ut_printf_buffer_size);
  tdd_assert(rx::st::sprintf(ut_printf_buffer,"% d", 4232) == 5);
  tdd_assert(ut_printf_buffer[5] == 0);
  tdd_assert(!strcmp(ut_printf_buffer, " 4232"));
}


rx_tdd(sprintf_base_X) {
  memset(ut_printf_buffer, 0xCC, ut_printf_buffer_size);
  rx::st::sprintf(ut_printf_buffer, "This is a test of %X", 0x12EFU);
  tdd_assert(!strncmp(ut_printf_buffer, "This is a test of 12EF", 22U));
  tdd_assert(ut_printf_buffer[22] == (char)0);
}


rx_tdd(snprintf_base) {
  rx::st::snprintf(ut_printf_buffer, ut_printf_buffer_size, "%d", -1000);
  tdd_assert(!strcmp(ut_printf_buffer, "-1000"));

  rx::st::snprintf(ut_printf_buffer, 3U, "%d", -1000);
  tdd_assert(!strcmp(ut_printf_buffer, "-1"));
}

static void vsnprintf_builder_1(char* buffer, ...)
{
  va_list args;
  va_start(args, buffer);
  rx::st::vsnprintf(buffer, ut_printf_buffer_size, "%d", args);
  va_end(args);
}

static void vsnprintf_builder_3(char* buffer, ...)
{
  va_list args;
  va_start(args, buffer);
  rx::st::vsnprintf(buffer, ut_printf_buffer_size, "%d %d %s", args);
  va_end(args);
}

rx_tddm(fmt,"fmt :: vsnprintf") {
  memset(ut_printf_buffer, 0xCC, ut_printf_buffer_size);
  vsnprintf_builder_1(ut_printf_buffer, 2345);
  tdd_assert(ut_printf_buffer[4] == 0);
  tdd_assert(!strcmp(ut_printf_buffer, "2345"));
}

rx_tddm(fmt,"fmt :: vsnprintf") {
  vsnprintf_builder_1(ut_printf_buffer, -1);
  tdd_assert(!strcmp(ut_printf_buffer, "-1"));

  vsnprintf_builder_3(ut_printf_buffer, 3, -1000, "test");
  tdd_assert(!strcmp(ut_printf_buffer, "3 -1000 test"));
}

rx_tddm(fmt,"fmt :: space flag") {
  char buffer[100];

  rx::st::sprintf(buffer, "% d", 42);
  tdd_assert(!strcmp(buffer, " 42"));

  rx::st::sprintf(buffer, "% d", -42);
  tdd_assert(!strcmp(buffer, "-42"));

  rx::st::sprintf(buffer, "% 5d", 42);
  tdd_assert(!strcmp(buffer, "   42"));

  rx::st::sprintf(buffer, "% 5d", -42);
  tdd_assert(!strcmp(buffer, "  -42"));

  rx::st::sprintf(buffer, "% 15d", 42);
  tdd_assert(!strcmp(buffer, "             42"));

  rx::st::sprintf(buffer, "% 15d", -42);
  tdd_assert(!strcmp(buffer, "            -42"));

  rx::st::sprintf(buffer, "% 15d", -42);
  tdd_assert(!strcmp(buffer, "            -42"));

  rx::st::sprintf(buffer, "% 15.3f", -42.987);
  tdd_assert(!strcmp(buffer, "        -42.987"));

  rx::st::sprintf(buffer, "% 15.3f", 42.987);
  tdd_assert(!strcmp(buffer, "         42.987"));

  rx::st::sprintf(buffer, "% s", "Hello testing");
  tdd_assert(!strcmp(buffer, "Hello testing"));

  rx::st::sprintf(buffer, "% d", 1024);
  tdd_assert(!strcmp(buffer, " 1024"));

  rx::st::sprintf(buffer, "% d", -1024);
  tdd_assert(!strcmp(buffer, "-1024"));

  rx::st::sprintf(buffer, "% i", 1024);
  tdd_assert(!strcmp(buffer, " 1024"));

  rx::st::sprintf(buffer, "% i", -1024);
  tdd_assert(!strcmp(buffer, "-1024"));

  rx::st::sprintf(buffer, "% u", 1024);
  tdd_assert(!strcmp(buffer, "1024"));

  rx::st::sprintf(buffer, "% u", 4294966272U);
  tdd_assert(!strcmp(buffer, "4294966272"));

  rx::st::sprintf(buffer, "% o", 511);
  tdd_assert(!strcmp(buffer, "777"));

  rx::st::sprintf(buffer, "% o", 4294966785U);
  tdd_assert(!strcmp(buffer, "37777777001"));

  rx::st::sprintf(buffer, "% x", 305441741);
  tdd_assert(!strcmp(buffer, "1234abcd"));

  rx::st::sprintf(buffer, "% x", 3989525555U);
  tdd_assert(!strcmp(buffer, "edcb5433"));

  rx::st::sprintf(buffer, "% X", 305441741);
  tdd_assert(!strcmp(buffer, "1234ABCD"));

  rx::st::sprintf(buffer, "% X", 3989525555U);
  tdd_assert(!strcmp(buffer, "EDCB5433"));

  rx::st::sprintf(buffer, "% c", 'x');
  tdd_assert(!strcmp(buffer, "x"));
}

rx_tddm(fmt,"fmt :: + flag") {
  char buffer[100];

  rx::st::sprintf(buffer, "%+d", 42);
  tdd_assert(!strcmp(buffer, "+42"));

  rx::st::sprintf(buffer, "%+d", -42);
  tdd_assert(!strcmp(buffer, "-42"));

  rx::st::sprintf(buffer, "%+5d", 42);
  tdd_assert(!strcmp(buffer, "  +42"));

  rx::st::sprintf(buffer, "%+5d", -42);
  tdd_assert(!strcmp(buffer, "  -42"));

  rx::st::sprintf(buffer, "%+15d", 42);
  tdd_assert(!strcmp(buffer, "            +42"));

  rx::st::sprintf(buffer, "%+15d", -42);
  tdd_assert(!strcmp(buffer, "            -42"));

  rx::st::sprintf(buffer, "%+s", "Hello testing");
  tdd_assert(!strcmp(buffer, "Hello testing"));

  rx::st::sprintf(buffer, "%+d", 1024);
  tdd_assert(!strcmp(buffer, "+1024"));

  rx::st::sprintf(buffer, "%+d", -1024);
  tdd_assert(!strcmp(buffer, "-1024"));

  rx::st::sprintf(buffer, "%+i", 1024);
  tdd_assert(!strcmp(buffer, "+1024"));

  rx::st::sprintf(buffer, "%+i", -1024);
  tdd_assert(!strcmp(buffer, "-1024"));

  rx::st::sprintf(buffer, "%+u", 1024);
  tdd_assert(!strcmp(buffer, "1024"));

  rx::st::sprintf(buffer, "%+u", 4294966272U);
  tdd_assert(!strcmp(buffer, "4294966272"));

  rx::st::sprintf(buffer, "%+o", 511);
  tdd_assert(!strcmp(buffer, "777"));

  rx::st::sprintf(buffer, "%+o", 4294966785U);
  tdd_assert(!strcmp(buffer, "37777777001"));

  rx::st::sprintf(buffer, "%+x", 305441741);
  tdd_assert(!strcmp(buffer, "1234abcd"));

  rx::st::sprintf(buffer, "%+x", 3989525555U);
  tdd_assert(!strcmp(buffer, "edcb5433"));

  rx::st::sprintf(buffer, "%+X", 305441741);
  tdd_assert(!strcmp(buffer, "1234ABCD"));

  rx::st::sprintf(buffer, "%+X", 3989525555U);
  tdd_assert(!strcmp(buffer, "EDCB5433"));

  rx::st::sprintf(buffer, "%+c", 'x');
  tdd_assert(!strcmp(buffer, "x"));

  rx::st::sprintf(buffer, "%+.0d", 0);
  tdd_assert(!strcmp(buffer, "+"));
}

rx_tddm(fmt,"fmt :: 0 flag") {
  char buffer[100];

  rx::st::sprintf(buffer, "%0d", 42);
  tdd_assert(!strcmp(buffer, "42"));

  rx::st::sprintf(buffer, "%0ld", 42L);
  tdd_assert(!strcmp(buffer, "42"));

  rx::st::sprintf(buffer, "%0d", -42);
  tdd_assert(!strcmp(buffer, "-42"));

  rx::st::sprintf(buffer, "%05d", 42);
  tdd_assert(!strcmp(buffer, "00042"));

  rx::st::sprintf(buffer, "%05d", -42);
  tdd_assert(!strcmp(buffer, "-0042"));

  rx::st::sprintf(buffer, "%015d", 42);
  tdd_assert(!strcmp(buffer, "000000000000042"));

  rx::st::sprintf(buffer, "%015d", -42);
  tdd_assert(!strcmp(buffer, "-00000000000042"));

  rx::st::sprintf(buffer, "%015.2f", 42.1234);
  tdd_assert(!strcmp(buffer, "000000000042.12"));

  rx::st::sprintf(buffer, "%015.3f", 42.9876);
  tdd_assert(!strcmp(buffer, "00000000042.988"));

  rx::st::sprintf(buffer, "%015.5f", -42.9876);
  tdd_assert(!strcmp(buffer, "-00000042.98760"));
}

rx_tddm(fmt,"fmt :: - flag") {
  char buffer[100];

  rx::st::sprintf(buffer, "%-d", 42);
  tdd_assert(!strcmp(buffer, "42"));

  rx::st::sprintf(buffer, "%-d", -42);
  tdd_assert(!strcmp(buffer, "-42"));

  rx::st::sprintf(buffer, "%-5d", 42);
  tdd_assert(!strcmp(buffer, "42   "));

  rx::st::sprintf(buffer, "%-5d", -42);
  tdd_assert(!strcmp(buffer, "-42  "));

  rx::st::sprintf(buffer, "%-15d", 42);
  tdd_assert(!strcmp(buffer, "42             "));

  rx::st::sprintf(buffer, "%-15d", -42);
  tdd_assert(!strcmp(buffer, "-42            "));

  rx::st::sprintf(buffer, "%-0d", 42);
  tdd_assert(!strcmp(buffer, "42"));

  rx::st::sprintf(buffer, "%-0d", -42);
  tdd_assert(!strcmp(buffer, "-42"));

  rx::st::sprintf(buffer, "%-05d", 42);
  tdd_assert(!strcmp(buffer, "42   "));

  rx::st::sprintf(buffer, "%-05d", -42);
  tdd_assert(!strcmp(buffer, "-42  "));

  rx::st::sprintf(buffer, "%-015d", 42);
  tdd_assert(!strcmp(buffer, "42             "));

  rx::st::sprintf(buffer, "%-015d", -42);
  tdd_assert(!strcmp(buffer, "-42            "));

  rx::st::sprintf(buffer, "%0-d", 42);
  tdd_assert(!strcmp(buffer, "42"));

  rx::st::sprintf(buffer, "%0-d", -42);
  tdd_assert(!strcmp(buffer, "-42"));

  rx::st::sprintf(buffer, "%0-5d", 42);
  tdd_assert(!strcmp(buffer, "42   "));

  rx::st::sprintf(buffer, "%0-5d", -42);
  tdd_assert(!strcmp(buffer, "-42  "));

  rx::st::sprintf(buffer, "%0-15d", 42);
  tdd_assert(!strcmp(buffer, "42             "));

  rx::st::sprintf(buffer, "%0-15d", -42);
  tdd_assert(!strcmp(buffer, "-42            "));

  rx::st::sprintf(buffer, "%0-15.3e", -42.);
  tdd_assert(!strcmp(buffer, "-4.200e+01     "));

  rx::st::sprintf(buffer, "%0-15.3g", -42.);
  tdd_assert(!strcmp(buffer, "-42.0          "));
}

rx_tddm(fmt,"fmt :: # flag") {
  char buffer[100];

  rx::st::sprintf(buffer, "%#.0x", 0);
  tdd_assert(!strcmp(buffer, ""));
  rx::st::sprintf(buffer, "%#.1x", 0);
  tdd_assert(!strcmp(buffer, "0"));
  rx::st::sprintf(buffer, "%#.0llx", (long long)0);
  tdd_assert(!strcmp(buffer, ""));
  rx::st::sprintf(buffer, "%#.8x", 0x614e);
  tdd_assert(!strcmp(buffer, "0x0000614e"));
  rx::st::sprintf(buffer,"%#b", 6);
  tdd_assert(!strcmp(buffer, "0b110"));
}

rx_tddm(fmt,"fmt :: specifier") {
  char buffer[100];

  rx::st::sprintf(buffer, "Hello testing");
  tdd_assert(!strcmp(buffer, "Hello testing"));

  rx::st::sprintf(buffer, "%s", "Hello testing");
  tdd_assert(!strcmp(buffer, "Hello testing"));

  rx::st::sprintf(buffer, "%d", 1024);
  tdd_assert(!strcmp(buffer, "1024"));

  rx::st::sprintf(buffer, "%d", -1024);
  tdd_assert(!strcmp(buffer, "-1024"));

  rx::st::sprintf(buffer, "%i", 1024);
  tdd_assert(!strcmp(buffer, "1024"));

  rx::st::sprintf(buffer, "%i", -1024);
  tdd_assert(!strcmp(buffer, "-1024"));

  rx::st::sprintf(buffer, "%u", 1024);
  tdd_assert(!strcmp(buffer, "1024"));

  rx::st::sprintf(buffer, "%u", 4294966272U);
  tdd_assert(!strcmp(buffer, "4294966272"));

  rx::st::sprintf(buffer, "%o", 511);
  tdd_assert(!strcmp(buffer, "777"));

  rx::st::sprintf(buffer, "%o", 4294966785U);
  tdd_assert(!strcmp(buffer, "37777777001"));

  rx::st::sprintf(buffer, "%x", 305441741);
  tdd_assert(!strcmp(buffer, "1234abcd"));

  rx::st::sprintf(buffer, "%x", 3989525555U);
  tdd_assert(!strcmp(buffer, "edcb5433"));

  rx::st::sprintf(buffer, "%X", 305441741);
  tdd_assert(!strcmp(buffer, "1234ABCD"));

  rx::st::sprintf(buffer, "%X", 3989525555U);
  tdd_assert(!strcmp(buffer, "EDCB5433"));

  rx::st::sprintf(buffer, "%%");
  tdd_assert(!strcmp(buffer, "%"));
}

rx_tddm(fmt,"fmt :: width") {
  char buffer[100];

  rx::st::sprintf(buffer, "%1s", "Hello testing");
  tdd_assert(!strcmp(buffer, "Hello testing"));

  rx::st::sprintf(buffer, "%1d", 1024);
  tdd_assert(!strcmp(buffer, "1024"));

  rx::st::sprintf(buffer, "%1d", -1024);
  tdd_assert(!strcmp(buffer, "-1024"));

  rx::st::sprintf(buffer, "%1i", 1024);
  tdd_assert(!strcmp(buffer, "1024"));

  rx::st::sprintf(buffer, "%1i", -1024);
  tdd_assert(!strcmp(buffer, "-1024"));

  rx::st::sprintf(buffer, "%1u", 1024);
  tdd_assert(!strcmp(buffer, "1024"));

  rx::st::sprintf(buffer, "%1u", 4294966272U);
  tdd_assert(!strcmp(buffer, "4294966272"));

  rx::st::sprintf(buffer, "%1o", 511);
  tdd_assert(!strcmp(buffer, "777"));

  rx::st::sprintf(buffer, "%1o", 4294966785U);
  tdd_assert(!strcmp(buffer, "37777777001"));

  rx::st::sprintf(buffer, "%1x", 305441741);
  tdd_assert(!strcmp(buffer, "1234abcd"));

  rx::st::sprintf(buffer, "%1x", 3989525555U);
  tdd_assert(!strcmp(buffer, "edcb5433"));

  rx::st::sprintf(buffer, "%1X", 305441741);
  tdd_assert(!strcmp(buffer, "1234ABCD"));

  rx::st::sprintf(buffer, "%1X", 3989525555U);
  tdd_assert(!strcmp(buffer, "EDCB5433"));

  rx::st::sprintf(buffer, "%1c", 'x');
  tdd_assert(!strcmp(buffer, "x"));
}

rx_tddm(fmt,"fmt :: width 20") {
  char buffer[100];

  rx::st::sprintf(buffer, "%20s", "Hello");
  tdd_assert(!strcmp(buffer, "               Hello"));

  rx::st::sprintf(buffer, "%20d", 1024);
  tdd_assert(!strcmp(buffer, "                1024"));

  rx::st::sprintf(buffer, "%20d", -1024);
  tdd_assert(!strcmp(buffer, "               -1024"));

  rx::st::sprintf(buffer, "%20i", 1024);
  tdd_assert(!strcmp(buffer, "                1024"));

  rx::st::sprintf(buffer, "%20i", -1024);
  tdd_assert(!strcmp(buffer, "               -1024"));

  rx::st::sprintf(buffer, "%20u", 1024);
  tdd_assert(!strcmp(buffer, "                1024"));

  rx::st::sprintf(buffer, "%20u", 4294966272U);
  tdd_assert(!strcmp(buffer, "          4294966272"));

  rx::st::sprintf(buffer, "%20o", 511);
  tdd_assert(!strcmp(buffer, "                 777"));

  rx::st::sprintf(buffer, "%20o", 4294966785U);
  tdd_assert(!strcmp(buffer, "         37777777001"));

  rx::st::sprintf(buffer, "%20x", 305441741);
  tdd_assert(!strcmp(buffer, "            1234abcd"));

  rx::st::sprintf(buffer, "%20x", 3989525555U);
  tdd_assert(!strcmp(buffer, "            edcb5433"));

  rx::st::sprintf(buffer, "%20X", 305441741);
  tdd_assert(!strcmp(buffer, "            1234ABCD"));

  rx::st::sprintf(buffer, "%20X", 3989525555U);
  tdd_assert(!strcmp(buffer, "            EDCB5433"));

  rx::st::sprintf(buffer, "%20c", 'x');
  tdd_assert(!strcmp(buffer, "                   x"));
}

rx_tddm(fmt,"fmt :: width *20") {
  char buffer[100];

  rx::st::sprintf(buffer, "%*s", 20, "Hello");
  tdd_assert(!strcmp(buffer, "               Hello"));

  rx::st::sprintf(buffer, "%*d", 20, 1024);
  tdd_assert(!strcmp(buffer, "                1024"));

  rx::st::sprintf(buffer, "%*d", 20, -1024);
  tdd_assert(!strcmp(buffer, "               -1024"));

  rx::st::sprintf(buffer, "%*i", 20, 1024);
  tdd_assert(!strcmp(buffer, "                1024"));

  rx::st::sprintf(buffer, "%*i", 20, -1024);
  tdd_assert(!strcmp(buffer, "               -1024"));

  rx::st::sprintf(buffer, "%*u", 20, 1024);
  tdd_assert(!strcmp(buffer, "                1024"));

  rx::st::sprintf(buffer, "%*u", 20, 4294966272U);
  tdd_assert(!strcmp(buffer, "          4294966272"));

  rx::st::sprintf(buffer, "%*o", 20, 511);
  tdd_assert(!strcmp(buffer, "                 777"));

  rx::st::sprintf(buffer, "%*o", 20, 4294966785U);
  tdd_assert(!strcmp(buffer, "         37777777001"));

  rx::st::sprintf(buffer, "%*x", 20, 305441741);
  tdd_assert(!strcmp(buffer, "            1234abcd"));

  rx::st::sprintf(buffer, "%*x", 20, 3989525555U);
  tdd_assert(!strcmp(buffer, "            edcb5433"));

  rx::st::sprintf(buffer, "%*X", 20, 305441741);
  tdd_assert(!strcmp(buffer, "            1234ABCD"));

  rx::st::sprintf(buffer, "%*X", 20, 3989525555U);
  tdd_assert(!strcmp(buffer, "            EDCB5433"));

  rx::st::sprintf(buffer, "%*c", 20,'x');
  tdd_assert(!strcmp(buffer, "                   x"));
}

rx_tddm(fmt,"fmt :: width -20") {
  char buffer[100];

  rx::st::sprintf(buffer, "%-20s", "Hello");
  tdd_assert(!strcmp(buffer, "Hello               "));

  rx::st::sprintf(buffer, "%-20d", 1024);
  tdd_assert(!strcmp(buffer, "1024                "));

  rx::st::sprintf(buffer, "%-20d", -1024);
  tdd_assert(!strcmp(buffer, "-1024               "));

  rx::st::sprintf(buffer, "%-20i", 1024);
  tdd_assert(!strcmp(buffer, "1024                "));

  rx::st::sprintf(buffer, "%-20i", -1024);
  tdd_assert(!strcmp(buffer, "-1024               "));

  rx::st::sprintf(buffer, "%-20u", 1024);
  tdd_assert(!strcmp(buffer, "1024                "));

  rx::st::sprintf(buffer, "%-20.4f", 1024.1234);
  tdd_assert(!strcmp(buffer, "1024.1234           "));

  rx::st::sprintf(buffer, "%-20u", 4294966272U);
  tdd_assert(!strcmp(buffer, "4294966272          "));

  rx::st::sprintf(buffer, "%-20o", 511);
  tdd_assert(!strcmp(buffer, "777                 "));

  rx::st::sprintf(buffer, "%-20o", 4294966785U);
  tdd_assert(!strcmp(buffer, "37777777001         "));

  rx::st::sprintf(buffer, "%-20x", 305441741);
  tdd_assert(!strcmp(buffer, "1234abcd            "));

  rx::st::sprintf(buffer, "%-20x", 3989525555U);
  tdd_assert(!strcmp(buffer, "edcb5433            "));

  rx::st::sprintf(buffer, "%-20X", 305441741);
  tdd_assert(!strcmp(buffer, "1234ABCD            "));

  rx::st::sprintf(buffer, "%-20X", 3989525555U);
  tdd_assert(!strcmp(buffer, "EDCB5433            "));

  rx::st::sprintf(buffer, "%-20c", 'x');
  tdd_assert(!strcmp(buffer, "x                   "));

  rx::st::sprintf(buffer, "|%5d| |%-2d| |%5d|", 9, 9, 9);
  tdd_assert(!strcmp(buffer, "|    9| |9 | |    9|"));

  rx::st::sprintf(buffer, "|%5d| |%-2d| |%5d|", 10, 10, 10);
  tdd_assert(!strcmp(buffer, "|   10| |10| |   10|"));

  rx::st::sprintf(buffer, "|%5d| |%-12d| |%5d|", 9, 9, 9);
  tdd_assert(!strcmp(buffer, "|    9| |9           | |    9|"));

  rx::st::sprintf(buffer, "|%5d| |%-12d| |%5d|", 10, 10, 10);
  tdd_assert(!strcmp(buffer, "|   10| |10          | |   10|"));
}

rx_tddm(fmt,"fmt :: width 0-20") {
  char buffer[100];

  rx::st::sprintf(buffer, "%0-20s", "Hello");
  tdd_assert(!strcmp(buffer, "Hello               "));

  rx::st::sprintf(buffer, "%0-20d", 1024);
  tdd_assert(!strcmp(buffer, "1024                "));

  rx::st::sprintf(buffer, "%0-20d", -1024);
  tdd_assert(!strcmp(buffer, "-1024               "));

  rx::st::sprintf(buffer, "%0-20i", 1024);
  tdd_assert(!strcmp(buffer, "1024                "));

  rx::st::sprintf(buffer, "%0-20i", -1024);
  tdd_assert(!strcmp(buffer, "-1024               "));

  rx::st::sprintf(buffer, "%0-20u", 1024);
  tdd_assert(!strcmp(buffer, "1024                "));

  rx::st::sprintf(buffer, "%0-20u", 4294966272U);
  tdd_assert(!strcmp(buffer, "4294966272          "));

  rx::st::sprintf(buffer, "%0-20o", 511);
  tdd_assert(!strcmp(buffer, "777                 "));

  rx::st::sprintf(buffer, "%0-20o", 4294966785U);
  tdd_assert(!strcmp(buffer, "37777777001         "));

  rx::st::sprintf(buffer, "%0-20x", 305441741);
  tdd_assert(!strcmp(buffer, "1234abcd            "));

  rx::st::sprintf(buffer, "%0-20x", 3989525555U);
  tdd_assert(!strcmp(buffer, "edcb5433            "));

  rx::st::sprintf(buffer, "%0-20X", 305441741);
  tdd_assert(!strcmp(buffer, "1234ABCD            "));

  rx::st::sprintf(buffer, "%0-20X", 3989525555U);
  tdd_assert(!strcmp(buffer, "EDCB5433            "));

  rx::st::sprintf(buffer, "%0-20c", 'x');
  tdd_assert(!strcmp(buffer, "x                   "));
}

rx_tddm(fmt,"fmt :: padding 20") {
  char buffer[100];

  rx::st::sprintf(buffer, "%020d", 1024);
  tdd_assert(!strcmp(buffer, "00000000000000001024"));

  rx::st::sprintf(buffer, "%020d", -1024);
  tdd_assert(!strcmp(buffer, "-0000000000000001024"));

  rx::st::sprintf(buffer, "%020i", 1024);
  tdd_assert(!strcmp(buffer, "00000000000000001024"));

  rx::st::sprintf(buffer, "%020i", -1024);
  tdd_assert(!strcmp(buffer, "-0000000000000001024"));

  rx::st::sprintf(buffer, "%020u", 1024);
  tdd_assert(!strcmp(buffer, "00000000000000001024"));

  rx::st::sprintf(buffer, "%020u", 4294966272U);
  tdd_assert(!strcmp(buffer, "00000000004294966272"));

  rx::st::sprintf(buffer, "%020o", 511);
  tdd_assert(!strcmp(buffer, "00000000000000000777"));

  rx::st::sprintf(buffer, "%020o", 4294966785U);
  tdd_assert(!strcmp(buffer, "00000000037777777001"));

  rx::st::sprintf(buffer, "%020x", 305441741);
  tdd_assert(!strcmp(buffer, "0000000000001234abcd"));

  rx::st::sprintf(buffer, "%020x", 3989525555U);
  tdd_assert(!strcmp(buffer, "000000000000edcb5433"));

  rx::st::sprintf(buffer, "%020X", 305441741);
  tdd_assert(!strcmp(buffer, "0000000000001234ABCD"));

  rx::st::sprintf(buffer, "%020X", 3989525555U);
  tdd_assert(!strcmp(buffer, "000000000000EDCB5433"));
}

rx_tddm(fmt,"fmt :: padding .20") {
  char buffer[100];

  rx::st::sprintf(buffer, "%.20d", 1024);
  tdd_assert(!strcmp(buffer, "00000000000000001024"));

  rx::st::sprintf(buffer, "%.20d", -1024);
  tdd_assert(!strcmp(buffer, "-00000000000000001024"));

  rx::st::sprintf(buffer, "%.20i", 1024);
  tdd_assert(!strcmp(buffer, "00000000000000001024"));

  rx::st::sprintf(buffer, "%.20i", -1024);
  tdd_assert(!strcmp(buffer, "-00000000000000001024"));

  rx::st::sprintf(buffer, "%.20u", 1024);
  tdd_assert(!strcmp(buffer, "00000000000000001024"));

  rx::st::sprintf(buffer, "%.20u", 4294966272U);
  tdd_assert(!strcmp(buffer, "00000000004294966272"));

  rx::st::sprintf(buffer, "%.20o", 511);
  tdd_assert(!strcmp(buffer, "00000000000000000777"));

  rx::st::sprintf(buffer, "%.20o", 4294966785U);
  tdd_assert(!strcmp(buffer, "00000000037777777001"));

  rx::st::sprintf(buffer, "%.20x", 305441741);
  tdd_assert(!strcmp(buffer, "0000000000001234abcd"));

  rx::st::sprintf(buffer, "%.20x", 3989525555U);
  tdd_assert(!strcmp(buffer, "000000000000edcb5433"));

  rx::st::sprintf(buffer, "%.20X", 305441741);
  tdd_assert(!strcmp(buffer, "0000000000001234ABCD"));

  rx::st::sprintf(buffer, "%.20X", 3989525555U);
  tdd_assert(!strcmp(buffer, "000000000000EDCB5433"));
}

rx_tddm(fmt,"fmt :: padding #020") {
  char buffer[100];

  rx::st::sprintf(buffer, "%#020d", 1024);
  tdd_assert(!strcmp(buffer, "00000000000000001024"));

  rx::st::sprintf(buffer, "%#020d", -1024);
  tdd_assert(!strcmp(buffer, "-0000000000000001024"));

  rx::st::sprintf(buffer, "%#020i", 1024);
  tdd_assert(!strcmp(buffer, "00000000000000001024"));

  rx::st::sprintf(buffer, "%#020i", -1024);
  tdd_assert(!strcmp(buffer, "-0000000000000001024"));

  rx::st::sprintf(buffer, "%#020u", 1024);
  tdd_assert(!strcmp(buffer, "00000000000000001024"));

  rx::st::sprintf(buffer, "%#020u", 4294966272U);
  tdd_assert(!strcmp(buffer, "00000000004294966272"));

  rx::st::sprintf(buffer, "%#020o", 511);
  tdd_assert(!strcmp(buffer, "00000000000000000777"));

  rx::st::sprintf(buffer, "%#020o", 4294966785U);
  tdd_assert(!strcmp(buffer, "00000000037777777001"));

  rx::st::sprintf(buffer, "%#020x", 305441741);
  tdd_assert(!strcmp(buffer, "0x00000000001234abcd"));

  rx::st::sprintf(buffer, "%#020x", 3989525555U);
  tdd_assert(!strcmp(buffer, "0x0000000000edcb5433"));

  rx::st::sprintf(buffer, "%#020X", 305441741);
  tdd_assert(!strcmp(buffer, "0X00000000001234ABCD"));

  rx::st::sprintf(buffer, "%#020X", 3989525555U);
  tdd_assert(!strcmp(buffer, "0X0000000000EDCB5433"));
}

rx_tddm(fmt,"fmt :: padding #20") {
  char buffer[100];

  rx::st::sprintf(buffer, "%#20d", 1024);
  tdd_assert(!strcmp(buffer, "                1024"));

  rx::st::sprintf(buffer, "%#20d", -1024);
  tdd_assert(!strcmp(buffer, "               -1024"));

  rx::st::sprintf(buffer, "%#20i", 1024);
  tdd_assert(!strcmp(buffer, "                1024"));

  rx::st::sprintf(buffer, "%#20i", -1024);
  tdd_assert(!strcmp(buffer, "               -1024"));

  rx::st::sprintf(buffer, "%#20u", 1024);
  tdd_assert(!strcmp(buffer, "                1024"));

  rx::st::sprintf(buffer, "%#20u", 4294966272U);
  tdd_assert(!strcmp(buffer, "          4294966272"));

  rx::st::sprintf(buffer, "%#20o", 511);
  tdd_assert(!strcmp(buffer, "                0777"));

  rx::st::sprintf(buffer, "%#20o", 4294966785U);
  tdd_assert(!strcmp(buffer, "        037777777001"));

  rx::st::sprintf(buffer, "%#20x", 305441741);
  tdd_assert(!strcmp(buffer, "          0x1234abcd"));

  rx::st::sprintf(buffer, "%#20x", 3989525555U);
  tdd_assert(!strcmp(buffer, "          0xedcb5433"));

  rx::st::sprintf(buffer, "%#20X", 305441741);
  tdd_assert(!strcmp(buffer, "          0X1234ABCD"));

  rx::st::sprintf(buffer, "%#20X", 3989525555U);
  tdd_assert(!strcmp(buffer, "          0XEDCB5433"));
}

rx_tddm(fmt,"fmt :: padding 20.5") {
  char buffer[100];

  rx::st::sprintf(buffer, "%20.5d", 1024);
  tdd_assert(!strcmp(buffer, "               01024"));

  rx::st::sprintf(buffer, "%20.5d", -1024);
  tdd_assert(!strcmp(buffer, "              -01024"));

  rx::st::sprintf(buffer, "%20.5i", 1024);
  tdd_assert(!strcmp(buffer, "               01024"));

  rx::st::sprintf(buffer, "%20.5i", -1024);
  tdd_assert(!strcmp(buffer, "              -01024"));

  rx::st::sprintf(buffer, "%20.5u", 1024);
  tdd_assert(!strcmp(buffer, "               01024"));

  rx::st::sprintf(buffer, "%20.5u", 4294966272U);
  tdd_assert(!strcmp(buffer, "          4294966272"));

  rx::st::sprintf(buffer, "%20.5o", 511);
  tdd_assert(!strcmp(buffer, "               00777"));

  rx::st::sprintf(buffer, "%20.5o", 4294966785U);
  tdd_assert(!strcmp(buffer, "         37777777001"));

  rx::st::sprintf(buffer, "%20.5x", 305441741);
  tdd_assert(!strcmp(buffer, "            1234abcd"));

  rx::st::sprintf(buffer, "%20.10x", 3989525555U);
  tdd_assert(!strcmp(buffer, "          00edcb5433"));

  rx::st::sprintf(buffer, "%20.5X", 305441741);
  tdd_assert(!strcmp(buffer, "            1234ABCD"));

  rx::st::sprintf(buffer, "%20.10X", 3989525555U);
  tdd_assert(!strcmp(buffer, "          00EDCB5433"));
}

rx_tddm(fmt,"fmt :: padding neg numbers") {
  char buffer[100];

  // space padding
  rx::st::sprintf(buffer, "% 1d", -5);
  tdd_assert(!strcmp(buffer, "-5"));

  rx::st::sprintf(buffer, "% 2d", -5);
  tdd_assert(!strcmp(buffer, "-5"));

  rx::st::sprintf(buffer, "% 3d", -5);
  tdd_assert(!strcmp(buffer, " -5"));

  rx::st::sprintf(buffer, "% 4d", -5);
  tdd_assert(!strcmp(buffer, "  -5"));

  // zero padding
  rx::st::sprintf(buffer, "%01d", -5);
  tdd_assert(!strcmp(buffer, "-5"));

  rx::st::sprintf(buffer, "%02d", -5);
  tdd_assert(!strcmp(buffer, "-5"));

  rx::st::sprintf(buffer, "%03d", -5);
  tdd_assert(!strcmp(buffer, "-05"));

  rx::st::sprintf(buffer, "%04d", -5);
  tdd_assert(!strcmp(buffer, "-005"));
}

rx_tddm(fmt,"fmt :: float padding neg numbers") {
  char buffer[100];

  // space padding
  rx::st::sprintf(buffer, "% 3.1f", -5.);
  tdd_assert(!strcmp(buffer, "-5.0"));

  rx::st::sprintf(buffer, "% 4.1f", -5.);
  tdd_assert(!strcmp(buffer, "-5.0"));

  rx::st::sprintf(buffer, "% 5.1f", -5.);
  tdd_assert(!strcmp(buffer, " -5.0"));

  rx::st::sprintf(buffer, "% 6.1g", -5.);
  tdd_assert(!strcmp(buffer, "    -5"));

  rx::st::sprintf(buffer, "% 6.1e", -5.);
  tdd_assert(!strcmp(buffer, "-5.0e+00"));

  rx::st::sprintf(buffer, "% 10.1e", -5.);
  tdd_assert(!strcmp(buffer, "  -5.0e+00"));

  // zero padding
  rx::st::sprintf(buffer, "%03.1f", -5.);
  tdd_assert(!strcmp(buffer, "-5.0"));

  rx::st::sprintf(buffer, "%04.1f", -5.);
  tdd_assert(!strcmp(buffer, "-5.0"));

  rx::st::sprintf(buffer, "%05.1f", -5.);
  tdd_assert(!strcmp(buffer, "-05.0"));

  // zero padding no decimal point
  rx::st::sprintf(buffer, "%01.0f", -5.);
  tdd_assert(!strcmp(buffer, "-5"));

  rx::st::sprintf(buffer, "%02.0f", -5.);
  tdd_assert(!strcmp(buffer, "-5"));

  rx::st::sprintf(buffer, "%03.0f", -5.);
  tdd_assert(!strcmp(buffer, "-05"));

  rx::st::sprintf(buffer, "%010.1e", -5.);
  tdd_assert(!strcmp(buffer, "-005.0e+00"));

  rx::st::sprintf(buffer, "%07.0E", -5.);
  tdd_assert(!strcmp(buffer, "-05E+00"));

  rx::st::sprintf(buffer, "%03.0g", -5.);
  tdd_assert(!strcmp(buffer, "-05"));
}

rx_tddm(fmt,"fmt :: length") {
  char buffer[100];

  rx::st::sprintf(buffer, "%.0s", "Hello testing");
  tdd_assert(!strcmp(buffer, ""));

  rx::st::sprintf(buffer, "%20.0s", "Hello testing");
  tdd_assert(!strcmp(buffer, "                    "));

  rx::st::sprintf(buffer, "%.s", "Hello testing");
  tdd_assert(!strcmp(buffer, ""));

  rx::st::sprintf(buffer, "%20.s", "Hello testing");
  tdd_assert(!strcmp(buffer, "                    "));

  rx::st::sprintf(buffer, "%20.0d", 1024);
  tdd_assert(!strcmp(buffer, "                1024"));

  rx::st::sprintf(buffer, "%20.0d", -1024);
  tdd_assert(!strcmp(buffer, "               -1024"));

  rx::st::sprintf(buffer, "%20.d", 0);
  tdd_assert(!strcmp(buffer, "                    "));

  rx::st::sprintf(buffer, "%20.0i", 1024);
  tdd_assert(!strcmp(buffer, "                1024"));

  rx::st::sprintf(buffer, "%20.i", -1024);
  tdd_assert(!strcmp(buffer, "               -1024"));

  rx::st::sprintf(buffer, "%20.i", 0);
  tdd_assert(!strcmp(buffer, "                    "));

  rx::st::sprintf(buffer, "%20.u", 1024);
  tdd_assert(!strcmp(buffer, "                1024"));

  rx::st::sprintf(buffer, "%20.0u", 4294966272U);
  tdd_assert(!strcmp(buffer, "          4294966272"));

  rx::st::sprintf(buffer, "%20.u", 0U);
  tdd_assert(!strcmp(buffer, "                    "));

  rx::st::sprintf(buffer, "%20.o", 511);
  tdd_assert(!strcmp(buffer, "                 777"));

  rx::st::sprintf(buffer, "%20.0o", 4294966785U);
  tdd_assert(!strcmp(buffer, "         37777777001"));

  rx::st::sprintf(buffer, "%20.o", 0U);
  tdd_assert(!strcmp(buffer, "                    "));

  rx::st::sprintf(buffer, "%20.x", 305441741);
  tdd_assert(!strcmp(buffer, "            1234abcd"));

  rx::st::sprintf(buffer, "%50.x", 305441741);
  tdd_assert(!strcmp(buffer, "                                          1234abcd"));

  rx::st::sprintf(buffer, "%50.x%10.u", 305441741, 12345);
  tdd_assert(!strcmp(buffer, "                                          1234abcd     12345"));

  rx::st::sprintf(buffer, "%20.0x", 3989525555U);
  tdd_assert(!strcmp(buffer, "            edcb5433"));

  rx::st::sprintf(buffer, "%20.x", 0U);
  tdd_assert(!strcmp(buffer, "                    "));

  rx::st::sprintf(buffer, "%20.X", 305441741);
  tdd_assert(!strcmp(buffer, "            1234ABCD"));

  rx::st::sprintf(buffer, "%20.0X", 3989525555U);
  tdd_assert(!strcmp(buffer, "            EDCB5433"));

  rx::st::sprintf(buffer, "%20.X", 0U);
  tdd_assert(!strcmp(buffer, "                    "));

  rx::st::sprintf(buffer, "%02.0u", 0U);
  tdd_assert(!strcmp(buffer, "  "));

  rx::st::sprintf(buffer, "%02.0d", 0);
  tdd_assert(!strcmp(buffer, "  "));
}

//比较1.234e+020和1.234e+20应该相同
inline bool cmp_float_expstr(const char* m,const char* s)
{
    const char* e=strchr(s,'e');
    if (e==NULL)
        return false;

    int fp_len=(int)(e-s)+1;
    if (strncmp(m,s,fp_len))
        return false;

    m+=fp_len+1;
    s+=fp_len+1;
    if (*s=='0') ++s;

    return strcmp(m,s)==0;
}

rx_tddm(fmt,"fmt :: float") {
  char buffer[100];

#if 0
  // test special-case floats using math.h macros
  rx::st::sprintf(buffer, "%8f", NAN);
  tdd_assert(!strcmp(buffer, "     nan"));

  rx::st::sprintf(buffer, "%8f", INFINITY);
  tdd_assert(!strcmp(buffer, "     inf"));

  rx::st::sprintf(buffer, "%-8f", -INFINITY);
  tdd_assert(!strcmp(buffer, "-inf    "));

  rx::st::sprintf(buffer, "%+8e", INFINITY);
  tdd_assert(!strcmp(buffer, "    +inf"));
#endif

  rx::st::sprintf(buffer, "%.4f", 3.1415354);
  tdd_assert(!strcmp(buffer, "3.1415"));

  rx::st::sprintf(buffer, "%.3f", 30343.1415354);
  tdd_assert(!strcmp(buffer, "30343.142"));

  rx::st::sprintf(buffer, "%.0f", 34.1415354);
  tdd_assert(!strcmp(buffer, "34"));

  rx::st::sprintf(buffer, "%.0f", 1.3);
  tdd_assert(!strcmp(buffer, "1"));

  rx::st::sprintf(buffer, "%.0f", 1.55);
  tdd_assert(!strcmp(buffer, "2"));

  rx::st::sprintf(buffer, "%.1f", 1.64);
  tdd_assert(!strcmp(buffer, "1.6"));

  rx::st::sprintf(buffer, "%.2f", 42.8952);
  tdd_assert(!strcmp(buffer, "42.90"));

  rx::st::sprintf(buffer, "%.9f", 42.8952);
  tdd_assert(!strcmp(buffer, "42.895200000"));

  rx::st::sprintf(buffer, "%.10f", 42.895223);
  tdd_assert(!strcmp(buffer, "42.8952230000"));

  // this testcase checks, that the precision is truncated to 9 digits.
  // a perfect working float should return the whole number
  rx::st::sprintf(buffer, "%.12f", 42.89522312345678);
  tdd_assert(!strcmp(buffer, "42.895223123000"));

  // this testcase checks, that the precision is truncated AND rounded to 9 digits.
  // a perfect working float should return the whole number
  rx::st::sprintf(buffer, "%.12f", 42.89522387654321);
  tdd_assert(!strcmp(buffer, "42.895223877000"));

  rx::st::sprintf(buffer, "%6.2f", 42.8952);
  tdd_assert(!strcmp(buffer, " 42.90"));

  rx::st::sprintf(buffer, "%+6.2f", 42.8952);
  tdd_assert(!strcmp(buffer, "+42.90"));

  rx::st::sprintf(buffer, "%+5.1f", 42.9252);
  tdd_assert(!strcmp(buffer, "+42.9"));

  rx::st::sprintf(buffer, "%f", 42.5);
  tdd_assert(!strcmp(buffer, "42.500000"));

  rx::st::sprintf(buffer, "%.1f", 42.5);
  tdd_assert(!strcmp(buffer, "42.5"));

  rx::st::sprintf(buffer, "%f", 42167.0);
  tdd_assert(!strcmp(buffer, "42167.000000"));

  rx::st::sprintf(buffer, "%.9f", -12345.987654321);
  tdd_assert(!strcmp(buffer, "-12345.987654321"));

  rx::st::sprintf(buffer, "%.1f", 3.999);
  tdd_assert(!strcmp(buffer, "4.0"));

  rx::st::sprintf(buffer, "%.0f", 3.5);
  tdd_assert(!strcmp(buffer, "4"));

  rx::st::sprintf(buffer, "%.0f", 4.5);
  tdd_assert(!strcmp(buffer, "4"));

  rx::st::sprintf(buffer, "%.0f", 3.49);
  tdd_assert(!strcmp(buffer, "3"));

  rx::st::sprintf(buffer, "%.1f", 3.49);
  tdd_assert(!strcmp(buffer, "3.5"));

  rx::st::sprintf(buffer, "a%-5.1f", 0.5);
  tdd_assert(!strcmp(buffer, "a0.5  "));

  rx::st::sprintf(buffer, "a%-5.1fend", 0.5);
  tdd_assert(!strcmp(buffer, "a0.5  end"));

  rx::st::sprintf(buffer, "%G", 12345.678);
  tdd_assert(!strcmp(buffer, "12345.7"));

  rx::st::sprintf(buffer, "%.7G", 12345.678);
  tdd_assert(!strcmp(buffer, "12345.68"));

  rx::st::sprintf(buffer, "%.5G", 123456789.);
  tdd_assert(!strcmp(buffer, "1.2346E+08"));

  rx::st::sprintf(buffer, "%.6G", 12345.);
  tdd_assert(!strcmp(buffer, "12345.0"));

  rx::st::sprintf(buffer, "%+12.4g", 123456789.);
  tdd_assert(!strcmp(buffer, "  +1.235e+08"));

  rx::st::sprintf(buffer, "%.2G", 0.001234);
  tdd_assert(!strcmp(buffer, "0.0012"));

  rx::st::sprintf(buffer, "%+10.4G", 0.001234);
  tdd_assert(!strcmp(buffer, " +0.001234"));

  rx::st::sprintf(buffer, "%+012.4g", 0.00001234);
  tdd_assert(!strcmp(buffer, "+001.234e-05"));

  rx::st::sprintf(buffer, "%.3g", -1.2345e-308);
  tdd_assert(!strcmp(buffer, "-1.23e-308"));

  rx::st::sprintf(buffer, "%+.3E", 1.23e+308);
  tdd_assert(!strcmp(buffer, "+1.230E+308"));

  // out of range for float: should switch to exp notation if supported, else empty
  rx::st::sprintf(buffer, "%.1f", 1E20);
  tdd_assert(!strcmp(buffer, "1.0e+20"));

  // brute force float
  bool fail = false;
  std::stringstream str;
  str.precision(5);
  for (float i = -100000; i < 100000; i += 11) {
    rx::st::sprintf(buffer, "%.5f", i / 10000);
    str.str("");
    str << std::fixed << i / 10000;
    fail = fail || !!strcmp(buffer, str.str().c_str());
  }
  tdd_assert(!fail);

#if 0
  fail = false;
  char tmp[100];
  for (double i = -1e20; i < 1e20; i += 1e15) {
    rx::st::sprintf(buffer, "%.5f", i);
    sprintf(tmp, "%.5e", i);
    bool f=!cmp_float_expstr(buffer, tmp);
    fail = fail || f;
    if (f)
        printf("%s != %s\n",buffer,tmp);
  }
  tdd_assert(!fail);
#endif
}

rx_tddm(fmt,"fmt :: types") {
  char buffer[100];

  rx::st::sprintf(buffer, "%i", 0);
  tdd_assert(!strcmp(buffer, "0"));

  rx::st::sprintf(buffer, "%i", 1234);
  tdd_assert(!strcmp(buffer, "1234"));

  rx::st::sprintf(buffer, "%i", 32767);
  tdd_assert(!strcmp(buffer, "32767"));

  rx::st::sprintf(buffer, "%i", -32767);
  tdd_assert(!strcmp(buffer, "-32767"));

  rx::st::sprintf(buffer, "%li", 30L);
  tdd_assert(!strcmp(buffer, "30"));

  rx::st::sprintf(buffer, "%li", -2147483647L);
  tdd_assert(!strcmp(buffer, "-2147483647"));

  rx::st::sprintf(buffer, "%li", 2147483647L);
  tdd_assert(!strcmp(buffer, "2147483647"));

  rx::st::sprintf(buffer, "%lli", 30LL);
  tdd_assert(!strcmp(buffer, "30"));

  rx::st::sprintf(buffer, "%lli", -9223372036854775807LL);
  tdd_assert(!strcmp(buffer, "-9223372036854775807"));

  rx::st::sprintf(buffer, "%lli", 9223372036854775807LL);
  tdd_assert(!strcmp(buffer, "9223372036854775807"));

  rx::st::sprintf(buffer, "%lu", 100000L);
  tdd_assert(!strcmp(buffer, "100000"));

  rx::st::sprintf(buffer, "%lu", 0xFFFFFFFFL);
  tdd_assert(!strcmp(buffer, "4294967295"));

  rx::st::sprintf(buffer, "%llu", 281474976710656);
  tdd_assert(!strcmp(buffer, "281474976710656"));

  rx::st::sprintf(buffer, "%llu", 18446744073709551615ull);
  tdd_assert(!strcmp(buffer, "18446744073709551615"));

  rx::st::sprintf(buffer, "%zu", 2147483647UL);
  tdd_assert(!strcmp(buffer, "2147483647"));

  rx::st::sprintf(buffer, "%zd", 2147483647UL);
  tdd_assert(!strcmp(buffer, "2147483647"));

  if (sizeof(size_t) == sizeof(long)) {
    rx::st::sprintf(buffer, "%zi", -2147483647L);
    tdd_assert(!strcmp(buffer, "-2147483647"));
  }
  else {
    rx::st::sprintf(buffer, "%zi", -2147483647LL);
    tdd_assert(!strcmp(buffer, "-2147483647"));
  }

  rx::st::sprintf(buffer, "%b", 60000);
  tdd_assert(!strcmp(buffer, "1110101001100000"));

  rx::st::sprintf(buffer, "%lb", 12345678L);
  tdd_assert(!strcmp(buffer, "101111000110000101001110"));

  rx::st::sprintf(buffer, "%o", 60000);
  tdd_assert(!strcmp(buffer, "165140"));

  rx::st::sprintf(buffer, "%lo", 12345678L);
  tdd_assert(!strcmp(buffer, "57060516"));

  rx::st::sprintf(buffer, "%lx", 0x12345678L);
  tdd_assert(!strcmp(buffer, "12345678"));

  rx::st::sprintf(buffer, "%llx", 0x1234567891234567);
  tdd_assert(!strcmp(buffer, "1234567891234567"));

  rx::st::sprintf(buffer, "%lx", 0xabcdefabL);
  tdd_assert(!strcmp(buffer, "abcdefab"));

  rx::st::sprintf(buffer, "%lX", 0xabcdefabL);
  tdd_assert(!strcmp(buffer, "ABCDEFAB"));

  rx::st::sprintf(buffer, "%c", 'v');
  tdd_assert(!strcmp(buffer, "v"));

  rx::st::sprintf(buffer, "%cv", 'w');
  tdd_assert(!strcmp(buffer, "wv"));

  rx::st::sprintf(buffer, "%s", "A Test");
  tdd_assert(!strcmp(buffer, "A Test"));

  rx::st::sprintf(buffer, "%hhu", 0xFFFFUL);
  tdd_assert(!strcmp(buffer, "255"));

  rx::st::sprintf(buffer, "%hu", 0x123456UL);
  tdd_assert(!strcmp(buffer, "13398"));

  rx::st::sprintf(buffer, "%s%hhi %hu", "Test", 10000, 0xFFFFFFFF);
  tdd_assert(!strcmp(buffer, "Test16 65535"));

  rx::st::sprintf(buffer, "%tx", &buffer[10] - &buffer[0]);
  tdd_assert(!strcmp(buffer, "a"));

// TBD
  if (sizeof(intmax_t) == sizeof(long)) {
    rx::st::sprintf(buffer, "%ji", -2147483647L);
    tdd_assert(!strcmp(buffer, "-2147483647"));
  }
  else {
    rx::st::sprintf(buffer, "%ji", -2147483647LL);
    tdd_assert(!strcmp(buffer, "-2147483647"));
  }
}

rx_tddm(fmt,"fmt :: pointer") {
  char buffer[100];

  rx::st::sprintf(buffer, "%p", (void*)0x1234U);
  if (sizeof(void*) == 4U) {
    tdd_assert(!strcmp(buffer, "00001234"));
  }
  else {
    tdd_assert(!strcmp(buffer, "0000000000001234"));
  }

  rx::st::sprintf(buffer, "%p", (void*)0x12345678U);
  if (sizeof(void*) == 4U) {
    tdd_assert(!strcmp(buffer, "12345678"));
  }
  else {
    tdd_assert(!strcmp(buffer, "0000000012345678"));
  }

  rx::st::sprintf(buffer, "%p-%p", (void*)0x12345678U, (void*)0x7EDCBA98U);
  if (sizeof(void*) == 4U) {
    tdd_assert(!strcmp(buffer, "12345678-7EDCBA98"));
  }
  else {
    tdd_assert(!strcmp(buffer, "0000000012345678-000000007EDCBA98"));
  }

  if (sizeof(uintptr_t) == sizeof(uint64_t)) {
    rx::st::sprintf(buffer, "%p", (void*)(uintptr_t)0xFFFFFFFFU);
    tdd_assert(!strcmp(buffer, "00000000FFFFFFFF"));
  }
  else {
    rx::st::sprintf(buffer, "%p", (void*)(uintptr_t)0xFFFFFFFFU);
    tdd_assert(!strcmp(buffer, "FFFFFFFF"));
  }
}

rx_tddm(fmt,"fmt :: unknown flag") {
  char buffer[100];

  rx::st::sprintf(buffer, "%kmarco", 42, 37);
  tdd_assert(!strcmp(buffer, "kmarco"));
}

rx_tddm(fmt,"fmt :: string length") {
  char buffer[100];

  rx::st::sprintf(buffer, "%.4s", "This is a test");
  tdd_assert(!strcmp(buffer, "This"));

  rx::st::sprintf(buffer, "%.4s", "test");
  tdd_assert(!strcmp(buffer, "test"));

  rx::st::sprintf(buffer, "%.7s", "123");
  tdd_assert(!strcmp(buffer, "123"));

  rx::st::sprintf(buffer, "%.7s", "");
  tdd_assert(!strcmp(buffer, ""));

  rx::st::sprintf(buffer, "%.4s%.2s", "123456", "abcdef");
  tdd_assert(!strcmp(buffer, "1234ab"));

  rx::st::sprintf(buffer, "%.4.2s", "123456");
  tdd_assert(!strcmp(buffer, ".2s"));

  rx::st::sprintf(buffer, "%.*s", 3, "123456");
  tdd_assert(!strcmp(buffer, "123"));
}

rx_tddm(fmt,"fmt :: buffer length") {
  char buffer[100];
  int ret;

  ret = rx::st::snprintf((char*)nullptr, 10, "%s", "Test");
  tdd_assert(ret == 4);
  ret = rx::st::snprintf((char*)nullptr, 0, "%s", "Test");
  tdd_assert(ret == 4);

  buffer[0] = (char)0xA5;
  ret = rx::st::snprintf(buffer, 0, "%s", "Test");
  tdd_assert(buffer[0] == (char)0xA5);
  tdd_assert(ret == 4);

  buffer[0] = (char)0xCC;
  rx::st::snprintf(buffer, 1, "%s", "Test");
  tdd_assert(buffer[0] == '\0');

  rx::st::snprintf(buffer, 2, "%s", "Hello");
  tdd_assert(!strcmp(buffer, "H"));
}

rx_tddm(fmt,"fmt :: ret value") {
  char buffer[100] ;
  int ret;

  ret = rx::st::snprintf(buffer, 6, "0%s", "1234");
  tdd_assert(!strcmp(buffer, "01234"));
  tdd_assert(ret == 5);

  ret = rx::st::snprintf(buffer, 6, "0%s", "12345");
  tdd_assert(!strcmp(buffer, "01234"));
  tdd_assert(ret == 6);  // '5' is truncated

  ret = rx::st::snprintf(buffer, 6, "0%s", "1234567");
  tdd_assert(!strcmp(buffer, "01234"));
  tdd_assert(ret == 8);  // '567' are truncated

  ret = rx::st::snprintf(buffer, 10, "hello, world");
  tdd_assert(ret == 12);

  ret = rx::st::snprintf(buffer, 3, "%d", 10000);
  tdd_assert(ret == 5);
  tdd_assert(strlen(buffer) == 2U);
  tdd_assert(buffer[0] == '1');
  tdd_assert(buffer[1] == '0');
  tdd_assert(buffer[2] == '\0');
}

rx_tddm(fmt,"fmt :: misc") {
  char buffer[100];

  rx::st::sprintf(buffer, "%u%u%ctest%d %s", 5, 3000, 'a', -20, "bit");
  tdd_assert(!strcmp(buffer, "53000atest-20 bit"));

  rx::st::sprintf(buffer, "%.*f", 2, 0.33333333);
  tdd_assert(!strcmp(buffer, "0.33"));

  rx::st::sprintf(buffer, "%.*d", -1, 1);
  tdd_assert(!strcmp(buffer, "1"));

  rx::st::sprintf(buffer, "%.3s", "foobar");
  tdd_assert(!strcmp(buffer, "foo"));

  rx::st::sprintf(buffer, "% .0d", 0);
  tdd_assert(!strcmp(buffer, " "));

  rx::st::sprintf(buffer, "%10.5d", 4);
  tdd_assert(!strcmp(buffer, "     00004"));

  rx::st::sprintf(buffer, "%*sx", -3, "hi");
  tdd_assert(!strcmp(buffer, "hi x"));

  rx::st::sprintf(buffer, "%.*g", 2, 0.33333333);
  tdd_assert(!strcmp(buffer, "0.33"));

  rx::st::sprintf(buffer, "%.*e", 2, 0.33333333);
  tdd_assert(!strcmp(buffer, "3.33e-01"));
}
