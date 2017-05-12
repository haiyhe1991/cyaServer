#include <map>
#include <set>
#include "cyaLog.h"
#include "cyaFdk.h"
#include "cyaThread.h"
#include "moduleInfo.h"
#include "cyaLocker.h"
#include "cyaStrdk.h"
#include "cyaFile.h"
#include "cyaMaxMin.h"

#if defined(OS_IS_LINUX)
	#include <dirent.h>
#endif

#if defined(OS_IS_WINDOWS)
	#pragma warning(disable: 4353)
#endif

#define LOG_FILENAME_LEN _tcslen(_T("cya##############.log"))

#define MIN_LOG_FILE_SIZE		(1024*64)
#define MAX_LOG_FILE_SIZE		(1024*1024*8)
#define MAX_EVENT_TYPE			32
#define MAX_EVENT_TITLE			32
#define DATE_TIME_LEN			32 // YYYY-MM-DD hh:mm:ss|
#define MAX_LOG_SAVE_TIME		3600*24*7	//7天(s)
#define MAX_FLUSH_LINES			32
#define MAX_FILE_NUM			1000*20

// 需要异步初始化文件列表的最小日志文件数
#define MIN_COUNT_BY_ASYNCHRONOUS_INITIALIZATION	5000
#define DEFAULT_DISC_RESERVE_MBYTES					1024 // 默认磁盘保留空间
#define MIN_DISC_RESERVE_MBYTES						64	 // 最小磁盘保留空间
#define CLEAN_TIME_INTERVAL							(1000*1*1) // 清理时间间隔，毫秒

typedef std::set<tstring> TStringList_;

static inline tstring& GetLogDir_()
{
	BEGIN_LOCAL_SAFE_STATIC_OBJ(tstring, logDir);
	logDir = GetExeDir();
	END_LOCAL_SAFE_STATIC_OBJ(logDir);
}

static inline BOOL IsSelfCreateLogFile(const TCHAR* fullPathName)
{
	if (NULL == fullPathName)
		return false;
	const TCHAR* logFileName = GetFileName(fullPathName);
	if (_tcslen(logFileName) != LOG_FILENAME_LEN || _tcsncmp(logFileName, _T("cya"), _tcslen("cya")) != 0)
		return false;
#if defined(__linux__)
	const TCHAR* suffixName = GetFileSuffixName(logFileName);
	if (_tcscmp(suffixName, _T("log")) != 0)
		return false;
#endif
	return true;
}

#if defined(WIN32)
	inline void OutputDebugString_Tmpl_(const char* str)
	{
		OutputDebugStringA(str);
	}
	inline void OutputDebugString_Tmpl_(const WCHAR* str)
	{
		OutputDebugStringW(str);
	}
#endif

struct LogDataObj;
static BOOL CyaSearchLogFile_(const TCHAR* logDir, LogDataObj* parent, BOOL bSync, const TCHAR* extName = _T("*.log"));


struct LogDataObj
{
	typedef TCHAR LOGTITLE[MAX_EVENT_TITLE];

	CXTLocker lock_;
	DWORD mask_;						// 当前日志输出级别
	int flushLines_;					// 每flushLines_行flush一次log文件
	int fileBytesLimit_;				// 每[fileBytesLimit_] bytes换一个log文件
	BOOL screen_;						// 是否输出到屏幕
	BOOL toFile_;						// 是否记录日志到文件


	DWORD holdSecs_;					// 日志文件保留秒数，0保留到磁盘满
	LOGTITLE titles_[MAX_EVENT_TYPE];	// 各种事件类型的显示名称
	int prefixSepChar_;					// 头分隔字符
	int discReserveMBytes_;				// 日志目录的保留空间，兆字节(MB)，0表示忽略该参数
	int mediumType_;					// 保存空间的介质类型 0 -- 硬盘 1 -- 内存
	int maxFileNum_;					// 最大文件个数，0表示忽略该参数
	BOOL outputToDebuger_;				// 是否输出到调试器
	LTMSEL ltLog_;						// 上一条日志记录时间

