#include "InstFinHistoryMan.h"
#include "RoleInfos.h"
#include "ConfigFileMan.h"
#include "DBSClient.h"
#include "GameCfgFileMan.h"

static InstFinHistoryMan* sg_instHistoryMan = NULL;
void InitInstFinHistoryMan()
{
	ASSERT(sg_instHistoryMan == NULL);
	sg_instHistoryMan = new InstFinHistoryMan();
	ASSERT(sg_instHistoryMan != NULL);
}

InstFinHistoryMan* GetInstFinHistoryMan()
{
	return sg_instHistoryMan;
}

void DestroyInstFinHistoryMan()
{
	InstFinHistoryMan* instHistoryMan = sg_instHistoryMan;
	sg_instHistoryMan = NULL;
	if (NULL != instHistoryMan)
		delete instHistoryMan;
}

InstFinHistoryMan::InstFinHistoryMan()
{
	DWORD timerTick = GetConfigFileMan()->GetSyncInstHistoryTime();
	if (timerTick > 0)
		m_refreshTimer.Start(RefreshInstFinHistoryTimer, this, timerTick * 1000);
}

InstFinHistoryMan::~InstFinHistoryMan()
{
	if (GetConfigFileMan()->GetSyncInstHistoryTime() > 0)
	{
		m_refreshTimer.Stop();
		OnRefreshInstFinHistory();
	}
}

void InstFinHistoryMan::InputInstFinRecord(UINT32 roleId, UINT16 instId, BYTE star)
{
	SInstHistoryRecord rec(roleId, instId, star);
	char szKey[32] = { 0 };
	sprintf(szKey, "%u-%d", roleId, instId);

	ASSERT(printf(" InputInstFinRecord roleid=%d,instid=%d,star=%d\n", roleId, instId, star));

	if (GetConfigFileMan()->GetSyncInstHistoryTime() > 0)
	{
		BOOL enforceSyncDB = false;
		{
			CXTAutoLock lock(m_locker);
			std::map<std::string, SInstHistoryRecord>::iterator it = m_instHistoryMap.find(szKey);
			if (it == m_instHistoryMap.end())
				m_instHistoryMap.insert(std::make_pair(szKey, rec));
			else
			{
				if (star > it->second.star)
					it->second.star = star;
				it->second.comlatedTimes = SGSU16Add(it->second.comlatedTimes, 1);
			}

			enforceSyncDB = m_instHistoryMap.size() >= GetConfigFileMan()->GetMaxCacheItems() ? true : false;
		}

		if (enforceSyncDB)
			m_refreshTimer.Enforce();
	}
	else
	{
		UpdateInstFinRecord(rec);
	}
}

void InstFinHistoryMan::RefreshInstFinHistoryTimer(void* param, TTimerID /*id*/)
{
	InstFinHistoryMan* pThis = (InstFinHistoryMan*)param;
	if (pThis)
		pThis->OnRefreshInstFinHistory();
}

void InstFinHistoryMan::OnRefreshInstFinHistory()
{
	if (m_instHistoryMap.empty())
		return;

	std::vector<SInstHistoryRecord> instHistoryVec;
	{
		CXTAutoLock lock(m_locker);
		std::map<std::string, SInstHistoryRecord>::iterator it = m_instHistoryMap.begin();
		for (; it != m_instHistoryMap.end(); ++it)
			instHistoryVec.push_back(it->second);
		m_instHistoryMap.clear();
	}

	size_t si = instHistoryVec.size();
	for (size_t i = 0; i < si; ++i)
		UpdateInstFinRecord(instHistoryVec[i]);
}

int InstFinHistoryMan::UpdateInstFinRecord(const SInstHistoryRecord& rec)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select id, star, complatednum from instancehistory where actorid=%u and instanceid=%d", rec.roleId, rec.instId);
	DBS_RESULT result = NULL;
	int ret = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (ret != DBS_OK)
	{
		ASSERT(false);
		return ret;
	}

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows == 0 || nCols == 0)
	{
		UINT16 chapterId = 0;
		if (GetGameCfgFileMan()->GetInstOwnerChapter(rec.instId, chapterId) == MLS_OK)
		{
			memset(szSQL, 0, sizeof(szSQL));
			sprintf(szSQL, "insert into instancehistory(actorid, instanceid, chapterid, star, startts, complatednum, lasttts, daynum) values(%u, %d, %d, %d, '%s', %d, '%s',1)",
				rec.roleId, rec.instId, chapterId, rec.star, rec.fintts, rec.comlatedTimes, rec.fintts);
			GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);
		}
	}
	else
	{
		memset(szSQL, 0, sizeof(szSQL));
		DBS_ROW row = DBS_FetchRow(result);
		ASSERT(row != NULL);
		UINT32 id = (UINT32)_atoi64(row[0]);
		BYTE star = (BYTE)atoi(row[1]);	//评星
		UINT16 complatedNum = (UINT16)atoi(row[2]);	//完成次数
		complatedNum = SGSU16Add(complatedNum, rec.comlatedTimes);
		///update by hxw 20151015 增加daynum=daynum+1，用于记录该副本每天的挑战次数。该次数会在服务器进行副本排行统计时清0
		if (rec.star > star)
			sprintf(szSQL, "update instancehistory set star=%d, complatednum=%d, startts='%s', lasttts='%s',daynum=daynum+1  where id=%u",
			rec.star, complatedNum, rec.fintts, rec.fintts, id);
		else
			sprintf(szSQL, "update instancehistory set complatednum=%u, lasttts='%s' daynum=daynum+1 where id=%u",
			complatedNum, rec.fintts, id);
		GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
	}
	DBS_FreeResult(result);
	return MLS_OK;
}

