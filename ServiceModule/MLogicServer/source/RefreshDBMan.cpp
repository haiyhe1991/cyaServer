#include "RefreshDBMan.h"
#include "ConfigFileMan.h"
#include "DBSClient.h"
#include "RolesInfoMan.h"
#include "cyaMaxMin.h"
#include "AchievementMan.h"
#include "CashDBMan.h"

static RefreshDBMan* sg_refreshDBMan = NULL;
void InitRefreshDBMan()
{
	ASSERT(sg_refreshDBMan == NULL);
	sg_refreshDBMan = new RefreshDBMan();
	ASSERT(sg_refreshDBMan != NULL);
}

RefreshDBMan* GetRefreshDBMan()
{
	return sg_refreshDBMan;
}

void DestroyRefreshDBMan()
{
	RefreshDBMan* refreshMan = sg_refreshDBMan;
	sg_refreshDBMan = NULL;
	if (NULL != refreshMan)
		delete refreshMan;
}

RefreshDBMan::RefreshDBMan()
{
	DWORD interval = GetConfigFileMan()->GetRefreshDBInterval();
	m_refreshDBTimer.Start(RefreshRoleTimer, this, interval * 1000);
}

RefreshDBMan::~RefreshDBMan()
{
	if (m_refreshDBTimer.IsStarted())
		m_refreshDBTimer.Stop();

	CXTAutoLock lock(m_locker);
	std::map<std::string, SUpdateDBInfo>::iterator it = m_updateMap.begin();
	for (; it != m_updateMap.end(); ++it)
		OnRefreshRoleInfo(it->second);
	m_updateMap.clear();
}

void RefreshDBMan::ExitSyncUserRoleInfo(UINT32 userId, const std::vector<UINT32>& rolesVec)
{
	BYTE si = (BYTE)rolesVec.size();
	if (si <= 0)
		return;

	std::vector<SUpdateDBInfo> upVec;

	{
		CXTAutoLock lock(m_locker);
		for (BYTE i = 0; i < si; ++i)
		{
			for (int j = REFRESH_ROLE_ID_START; j <= REFRESH_ROLE_ID_END; ++j)
			{
				char szKey[64] = { 0 };
				sprintf(szKey, "%u-%u-%d", userId, rolesVec[i], j);
				std::map<std::string, SUpdateDBInfo>::iterator it = m_updateMap.find(szKey);
				if (it == m_updateMap.end())
					continue;

				upVec.push_back(it->second);
				m_updateMap.erase(it);
			}
		}
	}

	si = upVec.size();
	for (size_t i = 0; i < si; ++i)
		OnRefreshRoleInfo(upVec[i]);
}

void RefreshDBMan::InputUpdateInfo(UINT32 userId, UINT32 roleId, UINT16 type)
{
	SUpdateDBInfo item(userId, roleId, type);
	char szKey[64] = { 0 };
	sprintf(szKey, "%u-%u-%d", userId, roleId, type);

	BOOL enforceSyncDB = false;
	{
		CXTAutoLock lock(m_locker);
		std::map<std::string, SUpdateDBInfo>::iterator it = m_updateMap.find(szKey);
		if (it == m_updateMap.end())
			m_updateMap.insert(std::make_pair(szKey, item));
		enforceSyncDB = m_updateMap.size() >= GetConfigFileMan()->GetMaxCacheItems() ? true : false;
	}

	if (enforceSyncDB)
		m_refreshDBTimer.Enforce();
}

void RefreshDBMan::RefreshRoleTimer(void* param, TTimerID id)
{
	RefreshDBMan* pThis = (RefreshDBMan*)param;
	if (pThis)
		pThis->OnRefreshRoleInfo();
}

void RefreshDBMan::OnRefreshRoleInfo()
{
	if (m_updateMap.empty())
		return;

	std::vector<SUpdateDBInfo> upVec;
	{
		CXTAutoLock lock(m_locker);
		std::map<std::string, SUpdateDBInfo>::iterator it = m_updateMap.begin();
		for (; it != m_updateMap.end(); ++it)
			upVec.push_back(it->second);
		m_updateMap.clear();
	}

	size_t si = upVec.size();
	for (size_t i = 0; i < si; ++i)
		OnRefreshRoleInfo(upVec[i]);
}

