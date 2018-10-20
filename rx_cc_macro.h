#ifndef _rx_cc_macro_
#define _rx_cc_macro_

    //-----------------------------------------------------
    //是否禁用内联前缀
    #ifndef RX_BAN_INLINE
        #define RX_BAN_INLINE               0
    #endif

    #if RX_BAN_INLINE
        #define RX_INLINE
    #endif

    //-----------------------------------------------------
    //定义常用编译器类型
    #define RX_CC_UNKNOWN                   0               //Unknown compiler
    #define RX_CC_TC                        1               //Borland Turbo C
    #define RX_CC_BCC                       2               //Borland C++
    #define RX_CC_GCC                       3               //Standard GCC/G++
    #define RX_CC_VC                        4               //Microsoft Visual Studio
    #define RX_CC_CLANG                     5               //CLang (LLVM) compiler
    #define RX_CC_CYGWIN                    6               //Cygwin (x86)
    #define RX_CC_MINGW32                   7               //MingW32 (x86) compiler for windows
    #define RX_CC_MINGW64                   8               //MingW64 (x64) compiler for windows
    #define RX_CC_LCC                       9               //LCC
    #define RX_CC_TINYC                     10              //Tiny C
    #define RX_CC_ULTIMATE                  11              //Ultimate C/C++
    #define RX_CC_ARMCC                     12              //ARM Compiler
    #define RX_CC_IAR                       13              //IAR C/C++
    #define RX_CC_ICC                       14              //Intel ICC/ICPC Compiler
    #define RX_CC_KEIL                      15              //Keil (uVision)

    //-----------------------------------------------------
    //进行编译器类型的自动侦测
    #ifndef RX_CC
        #define RX_CC                       RX_CC_UNKNOWN
    #endif

    #if RX_CC == RX_CC_UNKNOWN
        #undef RX_CC
        #if defined(__MINGW32__)
            #define RX_CC                   RX_CC_MINGW32
        #elif defined(__MINGW64__)
            #define RX_CC                   RX_CC_MINGW64
        #elif defined(__CYGWIN__)
            #define RX_CC                   RX_CC_CYGWIN
        #elif defined(__KEIL__) || (defined(__CC_ARM) && defined(__EDG__))
            #define RX_CC                   RX_CC_KEIL
        #elif defined(__clang__) || defined(__llvm__)
            #define RX_CC                   RX_CC_CLANG
        #elif defined(__INTEL_COMPILER) || defined(__ICC) || defined(__ECC) || defined(__TCL)
            #define RX_CC                   RX_CC_ICC
        #elif defined(__GNUC__) || defined(__GNUG__)
            #define RX_CC                   RX_CC_GCC
        #elif defined(__CC_ARM)
            #define RX_CC                   RX_CC_ARMCC
        #elif defined(_MSC_VER)
            #define RX_CC                   RX_CC_VC
        #elif defined(__TURBOC__)
            #define RX_CC                   RX_CC_TC
        #elif defined(__BORLANDC__)
            #define RX_CC                   RX_CC_BCC
        #elif defined(__LCC__)
            #define RX_CC                   RX_CC_LCC
        #elif defined(__TINYC__)
            #define RX_CC                   RX_CC_TINYC
        #elif defined(__IAR_SYSTEMS_ICC__)
            #define RX_CC                   RX_CC_IAR
        #elif defined(_UCC)
            #define RX_CC                   RX_CC_ULTIMATE

        #else
            #define RX_CC                   RX_CC_UNKNOWN
        #endif
    #endif

    //-----------------------------------------------------
    //自动侦测编译器的版本信息
    #undef RX_CC_NAME
    #undef RX_CC_TESTED
    #undef RX_CC_VER_MAJOR
    #undef RX_CC_VER_MINOR
    #undef RX_CC_VER_PATCH
    #undef RX_CC_VER_BUILD
    #if RX_CC == RX_CC_ARMCC
        #define RX_CC_NAME                  "ARMCC"
        #define RX_CC_VER_MAJOR             (__ARMCC_VERSION)/100000
        #define RX_CC_VER_MINOR             ((__ARMCC_VERSION)/10000)%10
        #define RX_CC_VER_PATCH             ((__ARMCC_VERSION)/1000)%10
        #define RX_CC_VER_BUILD             (__ARMCC_VERSION)%1000
        #define __LITTLE_IF_NOT_BIG__       //Defines __BIG_ENDIAN but not __LITTLE_ENDIAN
    #elif RX_CC == RX_CC_BCC
        #define RX_CC_NAME                  "Borland C++"
        #define RX_CC_VER_MAJOR             (__BORLANDC__)/0x100
        #define RX_CC_VER_MINOR             ((((__BORLANDC__)%0x100)/0x10)*10) + ((__BORLANDC__)%0x10)
    #elif RX_CC == RX_CC_CLANG
        #define RX_CC_NAME                  "CLANG (LLVM)"
        #define RX_CC_VER_MAJOR             __clang_major__
        #define RX_CC_VER_MINOR             __clang_minor__
        #define RX_CC_VER_PATCH             __clang_patchlevel__
        #define DEPRECATED(msg)             __attribute__((deprecated(msg)))
    #elif RX_CC == RX_CC_CYGWIN
        #define RX_CC_NAME                  "Cygwin"
        #define RX_CC_VER_MAJOR             __GNUC__
        #define RX_CC_VER_MINOR             __GNUC_MINOR__
        #ifdef __GNUC_PATCHLEVEL__
            #define RX_CC_VER_PATCH         __GNUC_PATCHLEVEL__
        #endif
        #define DEPRECATED(msg)             __attribute__((deprecated(msg)))
    #elif RX_CC == RX_CC_GCC
        #define RX_CC_NAME                  "GCC"
        #define RX_CC_VER_MAJOR             __GNUC__
        #define RX_CC_VER_MINOR             __GNUC_MINOR__
        #ifdef __GNUC_PATCHLEVEL__
            #define RX_CC_VER_PATCH         __GNUC_PATCHLEVEL__
        #endif
        #define DEPRECATED(msg)             __attribute__((deprecated(msg)))
    #elif RX_CC == RX_CC_IAR
        #define RX_CC_NAME                  "IAR C++"
        #define RX_CC_VER_MAJOR             (__VER__)/100
        #define RX_CC_VER_MINOR             (__VER__)%100
        #pragma diag_remark = Pe301
        #pragma diag_remark = Pe083
        #pragma diag_remark = Pe767
        #pragma diag_remark = Pe188
        #pragma diag_remark = Pe186
        #pragma diag_remark = Pe068
        #pragma diag_remark = Pa050
    #elif RX_CC == RX_CC_ICC
        #define RX_CC_NAME                  "Intel ICC"
        #ifdef __INTEL_COMPILER
            #define RX_CC_VER_MAJOR         (__INTEL_COMPILER)/100
            #define RX_CC_VER_MINOR         ((__INTEL_COMPILER)/10)%10
            #define RX_CC_VER_PATCH         (__INTEL_COMPILER)%10
            #ifdef __INTEL_COMPILER_BUILD_DATE
                #define RX_CC_VER_BUILD     __INTEL_COMPILER_BUILD_DATE
            #endif
        #endif
    #elif RX_CC == RX_CC_KEIL
        #define RX_CC_NAME                  "Keil"
        #if defined(__ARMCC_VERSION)
            #define RX_CC_VER_MAJOR         (__ARMCC_VERSION)/1000000
            #define RX_CC_VER_MINOR         ((__ARMCC_VERSION)/10000)%100
            #define RX_CC_VER_PATCH         (__ARMCC_VERSION)%10000
            #ifdef __EDG_VERSION__
                #define RX_CC_VER_BUILD     __EDG_VERSION__
            #endif
        #elif defined(__CA__)
            #define RX_CC_VER_MAJOR         (__CA__)/100
            #define RX_CC_VER_MINOR         (__CA__)%100
        #elif defined(__C166__)
            #define RX_CC_VER_MAJOR         (__C166__)/100
            #define RX_CC_VER_MINOR         (__C166__)%100
        #elif defined(__C51__)
            #define RX_CC_VER_MAJOR         (__C51__)/100
            #define RX_CC_VER_MINOR         (__C51__)%100
        #elif defined(__C251__)
            #define RX_CC_VER_MAJOR         (__C251__)/100
            #define RX_CC_VER_MINOR         (__C251__)%100
        #endif
        #define DEPRECATED(msg)             __attribute__((deprecated(msg)))
        #pragma anon_unions                 // Allow anonymous unions
        #pragma diag_remark 1293            // Turn off warning: assignment in condition
        #pragma diag_remark 83              // Turn off warning: type qualifier specified more than once
        #pragma diag_remark 767             // Turn off warning: conversion from pointer to smaller integer
        #pragma diag_remark 188             // Turn off warning: enumerated type mixed with another type
        #ifndef RX_INLINE                   // Get the Keil definition for inline
            #define RX_INLINE               __inline
        #endif
        #define __LITTLE_IF_NOT_BIG__       //Defines __BIG_ENDIAN but not __LITTLE_ENDIAN
    #elif RX_CC == RX_CC_LCC
        #define RX_CC_NAME                  "LCC"
    #elif RX_CC == RX_CC_MINGW32
        #define RX_CC_NAME                  "MingW32"
        #define RX_CC_VER_MAJOR             __GNUC__
        #define RX_CC_VER_MINOR             __GNUC_MINOR__
        #ifdef __GNUC_PATCHLEVEL__
            #define RX_CC_VER_PATCH         __GNUC_PATCHLEVEL__
        #endif
        #define DEPRECATED(msg)             __attribute__((deprecated(msg)))
    #elif RX_CC == RX_CC_MINGW64
        #define RX_CC_NAME                  "MingW64"
        #define RX_CC_VER_MAJOR             __GNUC__
        #define RX_CC_VER_MINOR             __GNUC_MINOR__
        #ifdef __GNUC_PATCHLEVEL__
            #define RX_CC_VER_PATCH         __GNUC_PATCHLEVEL__
        #endif
        #define DEPRECATED(msg)             __attribute__((deprecated(msg)))
    #elif RX_CC == RX_CC_TINYC
        #define RX_CC_NAME                  "Tiny C"
    #elif RX_CC == RX_CC_TC
        #define RX_CC_NAME                  "Borland Turbo C/C++"
        #if __TURBOC__ < 0x295 || __TURBOC__ >= 0x400
            #define RX_CC_VER_MAJOR         (__TURBOC__)/0x100
            #define RX_CC_VER_MINOR         (__TURBOC__)%0x100
        #elif __TURBOC__ == 0x295
            #define RX_CC_VER_MAJOR         1
            #define RX_CC_VER_MINOR         0
        #elif __TURBOC__ == 0x296
            #define RX_CC_VER_MAJOR         1
            #define RX_CC_VER_MINOR         1
        #elif __TURBOC__ == 0x297
            #define RX_CC_VER_MAJOR         2
            #define RX_CC_VER_MINOR         0
        #endif
    #elif RX_CC == RX_CC_ULTIMATE
        #define RX_CC_NAME                  "Ultimate C/C++"
        #define RX_CC_VER_MAJOR             _MAJOR_REV
        #define RX_CC_VER_MINOR             _MINOR_REV
    #elif RX_CC == RX_CC_VC
        #define RX_CC_NAME                  "Microsoft Visual Studio"
        #ifdef _MSC_FULL_VER
            #if _MSC_FULL_VER < 100000000
                #define RX_CC_VER_MAJOR     (_MSC_FULL_VER)/1000000
                #define RX_CC_VER_MINOR     ((_MSC_FULL_VER)/10000)%100
                #define RX_CC_VER_PATCH     ((_MSC_FULL_VER)/10000)%10000
            #else
                #define RX_CC_VER_MAJOR     (_MSC_FULL_VER)/10000000
                #define RX_CC_VER_MINOR     ((_MSC_FULL_VER)/100000)%100
                #define RX_CC_VER_PATCH     ((_MSC_FULL_VER)/100000)%10000
            #endif
        #else
            #define RX_CC_VER_MAJOR         (_MSC_VER)/100
            #define RX_CC_VER_MINOR         (_MSC_VER)%100
        #endif
        #ifdef _MSC_BUILD
            #define RX_CC_VER_BUILD         _MSC_BUILD
        #endif
        #define DEPRECATED(msg)             __declspec(deprecated(msg))
    #endif

    #ifndef RX_CC_TESTED
        #define RX_CC_TESTED                0
    #endif

    #ifndef RX_CC_NAME
        #define RX_CC_NAME                  "Unknown"
        #warning "unknown compiler."
    #endif

    #ifndef RX_CC_VER_MAJOR
        #define RX_CC_VER_MAJOR             0
    #endif

    #ifndef RX_CC_VER_MINOR
        #define RX_CC_VER_MINOR             0
    #endif

    #ifndef RX_CC_VER_PATCH
        #define RX_CC_VER_PATCH             0
    #endif

    #ifndef RX_CC_VER_BUILD
        #define RX_CC_VER_BUILD             0
    #endif

    #if !RX_CC_TESTED
    #endif

    #if RX_CC==RX_CC_MINGW32||RX_CC==RX_CC_MINGW64
        #define RX_CC_MINGW 1
    #endif
    //-----------------------------------------------------
    //定义常用CPU的类型
    #ifndef RX_CPU
        #define RX_CPU                      RX_CPU_UNKNOWN
    #endif
    #define RX_CPU_UNKNOWN                  0       //Unknown cpu
    #define RX_CPU_CORTEX_Mx                0x01    //Cortex Mx
    #define RX_CPU_X86                      0x10    //Intel x86
    #define RX_CPU_X64                      0x11    //Intel x64
    #define RX_CPU_IA64                     0x12    //Intel Itanium
    #define RX_CPU_POWERPC32                0x20    //PowerPC
    #define RX_CPU_POWERPC64                0x21    //PowerPC
    #define RX_CPU_SPARC                    0x22    //Sparc

    //自动侦测当前编译器使用的后端CPU类型(类型标识/架构名称/运算位长)
    #if RX_CPU == RX_CPU_UNKNOWN
        #undef RX_CPU
        #if defined(__ia64) || defined(__itanium__) || defined(_M_IA64)
            #define RX_CPU                  RX_CPU_IA64
            #define RX_CPU_ARCH             "IA64"
            #define RX_CC_BIT               64
        #elif defined(__powerpc__) || defined(__ppc__) || defined(__PPC__)
            #if defined(__powerpc64__) || defined(__ppc64__) || defined(__PPC64__) || defined(__64BIT__) || defined(_LP64) || defined(__LP64__)
                #define RX_CPU              RX_CPU_POWERPC64
                #define RX_CPU_ARCH         "PowerPC64"
                #define RX_CC_BIT           64
            #else
                #define RX_CPU              RX_CPU_POWERPC32
                #define RX_CPU_ARCH         "PowerPC32"
                #define RX_CC_BIT           32
            #endif
        #elif defined(__sparc)
            #define RX_CPU                  RX_CPU_SPARC
            #define RX_CPU_ARCH             "Sparc"
            #define RX_CC_BIT               32
        #elif defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64) || defined(_WIN64)
            #define RX_CPU                  RX_CPU_X64
            #define RX_CPU_ARCH             "X64"
            #define RX_CC_BIT               64
        #elif defined(__i386) || defined(_M_IX86) || defined(_WIN32)
            #define RX_CPU                  RX_CPU_X86
            #define RX_CPU_ARCH             "X86"
            #define RX_CC_BIT               32
        #else
            #define RX_CPU                  RX_CPU_UNKNOWN
            #define RX_CPU_ARCH             "UNKNOWN"
        #endif
    #endif

    #ifndef RX_CC_BIT
        #define RX_CC_BIT                   0
    #endif

    //-----------------------------------------------------
    //根据网络字节序在寄存器的值定义大小端类型 : 0x00 0x01 0x02 0x03
    #define RX_CPU_ENDIAN_UNKNOWN           0               //Unknown
    #define RX_CPU_ENDIAN_LITTLE            0x03020100      //LE
    #define RX_CPU_ENDIAN_BIG               0x00010203      //BE

    //进行CPU大小端的侦测
    #if RX_CPU == RX_CPU_X86
        #undef RX_CPU_NO_ALIGNMENT_FAULTS
        #define RX_CPU_NO_ALIGNMENT_FAULTS  1
        #undef RX_CPU_ENDIAN
        #define RX_CPU_ENDIAN               RX_CPU_ENDIAN_LITTLE
    #elif RX_CPU == RX_CPU_X64
        #undef RX_CPU_ENDIAN
        #define RX_CPU_ENDIAN               RX_CPU_ENDIAN_LITTLE
    #endif

    #ifndef RX_CPU_NO_ALIGNMENT_FAULTS
        #define RX_CPU_NO_ALIGNMENT_FAULTS  0
    #endif

    #ifndef RX_CPU_ENDIAN
        #define RX_CPU_ENDIAN               RX_CPU_ENDIAN_UNKNOWN
    #endif

    #if RX_CPU_ENDIAN == RX_CPU_ENDIAN_UNKNOWN
        #undef RX_CPU_ENDIAN
        #if (defined(__BYTE_ORDER__)&&defined(__ORDER_BIG_ENDIAN__)&&(__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__))              \
                || (defined(__BYTE_ORDER)&&defined(__BIG_ENDIAN)&&(__BYTE_ORDER == __BIG_ENDIAN))                           \
                || defined(__BIG_ENDIAN__) || defined(__BIG_ENDIAN) || defined(_BIG_ENDIAN)                                 \
                || defined(__MIPSEB__) || defined(__MIPSEB) || defined(_MIPSEB)                                             \
                || defined(__THUMBEB__) || defined(__AARCH64EB__) || defined(__ARMEB__)
            #define RX_CPU_ENDIAN           RX_CPU_ENDIAN_BIG
        #elif (defined(__BYTE_ORDER__)&&defined(__ORDER_LITTLE_ENDIAN__)&&(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__))      \
                || (defined(__BYTE_ORDER)&&defined(_LITTLE_ENDIAN)&&(__BYTE_ORDER == _LITTLE_ENDIAN))                       \
                || defined(__LITTLE_ENDIAN__) || defined(__LITTLE_ENDIAN) || defined(_LITTLE_ENDIAN)                        \
                || defined(__MIPSEL__) || defined(__MIPSEL) || defined(_MIPSEL)                                             \
                || defined(__THUMBEL__) || defined(__AARCH64EL__) || defined(__ARMEL__)                                     \
                || defined(__LITTLE_IF_NOT_BIG__)
            #define RX_CPU_ENDIAN           RX_CPU_ENDIAN_LITTLE
        #else
            #define RX_CPU_ENDIAN           RX_CPU_ENDIAN_UNKNOWN
        #endif
    #endif

    #ifndef DEPRECATED
        #define DEPRECATED(msg)
    #endif

    #ifndef RX_INLINE
        #define RX_INLINE                   inline
    #endif

    #if RX_CPU_ENDIAN==RX_CPU_ENDIAN_LITTLE
        #define RX_CPU_LEBE                 "LE"
    #else
        #define RX_CPU_LEBE                 "BE"
    #endif

    //-----------------------------------------------------
    //定义常用os类型
    #define RX_OS_LINUX                     1
    #define RX_OS_WIN32                     2
    #define RX_OS_WIN64                     3
    #define RX_OS_FREEBSD                   6
    #define RX_OS_NETBSD                    7
    #define RX_OS_MACOS                     8

    //进行OS的自动侦测
    #undef  RX_OS
    #if defined(__POSIX__)
        #define RX_OS_POSIX                 5
    #endif

    #if defined(_CONSOLE)||defined(__CONSOLE__)
        #define RX_IS_CONSOLE               1
    #endif

    #if defined(__FreeBSD__)
        #define RX_OS                       RX_OS_FREEBSD
        #define RX_OS_NAME                  "freebsd"
    #endif

    #if defined(__NetBSD__)
        #define RX_OS                       RX_OS_NETBSD
        #define RX_OS_NAME                  "netbsd"
    #endif

    #if defined(_WIN32)
        #define RX_OS                       RX_OS_WIN32
        #define RX_OS_NAME                  "win32"
    #endif

    #if defined(_WIN64)||defined(__w64)
        #undef  RX_OS
        #define RX_OS                       RX_OS_WIN64
        #undef  RX_OS_NAME
        #define RX_OS_NAME                  "win64"
    #endif

    #if (RX_OS==RX_OS_WIN32)||(RX_OS==RX_OS_WIN64)
        #define RX_OS_WIN                   4
    #endif

    #if defined(__linux__)
        #define RX_OS                       RX_OS_LINUX
        #define RX_OS_NAME                  "linux"
        #ifndef RX_OS_POSIX
            #define RX_OS_POSIX             5
        #endif
    #endif

    #if defined(__MACOSX__)
        #define RX_OS                       RX_OS_MACOS
        #define RX_OS_NAME                  "mac"
    #endif

    //-----------------------------------------------------
    //工具宏定义
    #define _RX_CONCAT_(A,B)                A##B
    #define RX_CT_CONCAT(A,B)               _RX_CONCAT_(A,B)                //宏拼接

    #define RX_CT_LINE                      RX_CT_CONCAT(_LINE,__LINE__)    //行号拼接得行符号 LINE_xxx
    #define RX_CT_LINE_EX(n)                RX_CT_CONCAT(_LINE,n)           //常量数字拼接得行符号

    #define RX_CT_SYM(prename)              RX_CT_CONCAT(prename,RX_CT_LINE)//在行符号的基础上再拼接符号前缀
    #define RX_CT_SYM_EX(prename,n)         RX_CT_CONCAT(prename,RX_CT_LINE_EX(n))

    #define RX_CT_STR(M)                    #M                              //宏转字符串
    #define RX_CT_N2S(N)                    RX_CT_STR(N)                    //宏数字转字符串

    #define is_empty(str)                   (str==NULL||str[0]==0)          //判断字符串是否为空(空指针或首字节为0)

    //-----------------------------------------------------
    //静态断言的实现(基于位域尺寸不能为0的特性,再结合结构体类型定义.)
    #define rx_static_assert(cond) struct RX_CT_SYM(rx_static_assert) {char static_assert_fail:cond;}

    //-----------------------------------------------------
    //用于进行宏默认参数的值替代
    template<typename T>
    struct macro_def_argv
    {
        static bool value() { return 0; }
        static T value(T v) { return v; }
    };

    //-----------------------------------------------------
    //定义指针类型
    typedef void* ptr_t;

    //-----------------------------------------------------
    //构造rx_cc_desc()宏或函数,便于获取当前编译期信息
