#include "cyaTime.h"
#include "cyaTaskPool.h"
#include "cyaMaxMin.h"

TTaskPool::TTaskPool(int initThreads, int holdThreads, int maxThreads, DWORD recycleInterval /*= 1000*20*/)
					: m_head(NULL)
					, m_tail(NULL)
					, m_taskCount(0)
					, m_initThreads(max(1, initThreads))
					, m_holdThreads(max(m_initThreads, holdThreads))
					, m_maxThreads(max(m_holdThreads, maxThreads))
					, m_idleThreadCount(0)
					, m_tickLastIncThread(GetTickCount())
					, m_recycleInterval(recycleInterval)
					, m_recycleFlag(false)
					, m_lastExitedTID(INVALID_OSTHREADID)
{
	for (int i = 0; i<m_initThreads; ++i)
	{
		OSThreadSite* p = new OSThreadSite;
		if (!p->Open(THWorker, this, 0))
		{
			delete p;
			break;
		}
		m_threads.push_back(p);
	}
	m_idleThreadCount = (long)m_threads.size();
	if (m_idleThreadCount > 0)
		m_recycleThread.Open(THRecycleWorker, this, 0/*64*1024*/);
}

TTaskPool::~TTaskPool()
{
	if (m_recycleThread.IsOpened())
	{
		m_recycleSleep.Interrupt();
		m_recycleThread.Close();
	}

	if (m_threads.size() > 0)
	{
		// 等待工作线程将待处理任务处理完
		for (;; Sleep(1))
		{
			CXTAutoLock locker(m_lock);
			if (m_taskCount <= 0)
				break;
		}
		ASSERT(NULL == m_head && NULL == m_tail);
		m_sema.Post((int)m_threads.size()); // 唤醒所有工作线程，通知它们全部退出
		for (int i = 0; i<(int)m_threads.size(); i++)
			delete m_threads[(INT_PTR)i];
	}
	else
		ASSERT(NULL == m_head && NULL == m_tail);
}

void TTaskPool::Push(ITBaseTask* task)
{
	if (NULL == task)
	{
		ASSERT(false);
		return;
	}

	{
		CXTAutoLock locker(m_lock);
		AutoIncrementThread_();
		task->m_nextPoolTask = NULL;
		if (m_tail)
		{
			m_tail->m_nextPoolTask = task;
			m_tail = task;
		}
		else
			m_head = m_tail = task;
		++m_taskCount;
	}
	m_sema.Post();
}

int TTaskPool::GetWaitingTasks()
{
	return m_taskCount;
}

int TTaskPool::GetThreadCount()
{
	return (int)m_threads.size();
}

int TTaskPool::GetIdleThreadCount()
{
	return m_idleThreadCount;
}

int TTaskPool::THWorker(void* thisObj)
{
	((TTaskPool*)thisObj)->Worker(); return 1;
}

void TTaskPool::Worker()
{
	while (true)
	{
		ITBaseTask* task = Pop_();
		if (NULL == task)
			break; // 退出线程（TTaskPool析构时）

		VERIFY(InterlockedDecrement(&m_idleThreadCount) >= 0);
		task->OnPoolTask();
		VERIFY(InterlockedIncrement(&m_idleThreadCount) >= 1);

		if (InterlockedExchange(&m_recycleFlag, false))
			break; // 退出线程（被通知回收）
	}
	m_lastExitedTID = OSThreadSelf();
}

void TTaskPool::AutoIncrementThread_()
{
	// 旧的一次只增加一个线程的策略，在一次性投递很多任务时可能导致有些任务不能立即得到相应
	//	if((int)m_threads.size()<m_maxThreads && m_taskCount>=m_idleThreadCount)

	// 改用一次可能增加多个线程的策略，且总多准备一个
	while ((int)m_threads.size()<m_maxThreads && m_taskCount + 1 >= m_idleThreadCount)
	{
		OSThreadSite* p = new OSThreadSite;
		if (!p->Open(THWorker, this))
		{
			delete p;
			return;
		}
		m_threads.push_back(p);
		VERIFY(InterlockedIncrement(&m_idleThreadCount) >= 1);
		m_tickLastIncThread = GetTickCount();
	}
}

