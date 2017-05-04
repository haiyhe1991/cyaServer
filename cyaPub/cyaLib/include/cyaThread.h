#ifndef __CYA_THREAD_H__
#define __CYA_THREAD_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#if defined(_MSC_VER) && !defined(_MT)
	/* Please set to 'Multithreading CRT lib' in project settings of VC */
	#error Must be Multithreading CRT lib.
#endif

#include "cyaOS_opt.h"
#if defined(WIN32)
	#include <process.h>
#endif

#ifdef __linux__
	#include <pthread.h>
	#include <semaphore.h>
	#if defined(OS_IS_MIPS_ARM_LINUX)
		#define PTHREAD_STACK_SIZE_DEFAULT (1024*256)+
	#else
		#define PTHREAD_STACK_SIZE_DEFAULT (1024*512)
	#endif
#endif

#include "cyaTypes.h"

#if defined(WIN32)
	#ifndef STACK_SIZE_PARAM_IS_A_RESERVATION
		#define STACK_SIZE_PARAM_IS_A_RESERVATION		0x00010000
	#endif
#endif

typedef int(*fnOSThreadRoutine)(void*);
typedef enum OSThreadPriority__
{
	OS_BAD_THREAD = -1,
	OS_LOW_THREAD = 0,
	OS_NORMAL_THREAD,
	OS_HIGH_THREAD,
	OS_URGENT_THREAD,
} OSThreadPriority;

/* �����߳�
* @fnRoutine[in] �̹߳�������
* @param[in] ���ݸ��̹߳��������Ĳ���
* @stackSize[in] �̵߳Ķ�ջ��С���ֽڣ���0��ʾʹ��ϵͳĬ��
* @thd[out] ���������߳̾��
* @tid[out] ���������߳�ID��������Ψһ��ʶ�̣߳�������Ҫ���ɴ�NULL
* @return FALSE - ʧ�ܣ�TRUE - �ɹ� */
BOOL OSCreateThread(OSThread* thd, OSThreadID* tid /*= NULL*/, fnOSThreadRoutine fnRoutine, void* param, DWORD stackSize);

/* �ر��̣߳���һֱ�ȵ��߳������˳� */
void OSCloseThread(OSThread thd);

/* ��dll�е�ȫ�ֶ�������ǰ����dll�������û��߳̿����ȱ�ϵͳkill�� */
BOOL ThreadIsKilled(OSThread thread);

/* ����/��ȡ�߳����ȼ� */
BOOL OSSetThreadPriority(OSThread thd, OSThreadPriority priority);
OSThreadPriority OSGetThreadPriority(OSThread thd);

//��ȡ��ǰ�߳�ID
OSThreadID OSThreadSelf();


/* �ֲ߳̾��洢����
* ˵��ͬwin api��TlsAlloc()��TlsFree()... */
DWORD OSTlsAlloc();
BOOL OSTlsFree(DWORD idx);
BOOL OSTlsSetValue(DWORD idx, LPVOID v);
LPVOID OSTlsGetValue(DWORD idx);

/* ��INT_PTR���͵ļ򵥷�װ */
BOOL OSTlsSetInt(DWORD idx, INT_PTR v);
INT_PTR OSTlsGetInt(DWORD idx);

#if defined(_MSC_VER)
	#pragma warning(disable : 4201) // nonstandard extension used : nameless struct/union
#endif


#ifdef __cplusplus

class OSThreadSite
{
private:
	OSThreadSite(const OSThreadSite&);
	OSThreadSite& operator= (const OSThreadSite&);

public:
	OSThread m_hd;
	union
	{
		OSThreadID m_id;
		void*	m_id_hex;
		INT_PTR m_id_int;
	};

public:
	OSThreadSite();
	~OSThreadSite();
	bool Open(fnOSThreadRoutine fnRoutine, void* param, DWORD stackSize = 0);
	bool IsOpened() const;
	void Close();
};

inline OSThreadSite::OSThreadSite()
	: m_hd(INVALID_OSTHREAD)
	, m_id(INVALID_OSTHREADID)
{
}

inline OSThreadSite::~OSThreadSite()
{
	Close();
}