#if RX_CC == RX_CC_VC
	#include <stdio.h>
    #if (RX_CC_VER_MAJOR<19)
        #define snprintf _snprintf
    #endif

	//visual studio, eg : "CPU:X64(LE)/Microsoft Visual Studio(19.0.1900.1)/64Bit"
	inline const char* rx_cc_desc()
	{
		static char desc[128];
		snprintf(desc,sizeof(desc),"OS:%s/CPU:%s(%s)/CC:%s(%d.%d.%d.%d)/WordLength:%dBit",RX_OS_NAME,RX_CPU_ARCH, RX_CPU_LEBE, RX_CC_NAME, RX_CC_VER_MAJOR, RX_CC_VER_MINOR, RX_CC_VER_PATCH, RX_CC_VER_BUILD, RX_CC_BIT);
		return desc;
	}
#else
    //自动拼装编译器和CPU信息描述. eg : "CPU:X86(LE)/MingW32(5.1.0.0)/32Bit"
    #define RX_CC_DESC ("OS:" RX_OS_NAME "/CPU:" RX_CPU_ARCH "(" RX_CPU_LEBE ")/CC:" RX_CC_NAME "(" RX_CT_N2S(RX_CC_VER_MAJOR) "." RX_CT_N2S(RX_CC_VER_MINOR) "." RX_CT_N2S(RX_CC_VER_PATCH) "." RX_CT_N2S(RX_CC_VER_BUILD) ")/WordLength:" RX_CT_N2S(RX_CC_BIT) "Bit")

    inline const char* rx_cc_desc() {return RX_CC_DESC;}
