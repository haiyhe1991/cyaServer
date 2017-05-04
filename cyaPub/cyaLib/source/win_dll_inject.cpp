#if defined(WIN32)

#include "win_dll_inject.h"
#include "cyaFdk.h"
#include "cyaProcess.h"
#include "cyaStrCodec.h"

#ifndef _INC_TOOLHELP32
	#include <tlhelp32.h>
#endif

//VC6下是<ImageHlp.h> VC7之后有#include <DbgHelp.h>
#ifndef _DBGHELP_
	#include <ImageHlp.h>
	#pragma comment(lib, "ImageHlp.lib")
	#define _DBGHELP_
#endif

#if defined(_MSC_VER) && (_MSC_VER > 1200) // > VC6.0
	#pragma comment(lib, "Dbghelp.lib")
#endif

static BOOL InjectDllW(DWORD dwProcessId, PCWSTR pszLibFile)
{
	BOOL ok = false;
	HANDLE hProcess = NULL;
	HANDLE hThread = NULL;
	PWSTR pszLibFileRemote = NULL;

	__try
	{
		hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, false, dwProcessId);
		if (NULL == hProcess)
			__leave;

		int cch = 1 + lstrlenW(pszLibFile);
		int cb = cch * sizeof(WCHAR);

		pszLibFileRemote = (PWSTR)VirtualAllocEx(hProcess, NULL, cb, MEM_COMMIT, PAGE_READWRITE);

		if (NULL == pszLibFileRemote)
			__leave;

		if (!WriteProcessMemory(hProcess, pszLibFileRemote, (PVOID)pszLibFile, cb, NULL))
			__leave;

		HMODULE h = GetModuleHandleW(L"Kernel32");
		if (NULL == h)
			__leave;

		PTHREAD_START_ROUTINE pfnThreadRtn = (PTHREAD_START_ROUTINE)GetProcAddress(h, "LoadLibraryW");
		if (NULL == pfnThreadRtn)
			__leave;

		hThread = CreateRemoteThread(hProcess, 0, NULL, pfnThreadRtn, pszLibFileRemote, 0, NULL);
		if (NULL == hThread)
			__leave;
		WaitForSingleObject(hThread, INFINITE);

		ok = true;
	}
	__finally
	{
		if (pszLibFileRemote)
			VirtualFreeEx(hProcess, pszLibFileRemote, 0, MEM_RELEASE);
		if (hThread)
			VERIFY(CloseHandle(hThread));
		if (hProcess)
			VERIFY(CloseHandle(hProcess));
	}

	return ok;
}

static BOOL EjectDllW(DWORD dwProcessId, PCWSTR pszLibFile)
{
	BOOL ok = false;
	HANDLE hthSnapshot = NULL;
	HANDLE hProcess = NULL;
	HANDLE hThread = NULL;

	__try
	{
		hthSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);
		if (NULL == hthSnapshot)
			__leave;

		MODULEENTRY32W me = { sizeof(me) };
		BOOL found = false;
		BOOL fMoreMods = Module32FirstW(hthSnapshot, &me);
		for (; fMoreMods; fMoreMods = Module32NextW(hthSnapshot, &me))
		{
			found = (0 == lstrcmpiW(me.szModule, pszLibFile))
				|| (0 == lstrcmpiW(me.szExePath, pszLibFile));
			if (found)
				break;
		}
		if (!found)
			__leave;

		hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION, false, dwProcessId);
		if (NULL == hProcess)
			__leave;

		HMODULE h = GetModuleHandleW(L"Kernel32");
		if (NULL == h)
			__leave;

		PTHREAD_START_ROUTINE pfnThreadRtn = (PTHREAD_START_ROUTINE)GetProcAddress(h, "FreeLibrary");

		if (NULL == pfnThreadRtn)
			__leave;

		hThread = CreateRemoteThread(hProcess, NULL, 0, pfnThreadRtn, me.modBaseAddr, 0, NULL);
		if (NULL == hThread)
			__leave;
		WaitForSingleObject(hThread, INFINITE);

		ok = true;
	}
	__finally
	{
		if (hthSnapshot)
			VERIFY(CloseHandle(hthSnapshot));
		if (hThread)
			VERIFY(CloseHandle(hThread));
		if (hProcess)
			VERIFY(CloseHandle(hProcess));
	}

	return ok;
}

//////////////////////////////////////////////////////////////////////////

BOOL InjectDLL(DWORD dwProcessId, LPCTSTR pszLibFile)
{
	if (!IsExistFile(pszLibFile))
		return false;

	WCHAR wszLibFile[MAX_PATH] = { 0 };
	tctowc(pszLibFile, wszLibFile, sizeof(wszLibFile));
	return InjectDllW(dwProcessId, wszLibFile);
}

BOOL EjectDLL(DWORD dwProcessId, LPCTSTR pszLibFile)
{
	if (!IsExistFile(pszLibFile))
		return false;

	WCHAR wszLibFile[MAX_PATH] = { 0 };
	tctowc(pszLibFile, wszLibFile, sizeof(wszLibFile));
	return EjectDllW(dwProcessId, wszLibFile);
}

BOOL InjectDLL(LPCTSTR pszProcessExeName, LPCTSTR pszLibFile, BOOL isFullProcessName, BOOL onlyFirstProcess)
{
	DWORD processID;
	DWORD ar[1024] = { 0 };
	int count = 0;
	if (!OSGetProcessId(pszProcessExeName, processID, isFullProcessName, ar, &count))
		return false;

	if (onlyFirstProcess)
		return InjectDLL(processID, pszLibFile);

	for (int i = 0; i < count; ++i)
	{
		if (!InjectDLL(ar[i], pszLibFile))
			return false;
	}

	return true;
}

BOOL EjectDLL(LPCTSTR pszProcessExeName, LPCTSTR pszLibFile, BOOL isFullProcessName, BOOL onlyFirstProcess)
{
	DWORD processID;
	DWORD ar[1024] = { 0 };
	int count = 0;
	if (!OSGetProcessId(pszProcessExeName, processID, isFullProcessName, ar, &count))
		return false;

	if (onlyFirstProcess)
		return EjectDLL(processID, pszLibFile);

	for (int i = 0; i < count; ++i)
	{
		if (!EjectDLL(ar[i], pszLibFile))
			return false;
	}

	return true;
}

BOOL CreateProcessWithDLL(LPCTSTR cmdLine, LPCTSTR injectDLL /*= NULL*/, DWORD dwPriorityClass /*= NORMAL_PRIORITY_CLASS*/)
{
	STARTUPINFO startupInfo;
	GetStartupInfo(&startupInfo);
	PROCESS_INFORMATION processInfo;

	if (!CreateProcess(NULL,
		(LPTSTR)cmdLine,
		NULL,
		NULL,
		false,
		CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED,
		NULL,
		NULL,
		&startupInfo,
		&processInfo))
	{
		return false;
	}

	if (injectDLL)
		VERIFY(InjectDLL(processInfo.dwProcessId, injectDLL));

	if (NORMAL_PRIORITY_CLASS != dwPriorityClass)
		VERIFY(SetPriorityClass(processInfo.hProcess, dwPriorityClass));
	VERIFY(1 == ResumeThread(processInfo.hThread));

	VERIFY(CloseHandle(processInfo.hProcess));
	VERIFY(CloseHandle(processInfo.hThread));
	return true;
}

#endif