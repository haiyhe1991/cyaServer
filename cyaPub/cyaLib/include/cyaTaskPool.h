// 通用任务线程池，timer管理器

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

	///推送任务
	virtual void Push(ITBaseTask* task) = 0;

	/// 获取已投递，还未处理的任务数量
	virtual int GetWaitingTasks() = 0;

	///获取线程池中已创建线程数量
	virtual int GetThreadCount() = 0;

	///获取线程池中处于空闲状态线程数量
	virtual int GetIdleThreadCount() = 0;
};

/// Timer池接口
struct ITTimerPool
{
	enum
	{
		MIN_STARTUP_TIMERS_AT_ONCE = 4,
		MAX_STARTUP_TIMERS_AT_ONCE = 1024,
	};

	virtual ~ITTimerPool() {}

	/// 添加一个新timer。注意：一旦timer开始执行，修改系统时间也不会影响timer的执行
	/// @id[out] 返回timer ID（失败时保证id返回NULL）
	/// @timerCall[in] timer回调函数
	/// @param[in] 回调参数
	/// @periodMSELs[in] 执行周期（毫秒: ms）
	/// @startTime[in] 启动绝对时间，<=GetMsel()为立即执行
	/// @return 成功返回true，失败返回false
	virtual BOOL AddTimer(TTimerID& id,
						fnTimerCallback timerCall,
						void* param,
						DWORD periodMSELs,
						LTMSEL startTime = MIN_UTC_MSEL) = 0;

	/// 删除timer
	/// @id[in] timer id
	/// @wait[in] false，异步删除，立即返回；
	///			true，同步删除，等待timer回调返回且删除完成后才返回
	///			注意：在timer的回调函数里不可对本timer进行同步删除
	/// @return true，timer是否存在
	virtual BOOL DelTimer(TTimerID id, BOOL wait /*= true*/) = 0;

	/// 设置timer执行时间间隔
	/// @id[in] timer id
	/// @periodMSELs[in] 执行时间间隔（毫秒: ms）
	/// @return true，timer是否存在
	virtual BOOL SetTimerInterval(TTimerID id, DWORD interval) = 0;

	/// 立即执行一次timer
	/// @return true，timer是否存在
	virtual	BOOL EnforceTimer(TTimerID id) = 0;

	/// 取消异步删除timer动作（如果不处于异步删除状态，则本函数什么也不做）
	/// @return true，timer是否存在
	virtual BOOL CancelAsyncDelTimer(TTimerID id) = 0;

	/// 得到timer信息
	/// @id[in] timer id
	/// @timerCall[out] 返回timer回调函数指针
	/// @param[out] 返回timer回调函数参数
	/// @periodMSELs[out] 返回timer周期
	/// @startTime[out] 返回timer启动绝对时间
	/// @syncDeleting[out] 返回是否处于异步删除timer状态
	/// @running[out] 返回是否处于运行状态（timer回调中）
	/// @return true，timer是否存在
	virtual BOOL GetTimerInfo(TTimerID id,
							fnTimerCallback* timerCall = NULL,
							void** param = NULL,
							DWORD* periodMSELs = NULL,
							LTMSEL* startTime = NULL,
							BOOL* syncDeleting = NULL,
							BOOL* running = NULL) const = 0;

	/// 关闭timer调度线程，并停止所有timer
	virtual void ShutDown() = 0;
};

class TTaskPool : public ITTaskPool
{
private:
	TTaskPool(const TTaskPool&);
	TTaskPool& operator=(const TTaskPool&);

public:
	/// initThreads 初始线程数
	/// holdThreads 维持线程数
	/// maxThreads 最大线程数
	/// recycleInterval 尝试回收空闲线程的时间间隔（毫秒）
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
	mutable CXTLocker m_lock;//mutable 可变的
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
	/// taskPoll，任务池实例，使用者保证在TTimerPool生命期taskPool对象一直有效
	/// startupTimersAtOnce，一次最多启动的timer数，一般为CPU数的2到4倍比较合适
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

		BOOL started;			// 是否已经开始调度，开始后，系统时间改变不影响timer调度
		DWORD lastTick;			// 上一次调度timer的tick
		BOOL running;			// timer正在被执行
		BOOL deleting;			// 同步删除标志，等待timer调用完成
		BOOL syncDeleting;		// 异步删除标志，若为true，将由timer调度线程删除

		BOOL enforceRunOnce;	// 是否立即调度一次timer

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