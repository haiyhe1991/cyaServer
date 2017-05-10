#include "cyaLog.h"
#include "RankConfigFileMan.h"
#include "RankDBSClient.h"
#include "RankGWClient.h"
#include "RankHandler.h"

RankHandler* sg_rankHandler = NULL;
void InitRankHandler()
{
	ASSERT(sg_rankHandler == NULL);
	sg_rankHandler = new RankHandler();
	ASSERT(sg_rankHandler != NULL);

	sg_rankHandler->SetInterval(GetRankConfigFileMan()->GetUpdateInterval(), GetRankConfigFileMan()->GetSaveInterval());

	// RANK服务器启动时查询一次排行信息
	sg_rankHandler->OnQueryRoleInfoTimer(NULL);
}

RankHandler* GetRankHandler()
{
	return sg_rankHandler;
}

void DestroyRankHandler()
{
	RankHandler* rank = sg_rankHandler;
	sg_rankHandler = NULL;
	if (rank)
		delete rank;
}

void FormatCompetitiveSQL(SRoleRankInfo & info, char* szSQL, int & len, int j, int index, char* pszRankTime, int main_save)
{
	if (0 == j)
		len += sprintf(szSQL + len, "values(%u, '%s', %u, %u, '%s', %u, %d, %u)", (index + 1), pszRankTime, info.ext.accountid, info.base.roleid, info.base.roleNick, info.base.cp, main_save, info.base.roleType);
	else
		len += sprintf(szSQL + len, ",(%u, '%s', %u, %u, '%s', %u, %d, %u)", (index + 1), pszRankTime, info.ext.accountid, info.base.roleid, info.base.roleNick, info.base.cp, main_save, info.base.roleType);
}

RankHandler::RankHandler() : m_ThreadExit(false), m_LogThreadExit(false),
m_interval(RANK_DEFAULT_UPDATE_INTERVAL * 1000), m_save_interval(RANK_DEFAULT_UPDATE_INTERVAL * 1000)
{
	m_displaynum = GetRankConfigFileMan()->GetDisplayNum();
	if (1 > m_displaynum)
		m_displaynum = MAX_RANK_NUM;

	m_LogQueue.Init();

	m_Thread.Open(ThreadWorker, this, 0);
	m_LogThread.Open(LogThreadWorker, this, 0);
}

RankHandler::~RankHandler()
{
	m_ThreadExit = true;
	m_LogThreadExit = true;
	m_Thread.Close();
	m_LogThread.Close();
}

int RankHandler::ProcessLogic(RankGWClient* gwsSession, UINT16 cmdCode, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	int retCode = RANK_OK;
	switch (cmdCode)
	{
	case RANK_QUERY_ROLE_INFO:
		retCode = OnQueryRoleInfo(gwsSession, pHead, pData, nDatalen);
		break;
	case RANK_QUERY_ROLE_RANK:
		retCode = OnQueryRoleRankInfo(gwsSession, pHead, pData, nDatalen);
		break;
	case RANK_SEND_LOG_INFO:
	{
							   CNoLockMsgBlock *mb = m_LogQueue.CreateMsgBlock(nDatalen);
							   if (NULL != mb)
							   {
								   //LogEx(LOG_INFO, LOG_RANK, "mb ok (datalen=%d)", nDatalen - sizeof(SGSLogBase));
								   SGSLogBase *pLogBase = (SGSLogBase *)pData;
								   pLogBase->ntoh();
								   mb->copy((BYTE *)pData, nDatalen);
								   m_LogQueue.putq(mb);
							   }
							   break;
	}
	default:
		gwsSession->SendBasicResponse(pHead, RANK_UNKNOWN_CMD_CODE, cmdCode);
		break;
	}

	return retCode;
}

