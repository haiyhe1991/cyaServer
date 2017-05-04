#ifndef __CYA_DLL_H__
#define __CYA_DLL_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/* linux平台的接口完全同WIN32 */
#if defined(__linux__)
#include <dlfcn.h>
#include "cyaTypes.h"

#ifdef OS_IS_MIPS_ARM_LINUX
	#define NPR_RTLD_NOLOAD		0
#else
	#define NPR_RTLD_NOLOAD		RTLD_NOLOAD
#endif

inline HMODULE GetModuleHandle(const char* lpLibFileName)
{
	return dlopen(lpLibFileName, RTLD_LAZY | NPR_RTLD_NOLOAD);
}


inline HMODULE LoadLibrary(const char* lpLibFileName)
{
	return dlopen(lpLibFileName, RTLD_LAZY);
}

inline HMODULE LoadLibraryEx(const char* lpLibFileName, HANDLE h/* not use */, DWORD flag/* not use */)
{
	return dlopen(lpLibFileName, RTLD_LAZY);
}

inline BOOL FreeLibrary(HMODULE hLibModule)
{
	return 0 == dlclose(hLibModule);
}

inline FARPROC GetProcAddress(HMODULE hModule, const char* lpProcName)
{
	return dlsym(hModule, lpProcName);
}

#endif /* __linux__ */

#endif