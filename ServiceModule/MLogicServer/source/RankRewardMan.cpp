#include "RankRewardMan.h"


static RankRewardMan* sg_pRankRewardMan = NULL;
void InitRankRewardMan()
{
	ASSERT(sg_pRankRewardMan == NULL);
	sg_pRankRewardMan = new RankRewardMan();
	ASSERT(sg_pRankRewardMan != NULL);
	sg_pRankRewardMan->RestRefreshRanks();
}

RankRewardMan* GetRankRewardMan()
{
	return sg_pRankRewardMan;
}

void DestroyRankRewardMan()
{
	RankRewardMan* pActitvityMan = sg_pRankRewardMan;
	sg_pRankRewardMan = NULL;
	if (pActitvityMan)
		delete pActitvityMan;
}

int RankRewardMan::RefreshRanks()
{
	m_lastRefTime = GetMsel();
	//更新副本挑战次数数据，并清0
	int ret = UpdateRankInsnum();
	if (ret != MLS_OK)
		return ret;

	ret = GetLvRanks();
	if (ret != MLS_OK)
		return ret;
	//if(!GetCpRanks())
	//	return FALSE;
	ret = GetInstnumRanks();
	if (ret != MLS_OK)
		return ret;
	return ret;
}

int RankRewardMan::RestRefreshRanks()
{
	int ret = GetLvRanks();
	if (ret != MLS_OK)
		return ret;
	//if(!GetCpRanks())
	//	return FALSE;
	ret = GetInstnumRanks();
	if (ret != MLS_OK)
		return ret;
	return ret;
}
int RankRewardMan::GetLvRanks()
{
	const UINT8 TYPE_POS = __Level_Rank_Type - 1;

	DBS_RESULT result = NULL;
	char szSQL[MAX_SQL_BUF_LEN] = { 0 };

	int length = GetGameCfgFileMan()->GetRewardPlayNum(__Level_Rank_Type);
	sprintf(szSQL, "SELECT A.id,A.rank,A.actorid,A.level,A.award,A.actorType,A.nick"
		" FROM ranklevel as A WHERE msign=1 and ranktts>=date_format(NOW(),'%%Y-%%m-%%d')"
		" limit %d;", length);

	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols <= 0)
	{
		DBS_FreeResult(result);
		return MLS_OK/*MLS_ROLE_NOT_EXIST*/;
	}
	DBS_ROW row = NULL;
	DBS_ROWLENGTH rowValLen = NULL;

	CXTAutoLock lock(m_lvRankLK);
	m_vRanks[TYPE_POS].clear();
	while ((row = DBS_FetchRow(result, &rowValLen)) != NULL)
	{
		SRanks sranks;
		memset(&sranks, 0, sizeof(SRanks));

		sranks.dataID = (UINT32)_atoi64(row[0]);
		sranks.rankID = (UINT16)atoi(row[1]);
		sranks.roleID = (UINT32)_atoi64(row[2]);
		sranks.iValue = (UINT32)_atoi64(row[3]);
		sranks.bGet = atoi(row[4]) == 1;
		sranks.actorType = (UINT16)atoi(row[5]);
		strcpy(sranks.nick, row[6]);
		m_vRanks[TYPE_POS].push_back(sranks);
	}
	DBS_FreeResult(result);
	return MLS_OK;

}
/*
int RankRewardMan::GetCpRanks()
{

DBS_RESULT result = NULL;
char szSQL[MAX_SQL_BUF_LEN] = { 0 };
sprintf(szSQL, "SELECT A.id,A.rank,A.actorid,A.cp,A.award"
"FROM rankcp as A WHERE msign=1 and ranktts>date_format(NOW(),'%Y-%m-%d') ORDER BY rank limit 10;");

int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
if (retCode != DBS_OK)
return retCode;

UINT32 nRows = DBS_NumRows(result);
UINT16 nCols = DBS_NumFields(result);
if(nRows <= 0 || nCols < 5)
{
DBS_FreeResult(result);
return MLS_OK;
}
DBS_ROW row = NULL;
DBS_ROWLENGTH rowValLen = NULL;

CXTAutoLock lock(m_vLocker[1]);
m_vRanks[0].clear();
while ((row = DBS_FetchRow(result, &rowValLen)) != NULL)
{
SRanks sranks;
memset(&sranks,0,sizeof(SRanks));

sranks.dataID = _atoi64(row[0]);
sranks.rankID = _atoi64(row[1]);
sranks.roleID = _atoi64(row[2]);
sranks.iValue  = _atoi64(row[3]);
sranks.bGet = _atoi64(row[4])==1;
m_vRanks[0].push_back(sranks);
}
DBS_FreeResult(result);
return MLS_OK;

}
*/