int RankHandler::OnQueryRoleInfo(RankGWClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerGetRankInfoReq) == nDatalen);
	if (sizeof(SLinkerGetRankInfoReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, RANK_INVALID_CMD_PACKET, RANK_QUERY_ROLE_INFO);

	SLinkerGetRankInfoReq* req = (SLinkerGetRankInfoReq*)pData;
	req->ntoh();

	switch (req->info.type)
	{
	case RANK_ACTOR_LEVEL:     // 角色等级
		return GetCacheRankInfo(gwsSession, pHead, req, m_vecLevel.size());
	case RANK_ACTOR_CP:        // 角色战斗力
		return GetCacheRankInfo(gwsSession, pHead, req, m_vecCP.size());
	case RANK_ACTOR_DUPLICATE:  // 角色副本
		return GetCacheRankInfo(gwsSession, pHead, req, m_vecInst.size());
	case RANK_ACTOR_POINT:      // 角色成就点数
		return GetCacheRankInfo(gwsSession, pHead, req, m_vecPoint.size());
	case RANK_ACTOR_COMP:       // 角色竞技模式
		return GetCacheRankInfo(gwsSession, pHead, req, m_vecComp.size());
	default:
		break;
	}
	return gwsSession->SendBasicResponse(pHead, RANK_UNKNOWN_QUERY_TYPE, RANK_QUERY_ROLE_INFO);
}

int RankHandler::GetCacheRankInfo(RankGWClient* gwsSession, SGWSProtocolHead* pHead, SLinkerGetRankInfoReq* & req, int size)
{
	CXTAutoLock lock(m_lock[req->info.type]);

	if (size <= (int)(req->info.start))
	{
		return gwsSession->SendBasicResponse(pHead, RANK_QUERY_NO_MORE_DATA, RANK_QUERY_ROLE_INFO);
	}

	int  total = size;
	int  start = req->info.start;
	int  end = (start + req->info.num) > total ? total : (start + req->info.num);
	int  nLen = GWS_COMMON_REPLY_LEN;
	char buf[MAX_BUF_LEN] = { 0 };

	SGetRankInfoResult* pBase = (SGetRankInfoResult*)(buf + GWS_COMMON_REPLY_LEN);
	pBase->type = req->info.type;
	pBase->num = end - start;
	nLen += sizeof(SGetRankInfoResult);

	int  i = 0;
	switch (req->info.type)
	{
	case RANK_ACTOR_LEVEL:     // 角色等级
	{
								   SRoleLevelAttr* pRank = (SRoleLevelAttr*)(buf + nLen);
								   for (i = start; i < end; i++)
								   {
									   pRank->level = (BYTE)(m_vecLevel[i].cp);
									   pRank->roleid = m_vecLevel[i].roleid;
									   pRank->roleType = m_vecLevel[i].roleType;
									   strncpy_traits(pRank->roleNick, m_vecLevel[i].roleNick, RANK_NICK_LEN);

									   pRank->hton();
									   pRank++;
								   }
								   nLen += (pBase->num) * sizeof(SRoleLevelAttr);
								   break;
	}

	case RANK_ACTOR_CP:        // 角色战斗力
	{
								   SRoleCPAttr* pRank = (SRoleCPAttr*)(buf + nLen);
								   for (i = start; i < end; i++)
								   {
									   pRank->cp = m_vecCP[i].cp;
									   pRank->roleid = m_vecCP[i].roleid;
									   pRank->roleType = m_vecCP[i].roleType;
									   strncpy_traits(pRank->roleNick, m_vecCP[i].roleNick, RANK_NICK_LEN);

									   pRank->hton();
									   pRank++;
								   }
								   nLen += (pBase->num) * sizeof(SRoleCPAttr);
								   break;
	}

	case RANK_ACTOR_DUPLICATE:  // 角色副本
	{
									SRoleInstAttr* pRank = (SRoleInstAttr*)(buf + nLen);
									for (i = start; i < end; i++)
									{
										pRank->instId = (UINT16)(m_vecInst[i].cp);
										pRank->roleid = m_vecInst[i].roleid;
										pRank->roleType = m_vecInst[i].roleType;
										strncpy_traits(pRank->roleNick, m_vecInst[i].roleNick, RANK_NICK_LEN);

										pRank->hton();
										pRank++;
									}
									nLen += (pBase->num) * sizeof(SRoleInstAttr);
									break;
	}

	case RANK_ACTOR_POINT:      // 角色成就点数
	{
									SRolePointAttr* pRank = (SRolePointAttr*)(buf + nLen);
									for (i = start; i < end; i++)
									{
										pRank->point = m_vecPoint[i].cp;
										pRank->roleid = m_vecPoint[i].roleid;
										pRank->roleType = m_vecPoint[i].roleType;
										strncpy_traits(pRank->roleNick, m_vecPoint[i].roleNick, RANK_NICK_LEN);

										pRank->hton();
										pRank++;
									}
									nLen += (pBase->num) * sizeof(SRolePointAttr);
									break;
	}

	case RANK_ACTOR_COMP:      // 角色竞技模式
	{
								   SRoleCompAttr* pRank = (SRoleCompAttr*)(buf + nLen);
								   for (i = start; i < end; i++)
								   {
									   pRank->competitive = m_vecComp[i].base.cp;
									   pRank->roleid = m_vecComp[i].base.roleid;
									   pRank->roleType = m_vecComp[i].base.roleType;
									   strncpy_traits(pRank->roleNick, m_vecComp[i].base.roleNick, RANK_NICK_LEN);

									   pRank->hton();
									   pRank++;
								   }
								   nLen += (pBase->num) * sizeof(SRoleCompAttr);
								   break;
	}

	default:
		return gwsSession->SendBasicResponse(pHead, RANK_UNKNOWN_QUERY_TYPE, RANK_QUERY_ROLE_INFO);
	}

	return gwsSession->SendResponse(pHead, RANK_OK, RANK_QUERY_ROLE_INFO, buf, nLen);
}