inline bool OSThreadSite::Open(fnOSThreadRoutine fnRoutine, void* param, DWORD stackSize /*= 0*/)
{
	if (IsOpened())
	{
		ASSERT(false);
		return false;
	}
	if (!OSCreateThread(&m_hd, &m_id, fnRoutine, param, stackSize))
	{
		m_hd = INVALID_OSTHREAD;
		m_id = INVALID_OSTHREADID;
		return false;
	}
	return true;
}

inline bool OSThreadSite::IsOpened() const
{
	return INVALID_OSTHREAD != m_hd;
}

inline void OSThreadSite::Close()
{
	if (IsOpened())
	{
		OSCloseThread(m_hd);
		m_hd = INVALID_OSTHREAD;
		m_id = INVALID_OSTHREADID;
	}
}

#endif	/* #ifdef __cplusplus */

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++++++++�̺߳���ʵ��++++++++++++++++++++++++++*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#if defined(WIN32)
	typedef LPTHREAD_START_ROUTINE fnThreadRoutine;
#elif defined(__linux__)
	typedef void* (*fnThreadRoutine)(void*);
#endif
inline BOOL OSCreateThread__(OSThread* thd, OSThreadID* tid, fnThreadRoutine fnRoutine, void* param, DWORD stackSize)
{
#if defined(WIN32) /* Windows 95 and Windows 98: LPDWORD lpThreadId may not be NULL. */
	OSThreadID win_tid = INVALID_OSTHREADID;
#endif

	if (NULL == thd)
	{
		ASSERT(FALSE);
		return FALSE;
	}

#if defined(WIN32)
	*thd = CreateThread(NULL, stackSize, fnRoutine, param, (GetWindowsVersion() >= Windows_XP ? STACK_SIZE_PARAM_IS_A_RESERVATION : 0), &win_tid);
	if (tid)
		*tid = win_tid;
	return *thd != NULL;
#elif defined(__linux__)
	int r;
#if defined(PTHREAD_STACK_SIZE_DEFAULT) && (PTHREAD_STACK_SIZE_DEFAULT>0)
	if (stackSize <= 0)
		stackSize = PTHREAD_STACK_SIZE_DEFAULT;
#endif
	if (stackSize <= 0)
		r = pthread_create(thd, NULL, fnRoutine, param);
	else
	{
		pthread_attr_t att;
		r = pthread_attr_init(&att);
		if (r == 0)
		{
			r = pthread_attr_setstacksize(&att, stackSize);
			if (r == 0)
				r = pthread_create(thd, &att, fnRoutine, param);
			pthread_attr_destroy(&att);
		}
	}
	if (r == 0)
	{
		if (tid)
			*tid = *thd;
		return TRUE;
	}
	return FALSE;
#endif
}

//�߳̽ṹ��;
struct OSThreadContextParam
{
	union
	{
		OSThreadID parentThreadID;
		void* parentThreadID_hex;
		INT_PTR parentThreadID_int;
	};
	fnOSThreadRoutine fnRoutine;
	void* param;
};
#if defined(WIN32)
	static DWORD WINAPI OSThreadRoutine(LPVOID param)
#else
	static void* OSThreadRoutine(void* param)
#endif
{
	int r;
	struct OSThreadContextParam info = *(struct OSThreadContextParam*)param;
	free(param);
	r = info.fnRoutine(info.param);

#if defined(WIN32)
	_endthreadex(r);
	return r;
#else
	return (void*)(INT_PTR)r;
#endif
}

inline BOOL OSCreateThread(OSThread* thd, OSThreadID* tid, fnOSThreadRoutine fnRoutine, void* param, DWORD stackSize)
{
	BOOL ok;
	struct OSThreadContextParam* info = (struct OSThreadContextParam*)malloc(sizeof(struct OSThreadContextParam));
	if (NULL == info)
		return FALSE;
	info->parentThreadID = OSThreadSelf();
	info->fnRoutine = fnRoutine;
	info->param = param;
	ok = OSCreateThread__(thd, tid, OSThreadRoutine, info, stackSize);
	if (!ok)
		free(info);
	return ok;
}

