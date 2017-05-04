// mapPingFile.h
// 内存映射文件

#ifndef __MAP_PING_FILE_H__
#define __MAP_PING_FILE_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaTypes.h"

//功    能：内存映射文件类
//使用说明：

class CMappingFile
{
private:
	PathName m_lpName;	//文件名
#if defined(WIN32)
	HANDLE	m_hFile;			//文件句柄
	HANDLE	m_hMap;				//map句柄
#elif defined(__linux__)
	int		m_hFile;
#endif

	BYTE*	m_lpFile;			//映射文件起始地址
	DWORD	m_dwMappedSize;		//映射的长度
	DWORD	m_dwFileSize;		//文件尺寸
	BOOL	m_bReadWrite;		//是否可以读写
	BOOL	m_bShareReadWrite;	//是否可以读写共享
	BOOL	m_bExecutable;

#if defined(WIN32)
	BOOL	 m_isCreatedBySysPageFile;
	PathName m_pagingMapName;
#endif

	void Init();
public:
	CMappingFile();
	CMappingFile(LPCTSTR lpszFile,
		BOOL  bReadWrite = false,
		DWORD dwFileOffset = 0,
		DWORD dwNumberOfBytesToMap = 0,
		BOOL  bShareReadWrite = false,
		LPCTSTR lpszPagingMapName = NULL,
		BOOL  bExecutable = false);
	~CMappingFile();

public:
	//功  能：建立内存映射文件
	//参  数：lpszFile，文件名，如果为NULL，将直接在系统页文件里映射；
	//		  bReadWrite，false，只读，true，可读写；
	//		  dwFileOffset，dwNumberOfBytesToMap默认为映射整个文件；
	//		  bShareReadWrite，是否可以读写共享
	//				『bShareReadWrite为true或者bReadWrite为false，文件都将以读写共享方式打开』；
	//		  lpszPagingMapName，页面映射对象名字，映射普通文件时该参数被忽略（仅Windows平台支持）；
	//		  bExecutable，true，映射的地址可以执行，bReadWrite参数无效；
	//返回值：
	BOOL Create(LPCTSTR lpszFile,
				BOOL bReadWrite = false,
				DWORD dwFileOffset = 0,
				DWORD dwNumberOfBytesToMap = 0,
				BOOL bShareReadWrite = false,
				LPCTSTR lpszPagingMapName = NULL,
				BOOL bExecutable = false);

	//建立页面文件映射
	BOOL CreatePagingMap(LPCTSTR lpszName,
						DWORD dwMapBytes,
						BOOL bReadWrite = true,
						BOOL bShareReadWrite = true)
	{
		ASSERT(lpszName);
		return Create(NULL, bReadWrite, 0, dwMapBytes, bShareReadWrite, lpszName);
	}

	//功  能：重新映射『页面文件不能重新映射』
	//参  数：dwFileOffset，映射偏移量，是SYSTEM_INFO::dwAllocationGranularity的整数倍『见GetSystemInfo(...)』
	//		  dwNumberOfBytesToMap，0表示映射整个文件
	//		  readWriteFlag，读写标志，1，只读；2，可读写；其它值，不变。
	//返回值：
	BOOL ReMap(DWORD dwFileOffset, DWORD dwNumberOfBytesToMap,
				OUT BYTE** ptrFile = NULL, OUT DWORD* mappedSize = NULL,
				int readWriteFlag = 0);

	void Release();

	//功  能：回写内存映射文件
	//参  数：pvAddr，回写起始地址；dwBytes，回写字节数
	//返回值：
	BOOL Flush(const void* pvAddr, DWORD dwBytes);

public:
	BOOL IsValid() const
	{
		return NULL != m_lpFile;
	}
	LPCTSTR FileName() const
	{
		return m_lpName;
	}

	BOOL IsPageMaping() const
	{
		return m_lpFile && 0 == m_lpName[0];
	}

	DWORD MappedSize() const
	{
		return m_dwMappedSize;
	}
	DWORD FileSize() const
	{
		return m_dwFileSize;
	}
	BOOL IsReadWrite() const
	{
		return m_bReadWrite;
	}

	const BYTE* Mapping() const
	{
		return m_lpFile;
	}
	BYTE* MappingWritable()
	{
		ASSERT(m_bReadWrite); return m_lpFile;
	}
	const BYTE* Get() const
	{
		return m_lpFile;
	}
	BYTE* GetWritable()
	{
		ASSERT(m_bReadWrite); return m_lpFile;
	}

	// 系统页面映射文件支持
public:
#if defined(WIN32)
	BOOL IsSysPageFileMapping() const
	{
		return (NULL != m_lpFile && 0 == m_lpName[0] && m_pagingMapName[0]);
	}
	BOOL IsCreatedBySysPageFile() const
	{
		ASSERT(IsSysPageFileMapping()); return m_isCreatedBySysPageFile;
	}
	LPCTSTR GetSysPageFilename() const
	{
		ASSERT(IsSysPageFileMapping()); return m_pagingMapName;
	}
#endif
};
#endif