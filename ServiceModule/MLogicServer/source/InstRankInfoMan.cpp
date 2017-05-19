#include "InstRankInfoMan.h"
#include "DBSClient.h"
#include "ConfigFileMan.h"
#include "LuaScriptMan.h"
#include "GameCfgFileMan.h"
#include "cyaLog.h"

static InstRankInfoMan* sg_instRankMan = NULL;
void InitInstRankInfoMan()
{
	ASSERT(sg_instRankMan == NULL);
	sg_instRankMan = new InstRankInfoMan();
	sg_instRankMan->LoadInstRankInfo();
	ASSERT(sg_instRankMan != NULL);
}

InstRankInfoMan* GetInstRankInfoMan()
{
	return sg_instRankMan;
}

void DestroyInstRankInfoMan()
{
	InstRankInfoMan* instRankMan = sg_instRankMan;
	sg_instRankMan = NULL;
	if (instRankMan != NULL)
		delete instRankMan;
}

InstRankInfoMan::InstRankInfoMan()
{

}

InstRankInfoMan::~InstRankInfoMan()
{
	m_refreshDBTimer.Stop();

	CXTAutoLock lock(m_locker);
	for (std::map<UINT16/*副本id*/, InstRankSite*>::iterator it = m_instRankMap.begin(); it != m_instRankMap.end(); ++it)
	{
		it->second->SyncDBInstRank();
		delete it->second;
	}
	m_instRankMap.clear();
}

void InstRankInfoMan::LoadInstRankInfo()
{
	UINT32 from = 0;
	UINT32 totals = 0;

#define MAX_INST_RANKS_PER_PAGE	1024

	while (true)
	{
		DBS_RESULT result = NULL;
		char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
		sprintf(szSQL, "select id, instid, actorid, nick, score, tts, accountid from rankinstance limit %u, %u", from, MAX_INST_RANKS_PER_PAGE);
		int ret = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
		if (ret != DBS_OK)
		{
			LogInfo(("检索数据库失败[%d]!", ret));
			break;
		}
		UINT32 nRows = DBS_NumRows(result);
		UINT16 nCols = DBS_NumFields(result);
		if (nRows <= 0 || nCols < 7)
		{
			DBS_FreeResult(result);
			break;
		}

		totals += nRows;

		DBS_ROW row = NULL;
		while ((row = DBS_FetchRow(result)) != NULL)
		{
			SInstRankTable rank;
			rank.uniqueId = (UINT32)_atoi64(row[0]);
			UINT16 instId = atoi(row[1]);
			rank.roleId = (UINT32)_atoi64(row[2]);
			strcpy(rank.roleNick, row[3]);
			rank.score = (UINT32)atoi(row[4]);
			strcpy(rank.tts, row[5]);
			rank.userId = (UINT32)_atoi64(row[6]);

			InsertDBInstRankTable(instId, rank);
		}
		DBS_FreeResult(result);

		if (nRows < MAX_INST_RANKS_PER_PAGE)
			break;
		from += nRows;
	}

	LogInfo(("载入%u条副本完成排行榜信息!", totals));

	DWORD dwTimerMSel = GetConfigFileMan()->GetSyncInstRankTime() * 1000;
	ASSERT(dwTimerMSel > 0);
	if (dwTimerMSel > 0)
		m_refreshDBTimer.Start(SyncInstRankTimer, this, dwTimerMSel);

#undef MAX_INST_RANKS_PER_PAGE
}

void InstRankInfoMan::InsertDBInstRankTable(UINT16 instId, SInstRankTable& instRank)
{
	InstRankSite* pSite = NULL;

	{
		CXTAutoLock lock(m_locker);
		std::map<UINT16/*副本id*/, InstRankSite*>::iterator it = m_instRankMap.find(instId);
		if (it == m_instRankMap.end())
		{
			pSite = new InstRankSite(instId);
			m_instRankMap.insert(std::make_pair(instId, pSite));
		}
		else
			pSite = it->second;
	}

	ASSERT(pSite != NULL);
	pSite->InsertInstRank(instRank);
}

void InstRankInfoMan::SyncInstRankTimer(void* param, TTimerID /*id*/)
{
	InstRankInfoMan* pThis = (InstRankInfoMan*)param;
	ASSERT(pThis != NULL);
	pThis->OnSyncInstRank();
}

