#if defined(__linux__)
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <sys/mman.h>
#endif
#include "mapPingFile.h"

void CMappingFile::Init()
{
	ZeroArray(m_lpName);
#if defined(WIN32)
	m_hFile = INVALID_HANDLE_VALUE;
	m_hMap = NULL;
#elif defined(__linux__)
	m_hFile = -1;
#endif
	m_lpFile = NULL;
	m_dwMappedSize = 0;
	m_dwFileSize = 0;
	m_bReadWrite = false;

	m_bExecutable = false;

#if defined(WIN32)
	m_isCreatedBySysPageFile = false;
	ZeroArray(m_pagingMapName);
#endif
}

CMappingFile::CMappingFile()
{
	Init();
}

CMappingFile::CMappingFile(LPCTSTR lpszFile,
							BOOL bReadWrite /*= false*/,
							DWORD dwFileOffset /*= 0*/,
							DWORD dwNumberOfBytesToMap /*= 0*/,
							BOOL bShareReadWrite /*= false*/,
							LPCTSTR lpszPagingMapName /*= NULL*/,
							BOOL bExecutable /*= false*/)
{
	Init();
	Create(lpszFile, bReadWrite, dwFileOffset, dwNumberOfBytesToMap, bShareReadWrite, lpszPagingMapName, bExecutable);
}

CMappingFile::~CMappingFile()
{
	Release();
}