int RankHandler::OnQueryRoleRankInfo(RankGWClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerGetRoleRankInfoReq) == nDatalen);
	if (sizeof(SLinkerGetRoleRankInfoReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, RANK_INVALID_CMD_PACKET, RANK_QUERY_ROLE_RANK);

	SLinkerGetRoleRankInfoReq* req = (SLinkerGetRoleRankInfoReq*)pData;
	req->ntoh();

	UINT32 roleid = req->info.roleId/*req->roleId*/;
	INT    i = 0;
	INT    total = 0;
	char buf[MAX_BUF_LEN] = { 0 };
	int  nLen = GWS_COMMON_REPLY_LEN;

	BYTE* pQueryType = (BYTE*)(buf + GWS_COMMON_REPLY_LEN);
	*pQueryType = req->info.querytype;
	nLen += 1;

	switch (req->info.querytype)
	{
	case RANK_ACTOR_LEVEL:     // 角色等级
	{
								   CXTAutoLock lock(m_lock[req->info.querytype]);

								   total = m_vecLevel.size();
								   SRoleLevelRank* pRank = (SRoleLevelRank*)(buf + nLen);
								   for (i = 0; i < total; i++)
								   {
									   if (m_vecLevel[i].roleid == roleid)
									   {
										   pRank->level = (BYTE)(m_vecLevel[i].cp);
										   pRank->level_rank = i + 1;
										   break;
									   }
								   }

								   pRank->hton();
								   nLen += sizeof(SRoleLevelRank);
								   break;
	}

	case RANK_ACTOR_CP:        // 角色战斗力
	{
								   CXTAutoLock lock(m_lock[req->info.querytype]);

								   total = m_vecCP.size();
								   SRoleCPRank* pRank = (SRoleCPRank*)(buf + nLen);
								   for (i = 0; i < total; i++)
								   {
									   if (m_vecCP[i].roleid == roleid)
									   {
										   pRank->cp = m_vecCP[i].cp;
										   pRank->cp_rank = i + 1;
										   break;
									   }
								   }

								   pRank->hton();
								   nLen += sizeof(SRoleCPRank);
								   break;
	}

	case RANK_ACTOR_DUPLICATE:  // 角色副本
	{
									CXTAutoLock lock(m_lock[req->info.querytype]);

									total = m_vecInst.size();
									SRoleInstRank* pRank = (SRoleInstRank*)(buf + nLen);
									for (i = 0; i < total; i++)
									{
										if (m_vecInst[i].roleid == roleid)
										{
											pRank->instId = (UINT16)(m_vecInst[i].cp);
											pRank->inst_rank = i + 1;
											break;
										}
									}

									pRank->hton();
									nLen += sizeof(SRoleInstRank);
									break;
	}

	case RANK_ACTOR_POINT:      // 角色成就点数
	{
									CXTAutoLock lock(m_lock[req->info.querytype]);

									total = m_vecPoint.size();
									SRolePointRank* pRank = (SRolePointRank*)(buf + nLen);
									for (i = 0; i < total; i++)
									{
										if (m_vecPoint[i].roleid == roleid)
										{
											pRank->point = m_vecPoint[i].cp;
											pRank->point_rank = i + 1;
											break;
										}
									}

									pRank->hton();
									nLen += sizeof(SRolePointRank);
									break;
	}

	case RANK_ACTOR_COMP:       // 角色竞技模式
	{
									CXTAutoLock lock(m_lock[req->info.querytype]);

									total = m_vecComp.size();
									SRoleCompRank* pRank = (SRoleCompRank*)(buf + nLen);
									for (i = 0; i < total; i++)
									{
										if (m_vecComp[i].base.roleid == roleid)
										{
											pRank->competitive = m_vecComp[i].base.cp;
											pRank->comp_rank = i + 1;
											break;
										}
									}

									pRank->hton();
									nLen += sizeof(SRoleCompRank);
									break;
	}

	default:
		return gwsSession->SendBasicResponse(pHead, RANK_UNKNOWN_QUERY_TYPE, RANK_QUERY_ROLE_RANK);
	}

	return gwsSession->SendResponse(pHead, RANK_OK, RANK_QUERY_ROLE_RANK, buf, nLen);
}

