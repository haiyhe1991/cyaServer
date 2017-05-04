#ifndef __CYA_BASE_H__
#define __CYA_BASE_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "msc_opt.h"

/*编译器版本,OS类型/位数,cpu位数/硬件构架等检查*/

/*********************************/
/******** 检查平台是否支持 *******/
/*********************************/
#if !defined(WIN32) && !defined(_WIN64) && !defined(__linux__)
	#error Unsupported platform.
#endif

/* 编译器/平台类型 */
#if defined(_MSC_VER) && (_MSC_VER <= 1200) // <= VC6.0
	#defin2e VS_IS_VC6

#elif defined(_MSC_VER) && (_MSC_VER == 1500) // == VC2008
	#define VS_IS_VC2008

#elif defined(_MSC_VER) && (_MSC_VER == 1600) // >= VC2010
	#define VS_IS_VC2010

#elif defined(_MSC_VER) && (_MSC_VER == 1700)	// == VC2012 
	#define VS_IS_VC2012

#elif defined(_MSC_VER) && (_MSC_VER == 1800)	/* == VC2013 */
	#define VS_IS_VC2013
#endif

//CPU
#if defined(_M_IX86) || defined(__i386__) || defined(__x86_64) || defined(__x86_64__)
	#define CPU_IS_X86
#endif

#if defined(_M_IA64) || defined(_M_X64) || defined(_M_IX64) || defined(__x86_64) || defined(__x86_64__)
	#define CPU_IS_X64
#endif

#if defined(__mips__) || defined(__MIPSEL__)
	#define CPU_IS_MIPS
#elif defined(__arm__) || defined(_M_ARM_FP)
	#define CPU_IS_ARM
#endif

//WINDOWS
#if defined(_MSC_VER)
	#define OS_IS_WINDOWS

	#if defined(_WIN64)
		#define OS_IS_WIN64
	#elif defined(WIN32)
		#define OS_IS_WIN32
	#elif defined(WIN16)
		#define OS_IS_WIN16
	#endif
#endif

//LINUX
#if defined(__linux__)
	#define OS_IS_LINUX

	#if defined(CPU_IS_MIPS) || defined(CPU_IS_ARM)
		#define OS_IS_MIPS_ARM_LINUX
	#endif
#endif

//ANDROID
#if defined(__ANDROID__) || defined(ANDROID)
	#define OS_IS_ANDROID

	#if defined(CPU_IS_X86) || defined(CPU_IS_X64)
		#define OS_IS_ANDROID_X86
	#elif defined(CPU_IS_ARM)
		#define OS_IS_ANDROID_ARM
	#endif
#endif

//APPLE
#if (defined(__APPLE__) && (defined(__GNUC__) || defined(__xlC__) || defined(__xlc__))) || defined(__MACOS__)
	#if (defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__) || defined(__IPHONE_OS_VERSION_MIN_REQUIRED))
		#define OS_IS_IPHONE
	#else
		#define OS_IS_MAC
	#endif
#endif
#if defined(OS_IS_IPHONE) || defined(OS_IS_MAC)
	#define OS_IS_APPLE
#endif

/* unicode */
#if defined(_UNICODE) || defined(UNICODE)
	#undef _UNICODE
	#undef UNICODE
	#define _UNICODE 1
	#define UNICODE 1
#endif

#ifdef __cplusplus
	#define X_NAMESPACE_IMPL_BEGIN(name) \
		namespace xpl_nms_##name	\
		{
#else
	#define X_NAMESPACE_IMPL_BEGIN(name)
#endif

#ifdef __cplusplus
	#define X_NAMESPACE_IMPL_END(name) \
				}; \
	using namespace xpl_nms_##name;
#else
	#define X_NAMESPACE_IMPL_END(name)
#endif

/**********基本系统头文件*********/
#if defined(WIN32)
	#ifndef _WINDOWS_
		#ifndef _WINSOCKAPI_
			#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
		#endif

		#ifndef _CONSOLE
			#include <windows.h>
		#endif
	#endif
#endif

#if defined(WIN32)
	#ifndef _WINSOCK2API_
		#pragma warning(disable: 4115) /* 'xxx' : named type definition in parentheses) */
		#include <winsock2.h>
	#endif
	#include <tchar.h>
#elif defined(__linux__)
	#ifdef _FILE_OFFSET_BITS
		#undef _FILE_OFFSET_BITS
		#define _FILE_OFFSET_BITS 64	//linux下支持大于4G的文件
	#else
		#define _FILE_OFFSET_BITS 64
	#endif

	#include <errno.h>
	#include <unistd.h>
	#include <netinet/in.h>
	#include <pthread.h>
	#include <sys/time.h>
	#include <sys/times.h>
#endif

#if defined(__cplusplus)
	#include <string>