BOOL CMappingFile::Create(LPCTSTR lpszFile,
							BOOL bReadWrite /*= false*/,
							DWORD dwFileOffset /*= 0*/,
							DWORD dwNumberOfBytesToMap /*= 0*/,
							BOOL bShareReadWrite /*= false*/,
							LPCTSTR lpszPagingMapName /*= NULL*/,
							BOOL bExecutable /*= false*/)
{
	if (IsValid())
	{
		ASSERT(false);
		return false;
	}

	m_bReadWrite = bReadWrite;
	m_bShareReadWrite = bShareReadWrite;
	m_bExecutable = bExecutable;

#if defined(WIN32)
	DWORD dwFileSizeHigh = 0;
	if (NULL != lpszFile)
	{
		_tcscpy(m_lpName, lpszFile);
		m_hFile = CreateFile(lpszFile,
			bExecutable ? (GENERIC_READ/*|GENERIC_EXECUTE*/) : (bReadWrite ? GENERIC_READ | GENERIC_WRITE : GENERIC_READ),
			bExecutable ? (FILE_SHARE_READ) : (bShareReadWrite || !bReadWrite ? FILE_SHARE_READ | FILE_SHARE_WRITE : FILE_SHARE_READ),
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if (INVALID_HANDLE_VALUE == m_hFile)
			goto fail_label_;

		m_dwFileSize = GetFileSize(m_hFile, &dwFileSizeHigh);
		if (0 != dwFileSizeHigh) // 不能大小超过4G的文件
			goto fail_label_;
		if (0 == m_dwFileSize) // 对0字节文件特殊处理
			return true;
	}
	else
	{
		m_hFile = INVALID_HANDLE_VALUE;
		ZeroArray(m_lpName);
		m_dwFileSize = 0;
	}

	LPCTSTR mapName;
	mapName = lpszPagingMapName;
	m_hMap = CreateFileMapping(m_hFile,
		NULL,
		bExecutable ? (PAGE_READONLY/*PAGE_EXECUTE_READ*/) : (bReadWrite ? PAGE_READWRITE : PAGE_READONLY),
		0,
		dwNumberOfBytesToMap,
		mapName);
	if (NULL == m_hMap)
		goto fail_label_;

	BOOL isCreatedBySysPageFile;
	if (NULL == lpszFile)
		isCreatedBySysPageFile = !(ERROR_ALREADY_EXISTS == GetLastError());
	else
		isCreatedBySysPageFile = false;

	if (0 == m_lpName[0])	//页面文件的文件大小 = 映射尺寸
	{
		ASSERT(dwNumberOfBytesToMap > 0);
		m_dwFileSize = dwNumberOfBytesToMap;
	}

	if (0 == m_lpName[0])
		m_lpName[0] = (TCHAR)1;	//页面文件的初始化映射，置m_lpName[0]为1以欺骗ReMapFile(...)不是页面映射文件

	if (!ReMap(dwFileOffset, dwNumberOfBytesToMap))
		goto fail_label_;

	if ((TCHAR)1 == m_lpName[0])
		m_lpName[0] = 0;

	m_isCreatedBySysPageFile = isCreatedBySysPageFile;
	if (NULL == lpszFile)
	{
		ASSERT(lpszPagingMapName);
		_tcscpy(m_pagingMapName, lpszPagingMapName);
	}

#elif defined(__linux__)
	ASSERT(!bExecutable); // 怎么实现???
	if (NULL != lpszFile)
	{
		strcpy(m_lpName, lpszFile);
		m_hFile = open(lpszFile, bReadWrite ? O_RDWR : O_RDONLY, 0);
		if (m_hFile < 0)
			goto fail_label_;

		struct stat st;
		if (fstat(m_hFile, &st) != 0)
			goto fail_label_;
		m_dwFileSize = st.st_size;
		if (0 == m_dwFileSize)
			return true;
	}
	else
	{
		if (lpszPagingMapName)
		{
			ASSERT(false); // 未实现，可用shmget()实现（lpszPagingMapName需要转换为int键值）
		}
		goto fail_label_;
	}

	void* p;
	p = mmap(NULL,
		0 == dwNumberOfBytesToMap ? m_dwFileSize : dwNumberOfBytesToMap,
		bReadWrite ? (PROT_READ | PROT_WRITE) : PROT_READ,
		bShareReadWrite ? MAP_SHARED : MAP_PRIVATE,
		m_hFile,
		dwFileOffset);
	if (p == MAP_FAILED || p == NULL)
		goto fail_label_;

	m_lpFile = (BYTE*)p;

	if (0 == dwFileOffset && 0 == dwNumberOfBytesToMap)
		m_dwMappedSize = m_dwFileSize;
	else if (0 == dwNumberOfBytesToMap)
		m_dwMappedSize = m_dwFileSize - dwFileOffset;
	else
		m_dwMappedSize = dwNumberOfBytesToMap;
#endif

	return true;

fail_label_:
	Release();
	return false;
}

BOOL CMappingFile::ReMap(DWORD dwFileOffset, DWORD dwNumberOfBytesToMap,
	BYTE** ptrFile /*= NULL*/, DWORD* mappedSize /*= NULL*/,
	int readWriteFlag /*= 0*/)
{
#if defined(WIN32)
	if (NULL == m_hMap)
	{
		ASSERT(false);
		return false;
	}
#endif

	if (IsPageMaping())
		return false;	//页面文件不支持重映射

	if (dwFileOffset >= m_dwFileSize)
		return false;

#if defined(WIN32)
	void* suggestBaseAddress = m_lpFile;
	if (NULL != m_lpFile)
		UnmapViewOfFile(m_lpFile);

	if (dwNumberOfBytesToMap > m_dwFileSize - dwFileOffset)
		dwNumberOfBytesToMap = m_dwFileSize - dwFileOffset;

	DWORD accessFlags;
	if (1 == readWriteFlag)
	{
		m_bExecutable = false;
		m_bReadWrite = false;
	}
	else if (2 == readWriteFlag)
	{
		m_bExecutable = false;
		m_bReadWrite = true;
	}
	accessFlags = (m_bExecutable ? (FILE_MAP_READ) : (m_bReadWrite ? FILE_MAP_READ | FILE_MAP_WRITE : FILE_MAP_READ));
	m_lpFile = (BYTE*)MapViewOfFileEx(m_hMap,
		accessFlags,
		0,
		dwFileOffset,
		dwNumberOfBytesToMap,
		suggestBaseAddress); // 尽量和上一次的地址相同
	if (NULL == m_lpFile)
		return false;
#elif defined(__linux__)
	if (0 != dwFileOffset)
	{
		ASSERT(false);
		return false;
	}

	void* suggestBaseAddress = m_lpFile;
	if (NULL != m_lpFile)
	{
		ASSERT(m_dwMappedSize > 0);
		munmap(m_lpFile, m_dwMappedSize);
	}

	if (dwNumberOfBytesToMap > m_dwFileSize - dwFileOffset)
		dwNumberOfBytesToMap = m_dwFileSize - dwFileOffset;

	if (1 == readWriteFlag)
	{
		m_bExecutable = false;
		m_bReadWrite = false;
	}
	else if (2 == readWriteFlag)
	{
		m_bExecutable = false;
		m_bReadWrite = true;
	}

	void* p = mmap(suggestBaseAddress, // 尽量和上一次的地址相同
		0 == dwNumberOfBytesToMap ? m_dwFileSize : dwNumberOfBytesToMap,
		m_bReadWrite ? (PROT_READ | PROT_WRITE) : PROT_READ,
		m_bShareReadWrite ? MAP_SHARED : MAP_PRIVATE,
		m_hFile,
		dwFileOffset);
	//	void* mremap(void* old_address, size_t old_size, size_t new_size, unsigned long flags);

	if (p == MAP_FAILED)
	{
		m_lpFile = NULL;
		return false;
	}
	m_lpFile = (BYTE*)p;
#endif

	if (0 == dwFileOffset && 0 == dwNumberOfBytesToMap)
		m_dwMappedSize = m_dwFileSize;
	else if (0 == dwNumberOfBytesToMap)
		m_dwMappedSize = m_dwFileSize - dwFileOffset;
	else
		m_dwMappedSize = dwNumberOfBytesToMap;

	if (ptrFile)
		*ptrFile = m_lpFile;
	if (mappedSize)
		*mappedSize = m_dwMappedSize;

	if (m_bExecutable)
	{
#if defined(_DEBUG) && defined(WIN32)
		//	#pragma compiling_remind("如何处理？？？")
		DWORD newflag = PAGE_READONLY | PAGE_EXECUTE_READ;
		DWORD oldflag;
		if (!VirtualProtect(m_lpFile, m_dwMappedSize, newflag, &oldflag))
		{
			//#$!#ifdef _DEBUG
			//#$!			TCHAR txt[512];
			//#$!			GetLastErrText(txt);
			//#$!			TRACE_EX(_T("CMappingFile::ReMap() failed, last error: %s\n"), txt);
			//#$!#endif
		}
#endif
	}

	return true;
}

void CMappingFile::Release()
{
	//	if(!IsValid())
	//		return;

#if defined(WIN32)
	if (NULL != m_lpFile)
		UnmapViewOfFile(m_lpFile);

	if (NULL != m_hMap)
		VERIFY(CloseHandle(m_hMap));

	if (INVALID_HANDLE_VALUE != m_hFile)
		VERIFY(CloseHandle(m_hFile));
#elif defined(__linux__)
	if (NULL != m_lpFile)
		munmap(m_lpFile, m_dwMappedSize);
	if (m_hFile >= 0)
		VERIFY(0 == close(m_hFile));
#endif

	Init();
}

BOOL CMappingFile::Flush(const void* pvAddr, DWORD dwBytes)
{
	if (!IsValid())
		return false;

	if (!m_bReadWrite)
		return false;

	if (pvAddr < m_lpFile ||
		((BYTE*)pvAddr + dwBytes) >((BYTE*)m_lpFile + m_dwMappedSize))
	{
		ASSERT(0);
		return false;
	}

#if defined(WIN32)
	return	FlushViewOfFile(pvAddr, dwBytes);
#elif defined(__linux__)
	return 0 == msync((void*)pvAddr, dwBytes, MS_SYNC);
#endif
}