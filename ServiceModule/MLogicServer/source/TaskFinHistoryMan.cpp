#include "TaskFinHistoryMan.h"
#include "DBSClient.h"
#include "ConfigFileMan.h"
#include "ServiceErrorCode.h"
#include "cyaMaxMin.h"
#include "GameCfgFileMan.h"
#include <set>

static TaskFinHistoryMan* sg_taskFinMan = NULL;
void InitTaskFinHistoryMan()
{
	sg_taskFinMan = new TaskFinHistoryMan();
	ASSERT(sg_taskFinMan != NULL);
}

TaskFinHistoryMan* GetTaskFinHistoryMan()
{
	return sg_taskFinMan;
}

void DestroyTaskFinHistoryMan()
{
	TaskFinHistoryMan* taskFinMan = sg_taskFinMan;
	sg_taskFinMan = NULL;
	if (taskFinMan != NULL)
		delete taskFinMan;
}

TaskFinHistoryMan::TaskFinHistoryMan()
{
	int nTimer = GetConfigFileMan()->GetSyncTaskHistoryTime();
	if (nTimer > 0)
		m_refreshTaskTimer.Start(SyncDBFinTskTimer, this, nTimer * 1000);
}

TaskFinHistoryMan::~TaskFinHistoryMan()
{
	if (GetConfigFileMan()->GetSyncTaskHistoryTime() > 0)
	{
		m_refreshTaskTimer.Stop();
		OnSyncDBFinTsk();
	}
}

int TaskFinHistoryMan::QueryRoleFinTaskHistory(UINT32 roleId, UINT16 from, UINT16 num, SQueryFinishedTaskRes* pFinTasks)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select taskid from taskhistory where actorid=%u limit %d, %d", roleId, from, num);
	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != MLS_OK)
		return retCode;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols <= 0)
	{
		DBS_FreeResult(result);
		return retCode;
	}

	UINT16 idx = 0;
	UINT16 maxNum = min(num, (MAX_RES_USER_BYTES - member_offset(SQueryFinishedTaskRes, num, finTaskIds)) / sizeof(UINT16));
	DBS_ROW row = NULL;
	while ((row = DBS_FetchRow(result)) != NULL)
	{
		UINT16 taskId = atoi(row[0]);
		pFinTasks->finTaskIds[idx] = htons(taskId);
		if (++idx >= maxNum)
			break;
	}
	pFinTasks->num = idx;
	DBS_FreeResult(result);
	return retCode;
}

BOOL TaskFinHistoryMan::IsRoleFinishedMainTasks(INT32 roleId, const std::vector<UINT16>& tasksVec)
{
	int nSize = (int)tasksVec.size();
	if (nSize <= 0)
		return true;

	char szSQL[GENERIC_SQL_BUF_LEN * 4] = { 0 };
	sprintf(szSQL, "select count(id) from taskhistory where actorid=%u and type=%d and (taskid=%d", roleId, ESGS_SYS_MAIN_TASK, tasksVec[0]);
	for (int i = 1; i < nSize; ++i)
	{
		char tasksBuf[64] = { 0 };
		sprintf(tasksBuf, " or taskid=%d", tasksVec[i]);
		strcat(szSQL, tasksBuf);
	}
	strcat(szSQL, ")");

	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != MLS_OK)
		return false;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols <= 0)
	{
		DBS_FreeResult(result);
		return false;
	}
	DBS_ROW row = DBS_FetchRow(result);
	int nCount = atoi(row[0]);
	DBS_FreeResult(result);

	return nCount >= nSize ? true : false;
}

void TaskFinHistoryMan::InputRoleFinTask(INT32 roleId, UINT16 taskId, BYTE taskType, UINT16 chapterId)
{
	if (GetConfigFileMan()->GetSyncTaskHistoryTime() > 0)
	{
		SFinTaskInfo finTaskInfo;
		finTaskInfo.taskId = taskId;
		finTaskInfo.taskType = taskType;
		finTaskInfo.chapterId = chapterId;
		finTaskInfo.fintts = GetMsel();

		BOOL enforceSyncDB = false;
		{
			CXTAutoLock lock(m_finTaskMapLocker);
			std::map<UINT32, std::list<SFinTaskInfo> >::iterator it = m_finTaskMap.find(roleId);
			if (it == m_finTaskMap.end())
			{
				std::list<SFinTaskInfo> finTaskList;
				m_finTaskMap.insert(std::make_pair(roleId, finTaskList));
			}

			std::list<SFinTaskInfo>& finTaskListRef = m_finTaskMap[roleId];
			finTaskListRef.push_back(finTaskInfo);
			enforceSyncDB = m_finTaskMap.size() >= GetConfigFileMan()->GetMaxCacheItems() ? true : false;
		}

		if (enforceSyncDB)
			m_refreshTaskTimer.Enforce();
	}
	else
	{
		char szDate[SGS_DATE_TIME_LEN] = { 0 };
		GetLocalStrTime(szDate);
		InsertDBFinTask(roleId, taskId, taskType, chapterId, szDate);
	}
}

