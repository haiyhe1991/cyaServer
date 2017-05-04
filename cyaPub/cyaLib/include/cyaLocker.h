#ifndef __CYA_LOCKER_H__
#define __CYA_LOCKER_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaThread.h"
#include "cyaAtomic.h"
#include "cyaSync.h"
#include "cyaPsMutex.h"

class CSpinLock
{
public:
	CSpinLock()
		: m_cpuCount(OSGetCpuCount())
#ifdef _DEBUG
		, m_lockedThreadID(INVALID_OSTHREADID)
#endif
	{
		OSSpinLockInit(&m_lock);
	}
	void Lock(DWORD spin = 0)
	{
		OSSpinLock(&m_lock, spin);
#ifdef _DEBUG
		ASSERT(INVALID_OSTHREADID == m_lockedThreadID);
		m_lockedThreadID = OSThreadSelf();
#endif
	}
	BOOL TryLock(DWORD /*timeout*/, DWORD spin = 0)
	{
		Lock(spin); return true;
	}
	void Unlock()
	{
#ifdef _DEBUG
		ASSERT(m_lockedThreadID == OSThreadSelf());
		m_lockedThreadID = INVALID_OSTHREADID;
#endif
		OSSpinUnlock(&m_lock);
	}
	BOOL IsLocked() const
	{
		return OSIsSpinLocked(&m_lock);
	}

public:
	class AutoLock
	{
	private:
		AutoLock(const AutoLock&);
		AutoLock& operator = (const AutoLock&);

		CSpinLock& m_spinLock;
		BOOL	m_lockIt;
		DWORD	m_spin;

	public:
		AutoLock(CSpinLock& spinLock, DWORD spin = 0, BOOL lockIt = true)
			: m_spinLock(spinLock)
			, m_lockIt(lockIt)
			, m_spin(spin)
		{
			if (lockIt)
				m_spinLock.Lock(m_spin);
			else
				m_spinLock.Unlock();
		}

		~AutoLock()
		{
			if (m_lockIt)
				m_spinLock.Unlock();
			else
				m_spinLock.Lock(m_spin);
		}
	};

private:
	CSpinLock(const CSpinLock&);
	CSpinLock& operator= (const CSpinLock&);

private:
	int const		m_cpuCount;
	mutable LONG	m_lock;
#ifdef _DEBUG
	union
	{
		OSThreadID	m_lockedThreadID;
		void*		m_lockedThreadID_hex;
		INT_PTR		m_lockedThreadID_int;
	};
#endif
};

class CAutoSpinLock
{
public:
	explicit CAutoSpinLock(LONG& lock, DWORD spinMsel = 0)
		: m_lock(lock)
	{
		OSSpinLock(&m_lock, spinMsel);
	}
	~CAutoSpinLock()
	{
		OSSpinUnlock(&m_lock);
	}

private:
	CAutoSpinLock(const CAutoSpinLock&);
	CAutoSpinLock& operator= (const CAutoSpinLock&);

	LONG& m_lock;
};

// 自动线程锁
#define CXTLocker COSRawLock
#define CXTAutoLock COSRawLock::AutoLock
#define CXTAutoTryLock COSRawLock::AutoTryLock
#define CXSpinAutoLock CSpinLock::AutoLock

// 自动进程锁
#define CXPAutoLock CXPLocker::AutoLock



//////////////////////////////////////////////////////////////////////////
//=========== 下面这些宏用自旋锁实现了线程安全的静态局部对象/数组 =========//
//////////////////////////////////////////////////////////////////////////

// 线程安全的局部静态变量，保证只初始化一次
#define BEGIN_LOCAL_SAFE_STATIC_VAR(VarType, varName)		\
	static LONG sSpinLock__ = 0;							\
	static BOOL volatile sIsInited__ = false;				\
	static VarType volatile varName;						\
															\
	if(sIsInited__)/*锁外一次判断，保证后续使用高效*/		\
		return varName;										\
	CAutoSpinLock locker(sSpinLock__, 1);					\
	if(sIsInited__)/*锁内二次判断，保证正确性*/				\
		return varName


#define END_LOCAL_SAFE_STATIC_VAR(varName)					\
	sIsInited__ = true;										\
	return varName

/*
例子：
INT64 GetCPUFreq()
{
BEGIN_LOCAL_SAFE_STATIC_VAR(INT64, sCPUFreq);

// 计算cpu频率...
sCPUFreq = ...
// ...

END_LOCAL_SAFE_STATIC_VAR(sCPUFreq);
}
*/
//-------------------------------------------------------------------end//