#endif

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <assert.h>

#if defined(_MSC_VER) && !defined(VS_IS_VC6)
	#include <intrin.h>
#endif


#if defined(_MSC_VER)
	#if defined(_WIN64)
		#define OPR_SYS_BITS		64
	#elif defined(WIN32)
		#define OPR_SYS_BITS		32
	#elif defined(WIN16)
		#define OPR_SYS_BITS		16
	#endif
#else
	#if defined(__LP64__)			\
	|| defined(_LP64)		\
	|| defined(__64BIT__)	\
	|| defined(__x86_64)	\
	|| defined(__x86_64__)	\
	|| defined(__amd64)		\
	|| defined(__amd64__)	\
	|| (64 == __WORDSIZE)
		#define OPR_SYS_BITS		64
	#else
		#define OPR_SYS_BITS		32
	#endif
#endif

#ifdef HDR_RDTSC_SUPPORTED
	#error HDR_RDTSC_SUPPORTED defined.
#endif

#if (defined(WIN32) || defined(_WIN64))
	#define HDR_RDTSC_SUPPORTED
#elif defined(__linux__)
	#if defined(CPU_IS_X86)
		#define HDR_RDTSC_SUPPORTED
	#endif
#endif

/*********************************/
#ifdef __cplusplus
	#define REF_OPERATER &
	#define DEFAULT_PARAM(v)	= v
#else
	#define REF_OPERATER
	#define DEFAULT_PARAM(v)
	#if defined(_MSC_VER)
		#define inline				static __inline
	#elif defined(OS_IS_MIPS_ARM_LINUX)
		#define inline				static inline
	#endif
#endif

/* const int SIZEOF(exp); */
#define SIZEOF(exp)				((const int)sizeof(exp))

#ifndef INFINITE
	#define INFINITE			0xFFFFFFFF  /* Infinite timeout */
#endif

/* const int ARRAY_LEN(ar); */
// #ifdef __cplusplus
// 	#if defined(_MSC_VER)// && (_MSC_VER <= 1200) //|| defined(OS_IS_MIPS_ARM_LINUX)
// 		#define ARRAY_LEN(ar)	(SIZEOF(ar) / SIZEOF(0[ar]))
// 	#else
// 		template<int BYTES_N>
// 		struct bytes_n_ARRAY_LEN { char b[BYTES_N]; };
// 		template<typename TYPE, int BYTES_N>
// 		bytes_n_ARRAY_LEN<BYTES_N> get_bytes_n_ARRAY_LEN(TYPE (&) [BYTES_N]);
// 		#define ARRAY_LEN(x)	SIZEOF(get_bytes_n_ARRAY_LEN(x))
// 	#endif
// #else
// 	#define ARRAY_LEN(ar)		(SIZEOF(ar) / SIZEOF(0[ar]))
// #endif

#define ARRAY_LEN(ar)		(SIZEOF(ar) / SIZEOF(0[ar]))

//derived类相对于base类的偏移地址
#define offset_of_class(base, derived)	 ((DWORD)(static_cast<base*>((derived*)64))-64)

/* s的成员m的相对偏移地址 */
#define offset_of(s, m)				((int)(UINT_PTR)&(((s*)64)->m) - 64) /* 不使用0地址，避免某些编译器警告 */

//s的成员b相对于成员a的偏移量
#define member_offset(s, a, b)	((int)((UINT_PTR)&(((s*)64)->b) - (UINT_PTR)&(((s*)64)->a)))

/* 得到成员size */
#define member_size(c, m)			((int)sizeof(((c*)64)->m))

/* 得到包含类的对象指针，c为类名字，m为c的某个数据成员 */
#define container_pointer(c, m)		(c*)((UINT_PTR)this - offset_of(c, m))

/* 初始化结构变量和数组 */
#define ZeroStru(stru)				memset(&(stru), 0, sizeof(stru))
#define ZeroArray(arr)				memset(arr, 0, sizeof(arr))

#if defined(WIN32)
	#if 64 == OPR_SYS_BITS
		#define asm_int_3_	__debugbreak()
	#else
		#define asm_int_3_	{	__asm int 3	}
	#endif
#elif defined(__linux__)
	#define asm_int_3_	__asm__ __volatile__(".byte 0xcc")
#endif

//是否支持epoll
#if defined(__linux__) && !defined(OS_IS_IPHONE)/* && !defined(__ANDROID__)*/	//手机上一般不会用到这么复杂(poll就够了)
	#if !(__GNUC_PATCHLEVEL__ == 2 && __GNUC__ == 3 && __GNUC_MINOR__ == 2)
		#define LINUX_EPOLL_SUPPORTED
	#endif
#endif

