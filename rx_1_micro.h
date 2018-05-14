#ifndef _rx_1_micro_
#define _rx_1_micro_

	#ifndef RX_BAN_INLINE
		#define RX_BAN_INLINE			FALSE
	#endif

	#if RX_BAN_INLINE
		#define RX_INLINE
	#endif

	#ifndef RX_CC
		#define RX_CC			RX_CC_UNKNOWN
	#endif
	#define RX_CC_UNKNOWN		0		//Unknown compiler
	#define RX_CC_TC		    1		//Borland Turbo C
	#define RX_CC_BCC		    2		//Borland C++
	#define RX_CC_GCC			3		//Standard GCC/G++
	#define RX_CC_VC			4		//Microsoft Visual Studio
	#define RX_CC_CLANG			5		//CLang (LLVM) compiler
	#define RX_CC_CYGWIN		6		//Cygwin (x86)
	#define RX_CC_MINGW32		7		//MingW32 (x86) compiler for windows
	#define RX_CC_MINGW64		8		//MingW64 (x64) compiler for windows
	#define RX_CC_LCC			9		//LCC
	#define RX_CC_TINYC			10		//Tiny C
	#define RX_CC_ULTIMATE		11		//Ultimate C/C++
	#define RX_CC_ARMCC			12		//ARM Compiler
	#define RX_CC_IAR			13		//IAR C/C++
	#define RX_CC_ICC			14		//Intel ICC/ICPC Compiler
	#define RX_CC_KEIL			15		//Keil (uVision)

	#if RX_CC == RX_CC_UNKNOWN
		#undef RX_CC
		#if defined(__MINGW32__)
			#define RX_CC	RX_CC_MINGW32
		#elif defined(__MINGW64__)
			#define RX_CC	RX_CC_MINGW64
		#elif defined(__CYGWIN__)
			#define RX_CC	RX_CC_CYGWIN
		#elif defined(__KEIL__) || (defined(__CC_ARM) && defined(__EDG__))
			#define RX_CC	RX_CC_KEIL
		#elif defined(__clang__) || defined(__llvm__)
			#define RX_CC	RX_CC_CLANG
		#elif defined(__INTEL_COMPILER) || defined(__ICC) || defined(__ECC) || defined(__TCL)
			#define RX_CC	RX_CC_ICC
		#elif defined(__GNUC__) || defined(__GNUG__)
			#define RX_CC	RX_CC_GCC
		#elif defined(__CC_ARM)
			#define RX_CC	RX_CC_ARMCC
		#elif defined(_MSC_VER)
			#define RX_CC	RX_CC_VC
		#elif defined(__TURBOC__)
			#define RX_CC	RX_CC_TC
		#elif defined(__BORLANDC__)
			#define RX_CC	RX_CC_BCC
		#elif defined(__LCC__)
			#define RX_CC	RX_CC_LCC
		#elif defined(__TINYC__)
			#define RX_CC	RX_CC_TINYC
		#elif defined(__IAR_SYSTEMS_ICC__)
			#define RX_CC	RX_CC_IAR
		#elif defined(_UCC)
			#define RX_CC	RX_CC_ULTIMATE

		#else
			#define RX_CC	RX_CC_UNKNOWN
		#endif
	#endif

	#undef RX_CC_NAME
	#undef RX_CC_TESTED
	#undef RX_CC_VERSION_MAJOR
	#undef RX_CC_VERSION_MINOR
	#undef RX_CC_VERSION_PATCH
	#undef RX_CC_VERSION_BUILD
	#if RX_CC == RX_CC_ARMCC
		#define RX_CC_NAME					"ARMCC"
		#define RX_CC_VERSION_MAJOR			((__ARMCC_VERSION)/100000)
		#define RX_CC_VERSION_MINOR			(((__ARMCC_VERSION)/10000)%10)
		#define RX_CC_VERSION_PATCH			(((__ARMCC_VERSION)/1000)%10)
		#define RX_CC_VERSION_BUILD			((__ARMCC_VERSION)%1000)
		#define __LITTLE_IF_NOT_BIG__				// Oops - Defines __BIG_ENDIAN but not __LITTLE_ENDIAN
	#elif RX_CC == RX_CC_BCC
		#define RX_CC_NAME					"Borland C++"
		#define RX_CC_VERSION_MAJOR			((__BORLANDC__)/0x100)
		#define RX_CC_VERSION_MINOR			(((((__BORLANDC__)%0x100)/0x10)*10) + ((__BORLANDC__)%0x10))
	#elif RX_CC == RX_CC_CLANG
		#define RX_CC_NAME					"CLang (LLVM)"
		#define RX_CC_TESTED					TRUE
		#define RX_CC_VERSION_MAJOR			(__clang_major__)
		#define RX_CC_VERSION_MINOR			(__clang_minor__)
		#define RX_CC_VERSION_PATCH			(__clang_patchlevel__)
		#define DEPRECATED(msg)						__attribute__((deprecated(msg)))
	#elif RX_CC == RX_CC_CYGWIN
		#define RX_CC_NAME					"Cygwin"
		#define RX_CC_TESTED					TRUE
		#define RX_CC_VERSION_MAJOR			(__GNUC__)
		#define RX_CC_VERSION_MINOR			(__GNUC_MINOR__)
		#ifdef __GNUC_PATCHLEVEL__
			#define RX_CC_VERSION_PATCH		(__GNUC_PATCHLEVEL__)
		#endif
		#define DEPRECATED(msg)						__attribute__((deprecated(msg)))
	#elif RX_CC == RX_CC_GCC
		#define RX_CC_NAME					"GCC"
		#define RX_CC_TESTED					TRUE
		#define RX_CC_VERSION_MAJOR			(__GNUC__)
		#define RX_CC_VERSION_MINOR			(__GNUC_MINOR__)
		#ifdef __GNUC_PATCHLEVEL__
			#define RX_CC_VERSION_PATCH		(__GNUC_PATCHLEVEL__)
		#endif
		#define DEPRECATED(msg)						__attribute__((deprecated(msg)))
	#elif RX_CC == RX_CC_IAR
		#define RX_CC_NAME					"IAR C++"
		#define RX_CC_TESTED					TRUE
		#define RX_CC_VERSION_MAJOR			((__VER__)/100)
		#define RX_CC_VERSION_MINOR			((__VER__)%100)
		#pragma diag_remark = Pe301
		#pragma diag_remark = Pe083
		#pragma diag_remark = Pe767
		#pragma diag_remark = Pe188
		#pragma diag_remark = Pe186
		#pragma diag_remark = Pe068
		#pragma diag_remark = Pa050
	#elif RX_CC == RX_CC_ICC
		#define RX_CC_NAME					"Intel ICC/ICPC"
		#ifdef __INTEL_COMPILER
			#define RX_CC_VERSION_MAJOR		((__INTEL_COMPILER)/100)
			#define RX_CC_VERSION_MINOR		(((__INTEL_COMPILER)/10)%10)
			#define RX_CC_VERSION_PATCH		((__INTEL_COMPILER)%10)
			#ifdef __INTEL_COMPILER_BUILD_DATE
				#define RX_CC_VERSION_BUILD	(__INTEL_COMPILER_BUILD_DATE)
			#endif
		#endif
	#elif RX_CC == RX_CC_KEIL
		#define RX_CC_NAME					"Keil"
		#define RX_CC_TESTED					TRUE
		#if defined(__ARMCC_VERSION)
			#define RX_CC_VERSION_MAJOR		((__ARMCC_VERSION)/1000000)
			#define RX_CC_VERSION_MINOR		(((__ARMCC_VERSION)/10000)%100)
			#define RX_CC_VERSION_PATCH		((__ARMCC_VERSION)%10000)
			#ifdef __EDG_VERSION__
				#define RX_CC_VERSION_BUILD	(__EDG_VERSION__)
			#endif
		#elif defined(__CA__)
			#define RX_CC_VERSION_MAJOR		((__CA__)/100)
			#define RX_CC_VERSION_MINOR		((__CA__)%100)
		#elif defined(__C166__)
			#define RX_CC_VERSION_MAJOR		((__C166__)/100)
			#define RX_CC_VERSION_MINOR		((__C166__)%100)
		#elif defined(__C51__)
			#define RX_CC_VERSION_MAJOR		((__C51__)/100)
			#define RX_CC_VERSION_MINOR		((__C51__)%100)
		#elif defined(__C251__)
			#define RX_CC_VERSION_MAJOR		((__C251__)/100)
			#define RX_CC_VERSION_MINOR		((__C251__)%100)
		#endif
		#define DEPRECATED(msg)						__attribute__((deprecated(msg)))
		#pragma anon_unions							// Allow anonymous unions
		#pragma diag_remark 1293					// Turn off warning: assignment in condition
		#pragma diag_remark 83						// Turn off warning: type qualifier specified more than once
		#pragma diag_remark 767						// Turn off warning: conversion from pointer to smaller integer
		#pragma diag_remark 188						// Turn off warning: enumerated type mixed with another type
		#ifndef RX_INLINE							// Get the Keil definition for inline
			#define RX_INLINE	__inline
		#endif
		#define __LITTLE_IF_NOT_BIG__				// Oops - Defines __BIG_ENDIAN but not __LITTLE_ENDIAN
	#elif RX_CC == RX_CC_LCC
		#define RX_CC_NAME					"LCC"
	#elif RX_CC == RX_CC_MINGW32
		#define RX_CC_NAME					"MingW32"
		#define RX_CC_TESTED					TRUE
		#define RX_CC_VERSION_MAJOR			(__GNUC__)
		#define RX_CC_VERSION_MINOR			(__GNUC_MINOR__)
		#ifdef __GNUC_PATCHLEVEL__
			#define RX_CC_VERSION_PATCH		(__GNUC_PATCHLEVEL__)
		#endif
		#define DEPRECATED(msg)						__attribute__((deprecated(msg)))
	#elif RX_CC == RX_CC_MINGW64
		#define RX_CC_NAME					"MingW64"
		#define RX_CC_VERSION_MAJOR			(__GNUC__)
		#define RX_CC_VERSION_MINOR			(__GNUC_MINOR__)
		#ifdef __GNUC_PATCHLEVEL__
			#define RX_CC_VERSION_PATCH		(__GNUC_PATCHLEVEL__)
		#endif
		#define DEPRECATED(msg)						__attribute__((deprecated(msg)))
	#elif RX_CC == RX_CC_TINYC
		#define RX_CC_NAME					"Tiny C"
		#define RX_CC_TESTED					TRUE
	#elif RX_CC == RX_CC_TC
		#define RX_CC_NAME					"Borland Turbo C/C++"
		#if __TURBOC__ < 0x295 || __TURBOC__ >= 0x400
			#define RX_CC_VERSION_MAJOR		((__TURBOC__)/0x100)
			#define RX_CC_VERSION_MINOR		((__TURBOC__)%0x100)
		#elif __TURBOC__ == 0x295
			#define RX_CC_VERSION_MAJOR		(1)
			#define RX_CC_VERSION_MINOR		(0)
		#elif __TURBOC__ == 0x296
			#define RX_CC_VERSION_MAJOR		(1)
			#define RX_CC_VERSION_MINOR		(1)
		#elif __TURBOC__ == 0x297
			#define RX_CC_VERSION_MAJOR		(2)
			#define RX_CC_VERSION_MINOR		(0)
		#endif
	#elif RX_CC == RX_CC_ULTIMATE
		#define RX_CC_NAME					"Ultimate C/C++"
		#define RX_CC_VERSION_MAJOR			(_MAJOR_REV)
		#define RX_CC_VERSION_MINOR			(_MINOR_REV)
	#elif RX_CC == RX_CC_VC
		#define RX_CC_NAME					"Microsoft Visual Studio"
		#ifdef _MSC_FULL_VER
			#if _MSC_FULL_VER < 100000000
				#define RX_CC_VERSION_MAJOR	((_MSC_FULL_VER)/1000000)
				#define RX_CC_VERSION_MINOR	(((_MSC_FULL_VER)/10000)%100)
				#define RX_CC_VERSION_PATCH	(((_MSC_FULL_VER)/10000)%10000)
			#else
				#define RX_CC_VERSION_MAJOR	((_MSC_FULL_VER)/10000000)
				#define RX_CC_VERSION_MINOR	(((_MSC_FULL_VER)/100000)%100)
				#define RX_CC_VERSION_PATCH	(((_MSC_FULL_VER)/100000)%10000)
			#endif
		#else
			#define RX_CC_VERSION_MAJOR		((_MSC_VER)/100)
			#define RX_CC_VERSION_MINOR		((_MSC_VER)%100)
		#endif
		#ifdef _MSC_BUILD
			#define RX_CC_VERSION_BUILD		(_MSC_BUILD)
		#endif
		#define DEPRECATED(msg)						__declspec(deprecated(msg))
	#endif

	#ifndef RX_CC_TESTED
		#define RX_CC_TESTED		FALSE
	#endif

	#ifndef RX_CC_NAME
		#define RX_CC_NAME		"Unknown"
		#warning "You are using an unknown compiler."
	#endif

	#if !RX_CC_TESTED
		#warning "You are using an un-tested compiler."
	#endif

	#ifndef RX_CC_VERSION_MAJOR
		#define RX_CC_VERSION_MAJOR	0
	#endif

	#ifndef RX_CC_VERSION_MINOR
		#define RX_CC_VERSION_MINOR	0
	#endif

	#ifndef RX_CC_VERSION_PATCH
		#define RX_CC_VERSION_PATCH	0
	#endif

	#ifndef RX_CC_VERSION_BUILD
		#define RX_CC_VERSION_BUILD	0
	#endif

	#ifndef RX_CPU
		#define RX_CPU					RX_CPU_UNKNOWN
	#endif
	#define RX_CPU_UNKNOWN				0		//**< Unknown cpu
	#define RX_CPU_CORTEX_M0			0x01	//**< Cortex M0
	#define RX_CPU_CORTEX_M1			0x02	//**< Cortex M1
	#define RX_CPU_CORTEX_M2			0x03	//**< Cortex M2
	#define RX_CPU_CORTEX_M3			0x04	//**< Cortex M3
	#define RX_CPU_CORTEX_M4			0x05	//**< Cortex M4
	#define RX_CPU_CORTEX_M4_FP		    0x06	//**< Cortex M4 with hardware floating point
	#define RX_CPU_CORTEX_M7			0x07	//**< Cortex M7
	#define RX_CPU_CORTEX_M7_FP		    0x08	//**< Cortex M7 with hardware floating point
	#define RX_CPU_X86					0x10	//**< Intel x86
	#define RX_CPU_X64					0x11	//**< Intel x64
	#define RX_CPU_IA64				    0x12	//**< Intel Itanium
	#define RX_CPU_POWERPC32			0x20	//**< PowerPC
	#define RX_CPU_POWERPC64			0x21	//**< PowerPC
	#define RX_CPU_SPARC				0x22	//**< Sparc

	#if RX_CPU == RX_CPU_UNKNOWN
		#undef RX_CPU
		#if defined(__ia64) || defined(__itanium__) || defined(_M_IA64)
			#define RX_CPU		RX_CPU_IA64
		#elif defined(__powerpc__) || defined(__ppc__) || defined(__PPC__)
			#if defined(__powerpc64__) || defined(__ppc64__) || defined(__PPC64__) || defined(__64BIT__) || defined(_LP64) || defined(__LP64__)
				#define RX_CPU		RX_CPU_POWERPC64
			#else
				#define RX_CPU		RX_CPU_POWERPC32
			#endif
		#elif defined(__sparc)
			#define RX_CPU		RX_CPU_SPARC
		#elif defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64) || defined(_WIN64)
			#define RX_CPU		RX_CPU_X64
		#elif defined(__i386) || defined(_M_IX86) || defined(_WIN32)
			#define RX_CPU		RX_CPU_X86
		#else
			#define RX_CPU		RX_CPU_UNKNOWN
		#endif
	#endif

	#if RX_CPU == RX_CPU_X86
		#undef RX_CPU_NO_ALIGNMENT_FAULTS
		#define RX_CPU_NO_ALIGNMENT_FAULTS		TRUE
		#undef RX_CPU_ENDIAN
		#define RX_CPU_ENDIAN					RX_CPU_ENDIAN_LITTLE
	#elif RX_CPU == RX_CPU_X64
		#undef RX_CPU_ENDIAN
		#define RX_CPU_ENDIAN					RX_CPU_ENDIAN_LITTLE
	#endif

	#ifndef RX_CPU_NO_ALIGNMENT_FAULTS
		#define RX_CPU_NO_ALIGNMENT_FAULTS		FALSE
	#endif

	#ifndef RX_CPU_ENDIAN
		#define RX_CPU_ENDIAN			        RX_CPU_ENDIAN_UNKNOWN
	#endif

	#define RX_CPU_ENDIAN_UNKNOWN		0				//**< Unknown endianness
	#define RX_CPU_ENDIAN_LITTLE		0x03020100		//**< Little endian
	#define RX_CPU_ENDIAN_BIG			0x00010203		//**< Big endian

	#if RX_CPU_ENDIAN == RX_CPU_ENDIAN_UNKNOWN
		#undef RX_CPU_ENDIAN
		#if (defined(__BYTE_ORDER__)&&defined(__ORDER_BIG_ENDIAN__)&&(__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)) 			\
				|| (defined(__BYTE_ORDER)&&defined(__BIG_ENDIAN)&&(__BYTE_ORDER == __BIG_ENDIAN))						\
				|| defined(__BIG_ENDIAN__) || defined(__BIG_ENDIAN) || defined(_BIG_ENDIAN) 							\
				|| defined(__MIPSEB__) || defined(__MIPSEB) || defined(_MIPSEB) 										\
				|| defined(__THUMBEB__) || defined(__AARCH64EB__) || defined(__ARMEB__)
			#define RX_CPU_ENDIAN			RX_CPU_ENDIAN_BIG
		#elif (defined(__BYTE_ORDER__)&&defined(__ORDER_LITTLE_ENDIAN__)&&(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__))	\
				|| (defined(__BYTE_ORDER)&&defined(_LITTLE_ENDIAN)&&(__BYTE_ORDER == _LITTLE_ENDIAN))					\
				|| defined(__LITTLE_ENDIAN__) || defined(__LITTLE_ENDIAN) || defined(_LITTLE_ENDIAN)					\
				|| defined(__MIPSEL__) || defined(__MIPSEL) || defined(_MIPSEL)											\
				|| defined(__THUMBEL__) || defined(__AARCH64EL__) || defined(__ARMEL__)									\
				|| defined(__LITTLE_IF_NOT_BIG__)
			#define RX_CPU_ENDIAN			RX_CPU_ENDIAN_LITTLE
		#else
			#define RX_CPU_ENDIAN			RX_CPU_ENDIAN_UNKNOWN
		#endif
	#endif

	#ifndef DEPRECATED
		#define DEPRECATED(msg)
	#endif

	#ifndef RX_INLINE
		#define RX_INLINE	inline
	#endif


#endif