int RankRewardMan::UpdateRankInsnum()
{
	char tts[33] = { 0 };
	GetLocalStrTime(tts);

	DBS_RESULT result = NULL;
	char szSQL[MAX_SQL_BUF_LEN] = { 0 };

	sprintf(szSQL, "select A.actorid,sum(A.daynum),B.nick,B.actorType from instancehistory AS A"
		" join actor as B on A.actorid=B.id where A.daynum>0 group by actorid order by sum(daynum) desc limit %d;", MAX_INS_RANK_NUM);
	//0 actorid  1 num  2 nick  3 actortype
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows < 0 || nCols < 4)
	{
		DBS_FreeResult(result);
		return MLS_OK;
	}
	DBS_ROW row = NULL;
	DBS_ROWLENGTH rowValLen = NULL;


	int i = 1;
	std::vector<SRanks> vranks;
	while ((row = DBS_FetchRow(result, &rowValLen)) != NULL)
	{
		//0 actorid  1 num  2 nick  3 actortype
		SRanks srank;
		memset(&srank, 0, sizeof(SRanks));
		srank.rankID = i++;
		srank.roleID = (UINT32)_atoi64(row[0]);
		srank.iValue = (UINT32)_atoi64(row[1]);
		strcpy(srank.nick, row[2]);
		srank.actorType = (BYTE)atoi(row[3]);
		vranks.push_back(srank);
	}
	DBS_FreeResult(result);


	memset(szSQL, 0, sizeof(szSQL));
	//std::vector<SRanks>::iterator it = vranks.begin();
	/*	if(it==vranks.end())
	{
	ASSERT(0);
	return -1;
	}*/
	sprintf(szSQL, "insert into rankinstnum(rank, ranktts, actorid, nick, instnum, award, msign,actortype)"
		"values ('%d', '%s', '%d', '%s', %d, 0,1,%d)",
		vranks[0].rankID, tts, vranks[0].roleID, vranks[0].nick, vranks[0].iValue, (UINT16)vranks[0].actorType);

	char cc[128] = { 0 };
	int n = 0;
	for (int i = 1; i<vranks.size(); i++)
	{
		n = strlen(szSQL);
		sprintf(cc, ",('%d', '%s', '%d', '%s', %d, 0,1,%d)"
			, vranks[i].rankID, tts, vranks[i].roleID, vranks[i].nick, vranks[i].iValue, (UINT16)vranks[i].actorType);
		memcpy(szSQL + n, cc, strlen(cc));
	}
	UINT32 newId = 0;
	int r = GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, &newId);
	if (r != LCS_OK)
		return r;

	//清空挑战次数数据
	memset(szSQL, 0, sizeof(szSQL));
	sprintf(szSQL, "update instancehistory set daynum = 0");
	r = GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
	if (r != LCS_OK)
		return r;
	return LCS_OK;
}

