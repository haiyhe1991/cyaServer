#include "PlayerInfoMan.h"

#include <limits.h>
#include <algorithm>
#include "ConfigFileMan.h"
#include "DBSClient.h"
#include "RoleInfos.h"
#include "EmailCleanMan.h"
#include "cyaMaxMin.h"
#include "GuildMan.h"
#include "GameCfgFileMan.h"
#include "RolesInfoMan.h"
#include "RefreshDBMan.h"
#include <math.h>

#define MAX_FETCH_ROBOT_NUM	10
#define MATCHING_CP_INCREMENT 500
#define MATCHING_LEVEL_INCREMENT 5

static PlayerInfoMan* sg_palyerMan = NULL;
void InitPlayerInfoMan()
{
	ASSERT(sg_palyerMan == NULL);
	sg_palyerMan = new PlayerInfoMan();
	sg_palyerMan->Start();
	ASSERT(sg_palyerMan != NULL);
}

PlayerInfoMan* GetPlayerInfoMan()
{
	return sg_palyerMan;
}

void DestroyPlayerInfoMan()
{
	PlayerInfoMan* playerMan = sg_palyerMan;
	sg_palyerMan = NULL;
	if (playerMan)
	{
		playerMan->Stop();
		delete playerMan;
	}
}

PlayerInfoMan::PlayerInfoMan()
: m_thHandle(INVALID_OSTHREAD)
, m_exit(true)
, m_lastHour(0)
{

}

PlayerInfoMan::~PlayerInfoMan()
{

}

void PlayerInfoMan::Start()
{
	m_exit = false;
	OSCreateThread(&m_thHandle, NULL, THWorker, this, 0);
}

void PlayerInfoMan::Stop()
{
	m_exit = true;
	if (m_thHandle != INVALID_OSTHREAD)
	{
		OSCloseThread(m_thHandle);
		m_thHandle = INVALID_OSTHREAD;
	}
}

//zpy 按等级匹配机器人
int PlayerInfoMan::MatchingByLevel(UINT32 userId, UINT32 roleId, UINT32 *findedRoleId)
{
	*findedRoleId = 0;

	// 获取玩家战力
	SRoleInfos role_info;
	int retCode = GetRolesInfoMan()->GetRoleInfo(userId, roleId, &role_info);
	if (MLS_OK != retCode)
	{
		return retCode;
	}
	UINT32 mine_level = role_info.level;

	// 获取级别
	const int level = mine_level / MATCHING_LEVEL_INCREMENT;
	CXTAutoLock lock(m_level_locker);
	if (level >= m_allRolesSortByLevel.size())
	{
		return MLS_GET_ROLE_ID_FAILED;
	}

	// 确立搜索范围
	std::vector<int> searchBand;
	{
		size_t sum = 0;
		for (size_t i = level; i < m_allRolesSortByLevel.size(); ++i)
		{
			if (!m_allRolesSortByLevel[i].empty())
			{
				sum += m_allRolesSortByLevel[i].size();
				searchBand.push_back(i);
				if (sum >= MAX_FETCH_ROBOT_NUM)
				{
					break;
				}
			}
		}
		if (level > 0 && sum < MAX_FETCH_ROBOT_NUM)
		{
			for (int i = level - 1; i >= 0; --i)
			{
				if (!m_allRolesSortByLevel[i].empty())
				{
					sum += m_allRolesSortByLevel[i].size();
					searchBand.push_back(i);
					if (sum >= MAX_FETCH_ROBOT_NUM)
					{
						break;
					}
				}
			}
		}
	}

	// 匹配玩家
	{
		srand(time(NULL));
		std::vector<int> available;
		std::random_shuffle(searchBand.begin(), searchBand.end());
		for (size_t i = 0; i < searchBand.size(); ++i)
		{
			const std::vector<SAccountInfo> &roles = m_allRolesSortByLevel[searchBand[i]];
			for (size_t j = 0; j < roles.size(); ++j)
			{
				if (roles[j].userId != userId)
				{
					bool unavailable = false;
					GetRolesInfoMan()->HasRobot(userId, roleId, roles[j].roleId, &unavailable);
					if (!unavailable)
					{
						available.push_back(j);
					}
				}
			}

			if (!available.empty())
			{
				int select = rand() % available.size();
				*findedRoleId = roles[available[select]].roleId;
				break;
			}
			available.clear();
		}
	}

	return *findedRoleId != 0 ? MLS_OK : MLS_GET_ROLE_ID_FAILED;
}