int RankHandler::QueryRankInfo(ERankType type, UINT32 querynum)
{
	char szSQL[1024] = { 0 };

	switch (type)
	{
	case RANK_ACTOR_LEVEL:     // 角色等级
		sprintf(szSQL, "select id, actortype, nick, level from actor order by level desc limit %u", querynum);
		return QueryDBRankInfo(type, szSQL, querynum, m_vecLevel);
	case RANK_ACTOR_CP:        // 角色战斗力
		sprintf(szSQL, "select id, actortype, nick, cp from actor order by cp desc limit %u", querynum);
		return QueryDBRankInfo(type, szSQL, querynum, m_vecCP);
	case RANK_ACTOR_DUPLICATE:  // 角色副本
		sprintf(szSQL, "select A.actorid, B.actortype, B.nick, A.instanceid from (select max(instanceid) as instanceid, actorid from instancehistory group by actorid order by instanceid desc limit %u) A left join actor as B on A.actorid = B.id", querynum);
		return QueryDBRankInfo(type, szSQL, querynum, m_vecInst);
	case RANK_ACTOR_POINT:      // 角色成就点数
		sprintf(szSQL, "select A.actorid, B.actortype, B.nick, A.point from (select max(point) as point, actorid from achievement group by actorid order by point desc limit %u) A left join actor as B on A.actorid = B.id", querynum);
		return QueryDBRankInfo(type, szSQL, querynum, m_vecPoint);
	case RANK_ACTOR_COMP:       // 角色竞技模式
		sprintf(szSQL, "select A.actorid, B.actortype, B.nick, B.accountid, A.win from (select max(win) as win, actorid from competitive group by actorid order by win desc limit %u) A left join actor as B on A.actorid = B.id", querynum);
		return QueryDBRankInfo(type, szSQL, querynum, (void *)&m_vecComp);
	default:
		return RANK_UNKNOWN_QUERY_TYPE;
	}
}