	DWORD lastCleanTickByDatetime_;		// 上一次按时间日期清理日志的时间
	DWORD lastCleanTickByDiscFreeSize_;	// 上一次按磁盘保留空间清理日志的时间
	DWORD lastCleanTickByMaxFileNum_;	// 上一次按照文件最大数清理日志的时间

	struct LogFileList
	{
		OSThread initThread;	// 异步初始化线程，在log文件特别多（多余MIN_COUNT_BY_ASYNCHRONOUS_INITIALIZATION个）
		// 时用来异步初始内存里面的log文件列表
		OSThreadID initThreadID;
		BOOL exitFlag;

		TStringList_ listSet;	// 因为所有的log文件为规则名字，所以_listSet自然是按照时间排序的
		INT64 totalLogBytes;	// 所有的log文件的总尺寸（字节）
		BOOL initFinished;

		LogFileList()
			: initThread(INVALID_OSTHREAD)
			, initThreadID(INVALID_OSTHREADID)
			, exitFlag(false)
			, totalLogBytes(0)
			, initFinished(false)
		{
		}
		~LogFileList()
		{
			if (INVALID_OSTHREAD != initThread)
			{
				ASSERT(!exitFlag);
				exitFlag = true;
				OSCloseThread(initThread);
			}
		}
		static int InitWoker(void* /*param*/);
	};
	LogFileList logFileList_; // 已经关闭的日志集合

	struct TFile
	{
		CCyaFile f;
		LTMSEL lt;			// 创建时间
		int newLines;		// 当前文件新记录的行数
		TFile()
			: lt(MIN_UTC_MSEL), newLines(0) {}
	};
	TFile file_;			// 当前日志文件

private:
	LogDataObj()
	{
		mask_ = LOG_ALL_MASK;
		flushLines_ = MAX_FLUSH_LINES;
		fileBytesLimit_ = MAX_LOG_FILE_SIZE;
		screen_ = true;
		toFile_ = true;

		holdSecs_ = MAX_LOG_SAVE_TIME; // 7天
		discReserveMBytes_ = DEFAULT_DISC_RESERVE_MBYTES;
		mediumType_ = 0;	//磁盘
		maxFileNum_ = MAX_FILE_NUM;
#ifdef _DEBUG
		outputToDebuger_ = true;
#else
		outputToDebuger_ = false;
#endif

		for (int i = 0; i<MAX_EVENT_TYPE; i++)
			_stprintf(titles_[i], _T("%04d"), i);

		_tcscpy(titles_[LOG_INFO], _T("信息"));
		_tcscpy(titles_[LOG_WARNING], _T("警告"));
		_tcscpy(titles_[LOG_ERROR], _T("错误"));
		_tcscpy(titles_[LOG_DEBUG], _T("调试"));

		prefixSepChar_ = ':';

		ltLog_ = MIN_UTC_MSEL;

		lastCleanTickByDatetime_ = GetTickCount() - CLEAN_TIME_INTERVAL * 2;
		lastCleanTickByDiscFreeSize_ = lastCleanTickByDatetime_;
		lastCleanTickByMaxFileNum_ = lastCleanTickByDatetime_;

		// 搜索所有已有的log文件，以纳入管理

		if (CyaSearchLogFile_(CyaLogGetDir(), this, false))
			logFileList_.initFinished = true;
	}

public:
	~LogDataObj()
	{
	}

	static LogDataObj& GetInstance();
};

