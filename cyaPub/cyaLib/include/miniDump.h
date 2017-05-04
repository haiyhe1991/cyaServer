#ifndef __MINI_DUMP_H__
#define __MINI_DUMP_H__

#include "cyaBase.h"
#if defined(__linux__)
	#include <sys/resource.h>
#endif
#include "moduleInfo.h"

#if defined(WIN32)
#if defined(_MSC_VER) && _MSC_VER > 1200
	#include <DbgHelp.h>
	#pragma comment(lib, "Dbghelp.lib")
#else
	#include <ImageHlp.h>
	#pragma comment(lib, "ImageHlp.lib")
#endif

static LONG WINAPI GenerateDumpFile(struct _EXCEPTION_POINTERS* pExceptionPointers)
{
	LONG ret = EXCEPTION_EXECUTE_HANDLER;
	BOOL bMiniDumpSuccessful;
	TCHAR szFileName[MAX_PATH] = { 0 };

	HANDLE hDumpFile;
	SYSTEMTIME stLocalTime;
	MINIDUMP_EXCEPTION_INFORMATION expParam;

	GetLocalTime(&stLocalTime);
	_stprintf(szFileName, _T("%s%04d%02d%02d%02d%02d%02d.dump"), GetExeDir(),
		stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
		stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond);

	hDumpFile = CreateFile(szFileName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_WRITE | FILE_SHARE_READ,
		0, CREATE_ALWAYS, 0, 0);
	if (INVALID_HANDLE_VALUE == hDumpFile)
		return ret;

	expParam.ThreadId = GetCurrentThreadId();
	expParam.ExceptionPointers = pExceptionPointers;
	expParam.ClientPointers = TRUE;
	bMiniDumpSuccessful = MiniDumpWriteDump(GetCurrentProcess(),
		GetCurrentProcessId(),
		hDumpFile, MiniDumpNormal,
		&expParam, NULL, NULL);

	ASSERT(bMiniDumpSuccessful);
	return ret;
}

void SaveExceptionDumpFile()
{
	::SetUnhandledExceptionFilter(GenerateDumpFile);
}
#elif defined(__linux__) /*&& !defined(OS_IS_APPLE) && !defined(OS_IS_ANDROID)*/
BOOL SaveExceptionDumpFile(INT64 maxBytes = 1024 * 1024 * 64)
{
	struct rlimit rl;
	rl.rlim_cur = (rlim_t)maxBytes;
	rl.rlim_max = (rlim_t)maxBytes;
	return 0 == setrlimit(RLIMIT_CORE, &rl) ? true : false;	//非root权限可能执行失败
}
#endif

#endif