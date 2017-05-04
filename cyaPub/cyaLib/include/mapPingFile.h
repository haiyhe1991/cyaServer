// mapPingFile.h
// �ڴ�ӳ���ļ�

#ifndef __MAP_PING_FILE_H__
#define __MAP_PING_FILE_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaTypes.h"

//��    �ܣ��ڴ�ӳ���ļ���
//ʹ��˵����

class CMappingFile
{
private:
	PathName m_lpName;	//�ļ���
#if defined(WIN32)
	HANDLE	m_hFile;			//�ļ����
	HANDLE	m_hMap;				//map���
#elif defined(__linux__)
	int		m_hFile;
#endif

	BYTE*	m_lpFile;			//ӳ���ļ���ʼ��ַ
	DWORD	m_dwMappedSize;		//ӳ��ĳ���
	DWORD	m_dwFileSize;		//�ļ��ߴ�
	BOOL	m_bReadWrite;		//�Ƿ���Զ�д
	BOOL	m_bShareReadWrite;	//�Ƿ���Զ�д����
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
	//��  �ܣ������ڴ�ӳ���ļ�
	//��  ����lpszFile���ļ��������ΪNULL����ֱ����ϵͳҳ�ļ���ӳ�䣻
	//		  bReadWrite��false��ֻ����true���ɶ�д��
	//		  dwFileOffset��dwNumberOfBytesToMapĬ��Ϊӳ�������ļ���
	//		  bShareReadWrite���Ƿ���Զ�д����
	//				��bShareReadWriteΪtrue����bReadWriteΪfalse���ļ������Զ�д����ʽ�򿪡���
	//		  lpszPagingMapName��ҳ��ӳ��������֣�ӳ����ͨ�ļ�ʱ�ò��������ԣ���Windowsƽ̨֧�֣���
	//		  bExecutable��true��ӳ��ĵ�ַ����ִ�У�bReadWrite������Ч��
	//����ֵ��
	BOOL Create(LPCTSTR lpszFile,
				BOOL bReadWrite = false,
				DWORD dwFileOffset = 0,
				DWORD dwNumberOfBytesToMap = 0,
				BOOL bShareReadWrite = false,
				LPCTSTR lpszPagingMapName = NULL,
				BOOL bExecutable = false);

	//����ҳ���ļ�ӳ��
	BOOL CreatePagingMap(LPCTSTR lpszName,
						DWORD dwMapBytes,
						BOOL bReadWrite = true,
						BOOL bShareReadWrite = true)
	{
		ASSERT(lpszName);
		return Create(NULL, bReadWrite, 0, dwMapBytes, bShareReadWrite, lpszName);
	}

	//��  �ܣ�����ӳ�䡺ҳ���ļ���������ӳ�䡻
	//��  ����dwFileOffset��ӳ��ƫ��������SYSTEM_INFO::dwAllocationGranularity������������GetSystemInfo(...)��
	//		  dwNumberOfBytesToMap��0��ʾӳ�������ļ�
	//		  readWriteFlag����д��־��1��ֻ����2���ɶ�д������ֵ�����䡣
	//����ֵ��
	BOOL ReMap(DWORD dwFileOffset, DWORD dwNumberOfBytesToMap,
				OUT BYTE** ptrFile = NULL, OUT DWORD* mappedSize = NULL,
				int readWriteFlag = 0);

	void Release();

	//��  �ܣ���д�ڴ�ӳ���ļ�
	//��  ����pvAddr����д��ʼ��ַ��dwBytes����д�ֽ���
	//����ֵ��
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

	// ϵͳҳ��ӳ���ļ�֧��
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