void InstRankInfoMan::OnSyncInstRank()
{
	if (m_instRankMap.empty())
		return;

	std::map<UINT16/*副本id*/, InstRankSite*> instRankMap;

	{
		CXTAutoLock lock(m_locker);
		instRankMap.insert(m_instRankMap.begin(), m_instRankMap.end());
	}

	std::map<UINT16/*副本id*/, InstRankSite*>::iterator it = instRankMap.begin();
	for (; it != instRankMap.end(); ++it)
	{
		InstRankSite* pSite = it->second;
		ASSERT(pSite != NULL);
		pSite->SyncDBInstRank();
	}
}

void InstRankInfoMan::UpdateInstRankInfo(UINT16 instId, UINT32 userId, const SInstRankInfo& rank, std::list<SInstOccupyEmail>& emailList)
{
	InstRankSite* pSite = NULL;

	{
		CXTAutoLock lock(m_locker);
		std::map<UINT16/*副本id*/, InstRankSite*>::iterator it = m_instRankMap.find(instId);
		if (it == m_instRankMap.end())
		{
			pSite = new InstRankSite(instId);
			m_instRankMap.insert(std::make_pair(instId, pSite));
		}
		else
			pSite = it->second;
	}

	ASSERT(pSite != NULL);
	SInstRankTable instRank;
	instRank.userId = userId;
	instRank.roleId = rank.roleId;
	instRank.uniqueId = 0;
	instRank.score = rank.score;
	strcpy(instRank.tts, rank.tts);
	strcpy(instRank.roleNick, rank.roleNick);
	pSite->UpdateInstRank(instRank, emailList);
}

BYTE InstRankInfoMan::QueryInstRankInfo(UINT16 instId, SInstRankInfo* pRankInfo, BYTE rankInfoNum)
{
	InstRankSite* pSite = NULL;

	{
		CXTAutoLock lock(m_locker);
		std::map<UINT16/*副本id*/, InstRankSite*>::iterator it = m_instRankMap.find(instId);
		if (it != m_instRankMap.end())
			pSite = it->second;
	}

	if (pSite == NULL)
		return 0;

	return pSite->QueryInstRankInfo(pRankInfo, rankInfoNum);
}

int InstRankInfoMan::GetRoleOccupyInst(UINT32 userId, UINT32 roleId, SGetRoleOccupyInstRes* pOccupyInstRes, int maxNum)
{
	UINT16 idx = 0;
	{
		CXTAutoLock lock(m_locker);
		std::map<UINT16/*副本id*/, InstRankSite*>::iterator it = m_instRankMap.begin();
		for (; it != m_instRankMap.end(); ++it)
		{
			UINT16 instId = 0;
			int retCode = it->second->GetOccupyInstIdByRoleId(roleId, instId);
			if (retCode != MLS_OK)
				continue;

			pOccupyInstRes->occupyInstIds[idx++] = instId;

			if (idx >= maxNum)
				break;
		}
	}
	pOccupyInstRes->num = idx;
	return MLS_OK;
}

void InstRankInfoMan::GetInstOccupyInfo(std::list<SInstOccupyRole>& occupyList)
{
	CXTAutoLock lock(m_locker);
	std::map<UINT16/*副本id*/, InstRankSite*>::iterator it = m_instRankMap.begin();
	for (; it != m_instRankMap.end(); ++it)
	{
		SInstOccupyRole occupy;
		int retCode = it->second->GetInstOccupyInfo(occupy.userId, occupy.roleId);
		if (retCode != MLS_OK)
			continue;

		occupy.instId = it->first;
		occupyList.push_back(occupy);
	}
}

void InstRankInfoMan::DelRoleInstRank(UINT32 roleId)
{
	CXTAutoLock lock(m_locker);
	std::map<UINT16/*副本id*/, InstRankSite*>::iterator it = m_instRankMap.begin();
	for (; it != m_instRankMap.end(); ++it)
	{
		InstRankSite* pSite = it->second;
		if (pSite == NULL)
			continue;

		pSite->DelRoleInstRank(roleId);
	}
}

InstRankInfoMan::InstRankSite::InstRankSite(UINT16 instId)
: m_instId(instId)
, m_instGameMode(0)
{

}

