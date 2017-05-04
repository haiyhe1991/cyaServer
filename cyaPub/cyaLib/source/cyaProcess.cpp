#include "cyaProcess.h"
#include "cyaFdk.h"
#include "cyaStrdk.h"
#include "cyaOS_opt.h"

// #if defined(__linux__)
// 	// pid = INVALID_OSPROCESSID,读取当前进程
// 	static int ReadProcStat_(OSProcessID pid, LinuxProcStat_* info)
// 	{
// 		char stat_file[MAX_PATH];
// 		if(INVALID_OSPROCESSID == pid)
// 			strcpy(stat_file, "/proc/self/stat");
// 		else
// 			sprintf(stat_file, "/proc/%d/stat", (int)pid);
// 		return ReadProcStatFile_(stat_file, info);
// 	}
// 	static int ReadThreadStat_(OSThreadRealID tid, LinuxProcStat_* info)
// 	{
// 		char stat_file[MAX_PATH];
// 		if(INVALID_OSTHREAD_REAL_ID == tid)
// 			tid = OSThreadIDToRealID(OSThreadSelf());
// 		sprintf(stat_file, "/proc/self/task/%d/stat", (int)(INT_PTR)tid);
// 		return ReadProcStatFile_(stat_file, info);
// 	}
// #endif

BOOL OSGetProcessId(LPCTSTR modFileName, OSProcessID& firstPID, BOOL isFullName, OSProcessID* allPIDs/* = NULL*/, int* pCount/* = NULL*/)
{
#if defined(WIN32)
	firstPID = INVALID_OSPROCESSID;
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	UINT i;

	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
		return false;

	cProcesses = cbNeeded / sizeof(DWORD);

	BOOL first = true;
	int count = 0;
	for (i = 0; i < cProcesses; i++)
	{
		DWORD processID = aProcesses[i];
		PathName szProcessName = _T("unknown");

		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, processID);
		BOOL ok = false;
		if (hProcess)
		{
			ok = (0 != GetModuleFileNameEx(hProcess, NULL, szProcessName, ARRAY_LEN(szProcessName)));
			VERIFY(CloseHandle(hProcess));
		}

		if (ok)
		{
			LPCTSTR s = isFullName ? szProcessName : GetFileName(szProcessName);
			if (0 == _tcsicmp(s, modFileName))
			{
				if (first)
				{
					firstPID = processID;
					first = false;
				}
				if (allPIDs)
					allPIDs[count++] = processID;
				else
					return true;
			}
		}
	}

	if (NULL != pCount)
		*pCount = count;

	return !first;
#elif defined(__linux__)
	int index = 0;
	BOOL isFirst = true;
	SCAN_FILE_FILTER scanfilter;
	scanfilter.count = scandir("/proc/", &scanfilter.nameList, SCAN_FILE_FILTER::scandir_filter_, alphasort);
	int count = scanfilter.count;
	struct dirent** nameList = scanfilter.nameList;
	if (count <= 0 || nameList == NULL)
		return false;

	for (int i = 0; i < count; ++i)
	{
		if (!IsStrNum(nameList[i]->d_name))
			continue;

		OSProcessID pid = (OSProcessID)atoi(nameList[i]->d_name);
		char linkPath[128] = { 0 };
		sprintf(linkPath, "/proc/%d/exe", pid);
		if (!IsExistFile(linkPath))
			continue;
		char pathname[MAX_PATH] = { 0 };
		int r = readlink(linkPath, pathname, MAX_PATH);
		if (r <= 0)
			continue;
		pathname[r] = '\0';

		if (isFullName)
		{
			if (strcmp(pathname, modFileName) != 0)
				continue;
		}
		else
		{
			const char* p = GetFileName(pathname);
			if (strcmp(p, modFileName) != 0)
				continue;
		}

		if (isFirst)
		{
			firstPID = pid;
			isFirst = false;
		}
		if (allPIDs)
			allPIDs[index++] = pid;
		else
			break;
	}
	if (NULL != pCount)
		*pCount = index;
	return !isFirst;
#endif
}

#if defined(WIN32)
void RemoveINT3_In_NTDLL()
{
	void* addr = GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "DbgBreakPoint");
	if (NULL == addr)
		return;
	if (0xCC != *(BYTE*)addr) // int 3指令的机器码为0xCC
		return;

	__try
	{
		BYTE nop = 0x90; // nop指令的机器码为0x90
		SIZE_T written;
		HANDLE h = GetCurrentProcess();
		if (WriteProcessMemory(h, addr, &nop, 1, &written) && 1 == written)
			FlushInstructionCache(h, addr, 1);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		// 忽略所有异常
	}
}
#endif

#if defined(WIN32)
UINT64 OSGetProcessMemory(OSProcessID pid)
{
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
	if (hProcess == NULL)
		return 0;

	PROCESS_MEMORY_COUNTERS mem;
	BOOL bRet = GetProcessMemoryInfo(hProcess, &mem, sizeof(PROCESS_MEMORY_COUNTERS));
	CloseHandle(hProcess);
	return bRet ? mem.WorkingSetSize : 0;
}

UINT64 OSGetProcessMemory(LPCTSTR procName, BOOL isFullName/* = false*/)
{
	OSProcessID pid = INVALID_OSPROCESSID;
	if (!OSGetProcessId(procName, pid, isFullName))
		return 0;
	return OSGetProcessMemory(pid);
}
#else
UINT64 OSGetProcessMemory(OSProcessID pid)
{
	char szProcFile[MAX_PATH] = { 0 };
	sprintf(szProcFile, "/proc/%d/status", pid);
	char szReadBuf[64 * 1024] = { 0 };
	if (!ReadLinuxProcFile(szProcFile, szReadBuf, sizeof(szReadBuf) - sizeof(char)))
		return 0;

	UINT64 n64Mem = 0;
	const char* sep = strstr(szReadBuf, "VmRSS:");
	if (NULL != sep)
	{
		sep += strlen("VmRSS:");
		while (*sep == '\x20' || *sep == '\t')
			sep++;
		n64Mem = _atoi64(sep);
	}
	return n64Mem * 1024;
}

UINT64 OSGetProcessMemory(LPCTSTR procName, BOOL isFullName/* = false*/)
{
	OSProcessID pid = INVALID_OSPROCESSID;
	if (!OSGetProcessId(procName, pid, isFullName))
		return 0;

	return OSGetProcessMemory(pid);
}
#endif

