#include "cyaCore.h"
#include "cyaCoreTimer.h"

CyaCoreTimer::CyaCoreTimer(fnTimerCallback timerCall, void* param, DWORD periodMs, LTMSEL startTime /*= MIN_UTC_MSEL*/)
{
	ASSERT(timerCall && INFINITE != periodMs);
	timerInfos_.id = NULL;
	timerInfos_.asyncStopping = false;
	timerInfos_.timerCall = timerCall;
	timerInfos_.param = param;
	timerInfos_.periodMs = periodMs;
	timerInfos_.startTime = startTime;
}

CyaCoreTimer::CyaCoreTimer()
{
	timerInfos_.id = NULL;
	timerInfos_.asyncStopping = false;
	timerInfos_.timerCall = NULL;
	timerInfos_.param = NULL;
	timerInfos_.periodMs = INFINITE;
	timerInfos_.startTime = MIN_UTC_MSEL;
}

CyaCoreTimer::~CyaCoreTimer()
{
	if (!timerInfos_.asyncStopping)
		Stop();
}

BOOL CyaCoreTimer::IsStarted() const
{
	if (timerInfos_.id)
	{
		ITTimerPool* pool = CyaCore_FetchTimerPool();
		if (pool && pool->GetTimerInfo(timerInfos_.id))
			return true;
	}
	return false;
}

STimerInfos CyaCoreTimer::GetTimerInfos() const
{
	if (IsStarted())
		return timerInfos_;
	STimerInfos infos = timerInfos_;
	infos.id = NULL;
	return infos;
}

BOOL CyaCoreTimer::Start(fnTimerCallback timerCall, void* param, DWORD periodMs, LTMSEL startTime /*= MIN_UTC_MSEL*/)
{
	if (NULL == timerCall || INFINITE == periodMs)
	{
		ASSERT(false); // �������
		return false;
	}

	CXTAutoLock locker(actionLock_);

	if (timerInfos_.timerCall == timerCall && timerInfos_.param == param)
	{
		timerInfos_.startTime = startTime;
		if (!Start())
			return false;
		if (periodMs != timerInfos_.periodMs)
			SetPeriod(periodMs);
		return true;
	}

	ITTimerPool* pool = CyaCore_FetchTimerPool();
	if (timerInfos_.id)
	{
		STimerInfos infos;
		BOOL syncDeleting;
		BOOL ok = pool->GetTimerInfo(timerInfos_.id, &infos.timerCall, &infos.param, &infos.periodMs, &infos.startTime, &syncDeleting);
		if (!ok || syncDeleting || timerCall != infos.timerCall || param != infos.param || startTime != infos.startTime)
		{
			if (ok)
				pool->DelTimer(timerInfos_.id, true); // ͬ��ɾ�����ȴ�ɾ����ɺ�ŷ��أ�
			timerInfos_.id = NULL;
		}
		else if (periodMs != infos.periodMs)
		{
			if (!pool->SetTimerInterval(timerInfos_.id, periodMs))
				timerInfos_.id = NULL; // ��timer�Ѿ�������
		}
	}

	BOOL ok = true;
	if (NULL == timerInfos_.id)
		ok = pool->AddTimer(timerInfos_.id, timerCall, param, periodMs, startTime);
	timerInfos_.asyncStopping = false;
	timerInfos_.timerCall = timerCall;
	timerInfos_.param = param;
	timerInfos_.periodMs = periodMs;
	timerInfos_.startTime = startTime;
	return ok;
}

BOOL CyaCoreTimer::Start()
{
	CXTAutoLock locker(actionLock_);
	if (NULL == timerInfos_.timerCall)
	{
		ASSERT(false); // ʹ�ô���
		return false;
	}

	timerInfos_.asyncStopping = false;
	ITTimerPool* pool = CyaCore_FetchTimerPool();
	if (NULL == timerInfos_.id)
		return pool->AddTimer(timerInfos_.id, timerInfos_.timerCall, timerInfos_.param, timerInfos_.periodMs, timerInfos_.startTime);

	if (pool->CancelAsyncDelTimer(timerInfos_.id))
		return true; // Cancel�ɹ���timer����
	timerInfos_.id = NULL; // ��timer�Ѿ�������

	return pool->AddTimer(timerInfos_.id, timerInfos_.timerCall, timerInfos_.param, timerInfos_.periodMs, timerInfos_.startTime);
}

void CyaCoreTimer::Stop(BOOL wait /*= true*/)
{
	if (wait)
	{
		CXTAutoLock locker(actionLock_);
		if (timerInfos_.id)
		{
			ITTimerPool* pool = CyaCore_FetchTimerPool();
			if (pool)
				pool->DelTimer(timerInfos_.id, true);
			timerInfos_.id = NULL;
		}
		else
			ASSERT(true);
	}
	else
	{
		// �첽ֹͣ�����ܼ�����һ��Ϊtimer�߳�������ã����׵���������
		if (timerInfos_.id)
		{
			ITTimerPool* pool = CyaCore_FetchTimerPool();
			if (pool)
			{
				pool->DelTimer(timerInfos_.id, false); // �����жϳɹ�ʧ��
				timerInfos_.asyncStopping = true;
			}
			else
				ASSERT(true);
		}
		else
			ASSERT(true);
	}
}

BOOL CyaCoreTimer::SetPeriod(DWORD periodMs)
{
	if (timerInfos_.id)
	{
		ITTimerPool* pool = CyaCore_FetchTimerPool();
		if (pool && pool->SetTimerInterval(timerInfos_.id, periodMs))
			return true;
		else
			ASSERT(true); // ��timer�Ѳ����ڣ�����������
	}
	else
		ASSERT(true);
	timerInfos_.periodMs = periodMs;
	return false;
}

BOOL CyaCoreTimer::Enforce()
{
	if (timerInfos_.id)
	{
		ITTimerPool* pool = CyaCore_FetchTimerPool();
		if (pool && pool->EnforceTimer(timerInfos_.id))
			return true;
		else
			ASSERT(true); // ��timer�Ѳ����ڣ�����������
	}
	else
		ASSERT(true);
	return false;
}