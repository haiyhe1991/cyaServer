#include "cyaOS_opt.h"
#include "cyaString.h"
#include "cyaMaxMin.h"
#include "cyaFdk.h"

#if defined(WIN32)
	#include "win_regedit.h"
#endif

#if defined(__linux__)
	#if defined(OS_IS_ANDROID)
		#include <sys/statfs.h>
		#include <sys/stat.h>
	#else
		#include <sys/statvfs.h>
	#endif
	#include <sys/reboot.h>
	#include <sys/resource.h>
#endif	//__linux__

#if defined(WIN32)
WindowsVersion GetWindowsVersion()
{
	#pragma comment(lib, "User32.lib")
	static WindowsVersion sWinVer = Invalid_WinVer;
	if (Invalid_WinVer != sWinVer)
		return sWinVer;

#if defined(_WINBASE_)
	//DWORD ver;
	OSVERSIONINFOEX osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if (!GetVersionEx((OSVERSIONINFO*)&osvi))
	{
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if (!GetVersionEx((OSVERSIONINFO*)&osvi))
			return (sWinVer = Unknown_WinVer);
	}

#if defined(_MSC_VER) && (_MSC_VER >= 1500) /* >= VC2008 */
	BYTE wProductType = osvi.wProductType;
#else
	BYTE* p = (BYTE*)osvi.wReserved;
	BYTE wProductType = p[2];
	#define VER_NT_WORKSTATION      0x0000001
	#define SM_SERVERR2             89
#endif
	if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
		sWinVer = Windows_NT;
	switch (osvi.dwMajorVersion)	 //判断主版本号
	{
	case 4:
		switch (osvi.dwMinorVersion)	//判断次版本号
		{
		case 0:
			if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
				sWinVer = Win32s;
			else if (osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
				sWinVer = Windows_95;
			break;
		case 10:
			sWinVer = Windows_98;
			break;
		case 90:
			sWinVer = Windows_Me;
			break;
		}
		break;
	case 5:
		switch (osvi.dwMinorVersion)
		{
		case 0:
			sWinVer = Windows_2000;
			break;
		case 1:
			sWinVer = Windows_XP;
			break;
		case 2:
			if (GetSystemMetrics(SM_SERVERR2) == 0)
				sWinVer = Windows_Server_2003;
			else if (GetSystemMetrics(SM_SERVERR2) != 0)
				sWinVer = Windows_Server_2003_R2;
			break;
		}
		break;
	case 6:
		switch (osvi.dwMinorVersion)
		{
		case 0:
			if (wProductType/*osvi.wProductType*/ == VER_NT_WORKSTATION)
				sWinVer = Windows_Vista;
			else
				sWinVer = Windows_Server_2008;   //服务器版本
			break;
		case 1:
			if (wProductType/*osvi.wProductType*/ == VER_NT_WORKSTATION)
				sWinVer = Windows_7;
			else
				sWinVer = Windows_Server_2008_R2;
			break;
		case 2:
			sWinVer = Windows_8;
			break;
		}
		break;
	default:
		sWinVer = Unknown_WinVer;
		break;
	}
#endif
#if defined(_MSC_VER) && (_MSC_VER < 1500) /* < VC2008 */
	#undef VER_NT_WORKSTATION
	#undef SM_SERVERR2
#endif
	return sWinVer;
}

const char* GetWindowsVersionText()
{
	WindowsVersion ver = GetWindowsVersion();
	const char* verTexts[] =
	{
		"Unknown_WinVer",
		"Win32s",
		"Windows_95",
		"Windows_98",
		"Windows_Me",
		"Windows_NT",
		"Windows_2000",
		"Windows_XP",
		"Windows_Server_2003",
		"Windows_Server_2003_R2",
		"Windows_Longhorn",
		"Windows_Vista",
		"Windows_Server_2008",
		"Windows_7",
		"Windows_Server_2008_R2",
		"Windows_8"
	};
	if (0 <= ver && ver<(int)(sizeof(verTexts) / sizeof(verTexts[0])))
		return verTexts[ver];
	return "Invalid_WinVer";
}
#endif

const char* GetCompilerName()
{
	const char* compiler =
#if defined(VS_IS_VC6)
		"VC6"
#elif defined(VS_IS_VC2008)
		"VC2008"
#elif defined(VS_IS_VC2010)
		"VC2010"
#elif defined(VS_IS_VC2012)
		"VC2012"
#elif defined(VS_IS_VC2013)
		"VC2013"
#elif defined(OS_IS_LINUX)
#ifdef OS_IS_MIPS_ARM_LINUX
		"MIPS_ARM_LINUX_"
#endif
		"GCCx.x"
#else
		"UNKNOWN_COMPILER"
#endif
		;

#if defined(OS_IS_LINUX)
	{
		/* 改写为GCC的正确版本号 */
		static int sInited = 0;
		static char ch[32];
		if (0 == sInited)
		{
			int l = (int)strlen(compiler);
			strcpy(ch, compiler);
			ch[l - 3] = (char)('0' + __GNUC__);
			ch[l - 1] = (char)('0' + __GNUC_MINOR__);
			sInited = 1;
		}
		return ch;
	}
#else
	return compiler;
#endif
}

const char* GetCompileBinaryType()
{
	const char* bin_type =
#ifdef _DEBUG
		"DEBUG"
#else
		"RELEASE"
#endif
#if defined(_UNICODE) || defined(UNICODE)
		"_UNICODE"
#endif
		;
	return bin_type;
}

BOOL IsConsoleApplication()
{
#if defined(WIN32)
	static BOOL inited = false;
	static BOOL is = false;
	if (!inited)
	{
		typedef HWND(WINAPI* NFGetConsoleWindow)();
		NFGetConsoleWindow fnGetConsoleWindow = (NFGetConsoleWindow)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "GetConsoleWindow");
		is = (fnGetConsoleWindow && fnGetConsoleWindow());
		inited = true;
	}
	return is;
#else
	return true;
#endif
}