int RankRewardMan::GetInstnumRanks()
{
	const UINT8 TYPE_POS = __InstanceNum_Rank_Type - 1;

	DBS_RESULT result = NULL;
	char szSQL[MAX_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "SELECT A.rank,A.actorid,A.instnum,A.award,A.nick,A.id,A.actorType"
		" FROM rankinstnum as A WHERE msign=1 and ranktts>=date_format(NOW(),'%%Y-%%m-%%d')"
		" limit %d;", MAX_INS_RANK_NUM);

	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols < 6)
	{
		DBS_FreeResult(result);
		return MLS_OK;
	}
	DBS_ROW row = NULL;
	DBS_ROWLENGTH rowValLen = NULL;

	CXTAutoLock lock(m_lvInsnumLK);
	m_vRanks[TYPE_POS].clear();
	while ((row = DBS_FetchRow(result, &rowValLen)) != NULL)
	{
		/*A.rank,A.actorid,A.instnum,A.award,A.nick,A.id*/
		SRanks sranks;
		memset(&sranks, 0, sizeof(SRanks));
		sranks.rankID = (UINT16)_atoi64(row[0]);
		sranks.roleID = (UINT32)_atoi64(row[1]);
		sranks.iValue = (UINT32)_atoi64(row[2]);
		sranks.bGet = atoi(row[3]) == 1;
		strcpy(sranks.nick, row[4]);
		sranks.dataID = (UINT32)_atoi64(row[5]);
		sranks.actorType = (BYTE)atoi(row[6]);
		m_vRanks[TYPE_POS].push_back(sranks);
#if defined(WIN32)
		if (nRows<m_vRanks[TYPE_POS].size())
		{
			printf("sql:%s, rownum=%d,vranksize=%d\n", szSQL, nRows, m_vRanks[TYPE_POS].size());
			ASSERT(0);
			for (int i = 0; i<m_vRanks[TYPE_POS].size(); i++)
			{
				printf("did=%d,rankid=%d\n", m_vRanks[TYPE_POS][i].dataID, m_vRanks[TYPE_POS][i].rankID);
			}
		}
#endif
	}
	DBS_FreeResult(result);
	return MLS_OK;
}


int RankRewardMan::RefreshOnlineOnZero()
{
	char szSQL[MAX_SQL_BUF_LEN] = { 0 };
	memset(szSQL, 0, sizeof(szSQL));
	sprintf(szSQL, "update online_reward set online=0,info=Null");
	return GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
}
/*
int RankRewardMan::GetPoRanks()
{
DBS_RESULT result = NULL;
char szSQL[MAX_SQL_BUF_LEN] = { 0 };
sprintf(szSQL, "SELECT A.id,A.rank,A.actorid,A.point,A.award"
"FROM rankpoint as A WHERE msign=1 and ranktts>date_format(NOW(),'%Y-%m-%d') ORDER BY rank limit 10;");

int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
if (retCode != DBS_OK)
return retCode;

UINT32 nRows = DBS_NumRows(result);
UINT16 nCols = DBS_NumFields(result);
if(nRows <= 0 || nCols < 5)
{
DBS_FreeResult(result);
return MLS_OK;
}
DBS_ROW row = NULL;
DBS_ROWLENGTH rowValLen = NULL;

CXTAutoLock lock(m_vLocker[3]);
m_vRanks[0].clear();
while ((row = DBS_FetchRow(result, &rowValLen)) != NULL)
{
SRanks sranks;
memset(&sranks,0,sizeof(SRanks));

sranks.dataID = _atoi64(row[0]);
sranks.rankID = _atoi64(row[1]);
sranks.roleID = _atoi64(row[2]);
sranks.iValue  = _atoi64(row[3]);
sranks.bGet = _atoi64(row[4])==1;
m_vRanks[0].push_back(sranks);
}
DBS_FreeResult(result);
return MLS_OK;
}
*/










// 查询LV排名 hxw 20150929 				 
int RankRewardMan::QueryLvRank(UINT32 roleId, SQueryRankRewardGet* ranks)
{

	CXTAutoLock lock(m_lvRankLK);
	const UINT8 TYPE_POS = __Level_Rank_Type - 1;
	int len = m_vRanks[TYPE_POS].size();//
	for (int i = 0; i<len; i++)
	{
		if (roleId == m_vRanks[TYPE_POS][i].roleID)
		{
			ranks->bGet = m_vRanks[TYPE_POS][i].bGet;
			ranks->rankid = m_vRanks[TYPE_POS][i].rankID;
		}
	}
	return MLS_OK;
}



