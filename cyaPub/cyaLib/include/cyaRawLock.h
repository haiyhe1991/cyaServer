/* roserawlock.h
*
* 原始通用线程锁（无调试辅助信息，OSMutex和CXTLocker有调试辅助信息）
*/

#ifndef __CYA_RAW_LOCK_H__
#define __CYA_RAW_LOCK_H__

#if defined(_MSC_VER) && (_MSC_VER > 1000)
	#pragma once
#endif

#include "cyaTypes.h"
#include "cyaTime.h"

#if defined(WIN32)
	typedef CRITICAL_SECTION OSRawLocker;
#elif defined(__linux__)
	typedef pthread_mutex_t OSRawLocker;
#else
	#error Unsupported platform.
#endif

void OSRawMutexInit(OSRawLocker* mutex, DWORD spinCount DEFAULT_PARAM(0));
void OSRawMutexDestroy(OSRawLocker* mutex);
void OSRawMutexLock(OSRawLocker* mutex);
BOOL OSRawMutexTryLock(OSRawLocker* mutex, DWORD timeout DEFAULT_PARAM(0));
void OSRawMutexUnlock(OSRawLocker* mutex);

#ifdef __cplusplus

class COSRawLock
{
	OSRawLocker m_mutex;

private:
	COSRawLock(const COSRawLock&);
	COSRawLock& operator= (const COSRawLock&);

public:
	explicit COSRawLock(DWORD spinCount = 0)
	{
		OSRawMutexInit(&m_mutex, spinCount);
	}
	~COSRawLock()
	{
		OSRawMutexDestroy(&m_mutex);
	}

	void Lock()
	{
		OSRawMutexLock(&m_mutex);
	}
	BOOL TryLock(DWORD timeout = 0)
	{
		return OSRawMutexTryLock(&m_mutex, timeout);
	}
	void Unlock()
	{
		OSRawMutexUnlock(&m_mutex);
	}

public:
	class AutoLock
	{
	private:
		AutoLock(const AutoLock&);
		AutoLock& operator= (const AutoLock&);

		COSRawLock& m_lock;
		BOOL const m_lockIt;

	public:
		explicit AutoLock(COSRawLock& mutex, BOOL lockIt = true)
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

	class AutoTryLock
	{
	private:
		AutoTryLock(const AutoTryLock&);
		AutoTryLock& operator= (const AutoTryLock&);

		COSRawLock& m_lock;
		BOOL		m_isLocked;

	public:
		explicit AutoTryLock(COSRawLock& mutex, DWORD timeout = 0, BOOL lockIt = true)
			: m_lock(mutex)
			, m_isLocked(false)
		{
			if (lockIt)
				m_isLocked = m_lock.TryLock(timeout);
			else
				m_isLocked = false;
		}
		~AutoTryLock()
		{
			if (m_isLocked)
			{
				m_lock.Unlock();
				m_isLocked = false;
			}
		}
		BOOL IsLocked()
		{
			return m_isLocked;
		}
	};
};
#endif	/* #ifdef __cplusplus */

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++++++++++函数实现++++++++++++++++++++++++++++*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#if defined(WIN32)
inline BOOL OnTryEnterCriticalSection(LPCRITICAL_SECTION cs)
{
	typedef BOOL(WINAPI* FUNC)(CRITICAL_SECTION*);
	static FUNC fn = NULL;
	if (NULL == fn)
		fn = (FUNC)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "TryEnterCriticalSection");
	return fn(cs);
}

inline BOOL OnInitializeCriticalSectionAndSpinCount(LPCRITICAL_SECTION cs, DWORD spinCount)
{
	typedef BOOL(WINAPI* FUNC)(CRITICAL_SECTION*, DWORD);
	static FUNC fn = NULL;
	if (NULL == fn)
		fn = (FUNC)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "InitializeCriticalSectionAndSpinCount");
	return fn(cs, spinCount);
}

inline DWORD OnSetCriticalSectionSpinCount(LPCRITICAL_SECTION cs, DWORD spinCount)
{
	typedef DWORD(WINAPI* FUNC)(CRITICAL_SECTION*, DWORD);
	static FUNC fn = NULL;
	if (NULL == fn)
		fn = (FUNC)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "SetCriticalSectionSpinCount");
	return fn(cs, spinCount);
}
#endif

inline void OSRawMutexInit(OSRawLocker* mutex, DWORD spinCount /*= 0*/)
{
#if defined(WIN32)
	OnInitializeCriticalSectionAndSpinCount(mutex, spinCount);	//自旋锁
#elif defined(__linux__)
	#if defined(OS_IS_APPLE) || defined(OS_IS_ANDROID)
		pthread_mutex_init(mutex, NULL);
	#elif defined(__USE_GNU)
		pthread_mutex_t m = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
		*mutex = m;
	#else
		#ifdef __cplusplus
			pthread_mutex_t m = { 0, 0, 0, PTHREAD_MUTEX_RECURSIVE_NP, { 0, 0 } };
			*mutex = m;
		#else
			pthread_mutex_t m = { { 0, 0, 0, PTHREAD_MUTEX_RECURSIVE_NP, 0, { 0 } } };
			*mutex = m;
		#endif
	#endif
#else
	#error Unsupported platform.
#endif
}

inline void OSRawMutexDestroy(OSRawLocker* mutex)
{
#if defined(WIN32)
	DeleteCriticalSection(mutex);
#elif defined(OS_IS_APPLE) || defined(OS_IS_ANDROID)
	pthread_mutex_destroy(mutex);
#else
	(void)mutex;
#endif
}

inline void OSRawMutexLock(OSRawLocker* mutex)
{
#if defined(WIN32)
	EnterCriticalSection(mutex);
#elif defined(__linux__)
	pthread_mutex_lock(mutex);
#else
	#error Unsupported platform.
#endif
}

inline BOOL OnOSRawMutexTryLock(OSRawLocker* mutex)
{
#if defined(WIN32)
	BOOL ok = OnTryEnterCriticalSection(mutex);
#elif defined(__linux__)
	int r = pthread_mutex_trylock(mutex);
	BOOL ok = (0 == r);
	ASSERT(ok || EBUSY == r);
#else
#error Unsupported platform.
#endif
	return ok;
}

inline BOOL OSRawMutexTryLock(OSRawLocker* mutex, DWORD timeout /*DEFAULT_PARAM(0)*/)
{
	if (INFINITE == timeout)
	{
		OSRawMutexLock(mutex);
		return true;
	}
	else if (0 == timeout)
		return OnOSRawMutexTryLock(mutex);
	else
	{
		DWORD tick0;
		if (OnOSRawMutexTryLock(mutex))
			return true;
		tick0 = GetTickCount();
		while (true)
		{
			Sleep(1);
			if (OnOSRawMutexTryLock(mutex))
				return true;
			else if (GetTickCount() - tick0 >= timeout)
				break;
		}
		return false;
	}
}

inline void OSRawMutexUnlock(OSRawLocker* mutex)
{
#if defined(WIN32)
	LeaveCriticalSection(mutex);
#elif defined(__linux__)
	pthread_mutex_unlock(mutex);
#else
#error Unsupported platform.
#endif
}

#endif