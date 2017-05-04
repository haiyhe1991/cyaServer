/* cyaPsMutex.h */
/* 进程锁 */

#ifndef __CYA_PS_MUTEX_H__
#define __CYA_PS_MUTEX_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaString.h"
#include "cyaStrCodec.h"
#include "cyaOS_opt.h"

#if defined(__linux__)
	#include <fcntl.h>
#endif

#if defined(WIN32)
	typedef struct OSPMutex__* OSPMutexH;
#elif defined(__linux__)
	typedef int OSPMutexH;
#endif

/* 一个进程终止后，其占有的进程锁会自动被释放
* 注意：同一个进程可以多次进入进程锁，进程锁没有重入计数
*/

/* 失败返回NULL */
OSPMutexH OSPMutexInit(LPCTSTR name, LPCTSTR tmp_path = NULL);
void OSPMutexDestroy(OSPMutexH mutex);

/* linux当timeout为指定超时时间(0, INFINITE)时，使用了循环试探查询方式，故效率不是很高，能否改为poll来实现?? */
BOOL OSPMutexTryLock(OSPMutexH mutex, DWORD timeout /* [0, INFINITE] */);
void OSPMutexUnlock(OSPMutexH mutex);

#if defined(__linux__)
	/* linux下支持读写锁 */
	/* forWrite，true - 写锁；false - 读锁 */
	BOOL OSPMutexTryLockEx(OSPMutexH mutex, DWORD timeout, BOOL forWrite);
#endif

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// 函数内联实现 ////////////////////////////////////
#if defined(WIN32)
inline OSPMutexH OSPMutexInit(LPCTSTR name, LPCTSTR /*tmp_path = NULL*/)
{
	HANDLE h;
	PathName globalName;
	VERIFY(_stprintf(globalName, _T("%s%s_OSPMUTEX"),
				GetWindowsVersion() >= Windows_NT ? _T("Global\\") : _T(""),
				name)
		< ARRAY_LEN(globalName));
	h = OpenMutex(MUTEX_ALL_ACCESS, false, globalName);
	if (NULL == h)
	{
		h = CreateMutex(NULL, false, globalName);
		if (NULL == h)
		{
			ASSERT(false);
			return NULL;
		}
	}
	return (OSPMutexH)h;
}

#ifdef VS_IS_VC2008
	#pragma warning(disable: 6387)
#endif

inline void OSPMutexDestroy(OSPMutexH mutex)
{
	ASSERT(mutex);
	VERIFY(CloseHandle((HANDLE)mutex));
}

inline BOOL OSPMutexTryLock(OSPMutexH mutex, DWORD timeout)
{
	DWORD ret;
	ASSERT(mutex);
	ret = WaitForSingleObject((HANDLE)mutex, timeout);
	return WAIT_OBJECT_0 == ret || WAIT_ABANDONED == ret; /* WAIT_ABANDONED表示拥有mutex的进程/线程被强行结束 */
}

inline void OSPMutexUnlock(OSPMutexH mutex)
{
	ASSERT(mutex);	ReleaseMutex((HANDLE)mutex);
}

#ifdef VS_IS_VC2008
	#pragma warning(default: 6387)
#endif

