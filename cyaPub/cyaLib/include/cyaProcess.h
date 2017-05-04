#ifndef __CYA_PROCESS_H__
#define __CYA_PROCESS_H__

#if defined(_MSC_VER) && (_MSC_VER > 1000)
	#pragma once
#endif

#include "cyaTypes.h"
#include "cyaFdk.h"

#if defined(WIN32)
	#include <process.h>
	#include <psapi.h>
	#include "win_toolhelp.h"
	#pragma comment(lib, "psapi.lib")
#elif defined(__linux__)
	#include <unistd.h>
	#include <dirent.h>
	#include <sys/types.h>
	#include <sys/wait.h>

	#if defined(OS_IS_ANDROID)
		#include <sys/socket.h>
	#endif
#endif	// WIN32 __linux__

#if defined(__linux__)
	/* ��ȡ��ǰ���̵����д򿪵�fd
	*  @pfds[out] ���ڷ��ؾ���������صľ����>0����Ҫʹ��free()�ͷŴ�ž������ʱ�ڴ�
	*  @return <0 - ��������>=0 - �������
	*  ���ӣ�
	* 		int* pfds = NULL;
	* 		int n = GetPrcessAllFDs(&pfds);
	* 		if(n > 0)
	* 		{
	* 			// ������...
	* 			free(pfds);
	* 		}
	*/
	int GetPrcessAllFDs(OUT int** pfds);
#endif

#if defined(WIN32)
	OSProcessID OSCreateProcess(LPCTSTR lpszCmdLine, BOOL newConsoleWindow = true, BOOL hideConsole = false);
#elif defined(__linux__)
	OSProcessID OSCreateProcess(const char* cmdLine, BOOL newConsoleWindow = false, BOOL hideConsole = false);
#endif

BOOL OSWaitProcess(OSProcessID pid, BOOL bBlock);
BOOL OSFindProcess(OSProcessID pid); /* equal to OSWaitProcess(pid, false) */
void OSCloseProcess(OSProcessID pid);
OSProcessID OSGetCurrentProcessID();
void OSKillProcess(OSProcessID pid);
OSProcessID OSGetParentProcessID();

BOOL OSGetProcessId(LPCTSTR modFileName, OSProcessID& firstPID,
					BOOL isFullName, OSProcessID* allPIDs = NULL, int* pCount = NULL);

UINT64 OSGetProcessMemory(OSProcessID pid);
UINT64 OSGetProcessMemory(LPCTSTR procName, BOOL isFullName = false);

#if defined(WIN32)
	/// ȥ��ntdll.dll�е�int 3����ϵ�
	void RemoveINT3_In_NTDLL();
#endif

/***********************************��������ʵ��************************************/

inline void OSKillProcess(OSProcessID pid)
{
	OSCloseProcess(pid);
}

