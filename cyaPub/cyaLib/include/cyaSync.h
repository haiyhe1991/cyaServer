#ifndef __CYA_SYNC_H__
#define __CYA_SYNC_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#if defined(__linux__)
	#include <semaphore.h>
	#include <poll.h>
	#include <fcntl.h>
#endif

#include "cyaRawLock.h"
#include "cyaThread.h"
#include "cyaTime.h"
#include "cyaAtomic.h"

/*************************** 性能测试 ***************************
...
*************************** 性能测试 ***************************/

#if defined(_MSC_VER) && !defined(__cplusplus)
	#pragma warning(disable: 4201)
#endif

#if defined(WIN32)
typedef HANDLE OSSema;
typedef HANDLE OSEvent;

#elif defined(__linux__)
typedef sem_t OSSema;
typedef struct { int h[2]; long count; } OSEvent;

#endif

/* 信号量 */
/*ios下不支持创建无名的信号量*/
#if defined(OS_IS_APPLE)
	OSSema* OSSemaInit(const char* semaName, int initValue DEFAULT_PARAM(0), int* err = NULL);
	void OSSemaDestroy(OSSema* sema, const char* semaName);
#else
	BOOL OSSemaInit(OSSema* sema, int initValue DEFAULT_PARAM(0), LONG maxCount DEFAULT_PARAM(0x7fffffff));
	void OSSemaDestroy(OSSema* sema);
#endif
BOOL OSSemaPost(OSSema* sema, int n DEFAULT_PARAM(1));
BOOL OSSemaWait(OSSema* sema, DWORD timeout DEFAULT_PARAM(INFINITE));

/* 事件 */

/* auto-reset, initial false */
BOOL OSEventInit(OSEvent* ev);
void OSEventDestroy(OSEvent* ev);
/* 注意, linux平台，OSEvent在Wait之前多次设置的结果是未定义的 */
void OSEventSet(OSEvent* ev);
/* 等待事件，并复位信号 */
BOOL OSEventWait(OSEvent* ev, DWORD timeout DEFAULT_PARAM(INFINITE));

/* 得到句柄， */
OSHandle OSEventGetHandle(const OSEvent* ev);

#ifdef VS_IS_VC2008
	#pragma warning(disable: 6011)
	#pragma warning(disable: 6386)
#endif

inline int FDPoll(FD_TYPE fd, int events, DWORD timeout, FD_TYPE forceBreakFD DEFAULT_PARAM(INVALID_SOCKET))
{
	int n;
	{
		int pollTimeout = (INFINITE == timeout ? -1 : (int)(timeout & 0x7FFFFFFF));
#if defined(__linux__)
		struct pollfd pfds[2];
		int poll_count = 1;

		pfds[0].fd = fd;
		pfds[0].events = 0;
		if (events & 0x1)
			pfds[0].events |= POLLIN;
		if (events & 0x2)
			pfds[0].events |= POLLPRI;
		if (events & 0x4)
			pfds[0].events |= POLLOUT;
		pfds[0].revents = 0;

		if (FDValid(forceBreakFD))
		{
			pfds[1].fd = forceBreakFD;
			pfds[1].events = POLLIN;
			poll_count = 2;
		}

		n = poll(pfds, poll_count, pollTimeout);
		while (n<0 && errno == EINTR)
		{
			if (pollTimeout == 0)
				break;
			else if (0<pollTimeout && pollTimeout <= 10)
			{
				Sleep(pollTimeout);
				pollTimeout = 0;
			}
			else if (pollTimeout > 10)
			{
				Sleep(10);
				pollTimeout -= 10;
			}
			else
			{
				ASSERT(-1 == pollTimeout);
			}
			n = poll(pfds, poll_count, pollTimeout);
		}
		if (n <= 0)
			return n;

		short revents = (pfds[0].events & pfds[0].revents);
		if (0 != revents)
		{
			n = 0;
			if (revents&POLLIN)
				n |= 1;
			if (revents&POLLPRI)
				n |= 2;
			if (revents&POLLOUT)
				n |= 4;
			return n;
		}
		return 0;

#elif defined(WIN32)
		struct fd_set rfds;
		struct fd_set wfds;
		struct fd_set exfds;
		struct timeval tv, *ptv;
		FD_ZERO(&rfds);
		if (events & 0x1)
			FD_SET(fd, &rfds);
		if (FDValid(forceBreakFD))
			FD_SET(forceBreakFD, &rfds);
		FD_ZERO(&exfds);
		if (events & 0x2)
			FD_SET(fd, &exfds);
		FD_ZERO(&wfds);
		if (events & 0x4)
			FD_SET(fd, &wfds);
		if (pollTimeout < 0)
			ptv = NULL;
		else
		{
			tv.tv_sec = pollTimeout / 1000;
			tv.tv_usec = (pollTimeout % 1000) * 1000;
			ptv = &tv;
		}
		n = select((int)fd + 1, &rfds, &wfds, &exfds, ptv);
		if (n <= 0)
			return n;
		n = 0;
		if (FD_ISSET(fd, &rfds))
			n |= 1;
		if (FD_ISSET(fd, &exfds))
			n |= 2;
		if (FD_ISSET(fd, &wfds))
			n |= 4;
		return n;
#endif /* end of !__linux__ */
	}
}