static BOOL CyaSearchLogFile_(const TCHAR* logDir, LogDataObj* parent, BOOL bSync, const TCHAR* extName /*= _T("*.log")*/)
{
	LogDataObj& logdata = *parent;//LogDataObj::GetInstance();
	ASSERT(!logdata.logFileList_.exitFlag);
#if defined(OS_IS_WINDOWS)
	TCHAR szFilePath[MAX_PATH] = { 0 };
	_tcscpy(szFilePath, logDir);
	_tcscat(szFilePath, extName);
	WIN32_FIND_DATA findData;
	HANDLE hd = ::FindFirstFile(szFilePath, &findData);
	if (hd == INVALID_HANDLE_VALUE)
		return true;

	do
	{
		TCHAR fullFileName[MAX_PATH] = { 0 };
		_tcscpy(fullFileName, logDir);
		_tcscat(fullFileName, findData.cFileName);
		if (!IsSelfCreateLogFile(fullFileName))
			continue;

		{
			CXTAutoLock locker(logdata.lock_);
			if (logdata.logFileList_.listSet.insert(fullFileName).second)
				logdata.logFileList_.totalLogBytes += GetFileSize(fullFileName);

			// 发现log文件过多，开始异步继续初始化
			if (!bSync && logdata.logFileList_.listSet.size() >= MIN_COUNT_BY_ASYNCHRONOUS_INITIALIZATION)
			{
				ASSERT(INVALID_OSTHREAD == logdata.logFileList_.initThread);
				VERIFY(OSCreateThread(&logdata.logFileList_.initThread,
					&logdata.logFileList_.initThreadID,
					LogDataObj::LogFileList::InitWoker,
					NULL, 0));
				return false;
			}
		}
	} while (FindNextFile(hd, &findData));
	FindClose(hd);
	return true;

#elif defined(OS_IS_LINUX)
	SCAN_FILE_FILTER scanfilter;
	scanfilter.count = scandir(logDir, &scanfilter.nameList, SCAN_FILE_FILTER::scandir_filter_, alphasort);
	int count = scanfilter.count;
	struct dirent** nameList = scanfilter.nameList;
	if (count <= 0 || nameList == NULL)
		return true;
	for (int i = 0; i < count; ++i)
	{
		char fullPath[MAX_PATH] = { 0 };
		strcpy(fullPath, logDir);
		strcat(fullPath, nameList[i]->d_name);

		TCHAR tcFilepathName[MAX_PATH] = { 0 };
		const TCHAR* filePathname = ctotc(fullPath, tcFilepathName, sizeof(tcFilepathName) - sizeof(TCHAR));
		if (!IsSelfCreateLogFile(filePathname))
			continue;

		{
			CXTAutoLock locker(logdata.lock_);
			if (logdata.logFileList_.listSet.insert(filePathname).second)
				logdata.logFileList_.totalLogBytes += GetFileSize(filePathname);

			// 发现log文件过多，开始异步继续初始化
			if (!bSync && logdata.logFileList_.listSet.size() >= MIN_COUNT_BY_ASYNCHRONOUS_INITIALIZATION)
			{
				ASSERT(INVALID_OSTHREAD == logdata.logFileList_.initThread);
				VERIFY(OSCreateThread(&logdata.logFileList_.initThread,
					&logdata.logFileList_.initThreadID,
					LogDataObj::LogFileList::InitWoker,
					NULL, 0));
				return false;
			}
		}
	}
	return true;
#endif
}

int LogDataObj::LogFileList::InitWoker(void* /*param*/)
{
	CyaSearchLogFile_(CyaLogGetDir(), &LogDataObj::GetInstance(), true);
	LogDataObj::GetInstance().logFileList_.initFinished = true;
	return 0;
}

LogDataObj& LogDataObj::GetInstance()
{
	BEGIN_LOCAL_SAFE_STATIC_OBJ(LogDataObj, logObj);
	END_LOCAL_SAFE_STATIC_OBJ(logObj);
}

