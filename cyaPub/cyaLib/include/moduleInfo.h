#ifndef __MODULE_INFO_H__
#define __MODULE_INFO_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
#pragma once
#endif

#if defined(WIN32) && !defined(_WINDOWS_)
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#endif
#include <windows.h>
#endif

#if defined(__linux__) && !defined(OS_IS_APPLE)
#include <link.h>
#endif

#include "cyaFdk.h"
#include "cyaLocker.h"
#include "cyaString.h"

#if defined(WIN32)
BOOL IsValidModule(HMODULE h);

#if (32==OPR_SYS_BITS)
// ���ص��ñ�������ָ��ƫ�Ƶ�ַ
int GetOffset_This_I();
// ���ص��ñ�������ָ�����һ��ָ���ƫ�Ƶ�ַ
int GetOffset_Next_I();
#endif
// ���ݵ�ַ�õ���������ҳ����ַ
LPVOID AlloctionBaseFromAddress(LPVOID lpv);

// ����ָ���ַ�õ�������ģ��HMODULE
HMODULE ModuleFromAddress(LPVOID lpv);
#endif

// �õ����ý��̳����ļ�(.exe)��Ϣ
#if defined(WIN32)
HMODULE GetExeModule();
#endif

#if defined(WIN32)
inline HMODULE GetExeModule()
{
	return GetModuleHandle(NULL);
}

inline HMODULE GetThisModule()
{
#if (32==OPR_SYS_BITS)
	LPVOID thisAddr = (LPVOID)GetOffset_This_I();
	return (HMODULE)AlloctionBaseFromAddress(thisAddr);
#else
	LPVOID thisAddr = (LPVOID)GetThisModule;
	return (HMODULE)AlloctionBaseFromAddress(thisAddr);
#endif
}
#endif

// ���ִ���ļ�ȫ·����
LPCTSTR GetExePathName();
LPCTSTR GetExeDir();
LPCTSTR GetExePath();

// �õ���ǰģ��(.exe��.dll��.ocx...)��Ϣ
#if defined(WIN32)
	HMODULE GetThisModule();
#endif

LPCTSTR GetThisModulePathName();
LPCTSTR GetThisModuleDir();
LPCTSTR GetThisModulePath();
LPCTSTR GetThisModuleName();

static LPCTSTR GetThisModuleFileName(TCHAR* szBuf, int bufLen)
{
	if (NULL == szBuf || bufLen <= 0)
	{
		ASSERT(false);
		return NULL;
	}

#if defined(WIN32)
	HMODULE module = GetThisModule();
	if (NULL == module)
		return NULL;
	if (0 == GetModuleFileName(module, szBuf, bufLen))
		return NULL;
	return szBuf;
#elif defined(OS_IS_ANDROID)
	return szBuf;
#elif defined(__linux__)
	Dl_info dlinfo;
	dladdr((const void*)GetThisModuleFileName, &dlinfo);
	if (bufLen <= (int)strlen(dlinfo.dli_fname))
	{
		ASSERT(false);
		return NULL;
	}
	char buf[MAX_PATH] = { 0 };
	if (strchr(dlinfo.dli_fname, '/') == NULL)
		sprintf(buf, "./%s", dlinfo.dli_fname);
	else
		strcpy(buf, dlinfo.dli_fname);
	return szBuf;
#endif
}

inline LPCTSTR GetThisModulePathName()
{
	static LONG lock = 0;
	static BOOL inited = false;
	static PathName sPathName;

	if (inited)
		return sPathName;
	CAutoSpinLock locker(lock);
	if (inited)
		return sPathName;

	VERIFY(GetThisModuleFileName(sPathName, ARRAY_LEN(sPathName)));
	inited = true;
	return sPathName;
}

inline LPCTSTR GetThisModuleDir()
{
	static LONG lock = 0;
	static BOOL inited = false;
	static PathName sDir;

	if (inited)
		return sDir;
	CAutoSpinLock locker(lock);
	if (inited)
		return sDir;

	_tcscpy(sDir, GetThisModulePathName());
	for (int i = (int)_tcslen(sDir) - 1; i >= 0; --i)
	{
		if (sDir[i] == OS_DIR_SEP_CHAR)
		{
			sDir[i + 1] = 0;
			break;
		}
	}
	inited = true;
	return sDir;
}

inline LPCTSTR GetThisModulePath()
{
	return GetThisModuleDir();
}

inline LPCTSTR GetThisModuleName()
{
	static LONG lock = 0;
	static LPCTSTR sName = NULL;

	if (sName)
		return sName;
	CAutoSpinLock locker(lock);
	if (sName)
		return sName;

	LPCTSTR pathName = GetThisModulePathName();
	for (int i = (int)_tcslen(pathName) - 1; i >= 0; --i)
	{
		if (pathName[i] == OS_DIR_SEP_CHAR)
		{
			sName = pathName + i + 1;
			break;
		}
	}
	return sName;
}

#endif