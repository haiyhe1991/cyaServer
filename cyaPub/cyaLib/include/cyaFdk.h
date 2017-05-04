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
	struct SCAN_FILE_FILTER	//文件搜索过滤
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

//文件是否存在
BOOL IsExistFile(const char* lpsz);
BOOL IsExistFile(const WCHAR* lpsz);
#if defined(__linux__)
	BOOL IsExistFile(int fd);
#endif

//是否是一个绝对路径文件、目录名；pathname非法或者为相对名都会返回false
BOOL IsAbsolutePath(const char* pathname);
BOOL IsAbsolutePath(const WCHAR* pathname);

//目录是否存在
BOOL IsExistDir(const char* lpsz);
BOOL IsExistDir(const WCHAR* lpsz);

//返回文件名指针（不对pathname做任何拷贝操作）
//c:\\path1\\a.txt->a.txt
// /root/test/a.txt->a.txt
//失败返回NULL
const char* GetFileName(const char* pathname, INT_PTR pathname_len = -1);
const WCHAR* GetFileName(const WCHAR* pathname, INT_PTR pathname_len = -1);

//得到目录名，pathname可以是路径名，也可以是文件名
//得到的dir按照统一的格式，目录后面一概有斜杠
//e:\\dir\\a.txt->e:\\dir\\
// /root/dir/a.txt->root/dir/
//不能处理网络路径
BOOL GetDirName(const char* pathname, char dir[MAX_PATH], INT_PTR pathname_len = -1);
BOOL GetDirName(const WCHAR* pathname, WCHAR dir[MAX_PATH], INT_PTR pathname_len = -1);

//获取文件名后缀名（不对file做任何拷贝操作）
const char* GetFileSuffixName(const char* file,INT_PTR file_len = -1);
const WCHAR* GetFileSuffixName(const WCHAR* file,INT_PTR file_len = -1);

//获取系统的临时文件路径，返回路径名后总带斜杠（windows:'\',else:'/'）
BOOL GetOSTempPath(char path[MAX_PATH]);
BOOL GetOSTempPath(WCHAR path[MAX_PATH]);

//创建路径，路径已经存在或者建立成功返回true，建立失败返回false
BOOL CreatePath(const char* lpszFullPath, INT_PTR len = -1);
BOOL CreatePath(const WCHAR* lpszFullPath, INT_PTR len = -1);

//合成绝对路径，不支持网络路径
const char* MergeFileName(const char* path, const char* file, char pathname[MAX_PATH]);
const WCHAR* MergeFileName(const WCHAR* path, const WCHAR* file, WCHAR pathname[MAX_PATH]);

//得到文件尺寸，成功返回文件长度，失败返回-1
INT64 GetFileSize(FILE* pf);
INT64 GetFileSize(const char* lpszFile);
INT64 GetFileSize(const WCHAR* lpszFile);
OSFHANDLE FILEToHandle(FILE* pf);

#if defined(__linux__)
	BOOL ReadLinuxProcFile(const char* procFilePath, char buf[1024 * 128], int bufLen = -1);
#endif
#endif