CYALOG_API BOOL CyaLogSetDir(LPCTSTR dir)
{
	tstring& logDir = GetLogDir_();
	if (NULL == dir || _T('\0') == *dir)
	{
		logDir = GetExeDir();
		return true;
	}

	tstring oldDir = logDir;
	if (IsAbsolutePath(dir))
		logDir = dir;
	else
	{
		TCHAR pathname[MAX_PATH] = { 0 };
		logDir = (LPCTSTR)MergeFileName(GetExeDir(), dir, pathname);
	}
	if (!CreatePath(logDir.c_str()))
	{
		logDir = oldDir;
		return false;
	}

	if (!IsSlashChar(logDir[logDir.length() - 1]))
		logDir += OS_DIR_SEP_CHAR_STR;

	return true;
}

CYALOG_API LPCTSTR CyaLogGetDir()
{
	return GetLogDir_().c_str();
}

CYALOG_API void CyaLogSetFlushLines(int n)
{
	LogDataObj::GetInstance().flushLines_ = max(n, 1);
}

CYALOG_API int CyaLogGetFlushLines()
{
	return LogDataObj::GetInstance().flushLines_;
}

CYALOG_API void CyaLogSetFileSizeLimit(int bytes)
{
	LogDataObj::GetInstance().fileBytesLimit_ = bytes;
}

CYALOG_API int CyaLogGetFileSizeLimit()
{
	return LogDataObj::GetInstance().fileBytesLimit_;
}

CYALOG_API void CyaLogSetOutScreen(BOOL onoff)
{
	LogDataObj::GetInstance().screen_ = onoff;
}

CYALOG_API BOOL CyaLogIsOutScreen()
{
	return LogDataObj::GetInstance().screen_;
}

CYALOG_API void CyaLogSetToFile(BOOL onoff)
{
	LogDataObj::GetInstance().toFile_ = onoff;
}

CYALOG_API BOOL CyaLogIsToFile()
{
	return LogDataObj::GetInstance().toFile_;
}

CYALOG_API void CyaLogSetLife2(int secs)
{
	LogDataObj::GetInstance().holdSecs_ = max(secs, 0);
}

CYALOG_API int CyaLogGetLife2()
{
	return LogDataObj::GetInstance().holdSecs_;
}

CYALOG_API void CyaLogSetLife(int days)
{
	ASSERT(0<days && days <= 365); CyaLogSetLife2(days * 24 * 3600);
}

CYALOG_API int CyaLogGetLife()
{
	return CyaLogGetLife2() / (24 * 3600);
}

CYALOG_API LPCTSTR CyaLogGetEventTitle(int logType)
{
	if (logType < 0 || logType >= MAX_EVENT_TYPE)
		return _T("");
	return LogDataObj::GetInstance().titles_[logType];
}

CYALOG_API void CyaLogSetEventTitle(int logType, LPCTSTR title)
{
	if (logType >= 0 && logType < MAX_EVENT_TYPE)
	{
		if (title && (int)_tcslen(title) < MAX_EVENT_TITLE)
			_tcscpy(LogDataObj::GetInstance().titles_[logType], title);
	}
}

CYALOG_API LPCTSTR CyaLogGetFileName()
{
	if (LogDataObj::GetInstance().file_.f.IsOpened())
		return LogDataObj::GetInstance().file_.f.FileName();
	else
		return _T("");
}

//根据当前tick得到log文件名
static LPCTSTR GetTickLogFileName_(TCHAR szLogFile[MAX_PATH])
{
	TCHAR szDate[32];
	MselToStr(GetMsel(), szDate, true/*false*/);
	LPCTSTR path = CyaLogGetDir();
	sprintf_traits(szLogFile,
		_T("%s%scya%s.log"),
		path,
		(IsSlashChar(*GetLastStr(path)) ? _T("") : OS_DIR_SEP_CHAR_STR),
		szDate);
	return szLogFile;
}

// 新文件返回true
static LPCTSTR GetNewLogFileName_(TCHAR sz[MAX_PATH], BOOL& file_is_valid)
{
	GetTickLogFileName_(sz);
	file_is_valid = IsExistFile(sz);
	return sz;
}