int RankHandler::QueryDBRankInfo(ERankType type, const char* pszSQL, UINT32 querynum, std::vector<SRoleCPAttr>& vector)
{
	DBS_ROW row = NULL;
	DBS_RESULT result = NULL;

	int retCode = GetRankDBSClient()->Query(GetRankConfigFileMan()->GetDBName(), pszSQL, result);
	if (retCode != DBS_OK)
		return retCode;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols < 4)
	{
		DBS_FreeResult(result);
		return DBS_ERR_FIELD;
	}

	CXTAutoLock lock(m_lock[type]);

	// 先清除原有记录
	vector.clear();

	UINT32 i = 0;
	SRoleCPAttr attr;
	while ((row = DBS_FetchRow(result)) != NULL)
	{
		memset(&attr, 0, sizeof(SRoleCPAttr));
		attr.roleid = (UINT32)_atoi64(row[0]);
		attr.roleType = (UINT16)atoi(row[1]);
		strncpy_traits(attr.roleNick, row[2], RANK_NICK_LEN);
		attr.cp = (UINT32)_atoi64(row[3]);

		vector.push_back(attr);
		++i;
		if (i >= querynum)
			break;
	}

	DBS_FreeResult(result);

	// 增加临时数据
	{
		UINT32 size = 0;
		INT total = vector.size();
		if (total >= m_displaynum)
		{
			return RANK_OK;
		}
		// 获取当前的最小排行值
		if (0 < total)
		{
			size = vector[total - 1].cp;
		}
		total = m_displaynum - total;

		INT j = 0;
		INT flag = 0;
		SRoleCPAttr attr;

		srand((unsigned)time(NULL)); // 种子
		for (j = 0; j < total; j++)
		{
			memset(&attr, 0, sizeof(SRoleCPAttr));
			flag = rand();

			attr.roleid = 0;//(UINT32)flag;
			attr.roleType = (flag % 3) + 1;  // 职业值没有最大定义, 客户端自然递增
			sprintf(attr.roleNick, "rose%d", j);
			attr.cp = size;

			vector.push_back(attr);
		}
	}

	return RANK_OK;
}

// 可用回调函数改写
int RankHandler::QueryDBRankInfo(ERankType type, const char* pszSQL, UINT32 querynum, void* pvector)
{
	DBS_ROW row = NULL;
	DBS_RESULT result = NULL;

	int retCode = GetRankDBSClient()->Query(GetRankConfigFileMan()->GetDBName(), pszSQL, result);
	if (retCode != DBS_OK)
		return retCode;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols < 5)
	{
		DBS_FreeResult(result);
		return DBS_ERR_FIELD;
	}

	CXTAutoLock lock(m_lock[type]);

	// 先清除原有记录
	if (RANK_ACTOR_COMP == type)
	{
		std::vector<SRoleRankInfo> *pvecComp = (std::vector<SRoleRankInfo> *)pvector;
		pvecComp->clear();

		UINT32 i = 0;
		SRoleRankInfo attr;

		memset(&attr, 0, sizeof(SRoleRankInfo));
		while ((row = DBS_FetchRow(result)) != NULL)
		{
			memset(&attr, 0, sizeof(SRoleRankInfo));
			attr.base.roleid = (UINT32)_atoi64(row[0]);
			attr.base.roleType = (UINT16)atoi(row[1]);
			strncpy_traits(attr.base.roleNick, row[2], RANK_NICK_LEN);
			attr.ext.accountid = (UINT32)_atoi64(row[3]);
			attr.base.cp = (UINT32)_atoi64(row[4]);

			pvecComp->push_back(attr);
			++i;
			if (i >= querynum)
				break;
		}

		DBS_FreeResult(result);

		// 增加临时数据
		{
			UINT32 size = 0;
			INT total = pvecComp->size();
			if (total >= m_displaynum)
			{
				return RANK_OK;
			}
			// 获取当前的最小排行值
			if (0 < total)
			{
				size = attr.base.cp;
				//size = pvecComp->at(total-1).base.cp;
			}
			total = m_displaynum - total;

			INT j = 0;
			INT flag = 0;
			SRoleRankInfo attr;

			srand((unsigned)time(NULL)); // 种子
			for (j = 0; j < total; j++)
			{
				memset(&attr, 0, sizeof(SRoleRankInfo));
				flag = rand();

				attr.base.roleid = 0;//(UINT32)flag;
				attr.base.roleType = (flag % 3) + 1;  // 职业值没有最大定义, 客户端自然递增
				sprintf(attr.base.roleNick, "rose%d", j);
				attr.base.cp = size;
				attr.ext.accountid = 0;

				pvecComp->push_back(attr);
			}
		}
	}

	return RANK_OK;
}