// 查询INUM排名 hxw 20150929 
int RankRewardMan::QueryInsnumRank(UINT32 roleId, SQueryRankRewardGet* ranks)
{
	const UINT8 TYPE_POS = __InstanceNum_Rank_Type - 1;
	CXTAutoLock lock(m_lvInsnumLK);
	int len = m_vRanks[TYPE_POS].size();//
	for (int i = 0; i<len; i++)
	{
		if (roleId == m_vRanks[TYPE_POS][i].roleID)
		{
			ranks->bGet = m_vRanks[TYPE_POS][i].bGet;
			ranks->rankid = m_vRanks[TYPE_POS][i].rankID;
		}
	}
	return MLS_OK;
}
//add by hxw 20151028 查询等级排行榜信息
int RankRewardMan::QueryLvRankInfo(SQueryInsnumRanksInfo* ranks)
{
	CXTAutoLock lock(m_lvRankLK);
	const UINT8 TYPE_POS = __Level_Rank_Type - 1;
	int len = m_vRanks[TYPE_POS].size();
	if (len <= 0)
		return MLS_QLV_INFO_RANK_REWARD_FAIL;
	//ASSERT(MAX_INS_RANK_NUM==len);
	for (int i = 0; i<len; i++)
	{
		strcpy(ranks->m_vInsRanks[i].nick, m_vRanks[TYPE_POS][i].nick);
		ranks->m_vInsRanks[i].num = m_vRanks[TYPE_POS][i].iValue;
		ranks->m_vInsRanks[i].occuId = m_vRanks[TYPE_POS][i].actorType;
		ranks->m_vInsRanks[i].rankid = m_vRanks[TYPE_POS][i].rankID;
	}
	return MLS_OK;
}
//end

//查询副本推图排行的排行榜
int RankRewardMan::QueryInsnumRankInfo(SQueryInsnumRanksInfo* ranks)
{
	CXTAutoLock lock(m_lvInsnumLK);
	const UINT8 TYPE_POS = __InstanceNum_Rank_Type - 1;
	int len = m_vRanks[TYPE_POS].size();
	if (len <= 0)
		return MLS_QINUM_INFO_RANK_REWARD_FAIL;
	//ASSERT(MAX_INS_RANK_NUM==len);
	for (int i = 0; i<len; i++)
	{
		strcpy(ranks->m_vInsRanks[i].nick, m_vRanks[TYPE_POS][i].nick);
		ranks->m_vInsRanks[i].num = m_vRanks[TYPE_POS][i].iValue;
		ranks->m_vInsRanks[i].occuId = m_vRanks[TYPE_POS][i].actorType;
		ranks->m_vInsRanks[i].rankid = m_vRanks[TYPE_POS][i].rankID;
	}
	return MLS_OK;
}




//获取强者奖励领取信息
int RankRewardMan::QueryStrongerInfoForDB(UINT32 roleID, SRoleInfos* role)
{
	DBS_RESULT result = NULL;
	char szSQL[MAX_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select cprewardid from cp_reward where actorid=%u", roleID);

	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols <= 0)
	{
		DBS_FreeResult(result);
		return MLS_OK;
	}
	DBS_ROW row = NULL;
	DBS_ROWLENGTH rowValLen = NULL;

	//add by hxw 20151106 测试代码
#if defined(WIN32)
	char getinfo[10] = { 0 };
	std::string str;
#endif

	while ((row = DBS_FetchRow(result, &rowValLen)) != NULL)
	{
		//GetRolesInfoMan()->InputStrongerReward(userId,roleId,_atoi64(row[0]));
		role->InputStrongerReward(atoi(row[0]));

#if defined(WIN32)
		sprintf(getinfo, " %d ", atoi(row[0]));
		str += getinfo;
#endif

	}

#if defined(WIN32)
	printf("Get Stronger info yet is:%s\n ", str.c_str());
#endif

	DBS_FreeResult(result);
	return MLS_OK;
}


int RankRewardMan::QueryStrongerInfo(UINT32 userId, UINT32 roleId, SQueryStrongerInfos* infos)
{
	//int ret = QueryStrongerInfoForDB(userId,roleId);
	//if(DBS_OK!=ret)
	//	return ret;


	std::vector<UINT16> infov;
	GetRolesInfoMan()->GetStrongerInfo(userId, roleId, infov);

	int len = infov.size();

	for (int i = 0; i<len; i++)
	{
		infos->rewards[i] = infov[i];
	}
	return MLS_OK;
}

