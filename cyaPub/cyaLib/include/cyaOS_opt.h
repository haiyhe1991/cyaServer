#ifndef __CYA_OS_OPT_H__
#define __CYA_OS_OPT_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaTypes.h"

//获取CPU个数
int OSGetCpuCount();

//获取计算机名称
BOOL OSGetComputerName(char name[MAX_PATH], DWORD nameLen);

//获取CPU频率
INT64 GetCpuFrequency();

//获取编译器名称
const char* GetCompilerName();
const char* GetCompileBinaryType();

BOOL IsConsoleApplication();

#if defined(WIN32)
	inline BOOL IsByDebugging();
	//获取windows版本
	WindowsVersion GetWindowsVersion();
	const char* GetWindowsVersionText();
	//设置错误报告(修改注册表)
	//enable true/false(启用/禁用)
	BOOL EnableAppErrorDlg(BOOL enable);
#endif

//获取系统内存
INT64 OSGetSysFreeMem(INT64* totalMem = NULL);

// 功  能：得到磁盘剩余/总空间
// 参  数：lpszRootPath，磁盘路径（注意：WIN32下lpszRootPath必须类似为 "C:\"，其它平台没有限制）
// 		  totalSpace，返回磁盘总的空间大小
// 返回值：成功返回指定磁盘的剩余空间(字节数)，否则返回0。
INT64 OSGetDiskSpace(const char* lpszRootPath, INT64* totalSpace = NULL);

//获取CPU使用率
#if defined(WIN32)
	#define system_basic_information 0 
	#define system_performance_information 2 
	#define system_time_information 3
	#define system_processorperformance_information 8
	typedef struct
	{
		DWORD dwUnknown1;
		ULONG uKeMaximumIncrement;
		ULONG uPageSize;
		ULONG uMmNumberOfPhysicalPages;
		ULONG uMmLowestPhysicalPage;
		ULONG uMmHighestPhysicalPage;
		ULONG uAllocationGranularity;
		PVOID pLowestUserAddress;
		PVOID pMmHighestUserAddress;
		ULONG uKeActiveProcessors;
		BYTE bKeNumberProcessors;
		BYTE bUnknown2;
		WORD wUnknown3;
	} SYSTEM_BASIC_INFORMATION;
typedef struct 
{ 
	LARGE_INTEGER liIdleTime; 
	DWORD dwSpare[76]; 
} SYSTEM_PERFORMANCE_INFORMATION;

typedef struct 
{ 
	LARGE_INTEGER liKeBootTime; 
	LARGE_INTEGER liKeSystemTime; 
	LARGE_INTEGER liExpTimeZoneBias; 
	ULONG uCurrentTimeZoneId; 
	DWORD dwReserved; 
} SYSTEM_TIME_INFORMATION;
typedef struct
{
	LARGE_INTEGER IdleTime;
	LARGE_INTEGER KernelTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER DpcTime;
	LARGE_INTEGER InterruptTime;
	ULONG Reserved2;
} SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;

typedef struct
{
	double dbOldIdleTime;	// save old total time
	double dbOldCurrentTime;
	double dbIdleTime;		// save time after calc
	double dbCurrentTime;
	float fUse;
} PROCESSORS_USE_TIME;
typedef LONG(WINAPI *fnNtQuerySysInfo)(UINT, PVOID, ULONG, PULONG);
typedef BOOL(WINAPI *fnGetSysTime)(LPFILETIME, LPFILETIME, LPFILETIME);
#endif

#if defined(__linux__)
struct cpu_occupy
{
	char name[64];
	unsigned int user;
	unsigned int nice;
	unsigned int system;
	unsigned int idle;

	cpu_occupy()
	{
		memset(this, 0, sizeof(cpu_occupy));
	}
};

#endif

int OSGetCpuUsage();

BOOL OSShutdown(BOOL enforce = true);	//关机
BOOL OSReboot(BOOL enforce = true);	//重启
BOOL OSLogOff(BOOL enforce = true);	//注销

#if defined(__linux__)
	BOOL UpdateSystemFdsLimited(INT64 maxFds);	//有可能更新失败(比如无root权限)
	void GetSystemFdsLimited(INT64* softLimit, INT64* hardLimit);
#endif

#endif