// 简单C++类包装
#ifdef __cplusplus

#if defined(_MSC_VER)
	#pragma warning(disable : 4201) // nonstandard extension used : nameless struct/union
#endif

//信号量类
class COSSema
{
	COSSema(const COSSema&);
	COSSema& operator= (const COSSema&);

#if defined(OS_IS_APPLE)
	OSSema* sema_;
	char semaName_[64];
#else
	OSSema sema_;
#endif

public:
	explicit COSSema(int initValue = 0)
	{
#if defined(OS_IS_APPLE)
		sema_ = NULL;
		memset(semaName_, 0, sizeof(semaName_));
		sprintf(semaName_, "rand_COSSema_%x", this);
		sema_ = OSSemaInit(semaName_, initValue);
		ASSERT(sema_ != NULL);
#else
		VERIFY(OSSemaInit(&sema_, initValue));
#endif
	}
	~COSSema()
	{
#if defined(OS_IS_APPLE)
		if (sema_ != NULL)
		{
			OSSemaDestroy(sema_, semaName_);
			sema_ = NULL;
		}
#else
		OSSemaDestroy(&sema_);
#endif
	}
	BOOL Post(int n = 1)
	{
#if defined(OS_IS_APPLE)
		if (NULL == sema_)
			return false;
		return OSSemaPost(sema_, n);
#else
		return OSSemaPost(&sema_, n);
#endif
	}
	BOOL Wait(DWORD timeout = INFINITE)
	{
#if defined(OS_IS_APPLE)
		if (NULL == sema_)
			return false;
		return OSSemaWait(sema_, timeout);
#else
		return OSSemaWait(&sema_, timeout);
#endif	
	}
};

//事件类
class COSEvent
{
	COSEvent(const COSEvent&);
	COSEvent& operator= (const COSEvent&);
	OSEvent ev_;

public:
	COSEvent()
	{
		VERIFY(OSEventInit(&ev_));
	}
	~COSEvent()
	{
		OSEventDestroy(&ev_);
	}
	void Set()
	{
		OSEventSet(&ev_);
	}
	BOOL Wait(DWORD timeout = INFINITE)
	{
		return OSEventWait(&ev_, timeout);
	}
	OSHandle GetEventHandle()
	{
		return OSEventGetHandle(&ev_);
	}
};

// 可以打断的Sleep()，多个线程使用同一个InterruptableSleep对象的Sleep()函数，不保证都能被打断
class InterruptableSleep
{
	InterruptableSleep(const InterruptableSleep&);
	InterruptableSleep& operator= (const InterruptableSleep&);
	COSSema sema_;

public:
	InterruptableSleep()
	{
	}
	~InterruptableSleep()
	{
		Clean();
	}
	void Clean()
	{
		while (sema_.Wait(0));
	}
	void Interrupt()
	{
		sema_.Post();
	}
	BOOL Sleep(DWORD ms, BOOL cleanFirst = false) // 返回true，被打断；返回false，超时
	{
		if (cleanFirst)
			Clean(); // 清理以前的打断通知

		BOOL ok = sema_.Wait(ms);
		if (ok)
			Clean(); // 清理掉本轮所有的打断通知

		return ok;
	}
};

#endif	//__cplusplus

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++++++++++函数实现++++++++++++++++++++++++++++*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

inline BOOL OSSemaInit(OSSema* sema, int initValue /*= 0*/, LONG maxCount /*DEFAULT_PARAM(0x7fffffff)*/)
{
	ASSERT(sema && initValue >= 0);
#if defined(WIN32)
	return NULL != (*sema = CreateSemaphore(NULL, initValue, maxCount/*0x7FFFFFFF*/, NULL));
#elif defined(__linux__)
	return 0 == sem_init(sema, 0, initValue);
#endif
}

inline void OSSemaDestroy(OSSema* sema)
{
	ASSERT(sema);
#if defined(WIN32)
	VERIFY(CloseHandle(*sema));
#elif defined(__linux__)
	VERIFY(0 == sem_destroy(sema));
#endif
	//CdCdObj(*sema);
}

#if defined(OS_IS_APPLE)
inline OSSema* OSSemaInit(const char* semaName, int initValue /*DEFAULT_PARAM(0)*/, int* err/* = NULL*/)
{
	if (NULL == semaName)
		return NULL;
	OSSema* sema = sem_open(semaName, O_CREAT, 0644, initValue);
	if (err)
		*err = errno;
	return sema;
}

inline void OSSemaDestroy(OSSema* sema, const char* semaName)
{
	if (sema)
		sem_close(sema);
	if (semaName)
		sem_unlink(semaName);
}
#endif