//zpy 按战力匹配机器人
int PlayerInfoMan::MatchingByFighting(UINT32 userId, UINT32 roleId, UINT32 *findedRoleId)
{
	*findedRoleId = 0;

	// 获取玩家战力
	UINT32 mine_cp = 0;
	int retCode = GetRolesInfoMan()->GetRoleCompatPower(userId, roleId, mine_cp);
	if (MLS_OK != retCode)
	{
		return retCode;
	}

	// 获取级别
	const int level = mine_cp / MATCHING_CP_INCREMENT;
	CXTAutoLock lock(m_cp_locker);
	if (level >= m_allRolesSortByCP.size())
	{
		return MLS_GET_ROLE_ID_FAILED;
	}

	// 确立搜索范围
	std::vector<int> searchBand;
	{
		size_t sum = 0;
		for (size_t i = level; i < m_allRolesSortByCP.size(); ++i)
		{
			if (!m_allRolesSortByCP[i].empty())
			{
				sum += m_allRolesSortByCP[i].size();
				searchBand.push_back(i);
				if (sum >= MAX_FETCH_ROBOT_NUM)
				{
					break;
				}
			}
		}
		if (level > 0 && sum < MAX_FETCH_ROBOT_NUM)
		{
			for (int i = level - 1; i >= 0; --i)
			{
				if (!m_allRolesSortByCP[i].empty())
				{
					sum += m_allRolesSortByCP[i].size();
					searchBand.push_back(i);
					if (sum >= MAX_FETCH_ROBOT_NUM)
					{
						break;
					}
				}
			}
		}
	}

	// 匹配玩家
	{
		srand(time(NULL));
		std::vector<int> available;
		std::random_shuffle(searchBand.begin(), searchBand.end());
		for (size_t i = 0; i < searchBand.size(); ++i)
		{
			const std::vector<SAccountInfo> &roles = m_allRolesSortByCP[searchBand[i]];
			for (size_t j = 0; j < roles.size(); ++j)
			{
				if (roles[j].userId != userId)
				{
					bool unavailable = false;
					GetRolesInfoMan()->HasRobot(userId, roleId, roles[j].roleId, &unavailable);
					if (!unavailable)
					{
						available.push_back(j);
					}
				}
			}

			if (!available.empty())
			{
				int select = rand() % available.size();
				*findedRoleId = roles[available[select]].roleId;
				break;
			}
			available.clear();
		}
	}

	return *findedRoleId != 0 ? MLS_OK : MLS_GET_ROLE_ID_FAILED;
}