#if defined(WIN32)
BOOL IsByDebugging()
{
	typedef BOOL(WINAPI* FNIsDebuggerPresent)();
	static FNIsDebuggerPresent fnIsDebuggerPresent = NULL;
	if (NULL == fnIsDebuggerPresent)
	{
		HMODULE mod = GetModuleHandle(_T("kernel32.dll"));
		fnIsDebuggerPresent = (FNIsDebuggerPresent)GetProcAddress(mod, "IsDebuggerPresent");
	}
	return (fnIsDebuggerPresent && fnIsDebuggerPresent());
}
#endif

int OSGetCpuCount()
{
	static int sCount = 0;
	if (0 == sCount)
	{
#if defined(WIN32)
		SYSTEM_INFO info;
		GetSystemInfo(&info);
		sCount = (int)info.dwNumberOfProcessors;
#elif defined(__linux__)
#ifdef __arm__
		const char* const PROCESSOR_VAR_NAME = "Processor";
#else
		const char* const PROCESSOR_VAR_NAME = "processor";
#endif //__arm__

		FILE* f = fopen("/proc/cpuinfo", "r");
		if (NULL == f)
		{
			ASSERT(false);
			return 1;
		}
#if defined(OS_IS_ANDROID)
		char szBuf[256] = { 0 };
		while (NULL != fgets(szBuf, sizeof(szBuf) - sizeof(char), f))
		{
			if (0 == strncmp(szBuf, PROCESSOR_VAR_NAME, 9))
				++sCount;
			memset(szBuf, 0, sizeof(szBuf));
		}
		VERIFY(0 == fclose(f));
#else
		char* line = NULL;
		size_t len = 0;
		while (-1 != getline(&line, &len, f))
		{
			if (0 == strncmp(line, PROCESSOR_VAR_NAME, 9))
				++sCount;
		}
		VERIFY(0 == fclose(f));
		if (line)
			free(line);
#endif

#endif
	}
	return sCount;
}

BOOL OSGetComputerName(char name[MAX_PATH], DWORD nameLen)
{
	if (name == NULL || nameLen <= 0)
		return false;

#if defined(WIN32)
	return GetComputerNameA(name, &nameLen);
#else
	return gethostname(name, nameLen) == -1 ? false : true;
#endif
}

INT64 GetCpuFrequency()
{
	static INT64 s_cpuFreq = 0;
	if (s_cpuFreq == 0)
	{
#if defined(WIN32)
		LARGE_INTEGER nFreq;
		if (QueryPerformanceFrequency(&nFreq))
			s_cpuFreq = nFreq.QuadPart;
#else
		//读/proc/cpuinfo??
#endif
	}
	return s_cpuFreq;
}

INT64 OSGetSysFreeMem(INT64* totalMem /*= NULL*/)
{
#if defined(WIN32)
#if _MSC_VER <= 1200	//vc6
	static fnGetMemProc fnGetMemInfoAddr = NULL;
	if (fnGetMemInfoAddr == NULL)
		fnGetMemInfoAddr = (fnGetMemProc)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "GlobalMemoryStatusEx");
	if (fnGetMemInfoAddr)
	{
		MEMORYSTATUSEX msEx;
		msEx.dwLength = sizeof(MEMORYSTATUSEX);
		fnGetMemInfoAddr(&msEx);
		if (totalMem)
			*totalMem = msEx.ullTotalPhys;
		return msEx.ullAvailPhys;
	}
	MEMORYSTATUS ms;
	ms.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&ms);
	if (totalMem)
		*totalMem = ms.dwTotalPhys;
	return ms.dwAvailPhys;