//文件切换
static void CyaLogSwitchFile_()
{
	LogDataObj& logdata = LogDataObj::GetInstance();
	if (!logdata.toFile_)
		return;

	BOOL isFirst = true;
	if (logdata.file_.f.IsOpened())
	{
		isFirst = false;
		int file_size = (int)GetFileSize(logdata.file_.f);
		if (file_size >= logdata.fileBytesLimit_)
		{
			tstring filename = logdata.file_.f.FileName();
			logdata.file_.f.Close();
			logdata.file_.lt = MIN_UTC_MSEL;
			logdata.file_.newLines = 0;

			//	ASSERT(file_size == (long)GetFileSize(filename.c_str()));
			{
				//CXTAutoLock locker(logdata.lock_);
				if (logdata.logFileList_.listSet.insert(filename).second)
					logdata.logFileList_.totalLogBytes += (long)GetFileSize(filename.c_str());
			}
		}
	}

	if (!logdata.file_.f.IsOpened())
	{
		TCHAR ch[MAX_PATH];
		BOOL file_is_valid = false;
		BOOL appendOld = false;
		if (isFirst && logdata.logFileList_.listSet.size() > 0)
		{
			tstring name = *logdata.logFileList_.listSet.rbegin();
			long file_size = (long)GetFileSize(name.c_str());
			if (file_size < logdata.fileBytesLimit_ / 2)
			{
				// 当最后一个log文件size还比较小时，不使用新的log文件，直接追加到最后一个log文件
				// 用于避免当程序频繁启动产生大量的小log文件
				_tcscpy(ch, name.c_str());
				file_is_valid = true;
				appendOld = true;

				// 清除其文件列表记录
				logdata.logFileList_.totalLogBytes -= file_size;
				VERIFY(1 == logdata.logFileList_.listSet.erase(name));
			}
		}
		if (!appendOld)
			GetNewLogFileName_(ch, file_is_valid);

		if (file_is_valid)
		{
			if (!logdata.file_.f.Open(ch, _T("rb+"))) // 不能用 "ab+"方式打开，此时fseek(fp, 0, SEEK_SET)为原来文件的末尾
				return;
			fseek(logdata.file_.f, 0, SEEK_END); // 如果日志记录得太快，可能本次的文件名会和上一次的名字相同，
			// 此时必须在原来的日志末尾追加记录，所以必须用追加方式打开
		}
		else
		{
			if (!logdata.file_.f.Open(ch, _T("wb")))
				return;
		}

		logdata.file_.newLines = 0;
	}
}

// 返回值：是否有清理动作
static BOOL CyaLogCleanLogByDatetime_()
{
	LogDataObj& logdata = LogDataObj::GetInstance();
	ASSERT(logdata.logFileList_.initFinished);
	ASSERT(logdata.holdSecs_ > 0);

	DWORD tick = GetTickCount();
	if (tick - logdata.lastCleanTickByDatetime_ < CLEAN_TIME_INTERVAL)
		return false;

	BOOL haveLogFileCleaned = false;
	for (TStringList_::iterator it = logdata.logFileList_.listSet.begin(); it != logdata.logFileList_.listSet.end();)
	{
		const tstring& pathname = *it;
		LPCTSTR sep = StrRChrRs(pathname.c_str(), pathname.c_str() + pathname.size() - 1, _T("/\\"));
		if (NULL == sep)
		{
			ASSERT(false);
			return haveLogFileCleaned;
		}
		LPCTSTR file_ = sep + 1;

		LTMSEL lt = StrToMsel(file_ + 3, true/*false*/); //去掉前面的3个字母，cya
		if (INVALID_UTC_MSEL == lt)
		{
			ASSERT(false);
			return haveLogFileCleaned;
		}

		LTMSEL ltNow = GetMsel();
		if (ltNow - lt > (LTMSEL)(LogDataObj::GetInstance().holdSecs_) * 1000)		//超过保留时间需要清理
		{
			INT64 sz = GetFileSize(pathname.c_str());
			ASSERT(logdata.logFileList_.totalLogBytes >= sz);
			char szPathName[MAX_PATH] = { 0 };
			unlink(tctoc(pathname.c_str(), szPathName, sizeof(szPathName) - sizeof(char)));

			logdata.logFileList_.listSet.erase(it++);
			logdata.logFileList_.totalLogBytes -= sz;
			haveLogFileCleaned = true;
		}
		else
		{
			break;	//排在前面的文件都不需要清理,后面的则都不需要清理
		}
	}
	logdata.lastCleanTickByDatetime_ = GetTickCount();
	return haveLogFileCleaned;
}

