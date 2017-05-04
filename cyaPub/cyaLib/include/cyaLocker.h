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

// �Զ��߳���
#define CXTLocker COSRawLock
#define CXTAutoLock COSRawLock::AutoLock
#define CXTAutoTryLock COSRawLock::AutoTryLock
#define CXSpinAutoLock CSpinLock::AutoLock

// �Զ�������
#define CXPAutoLock CXPLocker::AutoLock



//////////////////////////////////////////////////////////////////////////
//=========== ������Щ����������ʵ�����̰߳�ȫ�ľ�̬�ֲ�����/���� =========//
//////////////////////////////////////////////////////////////////////////

// �̰߳�ȫ�ľֲ���̬��������ֻ֤��ʼ��һ��
#define BEGIN_LOCAL_SAFE_STATIC_VAR(VarType, varName)		\
	static LONG sSpinLock__ = 0;							\
	static BOOL volatile sIsInited__ = false;				\
	static VarType volatile varName;						\
															\
	if(sIsInited__)/*����һ���жϣ���֤����ʹ�ø�Ч*/		\
		return varName;										\
	CAutoSpinLock locker(sSpinLock__, 1);					\
	if(sIsInited__)/*���ڶ����жϣ���֤��ȷ��*/				\
		return varName


#define END_LOCAL_SAFE_STATIC_VAR(varName)					\
	sIsInited__ = true;										\
	return varName

/*
���ӣ�
INT64 GetCPUFreq()
{
BEGIN_LOCAL_SAFE_STATIC_VAR(INT64, sCPUFreq);

// ����cpuƵ��...
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
	if(ptr##varName)/*����һ���жϣ���֤����ʹ�ø�Ч*/		\
		return (ObjType&)*ptr##varName;						\
	CAutoSpinLock locker(sSpinLock__, 1);					\
	if(ptr##varName)/*���ڶ����жϣ���֤��ȷ��*/			\
		return (ObjType&)*ptr##varName;

//---begin �ʺ���������ȱʡ���캯�������� --------------------------------//
// �̰߳�ȫ�ľֲ���̬���󣬱�ֻ֤��ʼ��һ��
#define BEGIN_LOCAL_SAFE_STATIC_OBJ(ObjType, varName)		\
	BEGIN_LOCAL_SAFE_STATIC_OBJ__(ObjType, varName)			\
	static ObjType varName;									\
	ptr##varName = &varName;

//---begin �ʺ���������Ҫ��������������� --------------------------------//
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

/* ����
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

// ��sStrִ��������ʼ�� ...
sStr.Format("abcd...");
// ...

END_LOCAL_SAFE_STATIC_OBJ(sStr);
}

// BEGIN_LOCAL_SAFE_STATIC_OBJ_EX
const CString& GetXXX()
{
BEGIN_LOCAL_SAFE_STATIC_OBJ_EX(CString, sStr, ('a', 3));

// ��sStrִ��������ʼ�� ...
// ...

END_LOCAL_SAFE_STATIC_OBJ(sStr);
}
*/
//-------------------------------------------------------------------end//

//---begin �ʺ�������û��ȱʡ���캯��������Ҫ�ر��ʼ�������� ------------//
// �̰߳�ȫ�ľֲ���̬���󣬱�ֻ֤��ʼ��һ��
#define BEGIN_LOCAL_SAFE_STATIC_PTR_OBJ(ObjType, ptrName)	\
	static LONG sSpinLock__ = 0;							\
	static BOOL volatile sIsInited__ = false;				\
	static ObjType* volatile ptrName = NULL;				\
															\
	if(sIsInited__)/*����һ���жϣ���֤����ʹ�ø�Ч*/		\
		return *ptrName;									\
	CAutoSpinLock locker(sSpinLock__, 1);					\
	if(sIsInited__)/*���ڶ����жϣ���֤��ȷ��*/				\
		return *ptrName


#define END_LOCAL_SAFE_STATIC_PTR_OBJ(ObjType, ptrName)		\
	LOCAL_STATIC_HEAP_OBJ_CLEANER(ObjType, (ObjType*&)ptrName, false);	\
	sIsInited__ = true;										\
	return *ptrName

/*
���ӣ�
const CString& GetXXX()
{
BEGIN_LOCAL_SAFE_STATIC_PTR_OBJ(CString, sStr);

sStr = new CString('a', 8);
sStr += ...
// ��sStrִ�г�ʼ��...

END_LOCAL_SAFE_STATIC_PTR_OBJ(CString, sStr);
}
*/
//-------------------------------------------------------------------end//

//---begin--------------------------------------------------------------//
// �̰߳�ȫ�ֲ���̬����
// ע�⣺�ú���Ҫ���ں�������ʹ��
#define LOCAL_SAFE_STATIC_OBJ_DEFINE(ObjType)		\
struct TLocalSafeStatic##ObjType##Case				\
{													\
	static ObjType& Get##ObjType()					\
				{	LOCAL_SAFE_STATIC_OBJ(ObjType);	}		\
}

// �õ���ȫ�ֲ���̬����
#define GET_LOCAL_SAFE_STATIC_OBJ(ObjType)		TLocalSafeStatic##ObjType##Case::Get##ObjType()

/*
���ӣ�
void SomeFunction()
{
LOCAL_SAFE_STATIC_OBJ_DEFINE(CXTLocker); // ������һ���ֲ���̬��CXTLocker���󣬹�����̱�֤�̰߳�ȫ
CXTAutoLock locker(GET_LOCAL_SAFE_STATIC_OBJ(CXTLocker));
// ...
}
*/
//-------------------------------------------------------------------end//

#endif