//查询机器人数据
int PlayerInfoMan::QueryRobotData(UINT32 roleId, SFetchRobotRes* pRobotRes)
{
	// 获取玩家数据
	DBS_ROW row = NULL;
	DBS_RESULT result = NULL;
	DBS_ROWLENGTH pRowLen = NULL;
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "SELECT A.id, A.nick, A.level, A.occu, A.faction, A.dressid, A.actortype, A.sex, A.guild, A.cp, D.wearequipment, D.ability, D.equipsock, D.lh_pos "
		"FROM actor as A left join data as D on (A.id=D.actorid) WHERE id = %u", roleId);
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;

	row = DBS_FetchRow(result, &pRowLen);
	if (row == NULL)
	{
		DBS_FreeResult(result);
		return MLS_GET_ROLE_ID_FAILED;
	}

	pRobotRes->robotId = (UINT32)_atoi64(row[0]);
	strcpy(pRobotRes->robotNick, row[1]);
	pRobotRes->level = (BYTE)atoi(row[2]);
	pRobotRes->titleId = (UINT16)atoi(row[3]);
	pRobotRes->factionId = (BYTE)atoi(row[4]);
	pRobotRes->dressId = (UINT16)atoi(row[5]);
	pRobotRes->occuId = (BYTE)atoi(row[6]);
	pRobotRes->sex = (BYTE)atoi(row[7]);
	pRobotRes->guildId = (UINT32)_atoi64(row[8]);
	pRobotRes->cp = (UINT32)_atoi64(row[9]);

	//穿戴装备
	int nSize = sizeof(SFetchRobotRes)-sizeof(char);
	BYTE maxNum = (BYTE)((MAX_RES_USER_BYTES - nSize) / sizeof(SPropertyAttr));
	SPropertyAttr* pRobotWearEquipment = (SPropertyAttr*)pRobotRes->data;

	BYTE idx = 0;
	int nDataLen = pRowLen[10];
	BYTE* pData = (BYTE*)row[10];
	BYTE nCount = (BYTE)((SGSU32Sub(nDataLen, sizeof(BYTE))) / sizeof(SPropertyAttr));
	nCount = min(nCount, *(BYTE*)pData);
	SPropertyAttr* pWearEquipment = (SPropertyAttr*)(pData + sizeof(BYTE));
	for (BYTE i = 0; i < nCount; ++i)
	{
		memcpy(&pRobotWearEquipment[idx], &pWearEquipment[i], sizeof(SPropertyAttr));
		pRobotWearEquipment[idx].hton();
		if (++idx >= maxNum)
			break;
	}
	pRobotRes->wearEquipNum = idx;

	//装载技能
	int offset = nSize + pRobotRes->wearEquipNum * sizeof(SPropertyAttr);
	SSkillMoveInfo* pRobotSkills = (SSkillMoveInfo*)((BYTE*)pRobotRes + offset);
	maxNum = (BYTE)((MAX_RES_USER_BYTES - offset) / sizeof(UINT16));

	idx = 0;
	nDataLen = pRowLen[11];
	pData = (BYTE*)row[11];
	nCount = (BYTE)((SGSU32Sub(nDataLen, sizeof(BYTE))) / sizeof(SSkillMoveInfo));
	nCount = min(nCount, *(BYTE*)pData);
	SSkillMoveInfo* pSkillMove = (SSkillMoveInfo*)(pData + sizeof(BYTE));
	for (BYTE i = 0; i < nCount; ++i)
	{
		if (pSkillMove[i].key != 0)
		{
			pSkillMove[i].hton();
			pRobotSkills[idx] = pSkillMove[i];
			if (++idx >= maxNum)
				break;
		}
	}
	pRobotRes->skillNum = idx;

	//装备位属性
	idx = 0;
	nDataLen = pRowLen[12];
	pData = (BYTE*)row[12];
	offset += pRobotRes->skillNum * sizeof(SSkillMoveInfo);

	maxNum = (BYTE)((MAX_RES_USER_BYTES - offset) / sizeof(SEquipPosAttr));
	nCount = (BYTE)((SGSU32Sub(nDataLen, sizeof(BYTE))) / sizeof(SEquipPosAttr));
	SEquipPosAttr *pEquipPosAttr = (SEquipPosAttr*)(pData + sizeof(BYTE));
	SEquipPosAttr *pRobotEquipPos = (SEquipPosAttr*)((BYTE*)pRobotRes + offset);
	for (BYTE i = 0; i < nCount; ++i)
	{
		pRobotEquipPos[idx] = pEquipPosAttr[i];
		pRobotEquipPos[idx].hton();
		if (++idx >= maxNum)
			break;
	}
	pRobotRes->equipPosNum = idx;

	//炼魂部件信息
	idx = 0;
	nDataLen = pRowLen[13];
	pData = (BYTE*)row[13];
	offset += pRobotRes->equipPosNum * sizeof(SEquipPosAttr);

	BYTE min_level = UCHAR_MAX;
	maxNum = (BYTE)((MAX_RES_USER_BYTES - offset) / sizeof(SChainSoulPosAttr));
	nCount = (BYTE)((SGSU32Sub(nDataLen, sizeof(BYTE))) / sizeof(SChainSoulPosAttr));
	SChainSoulPosAttr *pEhainSoulPosAttr = (SChainSoulPosAttr*)(pData + sizeof(BYTE));
	SChainSoulPosAttr *pRobotChainSoulPos = (SChainSoulPosAttr*)((BYTE*)pRobotRes + offset);
	for (BYTE i = 0; i < nCount; ++i)
	{
		pRobotChainSoulPos[idx] = pEhainSoulPosAttr[i];
		if (pRobotChainSoulPos[idx].level < min_level)
		{
			min_level = pRobotChainSoulPos[idx].level;
		}
		if (++idx >= maxNum)
			break;
	}
	pRobotRes->chainSoulPosNum = idx;
	pRobotRes->core_level = GetGameCfgFileMan()->GetCoreLevelByConditionLevel(min_level);

	DBS_FreeResult(result);

	return MLS_OK;
}