InstRankInfoMan::InstRankSite::~InstRankSite()
{

}

void InstRankInfoMan::InstRankSite::InsertInstRank(SInstRankTable& rank)
{
	CXTAutoLock lock(m_rankListLocker);
	if (m_rankList.empty())
		m_rankList.push_back(rank);
	else
	{
		BOOL isInsert = false;
		for (std::list<SInstRankTable>::iterator it = m_rankList.begin(); it != m_rankList.end(); ++it)
		{
			if (rank.score >= it->score)
			{
				isInsert = true;
				m_rankList.insert(it, rank);
				break;
			}
		}

		if (!isInsert)
			m_rankList.push_back(rank);
	}
}

void InstRankInfoMan::InstRankSite::UpdateInstRank(SInstRankTable& rank, std::list<SInstOccupyEmail>& emailList)
{
	//isOccupy = false;
	UINT32 occupyRoleId = 0;
	BYTE maxRankNum = GetGameCfgFileMan()->GetMaxInstRankNum();

	CXTAutoLock lock(m_rankListLocker);
	if (m_rankList.size() > 0)
		occupyRoleId = m_rankList.begin()->roleId;

	//清除自己已有的排名
	BOOL needUpdate = true;
	for (std::list<SInstRankTable>::iterator it = m_rankList.begin(); it != m_rankList.end(); ++it)
	{
		SInstRankTable& rkInfo = *it;
		if (rkInfo.roleId == rank.roleId)
		{
			if (rank.score >= rkInfo.score)
			{
				rank.uniqueId = rkInfo.uniqueId;
				m_rankList.erase(it);
			}
			else
				needUpdate = false;

			break;
		}
	}

	if (!needUpdate)
		return;

	BYTE curRankNum = (BYTE)m_rankList.size();
	BOOL isRankFull = curRankNum >= maxRankNum ? true : false;
	if (curRankNum <= 0)	//无排名
	{
		rank.up = 1;
		m_rankList.push_back(rank);

		if (occupyRoleId != rank.roleId)
		{
			SInstOccupyEmail email;
			email.type = EINST_FIRST_OCCUPY;
			emailList.push_back(email);
		}

		//	isOccupy = true;
		return;
	}

	SInstRankTable& backItem = m_rankList.back();
	if (rank.score < backItem.score)	//<最低连击数
	{
		if (!isRankFull)	//排行榜未满
		{
			rank.up = 1;
			m_rankList.push_back(rank);
		}
	}
	else
	{
		// 			SInstRankTable& firstItem = *m_rankList.begin();
		// 			if(rank.comboNumber >= firstItem.comboNumber)
		// 				isOccupy = true;

		std::list<SInstRankTable>::iterator insertIt = m_rankList.end();
		for (std::list<SInstRankTable>::iterator it = m_rankList.begin(); it != m_rankList.end(); ++it)
		{
			SInstRankTable& rkInfo = *it;
			if (rank.score >= rkInfo.score)
			{
				insertIt = it;
				break;
			}

			insertIt = it;
		}

		ASSERT(insertIt != m_rankList.end());
		if (isRankFull)	//排行榜满
		{
			//挤掉最后一个
			rank.up = 1;
			rank.uniqueId = backItem.uniqueId;

			SInstOccupyEmail email;
			email.type = EINST_OUT_RANK;
			email.instId = m_instId;
			email.occupiedRoleId = backItem.roleId;
			email.occupiedUserId = backItem.userId;
			email.occupiedNick = backItem.roleNick;
			email.occupierRoleId = rank.roleId;
			email.occupierUserId = rank.userId;
			email.occupierNick = rank.roleNick;
			emailList.push_back(email);

			if (insertIt == m_rankList.begin())
			{
				email.type = EINST_OCCUPY_BY_NEW_ROLE;
				email.occupiedRoleId = insertIt->roleId;
				email.occupiedUserId = insertIt->userId;
				email.occupiedNick = insertIt->roleNick;
				email.occupierRoleId = rank.roleId;
				email.occupierUserId = rank.userId;
				email.occupierNick = rank.roleNick;
				emailList.push_back(email);
			}

			m_rankList.insert(insertIt, rank);
			m_rankList.pop_back();
		}
		else	//排行榜未满则新插入
		{
			if (insertIt == m_rankList.begin())
			{
				SInstOccupyEmail email;
				if (rank.roleId == occupyRoleId)
					email.type = EINST_OCCUPY_REFRESH_BY_SELF;
				else
					email.type = EINST_OCCUPY_BY_NEW_ROLE;

				email.instId = m_instId;
				email.occupiedRoleId = insertIt->roleId;
				email.occupiedUserId = insertIt->userId;
				email.occupiedNick = insertIt->roleNick;
				email.occupierRoleId = rank.roleId;
				email.occupierUserId = rank.userId;
				email.occupierNick = rank.roleNick;
				emailList.push_back(email);
			}

			rank.up = 1;
			m_rankList.insert(insertIt, rank);
		}
	}
}

