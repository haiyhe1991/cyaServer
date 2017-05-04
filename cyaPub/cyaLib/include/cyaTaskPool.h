// ͨ�������̳߳أ�timer������

#ifndef __CYA_TASK_POOL_H__
#define __CYA_TASK_POOL_H__

#if defined(_MSC_VER) && (_MSC_VER > 1000)
	#pragma once
#endif

#include <vector>
#include <map>
#include "cyaThread.h"
#include "cyaLocker.h"
#include "cyaAtomic.h"
#include "cyaTime.h"
#include "ssmempool.h"

struct ITTaskPool
{
	virtual ~ITTaskPool() {}

	///��������
	virtual void Push(ITBaseTask* task) = 0;

	/// ��ȡ��Ͷ�ݣ���δ�������������
	virtual int GetWaitingTasks() = 0;

	///��ȡ�̳߳����Ѵ����߳�����
	virtual int GetThreadCount() = 0;

	///��ȡ�̳߳��д��ڿ���״̬�߳�����
	virtual int GetIdleThreadCount() = 0;
};

/// Timer�ؽӿ�
struct ITTimerPool
{
	enum
	{
		MIN_STARTUP_TIMERS_AT_ONCE = 4,
		MAX_STARTUP_TIMERS_AT_ONCE = 1024,
	};

	virtual ~ITTimerPool() {}

	/// ���һ����timer��ע�⣺һ��timer��ʼִ�У��޸�ϵͳʱ��Ҳ����Ӱ��timer��ִ��
	/// @id[out] ����timer ID��ʧ��ʱ��֤id����NULL��
	/// @timerCall[in] timer�ص�����
	/// @param[in] �ص�����
	/// @periodMSELs[in] ִ�����ڣ�����: ms��
	/// @startTime[in] ��������ʱ�䣬<=GetMsel()Ϊ����ִ��
	/// @return �ɹ�����true��ʧ�ܷ���false
	virtual BOOL AddTimer(TTimerID& id,
						fnTimerCallback timerCall,
						void* param,
						DWORD periodMSELs,
						LTMSEL startTime = MIN_UTC_MSEL) = 0;

	/// ɾ��timer
	/// @id[in] timer id
	/// @wait[in] false���첽ɾ�����������أ�
	///			true��ͬ��ɾ�����ȴ�timer�ص�������ɾ����ɺ�ŷ���
	///			ע�⣺��timer�Ļص������ﲻ�ɶԱ�timer����ͬ��ɾ��
	/// @return true��timer�Ƿ����
	virtual BOOL DelTimer(TTimerID id, BOOL wait /*= true*/) = 0;

	/// ����timerִ��ʱ����
	/// @id[in] timer id
	/// @periodMSELs[in] ִ��ʱ����������: ms��
	/// @return true��timer�Ƿ����
	virtual BOOL SetTimerInterval(TTimerID id, DWORD interval) = 0;

	/// ����ִ��һ��timer
	/// @return true��timer�Ƿ����
	virtual	BOOL EnforceTimer(TTimerID id) = 0;

	/// ȡ���첽ɾ��timer����������������첽ɾ��״̬���򱾺���ʲôҲ������
	/// @return true��timer�Ƿ����
	virtual BOOL CancelAsyncDelTimer(TTimerID id) = 0;

	/// �õ�timer��Ϣ
	/// @id[in] timer id
	/// @timerCall[out] ����timer�ص�����ָ��
	/// @param[out] ����timer�ص���������
	/// @periodMSELs[out] ����timer����
	/// @startTime[out] ����timer��������ʱ��
	/// @syncDeleting[out] �����Ƿ����첽ɾ��timer״̬
	/// @running[out] �����Ƿ�������״̬��timer�ص��У�
	/// @return true��timer�Ƿ����
	virtual BOOL GetTimerInfo(TTimerID id,
							fnTimerCallback* timerCall = NULL,
							void** param = NULL,
							DWORD* periodMSELs = NULL,
							LTMSEL* startTime = NULL,
							BOOL* syncDeleting = NULL,
							BOOL* running = NULL) const = 0;

	/// �ر�timer�����̣߳���ֹͣ����timer
	virtual void ShutDown() = 0;
};

class TTaskPool : public ITTaskPool
{
private:
	TTaskPool(const TTaskPool&);
	TTaskPool& operator=(const TTaskPool&);

public:
	/// initThreads ��ʼ�߳���
	/// holdThreads ά���߳���
	/// maxThreads ����߳���
	/// recycleInterval ���Ի��տ����̵߳�ʱ���������룩
	TTaskPool(int initThreads, int holdThreads, int maxThreads, DWORD recycleInterval = 1000 * 20);
	virtual ~TTaskPool();
	virtual void Push(ITBaseTask* task);
	virtual int GetWaitingTasks();
	virtual int GetThreadCount();
	virtual int GetIdleThreadCount();

private:
	static int THWorker(void* thisObj);
	void Worker();
	void AutoIncrementThread_();
	ITBaseTask* Pop_();

private:
	mutable CXTLocker m_lock;//mutable �ɱ��
	COSSema m_sema;
	ITBaseTask* m_head;
	ITBaseTask* m_tail;
	int m_taskCount;