void RefreshDBMan::OnRefreshRoleInfo(const SUpdateDBInfo& upInfo)
{
	switch (upInfo.type)
	{
	case REFRESH_ROLE_CP:	//刷新战力
		OnUpdateRoleCompatPower(upInfo.userId, upInfo.roleId);
		break;
	case REFRESH_ROLE_BACKPACK:	//刷新背包
		OnUpdateRoleBackpack(upInfo.userId, upInfo.roleId);
		break;
	case REFRESH_ROLE_TASK:	//刷新已接受任务
		OnUpdateRoleAcceptTask(upInfo.userId, upInfo.roleId);
		break;
	case REFRESH_ROLE_STORAGE:	//刷新存储空间
		OnUpdateRoleStorageSpace(upInfo.userId, upInfo.roleId);
		break;
	case REFRESH_ROLE_GOLD:	//刷新金币
		OnUpdateRoleGold(upInfo.userId, upInfo.roleId);
		break;
	case REFRESH_ROLE_VIST_INFO:	//刷新角色访问信息
		OnUpdateRoleVistInfo(upInfo.userId, upInfo.roleId);
		break;
	case REFRESH_ROLE_SKILLS:	//刷新角色已学技能
		OnUpdateRoleSkills(upInfo.userId, upInfo.roleId);
		break;
	case REFRESH_ROLE_FIN_INST:	//刷新角色完成副本
		OnUpdateRoleFinInst(upInfo.userId, upInfo.roleId);
		break;
	case REFRESH_ROLE_VIT:	//刷新角色体力
		OnUpdateRoleVit(upInfo.userId, upInfo.roleId);
		break;
	case REFRESH_ROLE_UPGRADE:
		OnUpdateRoleUpgrade(upInfo.userId, upInfo.roleId);
		break;
	case REFRESH_ROLE_FACTION:
		OnUpdateRoleFaction(upInfo.userId, upInfo.roleId);
		break;
	case REFRESH_ROLE_WEAR_BACKPACK:
		OnUpdateRoleWearBackpack(upInfo.userId, upInfo.roleId);
		break;
	case REFRESH_ROLE_WEAR:
		OnUpdateRoleWear(upInfo.userId, upInfo.roleId);
		break;
	case REFRESH_ROLE_TITLE_ID:
		OnUpdateRoleTitleId(upInfo.userId, upInfo.roleId);
		break;
	case REFRESH_ROLE_CASH:
		OnUpdateRoleCash(upInfo.userId, upInfo.roleId);
		break;
	case REFRESH_ROLE_DRESS:
		OnUpdateRoleDress(upInfo.userId, upInfo.roleId);
		break;
	case REFRESH_ROLE_DRESS_WEAR:
		OnUpdateRoleDressWear(upInfo.userId, upInfo.roleId);
		break;
	case REFRESH_ROLE_FRAGMENT:
		OnUpdateRoleFragment(upInfo.userId, upInfo.roleId);
		break;
	case REFRESH_ROLE_GOLD_EXCHANGE:
		OnUpdateRoleGoldExchange(upInfo.userId, upInfo.roleId);
		break;
	case REFRESH_ROLE_FIN_TASK:
		OnUpdateRoleFinTask(upInfo.userId, upInfo.roleId);
		break;
	case REFRESH_ROLE_EXP:
		OnUpdateRoleExp(upInfo.userId, upInfo.roleId);
		break;
	case REFRESH_ROLE_EQUIP_POS:
		OnUpdateRoleEquipPos(upInfo.userId, upInfo.roleId);
		break;

		/* zpy 成就系统新增 */
	case REFRESH_ROLE_KILL_MONSTER:
		OnUpdateRoleKillMonster(upInfo.userId, upInfo.roleId);
		break;
	case REFRESH_ROLE_PICKUP_BOX_SUM:
		OnUpdateRolePickupBoxSum(upInfo.userId, upInfo.roleId);
		break;
	case REFRESH_ROLE_COMBAT_PERFORMANCE:
		OnUpdateRoleCombatPerformance(upInfo.userId, upInfo.roleId);
		break;

		/* zpy 炼魂系统新增 */
	case REFRESH_ROLE_CHAINSOUL_FRAGMENT:
		OnUpdateRoleChainSoulFragment(upInfo.userId, upInfo.roleId);
		break;
	case REFRESH_ROLE_CHAINSOUL_POS:
		OnUpdateRoleChainSoulPos(upInfo.userId, upInfo.roleId);
		break;

		/* zpy 天梯系统 */
	case REFRESH_ROLE_LADDER_INFO:
		OnUpdateLadderInfo(upInfo.userId, upInfo.roleId);
		break;
	case REFRESH_ROLE_LADDER_MATCHING_INFO:
		OnUpdateLadderMatchingInfo(upInfo.userId, upInfo.roleId);
		break;
	case REFRESH_ROLE_RESET_LADDER_RANK_VIEW:
		OnResetLadderView(upInfo.userId, upInfo.roleId);
		break;

	default:
		break;
	}
}

