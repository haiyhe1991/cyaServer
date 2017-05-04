#ifndef __CYA_LOG_H__
#define __CYA_LOG_H__

#if defined(_MSC_VER) && (_MSC_VER > 1000)
	#pragma once
#endif

#include <stdarg.h>
#include "cyaTypes.h"

/*
编译说明：
【WIN32工程】
1、如果把CYALOG_DECL描述的函数/类加入到工程一起编译使用（不使用dll），则需要在使用工程里定义全局宏CYALOG_DECL。
2、如果把CYALOG_DECL描述的函数/类放到某个dll里面导出使用，则在dll工程里面需要定义全局宏CYALOG_EXPORTS，使用dll的工程不需要定义任何宏。
【Linux工程】
所有情况都不需要定义任何全局宏

导出C函数可以使用CYALOG_API或CYALOG_DECL修饰，
导出C++类必须使用CYALOG_DECL修饰
*/
#if defined(CYALOG_DECL)
	#undef CYALOG_DECL
	#define CYALOG_DECL
#endif

#ifndef CYALOG_DECL
	#ifdef WIN32
		#ifdef CYALOG_EXPORTS
			#define CYALOG_DECL __declspec(dllexport)
		#else
			#define CYALOG_DECL __declspec(dllimport)
		#endif
	#else
		#if defined(__GNUC__) && __GNUC__ >= 4
			#define CYALOG_DECL __attribute__((visibility("default")))
		#else
			#define CYALOG_DECL
		#endif
	#endif
#endif

#ifdef __cplusplus
	#define CYALOG_API extern "C" CYALOG_DECL
#else
	#define CYALOG_API CYALOG_DECL
#endif

// 日志类型
#define LOG_INFO				0
#define LOG_WARNING				1
#define LOG_ERROR				2
#define LOG_DEBUG				3

#define LOG_FILE_LINE_INFO		31	// 打印源文件名，行号

// 日志输出开关码
#define LOG_ALL_MASK				0xFFFFFFFF
#define LOG_NONE_MASK				0
#define LOG_INFO_MASK				(1 << LOG_INFO)
#define LOG_WARNING_MASK			(1 << LOG_WARNING)
#define LOG_ERROR_MASK				(1 << LOG_ERROR)
#define LOG_DEBUG_MASK				(1 << LOG_DEBUG)


// 设置日志的目录，默认为日志模块(可能是动态库，EXE, OCX...)的目录，如果dir为空表示恢复默认值
// 注意：需要任何其他日志函数调用前设置此参数
CYALOG_API BOOL CyaLogSetDir(LPCTSTR dir);
CYALOG_API LPCTSTR CyaLogGetDir(); // 使用CyaLogGetDir()的返回值期间，不得再调用CyaLogSetDir()，否则返回的指针可能无效

// 设置每n行flush一次文件，默认32行，至少1行
CYALOG_API void CyaLogSetFlushLines(int n);
CYALOG_API int CyaLogGetFlushLines();

// 设置单个日志文件大小(字节)，默认4M
CYALOG_API void CyaLogSetFileSizeLimit(int bytes);
CYALOG_API int CyaLogGetFileSizeLimit();

// 是否打印到屏幕开关，默认true
CYALOG_API void CyaLogSetOutScreen(BOOL onoff);
CYALOG_API BOOL CyaLogIsOutScreen();

// 是否将日至写入磁盘文件，默认true
CYALOG_API void CyaLogSetToFile(BOOL onoff);
CYALOG_API BOOL CyaLogIsToFile();


// 设置日志文件生命期，默认为7天，0表示保留到磁盘满【参见void CyaLogSetReserve(int MBytes)】
CYALOG_API void CyaLogSetLife2(int secs);
CYALOG_API int CyaLogGetLife2();
CYALOG_API void CyaLogSetLife(int days);
CYALOG_API int CyaLogGetLife();

// 设置日志标题，logType = LOG_INFO、LOG_WARNING...
CYALOG_API LPCTSTR CyaLogGetEventTitle(int logType);
CYALOG_API void CyaLogSetEventTitle(int logType, LPCTSTR title);

// 得到正在使用的日志文件名，失败返回""
CYALOG_API LPCTSTR CyaLogGetFileName();

//打印日志
CYALOG_API void CyaLogEventX(int logType, LPCTSTR fmt, va_list va);

//  flush 日志文件，把打印的所有日志立即写到磁盘
CYALOG_API void CyaLogFlushFile();

// 设置日志目录的保留空间，达到保留空间后，将会删除最旧的日志文件(0表示禁止该功能)
// 单位：兆(MB)
// 介质类型： 0 -- 硬盘  1 -- 内存
// 默认为1024M，最小值64M
CYALOG_API void CyaLogSetReserve(int MBytes, int mediumType = 0);
CYALOG_API int CyaLogGetReserve(int* mediumType = NULL);

// 设置日志文件的最大个数. 0表示禁止该功能
CYALOG_API void CyaLogSetMaxFileNum(int maxFileNum);
CYALOG_API int CyaLogGetMaxFileNum();

// 是否输出到调试器（windows平台有效），DEBUG版默认打开，RELEASE版默认关闭
CYALOG_API void CyaLogOutputToDebuger(BOOL en);
CYALOG_API BOOL CyaLogIsOutputDebuger();


#define IMPL_LogEvent_(logType)							\
	inline void LogEvent_##logType(const TCHAR* fmt, ...)		\
	{															\
		va_list va;												\
		va_start(va, fmt);										\
		::CyaLogEventX(logType, fmt, va);						\
		va_end(va);												\
	}

IMPL_LogEvent_(LOG_INFO)
IMPL_LogEvent_(LOG_WARNING)
IMPL_LogEvent_(LOG_ERROR)
IMPL_LogEvent_(LOG_DEBUG)

#undef IMPL_LogEvent_

#undef LogEvent
#undef LogPrint
#undef LogInfo
#undef LogWarning
#undef LogError
#undef LogDebug

#define LogEvent(logType, x__)		\
	do	{	LogEvent_##logType x__;		} while(false)

#define LogPrint(logType, x__)		LogEvent(logType, x__)
#define LogInfo(x__)				LogEvent(LOG_INFO, x__)
#define LogWarning(x__)				LogEvent(LOG_WARNING, x__)
#define LogError(x__)				LogEvent(LOG_ERROR, x__)

#define LogDebug(x__)				LogEvent(LOG_DEBUG, x__)
#endif