#else
	MEMORYSTATUSEX msEx;
	msEx.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&msEx);
	if (totalMem)
		*totalMem = msEx.ullTotalPhys;
	return msEx.ullAvailPhys;
#endif
#elif defined(__linux__)
#if defined(OS_IS_ANDROID) || defined(OS_IS_APPLE)
	char result[32 * 1024] = { 0 };	//__arm__下设置默认stack大小为256K
#else
	char result[64 * 1024] = { 0 };
#endif
	if (!ReadLinuxProcFile("/proc/meminfo", result, sizeof(result)))
		return 0;
	if (totalMem != NULL)
	{
		const char* sep = strstr(result, "MemTotal:");
		if (NULL != sep)
		{
			sep += strlen("MemTotal:");
			while (*sep == '\x20' || *sep == '\t')
				sep++;
			INT64 total = _atoi64(sep);
			*totalMem = total * 1024;
		}
	}

	INT64 free = 0;
	const char* sep2 = strstr(result, "MemFree:");
	if (NULL != sep2)
	{
		sep2 += strlen("MemFree:");
		while (*sep2 == '\x20' || *sep2 == '\t')
			sep2++;
		free = _atoi64(sep2);
		free *= 1024;
	}

	return free;
#else
	ASSERT(false);
	return 0;
#endif
}

INT64 OSGetDiskSpace(const char* lpszRootPath, INT64* totalSpace /*= NULL*/)
{
#if defined(WIN32)
	if (totalSpace)
		*totalSpace = 0;
	typedef BOOL(WINAPI* fnGetDiskFreeSpaceEx)(
		IN LPCSTR,
		OUT PULARGE_INTEGER,
		OUT PULARGE_INTEGER,
		OUT PULARGE_INTEGER);

	static HMODULE h = NULL;
	if (NULL == h)
	{
		h = GetModuleHandle(_T("kernel32.dll"));
		if (NULL == h)
			return 0;
	}
	static fnGetDiskFreeSpaceEx pGetDiskFreeSpaceEx = NULL;
	if (NULL == pGetDiskFreeSpaceEx)
		pGetDiskFreeSpaceEx = (fnGetDiskFreeSpaceEx)GetProcAddress(h, "GetDiskFreeSpaceExA");

	if (pGetDiskFreeSpaceEx)
	{
		ULARGE_INTEGER free_use = { 0 };
		ULARGE_INTEGER totals = { 0 };
		ULARGE_INTEGER frees = { 0 };
		if (pGetDiskFreeSpaceEx(lpszRootPath,
			(PULARGE_INTEGER)&free_use,
			(PULARGE_INTEGER)&totals,
			(PULARGE_INTEGER)&frees))
		{
			if (totalSpace)
				*totalSpace = totals.QuadPart;
			return free_use.QuadPart;
		}
	}

	DWORD sectors = 0;
	DWORD bytesPerSector = 0;
	DWORD clusters = 0;
	DWORD tClusters = 0;
	if (GetDiskFreeSpaceA(lpszRootPath, &sectors, &bytesPerSector, &clusters, &tClusters))
	{
		if (totalSpace)
			*totalSpace = (INT64)sectors * (INT64)bytesPerSector * (INT64)tClusters;
		return (INT64)sectors*(INT64)bytesPerSector*(INT64)clusters;
	}
	return 0;
#else
#if defined(OS_IS_ANDROID)
	struct statfs info;
	int status = statfs(lpszRootPath, &info);
#else
	struct statvfs info;
	int status = statvfs(lpszRootPath, &info);
#endif

	if (status >= 0)
	{
		if (totalSpace)
			*totalSpace = (INT64)info.f_blocks * (INT64)info.f_frsize;
		return (INT64)info.f_frsize * (INT64)info.f_bavail;
	}
	return 0;
#endif
}