void TaskFinHistoryMan::SyncDBFinTskTimer(void* param, TTimerID /*id*/)
{
	TaskFinHistoryMan* pThis = (TaskFinHistoryMan*)param;
	if (pThis)
		pThis->OnSyncDBFinTsk();
}

void TaskFinHistoryMan::OnSyncDBFinTsk()
{
	std::map<UINT32, std::list<SFinTaskInfo> > finTaskMap;
	{
		if (m_finTaskMap.empty())
			return;

		CXTAutoLock lock(m_finTaskMapLocker);
		finTaskMap.insert(m_finTaskMap.begin(), m_finTaskMap.end());
		m_finTaskMap.clear();
	}

	std::map<UINT32, std::list<SFinTaskInfo> >::iterator it = finTaskMap.begin();
	for (; it != finTaskMap.end(); ++it)
	{
		std::list<SFinTaskInfo>& finTaskList = it->second;
		std::list<SFinTaskInfo>::iterator it2 = finTaskList.begin();
		for (; it2 != finTaskList.end(); ++it2)
		{
			SYSTEMTIME st;
			char szDate[SGS_DATE_TIME_LEN] = { 0 };
			ToTimeEx(it2->fintts, &st, true);
			sprintf(szDate, _T("%04d-%02d-%02d %02d:%02d:%02d"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
			InsertDBFinTask(it->first, it2->taskId, it2->taskType, it2->chapterId, szDate);
		}
	}
}

void TaskFinHistoryMan::InsertDBFinTask(INT32 roleId, UINT16 taskId, BYTE taskType, UINT16 chapterId, const char* pszDate)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "insert into taskhistory(actorid, taskid, type, chapterid, tts) values(%u, %d, %d, %d, '%s')",
		roleId, taskId, taskType, chapterId, pszDate);
	GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);
}

BOOL TaskFinHistoryMan::IsFinishedTask(INT32 roleId, UINT16 taskId)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select count(id) from taskhistory where actorid=%u and taskid=%d", roleId, taskId);

	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return false;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows == 0 || nCols == 0)
	{
		DBS_FreeResult(result);
		return false;
	}

	DBS_ROW row = DBS_FetchRow(result);
	int n = atoi(row[0]);
	DBS_FreeResult(result);
	return n > 0 ? true : false;
}

UINT16 TaskFinHistoryMan::GetCurrentOpenChapter(UINT32 roleId)
{
	UINT16 chapterId = 0;
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select chapterid, taskid from taskhistory where actorid=%u and type=%d and chapterid="
		"(select max(chapterid) from taskhistory where actorid=%u and type=%d)", roleId, ESGS_SYS_MAIN_TASK, roleId, ESGS_SYS_MAIN_TASK);

	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return chapterId;

	DBS_ROW row = NULL;
	std::set<UINT16> mainTaskSet;
	while ((row == DBS_FetchRow(result)) != NULL)
	{
		chapterId = atoi(row[0]);
		mainTaskSet.insert(atoi(row[1]));
	}
	DBS_FreeResult(result);

	ChapterCfg::SChapterItem* pChapterItem = NULL;
	retCode = GetGameCfgFileMan()->GetChapterItem(chapterId, pChapterItem);
	if (retCode != MLS_OK)
		return chapterId;

	std::vector<UINT16>& mainTasks = pChapterItem->mainTasks;
	if (mainTaskSet.size() < mainTasks.size())
		return chapterId;

	int si = mainTasks.size();
	for (int i = 0; i < si; ++i)
	{
		if (mainTaskSet.find(mainTasks[i]) == mainTaskSet.end())
			return chapterId;
	}

	chapterId = pChapterItem->nextChapterId == 0 ? chapterId : pChapterItem->nextChapterId;
	return chapterId;
}