void RefreshDBMan::OnUpdateRoleCompatPower(UINT32 userId, UINT32 roleId)
{
	UINT32 cp = 0;
	int retCode = GetRolesInfoMan()->GetRoleCompatPower(userId, roleId, cp);
	if (retCode != MLS_OK)
		return;

	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "update actor set cp=%u where id=%u", cp, roleId);
	GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
}

void RefreshDBMan::OnUpdateRoleBackpack(UINT32 userId, UINT32 roleId)
{
	std::vector<SPropertyAttr> vec;
	int retCode = GetRolesInfoMan()->GetRoleBackpack(userId, roleId, vec);
	if (retCode != MLS_OK)
		return;

	char szSQL[MAX_SQL_BUF_LEN] = { 0 };
	strcpy(szSQL, "update data set backpack='");

	int n = (int)strlen(szSQL);
	BYTE si = (BYTE)vec.size();
	n += DBS_EscapeString(szSQL + n, (const char*)&si, sizeof(BYTE));
	if (si > 0)
		DBS_EscapeString(szSQL + n, (const char*)vec.data(), si * sizeof(SPropertyAttr));

	char szId[64] = { 0 };
	sprintf(szId, "' where actorid=%u", roleId);
	strcat(szSQL, szId);
	GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
}

void RefreshDBMan::OnUpdateRoleAcceptTask(UINT32 userId, UINT32 roleId)
{
	std::vector<SAcceptTaskInfo> vec;
	int retCode = GetRolesInfoMan()->GetRoleAcceptTask(userId, roleId, vec);
	if (retCode != MLS_OK)
		return;

	char szSQL[MAX_SQL_BUF_LEN] = { 0 };
	strcpy(szSQL, "update data set taskaccepted='");

	int n = (int)strlen(szSQL);
	BYTE si = (BYTE)vec.size();
	n += DBS_EscapeString(szSQL + n, (const char*)&si, sizeof(BYTE));
	if (si > 0)
		DBS_EscapeString(szSQL + n, (const char*)vec.data(), si * sizeof(SAcceptTaskInfo));

	char szId[64] = { 0 };
	sprintf(szId, "' where actorid=%u", roleId);
	strcat(szSQL, szId);
	GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
}

void RefreshDBMan::OnUpdateRoleStorageSpace(UINT32 userId, UINT32 roleId)
{
	BYTE packSize = 0;
	BYTE wareSize = 0;
	int retCode = GetRolesInfoMan()->GetRoleStorageSpace(userId, roleId, packSize, wareSize);
	if (retCode != MLS_OK)
		return;

	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "update actor set packsize=%d, waresize=%d where id=%u", packSize, wareSize, roleId);
	GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
}

void RefreshDBMan::OnUpdateRoleGold(UINT32 userId, UINT32 roleId)
{
	UINT64 gold = 0;
	UINT32 cash = 0;
	UINT32 cashcount = 0;
	int retCode = GetRolesInfoMan()->GetRoleMoney(userId, roleId, gold, cash, cashcount);
	if (retCode != MLS_OK)
		return;

	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "update actor set gold=%llu where id=%u", gold, roleId);
	GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
}

void RefreshDBMan::OnUpdateRoleVistInfo(UINT32 userId, UINT32 roleId)
{
	UINT32 vistCount = 0;
	UINT32 vit = 0;
	char szTime[32] = { 0 };
	int retCode = GetRolesInfoMan()->GetRoleVistInfo(userId, roleId, vistCount, vit, szTime);
	if (retCode != MLS_OK)
		return;

	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "update actor set vit=%u, vist=%u, lvtts='%s' where id=%u", vit, vistCount, szTime, roleId);
	GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
}

void RefreshDBMan::OnUpdateRoleSkills(UINT32 userId, UINT32 roleId)
{
	std::vector<SSkillMoveInfo> vec;
	int retCode = GetRolesInfoMan()->GetRoleSkills(userId, roleId, vec);
	if (retCode != MLS_OK)
		return;

	char szSQL[MAX_SQL_BUF_LEN] = { 0 };
	strcpy(szSQL, "update data set ability='");

	int n = (int)strlen(szSQL);
	BYTE si = (BYTE)vec.size();
	n += DBS_EscapeString(szSQL + n, (const char*)&si, sizeof(BYTE));
	if (si > 0)
		DBS_EscapeString(szSQL + n, (const char*)vec.data(), si * sizeof(SSkillMoveInfo));

	char szId[64] = { 0 };
	sprintf(szId, "' where actorid=%u", roleId);
	strcat(szSQL, szId);
	GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
}

