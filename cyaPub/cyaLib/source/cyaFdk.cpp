#include "cyaFdk.h"
#include "cyaStrdk.h"
#include "cyaStrCodec.h"
#include <sys/stat.h>
#include <vector>

#if defined(WIN32)
	#include <direct.h>
#elif defined(__linux__)
	#if defined(OS_IS_ANDROID)
		#include <sys/statfs.h>
	#else
		#include <sys/statvfs.h>
	#endif

	#include <fcntl.h>
	#include <unistd.h>
	#include <dirent.h>
#endif

#if defined(WIN32)
static inline HANDLE CreateFile_Tmpl_(const char* file, DWORD access, DWORD sMode, LPSECURITY_ATTRIBUTES attr, DWORD disp, DWORD fAttr, HANDLE h)
{
	return CreateFileA(file, access, sMode, attr, disp, fAttr, h);
}

static inline HANDLE CreateFile_Tmpl_(const WCHAR* file, DWORD access, DWORD sMode, LPSECURITY_ATTRIBUTES attr, DWORD disp, DWORD fAttr, HANDLE h)
{
	return CreateFileW(file, access, sMode, attr, disp, fAttr, h);
}

static inline DWORD GetFileAttributes_Tmpl_(const char* file)
{
	return GetFileAttributesA(file);
}

static inline DWORD GetFileAttributes_Tmpl_(const WCHAR* file)
{
	return GetFileAttributesW(file);
}

CHAR_TYPE_INLINE BOOL CreateBlankFile_Tmpl_(const CHAR_TYPE* lpszFile, INT64 size, BOOL openAlways /*= false*/)
{
	if (size<0 || size>(INT64)1024 * 1024 * 1024 * 1024) // [0, 1T]
	{
		ASSERT(false);
		return false;
	}
	if (!openAlways)
	{
		if (IsExistFile(lpszFile))
			return false;
	}
	HANDLE hFile = CreateFile_Tmpl_(lpszFile,
		GENERIC_WRITE,
		0,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (INVALID_HANDLE_VALUE == hFile)
		return false;

	LONG lHighMove = (LONG)(size >> 32);
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile,
		(LONG)size,
		&lHighMove,
		FILE_BEGIN)
		&& NO_ERROR != GetLastError())
		goto fail_label_;

	if (!SetEndOfFile(hFile))
		goto fail_label_;

	VERIFY(CloseHandle(hFile));
	return true;

fail_label_:
	VERIFY(CloseHandle(hFile));
	return false;
}
#else
static inline int stat_Tmpl_(const char* file, struct stat* fileinfo)
{
	return ::stat(file, fileinfo);
}

static inline int stat_Tmpl_(const WCHAR* file, struct stat* fileinfo)
{
	char szPath[MAX_PATH] = { 0 };
	if (NULL == wctoc(file, szPath, MAX_PATH))
		return -1;
	return ::stat(szPath, fileinfo);
}
#endif

CHAR_TYPE_INLINE BOOL IsExistFile_Tmpl_(const CHAR_TYPE* lpsz)
{
	if (lpsz == NULL)
		return false;

#if defined(WIN32)
	DWORD dwAttr = GetFileAttributes_Tmpl_(lpsz);
	if ((DWORD)-1 == dwAttr)
		return false;
	return 0 == (FILE_ATTRIBUTE_DIRECTORY & dwAttr);
#elif defined(__linux__)
	struct stat fileinfo;
	int r = stat_Tmpl_(lpsz, &fileinfo);
	return (r == 0 && S_IFREG == (fileinfo.st_mode & S_IFREG));
#endif
}

CHAR_TYPE_INLINE BOOL IsExistDir_Tmpl_(const CHAR_TYPE* lpsz)
{
	if (lpsz == NULL)
		return false;

#if defined(WIN32)
	DWORD dwAttr = GetFileAttributes_Tmpl_(lpsz);
	if ((DWORD)-1 == dwAttr)
		return false;
	return FILE_ATTRIBUTE_DIRECTORY == (FILE_ATTRIBUTE_DIRECTORY & dwAttr);
#elif defined(__linux__)
	struct stat fileinfo;
	int r = stat_Tmpl_(lpsz, &fileinfo);
	return (r == 0 && S_IFDIR == (fileinfo.st_mode & S_IFDIR));
#endif
}