void RankHandler::SaveRankInfo(ERankType ranktype, char* pszRankTime, int main_save)
{
	char szSQL[128] = { 0 };
	switch (ranktype)
	{
	case RANK_ACTOR_LEVEL:      // 角色等级
		sprintf(szSQL, "insert into ranklevel(rank, ranktts, actorid, nick, level, msign, actortype) ");
		return InsertRankInfoToDB(m_vecLevel, szSQL, pszRankTime, main_save);
	case RANK_ACTOR_CP:         // 角色战斗力
		sprintf(szSQL, "insert into rankcp(rank, ranktts, actorid, nick, cp, msign, actortype) ");
		return InsertRankInfoToDB(m_vecCP, szSQL, pszRankTime, main_save);
	case RANK_ACTOR_DUPLICATE:  // 角色副本
		sprintf(szSQL, "insert into rankinst(rank, ranktts, actorid, nick, instid, msign, actortype) ");
		return InsertRankInfoToDB(m_vecInst, szSQL, pszRankTime, main_save);
	case RANK_ACTOR_POINT:      // 角色成就点数
		sprintf(szSQL, "insert into rankpoint(rank, ranktts, actorid, nick, point, msign, actortype) ");
		return InsertRankInfoToDB(m_vecPoint, szSQL, pszRankTime, main_save);
	case RANK_ACTOR_COMP:       // 角色竞技模式
		sprintf(szSQL, "insert into rankcomp(rank, ranktts, accountid, actorid, nick, competitive, msign, actortype) ");
		return InsertRankInfoToDB(m_vecComp, FormatCompetitiveSQL, szSQL, pszRankTime, main_save);
	default:
		break;
	}
	return;;
}

void RankHandler::InsertRankInfoToDB(std::vector<SRoleCPAttr>& vector, char* header, char* pszRankTime, int main_save)
{
	char szSQL[2048] = { 0 };
	int i = 0;
	int j = 0;
	int total = vector.size();
	int multi = total / 10;
	int range = 10;
	int len = 0;
	int index = 0;
	int base = 0;

	if (1 > total)
		return;

	for (i = 0; i <= multi; i++)
	{
		base = i * 10;
		if (i == multi)
			range = total % 10;
		if (0 == range)
			break;

		len = sprintf(szSQL, "%s", header);
		for (j = 0; j < range; j++)
		{
			index = base + j;
			if (0 == vector[index].roleid)
			{
				if (0 != j)
				{
					GetRankDBSClient()->Insert(GetRankConfigFileMan()->GetDBName(), szSQL, NULL);
				}
				return;
			}

			if (0 == j)
				len += sprintf(szSQL + len, "values(%u, '%s', %u, '%s', %u, %d, %u)", (index + 1), pszRankTime, vector[index].roleid, vector[index].roleNick, vector[index].cp, main_save, vector[index].roleType);
			else
				len += sprintf(szSQL + len, ",(%u, '%s', %u, '%s', %u, %d, %u)", (index + 1), pszRankTime, vector[index].roleid, vector[index].roleNick, vector[index].cp, main_save, vector[index].roleType);
		}
		GetRankDBSClient()->Insert(GetRankConfigFileMan()->GetDBName(), szSQL, NULL);
	}

	return;
}

