#include "moduleInfo.h"

#if defined(WIN32)
BOOL IsValidModule(HMODULE h)
{
	if (NULL == h)
		return false;

	DWORD lasterr = GetLastError();
	SetLastError(NOERROR);
	GetProcAddress(h, "a");
	DWORD err = GetLastError();
	SetLastError(lasterr);
	return ERROR_MOD_NOT_FOUND != err;
}

#if (32 == OPR_SYS_BITS)
__declspec(naked) int GetOffset_This_I()
{
	//STATIC_ASSERT(sizeof(int) == 4);
	__asm
	{
		pop		eax
			push	eax
			sub		eax, 5
			ret
	}
}

__declspec(naked) int GetOffset_Next_I()
{
	//STATIC_ASSERT(sizeof(int) == 4);
	__asm
	{
		pop		eax
			push	eax
			add		eax, 3
			ret
	}
}
#endif

LPVOID AlloctionBaseFromAddress(LPVOID lpv)
{
	MEMORY_BASIC_INFORMATION mbi;
	memset(&mbi, 0, sizeof(MEMORY_BASIC_INFORMATION));
	if (0 == VirtualQuery(lpv, &mbi, sizeof(mbi)))
		return NULL;

	return mbi.AllocationBase;
}

HMODULE ModuleFromAddress(LPVOID lpv)
{
	LPVOID base = AlloctionBaseFromAddress(lpv);
	if (NULL == base)
		return NULL;

	HMODULE module = (HMODULE)base;
	if (!IsValidModule(module))
		return NULL;

	return module;
}
#endif // #if defined(WIN32)

LPCTSTR GetExePathName()
{
	static LONG lock = 0;
	static BOOL inited = false;
	static PathName sExeName;

	if (inited)
		return sExeName;
	CAutoSpinLock locker(lock);
	if (inited)
		return sExeName;
#if defined(WIN32)
	GetModuleFileName(GetModuleHandle(NULL), sExeName, ARRAY_LEN(sExeName));
#elif defined(__linux__)
	char buf[MAX_PATH];
	int r = readlink("/proc/self/exe", buf, MAX_PATH);
	buf[r >= 0 ? r : 0] = '\0';
	strcpy(sExeName, buf);
#endif
	inited = true;
	return sExeName;
}

LPCTSTR GetExeDir()
{
	static LONG lock = 0;
	static BOOL inited = false;
	static PathName sExeDir;

	if (inited)
		return sExeDir;
	CAutoSpinLock locker(lock);
	if (inited)
		return sExeDir;

	_tcscpy(sExeDir, GetExePathName());
	for (int i = (int)_tcslen(sExeDir) - 1; i >= 0; --i)
	{
		if (sExeDir[i] == OS_DIR_SEP_CHAR)
		{
			sExeDir[i + 1] = 0;
			break;
		}
	}
	inited = true;
	return sExeDir;
}

LPCTSTR GetExePath()
{
	return GetExeDir();
}