// 返回值：是否有清理动作
static BOOL CyaLogCleanLogByDiscReserveSize_(int disc_reserve_MBytes)
{
	LogDataObj& logdata = LogDataObj::GetInstance();
	ASSERT(logdata.logFileList_.initFinished);
	ASSERT(disc_reserve_MBytes > 0);

	DWORD tick = GetTickCount();
	if (tick - logdata.lastCleanTickByDatetime_ < CLEAN_TIME_INTERVAL)
		return false;

	//	CXTAutoLock locker(logdata.lock_);
	LPCTSTR log_dir = CyaLogGetDir();
	INT64 totalFreeBytes = 0;
	if (0 == logdata.mediumType_)
		totalFreeBytes = OSGetDiskSpace(log_dir);
	else if (1 == logdata.mediumType_)
	{
		totalFreeBytes = OSGetSysFreeMem();
		if (0 == totalFreeBytes)	// 获取空闲内存失败
			return false;
	}
	else
		ASSERT(false);

	INT64 need_clean_bytes = ((INT64)disc_reserve_MBytes * 1024 * 1024) - totalFreeBytes;
	if (need_clean_bytes <= 0)
	{
		logdata.lastCleanTickByDiscFreeSize_ = tick;
		return false;
	}

	BOOL haveLogFileCleaned = false;
	for (TStringList_::iterator it = logdata.logFileList_.listSet.begin(); it != logdata.logFileList_.listSet.end();)
	{
		const tstring& pathname = *it;
		INT64 sz = GetFileSize(pathname.c_str());
		ASSERT(logdata.logFileList_.totalLogBytes >= sz);
		char szPathName[MAX_PATH] = { 0 };
		unlink(tctoc(pathname.c_str(), szPathName, sizeof(szPathName) - sizeof(char)));

		logdata.logFileList_.listSet.erase(it++);
		logdata.logFileList_.totalLogBytes -= sz;
		haveLogFileCleaned = true;
		need_clean_bytes -= sz;
		if (need_clean_bytes <= 0)
			break;
	}
	logdata.lastCleanTickByDiscFreeSize_ = GetTickCount();
	return haveLogFileCleaned;
}

// 返回值：是否有清理动作
static BOOL CyaLogCleanLogByMaxFileNum_(int maxFileNum)
{
	LogDataObj& logdata = LogDataObj::GetInstance();
	ASSERT(logdata.logFileList_.initFinished);
	ASSERT(maxFileNum > 0);

	DWORD tick = GetTickCount();
	if (tick - logdata.lastCleanTickByDatetime_ < CLEAN_TIME_INTERVAL)
		return false;

	//CXTAutoLock locker(logdata.lock_);
	BOOL need_clean = (int)logdata.logFileList_.listSet.size() > maxFileNum;
	if (!need_clean)
	{
		logdata.lastCleanTickByMaxFileNum_ = tick;
		return false;
	}

	BOOL haveLogFileCleaned = false;
	for (TStringList_::iterator it = logdata.logFileList_.listSet.begin(); it != logdata.logFileList_.listSet.end();)
	{
		const tstring& pathname = *it;
		char szPathName[MAX_PATH] = { 0 };
		unlink(tctoc(pathname.c_str(), szPathName, sizeof(szPathName) - sizeof(char)));

		logdata.logFileList_.listSet.erase(it++);
		if ((int)logdata.logFileList_.listSet.size() <= maxFileNum)
			break;
	}
	logdata.lastCleanTickByMaxFileNum_ = GetTickCount();
	return haveLogFileCleaned;
}