BOOL IsExistFile(const char* lpsz)
{
	return IsExistFile_Tmpl_(lpsz);
}
BOOL IsExistFile(const WCHAR* lpsz)
{
	return IsExistFile_Tmpl_(lpsz);
}

#if defined(__linux__)
BOOL IsExistFile(int fd)
{
	struct stat fileinfo;
	int r = fstat(fd, &fileinfo);
	return (r == 0 && S_IFREG == (fileinfo.st_mode & S_IFREG));
}
#endif

CHAR_TYPE_INLINE BOOL IsAbsolutePath_Tmpl_(const CHAR_TYPE* pathname)
{
	if (pathname == NULL)
		return false;

#if defined(WIN32)
	if (IsLetter(pathname[0]) && ':' == pathname[1])
		return true;
	if ('\\' == pathname[0] && '\\' == pathname[1])
		return true;

#elif defined(__linux__)
	if ('/' == pathname[0])
		return true;
#endif

	return false;
}

BOOL IsAbsolutePath(const char* pathname)
{
	return IsAbsolutePath_Tmpl_(pathname);
}

BOOL IsAbsolutePath(const WCHAR* pathname)
{
	return IsAbsolutePath_Tmpl_(pathname);
}

BOOL IsExistDir(const char* lpsz)
{
	return IsExistDir_Tmpl_(lpsz);
}

BOOL IsExistDir(const WCHAR* lpsz)
{
	return IsExistDir_Tmpl_(lpsz);
}

CHAR_TYPE_INLINE const CHAR_TYPE* GetFileName_Tmpl_(const CHAR_TYPE* pathname, INT_PTR len)
{
	for (INT_PTR i = len; i >= 0; --i)
	{
		if (IsSlashChar(pathname[i]))
			return pathname + i + 1;
	}
	return pathname;
}

const char* GetFileName(const char* pathname, INT_PTR pathname_len/* = -1*/)
{
	if (pathname == NULL)
		return NULL;
	INT_PTR len = pathname_len == -1 ? strlen(pathname) - 1 : pathname_len - 1;
	return GetFileName_Tmpl_(pathname, len);
}

const WCHAR* GetFileName(const WCHAR* pathname, INT_PTR pathname_len/* = -1*/)
{
	if (pathname == NULL)
		return NULL;
	INT_PTR len = pathname_len == -1 ? wcslen(pathname) - 1 : pathname_len - 1;
	return GetFileName_Tmpl_(pathname, len);
}

CHAR_TYPE_INLINE BOOL GetDirName_Tmpl_(const CHAR_TYPE* pathname, CHAR_TYPE dir[MAX_PATH], INT_PTR len)
{
	if (pathname == NULL || dir == NULL)
		return false;
	for (INT_PTR i = len; i >= 0; --i)
	{
		if (IsSlashChar(pathname[i]))
		{
			memcpy(dir, pathname, (i + 1)*sizeof(CHAR_TYPE));
			return true;
		}
	}
#if defined(WIN32)
	CHAR_TYPE ch[] = { '.', '\\' };
#else
	CHAR_TYPE ch[] = { '.', '/' };
#endif
	memcpy(dir, ch, sizeof(ch));
	return true;
}

BOOL GetDirName(const char* pathname, char dir[MAX_PATH], INT_PTR pathname_len /*= -1*/)
{
	INT_PTR len = pathname_len == -1 ? strlen(pathname) - 1 : pathname_len - 1;
	return GetDirName_Tmpl_(pathname, dir, len);
}

BOOL GetDirName(const WCHAR* pathname, WCHAR dir[MAX_PATH], INT_PTR pathname_len /*= -1*/)
{
	INT_PTR len = pathname_len == -1 ? wcslen(pathname) - 1 : pathname_len - 1;
	return GetDirName_Tmpl_(pathname, dir, len);
}