void RankHandler::InsertRankInfoToDB(std::vector<SRoleRankInfo>& vector, pFormat pf, char* header, char* pszRankTime, int main_save)
{
	char szSQL[2048] = { 0 };
	int i = 0;
	int j = 0;
	int total = vector.size();
	int multi = total / 10;
	int range = 10;
	int len = 0;
	int index = 0;
	int base = 0;

	if (1 > total)
		return;

	for (i = 0; i <= multi; i++)
	{
		base = i * 10;
		if (i == multi)
			range = total % 10;
		if (0 == range)
			break;

		len = sprintf(szSQL, "%s", header);
		for (j = 0; j < range; j++)
		{
			index = base + j;
			if (0 == vector[index].base.roleid)
			{
				if (0 != j)
				{
					GetRankDBSClient()->Insert(GetRankConfigFileMan()->GetDBName(), szSQL, NULL);
				}
				return;
			}

			pf(vector[index], szSQL, len, j, index, pszRankTime, main_save);
		}
		GetRankDBSClient()->Insert(GetRankConfigFileMan()->GetDBName(), szSQL, NULL);
	}

	return;
}

void RankHandler::OnQueryRoleInfoTimer(char* pszRankTime, int main_save)
{
	// 避免长时间连接数据库
	if (ConnectRankDBSClient())
	{
		QueryRankInfo(RANK_ACTOR_LEVEL, m_displaynum);
		QueryRankInfo(RANK_ACTOR_CP, m_displaynum);
		QueryRankInfo(RANK_ACTOR_DUPLICATE, m_displaynum);
		QueryRankInfo(RANK_ACTOR_POINT, m_displaynum);
		QueryRankInfo(RANK_ACTOR_COMP, m_displaynum);

		if (NULL != pszRankTime)
		{
			SaveRankInfo(RANK_ACTOR_LEVEL, pszRankTime, main_save);
			SaveRankInfo(RANK_ACTOR_CP, pszRankTime, main_save);;
			SaveRankInfo(RANK_ACTOR_DUPLICATE, pszRankTime, main_save);
			SaveRankInfo(RANK_ACTOR_POINT, pszRankTime, main_save);
			SaveRankInfo(RANK_ACTOR_COMP, pszRankTime, main_save);
		}

		DisConnectRankDBSClient();
	}
	return;
}

int RankHandler::ThreadWorker(void* pParam)
{
	RankHandler* pThis = (RankHandler*)pParam;
	ASSERT(pThis != NULL);
	return pThis->OnThreadWorker();
}

int RankHandler::LogThreadWorker(void* pParam)
{
	RankHandler* pThis = (RankHandler*)pParam;
	ASSERT(pThis != NULL);
	return pThis->OnLogThreadWorker();
}