#if defined(WIN32)
#define Li2Double(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))
inline INT64 CompareFileTime__(const FILETIME& oldTime, const FILETIME& newTime)
{
	INT64 n = (INT64)oldTime.dwHighDateTime << 32 | oldTime.dwLowDateTime;
	INT64 m = (INT64)newTime.dwHighDateTime << 32 | newTime.dwLowDateTime;
	return m - n;
}
int OSGetCpuUsage()
{
	if (GetWindowsVersion() <= Windows_XP)
	{
		static LARGE_INTEGER liOldIdleTime = { 0, 0 };
		static LARGE_INTEGER liOldSystemTime = { 0, 0 };
		static fnNtQuerySysInfo nt_query_fun = NULL;
		if (nt_query_fun == NULL)
			nt_query_fun = (fnNtQuerySysInfo)GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "NtQuerySystemInformation");
		if (nt_query_fun == NULL)
			return 0;

		SYSTEM_BASIC_INFORMATION basicInfo;
		LONG status = nt_query_fun(system_basic_information, &basicInfo, sizeof(SYSTEM_BASIC_INFORMATION), NULL);
		if (status != NO_ERROR || basicInfo.bKeNumberProcessors <= 0)
			return 0;

		BYTE processors = basicInfo.bKeNumberProcessors;
		SYSTEM_TIME_INFORMATION tmInfo;
		status = nt_query_fun(system_time_information, &tmInfo, sizeof(SYSTEM_TIME_INFORMATION), NULL);
		if (status != NO_ERROR)
			return 0;

		SYSTEM_PERFORMANCE_INFORMATION perfInfo;
		status = nt_query_fun(system_performance_information, &perfInfo, sizeof(SYSTEM_PERFORMANCE_INFORMATION), NULL);
		if (status != NO_ERROR)
			return 0;

		SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION* processorPerfInfo = new SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION[processors];
		ASSERT(processorPerfInfo != NULL);
		//PROCESSORS_USE_TIME* processorUseTimeInfo = new PROCESSORS_USE_TIME[processors];
		//ASSERT(processorUseTimeInfo != NULL);
		status = nt_query_fun(system_processorperformance_information,
			processorPerfInfo,
			sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION) * processors,
			NULL);
		if (status != NO_ERROR)
		{
			delete processorPerfInfo;
			//delete processorUseTimeInfo;
			return 0;
		}

		double dbIdleTime = 0;
		double dbSystemTime = 0;
		if (liOldIdleTime.QuadPart != 0)
		{
			dbIdleTime = Li2Double(perfInfo.liIdleTime) - Li2Double(liOldIdleTime);
			dbSystemTime = Li2Double(tmInfo.liKeSystemTime) - Li2Double(liOldSystemTime);

			dbIdleTime = dbIdleTime / dbSystemTime;
			dbIdleTime = 100.0 - dbIdleTime * 100.0 / (double)basicInfo.bKeNumberProcessors + 0.5;
		}
		liOldIdleTime = perfInfo.liIdleTime;
		liOldSystemTime = tmInfo.liKeSystemTime;
		delete processorPerfInfo;
		//delete processorUseTimeInfo;
		return (int)dbIdleTime;
	}
	else
	{
		static FILETIME oldIdleTime = { 0, 0 };
		static FILETIME oldKernelTime = { 0, 0 };
		static FILETIME oldUserTime = { 0, 0 };

		FILETIME idleTime = { 0, 0 };
		FILETIME kernelTime = { 0, 0 };
		FILETIME userTime = { 0, 0 };

		double cpu = 0;
		static fnGetSysTime k_GetSysTime = NULL;
		if (k_GetSysTime == NULL)
			k_GetSysTime = (fnGetSysTime)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "GetSystemTimes");
		if (k_GetSysTime == NULL)
			return (int)cpu;
		BOOL ret = k_GetSysTime(&idleTime, &kernelTime, &userTime);
		if (!ret)
			return (int)cpu;
		if (oldIdleTime.dwHighDateTime != 0 || oldIdleTime.dwLowDateTime != 0)
		{
			INT64 idle = CompareFileTime__(oldIdleTime, idleTime);
			INT64 kernel = CompareFileTime__(oldKernelTime, kernelTime);
			INT64 user = CompareFileTime__(oldUserTime, userTime);
			cpu = ((double)(kernel + user - idle) / (double)(kernel + user)) * 100 + 0.5;
		}
		oldIdleTime = idleTime;
		oldKernelTime = kernelTime;
		oldUserTime = userTime;
		return (int)cpu;
	}
}
#endif