#define BEGIN_LOCAL_SAFE_STATIC_OBJ__(ObjType, varName)		\
	static LONG sSpinLock__ = 0;							\
	static volatile ObjType* volatile ptr##varName = NULL;	\
															\
	if(ptr##varName)/*锁外一次判断，保证后续使用高效*/		\
		return (ObjType&)*ptr##varName;						\
	CAutoSpinLock locker(sSpinLock__, 1);					\
	if(ptr##varName)/*锁内二次判断，保证正确性*/			\
		return (ObjType&)*ptr##varName;

//---begin 适合于所有有缺省构造函数的类型 --------------------------------//
// 线程安全的局部静态对象，保证只初始化一次
#define BEGIN_LOCAL_SAFE_STATIC_OBJ(ObjType, varName)		\
	BEGIN_LOCAL_SAFE_STATIC_OBJ__(ObjType, varName)			\
	static ObjType varName;									\
	ptr##varName = &varName;

//---begin 适合于所有需要带参数构造的类型 --------------------------------//
#define BEGIN_LOCAL_SAFE_STATIC_OBJ_EX(ObjType, varName, param__)		\
	BEGIN_LOCAL_SAFE_STATIC_OBJ__(ObjType, varName)			\
	static ObjType varName param__;							\
	ptr##varName = &varName;


#define END_LOCAL_SAFE_STATIC_OBJ(varName)					\
	return varName

#define LOCAL_SAFE_STATIC_OBJ(ObjType)						\
			BEGIN_LOCAL_SAFE_STATIC_OBJ(ObjType, obj)		\
			END_LOCAL_SAFE_STATIC_OBJ(obj)

#define LOCAL_SAFE_STATIC_OBJ_EX(ObjType, param__)					\
			BEGIN_LOCAL_SAFE_STATIC_OBJ_EX(ObjType, obj, param__)	\
			END_LOCAL_SAFE_STATIC_OBJ(obj)

/* 例子
// LOCAL_SAFE_STATIC_OBJ
const CString& GetXXX()
{	LOCAL_SAFE_STATIC_OBJ(CString);	}

// LOCAL_SAFE_STATIC_OBJ_EX
const CString& GetXXX()
{	LOCAL_SAFE_STATIC_OBJ_EX(CString, ('a', 3));	}

// BEGIN_LOCAL_SAFE_STATIC_OBJ
const CString& GetXXX()
{
BEGIN_LOCAL_SAFE_STATIC_OBJ(CString, sStr);

// 对sStr执行其它初始化 ...
sStr.Format("abcd...");
// ...

END_LOCAL_SAFE_STATIC_OBJ(sStr);
}

// BEGIN_LOCAL_SAFE_STATIC_OBJ_EX
const CString& GetXXX()
{
BEGIN_LOCAL_SAFE_STATIC_OBJ_EX(CString, sStr, ('a', 3));

// 对sStr执行其它初始化 ...
// ...

END_LOCAL_SAFE_STATIC_OBJ(sStr);
}
*/
//-------------------------------------------------------------------end//

//---begin 适合于所有没有缺省构造函数或者需要特别初始化的类型 ------------//
// 线程安全的局部静态对象，保证只初始化一次
#define BEGIN_LOCAL_SAFE_STATIC_PTR_OBJ(ObjType, ptrName)	\
	static LONG sSpinLock__ = 0;							\
	static BOOL volatile sIsInited__ = false;				\
	static ObjType* volatile ptrName = NULL;				\
															\
	if(sIsInited__)/*锁外一次判断，保证后续使用高效*/		\
		return *ptrName;									\
	CAutoSpinLock locker(sSpinLock__, 1);					\
	if(sIsInited__)/*锁内二次判断，保证正确性*/				\
		return *ptrName


#define END_LOCAL_SAFE_STATIC_PTR_OBJ(ObjType, ptrName)		\
	LOCAL_STATIC_HEAP_OBJ_CLEANER(ObjType, (ObjType*&)ptrName, false);	\
	sIsInited__ = true;										\
	return *ptrName

/*
例子：
const CString& GetXXX()
{
BEGIN_LOCAL_SAFE_STATIC_PTR_OBJ(CString, sStr);

sStr = new CString('a', 8);
sStr += ...
// 对sStr执行初始化...

END_LOCAL_SAFE_STATIC_PTR_OBJ(CString, sStr);
}
*/
//-------------------------------------------------------------------end//

//---begin--------------------------------------------------------------//
// 线程安全局部静态对象
// 注意：该宏需要放在函数里面使用
#define LOCAL_SAFE_STATIC_OBJ_DEFINE(ObjType)		\
struct TLocalSafeStatic##ObjType##Case				\
{													\
	static ObjType& Get##ObjType()					\
				{	LOCAL_SAFE_STATIC_OBJ(ObjType);	}		\
}

// 得到安全局部静态对象
#define GET_LOCAL_SAFE_STATIC_OBJ(ObjType)		TLocalSafeStatic##ObjType##Case::Get##ObjType()

/*
例子：
void SomeFunction()
{
LOCAL_SAFE_STATIC_OBJ_DEFINE(CXTLocker); // 构造了一个局部静态的CXTLocker对象，构造过程保证线程安全
CXTAutoLock locker(GET_LOCAL_SAFE_STATIC_OBJ(CXTLocker));
// ...
}
*/
//-------------------------------------------------------------------end//

#endif