static void CyaLogAutoClean_()
{
	LogDataObj& logdata = LogDataObj::GetInstance();

	// 初始化文件列表还未完成，不做清理
	if (!logdata.logFileList_.initFinished)
		return;

	// 清理过期的日志文件
	if (logdata.holdSecs_ > 0)
	{
		if (CyaLogCleanLogByDatetime_())
			return;
	}

	// 按照磁盘保留尺寸清理
	if (logdata.discReserveMBytes_ > 0)
		CyaLogCleanLogByDiscReserveSize_(logdata.discReserveMBytes_);

	if (logdata.maxFileNum_ > 0)
		CyaLogCleanLogByMaxFileNum_(logdata.maxFileNum_);
}

static void CyaLogLine_Tmpl_(int logType, const char* prefix, int prefixLen, const char* line, int lineLen)
{
	ASSERT(prefix != NULL && prefixLen > 0 && line != NULL && lineLen > 0);
	LogDataObj& logdata = LogDataObj::GetInstance();
	if (logdata.screen_)
	{
		// 日志文件总用多字节编码方式
		if (LOG_FILE_LINE_INFO != logType)
			fwrite((const char*)prefix, 1, prefixLen, stderr);
		fwrite((const char*)line, 1, lineLen, stderr);
		if (LOG_FILE_LINE_INFO != logType)
			fputs("\r\n", stderr);

#if defined(WIN32)
		if (LOG_FILE_LINE_INFO != logType)
		{
			OutputDebugString_Tmpl_(prefix);
			OutputDebugString_Tmpl_(line);
			OutputDebugString_Tmpl_("\n");
		}
#endif
	}

	if (logdata.toFile_ && logdata.file_.f.IsOpened())
	{
		// 日志文件总用多字节编码方式
		if (LOG_FILE_LINE_INFO != logType)
			fwrite((const char*)prefix, 1, prefixLen, logdata.file_.f);
		fwrite((const char*)line, 1, lineLen, logdata.file_.f);
		if (LOG_FILE_LINE_INFO != logType)
		{
			fputs("\r\n", logdata.file_.f);
			logdata.file_.newLines++;
		}
	}
}

static void CyaLogTextX_Tmpl_(int logType, const TCHAR* text)
{
	LogDataObj& logdata = LogDataObj::GetInstance();
	CXTAutoLock locker(logdata.lock_);

	if (NULL == text)
		text = _T("NULL");
	ASSERT(logType >= 0 && logType < MAX_EVENT_TYPE);

	// 自动清理日志文件
	CyaLogAutoClean_();

	// 合成日期时间logType前缀
	LTMSEL const ltNow = GetMsel();
	SYSTEMTIME const t_now = ToTime(ltNow);
	TCHAR ch_t[32] = { 0 };
	sprintf_traits(ch_t, _T("%04d-%02d-%02d %02d:%02d:%02d"), t_now.wYear, t_now.wMonth, t_now.wDay, t_now.wHour, t_now.wMinute, t_now.wSecond);


	char prefix[DATE_TIME_LEN + MAX_EVENT_TITLE] = { 0 };
	int prefixLen = sprintf_traits(prefix,
		_T("%s%s|%s%c"),
		ch_t,
		(const TCHAR*)_T(""),
		logdata.titles_[logType],
		(char)logdata.prefixSepChar_);

	// 决定是否换文件
	if (logdata.ltLog_ / 1000 != ltNow / 1000) // 如果在开始一个新的日志文件后系统发生了校时动作，可能发生tNow < logdata.tLog_
		CyaLogSwitchFile_(); // 发生换秒才调用
	else
		ASSERT(true);

	logdata.ltLog_ = ltNow;

	{
		while (true)
		{
			const char* p = text;
			while (*p && *p != '\n')
				++p;
			int n = (int)(p - text);
			if (*p)
			{
				CyaLogLine_Tmpl_(logType,
					(LOG_FILE_LINE_INFO == logType ? (const TCHAR*)_T("") : prefix),
					(LOG_FILE_LINE_INFO == logType ? 0 : prefixLen), text, n);
				--n;
				++p;
				if (!(*p))
					break;
				text = p;
			}
			else
			{
				CyaLogLine_Tmpl_(logType,
					(LOG_FILE_LINE_INFO == logType ? (const TCHAR*)_T("") : prefix),
					(LOG_FILE_LINE_INFO == logType ? 0 : prefixLen), text, n);
				break;
			}
		}
	}

	// 刷新日志文件
	if (logdata.toFile_ && logdata.file_.f.IsOpened() && logdata.file_.newLines >= logdata.flushLines_)
	{
		logdata.file_.newLines = 0;
		fflush(logdata.file_.f);
	}
}