#if defined(__linux__)
#define MAX_CPU_CORE_NUM 64 
static cpu_occupy sg_occupy[MAX_CPU_CORE_NUM];
int GetCpuUsage()
{
	int cpuUsage = 0;
	cpu_occupy cur_occupy[MAX_CPU_CORE_NUM];
	int nProcessors = min(MAX_CPU_CORE_NUM, OSGetCpuCount());
	FILE* fd = NULL;
	char buff[256] = { 0 };
	fd = fopen("/proc/stat", "r");
	if (fd == NULL)
		return cpuUsage;

	fgets(buff, sizeof(buff) - 1, fd);
	for (int n = 0; n < nProcessors; n++)
	{
		fgets(buff, sizeof(buff) - 1, fd);
		sscanf(buff, "%s %u %u %u %u", &cur_occupy[n].name, &cur_occupy[n].user, &cur_occupy[n].nice, &cur_occupy[n].system, &cur_occupy[n].idle);
	}
	fclose(fd);

	for (int n = 0; n < nProcessors; ++n)
	{
		double od, nd, id, sd;
		od = (double)(sg_occupy[n].user + sg_occupy[n].nice + sg_occupy[n].system + sg_occupy[n].idle);
		nd = (double)(cur_occupy[n].user + cur_occupy[n].nice + cur_occupy[n].system + cur_occupy[n].idle);
		id = (double)(cur_occupy[n].user - sg_occupy[n].user);
		sd = (double)(cur_occupy[n].system - sg_occupy[n].system);
		float cpu_used = ((sd + id)*100.0) / (nd - od);
		cpuUsage += (int)cpu_used;
	}
	memcpy(sg_occupy, cur_occupy, sizeof(cpu_occupy) * nProcessors);
	return cpuUsage;
}
#endif

#if defined(WIN32)
static BOOL EnableShutdownPrivilege__()
{
	TOKEN_PRIVILEGES tkp;
	HANDLE hToken = NULL;
	WindowsVersion winVer = GetWindowsVersion();
	if (winVer >= Windows_NT)
	{
		BOOL ret = OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		if (!ret || hToken == NULL)
			return false;
		tkp.PrivilegeCount = 1;
		if (!LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid))
		{
			CloseHandle(hToken);
			return false;
		}
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		ret = AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);	//调整权限
		CloseHandle(hToken);
		return ret;
	}
	return true;
}
#endif

#if defined(WIN32)
BOOL EnableAppErrorDlg(BOOL enable)
{
	HKEY hKey;
	BOOL ret = false;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SYSTEM\\ControlSet001\\Control\\Windows"), 0, KEY_WRITE, &hKey) != ERROR_SUCCESS)
		return ret;

	DWORD v = enable ? 0 : 2;	//v = 2禁用错误弹出框, v = 0启用错误弹出框
	ret = ERROR_SUCCESS == RegSetValueEx(hKey, _T("ErrorMode"), 0, REG_DWORD, (BYTE*)&v, sizeof(DWORD)) ? true : false;
	RegCloseKey(hKey);
	return ret;
}
#endif

BOOL OSShutdown(BOOL enforce/* = true*/)		//关机
{
#if defined(WIN32)
	EnableShutdownPrivilege__();
	return ExitWindowsEx(enforce ? EWX_SHUTDOWN | EWX_FORCE : EWX_SHUTDOWN, 0);
#else
	enforce;
	sync();	//同步磁盘数据,将缓存数据回写到硬盘,以防数据丢失
	reboot(RB_POWER_OFF);
	return true;
#endif
}

BOOL OSReboot(BOOL enforce/* = true*/)	//重启
{
#if defined(WIN32)
	EnableShutdownPrivilege__();
	return ExitWindowsEx(enforce ? EWX_REBOOT | EWX_FORCE : EWX_REBOOT, 0);
#else
	enforce;
	sync();	//同步磁盘数据,将缓存数据回写到硬盘,以防数据丢失
	reboot(RB_AUTOBOOT);
	return true;
#endif
}

BOOL OSLogOff(BOOL enforce/* = true*/)	//注销
{
#if defined(WIN32)
	EnableShutdownPrivilege__();
	return ExitWindowsEx(enforce ? EWX_LOGOFF | EWX_FORCE : EWX_LOGOFF, 0);
#else
	enforce;
	sync();	//同步磁盘数据,将缓存数据回写到硬盘,以防数据丢失
	reboot(RB_AUTOBOOT);
	return true;
#endif
}

#if defined(__linux__)
BOOL UpdateSystemFdsLimited(INT64 maxFds)	//有可能更新失败(比如无root权限)
{
	struct rlimit rl;
	rl.rlim_cur = (rlim_t)maxFds;
	rl.rlim_max = (rlim_t)maxFds;
	return 0 == setrlimit(RLIMIT_NOFILE, &rl) ? true : false;
}

void GetSystemFdsLimited(INT64* softLimit, INT64* hardLimit)
{
	struct rlimit rl;
	if (0 != getrlimit(RLIMIT_NOFILE, &rl))
		return;

	if (softLimit)
		*softLimit = rl.rlim_cur;
	if (hardLimit)
		*hardLimit = rl.rlim_max;
}
#endif