//是否支持gcc原子操作
#if defined(__linux__)
	#if defined(__GNUC__) && defined(__GNUC_MINOR__) && defined(__GNUC_PATCHLEVEL__)
		#if (__GNUC__*100 + __GNUC_MINOR__*10 + __GNUC_PATCHLEVEL__) >= 412
			#define GCC_ATOMIC_SUPPORTED
		#endif
	#endif
#endif

#if defined(ASSERT)
	#undef ASSERT
#endif
#define ASSERT assert

#if defined(VERIFY)
	#undef VERIFY
#endif
#define VERIFY(f) (void)(f)


/* 宏 */
#ifndef IN
#define IN
#endif
#ifndef OUT
#define OUT
#endif

#ifndef CONST
#define CONST const
#endif

#ifndef VOID
#define VOID void
#endif

#ifndef NULL
	#ifdef __cplusplus
		#define NULL 0
	#else
		#define NULL ((void*)0)
	#endif
#endif

#ifndef TRUE
	#define TRUE 1
#endif
#ifndef FALSE
	#define FALSE 0
#endif

#ifndef __cplusplus
	#undef true
	#undef false
	#define true 1
	#define false 0
#endif

/* 基本数据类型 */
#if defined(OS_IS_APPLE)
	#ifdef BOOL
		#undef BOOL
	#endif
		#define BOOL int
#else
	typedef int						BOOL;
#endif

typedef int							INT;
typedef unsigned int				UINT;
typedef short						SHORT;
typedef unsigned short				USHORT;
typedef long						LONG;
typedef unsigned long				ULONG;

typedef char						CHAR;
typedef wchar_t						WCHAR;

typedef signed char					INT8;
typedef unsigned char				UINT8;
typedef signed short				INT16;
typedef unsigned short				UINT16;
typedef unsigned char				BYTE;
typedef unsigned short				WORD;

#if defined(WIN32)
	#if 64 == OPR_SYS_BITS
		typedef signed __int32		INT32;
		typedef unsigned __int32	UINT32;
	#else
		typedef signed int			INT32;
		typedef unsigned int		UINT32;
	#endif
	typedef signed __int64			INT64;
	typedef unsigned __int64		UINT64;
	#define I64CONST(x)				x
	#define U64CONST(x)				x
#else
	#if 32 == OPR_SYS_BITS || 64 == OPR_SYS_BITS
		typedef int					INT32;
		typedef unsigned int		UINT32;
	#else
		typedef signed long			INT32;
		typedef unsigned long		UINT32;
	#endif
	typedef signed long long		INT64;
	typedef unsigned long long		UINT64;
	#define I64CONST(x)				x##LL
	#define U64CONST(x)				x##LLU
#endif

#if defined(__linux__) && 64 == OPR_SYS_BITS
	typedef unsigned int			DWORD;
#else
	typedef unsigned long			DWORD;
#endif

#if 16 == OPR_SYS_BITS
	typedef short					INT_PTR;
	typedef unsigned short			UINT_PTR;
#elif 32 == OPR_SYS_BITS
	#if !defined(WIN32)
	typedef int					INT_PTR;
	typedef unsigned int		UINT_PTR;
	#endif
#elif 64 == OPR_SYS_BITS
	typedef INT64					INT_PTR;
	typedef UINT64					UINT_PTR;
#else
	#error Unsupported platform.
#endif

typedef void*						LPVOID;
typedef const void*					LPCVOID;

typedef char*						LPSTR;
typedef const char*					LPCSTR;
typedef WCHAR*						LPWSTR;
typedef const WCHAR*				LPCWSTR;

#if defined(_UNICODE) || defined(UNICODE)
	typedef WCHAR					TCHAR;
#else
	typedef char					TCHAR;
#endif
	typedef TCHAR*						LPTSTR;
	typedef const TCHAR*				LPCTSTR;

/* 句柄类型等 */
#if defined(__linux__)
	typedef void*					HANDLE;
	typedef void*					HINSTANCE;
	typedef void*					HMODULE;
	typedef void*					FARPROC;
	typedef void*					PROC;
	typedef int						OSFHANDLE;
#endif

#if defined(WIN32)
	typedef HANDLE					OSFHANDLE;
#endif

#if defined(_MSC_VER)
	typedef SOCKET FD_TYPE;
#else
	typedef int FD_TYPE;
	typedef int SOCKET;
	#ifndef INVALID_SOCKET
		#define INVALID_SOCKET	-1
	#endif
	#ifndef SOCKET_ERROR
		#define SOCKET_ERROR	-1
	#endif
#endif

#ifdef __cplusplus
	#define CHAR_TYPE_INLINE			template<typename CHAR_TYPE> static inline
	#define XCHAR_TYPE					CHAR_TYPE
#else
	#define CHAR_TYPE_INLINE			inline
	#define XCHAR_TYPE					TCHAR
#endif

#endif