static void CyaLogEventX_Tmpl_(int logType, const TCHAR* fmt, va_list va)
{
#if !defined(_DEBUG)
	if (logType == LOG_DEBUG)
		return;
#endif
	ASSERT(fmt && logType >= 0 && logType < MAX_EVENT_TYPE);
	int n = calcfmtlen_traits(fmt, va);
	if (n <= 0)
		return;

	if (n < 32 * 1024)
	{
		TCHAR buf[32 * 1024] = { 0 };
		int r = sprintf_traits(buf, fmt, va);
		if (r > 0)
			CyaLogTextX_Tmpl_(logType, buf);
	}
	else
	{
		TCHAR* buf = (TCHAR*)malloc(n + sizeof(TCHAR));
		if (buf == NULL)
			return;
		int r = sprintf_traits(buf, fmt, va);
		if (r > 0)
			CyaLogTextX_Tmpl_(logType, buf);
		free(buf);
	}
}

CYALOG_API void CyaLogEventX(int logType, LPCTSTR fmt, va_list va)
{
	CyaLogEventX_Tmpl_(logType, fmt, va);
}

CYALOG_API void CyaLogFlushFile()
{
	LogDataObj& logdata = LogDataObj::GetInstance();
	CXTAutoLock locker(logdata.lock_);

	if (logdata.file_.f.IsOpened())
	{
		if (logdata.file_.newLines >= logdata.flushLines_)
		{
			logdata.file_.newLines = 0;
			fflush(logdata.file_.f);
		}
	}
}

CYALOG_API void CyaLogSetReserve(int MBytes, int mediumType/*= 0*/)
{
	LogDataObj& logObj = LogDataObj::GetInstance();

	if (MBytes < MIN_DISC_RESERVE_MBYTES && mediumType == 0)
		MBytes = MIN_DISC_RESERVE_MBYTES;

	ASSERT(mediumType == 0 || mediumType == 1);
	logObj.mediumType_ = mediumType;
	logObj.discReserveMBytes_ = MBytes;
}

CYALOG_API int CyaLogGetReserve(int* mediumType /*= NULL*/)
{
	if (mediumType)
		*mediumType = LogDataObj::GetInstance().mediumType_;
	return LogDataObj::GetInstance().discReserveMBytes_;
}

CYALOG_API void CyaLogSetMaxFileNum(int maxFileNum)
{
	LogDataObj::GetInstance().maxFileNum_ = maxFileNum;
}

CYALOG_API int CyaLogGetMaxFileNum()
{
	return LogDataObj::GetInstance().maxFileNum_;
}

CYALOG_API void CyaLogOutputToDebuger(BOOL en)
{
	LogDataObj::GetInstance().outputToDebuger_ = en;
}

CYALOG_API BOOL CyaLogIsOutputDebuger()
{
	return LogDataObj::GetInstance().outputToDebuger_;
}