int RankRewardMan::UpDBLvRanksGet(UINT32 id)
{
	char szSQL[LCS_SQL_BUF_SIZE] = { 0 };
	sprintf(szSQL, "update ranklevel set award=1 where id=%d", id);
	int retCode = GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
	if (retCode != DBS_OK)
		return retCode;
	return MLS_OK;
}

int RankRewardMan::UpDBInsnumRanksGet(UINT32 id)
{
	char szSQL[LCS_SQL_BUF_SIZE] = { 0 };
	sprintf(szSQL, "update rankinstnum set award=1 where id=%d", id);
	int retCode = GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
	if (retCode != DBS_OK)
		return retCode;
	return MLS_OK;

}

int RankRewardMan::UpDBCPRewardGet(UINT32 roleId, UINT16 rewardid, UINT32 cp)
{
	char szSQL[128] = { 0 };
	sprintf(szSQL, "insert into cp_reward(actorid, cprewardid, cp, tts) values(%u, %d, %u, NOW())", roleId, rewardid, cp);
	return GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);
}

//插入在线信息
int RankRewardMan::InsertOnline(UINT32 roleID, const char* tts)
{
	if (roleID <= 0)
	{
		ASSERT(0);
	}
	char szSQL[128] = { 0 };
	if (tts == NULL)
		sprintf(szSQL, "insert into online_reward(actorid, date,lasttts) values(%u, NOW()，NOW())", roleID);
	else
		sprintf(szSQL, "insert into online_reward(actorid, date,lasttts) values(%u, NOW(),'%s')", roleID, tts);
	int ret = GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);
	return ret;
}


//更新在线时间
int RankRewardMan::UpdateOnlineTime(UINT32 id, LTMSEL time)
{
	char szSQL[LCS_SQL_BUF_SIZE] = { 0 };
	sprintf(szSQL, "update online_reward set online=online+%lld,lasttts=NOW() where id=%u", time, id);
	int retCode = GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
	if (retCode != DBS_OK)
		return retCode;
	return DBS_OK;
}


//获取强者奖励领取信息
int RankRewardMan::GetStrongerReward(UINT32 userId, UINT32 roleId, UINT16 rewardID, SGetRankRewardRes *pRankAward, SReceiveRewardRefresh *pRefresh)
{
	const UINT8 TYPE_POS = __Stronger_Reward_Type;
	if (rewardID == 0)
		return MLS_GONLINE_REWARD_FAIL;

	if (GetRolesInfoMan()->IsGetStrongerReward(userId, roleId, rewardID))
		return MLS_REWARD_ISGET;

	//add by hxw 20151105 判断战力是否足够领取
	UINT32 cp = 0;
	GetRolesInfoMan()->GetRoleCompatPower(userId, roleId, cp);
	if (!GetGameCfgFileMan()->IsCanGet(TYPE_POS, rewardID, cp))
		return MLS_REWARD_CANNOT_GET;
	//end

	//拿取奖励
	if (MLS_OK != GetRolesInfoMan()->ReceiveRankReward(userId, roleId, TYPE_POS, rewardID, pRankAward, pRefresh))
	{
		return MLS_GET_CP_REWARD_FAIL;
	}
	//拿取结束	更新数据库	

	if (MLS_OK != UpDBCPRewardGet(roleId, rewardID))
	{
		return MLS_GET_CP_REWARD_FAIL;
	}
	//pRinfo.strongerVec.push_back(rewardID);
	GetRolesInfoMan()->InputStrongerReward(userId, roleId, rewardID);
	return MLS_OK;
}