CHAR_TYPE_INLINE const CHAR_TYPE* GetFileSuffixName_Tmpl_(const CHAR_TYPE* file, INT_PTR len)
{
	for (INT_PTR i = len; i >= 0; --i)
	{
		if (file[i] == '.')
			return file + i + 1;
	}
	return file;
}

const char* GetFileSuffixName(const char* file, INT_PTR file_len /*= -1*/)
{
	if (file == NULL)
		return NULL;
	INT_PTR len = file_len == -1 ? strlen(file) - 1 : file_len - 1;
	return GetFileSuffixName_Tmpl_(file, len);
}

const WCHAR* GetFileSuffixName(const WCHAR* file, INT_PTR file_len /*= -1*/)
{
	if (file == NULL)
		return NULL;
	INT_PTR len = file_len == -1 ? wcslen(file) - 1 : file_len - 1;
	return GetFileSuffixName_Tmpl_(file, len);
}

#if defined(WIN32)
static inline DWORD GetOSTempPath_Tmpl_(DWORD nBufferLength, char* lpBuffer)
{
	return GetTempPathA(nBufferLength, lpBuffer);
}
static inline DWORD GetOSTempPath_Tmpl_(DWORD nBufferLength, WCHAR* lpBuffer)
{
	return GetTempPathW(nBufferLength, lpBuffer);
}
#endif

BOOL GetTempPath(char path[MAX_PATH])
{
	if (NULL == path)
	{
		ASSERT(false);
		return false;
	}
#if defined(WIN32)
	int len;
	if ((len = (int)GetOSTempPath_Tmpl_(MAX_PATH, path)) <= 0)
		return false;
	if (!IsSlashChar(path[len - 1]))
	{
		path[len] = OS_DIR_SEP_CHAR;
		path[len + 1] = 0;
	}
#else
	strcpy(path, "/tmp/");
#endif
	return true;
}

BOOL GetTempPath(WCHAR path[MAX_PATH])
{
	if (NULL == path)
	{
		ASSERT(false);
		return false;
	}
#if defined(WIN32)
	int len;
	if ((len = (int)GetOSTempPath_Tmpl_(MAX_PATH, path)) <= 0)
		return false;
	if (!IsSlashChar(path[len - 1]))
	{
		path[len] = OS_DIR_SEP_CHAR;
		path[len + 1] = 0;
	}
#else
	wcscpy(path, L"/tmp/");
#endif
	return true;
}

static inline BOOL OSCreateDir_Tmpl_(const char* dir)
{
#if defined(WIN32)
	return !!CreateDirectoryA(dir, NULL);
#else
	return 0 == mkdir(dir, 0777);
#endif
}
static inline BOOL OSCreateDir_Tmpl_(const WCHAR* dir)
{
#if defined(WIN32)
	return !!CreateDirectoryW(dir, NULL);
#else
	char szDir[MAX_PATH] = { 0 };
	if (NULL == wctoc(dir, szDir, sizeof(szDir) - 1))
		return false;
	return 0 == mkdir(szDir, 0777);
#endif
}

CHAR_TYPE_INLINE BOOL OSCreatePath_Tmpl_(IN XCHAR_TYPE* path, int len)
{
	if (IsExistDir(path))
		return true;
#if defined(WIN32)
	if (ERROR_INVALID_NAME == GetLastError())
		return false;
#endif

	if (OSCreateDir_Tmpl_(path))
		return true;

	XCHAR_TYPE const slashStr[] = { '\\', '/', '\0' };
	XCHAR_TYPE* sep = (XCHAR_TYPE*)StrRChrRs(path, path + len - 1, slashStr);
	if (NULL == sep)
		return false;

	XCHAR_TYPE c = *sep;
	*sep = 0;
	BOOL ok = OSCreatePath_Tmpl_(path, sep - path);
	*sep = c;
	if (!ok)
		return false;

	ok = OSCreateDir_Tmpl_(path);	//如果此处失败，指定path将会部分创建成功
	return ok;
}