BYTE InstRankInfoMan::InstRankSite::QueryInstRankInfo(SInstRankInfo* pRankInfo, BYTE rankInfoNum)
{
	BYTE idx = 0;
	{
		CXTAutoLock lock(m_rankListLocker);
		std::list<SInstRankTable>::iterator it = m_rankList.begin();
		for (; it != m_rankList.end(); ++it)
		{
			SInstRankTable& rankInfo = *it;
			pRankInfo[idx].roleId = rankInfo.roleId;
			pRankInfo[idx].score = rankInfo.score;
			strcpy(pRankInfo[idx].roleNick, rankInfo.roleNick);
			strcpy(pRankInfo[idx].tts, rankInfo.tts);
			pRankInfo[idx].hton();
			if (++idx >= rankInfoNum)
				break;
		}
	}
	return idx;
}

void InstRankInfoMan::InstRankSite::SyncDBInstRank()
{
	if (m_rankList.empty())
		return;

	CXTAutoLock lock(m_rankListLocker);
	std::list<SInstRankTable>::iterator it = m_rankList.begin();
	for (; it != m_rankList.end(); ++it)
	{
		SInstRankTable& rankInfo = *it;
		char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
		if (rankInfo.uniqueId == 0)	//需要插入数据库
		{
			sprintf(szSQL, "insert into rankinstance(instid, actorid, accountid, nick, score, gamemode, tts) values(%d, %u, %u, '%s', %u, %d, '%s')",
				m_instId, rankInfo.roleId, rankInfo.userId, rankInfo.roleNick, rankInfo.score, m_instGameMode, rankInfo.tts);

			GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, &rankInfo.uniqueId);
			ASSERT(rankInfo.uniqueId > 0);
			rankInfo.up = 0;
		}
		else	//更新数据库
		{
			if (!rankInfo.up)
				continue;

			sprintf(szSQL, "update rankinstance set actorid=%u, accountid=%u, nick='%s', score=%u, tts='%s' where id=%u",
				rankInfo.roleId, rankInfo.userId, rankInfo.roleNick, rankInfo.score, rankInfo.tts, rankInfo.uniqueId);

			GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
			rankInfo.up = 0;
		}
	}
}

int InstRankInfoMan::InstRankSite::GetOccupyInstIdByRoleId(UINT32 roleId, UINT16& instId)
{
	CXTAutoLock lock(m_rankListLocker);
	if (m_rankList.empty())
		return MLS_NOT_EXIST_INST;

	std::list<SInstRankTable>::iterator it = m_rankList.begin();
	if (it->roleId != roleId)
		return MLS_NOT_EXIST_INST;

	instId = m_instId;
	return MLS_OK;
}

int  InstRankInfoMan::InstRankSite::GetInstOccupyInfo(UINT32& userId, UINT32& roleId)
{
	CXTAutoLock lock(m_rankListLocker);
	if (m_rankList.empty())
		return MLS_NOT_EXIST_INST;

	std::list<SInstRankTable>::iterator it = m_rankList.begin();
	roleId = it->roleId;
	userId = it->userId;
	return MLS_OK;
}

void InstRankInfoMan::InstRankSite::DelRoleInstRank(UINT32 roleId)
{
	CXTAutoLock lock(m_rankListLocker);
	std::list<SInstRankTable>::iterator it = m_rankList.begin();
	for (; it != m_rankList.end(); ++it)
	{
		if (it->roleId == roleId)
		{
			m_rankList.erase(it);
			break;
		}
	}
}