ITBaseTask* TTaskPool::Pop_()
{
	m_sema.Wait();
	CXTAutoLock locker(m_lock);
	if (NULL == m_head)
		return NULL; // 被唤醒，但队列为空，表示通知工作线程退出
	ITBaseTask* task = m_head;
	if (m_head == m_tail)
		m_head = m_tail = NULL;
	else
		m_head = task->m_nextPoolTask;
	--m_taskCount;
	return task;
}

int TTaskPool::THRecycleWorker(void* param)
{
	TTaskPool* thisobj = (TTaskPool*)param;
	thisobj->RecycleWorker();
	return 1;
}

void TTaskPool::RecycleWorker()
{
	while (!m_recycleSleep.Sleep(m_recycleInterval))
	{
		// 最后一次创建线程离现在太近，不回收
		if (GetTickCount() - m_tickLastIncThread < m_recycleInterval)
			continue;

		// 如果idle的线程数大于维持线程数，则开始做线程回收
		while (m_idleThreadCount > m_holdThreads)
		{
			// 通知任意一个正在任务执行中的线程，其任务执行完后退出
			VERIFY(!InterlockedExchange(&m_recycleFlag, true));

			// 如果此时没有任何正在处理的任务，post一个空任务
			if (1 == InterlockedIncrement(&m_nullTask._ref))
				this->Push(&m_nullTask);
			else
				VERIFY(InterlockedDecrement(&m_nullTask._ref) >= 0);

			while (INVALID_OSTHREADID == m_lastExitedTID)
			{
				if (m_recycleSleep.Sleep(1))
					return; // 析构TTaskPool时
			}
			ASSERT(!m_recycleFlag);

			int i;
			for (i = 0; i < m_maxThreads; ++i)
			{
				if (m_lastExitedTID == m_threads[(INT_PTR)i]->m_id)
					break;
			}
			ASSERT(i < m_maxThreads);
			m_lastExitedTID = INVALID_OSTHREADID;
			{
				CXTAutoLock locker(m_lock);
				delete m_threads[(INT_PTR)i];
				m_threads.erase(m_threads.begin() + i);
				VERIFY(InterlockedDecrement(&m_idleThreadCount) >= 0);
			}
			if (m_recycleSleep.Sleep(1))
				return; // 析构TTaskPool时
		}
	}
}

// class TTimerPool
// ================================================================================================= //
TTimerPool::TTimerPool(ITTaskPool* taskPool, int startupTimersAtOnce /*= MIN_STARTUP_TIMERS_AT_ONCE*/)
					: m_startupTimersAtOnce(max((int)MIN_STARTUP_TIMERS_AT_ONCE, min(startupTimersAtOnce, (int)MAX_STARTUP_TIMERS_AT_ONCE)))
					, m_taskPool(taskPool)
					, m_infoPool(sizeof(Info), 1024, 256)
					, m_syncDeleting(false)
					, m_nextTimerID(1)
					, m_exit(false)
{
	ASSERT(taskPool);
	VERIFY(m_thread.Open(THWorker, this, 0));
}

TTimerPool::~TTimerPool()
{
	ShutDown();
	ASSERT(0 == m_timerMap.size());
}

BOOL TTimerPool::AddTimer(TTimerID& id,
						fnTimerCallback timerCall,
						void* param,
						DWORD periodMs,
						LTMSEL startTime /*= MIN_UTC_MSEL*/)
{
	id = NULL;
	if (m_exit)
		return false;
	if (NULL == timerCall || INFINITE == periodMs)
	{
		ASSERT(false);
		return false;
	}

	{
		CXTAutoLock locker(m_lock);
		INT_PTR nId = 0;
		for (int i = 0; i<1024 * 1024; ++i)
		{
			BOOL ok = (m_timerMap.end() == m_timerMap.find(m_nextTimerID));
			if (ok)
				nId = m_nextTimerID;
			++m_nextTimerID;
			if (0 == m_nextTimerID) // 0为无效timerID
				m_nextTimerID = 1;
			if (ok)
				break;
		}
		if (0 == nId)
			return false;

		Info* p = m_infoPool.NewObj();
		if (NULL == p)
			return false;
		p->Init();
		p->id = (TTimerID)nId;
		p->timerCall = timerCall;
		p->param = param;
		p->periodMs = max(periodMs, (DWORD)1);
		p->startTime = startTime;
		id = (TTimerID)nId;
		VERIFY(m_timerMap.insert(std::make_pair(nId, p)).second);
	}
	m_sleep.Interrupt();
	return true;
}