inline void OSCloseThread(OSThread thd)
{
#if defined(WIN32)
	VERIFY(WAIT_OBJECT_0 == WaitForSingleObject(thd, INFINITE));
	VERIFY(CloseHandle(thd));
#elif defined(__linux__)
	VERIFY(0 == pthread_join(thd, NULL));
#endif
}

inline BOOL ThreadIsKilled(OSThread thread)
{
	if (INVALID_OSTHREAD == thread)
		return true;
#if defined(WIN32)
	{
		DWORD ret = WaitForSingleObject(thread, 0);
		if (WAIT_OBJECT_0 == ret || WAIT_FAILED == ret)
			return true;
	}
#endif
	return false;
}

inline BOOL OSSetThreadPriority(OSThread thd, OSThreadPriority priority)
{
#if defined(WIN32)
	static int s_map_inited = 0;
	static int s_map[4];
	if (!s_map_inited)
	{
		s_map[0] = THREAD_PRIORITY_LOWEST;
		s_map[1] = THREAD_PRIORITY_NORMAL;
		s_map[2] = THREAD_PRIORITY_HIGHEST;
		s_map[3] = THREAD_PRIORITY_TIME_CRITICAL;
		s_map_inited = 1;
	}
	if (priority < OS_LOW_THREAD)
		priority = OS_LOW_THREAD;
	else if (priority > OS_URGENT_THREAD)
		priority = OS_URGENT_THREAD;
	return !!SetThreadPriority(thd, s_map[priority]);
#elif defined(__linux__)
	return TRUE;//??
#endif
}

inline OSThreadPriority OSGetThreadPriority(OSThread thd)
{
#if defined(WIN32)
	int pri = GetThreadPriority(thd);
	if (pri < THREAD_PRIORITY_NORMAL)
		return OS_LOW_THREAD;
	if (pri == THREAD_PRIORITY_NORMAL)
		return OS_NORMAL_THREAD;
	if (pri <= THREAD_PRIORITY_HIGHEST)
		return OS_HIGH_THREAD;
	return OS_URGENT_THREAD;
#elif defined(__linux__)
	return OS_NORMAL_THREAD;
#endif
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+++++++++++++++++++++++++TLS����ʵ��++++++++++++++++++++++++++*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#if defined(WIN32)
	inline DWORD OSTlsAlloc()
	{
		return TlsAlloc();
	}

	inline BOOL OSTlsFree(DWORD idx)
	{
		return !!TlsFree(idx);
	}

	inline BOOL OSTlsSetValue(DWORD idx, LPVOID v)
	{
		return !!TlsSetValue(idx, v);
	}

	inline LPVOID OSTlsGetValue(DWORD idx)
	{
		return TlsGetValue(idx);
	}

#elif defined(__linux__)
	inline DWORD OSTlsAlloc()
	{
		pthread_key_t key;
		if (0 == pthread_key_create(&key, NULL))
			return key;
		else
			return (DWORD)-1;
	}

	inline BOOL OSTlsFree(DWORD idx)
	{
		return 0 == pthread_key_delete((pthread_key_t)idx);
	}

	inline BOOL OSTlsSetValue(DWORD idx, LPVOID v)
	{
		BOOL ok = (0 == pthread_setspecific((pthread_key_t)idx, v));
		ASSERT(ok);
		return ok;
	}

	inline LPVOID OSTlsGetValue(DWORD idx)
	{
		LPVOID p = pthread_getspecific((pthread_key_t)idx);
		/* ASSERT(p); û����⵱������0ֵ�����?? */
		return p;
	}
#endif

inline BOOL OSTlsSetInt(DWORD idx, INT_PTR v)
{
	BOOL ok = OSTlsSetValue(idx, (LPVOID)v);
	ASSERT(ok);
	return ok;
}
inline INT_PTR OSTlsGetInt(DWORD idx)
{
	LPVOID p = OSTlsGetValue(idx);
#if defined(WIN32)
	ASSERT(p || NO_ERROR == GetLastError());
#endif
	return (INT_PTR)p;
}

inline OSThreadID OSThreadSelf()
{
#if defined(WIN32)
	return GetCurrentThreadId();
#elif defined(__linux__)
	return pthread_self();
#endif
}




#endif