void RefreshDBMan::OnUpdateRoleFinTask(UINT32 userId, UINT32 roleId)
{
	SRoleInfos roleInfo;
	int retCode = GetRolesInfoMan()->GetRoleInfo(userId, roleId, &roleInfo);
	if (retCode != MLS_OK)
		return;
	//mid_cash by hxw 20151116
	char szSQL[MAX_SQL_BUF_LEN] = { 0 };
	if (GetConfigFileMan()->GetCashSaveType() == CASH_SAVE_TYPE_TO_ACTOR)
	{
		sprintf(szSQL, "update actor set gold=%llu, exp=%llu , cash=%u where id=%u",
			BigNumberToU64(roleInfo.gold), BigNumberToU64(roleInfo.exp), roleInfo.cash, roleId);
		GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
	}
	else
	{
		sprintf(szSQL, "update actor set gold=%llu, exp=%llu where id=%u",
			BigNumberToU64(roleInfo.gold), BigNumberToU64(roleInfo.exp), roleId);
		GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
		UpdateCashToCashDB(userId, roleInfo.cash);
	}
	//end 20151116

	//更新背包
	OnUpdateRoleBackpack(userId, roleId);

	//刷新碎片
	OnUpdateRoleFragment(userId, roleId);

	//刷新已接任务
	OnUpdateRoleAcceptTask(userId, roleId);
}

void RefreshDBMan::OnUpdateRoleVit(UINT32 userId, UINT32 roleId)
{
	UINT32 vit = 0;
	int retCode = GetRolesInfoMan()->GetRoleCurrentVit(userId, roleId, vit);
	if (retCode != MLS_OK)
		return;

	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "update actor set vit=%u where id=%u", vit, roleId);
	GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
}

void RefreshDBMan::OnUpdateRoleUpgrade(UINT32 userId, UINT32 roleId)
{
	//更新技能
	OnUpdateRoleSkills(userId, roleId);

	SRoleInfos roleInfo;
	int retCode = GetRolesInfoMan()->GetRoleInfo(userId, roleId, &roleInfo);
	if (retCode != MLS_OK)
		return;

	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "update actor set level=%d, exp=%llu, hp=%u, mp=%u, attack=%u, def=%u, cp=%u, ap=%u where id=%u",
		roleInfo.level, BigNumberToU64(roleInfo.exp), roleInfo.hp, roleInfo.mp,
		roleInfo.attack, roleInfo.def, roleInfo.cp, roleInfo.ap, roleId);
	GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
}

void RefreshDBMan::OnUpdateRoleFaction(UINT32 userId, UINT32 roleId)
{
	BYTE factionId = 0;
	int retCode = GetRolesInfoMan()->GetRoleFaction(userId, roleId, factionId);
	if (retCode != MLS_OK)
		return;

	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "update actor set faction=%d where id=%u", factionId, roleId);
	GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
}

void RefreshDBMan::OnUpdateRoleWearBackpack(UINT32 userId, UINT32 roleId)
{
	OnUpdateRoleWear(userId, roleId);
	OnUpdateRoleBackpack(userId, roleId);
}

void RefreshDBMan::OnUpdateRoleWear(UINT32 userId, UINT32 roleId)
{
	//刷新战力
	OnUpdateRoleCompatPower(userId, roleId);

	//穿戴装备
	std::vector<SPropertyAttr> wear;
	int retCode = GetRolesInfoMan()->GetRoleWearEquip(userId, roleId, wear);
	if (retCode != MLS_OK)
		return;

	char szSQL[MAX_SQL_BUF_LEN] = { 0 };
	strcpy(szSQL, "update data set wearequipment='");

	BYTE si = (BYTE)wear.size();
	int n = (int)strlen(szSQL);
	n += DBS_EscapeString(szSQL + n, (const char*)&si, sizeof(BYTE));
	if (si > 0)
		DBS_EscapeString(szSQL + n, (const char*)wear.data(), si * sizeof(SPropertyAttr));

	char szId[64] = { 0 };
	sprintf(szId, "' where actorid=%u", roleId);
	strcat(szSQL, szId);
	GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
}