BOOL TTimerPool::DelTimer(TTimerID id, BOOL wait)
{
	INT_PTR const nId = (INT_PTR)id;
	if (wait)
	{
		OSThreadID const tid = OSThreadSelf();
		BOOL first = true;
		for (;; Sleep(1))
		{
			CXTAutoLock locker(m_lock);
			TimerMap::iterator it = m_timerMap.find(nId);
			if (m_timerMap.end() == it)
			{
				if (first)
					return false; // timer不存在
				break;
			}
			first = false;

			Info* p = it->second;
			ASSERT(id == p->id);
			if (tid == p->runningThreadID)
			{
				ASSERT(false);	//在timer回调函数中对自身做同步删除,会造成死锁,要避免这种操作
				return false;
			}
			p->deleting = true;
			if (p->running)
				continue;
			m_timerMap.erase(it);
			m_infoPool.DeleteObj(p);
			break;
		}
	}
	else
	{
		CXTAutoLock locker(m_lock);
		TimerMap::iterator it = m_timerMap.find(nId);
		if (m_timerMap.end() == it)
			return false; // timer不存在
		Info* p = it->second;
		p->syncDeleting = true;
		m_syncDeleting = true;
	}

	return true;
}

BOOL TTimerPool::SetTimerInterval(TTimerID id, DWORD periodMs)
{
	if (periodMs > 0x7fffffff)
	{
		ASSERT(false); // 参数异常，调整为最大时间值
		periodMs = 0x7fffffff;
	}
	INT_PTR const nId = (INT_PTR)id;
	CXTAutoLock locker(m_lock);
	TimerMap::iterator it = m_timerMap.find(nId);
	if (m_timerMap.end() == it)
		return false;
	Info* p = it->second;
	ASSERT(id == p->id);
	p->periodMs = max(periodMs, (DWORD)1);
	return true;
}

BOOL TTimerPool::EnforceTimer(TTimerID id)
{
	INT_PTR const nId = (INT_PTR)id;
	{
		CXTAutoLock locker(m_lock);
		TimerMap::iterator it = m_timerMap.find(nId);
		if (m_timerMap.end() == it)
			return false;
		Info* p = it->second;
		ASSERT(id == p->id);
		p->enforceRunOnce = true;
	}
	if (!m_exit)
		m_sleep.Interrupt();
	return true;
}

BOOL TTimerPool::CancelAsyncDelTimer(TTimerID id)
{
	INT_PTR const nId = (INT_PTR)id;
	{
		CXTAutoLock locker(m_lock);
		TimerMap::iterator it = m_timerMap.find(nId);
		if (m_timerMap.end() == it)
			return false;
		Info* p = it->second;
		ASSERT(id == p->id);
		p->syncDeleting = false;
	}
	return true;
}

BOOL TTimerPool::GetTimerInfo(TTimerID id,
							fnTimerCallback* timerCall /*= NULL*/,
							void** param /*= NULL*/,
							DWORD* periodMSELs /*= NULL*/,
							LTMSEL* startTime /*= NULL*/,
							BOOL* syncDeleting /*= NULL*/,
							BOOL* running /*= NULL*/) const
{
	INT_PTR const nId = (INT_PTR)id;
	CXTAutoLock locker(m_lock);
	TimerMap::const_iterator it = m_timerMap.find(nId);
	if (m_timerMap.end() == it)
		return false;
	const Info* p = it->second;
	ASSERT(id == p->id);
	if (timerCall)
		*timerCall = p->timerCall;
	if (param)
		*param = p->param;
	if (periodMSELs)
		*periodMSELs = p->periodMs;
	if (startTime)
		*startTime = p->startTime;
	if (syncDeleting)
		*syncDeleting = p->syncDeleting;
	if (running)
		*running = p->running;
	return true;
}

