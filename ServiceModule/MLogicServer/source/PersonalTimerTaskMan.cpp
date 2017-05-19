#include "PersonalTimerTaskMan.h"
#include "DBSClient.h"
#include "ConfigFileMan.h"
#include "cyaMaxMin.h"
#include "GWSClientMan.h"

#define TIMER_CORRECTED_VALUE 30	//ms

static PersonalTimerTaskMan* sg_ptimerTaskMan = NULL;
void InitPersonalTimerTaskMan()
{
	ASSERT(sg_ptimerTaskMan == NULL);
	sg_ptimerTaskMan = new PersonalTimerTaskMan();
	ASSERT(sg_ptimerTaskMan != NULL);
	//sg_ptimerTaskMan->Start();
}

PersonalTimerTaskMan* GetPersonalTimerTaskMan()
{
	return sg_ptimerTaskMan;
}

void DestroyPersonalTimerTaskMan()
{
	PersonalTimerTaskMan* ptimerTaskMan = sg_ptimerTaskMan;
	sg_ptimerTaskMan = NULL;
	if (ptimerTaskMan != NULL)
	{
		ptimerTaskMan->Stop();
		delete ptimerTaskMan;
	}
}

PersonalTimerTaskMan::PersonalTimerTaskMan()
: m_timerThread(INVALID_OSTHREAD)
, m_exit(true)
{

}

PersonalTimerTaskMan::~PersonalTimerTaskMan()
{

}

void PersonalTimerTaskMan::Start()
{
	LoadPersonalTimerTask();
	m_exit = false;
	OSCreateThread(&m_timerThread, NULL, THWorker, this, 0);
}

void PersonalTimerTaskMan::Stop()
{
	m_exit = true;
	m_sleep.Interrupt();
	if (m_timerThread != INVALID_OSTHREAD)
	{
		OSCloseThread(m_timerThread);
		m_timerThread = INVALID_OSTHREAD;
	}

	CXTAutoLock lock(m_locker);
	m_personalTimerTasks.clear();
}

int PersonalTimerTaskMan::JoinPersonalTimerTaskQ(UINT32 roleId, UINT16 taskId, BYTE targetId, UINT32 userSessionId, UINT32& newId)
{
	char szTime[32] = { 0 };
	GetLocalStrTime(szTime);

	PersonalTimerTask timerTask;
	timerTask.startMSel = LocalStrTimeToMSel(szTime);
	timerTask.targetId = targetId;
	timerTask.taskId = taskId;
	timerTask.userId = userSessionId;
	timerTask.taskSec = 0;	//读配置文件获取该定时任务相关信息

	{
		CXTAutoLock lock(m_locker);
		std::map<UINT32, PersonalTimerTask>::iterator it = m_personalTimerTasks.end();
		if (it == m_personalTimerTasks.begin())
			newId = 1;
		else
		{
			--it;
			newId = it->first + 1;
		}
		m_personalTimerTasks.insert(std::make_pair(newId, timerTask));
	}

	//所有的定时任务都需要写进数据库吗 ???? , 有点无谓的消耗资源
	char szSQL[1024] = { 0 };
	sprintf(szSQL, "insert into personaltimertask values(%u, %d, %u, '%s')", newId, taskId, roleId, szTime);
	int retCode = GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);
	if (retCode == DBS_OK)
	{
		m_sleep.Interrupt();	//打断休眠
		return MLS_OK;
	}

	{
		CXTAutoLock lock(m_locker);
		m_personalTimerTasks.erase(newId);
	}

	newId = 0;
	return retCode;
}

void PersonalTimerTaskMan::LoadPersonalTimerTask()
{
	DBS_RESULT result = NULL;
	int ret = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), "select id, taskid, actorid, starttime from personaltimertask", result);
	if (ret != DBS_OK)
		return;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols < 4)
	{
		DBS_FreeResult(result);
		return;
	}

	LTMSEL nowMSel = GetMsel();

	DBS_ROW row = NULL;
	while ((row = DBS_FetchRow(result)) != NULL)
	{
		UINT32 timerSec = 0;
		UINT32 id = (UINT32)_atoi64(row[0]);
		LTMSEL startSec = LocalStrTimeToMSel(row[3]);
		LTMSEL pastSec = nowMSel - startSec;
		if (pastSec <= 0)
		{
			ASSERT(false);
			continue;
		}

		PersonalTimerTask timerTask;
		timerTask.taskId = atoi(row[1]);
		timerTask.startMSel = startSec;
		timerTask.taskSec = timerSec;
		m_personalTimerTasks.insert(std::make_pair(id, timerTask));
	}
	DBS_FreeResult(result);
}

int PersonalTimerTaskMan::THWorker(void* param)
{
	PersonalTimerTaskMan* pThis = (PersonalTimerTaskMan*)param;
	ASSERT(pThis != NULL);
	return pThis->OnWorker();
}

int PersonalTimerTaskMan::OnWorker()
{
	UINT32 sleepMSel = 10;
	for (; !m_exit; m_sleep.Sleep(sleepMSel))
	{
		if (m_personalTimerTasks.empty())
			continue;

		sleepMSel = 0;
		LTMSEL nowMSel = GetMsel();
		std::map<UINT32, PersonalTimerTask> okTasks;

		{
			CXTAutoLock lock(m_locker);
			std::map<UINT32, PersonalTimerTask>::iterator it = m_personalTimerTasks.begin();
			for (; it != m_personalTimerTasks.end();)
			{
				if (nowMSel < it->second.startMSel)
				{
					ASSERT(false);
					++it;
					continue;
				}

				LTMSEL taskMSec = it->second.taskSec * 1000;
				LTMSEL leftMSel = taskMSec - (nowMSel - it->second.startMSel);
				if (leftMSel <= TIMER_CORRECTED_VALUE)
				{
					okTasks.insert(std::make_pair(it->first, it->second));
					m_personalTimerTasks.erase(it++);
				}
				else
				{
					UINT32 msel = leftMSel / 2;
					if (sleepMSel == 0)
						sleepMSel = msel;
					else if (sleepMSel > msel)
						sleepMSel = msel;
					++it;
				}
			}
		}

		sleepMSel = max(10, sleepMSel);

		char buf[GWS_COMMON_REPLY_LEN + sizeof(SPersonalTimerTaskRes)] = { 0 };
		SPersonalTimerTaskRes* pNotify = (SPersonalTimerTaskRes*)(buf + GWS_COMMON_REPLY_LEN);
		std::map<UINT32, PersonalTimerTask>::iterator it2 = okTasks.begin();
		for (; it2 != okTasks.end(); ++it2)
		{
			//通知用户定时任务完成
			pNotify->newId = it2->first;
			pNotify->taskId = it2->second.taskId;
			pNotify->hton();
			GWSClient* gwsSession = GetGWSClientMan()->RandGWSClient();
			if (gwsSession == NULL)
				continue;

			gwsSession->SendResponse(it2->second.targetId, it2->second.userId, MLS_OK, MLS_PERSONAL_TIMER_TASK_FIN, buf, GWS_COMMON_REPLY_LEN + sizeof(SPersonalTimerTaskRes));
		}
	}
	return 0;
}