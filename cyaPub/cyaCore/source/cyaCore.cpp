#include "cyaCore.h"
#include "cyaLocker.h"
#include "cyaOS_opt.h"
#include "cyaMemPool.h"
#include "cyaMaxMin.h"

struct TPoolObj
{
	CXTLocker locker;
	TTaskPool* taskPool;
	TTimerPool* timerPool;
	int initThreads;
	int holdThreads;
	int maxThreads;
	int recycleInterval;
	int startupTimersAtOnce;

	TPoolObj() : taskPool(NULL)
		, initThreads(1)
		, holdThreads(max(32, OSGetCpuCount() * 2))
		, maxThreads(512)
		, recycleInterval(1000 * 60)
		, startupTimersAtOnce(4/*TimerPoolSite::MIN_STARTUP_TIMERS_AT_ONCE*/)
	{

	}

	~TPoolObj()
	{
		CyaCore_DestroyTaskPool();
	}
};

static inline TPoolObj& OnGetXPoolObj()
{
	LOCAL_SAFE_STATIC_OBJ(TPoolObj);
}

//设置线程池参数
CYACORE_API void CyaCore_SetTaskPoolArgs(int initThreads /*= 1*/,
	int holdThreads /*= 32*/,
	int maxThreads /*= 1024*/,
	int recycleInterval /*= 1000*60*/,
	int startupTimersAtOnce /*= 4*/)
{
	TPoolObj& poolObj = OnGetXPoolObj();
	CXTAutoLock lock(poolObj.locker);
	ASSERT(poolObj.taskPool == NULL);
	poolObj.initThreads = max(1, min(initThreads, 128));
	poolObj.holdThreads = max(1, min(holdThreads, 256));
	poolObj.maxThreads = max(1, min(maxThreads, 1024));
	poolObj.recycleInterval = max(1000, min(recycleInterval, 1000 * 60 * 60));
	poolObj.startupTimersAtOnce = 4;
}

//获取全局线程池对象
CYACORE_API ITTaskPool* CyaCore_FetchTaskPool()
{
	TPoolObj& poolObj = OnGetXPoolObj();
	if (poolObj.taskPool)
		return poolObj.taskPool;
	CXTAutoLock lock(poolObj.locker);
	if (poolObj.taskPool)
		return poolObj.taskPool;
	poolObj.taskPool = new TTaskPool(poolObj.initThreads,
		poolObj.holdThreads,
		poolObj.maxThreads,
		poolObj.recycleInterval);
	return poolObj.taskPool;
}

//获取全局定时器
CYACORE_API ITTimerPool* CyaCore_FetchTimerPool()
{
	TPoolObj& poolObj = OnGetXPoolObj();
	if (poolObj.timerPool)
		return poolObj.timerPool;

	ITTaskPool* taskPool = CyaCore_FetchTaskPool();
	CXTAutoLock locker(poolObj.locker);
	if (poolObj.timerPool)
		return poolObj.timerPool;
	poolObj.timerPool = new TTimerPool(taskPool, poolObj.startupTimersAtOnce);
	return poolObj.timerPool;
}

// 销毁全局线程池对象（一般可在主函数最后调用）
CYACORE_API void CyaCore_DestroyTaskPool()
{
	TPoolObj& poolObj = OnGetXPoolObj();
	if (NULL == poolObj.taskPool && poolObj.timerPool == NULL)
		return;
	CXTAutoLock lock(poolObj.locker);
	if (poolObj.taskPool)
	{
		delete poolObj.taskPool;
		poolObj.taskPool = NULL;
	}
	if (poolObj.timerPool)
	{
		delete poolObj.timerPool;
		poolObj.timerPool = NULL;
	}
}

static inline TSmallMemPoolObj& OnGetTSmallMemPoolObj(INT_PTR itemKBytes = 64)
{
	BEGIN_LOCAL_SAFE_STATIC_OBJ_EX(TSmallMemPoolObj, memPoolObj, (itemKBytes));
	END_LOCAL_SAFE_STATIC_OBJ(memPoolObj);
}

//设置内存池参数
CYACORE_API void CyaCore_SetSmallMemPoolArg(INT_PTR kBytes)
{
	OnGetTSmallMemPoolObj(kBytes);
}

//分配内存
CYACORE_API void* CyaCore_Alloc(INT_PTR si)
{
	return OnGetTSmallMemPoolObj().m_poolObj->Alloc(si);
}

//释放内存
CYACORE_API void CyaCore_Free(void* p)
{
	OnGetTSmallMemPoolObj().m_poolObj->Free(p);
}