//获取副本挑战次数奖励
int RankRewardMan::GetInsnumRankReward(UINT32 userId, UINT32 roleId, SGetRankRewardRes *pRankAward, SReceiveRewardRefresh *pRefresh)
{
	CXTAutoLock lock(m_lvInsnumLK);
	const UINT8 TYPE_POS = __InstanceNum_Rank_Type - 1;
	int len = m_vRanks[TYPE_POS].size();//
	for (int i = 0; i<len; i++)
	{
		if (roleId == m_vRanks[TYPE_POS][i].roleID)
		{
			UINT16 id = m_vRanks[TYPE_POS][i].rankID;
			//add by hxw 20151105 判断等级是否足够领取		
			if (!GetGameCfgFileMan()->IsCanGet(TYPE_POS + 1, id, m_vRanks[TYPE_POS][i].iValue))
				return MLS_REWARD_CANNOT_GET;
			//end
			//更新数据库
			int ret = UpDBInsnumRanksGet(m_vRanks[TYPE_POS][i].dataID);
			if (ret != DBS_OK)
				return ret;
			m_vRanks[TYPE_POS][i].bGet = TRUE;
			//拿取奖励
			if (MLS_OK != GetRolesInfoMan()->ReceiveRankReward(userId, roleId, TYPE_POS + 1, id, pRankAward, pRefresh))
			{
				return MLS_GET_CP_REWARD_FAIL;
			}
			//拿取结束		

			return MLS_OK;
		}
	}
	return MLS_QINUM_RANK_REWARD_NULL;
}

//获取等级排行奖励
int RankRewardMan::GetLvRankReward(UINT32 userId, UINT32 roleId, SGetRankRewardRes *pRankAward, SReceiveRewardRefresh *pRefresh)
{
	const UINT8 TYPE_POS = __Level_Rank_Type - 1;
	CXTAutoLock lock(m_lvRankLK);

	int len = m_vRanks[TYPE_POS].size();//
	for (int i = 0; i<len; i++)
	{
		if (roleId == m_vRanks[TYPE_POS][i].roleID)
		{

			if (m_vRanks[TYPE_POS][i].bGet)
				return MLS_REWARD_ISGET;
			//更新数据库
			UINT32 id = m_vRanks[TYPE_POS][i].rankID;

			//add by hxw 20151105 判断刷的次数是否足够领取
			/*SRoleInfos Rinfo;
			memset(&Rinfo,0,sizeof(Rinfo));
			GetRolesInfoMan()->GetRoleInfo(userId,roleId, &Rinfo);*/
			if (!GetGameCfgFileMan()->IsCanGet(TYPE_POS + 1, id, m_vRanks[TYPE_POS][i].iValue))
				return MLS_REWARD_CANNOT_GET;
			//end

			int iret = UpDBLvRanksGet(id);
			if (iret != DBS_OK)
				return iret;
			m_vRanks[TYPE_POS][i].bGet = TRUE;
			//拿取奖励
			if (MLS_OK != GetRolesInfoMan()->ReceiveRankReward(userId, roleId, __Level_Rank_Type, m_vRanks[TYPE_POS][i].rankID, pRankAward, pRefresh))
			{
				return MLS_GET_CP_REWARD_FAIL;
			}
			//拿取结束
			return MLS_OK;
		}
	}
	return MLS_QLV_RANK_REWARD_NULL;
}




int RankRewardMan::QueryOnlineInfoForDB(UINT32 roleID, SRoleInfos* role, const char* tts)
{
	DBS_RESULT result = NULL;
	char szSQL[MAX_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select id,info,online from online_reward where actorid=%u and date=date_format(NOW(),'%%Y-%%m-%%d')", roleID);

	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols <= 0)
	{
		DBS_FreeResult(result);
		//如果没有该数据则插入该数据 并返回函数
		if (DBS_OK != InsertOnline(roleID, tts))
			return DBS_ERR_SQL;
		retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
		if (retCode != DBS_OK)
			return retCode;
	}

	DBS_ROW row = DBS_FetchRow(result);
	if (row == NULL)
	{
		DBS_FreeResult(result);
		return retCode;
	}
	UINT32 id = (UINT32)_atoi64(row[0]);
	UINT32 time = (UINT32)_atoi64(row[2]);

	role->SetOnlineInfo(id, time);

	char* p = row[1];
	BYTE nCount = *(BYTE*)p;
	UINT16* pHelp = (UINT16*)(p + sizeof(BYTE));

	//add by hxw 20151106 增加测试打印
#if defined(WIN32)
	char getinfo[10] = { 0 };
	std::string str;
#endif

	for (UINT16 i = 0; i < nCount; ++i)
	{
		role->InputOnlineReward(pHelp[i]);

#if defined(WIN32)
		sprintf(getinfo, " %d ", pHelp[i]);
		str += getinfo;
#endif
	}

#if defined(WIN32)
	printf("Get Online Reward info yet is:%s\n ", str.c_str());
#endif

	DBS_FreeResult(result);
	return MLS_OK;
}