inline BOOL OSSemaPost(OSSema* sema, int n /*DEFAULT_PARAM(1)*/)
{
	BOOL ok;
	ASSERT(sema && n >= 1);
#if defined(WIN32)
	while (true)
	{
		ok = ReleaseSemaphore(*sema, n, NULL);
		if (!ok && ERROR_TOO_MANY_POSTS == GetLastError())
		{
			Sleep(1);
			continue;
		}
		break;
	}
#elif defined(__linux__)
	ok = false;
	while (n-- > 0)
	{
		BOOL ok2 = (0 == sem_post(sema));
		if (ok2)
			ok = ok2;
		else
		{
			if (!ok)
				break;
			else
				Sleep(1);
		}
	}
#endif
	ASSERT(ok);
	return ok;
}

inline BOOL OSSemaWait(OSSema* sema, DWORD timeout /*= INFINITE*/)
{
	ASSERT(sema);
#if defined(WIN32)
	{
		DWORD ret = WaitForSingleObject(*sema, timeout);
		return (WAIT_OBJECT_0 == ret);
	}

#elif defined(__linux__)
	if (INFINITE == timeout)
	{
		int ret = sem_wait(sema);
		while (ret == -1 && errno == EINTR)
			ret = sem_wait(sema);
		ASSERT(0 == ret);
		return (0 == ret);
	}
	else if (0 == timeout)
	{
		return (0 == sem_trywait(sema));
	}
	else
	{
		/* FC14环境的sem_timedwait()有问题, 在arm下也有问题,在很多系统下发现有问题故注释掉 ???*/
		// 	#if defined(__USE_XOPEN2K) && !defined(OS_IS_MIPS_ARM_LINUX)	\
		// 				&& !(__GNUC__==4 && __GNUC_MINOR__==5 && __GNUC_PATCHLEVEL__==1)	\
		// 				&& !(__GNUC__==4 && __GNUC_MINOR__==1 && __GNUC_PATCHLEVEL__==2)
		// 		LTMSEL lt = GetMsel() + timeout;
		// 		struct timespec tp;
		// 		tp.tv_sec = (time_t)(lt/1000);
		// 		tp.tv_nsec = (long)(lt%1000) * 1000 * 1000;
		// 		return 0 == sem_timedwait(sema, &tp);
		// 	#else
		int ret;
		DWORD tick0;
		struct timeb b;
		ftime(&b);
		tick0 = b.time * 1000 + b.millitm;
		ret = sem_trywait(sema);
		while (0 != ret)
		{
			DWORD tick;
			ftime(&b);
			tick = b.time * 1000 + b.millitm;
			if (tick - tick0 >= timeout)
				return false;
			usleep(1000); /* 1 ms */
			ret = sem_trywait(sema);
		}
		return true;
		//	#endif
	}
#endif
}

inline BOOL OSEventInit(OSEvent* ev)
{
	ASSERT(ev);
#if defined(WIN32)
	return (*ev = CreateEvent(NULL, false, false, NULL)) != NULL; /* auto-reset, initial false */
#elif defined(__linux__)
	ev->count = 0;
	return (0 == pipe(ev->h));
#endif
}

inline void OSEventDestroy(OSEvent* ev)
{
	ASSERT(ev);
#if defined(WIN32)
	VERIFY(CloseHandle(*ev));
#elif defined(__linux__)
	ASSERT(0 <= ev->count && ev->count <= 1);
	VERIFY(0 == close(ev->h[0]));
	VERIFY(0 == close(ev->h[1]));
	ev->h[0] = ev->h[1] = -1;
#endif
	//CdCdObj(*ev);
}

inline void OSEventSet(OSEvent* ev)
{
	ASSERT(ev);
#if defined(WIN32)
	VERIFY(SetEvent(*ev));
#elif defined(__linux__)
	ASSERT(0 == ev->count);
	VERIFY(1 == InterlockedIncrement(&ev->count));
	{
		int ret = write(ev->h[1], "E", 1);
		if (1 != ret)
		{
			printf("OSEventSet() failed, ret: %d, errno: %d\n", ret, errno);
			ASSERT(false);
		}
	}
#endif
}

inline BOOL OSEventWait(OSEvent* ev, DWORD timeout /*DEFAULT_PARAM(INFINITE)*/)
{
	ASSERT(ev);
#if defined(WIN32)
	return WAIT_OBJECT_0 == WaitForSingleObject(*ev, timeout);
#elif defined(__linux__)
	ASSERT(0 <= ev->count && ev->count <= 1);
	{
		char c;
		int ret;
		if (timeout != INFINITE)
		{
			if (0 == (1 & FDPoll(ev->h[0], 1, timeout & 0x7FFFFFFF, INVALID_SOCKET)))
				return false;
		}
		ret = read(ev->h[0], &c, 1);
		if (1 != ret)
		{
			printf("OSEventWait() failed, ret: %d, errno: %d\n", ret, errno);
			ASSERT(false);
			return false;
		}
		ASSERT(1 == ev->count);
		VERIFY(0 == InterlockedDecrement(&ev->count));
		return true;
	}
#endif
}

inline OSHandle OSEventGetHandle(const OSEvent* ev)
{
#if defined(WIN32)
	return *ev;
#elif defined(__linux__)
	return ev->h[0];
#endif
}

#ifdef VS_IS_VC2008
	#pragma warning(default: 6011)
	#pragma warning(default: 6386)
#endif

#endif