#elif defined(__linux__)	// #if defined(WIN32)
	inline OSPMutexH OSPMutexInit(LPCTSTR name, LPCTSTR tmp_path/* = NULL*/)
	{
		int fd;
		PathName globalName;
		char c_globalName[MAX_PATH];
		if (tmp_path && _tcslen(tmp_path) > 0)
		{
			if (tmp_path[_tcslen(tmp_path) - 1] != '/')
				VERIFY(_sntprintf(globalName, MAX_PATH, _T("%s/%s.OSPMUTEX"), tmp_path, name) < ARRAY_LEN(globalName));
			else
				VERIFY(_sntprintf(globalName, MAX_PATH, _T("%s%s.OSPMUTEX"), tmp_path, name) < ARRAY_LEN(globalName));
		}
		else
			VERIFY(_sntprintf(globalName, MAX_PATH, _T("/tmp/%s.OSPMUTEX"), name) < ARRAY_LEN(globalName));

		fd = open(tctoc(globalName, c_globalName, sizeof(c_globalName), -1, NULL), O_CREAT | O_TRUNC | O_RDWR, 0666);
		if (fd <= 0)
			return NULL;
		return (OSPMutexH)fd;
	}

	inline void OSPMutexDestroy(OSPMutexH mutex)
	{
		ASSERT(mutex); VERIFY(0 == close((FD_TYPE)mutex));
	}

	inline BOOL OSPMutexTryLockEx(OSPMutexH mutex, DWORD timeout, BOOL forWrite)
	{
		ASSERT(mutex);
		int ret;
		struct flock lock;
		lock.l_type = forWrite ? F_WRLCK : F_RDLCK;
		lock.l_whence = SEEK_SET;
		lock.l_start = 0;
		lock.l_len = 0; // lock all
		lock.l_pid = 0;

		if (0 == timeout)
		{
			ret = fcntl((FD_TYPE)mutex, F_SETLK, &lock); // F_SETLK为试探锁，立即返回
		}
		else if (INFINITE == timeout)
		{
			ret = fcntl((FD_TYPE)mutex, F_SETLKW, &lock); // F_SETLKW为阻塞锁，直到锁成功才返回
			ASSERT(0 == ret);
		}
		else
		{
			ret = fcntl((FD_TYPE)mutex, F_SETLK, &lock);
			if (0 != ret)
			{
				DWORD tick0 = GetTickCount();
				while (true)
				{
					Sleep(1);
					ret = fcntl((FD_TYPE)mutex, F_SETLK, &lock);
					if (0 == ret)
						break;
					else if (GetTickCount() - tick0 >= timeout)
						break;
				}
			}
		}

		return 0 == ret;
	}

	inline BOOL OSPMutexTryLock(OSPMutexH mutex, DWORD timeout)
	{
		return OSPMutexTryLockEx(mutex, timeout, true);
	}

	inline void OSPMutexUnlock(OSPMutexH mutex)
	{
		ASSERT(mutex);
		struct flock lock;
		lock.l_type = F_UNLCK;
		lock.l_whence = SEEK_SET;
		lock.l_start = 0;
		lock.l_len = 0;
		lock.l_pid = 0;
		VERIFY(0 == fcntl((FD_TYPE)mutex, F_SETLK, &lock));
	}
#endif // #elif defined(__linux__)

#ifdef __cplusplus

	//进程锁
	class CXPLocker
	{
	public:
		explicit CXPLocker(LPCTSTR name, LPCTSTR tmp_path = NULL)
		{
			mutex_ = OSPMutexInit(name, tmp_path);
			ASSERT(mutex_);
		}
		~CXPLocker()
		{
			if (mutex_)
				OSPMutexDestroy(mutex_);
		}

		void Lock()
		{
			VERIFY(OSPMutexTryLock(mutex_, INFINITE));
		}
		BOOL TryLock(DWORD timeout = 0)
		{
			return OSPMutexTryLock(mutex_, timeout);
		}
		void Unlock()
		{
			OSPMutexUnlock(mutex_);
		}
		int GetLockCount() const
		{
			ASSERT(false); /* 错误用法*/ return 0;
		}

	public:
		class AutoLock
		{
		private:
			AutoLock(const AutoLock&);
			AutoLock& operator= (const AutoLock&);

			CXPLocker& m_lock;
			BOOL const m_lockIt;

		public:
			explicit AutoLock(CXPLocker& mutex, BOOL lockIt = true)
				: m_lock(mutex)
				, m_lockIt(lockIt)
			{
				if (m_lockIt)
					m_lock.Lock();
				else
					m_lock.Unlock();
			}
			~AutoLock()
			{
				if (m_lockIt)
					m_lock.Unlock();
				else
					m_lock.Lock();
			}
		};

	private:
		CXPLocker(const CXPLocker&);
		CXPLocker& operator= (const CXPLocker&);
		OSPMutexH mutex_;
	};
#endif	//__cplusplus
#endif