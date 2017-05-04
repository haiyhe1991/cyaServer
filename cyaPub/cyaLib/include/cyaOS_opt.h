#ifndef __CYA_OS_OPT_H__
#define __CYA_OS_OPT_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaTypes.h"

//��ȡCPU����
int OSGetCpuCount();

//��ȡ���������
BOOL OSGetComputerName(char name[MAX_PATH], DWORD nameLen);

//��ȡCPUƵ��
INT64 GetCpuFrequency();

//��ȡ����������
const char* GetCompilerName();
const char* GetCompileBinaryType();

BOOL IsConsoleApplication();

#if defined(WIN32)
	inline BOOL IsByDebugging();
	//��ȡwindows�汾
	WindowsVersion GetWindowsVersion();
	const char* GetWindowsVersionText();
	//���ô��󱨸�(�޸�ע���)
	//enable true/false(����/����)
	BOOL EnableAppErrorDlg(BOOL enable);
#endif

//��ȡϵͳ�ڴ�
INT64 OSGetSysFreeMem(INT64* totalMem = NULL);

// ��  �ܣ��õ�����ʣ��/�ܿռ�
// ��  ����lpszRootPath������·����ע�⣺WIN32��lpszRootPath��������Ϊ "C:\"������ƽ̨û�����ƣ�
// 		  totalSpace�����ش����ܵĿռ��С
// ����ֵ���ɹ�����ָ�����̵�ʣ��ռ�(�ֽ���)�����򷵻�0��
INT64 OSGetDiskSpace(const char* lpszRootPath, INT64* totalSpace = NULL);

//��ȡCPUʹ����
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

BOOL OSShutdown(BOOL enforce = true);	//�ػ�
BOOL OSReboot(BOOL enforce = true);	//����
BOOL OSLogOff(BOOL enforce = true);	//ע��

#if defined(__linux__)
	BOOL UpdateSystemFdsLimited(INT64 maxFds);	//�п��ܸ���ʧ��(������rootȨ��)
	void GetSystemFdsLimited(INT64* softLimit, INT64* hardLimit);
#endif

#endif