void InstFinHistoryMan::EnforceSyncInstFinRecord(UINT32 roleId)
{
	BOOL needSyncDB = false;
	SInstHistoryRecord instRec;

	{
		CXTAutoLock lock(m_locker);
		std::map<std::string, SInstHistoryRecord>::iterator it = m_instHistoryMap.begin();
		for (; it != m_instHistoryMap.end(); ++it)
		{
			const char* p = it->first.c_str();
			UINT32 id = (UINT32)_atoi64(p);
			if (id != roleId)
				continue;

			needSyncDB = true;
			instRec = it->second;
			m_instHistoryMap.erase(it);
			break;
		}
	}

	if (needSyncDB)
		UpdateInstFinRecord(instRec);
}

int InstFinHistoryMan::QueryFinishedInst(UINT32 roleId, UINT16 from, BYTE num, SQueryFinishedInstRes* pFinInstRes, BYTE maxNum)
{
	//查数据库总记录条数
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select count(actorid) from instancehistory where actorid=%u", roleId);
	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != MLS_OK)
		return retCode;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols < 1)
	{
		DBS_FreeResult(result);
		return retCode;
	}

	DBS_ROW row = DBS_FetchRow(result);
	ASSERT(row != NULL);
	UINT16 total = atoi(row[0]);
	DBS_FreeResult(result);

	pFinInstRes->total = total;
	if (total <= 0 || num <= 0)
		return retCode;

	//查数据库历史记录信息
	memset(szSQL, 0, sizeof(szSQL));
	sprintf(szSQL, "select id, instanceid, star, complatednum from instancehistory where actorid=%u limit %d, %d", roleId, from, num);
	retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != MLS_OK)
		return retCode;

	ASSERT(printf("QueryFinishedInst roleid=%u,allnum=%d,fromnum=%d,endnum=%d,maxnum=%d\n", roleId, total, from, num, maxNum));

	nRows = DBS_NumRows(result);
	nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols < 4)
	{
		DBS_FreeResult(result);
		return retCode;
	}

	BYTE idx = 0;
	while ((row = DBS_FetchRow(result)) != NULL)
	{
		pFinInstRes->instInfos[idx].id = _atoi64(row[0]);
		pFinInstRes->instInfos[idx].instId = atoi(row[1]);
		pFinInstRes->instInfos[idx].star = atoi(row[2]);
		pFinInstRes->instInfos[idx].complatedNum = atoi(row[3]);
		pFinInstRes->instInfos[idx].hton();
		++idx;
		if (idx >= num || idx >= maxNum)
			break;
	}
	pFinInstRes->curNum = idx;
	DBS_FreeResult(result);
	return retCode;
}

BOOL InstFinHistoryMan::IsFinishedInst(UINT32 roleId, UINT16 instId)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select id from instancehistory where actorid=%u and instanceid=%d", roleId, instId);

	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != MLS_OK)
		return false;

	UINT32 nRows = DBS_NumRows(result);
	DBS_FreeResult(result);

	return nRows > 0 ? true : false;
}

int InstFinHistoryMan::GetAvtiveInstChallengeTimes(UINT32 roleId, SGetActiveInstChallengeTimesRes* pChallengeTimesRes)
{
	UINT16 maxNum = (MAX_RES_USER_BYTES - member_offset(SGetActiveInstChallengeTimesRes, num, data)) / sizeof(SActiveInstItem);

	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select instid, times from activeinst where actorid=%u", roleId);
	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols < 2)
	{
		DBS_FreeResult(result);
		return MLS_OK;
	}

	DBS_ROW row = NULL;
	while ((row = DBS_FetchRow(result)) != NULL)
	{
		pChallengeTimesRes->data[pChallengeTimesRes->num].instId = atoi(row[0]);
		pChallengeTimesRes->data[pChallengeTimesRes->num].times = atoi(row[1]);
		if (++pChallengeTimesRes->num >= maxNum)
			break;
	}
	DBS_FreeResult(result);
	return MLS_OK;
}

int InstFinHistoryMan::UpdateAvtiveInstFinTimes(UINT32 roleId, UINT16 instId)
{
	char szDate[32] = { 0 };
	GetLocalStrTime(szDate);
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "update activeinst set times=times+1, tts='%s' where actorid=%u and instid=%d", szDate, roleId, instId);
	UINT32 affectRows = 0;
	GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL, &affectRows);
	if (affectRows > 0)
		return MLS_OK;

	memset(szSQL, 0, sizeof(szSQL));
	sprintf(szSQL, "insert into activeinst(actorid, instid, times, tts) values(%u, %d, %d, '%s')", roleId, instId, 1, szDate);
	return GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);
}

void InstFinHistoryMan::ClearPlayersActiveInstRecord()
{
	GetDBSClient()->Del(GetConfigFileMan()->GetDBName(), "delete from activeinst");
}