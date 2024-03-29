#ifndef _rx_cc_macro_
#define _rx_cc_macro_

    //-----------------------------------------------------
    //是否禁用内联前缀
    #ifndef RX_BAN_INLINE
        #define RX_BAN_INLINE               0
    #endif

    #if RX_BAN_INLINE
        #define rx_inline
    #endif

    //-----------------------------------------------------
    //定义常用编译器类型
    #define RX_CC_UNKNOWN                   0               //Unknown compiler
    #define RX_CC_TC                        1               //Borland Turbo C
    #define RX_CC_BCC                       2               //Borland C++
    #define RX_CC_GCC                       3               //Standard GCC/G++
    #define RX_CC_VC                        4               //Microsoft Visual Studio
    #define RX_CC_CLANG                     5               //CLang (LLVM) compiler
    #define RX_ENV_CYGWIN                   6               //Cygwin (x86)
    #define RX_ENV_MINGW32                  7               //MingW32 (x86) compiler for windows
    #define RX_ENV_MINGW64                  8               //MingW64 (x64) compiler for windows
    #define RX_CC_LCC                       9               //LCC
    #define RX_CC_TINYC                     10              //Tiny C
    #define RX_CC_ULTIMATE                  11              //Ultimate C/C++
    #define RX_CC_ARMCC                     12              //ARM Compiler
    #define RX_CC_IAR                       13              //IAR C/C++
    #define RX_CC_ICC                       14              //Intel ICC/ICPC Compiler
    #define RX_CC_KEIL                      15              //Keil (uVision)

    //-----------------------------------------------------
    //进行编译器环境的自动侦测
    #ifndef RX_CC_ENV
        #if defined(__MINGW64__)
            #define RX_CC_ENV               RX_ENV_MINGW64
            #define RX_CC_ENV_NAME          "mingw64"
        #elif defined(__MINGW32__)
            #define RX_CC_ENV               RX_ENV_MINGW32
            #define RX_CC_ENV_NAME          "mingw32"
        #elif defined(__CYGWIN__)
            #define RX_CC_ENV               RX_ENV_CYGWIN
            #define RX_CC_ENV_NAME          "cygwin"
        #else
            #define RX_CC_ENV               RX_CC_UNKNOWN
            #define RX_CC_ENV_NAME          "native"
        #endif
    #endif

    //-----------------------------------------------------
    //进行编译器类型的自动侦测
    #ifndef RX_CC
        #if defined(__KEIL__) || (defined(__CC_ARM) && defined(__EDG__))
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
    //根据编译器类型,侦测编译器的版本信息
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
        #ifndef rx_inline                   // Get the Keil definition for inline
            #define rx_inline               __inline
        #endif
        #define __LITTLE_IF_NOT_BIG__       //Defines __BIG_ENDIAN but not __LITTLE_ENDIAN
    #elif RX_CC == RX_CC_LCC
        #define RX_CC_NAME                  "LCC"
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
        #define RX_CC_NAME                  "Microsoft Visual C++"
        #ifdef _MSC_FULL_VER
            #if _MSC_FULL_VER < 100000000
                #define RX_CC_VER_MAJOR     ((_MSC_FULL_VER)/1000000)
                #define RX_CC_VER_MINOR     (((_MSC_FULL_VER)/10000)%100)
                #define RX_CC_VER_PATCH     (((_MSC_FULL_VER)/10000)%10000)
            #else
                #define RX_CC_VER_MAJOR     ((_MSC_FULL_VER)/10000000)
                #define RX_CC_VER_MINOR     (((_MSC_FULL_VER)/100000)%100)
                #define RX_CC_VER_PATCH     (((_MSC_FULL_VER)/100000)%10000)
            #endif
        #else
            #define RX_CC_VER_MAJOR         ((_MSC_VER)/100)
            #define RX_CC_VER_MINOR         ((_MSC_VER)%100)
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

    #if RX_CC_ENV==RX_ENV_MINGW32||RX_CC_ENV==RX_ENV_MINGW64
        #define RX_CC_MINGW 1
    #endif
    //-----------------------------------------------------
    //定义常用CPU的类型
    #define RX_CPU_UNKNOWN                  0       //Unknown cpu
    #define RX_CPU_CORTEX_Mx                0x01    //Cortex Mx
    #define RX_CPU_X86                      0x10    //Intel x86
    #define RX_CPU_X64                      0x11    //Intel x64
    #define RX_CPU_IA64                     0x12    //Intel Itanium
    #define RX_CPU_POWERPC32                0x20    //PowerPC
    #define RX_CPU_POWERPC64                0x21    //PowerPC
    #define RX_CPU_SPARC                    0x22    //Sparc

    //自动侦测当前编译器使用的后端CPU类型(类型标识/架构名称/运算位长)
    #ifndef RX_CPU
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

    #ifndef rx_inline
        #define rx_inline                   inline
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
        #define RX_IS_OS_WIN                4
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
    #define RX_CT_CAT(A,B)					A##B							//宏字面拼接
    #define RX_CT_CONCAT(A,B)               RX_CT_CAT(A,B)                  //宏展开拼接

    //拼装_LINE_xx的标识符
    #define RX_CT_LINE                      RX_CT_CONCAT(_LINE,__LINE__)    //行号拼接得行符号 LINE_xxx
    #define RX_CT_LINE_EX(n)                RX_CT_CONCAT(_LINE,n)           //常量数字拼接得行符号

    //静态拼装标识符,可用于变量定义或引用(由于结合了行号,所以单一文件内多行相同前缀使用不会重复)
    #define RX_CT_SYM(prename)              RX_CT_CONCAT(prename,RX_CT_LINE)//在行符号的基础上再拼接符号前缀
    #define RX_CT_SYM_EX(prename,n)         RX_CT_CONCAT(prename,RX_CT_LINE_EX(n))

    //将宏符号或任意符号转换为字符串
    #define RX_CT_STR(M)                    #M                              //宏转字符串
    #define RX_CT_N2S(N)                    RX_CT_STR(N)                    //宏数字转字符串

    //判断指针是否为空或空串
    #define is_empty(str)                   (str==NULL||str[0]==0)          //判断字符串是否为空(空指针或首字节为0)
    //避免空指针出现
    #define not_empty(ptr)                  (ptr==NULL?"":ptr)              //使用空字符串代替空指针

    //将str字符串中的连续两个字节,转换为字符集编码值
    #define str2code(str) ((((uint8_t)str[0]) << 8) | (uint8_t)str[1])

    //将原始指针ptr从头部偏移offset的位置,转换为类型type的指针
    #define rx_ptr_head_cast(ptr,offset,type) (type*)((uint8_t*)(ptr)+(offset))

    //将内存长度为size的原始指针ptr从尾部偏移offset的位置,转换为类型type的指针
    #define rx_ptr_tail_cast(ptr,size,offset,type) rx_ptr_head_cast(ptr,(size-offset),type)

    //-----------------------------------------------------
    //静态断言的实现(基于位域尺寸不能为0的特性,再结合结构体类型定义.)
    #define rx_static_assert(cond) struct RX_CT_SYM(rx_static_assert) {int static_assert_fail:(cond);}

    //ms毫秒到us微秒
    #define ms2us(ms) (1000*ms)
    //sec秒到us微秒
    #define sec2us(s) ms2us(1000*s)
	//计算数组长度
	#define rx_arrsize(arr) (sizeof(arr)/sizeof(arr[0]))

    //-----------------------------------------------------
    //根据上面的各类分析,引入各个平台的开发基础头文件
    #if RX_IS_OS_WIN
        #ifndef _WIN32_WINNT
            #define _WIN32_WINNT 0x0600
        #endif

        #define WIN32_LEAN_AND_MEAN
        #define _CRT_SECURE_NO_WARNINGS 1

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

    #include <stdio.h>
    #include <stdint.h>
    #include <stddef.h>
    #include <stdarg.h>

    //-----------------------------------------------------
    rx_static_assert(sizeof(int64_t)==sizeof(long long));
    rx_static_assert(sizeof(size_t)==sizeof(ptrdiff_t));

	namespace rx
	{
		typedef uint16_t wchar;								//定义2个字节的unicode字符(标准的wchar_t在vc中是2字节,在linux/gcc上是4个字节)
	}
	enum { CPU_CACHELINE_SIZE = 64 };                       // CACHE-LINE高速缓存的尺寸

	//==========================================================
	// 参考 https://github.com/swansontec/map-macro
	//核心原理: 利用宏的动态展开,以及交叉调用,再结合最外层足够次数的展开保护.
	//递归嵌套宏展开,多层嵌套调用确保展开次数足够(默认展开365次)
	#define RX_MACRO_EXPAND(...) __VA_ARGS__
	#define RX_MACRO_EVAL1(...) RX_MACRO_EXPAND(RX_MACRO_EXPAND(RX_MACRO_EXPAND(__VA_ARGS__)))
	#define RX_MACRO_EVAL2(...) RX_MACRO_EVAL1(RX_MACRO_EVAL1(RX_MACRO_EVAL1(__VA_ARGS__)))
	#define RX_MACRO_EVAL3(...) RX_MACRO_EVAL2(RX_MACRO_EVAL2(RX_MACRO_EVAL2(__VA_ARGS__)))
	#define RX_MACRO_EVAL4(...) RX_MACRO_EVAL3(RX_MACRO_EVAL3(RX_MACRO_EVAL3(__VA_ARGS__)))
	#define RX_MACRO_EVAL(...)  RX_MACRO_EVAL4(RX_MACRO_EVAL4(RX_MACRO_EVAL4(__VA_ARGS__)))

	//宏哑元,只用来占位
	#define RX_MACRO_DUMMY
	//逗号分隔符
	#define RX_MACRO_COMMA ,

	//宏嵌套递归,与EAT宏配合,用于结束参数列表的遍历
	#define _MACRO_MAP_END0(...)
	#define _MACRO_MAP_END1() 0, _MACRO_MAP_END0 //0作为需要被最后吃掉的占位符
	#define _MACRO_MAP_END2(...) _MACRO_MAP_END1
	#define _MACRO_MAP_END(...)  _MACRO_MAP_END2

	//宏嵌套递归,丢弃参数列表中的前一项,引用后一项
	#define _MACRO_MAP_EAT0(discard, next, ...) next RX_MACRO_DUMMY
	#if RX_CC == RX_CC_VC
	#define _MACRO_MAP_EAT1(discard, next)  RX_MACRO_EXPAND(_MACRO_MAP_EAT0(discard, next, 0)) //对于vc需要额外展开一次,否则不会正确遍历结束
	#define _MACRO_MAP_EATL1(discard, next) RX_MACRO_EXPAND(_MACRO_MAP_EAT0(discard, RX_MACRO_COMMA next, 0))
	#else
	#define _MACRO_MAP_EAT1(discard, next)  _MACRO_MAP_EAT0(discard, next, 0)
	#define _MACRO_MAP_EATL1(discard, next) _MACRO_MAP_EAT0(discard, RX_MACRO_COMMA next, 0)
	#endif
	#define _MACRO_MAP_EAT(discard, next)  _MACRO_MAP_EAT1(_MACRO_MAP_END discard, next)

	//宏嵌套递归,前置使用入参f(x)后,利用动态宏展开,交叉引用逐一遍历参数列表,生成最终的展开结果
	#define _MACRO_MAP0(f, x, nextx, ...) f(x) _MACRO_MAP_EAT(nextx, _MACRO_MAP1)(f, nextx, __VA_ARGS__)
	#define _MACRO_MAP1(f, x, nextx, ...) f(x) _MACRO_MAP_EAT(nextx, _MACRO_MAP0)(f, nextx, __VA_ARGS__)

	//宏嵌套递归,在中间展开的时候插入分隔符,便于构造参数列表
	#define _MACRO_MAP_EATL(discard, next)  _MACRO_MAP_EATL1(_MACRO_MAP_END discard, next)
	#define _MACRO_MAPL0(f, x, nextx, ...) f(x) _MACRO_MAP_EATL(nextx, _MACRO_MAPL1)(f, nextx, __VA_ARGS__)
	#define _MACRO_MAPL1(f, x, nextx, ...) f(x) _MACRO_MAP_EATL(nextx, _MACRO_MAPL0)(f, nextx, __VA_ARGS__)

	//-----------------------------------------------------
	//参数列表功能,中间生成逗号分隔符
	#define RX_MACRO_MAPL(f, ...) RX_MACRO_EVAL(_MACRO_MAPL1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))
	//参数映射功能,原样输出f和参数列表
	#define RX_MACRO_MAP(f, ...) RX_MACRO_EVAL(_MACRO_MAP1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0)) //()()()是与_MACRO_MAP_END层级配合,确保递归可结束

	//-----------------------------------------------------
	//获取动态宏参数的个数
	#define RX_MACRO_NARG(...) RX_MACRO_EXPAND(_MACRO_NARG_CALC(__VA_ARGS__,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1))
	#define _MACRO_NARG_CALC(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,N,...)  N

#endif