	int const m_initThreads;
	int const m_holdThreads;
	int const m_maxThreads;
	std::vector<OSThreadSite*> m_threads;
	long m_idleThreadCount;
	DWORD m_tickLastIncThread;

private:
	struct NullTask : public ITBaseTask
	{
		long _ref;
		NullTask() : _ref(0) {}
		virtual void OnPoolTask()
		{
			VERIFY(InterlockedDecrement(&_ref) >= 0);
		}
	};

	static int THRecycleWorker(void* thisobj);
	void RecycleWorker();

private:
	DWORD const m_recycleInterval;
	InterruptableSleep m_recycleSleep;
	OSThreadSite m_recycleThread;

	long m_recycleFlag;
	OSThreadID m_lastExitedTID;
	NullTask m_nullTask;
};

// class TTimerPool
// ================================================================================================= //
class TTimerPool : public ITTimerPool
{
private:
	TTimerPool(const TTimerPool&);
	TTimerPool& operator= (const TTimerPool&);

public:
	/// taskPoll�������ʵ����ʹ���߱�֤��TTimerPool������taskPool����һֱ��Ч
	/// startupTimersAtOnce��һ�����������timer����һ��ΪCPU����2��4���ȽϺ���
	explicit TTimerPool(ITTaskPool* taskPool, int startupTimersAtOnce = MIN_STARTUP_TIMERS_AT_ONCE);
	virtual ~TTimerPool();

	virtual BOOL AddTimer(TTimerID& id,
						fnTimerCallback timerCall,
						void* param,
						DWORD periodMSELs,
						LTMSEL startTime /*= MIN_UTC_MSEL*/);
	virtual BOOL DelTimer(TTimerID id, BOOL wait);
	virtual BOOL SetTimerInterval(TTimerID id, DWORD periodMs);
	virtual	BOOL EnforceTimer(TTimerID id);
	virtual BOOL CancelAsyncDelTimer(TTimerID id);
	virtual BOOL GetTimerInfo(TTimerID id,
							fnTimerCallback* timerCall /*= NULL*/,
							void** param /*= NULL*/,
							DWORD* periodMSELs /*= NULL*/,
							LTMSEL* startTime /*= NULL*/,
							BOOL* syncDeleting /*= NULL*/,
							BOOL* running /*= NULL*/) const;
	virtual void ShutDown();

private:
	static int THWorker(void* thisobj);
	void Worker();

private:
	mutable CXTLocker m_lock;
	int const m_startupTimersAtOnce;
	ITTaskPool* const m_taskPool;
	struct Info : public ITBaseTask
	{
		union
		{
			OSThreadID runningThreadID;
			void* runningThreadID_hex;
			INT_PTR runningThreadID_int;
		};

		TTimerID id;
		fnTimerCallback timerCall;
		void* param;
		DWORD periodMs;
		LTMSEL startTime;

		BOOL started;			// �Ƿ��Ѿ���ʼ���ȣ���ʼ��ϵͳʱ��ı䲻Ӱ��timer����
		DWORD lastTick;			// ��һ�ε���timer��tick
		BOOL running;			// timer���ڱ�ִ��
		BOOL deleting;			// ͬ��ɾ����־���ȴ�timer�������
		BOOL syncDeleting;		// �첽ɾ����־����Ϊtrue������timer�����߳�ɾ��

		BOOL enforceRunOnce;	// �Ƿ���������һ��timer

		Info()
		{
			Init();
		}
		void Init()
		{
			runningThreadID = INVALID_OSTHREADID;
			id = NULL;
			timerCall = NULL;
			param = NULL;
			periodMs = INFINITE;
			startTime = INVALID_UTC_MSEL;
			started = false;
			lastTick = 0;
			running = false;
			deleting = false;
			syncDeleting = false;
			enforceRunOnce = false;
		}
		virtual void OnPoolTask();
	};
	friend struct Info;
	CMemPoolObj<Info> m_infoPool;
	typedef std::map<INT_PTR, Info*> TimerMap;
	TimerMap m_timerMap;
	BOOL m_syncDeleting;
	INT_PTR m_nextTimerID;

	OSThreadSite m_thread;
	InterruptableSleep m_sleep;
	BOOL m_exit;
};

#endif