inline OSProcessID OSGetParentProcessID()
{
#if defined(WIN32)
	CToolhelp th(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe = { sizeof(pe) };
	VERIFY(th.ProcessFind(GetCurrentProcessId(), &pe));
	return pe.th32ParentProcessID;
#else
	return getppid();
#endif
}

#if defined(WIN32)

inline OSProcessID OSCreateProcess(LPCTSTR lpszCmdLine, BOOL /*newConsoleWindow*/ /*= false*/, BOOL hideConsole/* = false*/)
{
	PathName szCmdLine = { 0 };
	_tcscpy(szCmdLine, lpszCmdLine);

	PROCESS_INFORMATION processInfo;
	STARTUPINFO startupInfo;
	ZeroStru(startupInfo);
	startupInfo.cb = sizeof(startupInfo);

	DWORD dwFlags = CREATE_DEFAULT_ERROR_MODE;
	dwFlags |= hideConsole ? CREATE_NO_WINDOW : CREATE_NEW_CONSOLE;

	BOOL bRet = CreateProcess(NULL, szCmdLine, NULL, NULL, false, dwFlags,
		NULL, NULL, &startupInfo, &processInfo);
	if (!bRet)
		return INVALID_OSPROCESSID;

	CloseHandle(processInfo.hProcess);
	CloseHandle(processInfo.hThread);
	return processInfo.dwProcessId;
}

inline BOOL OSFindProcess(OSProcessID pid)
{
	BOOL bFind = false;
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
	if (hProcess)
	{
		DWORD dwRetval = 0;
		if (GetExitCodeProcess(hProcess, &dwRetval))
		{
			if (dwRetval == STILL_ACTIVE)
				bFind = true;
		}
		CloseHandle(hProcess);
	}
	return bFind;
}

inline void OSCloseProcess(OSProcessID pid)
{
	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
	if (NULL == hProcess)
		return;
	TerminateProcess(hProcess, 0);
	CloseHandle(hProcess);
}

inline BOOL OSWaitProcess(OSProcessID pid, BOOL bBlock)
{
	DWORD stat = 0;
	BOOL bRet = false;
	DWORD time = (bBlock ? INFINITE : 0);

	HANDLE hProcess = OpenProcess(SYNCHRONIZE, FALSE, pid);
	if (NULL == hProcess)
		return false;

	if (WAIT_OBJECT_0 == (stat = WaitForSingleObject(hProcess, time)))
		bRet = false;
	else if (stat == WAIT_TIMEOUT)
		bRet = true;

	CloseHandle(hProcess);
	return bRet;
}

inline OSProcessID OSGetCurrentProcessID()
{
	return GetCurrentProcessId();
}

#elif defined(__linux__)

static int GPFD_scandir_filter_(const struct dirent* nl)
{
	if (0 == strcmp(nl->d_name, ".") || 0 == strcmp(nl->d_name, ".."))
		return 0;
	return 1;
}

inline int GetPrcessAllFDs(int** pfds)
{
	struct dirent** nameList = NULL;
	int count;

	if (NULL == pfds)
	{
		ASSERT(false);
		return -1;
	}
	*pfds = NULL;

	count = scandir("/proc/self/fd", &nameList, GPFD_scandir_filter_, alphasort);
	if (count < 0)
	{
		ASSERT(NULL == nameList);
		return -1;
	}
	if (nameList)
	{
		if (count > 0)
			*pfds = (int*)malloc(sizeof(int)*count);
		for (int i = 0; i<count; ++i)
		{
			(*pfds)[i] = atoi(nameList[i]->d_name);
			::free(nameList[i]);
		}
		::free(nameList);
	}
	return count;
}

static inline char** ParseArgv__(const char* cmdLine)
{
	char* prog = (char*)malloc(strlen(cmdLine) + 1);
	strcpy(prog, cmdLine);
	int cn = 1;
	char* cmd = prog;
	while (NULL != (cmd = strchr(cmd, ' ')))
	{
		*cmd = '\0';
		cmd++;
		while (*cmd == ' ')
			cmd++;
		cn++;
	}

	char** ret = (char**)malloc((cn + 1)*sizeof(char*));

	cmd = prog;
	for (int i = 0; i<cn; ++i)
	{
		ret[i] = cmd;
		cmd += (strlen(cmd) + 1);
		while (*cmd == ' ')
			cmd++;
	}

	ret[cn] = NULL;
	return ret;
}

inline OSProcessID OSCreateProcess(const char* cmdLine, BOOL /*newConsoleWindow*/, BOOL /*hileConsole*/)
{
	OSProcessID pid = fork();
	if (0 == pid)
	{
		/* �˴������Ѿ������ӽ��� */
		int* pfds = NULL;
		int n;
		char** cargv = ParseArgv__(cmdLine);

		/* ִ��execv()֮ǰ�ȹرս��̵�����socket */
		n = GetPrcessAllFDs(&pfds);
		if (n > 0)
		{
			for (int i = 0; i<n; ++i)
			{
				int sockType;
				int sockTypeLen = sizeof(sockType);
				BOOL isSocket = (SOCKET_ERROR != getsockopt((SOCKET)(pfds[i]), SOL_SOCKET, SO_TYPE, (char*)&sockType, (socklen_t*)&sockTypeLen));
				if (isSocket)
					close(pfds[i]);
			}
			free(pfds);
		}

		execv(cargv[0], cargv);
		free(cargv[0]);
		free(cargv);
		exit(0);
	}
	else if (pid > 0)
	{
		return pid;
	}
	return INVALID_OSPROCESSID;
}

inline BOOL OSWaitProcess(OSProcessID pid, BOOL bBlock)
{
	int options = (bBlock ? 0 : WNOHANG);
	int r = waitpid(pid, NULL, options);
	return (0 == r);
}

inline BOOL OSFindProcess(OSProcessID pid)
{
	return (0 == waitpid(pid, NULL, WNOHANG));
}

inline void OSCloseProcess(OSProcessID pid)
{
	kill(pid, SIGKILL);
}

inline OSProcessID OSGetCurrentProcessID()
{
	static OSProcessID pid = INVALID_OSPROCESSID;
	if (INVALID_OSPROCESSID != pid)
		return pid;
	char buf[MAX_PATH];
	int l = readlink("/proc/self", buf, MAX_PATH);
	if (l <= 0)
	{
		ASSERT(false);
		return INVALID_OSPROCESSID;
	}
	buf[l] = '\0';
	const char* sep = strrchr(buf, '/');
	if (sep)
		pid = (OSProcessID)atoi(sep + 1);
	else
		pid = (OSProcessID)atoi(buf);
	return pid;
}

#endif /* #if defined(__linux__) */

#endif