// zpy 2015.10.13修改
int PlayerInfoMan::FetchRobotData(UINT32 userId, UINT32 roleId, const SFetchRobotReq* req, SFetchRobotRes* pRobotRes)
{
	// 如果没有指明指定角色，则按照匹配规则匹配
	UINT32 findRoleId = req->targetId;
	if (findRoleId == 0)
	{
		int retCode = MLS_OK;
		if (req->rule == MATCH_ROBOT_BY_CP)
		{
			retCode = MatchingByFighting(userId, roleId, &findRoleId);
		}
		else
		{
			retCode = MatchingByLevel(userId, roleId, &findRoleId);
		}
		if (retCode != MLS_OK) return retCode;
		GetRolesInfoMan()->AddRobot(userId, roleId, findRoleId);
	}

	// 查询机器人数据
	int retCode = QueryRobotData(findRoleId, pRobotRes);
	return retCode;
}

int PlayerInfoMan::QueryPlayerAppearance(UINT32 playerId, SQueryRoleAppearanceRes* pAppearanceRes)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select A.id, A.nick, A.level, A.dressid, A.occu, D.wearequipment "
		"from actor as A left join data as D on (A.id=D.actorid) where A.id=%u", playerId);

	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols < 6)
	{
		DBS_FreeResult(result);
		return MLS_ROLE_NOT_EXIST;
	}

	DBS_ROWLENGTH pRowLen = NULL;
	DBS_ROW row = DBS_FetchRow(result, &pRowLen);
	pAppearanceRes->roleId = playerId;
	strcpy(pAppearanceRes->nick, row[1]);
	pAppearanceRes->level = (BYTE)atoi(row[2]);
	pAppearanceRes->dressId = (UINT16)atoi(row[3]);
	pAppearanceRes->titleId = (UINT16)atoi(row[4]);

	BYTE idx = 0;
	int nDataLen = pRowLen[5];
	BYTE* pData = (BYTE*)row[5];
	BYTE nCount = (BYTE)((nDataLen - sizeof(BYTE)) / sizeof(SPropertyAttr));
	nCount = min(nCount, *(BYTE*)pData);

	BYTE maxNum = (BYTE)((MAX_RES_USER_BYTES - member_offset(SQueryRoleAppearanceRes, roleId, props)) / sizeof(SPropertyAttr));
	SPropertyAttr* pWearEquipment = (SPropertyAttr*)(pData + sizeof(BYTE));
	for (BYTE i = 0; i < nCount; ++i)
	{
		memcpy(&pAppearanceRes->props[idx], &pWearEquipment[i], sizeof(SPropertyAttr));
		pAppearanceRes->props[idx].hton();
		if (++idx >= maxNum)
			break;
	}
	pAppearanceRes->num = idx;
	DBS_FreeResult(result);
	return retCode;
}