void TTimerPool::ShutDown()
{
	m_exit = true;
	m_thread.Close();

	{
		CXTAutoLock locker(m_lock);
		for (TimerMap::iterator it = m_timerMap.begin(); it != m_timerMap.end(); ++it)
		{
			Info* p = it->second;
			p->deleting = true;
		}
	}

	for (int beginPos = 0; true; Sleep(1))
	{
		CXTAutoLock locker(m_lock);
		TimerMap::iterator it = m_timerMap.begin();
		for (int j = 0; j < beginPos; ++j)
			++it;
		int i;
		for (i = beginPos; i<(int)m_timerMap.size(); ++i, ++it)
		{
			//Info* p = m_timerMap.begin()[i].second;
			Info* p = it->second;
			if (p->running)
			{
				beginPos = i;
				break;
			}
		}
		if (i >= (int)m_timerMap.size())
			break;
	}
	//for(int i=0; i<(int)m_timerMap.size(); ++i)
	for (TimerMap::iterator it = m_timerMap.begin(); it != m_timerMap.end(); ++it)
	{
		Info* p = it->second;
		m_infoPool.DeleteObj(p);
	}
	m_timerMap.clear();
}

void TTimerPool::Info::OnPoolTask()
{
	runningThreadID = OSThreadSelf();
	ASSERT(running);
	if (!deleting && !syncDeleting)
		timerCall(param, id);
	ASSERT(running);
	//	lastTick = GetTickCount(); // 是否应该修正为本次Timer调用完成的时间？
	runningThreadID = INVALID_OSTHREADID;
	running = false;
}

int TTimerPool::THWorker(void* thisobj)
{
	((TTimerPool*)thisobj)->Worker(); return 0;
}

void TTimerPool::Worker()
{
	for (int beginPos = 0; !m_exit; m_sleep.Sleep(1))
	{
		CXTAutoLock locker(m_lock);
		// 		CXTAutoTryLock locker(m_lock);
		// 		if(!locker.IsLocked())
		// 			continue;

		// 执行清理异步删除timer动作
		if (m_syncDeleting)
		{
			BOOL doneAll = true;
			for (TimerMap::iterator it = m_timerMap.begin(); it != m_timerMap.end();)
			{
				Info* p = it->second;
				if (p->syncDeleting)
				{
					if (p->running)
					{
						doneAll = false;
						break;
					}
					else
					{
						//it = m_timerMap.erase(it);
						m_timerMap.erase(it++);
						m_infoPool.DeleteObj(p);
						continue;
					}
				}
				else
					ASSERT(true);

				++it;
			}
			if (doneAll)
				m_syncDeleting = false;
		}

		// 调度timer
		LTMSEL ltNow = INVALID_UTC_MSEL;
		BOOL ltNowGot = false; // 用于减少GetMsel()的调用，GetMsel()在linux环境比下较耗时
		DWORD tickNow = 0;
		BOOL tickNowGot = false;
		int transactedCount = 0; // 本轮循环启动的timer数

		TimerMap::iterator /*const*/ itTimerBegin = m_timerMap.begin();
		int const totalTimers = (int)m_timerMap.size();
		if (beginPos >= totalTimers)
			beginPos = 0;

		for (int i = 0; i < beginPos; ++i)
			++itTimerBegin;

		int pos;
		for (pos = beginPos; pos<totalTimers; ++pos, ++itTimerBegin)
		{
			//Info* p = itTimerBegin[pos].second;
			Info* p = itTimerBegin->second;
			if (p->running || p->deleting || p->syncDeleting)
				continue;

			if (!tickNowGot)
			{
				tickNow = GetTickCount();
				tickNowGot = true;
			}
			if (!p->started)
			{
				if (!ltNowGot)
				{
					ltNow = GetMsel();
					ltNowGot = true;
				}
				if (ltNow >= p->startTime)
				{
					p->started = true;
					p->lastTick = tickNow - p->periodMs;
				}
			}

			if (p->enforceRunOnce)
			{
				p->enforceRunOnce = false;
			}
			else
			{
				if (!p->started)
					continue;
				if (tickNow - p->lastTick < p->periodMs)
					continue;
			}

			p->running = true;
			p->lastTick = tickNow;
			m_taskPool->Push(p);
			++transactedCount;
			if (transactedCount >= m_startupTimersAtOnce)
			{
				beginPos = pos + 1;
				break;
			}
		}
		if (pos >= totalTimers)
			beginPos = 0;
	}
}