void RefreshDBMan::OnUpdateRoleTitleId(UINT32 userId, UINT32 roleId)
{
	UINT16 titleId = 0;
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	int retCode = GetRolesInfoMan()->GetRoleTitleId(userId, roleId, titleId);
	if (retCode != MLS_OK)
		return;

	UINT32 cp = 0;
	retCode = GetRolesInfoMan()->GetRoleCompatPower(userId, roleId, cp);
	if (retCode != MLS_OK)
	{
		sprintf(szSQL, "update actor set occu=%d where id=%u", titleId, roleId);
		GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
	}
	else
	{
		sprintf(szSQL, "update actor set occu=%d, cp=%u where id=%u", titleId, cp, roleId);
		GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
	}
}

void RefreshDBMan::OnUpdateRoleCash(UINT32 userId, UINT32 roleId)
{
	UINT32 cash = 0;
	UINT32 cashcount = 0;
	UINT64 gold = 0;
	int retCode = GetRolesInfoMan()->GetRoleMoney(userId, roleId, gold, cash, cashcount);
	if (retCode != MLS_OK)
		return;

	//mid_cash by hxw 20151116
	/*
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "update actor set cash=%u, cashcount=%u where id=%u", cash, cashcount, roleId);
	GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
	*/
	//mid to
	if (GetConfigFileMan()->GetCashSaveType() == CASH_SAVE_TYPE_TO_ACTOR)
		UpdateCashToActorDB(roleId, cash, cashcount);
	else
		UpdateCashToCashDB(userId, cash, cashcount);
	//end 20151116
}

//mid_cash add by hxw 20151116 写入代币到数据库
//写入代币到角色数据库
INT RefreshDBMan::UpdateCashToActorDB(UINT32 roleId, UINT32 cash, UINT32 cashcount)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	if (cashcount == -1)
		sprintf(szSQL, "update actor set cash=%u where id=%u", cash, roleId);
	else
		sprintf(szSQL, "update actor set cash=%u, cashcount=%u where id=%u", cash, cashcount, roleId);
	return GetDBSClient()->Update(GetConfigFileMan()->GetCashDBName(), szSQL);
}

INT RefreshDBMan::UpdateCashToCashDB(UINT32 userID, UINT32 cash, UINT32 cashcount)
{
	return GetCashDBMan()->SetCash(userID, cash, cashcount);
}

void RefreshDBMan::OnUpdateRoleDress(UINT32 userId, UINT32 roleId)
{
	UINT16 dressId = 0;
	std::vector<UINT16> dressVec;

	int retCode = GetRolesInfoMan()->GetRoleWearDressId(userId, roleId, dressId, &dressVec);
	if (retCode != MLS_OK)
		return;

	char szSQL[MAX_SQL_BUF_LEN] = { 0 };
	strcpy(szSQL, "update data set dress='");

	int n = (int)strlen(szSQL);
	UINT16 si = (UINT16)min(MAX_HAS_DRESS_NUM, dressVec.size());
	n += DBS_EscapeString(szSQL + n, (const char*)&si, sizeof(UINT16));
	if (si > 0)
		DBS_EscapeString(szSQL + n, (const char*)dressVec.data(), si * sizeof(UINT16));

	char szId[64] = { 0 };
	sprintf(szId, "' where actorid=%u", roleId);
	strcat(szSQL, szId);
	GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
}

void RefreshDBMan::OnUpdateRoleDressWear(UINT32 userId, UINT32 roleId)
{
#if 0
	UINT16 dressId = 0;
	std::vector<UINT16> dressVec;
	int retCode = GetRolesInfoMan()->GetRoleWearDressId(userId, roleId, dressId, &dressVec);
	if (retCode != MLS_OK)
		return;

	char szSQL[MAX_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "update actor set dressid=%d where id=%u", dressId, roleId);
	GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);

	memset(szSQL, 0, sizeof(szSQL));
	strcpy(szSQL, "update data set dress='");

	int n = (int)strlen(szSQL);
	UINT16 si = min(MAX_HAS_DRESS_NUM, dressVec.size());
	n += DBS_EscapeString(szSQL + n, (const char*)&si, sizeof(UINT16));
	if (si > 0)
		DBS_EscapeString(szSQL + n, (const char*)dressVec.data(), si * sizeof(UINT16));

	char szId[64] = { 0 };
	sprintf(szId, "' where actorid=%u", roleId);
	strcat(szSQL, szId);
	GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