#endif

    //-----------------------------------------------------
    //根据上面的各类分析,引入各个平台的开发基础头文件
    #if RX_OS_WIN
        #ifndef _WIN32_WINNT
            #define _WIN32_WINNT 0x0600
        #endif

        #define WIN32_LEAN_AND_MEAN

        #include <windows.h>

        #if RX_CC==RX_CC_VC&&RX_CC_VER_MAJOR<=16
        #else
            #include <synchapi.h>
        #endif
    #endif

    #if defined(_DEBUG)||defined(DEBUG)
        #define RX_CC_DEBUG                 1
        #define RX_CC_LIBTAG "d"
    #else
        #define RX_CC_LIBTAG "r"
    #endif

    #include <stdint.h>

    //-----------------------------------------------------
    //定义常用的数学常量
    static const double MATH_E        =2.71828182845904523536   ;// e
    static const double MATH_LOG2E    =1.44269504088896340736   ;// log2(e)
    static const double MATH_LOG10E   =0.434294481903251827651  ;// log10(e)
    static const double MATH_LN2      =0.693147180559945309417  ;// ln(2)
    static const double MATH_LN10     =2.30258509299404568402   ;// ln(10)
    static const double MATH_PI       =3.14159265358979323846   ;// pi
    static const double MATH_PI_2     =1.57079632679489661923   ;// pi/2
    static const double MATH_PI_4     =0.785398163397448309616  ;// pi/4
    static const double MATH_1_PI     =0.318309886183790671538  ;// 1/pi
    static const double MATH_2_PI     =0.636619772367581343076  ;// 2/pi
    static const double MATH_2_SQRTPI =1.12837916709551257390   ;// 2/sqrt(pi)
    static const double MATH_SQRT2    =1.41421356237309504880   ;// sqrt(2)
    static const double MATH_SQRT1_2  =0.707106781186547524401  ;// 1/sqrt(2)

#endif