CHAR_TYPE_INLINE BOOL CreatePath_Tmpl_(const XCHAR_TYPE* lpszFullPath, INT_PTR len /*= -1*/)
{
	ASSERT(len > 0 && lpszFullPath != NULL);

	XCHAR_TYPE ch[MAX_PATH] = { 0 };
	if (len >= ARRAY_LEN(ch))
	{
		ASSERT(false);
		return false;
	}
	memcpy(ch, lpszFullPath, len * sizeof(XCHAR_TYPE));
	return OSCreatePath_Tmpl_(ch, len);
}

BOOL CreatePath(const char* lpszFullPath, INT_PTR len/* = -1*/)
{
	if (lpszFullPath == NULL)
		return false;
	INT_PTR nLen = len > 0 ? len : strlen(lpszFullPath);
	if (nLen <= 0)
		return false;
	return CreatePath_Tmpl_(lpszFullPath, nLen);
}

BOOL CreatePath(const WCHAR* lpszFullPath, INT_PTR len/* = -1*/)
{
	if (lpszFullPath == NULL)
		return false;
	INT_PTR nLen = len > 0 ? len : wcslen(lpszFullPath);
	if (nLen <= 0)
		return false;
	return CreatePath_Tmpl_(lpszFullPath, nLen);
}

CHAR_TYPE_INLINE const XCHAR_TYPE* MergeFileName_Tmpl_(const XCHAR_TYPE* path, const XCHAR_TYPE* file, XCHAR_TYPE pathname[MAX_PATH])
{
	if (NULL == path || pathname == NULL)
		return pathname;
	pathname[0] = 0;
	if (file == NULL)
	{
		strcpy_traits(pathname, file);
		return pathname;
	}

	if (0 == path[0])
	{
		strcpy_traits(pathname, file);
		return pathname;
	}
	if (0 == file[0])
	{
		strcpy_traits(pathname, path);
		return pathname;
	}
	if (IsAbsolutePath(file))
	{
		strcpy_traits(pathname, file);
		return pathname;
	}

#if defined(__linux__)
	if (IsAbsolutePath(path))
		pathname[0] = OS_DIR_SEP_CHAR;
#endif

	typedef std::basic_string<XCHAR_TYPE> XCHAR_STD_STRING;
	std::vector<XCHAR_STD_STRING> vec1;
	std::vector<XCHAR_STD_STRING> vec2;
	const XCHAR_TYPE* p1 = path;
	const XCHAR_TYPE* pPrev = path;
	while (*p1)
	{
		if (IsSlashChar(*p1))
		{
			int count = p1 - pPrev;
			if (count > 0)
			{
				XCHAR_STD_STRING s;
				s.assign(pPrev, p1 - pPrev);
				vec1.push_back(s);
			}
			pPrev = ++p1;
		}
		else
			++p1;
	}
	int count = p1 - pPrev;
	if (count > 0)
	{
		XCHAR_STD_STRING s;
		s.assign(pPrev, p1 - pPrev);
		vec1.push_back(s);
	}

	p1 = file;
	pPrev = file;
	while (*p1)
	{
		if (IsSlashChar(*p1))
		{
			int count = p1 - pPrev;
			if (count > 0)
			{
				XCHAR_STD_STRING s;
				s.assign(pPrev, p1 - pPrev);
				vec2.push_back(s);
			}
			pPrev = ++p1;
		}
		else
			++p1;
	}
	count = p1 - pPrev;
	if (count > 0)
	{
		XCHAR_STD_STRING s;
		s.assign(pPrev, p1 - pPrev);
		vec2.push_back(s);
	}

	XCHAR_TYPE level1[] = { '.', '\0' };
	XCHAR_TYPE level2[] = { '.', '.', '\0' };
	int step = 0;
	int step2 = 0;
	for (step = (int)vec2.size() - 1; step >= 0; --step)
	{
		if (vec2[step] == level1)
		{
			step2 = step + 1;
			--step;
			break;
		}
		if (vec2[step] == level2)
		{
			step2 = step + 1;
			break;
		}
	}
	++step;
	if (step >= (int)vec1.size())
	{
		strcpy_traits(pathname, file);
		return path;
	}

#if defined(WIN32)
	XCHAR_TYPE slash[] = { '\\', '\0' };
#elif defined(__linux__)
	XCHAR_TYPE slash[] = { '/', '\0' };
#endif
	for (size_t i = 0; i < vec1.size() - step; ++i)
	{
		strcat_traits(pathname, vec1[i].c_str());
		strcat_traits(pathname, slash);
	}
	for (size_t j = step2; j < vec2.size(); ++j)
	{
		strcat_traits(pathname, vec2[j].c_str());
		strcat_traits(pathname, slash);
	}
	pathname[strlen_traits(pathname) - 1] = '\0';
	return pathname;
}

