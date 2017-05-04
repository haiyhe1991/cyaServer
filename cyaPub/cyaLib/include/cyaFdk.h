#ifndef __CYA_FDK_H__
#define __CYA_FDK_H__

#if defined(_MSC_VER)&&_MSC_VER>1000
	#pragma once
#endif

#include "cyaTypes.h"
#if defined(WIN32)
	#include <io.h>
#endif
#if defined(__linux__)
	#include <dirent.h>
#endif
#if defined(__linux__)
	struct SCAN_FILE_FILTER	//�ļ���������
	{
		SCAN_FILE_FILTER()
			: count(0)
			, nameList(NULL)
		{
		}

		~SCAN_FILE_FILTER()
		{
			if (count == 0 || nameList == NULL)
				return;
			for (int i = 0; i < count; ++i)
				::free(nameList[i]);
			::free(nameList);
		}

		int count;
		struct dirent** nameList;
		static int scandir_filter_(const struct dirent* nl)
		{
			if (0 == strcmp(nl->d_name, ".") || 0 == strcmp(nl->d_name, ".."))
				return 0;
			return 1;
		}
	};
#endif

//�ļ��Ƿ����
BOOL IsExistFile(const char* lpsz);
BOOL IsExistFile(const WCHAR* lpsz);
#if defined(__linux__)
	BOOL IsExistFile(int fd);
#endif

//�Ƿ���һ������·���ļ���Ŀ¼����pathname�Ƿ�����Ϊ��������᷵��false
BOOL IsAbsolutePath(const char* pathname);
BOOL IsAbsolutePath(const WCHAR* pathname);

//Ŀ¼�Ƿ����
BOOL IsExistDir(const char* lpsz);
BOOL IsExistDir(const WCHAR* lpsz);

//�����ļ���ָ�루����pathname���κο���������
//c:\\path1\\a.txt->a.txt
// /root/test/a.txt->a.txt
//ʧ�ܷ���NULL
const char* GetFileName(const char* pathname, INT_PTR pathname_len = -1);
const WCHAR* GetFileName(const WCHAR* pathname, INT_PTR pathname_len = -1);

//�õ�Ŀ¼����pathname������·������Ҳ�������ļ���
//�õ���dir����ͳһ�ĸ�ʽ��Ŀ¼����һ����б��
//e:\\dir\\a.txt->e:\\dir\\
// /root/dir/a.txt->root/dir/
//���ܴ�������·��
BOOL GetDirName(const char* pathname, char dir[MAX_PATH], INT_PTR pathname_len = -1);
BOOL GetDirName(const WCHAR* pathname, WCHAR dir[MAX_PATH], INT_PTR pathname_len = -1);

//��ȡ�ļ�����׺��������file���κο���������
const char* GetFileSuffixName(const char* file,INT_PTR file_len = -1);
const WCHAR* GetFileSuffixName(const WCHAR* file,INT_PTR file_len = -1);

//��ȡϵͳ����ʱ�ļ�·��������·�������ܴ�б�ܣ�windows:'\',else:'/'��
BOOL GetOSTempPath(char path[MAX_PATH]);
BOOL GetOSTempPath(WCHAR path[MAX_PATH]);

//����·����·���Ѿ����ڻ��߽����ɹ�����true������ʧ�ܷ���false
BOOL CreatePath(const char* lpszFullPath, INT_PTR len = -1);
BOOL CreatePath(const WCHAR* lpszFullPath, INT_PTR len = -1);

//�ϳɾ���·������֧������·��
const char* MergeFileName(const char* path, const char* file, char pathname[MAX_PATH]);
const WCHAR* MergeFileName(const WCHAR* path, const WCHAR* file, WCHAR pathname[MAX_PATH]);

//�õ��ļ��ߴ磬�ɹ������ļ����ȣ�ʧ�ܷ���-1
INT64 GetFileSize(FILE* pf);
INT64 GetFileSize(const char* lpszFile);
INT64 GetFileSize(const WCHAR* lpszFile);
OSFHANDLE FILEToHandle(FILE* pf);

#if defined(__linux__)
	BOOL ReadLinuxProcFile(const char* procFilePath, char buf[1024 * 128], int bufLen = -1);
#endif
#endif