int PlayerInfoMan::QueryPlayerExplicit(UINT32 playerId, SQueryRoleExplicitRes* pExplicitRes)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select A.id, A.nick, A.gold, A.level, A.occu, A.exp, A.cash, "
		"A.cashcount, A.actortype, A.sex, A.hp, A.mp, A.vit, "
		"A.cp, A.ap, A.packsize, A.waresize, A.guild, A.coords, D.ability "
		"from actor as A left join data as D on (A.id=D.actorid) where A.id=%u", playerId);

	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols < 20)
	{
		DBS_FreeResult(result);
		return MLS_ROLE_NOT_EXIST;
	}

	DBS_ROWLENGTH pRowLen = NULL;
	DBS_ROW row = DBS_FetchRow(result, &pRowLen);

	pExplicitRes->roleId = playerId;			//角色Id
	strcpy(pExplicitRes->roleNick, row[1]);	//角色昵称[33];
	pExplicitRes->roleLevel = (BYTE)atoi(row[3]);		//角色等级
	pExplicitRes->titleId = atoi(row[4]);		//称号id;
	U64ToBigNumber(_atoi64(row[2]), pExplicitRes->gold);	//游戏币
	U64ToBigNumber(_atoi64(row[5]), pExplicitRes->exp);	//经验
	pExplicitRes->cash = (UINT32)_atoi64(row[6]);		//现金
	pExplicitRes->occuId = atoi(row[8]);	//职业id
	pExplicitRes->sex = atoi(row[9]);		//性别
	pExplicitRes->cashcount = (UINT32)_atoi64(row[7]);	//充值总额
	pExplicitRes->hp = (UINT32)_atoi64(row[10]);		//生命
	pExplicitRes->mp = (UINT32)_atoi64(row[11]);		//魔力
	pExplicitRes->vit = (UINT32)_atoi64(row[12]);		//体力
	pExplicitRes->cp = (UINT32)_atoi64(row[13]);		//当前战力;
	pExplicitRes->ap = (UINT16)atoi(row[14]);			//当前ap;
	pExplicitRes->packSize = (BYTE)atoi(row[15]);		//背包大小
	pExplicitRes->wareSize = (BYTE)atoi(row[16]);		//仓库大小
	pExplicitRes->guildId = (UINT32)_atoi64(row[17]);		//公会ID;
	if (pRowLen[18] > 0)
		memcpy(&pExplicitRes->pos, row[18], sizeof(pExplicitRes->pos));	//位置

	//mid_cash add by hxw 20151116 角色信息包含代币


	/*memset(szSQL,0,szSQL);
	sprintf(szSQL, "select where A.id=%u", playerId);

	DBS_FreeResult(result);
	result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if(retCode != DBS_OK)
	return retCode;*/
	//end

	BYTE idx = 0;
	int nDataLen = pRowLen[19];
	BYTE* pData = (BYTE*)row[19];
	BYTE nCount = (BYTE)((nDataLen - sizeof(BYTE)) / sizeof(SSkillMoveInfo));
	nCount = min(nCount, *(BYTE*)pData);
	BYTE maxNum = (BYTE)((MAX_RES_USER_BYTES - member_offset(SQueryRoleExplicitRes, roleId, skills)) / sizeof(SSkillMoveInfo));
	SSkillMoveInfo* pSkillMove = (SSkillMoveInfo*)(pData + sizeof(BYTE));
	for (BYTE i = 0; i < nCount; ++i)
	{
		memcpy(&pExplicitRes->skills[idx], &pSkillMove[i], sizeof(SSkillMoveInfo));
		pExplicitRes->skills[idx].hton();
		if (++idx >= maxNum)
			break;
	}
	pExplicitRes->num = idx;
	DBS_FreeResult(result);

	SQueryGuildNameRes guildNameRes;
	retCode = GetGuildMan()->QueryGuildName(pExplicitRes->guildId, &guildNameRes);
	if (retCode == MLS_OK)
		strcpy(pExplicitRes->guildNick, guildNameRes.guildName);

	SRoleAttrPoint attr;
	GetGameCfgFileMan()->GetRoleCfgBasicAttr(pExplicitRes->occuId, pExplicitRes->roleLevel, attr);
	pExplicitRes->attack = attr.attack;
	pExplicitRes->def = attr.def;
	return MLS_OK;
}