const char* MergeFileName(const char* path, const char* file, char pathname[MAX_PATH])
{
	return MergeFileName_Tmpl_(path, file, pathname);
}

const WCHAR* MergeFileName(const WCHAR* path, const WCHAR* file, WCHAR pathname[MAX_PATH])
{
	return MergeFileName_Tmpl_(path, file, pathname);
}

INT64 GetFileSize(FILE* pf)
{
	if (NULL == pf)
	{
		ASSERT(false);
		return -1;
	}

#if defined(WIN32)
	DWORD dwLSize, dwHSize;
	dwLSize = GetFileSize(FILEToHandle(pf), &dwHSize);
	if (INVALID_FILE_SIZE == dwLSize && NO_ERROR != GetLastError())
		return -1;
	return (INT64(dwHSize) << 32) + INT64(dwLSize);
#elif defined(__linux__)
	off_t pos0 = ftello(pf);	//返回当前位置
	if (-1 == pos0)
		return -1;
	fseeko(pf, 0, SEEK_END);	//seek到文件尾
	off_t pos = ftello(pf);		//获取大小
	fseeko(pf, pos0, SEEK_SET);	//seek到原来位置
	return pos;
#endif
}

CHAR_TYPE_INLINE INT64 GetFileSize_Tmpl_(const CHAR_TYPE* lpszFile)
{
	if (lpszFile == NULL)
		return -1;

#if defined(WIN32)
	HANDLE hFile = CreateFile_Tmpl_(lpszFile,
		0,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_READONLY,
		NULL);
	if (INVALID_HANDLE_VALUE == hFile)
		return -1;

	DWORD dwLSize, dwHSize;
	dwLSize = GetFileSize(hFile, &dwHSize);
	if (INVALID_FILE_SIZE == dwLSize && NO_ERROR != GetLastError())
	{
		VERIFY(CloseHandle(hFile));
		return -1;
	}

	VERIFY(CloseHandle(hFile));
	return (INT64(dwHSize) << 32) + INT64(dwLSize);

#else
	struct stat fileinfo;
	if (stat_Tmpl_(lpszFile, &fileinfo) == 0)
		return fileinfo.st_size;
	else
		return -1;
#endif
}

INT64 GetFileSize(const char* lpszFile)
{
	return GetFileSize_Tmpl_(lpszFile);
}

INT64 GetFileSize(const WCHAR* lpszFile)
{
	return GetFileSize_Tmpl_(lpszFile);
}

OSFHANDLE FILEToHandle(FILE* pf)
{
#if defined(WIN32)
	return (OSFHANDLE)_get_osfhandle(fileno(pf));
#elif defined(__linux__)
	return fileno(pf);
#else
#error not support!!!
#endif
}

#if defined(__linux__)
BOOL ReadLinuxProcFile(const char* procFilePath, char buf[1024 * 128], int bufLen/* = -1*/)
{
	if (NULL == procFilePath || NULL == buf || strncmp(procFilePath, "/proc/", 6) != 0)
	{
		ASSERT(false);
		return false;
	}

	int fd = ::open(procFilePath, O_RDONLY);
	if (fd <= 0)
		return false;

	int si = ::read(fd, buf, bufLen <= 0 ? 1024 * 128 - 1 : bufLen - 1);
	VERIFY(0 == ::close(fd));
	if (si < 0)
		return false;

	buf[si] = 0;
	return true;
}
#endif