int RankHandler::OnThreadWorker()
{
	BOOL   bQueryDB = false;
	LTMSEL oneday = 24 * 60 * 60 * 1000;
	LTMSEL tz = GetTimeZoneMsel();  // 28800000 = 8 hour

	LTMSEL curr = GetMsel();
	// 第二天0点的时间
	LTMSEL offset = (oneday - tz - curr%oneday);
	LTMSEL nextday = curr + offset;
	LTMSEL nextsave = ((curr + m_save_interval - 1) / m_save_interval) * m_save_interval;
	LTMSEL next = ((curr + m_interval - 1) / m_interval) * m_interval;

	CHAR  szLog[512] = { 0 };
	TCHAR szCurrBuf[32] = { 0 };
	TCHAR szNextBuf[32] = { 0 };

	MselToStr(curr, szCurrBuf, true);
	MselToStr(next, szNextBuf, true);
#if defined(__linux__)
	sprintf(szLog, "CurrTime[%s, %lld], NextQueryTime[%s, %lld], QueryInterval[%d s]", szCurrBuf, curr, szNextBuf, next, m_interval / 1000);
#else
	sprintf(szLog, "CurrTime[%s, %I64d], NextQueryTime[%s, %I64d], QueryInterval[%d s]", szCurrBuf, curr, szNextBuf, next, m_interval / 1000);
#endif
	LogDebug(("排行服务器启动时间, %s", szLog));

	MselToStr(nextsave, szCurrBuf, true);
	MselToStr(nextday, szNextBuf, true);
	LogDebug(("数据库第一次记录排行信息时间[%s], Midnight[%s], SaveInterval[%d s]", szCurrBuf, szNextBuf, m_save_interval / 1000));

	while (!m_ThreadExit)
	{
		curr = GetMsel();
		if (next <= curr)
		{
			GetLocalStrTime(szNextBuf);
			LogDebug(("排行服务器当前查询时间, %s", szNextBuf));
			next += m_interval;

			// 零点推送主排行数据
			if (nextday <= curr)
			{
				MselToStr(nextday, szNextBuf, true);
				nextday += oneday;
				if (nextsave <= curr)
					nextsave += m_save_interval;

				bQueryDB = true;
				OnQueryRoleInfoTimer(szNextBuf, 1);
			}

			if (!bQueryDB)
			{
				// 根据配置间隔推送排行数据
				if (nextsave <= curr)
				{
					MselToStr(nextsave, szNextBuf, true);
					nextsave += m_save_interval;
					OnQueryRoleInfoTimer(szNextBuf);
				}
				else
				{
					OnQueryRoleInfoTimer(NULL);
				}
			}
			bQueryDB = false;
		}
		else
		{
			Sleep(2000);
		}
	}
	return 0;
}

int RankHandler::OnLogThreadWorker()
{
	BOOL exit = false;
	CNoLockMsgBlock *mb = NULL;

	while (!exit)
	{
		m_LogQueue.getq(mb);
		if (NULL == mb)
		{
			if (m_LogThreadExit)
			{
				exit = true;
				m_LogQueue.getlast(mb);
				if (NULL == mb)
					return 0;
			}
			else
			{
				Sleep(100);
				continue;
			}
		}

		//BYTE  *pData = mb->getdata();
		//TCHAR *wch = (TCHAR *)(pData + sizeof(SGSLogBase));
		//SGSLogBase *pLogBase = (SGSLogBase *)pData;
		//这儿暂时先不管。 LogAsync(pLogBase->level, pLogBase->module, pLogBase->time, "%s", wch);
		LogDebug(("先不管"));
		mb->release();
		mb = NULL;
	}

	return 0;
}

// 传入秒数,需要转换为毫秒数
void RankHandler::SetInterval(int updateinterval, int saveinterval)
{
	if (0 < updateinterval)
	{
		int oneday = 24 * 60 * 60;
		if (0 == (oneday%updateinterval))
		{
			m_interval = updateinterval * 1000;
		}
	}

	if (0 < saveinterval)
		m_save_interval = saveinterval * 1000;

	if (m_save_interval < m_interval)
		m_save_interval = m_interval;

	// save_interval必须是update_interval的倍数
	if (0 != (m_save_interval % m_interval))
		m_save_interval = (m_save_interval / m_interval + 1) * m_interval;

	LogDebug(("Input&Save time: updateinterval[%d s, %d s], SaveInterval[%d s, %d s]", updateinterval, m_interval / 1000, saveinterval, m_save_interval / 1000));
}