//获取在线奖励信息
int RankRewardMan::QueryOnlineInfo(UINT32 userId, UINT32 roleId, SQueryOnlineInfos* infos)
{
	//ranktts>=date_format(NOW(),'%%Y-%%m-%%d')"
	//int ret = QueryOnlineInfoForDB(userId,roleId);
	//if(ret!=DBS_OK)
	//	return ret;

	SOnlineInfo pRinfo;
	std::vector<UINT16> vonline;
	int ret = GetRolesInfoMan()->GetOnlineInfo(userId, roleId, &pRinfo, vonline);
	if (MLS_OK != ret)
		return ret;

	if (pRinfo.id <= 0)
	{
		return MLS_QONLINE_REWARD_FAIL;
	}
	int n = vonline.size();

	for (int i = 0; i<n&&i<ONLINE_NUM; i++)
	{
		infos->rewards[i] = vonline[i];
	}

	infos->time = pRinfo.itmes;

	return MLS_OK;

}
//更新在线领取数据
int RankRewardMan::UpDBOnline(UINT32 id, LTMSEL otime, const std::vector<UINT16>* pOnline)
{
	char szSQL[MAX_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "update online_reward set online=%lld,lasttts=NOW(), info='", otime);

	int n = (int)strlen(szSQL);
	BYTE si = (BYTE)pOnline->size();
	if (si>0)
	{
		n += DBS_EscapeString(szSQL + n, (const char*)&si, sizeof(BYTE));
		if (si > 0)
			DBS_EscapeString(szSQL + n, (const char*)pOnline->data(), si * sizeof(UINT16));
	}
	char szId[128] = { 0 };
	sprintf(szId, "'where id=%u", id);
	strcat(szSQL, szId);
	return GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
}

//获取在线奖励
int RankRewardMan::GetOnlineReward(UINT32 userId, UINT32 roleId, UINT16 rewardID, SGetRankRewardRes *pRankAward, SReceiveRewardRefresh *pRefresh)
{
	const UINT8 TYPE_POS = __Online_Reward_Type;
	if (rewardID == 0)
		return MLS_GONLINE_REWARD_FAIL;
	if (GetRolesInfoMan()->IsGetOnlineReward(userId, roleId, rewardID))
		return MLS_REWARD_ISGET;

	SOnlineInfo info;
	std::vector<UINT16> vonline;
	GetRolesInfoMan()->GetOnlineInfo(userId, roleId, &info, vonline);
	LTMSEL nowTime = GetMsel();

	//add by hxw 20151105 判断在线时间是否足够领取		
	if (!GetGameCfgFileMan()->IsCanGet(TYPE_POS, rewardID, (nowTime - info.lasttime) / 1000 + info.itmes + 20))//服务器允许20秒误差内都可领取
		return MLS_REWARD_CANNOT_GET;
	//end	


	//拿取奖励
	if (MLS_OK != GetRolesInfoMan()->ReceiveRankReward(userId, roleId, TYPE_POS, rewardID, pRankAward, pRefresh))
	{
		return MLS_GET_CP_REWARD_FAIL;
	}
	//拿取结束

	GetRolesInfoMan()->SetOnlineInfo(userId, roleId, 0, 0, nowTime);
	GetRolesInfoMan()->InputOnlineReward(userId, roleId, rewardID);

	//重新获取数组的数并插入
	//GetRolesInfoMan()->GetOnlineInfo(userId,roleId,&info,vonline);
	vonline.push_back(rewardID);
	if (MLS_OK == UpDBOnline(info.id, info.itmes, &vonline))
		return MLS_OK;
	ASSERT(0);
	return MLS_GONLINE_REWARD_FAIL;
}