#else
	UINT16 dressId = 0;
	char szSQL[GENERIC_SQL_BUF_LEN/*MAX_SQL_BUF_LEN*/] = { 0 };
	int retCode = GetRolesInfoMan()->GetRoleWearDressId(userId, roleId, dressId, NULL);
	if (retCode != MLS_OK)
		return;

	UINT32 cp = 0;
	retCode = GetRolesInfoMan()->GetRoleCompatPower(userId, roleId, cp);
	if (retCode != MLS_OK)
	{
		sprintf(szSQL, "update actor set dressid=%d where id=%u", dressId, roleId);
		GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
	}
	else
	{
		sprintf(szSQL, "update actor set dressid=%d, cp=%u where id=%u", dressId, cp, roleId);
		GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
	}
#endif
}

void RefreshDBMan::OnUpdateRoleFragment(UINT32 userId, UINT32 roleId)
{
	std::vector<SEquipFragmentAttr> fragmentVec;
	int retCode = GetRolesInfoMan()->GetRoleEquipFragment(userId, roleId, fragmentVec);
	if (retCode != MLS_OK)
		return;

	char szSQL[MAX_SQL_BUF_LEN] = { 0 };
	strcpy(szSQL, "update data set fragment='");

	int n = (int)strlen(szSQL);
	BYTE si = (BYTE)fragmentVec.size();
	n += DBS_EscapeString(szSQL + n, (const char*)&si, sizeof(BYTE));
	if (si > 0)
		DBS_EscapeString(szSQL + n, (const char*)fragmentVec.data(), si * sizeof(SEquipFragmentAttr));

	char szId[64] = { 0 };
	sprintf(szId, "' where actorid=%u", roleId);
	strcat(szSQL, szId);
	GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
}

void RefreshDBMan::OnUpdateRoleGoldExchange(UINT32 userId, UINT32 roleId)
{
	UINT64 gold = 0;
	UINT32 cash = 0;
	UINT32 cashcount = 0;
	int retCode = GetRolesInfoMan()->GetRoleMoney(userId, roleId, gold, cash, cashcount);
	if (retCode != MLS_OK)
		return;

	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };

	//mid_cash by hxw 20151116
	//如果代币是存储在角色的保存类型
	if (GetConfigFileMan()->GetCashSaveType() == CASH_SAVE_TYPE_TO_ACTOR)
	{
		sprintf(szSQL, "update actor set gold=%llu, cash=%u where id=%u", gold, cash, roleId);
		GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
	}
	else
	{
		//mid add 
		memset(szSQL, 0, sizeof(szSQL));
		sprintf(szSQL, "update actor set gold=%llu where id=%u", gold, cash, roleId);
		GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);

		UpdateCashToCashDB(userId, cash);
		//end 20151116
	}
}

void RefreshDBMan::OnUpdateRoleFinInst(UINT32 userId, UINT32 roleId)
{
	//刷新背包
	OnUpdateRoleBackpack(userId, roleId);

	SRoleInfos roleInfo;
	int retCode = GetRolesInfoMan()->GetRoleInfo(userId, roleId, &roleInfo);
	if (retCode != MLS_OK)
		return;

	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };

	//mid_cash by hxw 20151117
	//如果代币在
	if (GetConfigFileMan()->GetCashSaveType() == CASH_SAVE_TYPE_TO_ACTOR)
	{
		sprintf(szSQL, "update actor set level=%d, exp=%llu, gold=%llu, cash=%u, vit=%u, hp=%u, mp=%u, attack=%u, def=%u where id=%u",
			roleInfo.level, BigNumberToU64(roleInfo.exp), BigNumberToU64(roleInfo.gold),
			roleInfo.cash, roleInfo.vit, roleInfo.hp, roleInfo.mp, roleInfo.attack, roleInfo.def, roleId);
		GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
	}
	else
	{
		sprintf(szSQL, "update actor set level=%d, exp=%llu, gold=%llu, vit=%u, hp=%u, mp=%u, attack=%u, def=%u where id=%u",
			roleInfo.level, BigNumberToU64(roleInfo.exp), BigNumberToU64(roleInfo.gold),
			roleInfo.vit, roleInfo.hp, roleInfo.mp, roleInfo.attack, roleInfo.def, roleId);
		GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);

		UpdateCashToCashDB(userId, roleInfo.cash);
	}

	//end 20151117


}

void RefreshDBMan::OnUpdateRoleExp(UINT32 userId, UINT32 roleId)
{
	SBigNumber exp;
	int retCode = GetRolesInfoMan()->GetRoleExp(userId, roleId, exp);
	if (retCode != MLS_OK)
		return;

	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "update actor set exp=%llu where id=%u", BigNumberToU64(exp), roleId);
	GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
}