void PlayerInfoMan::InputPlayerMsg(int msgType, UINT32 userId, UINT32 roleId)
{
	SPlayerMsg msg;
	msg.msg = msgType;
	msg.cash.userId = userId;
	msg.cash.roleId = roleId;

	CXTAutoLock lock(m_locker);
	m_playerMsgQ.push_back(msg);
}

int PlayerInfoMan::THWorker(void* param)
{
	PlayerInfoMan* pThis = (PlayerInfoMan*)param;
	ASSERT(pThis != NULL);
	return pThis->OnWorker();
}

int PlayerInfoMan::OnWorker()
{
	while (!m_exit)
	{
		// zpy 刷新战力缓存
		SYSTEMTIME lcTime;
		GetLocalTime(&lcTime);
		if (lcTime.wHour != m_lastHour)
		{
			LoadRolesFightingCache();
			LoadRolesLevelCache();
			m_lastHour = lcTime.wHour;
		}

		if (m_playerMsgQ.empty())
		{
			Sleep(1);
			continue;
		}

		SPlayerMsg msg;
		BOOL needSleep = false;

		{
			CXTAutoLock lock(m_locker);
			if (m_playerMsgQ.empty())
			{
				needSleep = true;
			}
			else
			{
				msg = *m_playerMsgQ.begin();
				m_playerMsgQ.erase(m_playerMsgQ.begin());
				needSleep = false;
			}
		}

		if (needSleep)
		{
			Sleep(1);
			continue;
		}

		OnProcessMsg(msg);
	}

	return 0;
}

void PlayerInfoMan::OnProcessMsg(const SPlayerMsg& msg)
{
	switch (msg.msg)
	{
	case E_HAS_CASH_TOKEN:
		CheckOrderStatus(msg.cash.userId, msg.cash.roleId);
		break;
	default:
		break;
	}
}

void PlayerInfoMan::CheckOrderStatus(UINT32 userId, UINT32 roleId)
{
	BOOL hasNotReceiveOrder = false;
	GetRolesInfoMan()->CheckPlayerRechargeOrderStatus(userId, roleId, hasNotReceiveOrder);
	if (hasNotReceiveOrder)
		GetRefreshDBMan()->InputUpdateInfo(userId, roleId, REFRESH_ROLE_CASH);
}

// zpy新增 加载角色战力信息
int PlayerInfoMan::LoadRolesFightingCache()
{
	DBS_ROW row = NULL;
	DBS_RESULT result = NULL;
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "SELECT accountid,id,cp FROM actor ORDER BY cp");
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;

	CXTAutoLock lock(m_cp_locker);
	m_allRolesSortByCP.clear();
	UINT32 nRows = DBS_NumRows(result);
	for (int i = 0; i < nRows; ++i)
	{
		row = DBS_FetchRow(result);
		if (row != NULL)
		{
			UINT32 uid = atoi(row[0]);
			UINT32 rid = atoi(row[1]);
			UINT32 cp = atoi(row[2]);
			int index = cp / MATCHING_CP_INCREMENT;
			m_allRolesSortByCP.resize(index + 1);
			m_allRolesSortByCP[index].push_back(SAccountInfo(uid, rid));
		}
	}
	DBS_FreeResult(result);

	return MLS_OK;
}

//zpy新增 加载角色等级信息
int PlayerInfoMan::LoadRolesLevelCache()
{
	DBS_ROW row = NULL;
	DBS_RESULT result = NULL;
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "SELECT accountid,id,level FROM actor ORDER BY level");
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;

	CXTAutoLock lock(m_level_locker);
	m_allRolesSortByLevel.clear();
	UINT32 nRows = DBS_NumRows(result);
	for (int i = 0; i < nRows; ++i)
	{
		row = DBS_FetchRow(result);
		if (row != NULL)
		{
			UINT32 uid = atoi(row[0]);
			UINT32 rid = atoi(row[1]);
			UINT32 level = atoi(row[2]);
			int index = level / MATCHING_LEVEL_INCREMENT;
			m_allRolesSortByLevel.resize(index + 1);
			m_allRolesSortByLevel[index].push_back(SAccountInfo(uid, rid));
		}
	}
	DBS_FreeResult(result);

	return MLS_OK;
}