void RefreshDBMan::OnUpdateRoleEquipPos(UINT32 userId, UINT32 roleId)
{
	std::vector<SEquipPosAttr> equipPosVec;
	int retCode = GetRolesInfoMan()->GetRoleEquipPosAttr(userId, roleId, equipPosVec);
	if (retCode != MLS_OK)
		return;

	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	strcpy(szSQL, "update data set equipsock='");

	int n = (int)strlen(szSQL);
	BYTE si = (BYTE)equipPosVec.size();
	n += DBS_EscapeString(szSQL + n, (const char*)&si, sizeof(BYTE));
	if (si > 0)
		DBS_EscapeString(szSQL + n, (const char*)equipPosVec.data(), si * sizeof(SEquipPosAttr));

	char szId[64] = { 0 };
	sprintf(szId, "' where actorid=%u", roleId);
	strcat(szSQL, szId);
	GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
}

/* zpy 成就系统新增 */
void RefreshDBMan::OnUpdateRoleKillMonster(UINT32 userId, UINT32 roleId)
{
	AchievementMan::SAchievementRecord record;
	if (MLS_OK == GetAchievementMan()->GetAchievementRecord(userId, roleId, record))
	{
		char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
		sprintf(szSQL, "UPDATE achievement_total SET common=%u,cream=%u,boss=%u WHERE actorid=%u", record.kill_common, record.kill_cream, record.kill_boss, roleId);
		GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
	}
}

/* zpy 成就系统新增 */
void RefreshDBMan::OnUpdateRolePickupBoxSum(UINT32 userId, UINT32 roleId)
{
	AchievementMan::SAchievementRecord record;
	if (MLS_OK == GetAchievementMan()->GetAchievementRecord(userId, roleId, record))
	{
		UINT32 affectRows = 0;
		char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
		sprintf(szSQL, "UPDATE achievement_total SET boxsum=%u WHERE actorid=%u", record.pickup_box_sum, roleId);
		GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL, &affectRows);
		if (affectRows == 0)
		{
			sprintf(szSQL, "INSERT INTO achievement_total(actorid,boxsum) VALUES(%u,%u)", roleId, record.pickup_box_sum);
			GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);
		}
	}
}

/* zpy 成就系统新增 */
void RefreshDBMan::OnUpdateRoleCombatPerformance(UINT32 userId, UINT32 roleId)
{
	AchievementMan::SAchievementRecord record;
	if (MLS_OK == GetAchievementMan()->GetAchievementRecord(userId, roleId, record))
	{
		UINT32 affectRows = 0;
		char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
		sprintf(szSQL, "UPDATE achievement_total SET continuous=%u,speed_evaluation=%u,hurt_evaluation=%u,star_evaluation=%u WHERE actorid=%u",
			record.continuous, record.speed_evaluation, record.hurt_evaluation, record.star_evaluation, roleId);
		GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL, &affectRows);
		if (affectRows == 0)
		{
			sprintf(szSQL, "INSERT INTO achievement_total(actorid,continuous,speed_evaluation,hurt_evaluation,star_evaluation) VALUES(%u,%u,%u,%u,%u)",
				roleId, record.continuous, record.speed_evaluation, record.hurt_evaluation, record.star_evaluation);
			GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);
		}
	}
}

/* zpy 炼魂系统新增 */
void RefreshDBMan::OnUpdateRoleChainSoulFragment(UINT32 userId, UINT32 roleId)
{
	std::vector<SChainSoulFragmentAttr> fragmentVec;
	int retCode = GetRolesInfoMan()->GetRoleChainSoulFragment(userId, roleId, fragmentVec);
	if (retCode != MLS_OK)
		return;

	char szSQL[MAX_SQL_BUF_LEN] = { 0 };
	strcpy(szSQL, "update data set lh_fragment='");

	int n = (int)strlen(szSQL);
	BYTE si = (BYTE)fragmentVec.size();
	n += DBS_EscapeString(szSQL + n, (const char*)&si, sizeof(BYTE));
	if (si > 0)
		DBS_EscapeString(szSQL + n, (const char*)fragmentVec.data(), si * sizeof(SChainSoulFragmentAttr));

	char szId[64] = { 0 };
	sprintf(szId, "' where actorid=%u", roleId);
	strcat(szSQL, szId);
	GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
}

/* zpy 炼魂系统新增 */
void RefreshDBMan::OnUpdateRoleChainSoulPos(UINT32 userId, UINT32 roleId)
{
	std::vector<SChainSoulPosAttr> posVec;
	int retCode = GetRolesInfoMan()->GetRoleChainSoulPos(userId, roleId, posVec);
	if (retCode != MLS_OK)
		return;

	char szSQL[MAX_SQL_BUF_LEN] = { 0 };
	strcpy(szSQL, "update data set lh_pos='");

	int n = (int)strlen(szSQL);
	BYTE si = (BYTE)posVec.size();
	n += DBS_EscapeString(szSQL + n, (const char*)&si, sizeof(BYTE));
	if (si > 0)
		DBS_EscapeString(szSQL + n, (const char*)posVec.data(), si * sizeof(SChainSoulPosAttr));

	char szId[64] = { 0 };
	sprintf(szId, "' where actorid=%u", roleId);
	strcat(szSQL, szId);
	GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
}

/* zpy 天梯系统新增 */
void RefreshDBMan::OnUpdateLadderInfo(UINT32 userId, UINT32 roleId)
{
	// 获取玩家信息
	SRoleInfos role_info;
	int retCode = GetRolesInfoMan()->GetRoleInfo(userId, roleId, &role_info);
	if (MLS_OK != retCode) return;

	UINT32 affectRows = 0;
	char szDate[32] = { 0 };
	char szSQL[MAX_SQL_BUF_LEN] = { 0 };
	MselToLocalStrTime(role_info.ladder_last_finish_time, szDate);
	sprintf(szSQL, "UPDATE ladder SET accountid=%u,win_in_row=%u,refresh_count=%u,eliminate_count=%u,challenge_count=%u,last_finish_time='%s' WHERE actorid=%u",
		userId,
		role_info.ladder_win_in_row,
		role_info.ladder_refresh_count,
		role_info.ladder_eliminate_count,
		role_info.ladder_challenge_count,
		szDate,
		roleId);
	GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL, &affectRows);
	if (affectRows == 0)
	{
		sprintf(szSQL, "INSERT INTO ladder(actorid,accountid,win_in_row,refresh_count,eliminate_count,challenge_count,last_finish_time) VALUES(%u, %u, %u, %u, %u, %u, '%s')",
			roleId,
			userId,
			role_info.ladder_win_in_row,
			role_info.ladder_refresh_count,
			role_info.ladder_eliminate_count,
			role_info.ladder_challenge_count,
			szDate);
		GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);
	}
}

void RefreshDBMan::OnUpdateLadderMatchingInfo(UINT32 userId, UINT32 roleId)
{
	// 获取玩家信息
	SRoleInfos role_info;
	int retCode = GetRolesInfoMan()->GetRoleInfo(userId, roleId, &role_info);
	if (MLS_OK != retCode) return;

	char szSQL[MAX_SQL_BUF_LEN] = { 0 };
	strcpy(szSQL, "UPDATE ladder SET user_data='");

	int n = (int)strlen(szSQL);
	BYTE size = LADDER_MATCHTING_MAX_NUM;
	n += DBS_EscapeString(szSQL + n, (const char*)&size, sizeof(BYTE));
	if (size > 0)
		DBS_EscapeString(szSQL + n, (const char*)role_info.ladder_matching_rank, size * sizeof(UINT32));

	char szId[64] = { 0 };
	UINT32 affectRows = 0;
	sprintf(szId, "' where actorid=%u", roleId);
	strcat(szSQL, szId);
	GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL, &affectRows);
	if (affectRows == 0)
	{
		char szDate[32] = { 0 };
		MselToLocalStrTime(role_info.ladder_last_finish_time, szDate);
		sprintf(szSQL, "INSERT INTO ladder(actorid,accountid,win_in_row,refresh_count,eliminate_count,challenge_count,last_finish_time,user_data) VALUES(%u, %u, %u, %u, %u, %u, '%s', '",
			roleId,
			userId,
			role_info.ladder_win_in_row,
			role_info.ladder_refresh_count,
			role_info.ladder_eliminate_count,
			role_info.ladder_challenge_count,
			szDate);
		n = (int)strlen(szSQL);
		n += DBS_EscapeString(szSQL + n, (const char*)&size, sizeof(BYTE));
		if (size > 0)
			DBS_EscapeString(szSQL + n, (const char*)role_info.ladder_matching_rank, size * sizeof(UINT32));
		strcat(szSQL, "')");
		GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);
	}
}

void RefreshDBMan::OnResetLadderView(UINT32 userId, UINT32 roleId)
{
	char szSQL[MAX_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "UPDATE ladder SET user_data=NULL WHERE actorid=%u AND accountid=%u", roleId, userId);
	GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
}