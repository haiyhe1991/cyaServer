#include <sstream>
#include "RolesInfoMan.h"
#include "DBSClient.h"
#include "LuaScriptMan.h"
#include "ConfigFileMan.h"
#include "GameCfgFileMan.h"
#include "cyaLog.h"
#include "cyaMaxMin.h"
#include "EmailCleanMan.h"
#include "GuildMan.h"
#include "RechargeMan.h"
#include "SystemNoticeMan.h"
#include "GoldExchangeMan.h"
#include "FriendMan.h"
#include "PermanenDataMan.h"
#include "stringHash.h"
#include "OnlineSingnInMan.h"
#include "ConsumeProduceMan.h"

#include "AchievementMan.h"
#include "VitExchangeMan.h"
#include "LadderRankMan.h"

#include "RankRewardMan.h"
#include "CashDBMan.h"

#include "LogicLog.h"

static RolesInfoMan* sg_rolesMan = NULL;



#define CREAT_GENERAL_ROLES_MAN_FUN(funname)	\
	void RolesInfoMan::funname##()	{ \
	CXTAutoLock lock(m_userRolesMapLocker);	\
	std::map<UINT32/*userId*/, RoleSite*>::iterator it = m_userRolesMap.begin();	\
for (; it != m_userRolesMap.end(); ++it)	it->second->funname##();	\
	}



void InitRolesInfoMan()
{
	ASSERT(sg_rolesMan == NULL);
	sg_rolesMan = new RolesInfoMan();
	ASSERT(sg_rolesMan != NULL);
}

RolesInfoMan* GetRolesInfoMan()
{
	return sg_rolesMan;
}

void DestroyRolesInfoMan()
{
	RolesInfoMan* rolesMan = sg_rolesMan;
	sg_rolesMan = NULL;
	if (rolesMan != NULL)
		delete rolesMan;
}

RolesInfoMan::RolesInfoMan()
: m_totalRoles(0)
#if USE_MEM_POOL
, m_roleSitePool(sizeof(RoleSite), 64 * 1024, 1024)
#endif
{
	DBS_RESULT result = NULL;
	int ret = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), "select count(id) from actor", result);
	if (ret != MLS_OK)
	{
		ASSERT(false);
		return;
	}
	DBS_ROW row = DBS_FetchRow(result);
	ASSERT(row != NULL);
	m_totalRoles = _atoi64(row[0]);
	DBS_FreeResult(result);
	LogInfo(("该分区共有%u个角色", m_totalRoles));
}

RolesInfoMan::~RolesInfoMan()
{
	CXTAutoLock lock(m_userRolesMapLocker);
	std::map<UINT32/*userId*/, RoleSite*>::iterator it = m_userRolesMap.begin();
	for (; it != m_userRolesMap.end(); ++it)
		it->second->ReleaseRef();
	m_userRolesMap.clear();
}

//查询拥有的角色数量 add by hxw 20151210
int RolesInfoMan::QueryUserRolesNum(UINT32 userId)
{
	RoleSite* pSite = FetchRoleSite(userId);
	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->QueryUserRolesNum();
}

int RolesInfoMan::QueryUserRoles(UINT32 userId, SQueryRolesRes* pRoles)
{
	RoleSite* pSite = FetchRoleSite(userId);
	ASSERT(pSite != NULL);

#if 0
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->QueryUserRoles(userId, pRoles);
#else
	int retCode = MLS_OK;
	{
		AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
		retCode = pAutoObj->QueryUserRoles(userId, pRoles);
		if (retCode == MLS_OK)
			return retCode;
	}

	RemoveUserRoles(userId);
	return retCode;
#endif
}

int RolesInfoMan::QueryRoleAppearance(UINT32 userId, UINT32 roleId, SQueryRoleAppearanceRes* pRoleRes)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
	{
		int retCode = QueryUserRoles(userId, NULL);
		if (retCode != MLS_OK)
			return retCode;

		pSite = GetRoleSite(userId);
		if (pSite == NULL)
			return MLS_ACCOUNT_NOT_EXIST;
	}
	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->QueryRoleAppearance(roleId, pRoleRes);
}

int RolesInfoMan::QueryRoleExplicit(UINT32 userId, UINT32 roleId, SQueryRoleExplicitRes* pRoleRes)
{
	int retCode = MLS_OK;
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
	{
		retCode = QueryUserRoles(userId, NULL);
		if (retCode != MLS_OK)
			return retCode;
		pSite = GetRoleSite(userId);
		if (pSite == NULL)
			return MLS_ACCOUNT_NOT_EXIST;
	}

	{
		ASSERT(pSite != NULL);
		AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
		retCode = pAutoObj->QueryRoleExplicit(roleId, pRoleRes);
		if (retCode != MLS_OK)
			return retCode;
	}

	pRoleRes->unReadEmails = GetEmailCleanMan()->GetRoleUnReadEmails(roleId);

	if (pRoleRes->guildId <= 0)
		return retCode;
	SQueryGuildNameRes guildNameRes;
	retCode = GetGuildMan()->QueryGuildName(pRoleRes->guildId, &guildNameRes);
	if (retCode == MLS_OK)
		strcpy(pRoleRes->guildNick, guildNameRes.guildName);
	return MLS_OK;
}

BOOL RolesInfoMan::IsUserRoleOnline(UINT32 userId, UINT32 roleId)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return false;
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->IsRoleOnline(roleId);
}

BOOL RolesInfoMan::IsUserOnline(UINT32 userId)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return false;
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->IsRoleOnline(0);
}

int RolesInfoMan::GetUserLinkerId(UINT32 userId, BYTE& linkerId)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	linkerId = pAutoObj->GetLinkerId();
	return MLS_OK;
}

int RolesInfoMan::SetUserLinkerId(UINT32 userId, BYTE linkerId)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	pAutoObj->SetLinkerId(linkerId);
	return MLS_OK;
}

int RolesInfoMan::UserEnterRole(UINT32 userId, UINT32 roleId, BYTE linkerId, SEnterRoleRes* pEnterRes)
{
	int retCode = MLS_OK;
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
	{
		retCode = QueryUserRoles(userId, NULL);
		if (retCode != MLS_OK)
			return retCode;

		pSite = GetRoleSite(userId);
		if (pSite == NULL)
			return MLS_ACCOUNT_NOT_EXIST;
	}

	{
		ASSERT(pSite != NULL);
		AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
		retCode = pAutoObj->EnterRole(userId, roleId, pEnterRes);
		if (retCode != MLS_OK)
			return retCode;

		pAutoObj->SetLinkerId(linkerId);
	}
	pEnterRes->unReadEmails = GetEmailCleanMan()->GetRoleUnReadEmails(roleId);
	return retCode;
}

int RolesInfoMan::UserLeaveRole(UINT32 userId, UINT32 roleId)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	pAutoObj->SetLinkerId(0);
	return pAutoObj->LeaveRole(roleId);
}

int RolesInfoMan::QueryBackpack(UINT32 userId, UINT32 roleId, BYTE from, BYTE nums, SQueryBackpackRes* pBackpackRes)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
	{
		int retCode = QueryUserRoles(userId, NULL);
		if (retCode != MLS_OK)
			return retCode;

		pSite = GetRoleSite(userId);
		if (pSite == NULL)
			return MLS_ACCOUNT_NOT_EXIST;
	}

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->QueryBackpack(roleId, from, nums, pBackpackRes);
}

int RolesInfoMan::CleanupBackpack(UINT32 userId, UINT32 roleId, BOOL& isCleanup)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->CleanupBackpack(roleId, isCleanup);
}

//end 

int RolesInfoMan::CreateRole(UINT32 userId, BYTE occuId, BYTE sex, const char* roleNick, SCreateRoleRes* pCreateRes)
{
#if 0
	RoleSite* pSite = FetchRoleSite(userId);
	ASSERT(pSite != NULL);
#else
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
	{
		int retCode = QueryUserRoles(userId, NULL);
		if (retCode != MLS_OK)
			return retCode;

		pSite = GetRoleSite(userId);
		if (pSite == NULL)
			return MLS_ACCOUNT_NOT_EXIST;
	}
#endif
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	int retCode = pAutoObj->CreateRole(userId, occuId, sex, roleNick, pCreateRes);
	if (retCode == MLS_OK)
		m_totalRoles++;
	return retCode;
}

int RolesInfoMan::DropBackpackProperty(UINT32 userId, UINT32 roleId, UINT32 id, BYTE num)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->DropBackpackProperty(roleId, id, num);
}

int RolesInfoMan::DropTask(UINT32 userId, UINT32 roleId, UINT16 taskId)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->DropTask(roleId, taskId);
}

int RolesInfoMan::CommitTaskFinish(UINT32 userId, UINT32 roleId, UINT16 taskId, SFinishTaskFailNotify* pFinTaskNotify)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->CommitTaskFinish(roleId, taskId, pFinTaskNotify);
}

int RolesInfoMan::BuyStorageSpace(UINT32 userId, UINT32 roleId, BYTE buyType, BYTE num, UINT32& spend, BYTE& newNum)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->BuyStorageSpace(roleId, buyType, num, spend, newNum);
}

int RolesInfoMan::SellProperty(UINT32 userId, UINT32 roleId, UINT32 id, BYTE num, SSellPropertyRes* pSellRes, SPropertyAttr& propAttr)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->SellProperty(roleId, id, num, pSellRes, propAttr);
}

int RolesInfoMan::BuyBackProperty(UINT32 userId, UINT32 roleId, SPropertyAttr& buyProp, BYTE num, SPropBuyBackRes* pBuyBackRes)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->BuyBackProperty(roleId, buyProp, num, pBuyBackRes);
}

int RolesInfoMan::AcceptTask(UINT32 userId, UINT32 roleId, UINT16 taskId)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->AcceptTask(roleId, taskId);
}

int RolesInfoMan::QueryAcceptedTasks(UINT32 userId, UINT32 roleId, SQueryAcceptTasksRes* pTaskRes)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->QueryAcceptedTasks(roleId, pTaskRes);
}


//BY add hxw 20151016
//获取排行通用奖励
int RolesInfoMan::ReceiveRankReward(UINT32 userId, UINT32 roleId, BYTE type, UINT16 rewardid, SGetRankRewardRes *pRoleSign, SReceiveRewardRefresh *pRefresh)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->ReceiveRankReward(userId, roleId, type, rewardid, pRoleSign, pRefresh);
}

//add by hxw 20151022

BOOL RolesInfoMan::IsGetStrongerReward(UINT32 userId, UINT32 roleId, UINT16 rewardid)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->IsGetStrongerReward(roleId, rewardid);
}

BOOL RolesInfoMan::IsGetOnlineReward(UINT32 userId, UINT32 roleId, UINT16 rewardid)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->IsGetOnlineReward(roleId, rewardid);
}

BOOL RolesInfoMan::GetOnlineInfo(UINT32 userId, UINT32 roleId, SOnlineInfo* info, std::vector<UINT16>& vonline)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GetOnlineInfo(roleId, info, vonline);
}
BOOL RolesInfoMan::GetStrongerInfo(UINT32 userId, UINT32 roleId, std::vector<UINT16>& infov)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GetStrongerInfo(roleId, infov);
}

int RolesInfoMan::InputStrongerReward(UINT32 userId, UINT32 roleId, UINT16 rewardid)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->InputStrongerReward(roleId, rewardid);
}
int RolesInfoMan::InputOnlineReward(UINT32 userId, UINT32 roleId, UINT16 rewardid)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->InputOnlineReward(roleId, rewardid);
}
int RolesInfoMan::SetOnlineInfo(UINT32 userId, UINT32 roleId, UINT32 id, UINT32 time, LTMSEL lastTime)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->SetOnlineInfo(roleId, id, time, lastTime);
}
//end


int RolesInfoMan::LoadEquipment(UINT32 userId, UINT32 roleId, UINT32 id, BYTE pos)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->LoadEquipment(roleId, id, pos);
}

int RolesInfoMan::UnLoadEquipment(UINT32 userId, UINT32 roleId, UINT32 id)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->UnLoadEquipment(roleId, id);
}

int RolesInfoMan::UseProperty(UINT32 userId, UINT32 roleId, UINT32 id, UINT16& propId, SUsePropertyRes *pUseProperty)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->UseProperty(roleId, id, propId, pUseProperty);
}

int RolesInfoMan::DelRole(UINT32 userId, UINT32 roleId, SDelRoleRes *pDelRes)
{
	/**
	* 2016.1.5新增
	* 策划张鹏要求角色必须要创建数日后才能删除
	*/
	pDelRes->success = true;
	pDelRes->roleId = roleId;
	pDelRes->remainder_day = 0;
	DBS_RESULT result = NULL;
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "SELECT DATEDIFF(CURRENT_DATE(),createtts) FROM actor WHERE id=%u", roleId);
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;

	DBS_ROWLENGTH row_bytes = 0;
	DBS_ROW row_data = DBS_FetchRow(result, &row_bytes);
	if (row_data != NULL && row_bytes[0] > 0)
	{
		int interval = atoi(row_data[0]);
		pDelRes->remainder_day = interval >= GetGameCfgFileMan()->GetDelRoleTimeLimit() ? 0 : GetGameCfgFileMan()->GetDelRoleTimeLimit() - interval;
		if (pDelRes->remainder_day > 0)
		{
			pDelRes->success = false;
			DBS_FreeResult(result);
			return MLS_OK;
		}
	}
	else
	{
		return MLS_ROLE_NOT_EXIST;
	}
	DBS_FreeResult(result);

	RoleSite* pSite = GetRoleSite(userId);
	if (pSite != NULL)
	{
		AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
		retCode = pAutoObj->DelRole(roleId);
		if (retCode != MLS_OK)
		{
			return retCode;
		}
	}

	UINT32 delRows = 0;
	sprintf(szSQL, "delete from actor where id=%u and accountid=%u", roleId, userId);
	retCode = GetDBSClient()->Del(GetConfigFileMan()->GetDBName(), szSQL, &delRows);
	if (retCode != DBS_OK)
	{
		return retCode;
	}

	if (delRows > 0)
	{
		m_totalRoles--;
		return MLS_OK;
	}
	else
	{
		return MLS_ROLE_NOT_EXIST;
	}
}

//zpy 查询所有角色的ID
int RolesInfoMan::QueryAllRoles(UINT32 userId, SQueryAllRolesRes* pAllRoles)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->QueryAllRoles(userId, pAllRoles);
}

int RolesInfoMan::EquipPosInlaidGem(UINT32 userId, UINT32 roleId, BYTE pos, UINT32 gemId, BYTE holeSeq)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->EquipPosInlaidGem(roleId, pos, gemId, holeSeq);
}

int RolesInfoMan::EquipPosRemoveGem(UINT32 userId, UINT32 roleId, BYTE pos, BYTE holeSeq, SEquipPosRemoveGemRes* pRemoveRes)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->EquipPosRemoveGem(roleId, pos, holeSeq, pRemoveRes);
}

int	RolesInfoMan::GemCompose(UINT32 userId, UINT32 roleId, BYTE num, const SConsumeProperty* pMaterial, SGemComposeRes* pComposeRes)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GemCompose(roleId, num, pMaterial, pComposeRes);
}

int RolesInfoMan::EquipmentCompose(UINT32 userId, UINT32 roleId, UINT16 composeId, SEquipComposeRes* pComposeRes)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->EquipmentCompose(roleId, composeId, pComposeRes);
}

void RolesInfoMan::RemoveUserRoles(UINT32 userId)
{
	RoleSite* site = NULL;
	{
		CXTAutoLock lock(m_userRolesMapLocker);
		std::map<UINT32/*userId*/, RoleSite*>::iterator it = m_userRolesMap.find(userId);
		if (it == m_userRolesMap.end())
			return;
		site = it->second;

		/* zpy 成就系统新增 */
		site->RemoveAllRole();

		m_userRolesMap.erase(it);
	}
	ASSERT(site != NULL);
	site->ReleaseRef();
}

int RolesInfoMan::EnterInst(UINT32 userId, UINT32 roleId, UINT16 instId, SEnterInstRes* pEnterInstRes, int& dataLen, LTMSEL enterTTS, PlayerActionMan::SEnterInstItem& enterInstItem)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->EnterInst(roleId, instId, pEnterInstRes, dataLen, enterTTS, enterInstItem);
}

int RolesInfoMan::FetchInstAreaMonsterDrops(UINT32 userId, UINT32 roleId, UINT16 instId, BYTE areaId, SFetchInstAreaDropsRes* pAreaDropsRes)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->FetchInstAreaMonsterDrops(roleId, instId, areaId, pAreaDropsRes);
}

int RolesInfoMan::FinishInst(UINT32 userId, UINT32 roleId, UINT16 instId, UINT32 score, BYTE star, LTMSEL& enterTTS, const std::vector<SPropertyItem> &rewardVec, SFinishInstRes* pFinishInstRes, SReceiveRewardRefresh *pRefresh)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->FinishInst(userId, roleId, instId, score, star, enterTTS, rewardVec, pFinishInstRes, pRefresh);
}

int RolesInfoMan::PickupInstMonsterDrop(UINT32 userId, UINT32 roleId, UINT16 instId, BYTE areaId, BYTE dropId, SPickupInstDropNotify* pNotify)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->PickupInstMonsterDrop(userId, roleId, instId, areaId, dropId, pNotify);
}

int RolesInfoMan::PickupInstBrokenDrop(UINT32 userId, UINT32 roleId, UINT16 instId, BYTE dropId, SPickupInstDropNotify* pNotify)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->PickupInstBrokenDrop(userId, roleId, instId, dropId, pNotify);
}

int RolesInfoMan::PickupInstBoxDrop(UINT32 userId, UINT32 roleId, UINT16 instId, BYTE boxId, BYTE id, SPickupInstDropNotify* pNotify)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->PickupInstBoxDrop(userId, roleId, instId, boxId, id, pNotify);
}

//杀怪得经验
int RolesInfoMan::KillMonsterExp(UINT32 userId, UINT32 roleId, UINT16 instId, BYTE areaId, UINT16 monsterId, BYTE monsterLv, BOOL& isUpgrade)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->KillMonsterExp(roleId, instId, areaId, monsterId, monsterLv, isUpgrade);
}

/* zpy 成就系统新增 */
int RolesInfoMan::GetMaxGemLevel(UINT32 userId, UINT32 roleId, BYTE &max_level)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GetMaxGemLevel(roleId, max_level);
}

/* zpy 成就系统新增 */
int RolesInfoMan::GetInlaidGemLevelSum(UINT32 userId, UINT32 roleId, UINT32 &sum)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GetInlaidGemLevelSum(roleId, sum);
}

/* zpy 成就系统新增 */
int RolesInfoMan::GetEquipStarLevelSum(UINT32 userId, UINT32 roleId, UINT32 &sum)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GetEquipStarLevelSum(roleId, sum);
}

/* zpy 成就系统新增 */
int RolesInfoMan::GetSkillLevelSum(UINT32 userId, UINT32 roleId, UINT32 &sum)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GetSkillLevelSum(roleId, sum);
}

int RolesInfoMan::CashProperty(UINT32 userId, UINT32 roleId, UINT16 cashId, SCashPropertyRes* pCashRes)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->CashProperty(roleId, cashId, pCashRes);
}

int RolesInfoMan::QueryRoleSkills(UINT32 userId, UINT32 roleId, SQuerySkillsRes* pSkills)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->QueryRoleSkills(roleId, pSkills);
}

int RolesInfoMan::SkillUpgrade(UINT32 userId, UINT32 roleId, UINT16 skillId, BYTE step, SSkillUpgradeRes* pUpgradeRes)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->SkillUpgrade(roleId, skillId, step, pUpgradeRes);
}

int RolesInfoMan::ChangeFaction(UINT32 userId, UINT32 roleId, BYTE factionId, BOOL& needSyncDB)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->ChangeFaction(roleId, factionId, needSyncDB);
}

int RolesInfoMan::LoadSkill(UINT32 userId, UINT32 roleId, UINT16 skillId, BYTE key, BOOL& needSyncDB)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->LoadSkill(roleId, skillId, key, needSyncDB);
}

int RolesInfoMan::CancelSkill(UINT32 userId, UINT32 roleId, UINT16 skillId, BOOL& needSyncDB)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->CancelSkill(roleId, skillId, needSyncDB);
}

int RolesInfoMan::GetDailyTasks(UINT32 userId, UINT32 roleId, SGetDailyTaskRes* pDailyTasks)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GetDailyTasks(roleId, pDailyTasks);
}

int RolesInfoMan::UpdateTaskTarget(UINT32 userId, UINT32 roleId, UINT16 taskId, const UINT32* pTarget, int n/* = MAX_TASK_TARGET_NUM*/)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->UpdateTaskTarget(roleId, taskId, pTarget, n);
}

int RolesInfoMan::ReceiveTaskReward(UINT32 userId, UINT32 roleId, UINT16 taskId, SReceiveTaskRewardRes* pTaskRewardRes)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->ReceiveTaskReward(userId, roleId, taskId, pTaskRewardRes);
}

int RolesInfoMan::ReceiveEmailAttachments(UINT32 userId, UINT32 roleId, BYTE emailType, const SPropertyAttr* pAttachmentsProp, BYTE attachmentsNum, SReceiveEmailAttachmentsRes* pEmailAttachmentsRes, BOOL& isUpgrade)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->ReceiveEmailAttachments(roleId, emailType, pAttachmentsProp, attachmentsNum, pEmailAttachmentsRes, isUpgrade);
}

int RolesInfoMan::RechargeToken(UINT32 userId, UINT32 roleId, UINT16 cashId, SRechargeRes* pRechargeRes, char* pszRoleNick)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->RechargeToken(roleId, cashId, pRechargeRes, pszRoleNick);
}

int RolesInfoMan::SetRoleTitleId(UINT32 userId, UINT32 roleId, UINT16 titleId)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->SetRoleTitleId(roleId, titleId);
}

int RolesInfoMan::MallBuyGoods(UINT32 userId, UINT32 roleId, UINT32 buyId, UINT16 num, SMallBuyGoodsRes* pBuyGoodsRes, char* pszRoleNick, UINT32& spendToken)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->MallBuyGoods(roleId, buyId, num, pBuyGoodsRes, pszRoleNick, spendToken);
}

int RolesInfoMan::QueryRoleDress(UINT32 userId, UINT32 roleId, SQueryRoleDressRes* pDressRes)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->QueryRoleDress(roleId, pDressRes);
}

int RolesInfoMan::WearDress(UINT32 userId, UINT32 roleId, UINT16 dressId)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->WearDress(roleId, dressId);
}

int RolesInfoMan::QueryEquipFragment(UINT32 userId, UINT32 roleId, SQueryEquipFragmentRes* pFragmentRes)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->QueryEquipFragment(roleId, pFragmentRes);
}

/* zpy 查询炼魂碎片 */
int RolesInfoMan::QueryChainSoulFragment(UINT32 userId, UINT32 roleId, SQueryChainSoulFragmentRes* pFragmentRes)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->QueryChainSoulFragment(roleId, pFragmentRes);
}

/* zpy 查询炼魂部件 */
int RolesInfoMan::QueryChainSoulPos(UINT32 userId, UINT32 roleId, SQueryChainSoulPosRes* pChainSoulPos)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->QueryChainSoulPos(roleId, pChainSoulPos);
}

/* zpy 升级炼魂部件 */
int RolesInfoMan::ChainSoulPosUpgrade(UINT32 userId, UINT32 roleId, BYTE pos, SChainSoulPosUpgradeRes* pChainSoulPosUpgrade)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->ChainSoulPosUpgrade(roleId, pos, pChainSoulPosUpgrade);
}

/* zpy 复活角色 */
int RolesInfoMan::ResurrectionInWorldBoss(UINT32 userId, UINT32 roleId, UINT16 reviveId, UINT32 consume_count, UINT32 &out_rpcoin)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->ResurrectionInWorldBoss(roleId, reviveId, consume_count, out_rpcoin);
}

int RolesInfoMan::RoleRevive(UINT32 userId, UINT32 roleId, UINT16 reviveId, SReviveRoleRes* pReviveRes)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->RoleRevive(roleId, reviveId, pReviveRes);
}

int RolesInfoMan::QueryGoldExchange(UINT32 userId, UINT32 roleId, SQueryGoldExchangeRes* pExchangeRes)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->QueryGoldExchange(roleId, pExchangeRes);
}

int RolesInfoMan::GoldExchange(UINT32 userId, UINT32 roleId, SGoldExchangeRes* pExchangeRes)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GoldExchange(roleId, pExchangeRes);
}

/* zpy 体力兑换 */
int RolesInfoMan::QueryVitExchange(UINT32 userId, UINT32 roleId, SQueryVitExchangeRes* pExchangeRes)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->QueryVitExchange(roleId, pExchangeRes);
}

/* zpy 体力兑换 */
int RolesInfoMan::VitExchange(UINT32 userId, UINT32 roleId, SVitExchangeRes* pExchangeRes)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->VitExchange(roleId, pExchangeRes);
}

int RolesInfoMan::GetRecommendFriends(UINT32 userId, UINT32 roleId, SGetRecommendFriendsRes* pFriendsRes)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GetRecommendFriends(userId, roleId, pFriendsRes);
}

int RolesInfoMan::ReceiveVIPReward(UINT32 userId, UINT32 roleId, BYTE vipLv, SReceiveVIPRewardRes* pVIPRewardRes, SReceiveRewardRefresh* pRefresh)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->ReceiveVIPReward(userId, roleId, vipLv, pVIPRewardRes, pRefresh);
}

/* zpy 成就系统新增 */
int RolesInfoMan::ReceiveAchievementReward(UINT32 userId, UINT32 roleId, UINT32 achievement_id, SReceiveRewardRes *pAchievementReward, SReceiveRewardRefresh *pRefresh)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->ReceiveAchievementReward(userId, roleId, achievement_id, pAchievementReward, pRefresh);
}

int RolesInfoMan::QueryRoleHelps(UINT32 userId, UINT32 roleId, SQueryFinishedHelpRes* pHelpRes)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->QueryRoleHelps(roleId, pHelpRes);
}

int RolesInfoMan::FinishRoleHelp(UINT32 userId, UINT32 roleId, UINT16 helpId)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->FinishRoleHelp(roleId, helpId);
}

int RolesInfoMan::QueryRoleMagics(UINT32 userId, UINT32 roleId, SQueryRoleMagicsRes* pMagicsRes)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->QueryRoleMagics(roleId, pMagicsRes);
}

int RolesInfoMan::GainMagic(UINT32 userId, UINT32 roleId, UINT16 instId, UINT16 magicId)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GainMagic(roleId, instId, magicId);
}

int RolesInfoMan::EquipPosStrengthen(UINT32 userId, UINT32 roleId, BYTE pos, SEquipPosStrengthenRes* pStrengthenRes)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->EquipPosStrengthen(roleId, pos, pStrengthenRes);
}

int RolesInfoMan::EquipPosRiseStar(UINT32 userId, UINT32 roleId, BYTE pos, const SRiseStarAttachParam& param, SEquipPosRiseStarRes* pRiseStarRes)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->EquipPosRiseStar(roleId, pos, param, pRiseStarRes);
}

int RolesInfoMan::QuerySignIn(UINT32 userId, UINT32 roleId, SQueryOnlineSignInRes* pSignInRes)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->QuerySignIn(roleId, pSignInRes);
}

int RolesInfoMan::SignIn(UINT32 userId, UINT32 roleId)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->SignIn(userId, roleId);
}

int RolesInfoMan::InstSweep(UINT32 userId, UINT32 roleId, UINT16 instId, SInstSweepRes* pSweepRes, BOOL& isUpgrade)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->InstSweep(userId, roleId, instId, pSweepRes, isUpgrade);
}

int RolesInfoMan::UnlockContent(UINT32 userId, UINT32 roleId, BYTE type, UINT16 unlockId)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->UnlockContent(roleId, type, unlockId);
}

int RolesInfoMan::ExtractRouletteReward(UINT32 userId, UINT32 roleId, const SRoulettePropItem& extractReward, SExtractRouletteRewardRes* pRewardRes, BOOL& isUpgrade)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->ExtractRouletteReward(userId, roleId, extractReward, pRewardRes, isUpgrade);
}

int RolesInfoMan::FetchRouletteBigReward(UINT32 userId, UINT32 roleId, const SRoulettePropItem& bigReward, SFetchRouletteBigRewardRes* pRewardRes, BOOL& isUpgrade)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->FetchRouletteBigReward(userId, roleId, bigReward, pRewardRes, isUpgrade);
}

int RolesInfoMan::UpdateRouletteBigReward(UINT32 userId, UINT32 roleId, UINT32 spendToken, UINT32& curToken)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->UpdateRouletteBigReward(roleId, spendToken, curToken);
}

RolesInfoMan::RoleSite::RoleSite()
: m_parent(NULL)
, m_linkerId(0)
{

}

RolesInfoMan::RoleSite::~RoleSite()
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*roleId*/, SRoleInfos*>::iterator it = m_rolesMap.begin();
	for (; it != m_rolesMap.end(); ++it)
		GetRoleInfosObjAllocator()->DeleteRoleInfosObj(it->second);
	m_rolesMap.clear();
}

void RolesInfoMan::RoleSite::DeleteObj()
{
	m_parent->DeleteRoleSite(this);
}

void RolesInfoMan::RoleSite::SetParent(RolesInfoMan* parent)
{
	m_parent = parent;
}

void RolesInfoMan::RoleSite::SetLinkerId(BYTE linkerId)
{
	m_linkerId = linkerId;
}

BYTE RolesInfoMan::RoleSite::GetLinkerId() const
{
	return m_linkerId;
}

//查询拥有的角色数量 add by hxw 20151210
int RolesInfoMan::RoleSite::QueryUserRolesNum()
{
	return m_rolesMap.size();
}

int RolesInfoMan::RoleSite::QueryUserRoles(UINT32 userId, SQueryRolesRes* pRoles)
{
	BYTE maxRetRoleCount = (MAX_RES_USER_BYTES - member_offset(SQueryRolesRes, rolesCount, roles)) / sizeof(SQueryRoleAttr);
	CXTAutoLock lock(m_rolesMapLocker);
	if (m_rolesMap.empty())
	{
		int ret = OnQueryRolesInfoByUserId(userId);
		if (ret != MLS_OK)
			return ret;
	}

	if (pRoles == NULL)
		return MLS_OK;

	BYTE idx = 0;
	std::map<UINT32/*roleId*/, SRoleInfos*>::iterator it = m_rolesMap.begin();
	for (; it != m_rolesMap.end(); ++it)
	{
		SRoleInfos* pInfos = it->second;
		pRoles->roles[idx].cashs = pInfos->cashcount;
		pRoles->roles[idx].rpcoin = pInfos->cash;
		pRoles->roles[idx].exp = pInfos->exp;
		pRoles->roles[idx].gold = pInfos->gold;
		pRoles->roles[idx].id = it->first;
		pRoles->roles[idx].occuId = pInfos->occuId;
		pRoles->roles[idx].sex = pInfos->sex;
		pRoles->roles[idx].level = pInfos->level;
		strcpy(pRoles->roles[idx].nick, pInfos->nick);
		pRoles->roles[idx].cashcount = pInfos->cashcount;
		pRoles->roles[idx].owncopy = pInfos->pos.owncopy;
		pRoles->roles[idx].mainWeapon = 0;
		pRoles->roles[idx].subWeapon = 0;
		pRoles->roles[idx].dress = pInfos->dressId;
		strcpy(pRoles->roles[idx].lastEnterTime, pInfos->lastEnterTime);
		// 		std::vector<SPropertyAttr>&	packs = pInfos->packs;
		// 		int si = (int)packs.size();
		// 		for(int i = 0; i < si; ++i)
		// 		{
		// 			if(packs[i].pos == PROP_MAIN_WEAPON_POS)
		// 				pRoles->roles[idx].mainWeapon = packs[i].propId;
		// 			else if(packs[i].pos == PROP_SUB_WEAPON_POS)
		// 				pRoles->roles[idx].subWeapon = packs[i].propId;
		// 			else if(packs[i].pos == PROP_DRESS_POS)
		// 				pRoles->roles[idx].dress = packs[i].propId;
		// 
		// 			if(pRoles->roles[idx].mainWeapon != 0 && pRoles->roles[idx].subWeapon != 0 && pRoles->roles[idx].dress != 0)
		// 				break;
		// 			
		// 		}
		pRoles->roles[idx].hton();
		if (++idx >= maxRetRoleCount)
			break;
	}
	pRoles->rolesCount = idx;
	return MLS_OK;
}

int RolesInfoMan::RoleSite::QueryRoleAppearance(UINT32 roleId, SQueryRoleAppearanceRes* pRoleRes)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*roleId*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	SRoleInfos* pRoleInfo = it->second;
	pRoleRes->level = pRoleInfo->level;
	pRoleRes->roleId = roleId;
	pRoleRes->dressId = pRoleInfo->dressId;
	pRoleRes->titleId = pRoleInfo->titleId;
	strcpy(pRoleRes->nick, pRoleInfo->nick);

	BYTE maxPropNum = (MAX_RES_USER_BYTES - member_offset(SQueryRoleAppearanceRes, roleId, props)) / sizeof(SPropertyAttr);
	pRoleRes->num = pRoleInfo->GetWearEquipmentProps(pRoleRes->props, maxPropNum);
	return MLS_OK;
}

int RolesInfoMan::RoleSite::QueryRoleExplicit(UINT32 roleId, SQueryRoleExplicitRes* pRoleRes)
{
	{
		BYTE maxNum = (BYTE)((MAX_RES_USER_BYTES - member_offset(SQueryRoleExplicitRes, roleId, skills)) / sizeof(SSkillMoveInfo));

		CXTAutoLock lock(m_rolesMapLocker);
		std::map<UINT32/*roleId*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
		if (it == m_rolesMap.end())
			return MLS_ROLE_NOT_EXIST;

		SRoleInfos* pRoleInfo = it->second;
		pRoleRes->roleId = roleId;						//角色Id
		strcpy(pRoleRes->roleNick, pRoleInfo->nick);	//角色昵称[33];
		pRoleRes->roleLevel = pRoleInfo->level;	//角色等级
		pRoleRes->titleId = pRoleInfo->titleId;		//称号id;
		pRoleRes->gold = pRoleInfo->gold;		//游戏币
		pRoleRes->guildId = pRoleInfo->guildId;	//公会ID;
		pRoleRes->exp = pRoleInfo->exp;			//经验
		pRoleRes->cash = pRoleInfo->cash;		//现金
		pRoleRes->occuId = pRoleInfo->occuId;	//职业id
		pRoleRes->sex = pRoleInfo->sex;
		pRoleRes->cashcount = pRoleInfo->cashcount;	//充值总额
		pRoleRes->hp = pRoleInfo->hp;			//生命
		pRoleRes->mp = pRoleInfo->mp;			//魔力
		pRoleRes->vit = pRoleInfo->vit;		//体力
		pRoleRes->ap = pRoleInfo->ap;			//剩余属性点数(数据库里是int)
		pRoleRes->packSize = pRoleInfo->packSize;		//背包大小
		pRoleRes->wareSize = pRoleInfo->wareSize;		//仓库大小
		memcpy(&pRoleRes->pos, &pRoleInfo->pos, sizeof(SGSPosition));	//所在位置
		pRoleRes->cp = pRoleInfo->cp;			//当前战斗力;
		pRoleRes->num = pRoleInfo->GetStudySkills(pRoleRes->skills, maxNum);
		pRoleRes->attack = pRoleInfo->attack;
		pRoleRes->def = pRoleInfo->def;
	}

	return MLS_OK;
}

BOOL RolesInfoMan::RoleSite::IsRoleOnline(UINT32 roleId)
{
	BOOL online = false;
	CXTAutoLock lock(m_rolesMapLocker);
	if (roleId > 0)
	{
		std::map<UINT32/*roleId*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
		if (it != m_rolesMap.end())
			online = it->second->isOnline ? true : false;
	}
	else
	{
		std::map<UINT32/*roleId*/, SRoleInfos*>::iterator it = m_rolesMap.begin();
		for (; it != m_rolesMap.end(); ++it)
		{
			if (it->second->isOnline)
			{
				online = true;
				break;
			}
		}
	}
	return online;
}

int RolesInfoMan::RoleSite::GetUserEnteredRoles(std::vector<UINT32>& rolesVec)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*roleId*/, SRoleInfos*>::iterator it = m_rolesMap.begin();
	for (; it != m_rolesMap.end(); ++it)
	{
		if (!it->second->isEnterGame)
			continue;

		rolesVec.push_back(it->first);
	}
	return MLS_OK;
}

int RolesInfoMan::RoleSite::RecoveryOnlineRoleVit(UINT32 roleId, UINT32& vit, BOOL& needNotify)
{
	UINT32 maxVit = GetGameCfgFileMan()->GetMaxAutoRecoveryVit();

	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*roleId*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	if (it->second->vit >= maxVit)
	{
		needNotify = false;
		return MLS_OK;
	}

	UINT32 newVit = SGSU32Add(it->second->vit, vit);
	it->second->vit = min(newVit, maxVit);	//恢复体力后总的点数

	it->second->onlineRecoveryVit = SGSU32Add(it->second->onlineRecoveryVit, vit);
	vit = it->second->vit;
	needNotify = true;
	return MLS_OK;
}

int RolesInfoMan::RoleSite::EnterRole(UINT32 userId, UINT32 roleId, SEnterRoleRes* pEnterRes)
{
	char szTime[32] = { 0 };
	GetLocalStrTime(szTime);
	BYTE maxNum = (BYTE)((MAX_RES_USER_BYTES - member_offset(SEnterRoleRes, userId, data)) / sizeof(SSkillMoveInfo));

	{
		CXTAutoLock lock(m_rolesMapLocker);
		std::map<UINT32/*roleId*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
		if (it == m_rolesMap.end())
			return MLS_ROLE_NOT_EXIST;

		SRoleInfos* pInfo = it->second;

		// 2016.01.12 zpy 新增
		pInfo->CalcRoleCombatPower();

		//mid_cash add by hxw 20151121 进入角色选择更新一次角色身上的代币数据
		if (GetConfigFileMan()->GetCashSaveType() == CASH_SAVE_TYPE_TO_CASHDB)
		{
			GetCashDBMan()->GetCash(userId, pInfo->cash, pInfo->cashcount);
		}
		//end

		pInfo->isOnline = 1;
		pInfo->isEnterGame = 1;
		pInfo->onlineRecoveryVit = 0;
		pInfo->AutoRecoveryVit(GetGameCfgFileMan()->GetMaxAutoRecoveryVit(), GetMsel());

		pEnterRes->ap = pInfo->ap;
		strcpy(pEnterRes->nick, pInfo->nick);	//char   nick[33];//昵称[33];
		pEnterRes->titleId = pInfo->titleId;	//称号id
		pEnterRes->roleLevel = pInfo->level;	//UINT16 roleLevel;	//角色等级 ??? BYTE   level;(数据库里是tinyint)
		pEnterRes->gold = pInfo->gold;			//SBigNumber gold;// 游戏币
		pEnterRes->exp = pInfo->exp;			//SBigNumber exp;		//经验
		pEnterRes->cash = pInfo->cash;			//UINT32 cash;		//现金
		pEnterRes->occuId = pInfo->occuId;		//职业id
		pEnterRes->sex = pInfo->sex;			//性别
		pEnterRes->cashcount = pInfo->cashcount;//UINT32 cashcount;	//充值总额
		pEnterRes->guildId = pInfo->guildId;	//公会id
		pEnterRes->hp = pInfo->hp;				//UINT32 hp;			//生命
		pEnterRes->mp = pInfo->mp;				//UINT32 mp;			//魔力
		pEnterRes->attack = pInfo->attack;
		pEnterRes->def = pInfo->def;
		pEnterRes->vit = pInfo->vit;			//UINT32 vit;		//体力
		pEnterRes->ap = pInfo->ap;				//UINT16 ap;			//剩余属性点数
		pEnterRes->packSize = pInfo->packSize;	//BYTE packSize;		//背包大小
		pEnterRes->wareSize = pInfo->wareSize;	//BYTE wareSize;		//仓库大小
		pEnterRes->bt = pInfo->cp;				//战力
		pEnterRes->factionId = pInfo->factionId;	//阵营
		memcpy(&pEnterRes->pos, &pInfo->pos, sizeof(SGSPosition));	//位置

		BYTE idx = 0;
		pEnterRes->skillNum = min(maxNum, (BYTE)pInfo->studySkills.size());
		for (BYTE i = 0; i < pEnterRes->skillNum; ++i, ++idx)
		{
			pEnterRes->data[idx] = pInfo->studySkills[i];
			pEnterRes->data[idx].hton();
		}

		++pInfo->vist;	//增加访问次数
		strcpy(pInfo->lastEnterTime, szTime);	//更新最后进入游戏时间

		//add by hxw 20151020
		GetRankRewardMan()->QueryOnlineInfoForDB(roleId, pInfo, szTime);
		GetRankRewardMan()->QueryStrongerInfoForDB(roleId, pInfo);
		//end

	}
	return MLS_OK;
}

int RolesInfoMan::RoleSite::QueryBackpack(UINT32 roleId, BYTE from, BYTE nums, SQueryBackpackRes* pBackpackRes)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*roleId*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	BYTE maxCount = (MAX_RES_USER_BYTES - member_offset(SQueryBackpackRes, total, props)) / sizeof(SPropertyAttr);
	SRoleInfos* pRoleInfos = it->second;
	pBackpackRes->curNum = pRoleInfos->GetBackpackProps(pBackpackRes->props, min(maxCount, nums), from, &pBackpackRes->total);
	return MLS_OK;
}

int RolesInfoMan::RoleSite::CleanupBackpack(UINT32 roleId, BOOL& isCleanup)
{
	isCleanup = false;
	std::map<UINT16/*物品id*/, std::vector<SPropertyAttr> > propMap;

	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*roleId*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	std::vector<SPropertyAttr>& backpack = it->second->packs;
	for (std::vector<SPropertyAttr>::iterator it = backpack.begin(); it != backpack.end();)
	{
		if (it->type == ESGS_PROP_EQUIP)	//装备不能堆叠,故不用整理
			++it;
		else
		{
			BYTE maxStack = GetGameCfgFileMan()->GetPropertyMaxStack(it->type, it->propId);
			if (it->num >= maxStack)	//格子堆叠满了也不需要整理
			{
				++it;
				continue;
			}

			std::map<UINT16, std::vector<SPropertyAttr> >::iterator it2 = propMap.find(it->propId);
			if (it2 == propMap.end())
			{
				std::vector<SPropertyAttr> vec;
				propMap.insert(std::make_pair(it->propId, vec));
				std::vector<SPropertyAttr>& vecRef = propMap[it->propId];
				vecRef.push_back(*it);
			}
			else
				it2->second.push_back(*it);

			it = backpack.erase(it);
		}
	}

	for (std::map<UINT16, std::vector<SPropertyAttr> >::iterator it = propMap.begin(); it != propMap.end(); ++it)
	{
		std::vector<SPropertyAttr>& vec = it->second;
		BYTE si = (BYTE)vec.size();
		ASSERT(si > 0);
		BYTE maxStack = GetGameCfgFileMan()->GetPropertyMaxStack(vec[0].type, it->first);
		ASSERT(maxStack > 0);

		UINT16 sum = 0;
		for (BYTE i = 0; i < si; ++i)
			sum = SGSU16Add(sum, vec[i].num);

		ASSERT(sum > 0);
		isCleanup = true;

		BYTE idx = 0;
		BYTE k = sum % maxStack;
		BYTE m = sum / maxStack;
		while (m--)
		{
			ASSERT(idx < si);
			SPropertyAttr prop;
			memcpy(&prop, &vec[idx], sizeof(SPropertyAttr));
			prop.num = maxStack;
			backpack.push_back(prop);
			++idx;
		}

		ASSERT(idx < si);
		if (k > 0)
		{
			SPropertyAttr prop;
			memcpy(&prop, &vec[idx], sizeof(SPropertyAttr));
			prop.num = k;
			backpack.push_back(prop);
		}
	}
	return MLS_OK;
}

int RolesInfoMan::RoleSite::CreateRole(UINT32 userId, BYTE occuId, BYTE sex, const char* roleNick, SCreateRoleRes* pCreateRes)
{
	//是否超过最大数量
	if (m_rolesMap.size() >= GetGameCfgFileMan()->GetMaxCreateRoles())
		return MLS_OVER_MAX_ROLES_THRESHOLD;

	//hash角色id
	UINT32 roleId = ELFHash(roleNick);

	//add by hxw 20151116
	if (roleId <= 0)
		return MLS_GET_ROLE_ID_FAILED;
	//end

	//检查角色昵称是否唯一
	char szSQL[MAX_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select count(id) from actor where id=%u", roleId);
	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != MLS_OK)
		return retCode;

	DBS_ROW row = DBS_FetchRow(result);
	ASSERT(row != NULL);
	int n = atoi(row[0]);
	DBS_FreeResult(result);
	result = NULL;
	if (n > 0)
		return MLS_ROLE_NICK_IS_EXISTED;

	//zpy新增 检测角色职业是否重复
	sprintf(szSQL, "SELECT actortype FROM actor WHERE accountid=%u", userId);
	retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != MLS_OK)
		return retCode;
	while ((row = DBS_FetchRow(result)) != NULL)
	{
		if (occuId == atoi(row[0]))
		{
			DBS_FreeResult(result);
			return MLS_EXIST_SAME_JOB;
		}
	}
	DBS_FreeResult(result);

	//获取角色默认值
	AutoRoleInfosObj pRole;
	if (!pRole.IsValid())
		return MLS_RESOURCE_NOT_ENOUGH;

	pRole->roleId = roleId;				// 2016.01.12 zpy新增
	pRole->level = 1;	//初始等级为1
	pRole->occuId = occuId;	//职业
	pRole->sex = sex;

	//初始化角色赠送 位置提前
	pRole->InitNewRoleGiveItems(userId);

	SRoleAttrPoint attr;
	retCode = GetGameCfgFileMan()->GetRoleCfgBasicAttr(occuId, pRole->level, attr);	//基本属性
	if (retCode != MLS_OK)
		return retCode;

	pRole->hp = attr.u32HP;
	pRole->mp = attr.u32MP;
	pRole->attack = attr.attack;
	pRole->def = attr.def;

	//初始背包大小
	pRole->packSize = GetGameCfgFileMan()->GetInitBackpackSize();

	//初始仓库大小
	pRole->wareSize = GetGameCfgFileMan()->GetInitWarehouseSize();

	//初始体力
	pRole->vit = GetGameCfgFileMan()->GetMaxAutoRecoveryVit();

	//初始技能
	retCode = GetGameCfgFileMan()->GetRoleAllInitSkills(occuId, 0, pRole->level, pRole->studySkills);
	if (retCode != MLS_OK)
		return retCode;

	/* 位置提前 20160104
	//初始化角色赠送
	pRole->InitNewRoleGiveItems(userId);
	*/
	//写数据库(actor表)
	char szPos[64] = { 0 };
	DBS_EscapeString(szPos, (const char*)&pRole->pos, sizeof(pRole->pos));

	//创建角色时间
	GetLocalStrTime(pRole->lastEnterTime);

	//计算新建角色战力
	pRole->CalcRoleCombatPower();

	//写数据库
	UINT16 dressId = 0;
	memset(szSQL, 0, sizeof(szSQL));
	char szCreateDate[32] = { 0 };
	GetLocalStrTime(szCreateDate);

	sprintf(szSQL, "insert into actor(id, createtts, accountid, nick, sex, gold, level, occu, exp, cash, cashcount, actortype, "
		"hp, mp, attack, def, vit, cp, ap, packsize, waresize, guild, dressid, duplicate, coords, faction, vist, lvtts) "
		"values(%u, '%s', %u, '%s', '%d', %llu, %d, %d, %llu, %u, %u, %d, %u, %u, %u, %u, %u, %u, %u, %d, %d, "
		"%d, %d, %d, '%s', %d, %u, '%s')", roleId, szCreateDate, userId, roleNick, sex, BigNumberToU64(pRole->gold),
		pRole->level, pRole->titleId, BigNumberToU64(pRole->exp), pRole->cash, pRole->cashcount, occuId, pRole->hp,
		pRole->mp, pRole->attack, pRole->def, pRole->vit, pRole->cp, pRole->ap, pRole->packSize, pRole->wareSize,
		pRole->guildId, dressId, pRole->pos.owncopy, szPos, pRole->factionId, 0, pRole->lastEnterTime);

	retCode = GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);
	if (retCode != MLS_OK)
		return retCode;

	if (roleId <= 0)
		return MLS_GET_ROLE_ID_FAILED;



	//data表
	BYTE bin = 0;
	char szBin[8] = { 0 };
	DBS_EscapeString(szBin, (const char*)&bin, sizeof(bin));
	memset(szSQL, 0, sizeof(szSQL));

	//构造技能写入数据库数据
	char szSkills[MAX_BUF_LEN * 2] = { 0 };
	BYTE skillNum = (BYTE)pRole->studySkills.size();
	n = DBS_EscapeString(szSkills, (const char*)&skillNum, sizeof(BYTE));
	if (skillNum > 0)
		DBS_EscapeString(szSkills + n, (const char*)pRole->studySkills.data(), skillNum * sizeof(SSkillMoveInfo));

	//构造背包装备写入数据库数据
	char szBackpack[MAX_BUF_LEN * 2] = { 0 };
	BYTE backpackNum = (BYTE)pRole->packs.size();
	n = DBS_EscapeString(szBackpack, (const char*)&backpackNum, sizeof(BYTE));
	if (backpackNum > 0)
		DBS_EscapeString(szBackpack + n, (const char*)pRole->packs.data(), backpackNum * sizeof(SPropertyAttr));

	//构造已穿戴装备写入数据库数据
	char szWearEquip[MAX_BUF_LEN] = { 0 };
	BYTE wearEquipNum = (BYTE)pRole->wearEquipment.size();
	n = DBS_EscapeString(szWearEquip, (const char*)&wearEquipNum, sizeof(BYTE));
	if (wearEquipNum > 0)
		DBS_EscapeString(szWearEquip + n, (const char*)pRole->wearEquipment.data(), wearEquipNum * sizeof(SPropertyAttr));

	//构造初始时装
	char szDressBuf[MAX_BUF_LEN] = { 0 };
	UINT16 dressNum = (UINT16)pRole->dress.size();
	n = DBS_EscapeString(szDressBuf, (const char*)&dressNum, sizeof(UINT16));
	if (dressNum > 0)
		DBS_EscapeString(szDressBuf + n, (const char*)pRole->dress.data(), dressNum * sizeof(UINT16));

	//构造初始装备碎片
	char szFragmentBuf[MAX_BUF_LEN] = { 0 };
	BYTE fragmentNum = (BYTE)pRole->fragments.size();
	n = DBS_EscapeString(szFragmentBuf, (const char*)&fragmentNum, sizeof(BYTE));
	if (fragmentNum > 0)
		DBS_EscapeString(szFragmentBuf + n, (const char*)pRole->fragments.data(), fragmentNum * sizeof(SEquipFragmentAttr));

	//装备位
	char szEquipPos[MAX_BUF_LEN] = { 0 };
	BYTE posNum = (BYTE)pRole->equipPos.size();
	n = DBS_EscapeString(szEquipPos, (const char*)&posNum, sizeof(BYTE));
	if (posNum > 0)
		DBS_EscapeString(szEquipPos + n, (const char*)pRole->equipPos.data(), posNum * sizeof(SEquipPosAttr));

	// zpy 构造初始炼魂碎片
	char szChainSoulFragmentBuf[MAX_BUF_LEN] = { 0 };
	BYTE chainSoulFragmentNum = (BYTE)pRole->chainSoulFragments.size();
	n = DBS_EscapeString(szChainSoulFragmentBuf, (const char*)&chainSoulFragmentNum, sizeof(BYTE));
	if (chainSoulFragmentNum > 0)
		DBS_EscapeString(szChainSoulFragmentBuf + n, (const char*)pRole->chainSoulFragments.data(), chainSoulFragmentNum * sizeof(SChainSoulFragmentAttr));

	// zpy 构造炼魂部件
	char szChainSoulPos[MAX_BUF_LEN] = { 0 };
	BYTE chainSoulPosNum = (BYTE)pRole->chainSoulPos.size();
	n = DBS_EscapeString(szChainSoulPos, (const char*)&chainSoulPosNum, sizeof(BYTE));
	if (chainSoulPosNum > 0)
		DBS_EscapeString(szChainSoulPos + n, (const char*)pRole->chainSoulPos.data(), chainSoulPosNum * sizeof(SChainSoulPosAttr));

	//写数据库
	sprintf(szSQL, "insert into data(actorid, whpass, buffer, backpack, warehouse, wearequipment, taskaccepted, ability, help, dress, fragment, equipsock, lh_fragment, lh_pos) values"
		"(%u, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')", roleId, "123456", szBin, szBackpack, szBin, szWearEquip,
		szBin, szSkills, szBin, szDressBuf, szFragmentBuf, szEquipPos, szChainSoulFragmentBuf, szChainSoulPos);
	retCode = GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);
	if (retCode != MLS_OK)
	{
		memset(szSQL, 0, sizeof(szSQL));
		sprintf(szSQL, "delete from actor where id=%u", roleId);
		GetDBSClient()->Del(GetConfigFileMan()->GetDBName(), szSQL, NULL);
		return retCode;
	}

	//初始化新手引导,魔导器数据
	retCode = GetPermanenDataMan()->InitNewRolePermanenData(roleId, pRole->magics);
	if (retCode != MLS_OK)
	{
		memset(szSQL, 0, sizeof(szSQL));
		sprintf(szSQL, "delete from actor where id=%u", roleId);
		GetDBSClient()->Del(GetConfigFileMan()->GetDBName(), szSQL, NULL);
		return retCode;
	}

	//初始邮件
	pRole->InitNewRoleEmail(userId, roleId);

	//昵称
	strcpy(pRole->nick, roleNick);

	//返回数据
	pCreateRes->id = roleId;	//角色id
	pCreateRes->occuId = occuId;	//职业id
	pCreateRes->sex = sex;		//性别
	strcpy(pCreateRes->nick, roleNick);	//角色昵称
	pCreateRes->level = pRole->level;	//角色等级
	pCreateRes->dress = pRole->dressId;		//角色时装;
	pCreateRes->mainWeapon = 0;				//角色主手武器;
	pCreateRes->subWeapon = 0;				//角色副手武器;
	pCreateRes->gold = pRole->gold;			//游戏币;
	pCreateRes->rpcoin = pRole->cash;		//代币;
	pCreateRes->exp = pRole->exp;			//经验;
	pCreateRes->owncopy = pRole->pos.owncopy;		//所在副本;

	//add by hxw 20151120 从代币服务器获取代币
	if (GetConfigFileMan()->GetCashSaveType() == CASH_SAVE_TYPE_TO_CASHDB)
	{
		GetCashDBMan()->GetCash(userId, pRole->cash, pRole->cashcount);
	}
	//end

	/* zpy 成就系统新增 */
	if (!GetAchievementMan()->CreateCache(roleId, retCode))
	{
		DBS_FreeResult(result);
		return retCode;
	}

	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it != m_rolesMap.end())
	{
		ASSERT(false);
		return MLS_ROLE_IS_EXISTED;
	}



	m_rolesMap.insert(std::make_pair(roleId, pRole.Drop()));

	return MLS_OK;
}

int RolesInfoMan::RoleSite::DropBackpackProperty(UINT32 roleId, UINT32 id, BYTE num)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	return it->second->DropBackpackProp(id, num);
}

int RolesInfoMan::RoleSite::DropTask(UINT32 roleId, UINT16 taskId)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	return it->second->DropTask(taskId);
}

int RolesInfoMan::RoleSite::CommitTaskFinish(UINT32 roleId, UINT16 taskId, SFinishTaskFailNotify* pFinTaskNotify)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	SRoleInfos* pRole = it->second;

	//判断任务是否完成
	return pRole->JudgeTaskFinish(taskId, pFinTaskNotify);
}

int RolesInfoMan::RoleSite::BuyStorageSpace(UINT32 roleId, BYTE buyType, BYTE num, UINT32& spend, BYTE& newNum)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	if (buyType == 0)	//背包
	{
		if (it->second->packSize >= GetGameCfgFileMan()->GetMaxBackpackSize())
		{
			return MLS_BACKPACK_UPPER_LIMIT;
		}

		it->second->packSize = SGSU8Add(it->second->packSize, num);
		if (it->second->packSize > GetGameCfgFileMan()->GetMaxBackpackSize())
		{
			it->second->packSize = GetGameCfgFileMan()->GetMaxBackpackSize();
		}
		newNum = it->second->packSize;
		spend = 0;
	}
	else	//仓库
	{
		if (it->second->wareSize >= GetGameCfgFileMan()->GetMaxWarehouseSize())
		{
			return MLS_WAREHOUSE_UPPER_LIMIT;
		}

		it->second->wareSize = SGSU8Add(it->second->wareSize, num);
		if (it->second->wareSize > GetGameCfgFileMan()->GetMaxWarehouseSize())
		{
			it->second->wareSize = GetGameCfgFileMan()->GetMaxWarehouseSize();
		}
		newNum = it->second->wareSize;
		spend = 0;
	}

	return MLS_OK;
}

int RolesInfoMan::RoleSite::SellProperty(UINT32 roleId, UINT32 id, BYTE num, SSellPropertyRes* pSellRes, SPropertyAttr& propAttr)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	SRoleInfos* pRoleInfo = it->second;
	int retCode = pRoleInfo->SellProperty(id, num, propAttr, pSellRes);
	if (retCode != MLS_OK)
		return retCode;

	pSellRes->gold = pRoleInfo->gold;
	return retCode;
}

int RolesInfoMan::RoleSite::BuyBackProperty(UINT32 roleId, SPropertyAttr& buyProp, BYTE num, SPropBuyBackRes* pBuyBackRes)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	return it->second->BuyBackProperty(buyProp, num, pBuyBackRes);
}

int RolesInfoMan::RoleSite::AcceptTask(UINT32 roleId, UINT16 taskId)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	return it->second->AcceptTask(taskId);
}

int RolesInfoMan::RoleSite::QueryAcceptedTasks(UINT32 roleId, SQueryAcceptTasksRes* pTaskRes)
{
	BYTE idx = 0;
	BYTE maxNum = (MAX_RES_USER_BYTES - member_offset(SQueryAcceptTasksRes, num, tasks)) / sizeof(SAcceptTaskInfo);

	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	SRoleInfos* pRole = it->second;
	std::vector<SAcceptTaskInfo>& acceptTasks = pRole->acceptTasks;
	BYTE taskNum = (BYTE)acceptTasks.size();
	for (BYTE i = 0; i < taskNum; ++i)
	{
#if 1
		if (!acceptTasks[i].isFinished)
		{
			TaskCfg::STaskAttrCfg* pTaskInfo = NULL;
			int retCode = GetGameCfgFileMan()->GetTaskInfo(acceptTasks[i].taskId, pTaskInfo);
			if (retCode != MLS_OK)
				continue;

			BYTE targetType = pTaskInfo->targetType;
			switch (targetType)
			{
			case ESGS_TASK_TARGET_ESCORT:			//护送
				acceptTasks[i].target[2] = 0;	//血量
				break;

			case ESGS_TASK_TARGET_PASS_NO_HURT:	//无伤通关
				acceptTasks[i].target[1] = 0;	//血量
				break;

			case ESGS_TASK_TARGET_LIMIT_TIME:		//限时通关
				acceptTasks[i].target[1] = 0;	//时间
				break;

			case ESGS_TASK_TARGET_KILL_MONSTER_LIMIT_TIME:	//限时杀怪
			case ESGS_TASK_TARGET_DESTROY_SCENE_LIMIT_TIME:	//限时破坏场景物（复数）
				acceptTasks[i].target[2] = 0;	//时间
				acceptTasks[i].target[3] = 0;	//数量
				break;

			case ESGS_TASK_TARGET_FETCH_PROP_LIMIT_TIME:	//限时获取道具（复数）
				acceptTasks[i].target[3] = 0;	//时间
				acceptTasks[i].target[4] = 0;	//数量
				break;

			case ESGS_TASK_TARGET_BLOCK_TIMES:		//格挡多少次
			case ESGS_TASK_TARGET_COMBO_NUM_REACH:	//达到足够多的连击数
			case ESGS_TASK_TARGET_KEEP_BLOOD:		//血量保持
				acceptTasks[i].target[1] = 0;	//数量
				break;

			default:
				break;
			}
		}
#endif
		memcpy(&pTaskRes->tasks[idx], &acceptTasks[i], sizeof(SAcceptTaskInfo));
		pTaskRes->tasks[idx].hton();

		if (++idx >= maxNum)
			break;
	}
	pTaskRes->num = idx;
	return MLS_OK;
}

int RolesInfoMan::RoleSite::LeaveRole(UINT32 roleId)
{
	//add by hxw 20151020 
	UINT32 upid = 0;
	//end
	UINT16 curInstId = 0;
	LTMSEL lastEnterMSel = 0;
	{
		CXTAutoLock lock(m_rolesMapLocker);
		std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
		if (it == m_rolesMap.end())
			return MLS_ROLE_NOT_EXIST;

		it->second->isOnline = 0;
		it->second->onlineRecoveryVit = 0;
		curInstId = it->second->pos.owncopy;
		lastEnterMSel = LocalStrTimeToMSel(it->second->lastEnterTime);
		upid = it->second->onlineVec.id;
		it->second->onlines.clear();
		it->second->strongerVec.clear();
	}

	//更新在线时间
	LTMSEL tmInterval = (GetMsel() - lastEnterMSel) / 1000;
	if (tmInterval > 0)
	{
		char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
		sprintf(szSQL, "update actor set online=online+%lld where id=%u", tmInterval, roleId);
		GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL, NULL);

		//add by hxw 20151020 每天在线时间统计
		if (upid>0)
			GetRankRewardMan()->UpdateOnlineTime(upid, tmInterval);
		//end
	}

	return MLS_OK;
}

int RolesInfoMan::RoleSite::LoadEquipment(UINT32 roleId, UINT32 id, BYTE pos)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	return it->second->LoadEquipment(id, pos);
}

int RolesInfoMan::RoleSite::UnLoadEquipment(UINT32 roleId, UINT32 id)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	return it->second->UnLoadEquipment(id);
}

int RolesInfoMan::RoleSite::UseProperty(UINT32 roleId, UINT32 id, UINT16& propId, SUsePropertyRes *pUseProperty)
{
	BYTE   propType = 0;
	int retCode = MLS_ID_NOT_EXIST;

	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	// 背包扩展符单独处理
	if (id != ESGS_MATERIAL_BACKPACK_EXTEND)
	{
		std::vector<SPropertyAttr>& packs = it->second->packs;
		for (std::vector<SPropertyAttr>::iterator pack_it = packs.begin(); pack_it != packs.end(); ++pack_it)
		{
			if (pack_it->id != id)
				continue;

			propId = pack_it->propId;
			propType = pack_it->type;
			pack_it->num = SGSU8Sub(pack_it->num, 1);
			pUseProperty->data[pUseProperty->deduct_num].uid = id;
			pUseProperty->data[pUseProperty->deduct_num].num = 1;
			pUseProperty->data[pUseProperty->deduct_num].type = propType;
			++pUseProperty->deduct_num;
			if (pack_it->num == 0)
				packs.erase(pack_it);

			retCode = MLS_OK;
			break;
		}

		if (retCode != MLS_OK)
			return retCode;

		//体力药水
		else if (IS_VIT_POSION(propId))
		{
			UINT32 addVit = 0;
			GetGameCfgFileMan()->GetMaterialFuncValue(propId, addVit);
			it->second->vit = SGSU32Add(it->second->vit, addVit);
		}
	}
	else
	{
		propId = ESGS_MATERIAL_BACKPACK_EXTEND;

		// 是否达到上限
		if (it->second->packSize >= GetGameCfgFileMan()->GetMaxBackpackSize())
		{
			return MLS_BACKPACK_UPPER_LIMIT;
		}

		// 获取背包扩展符数量
		UINT32 num = 0;
		std::vector<SPropertyAttr>& packs = it->second->packs;
		for (std::vector<SPropertyAttr>::iterator pack_it = packs.begin(); pack_it != packs.end(); ++pack_it)
		{
			if (pack_it->propId == ESGS_MATERIAL_BACKPACK_EXTEND)
			{
				num += pack_it->num;
			}
		}

		// 需要消耗的扩展符数量
		int need_num = GetGameCfgFileMan()->GetPackParam() + it->second->packSize - GetGameCfgFileMan()->GetInitBackpackSize();
		if (need_num > num)
		{
			int cont_rpcoin = 0;
			MallCfg::SMallGoodsAttr *pGoodsAttr = NULL;
			if (MLS_OK == GetGameCfgFileMan()->GetMallBuyGoodsAttrByPropId(ESGS_MATERIAL_BACKPACK_EXTEND, pGoodsAttr))
			{
				cont_rpcoin = (need_num - num) * pGoodsAttr->priceToken;
			}

			if (cont_rpcoin > it->second->cash)
			{
				return MLS_CASH_NOT_ENOUGH;
			}

			//扣除代币
			it->second->ConsumeToken(cont_rpcoin, "购买背包扩展符");
		}

		// 扣除背包扩展符
		UINT32 deduct = need_num >= num ? num : need_num;
		for (std::vector<SPropertyAttr>::iterator pack_it = packs.begin(); pack_it != packs.end() && deduct > 0;)
		{
			if (pack_it->propId == ESGS_MATERIAL_BACKPACK_EXTEND)
			{
				if (pack_it->num > 0)
				{
					pUseProperty->data[pUseProperty->deduct_num].uid = pack_it->id;
					pUseProperty->data[pUseProperty->deduct_num].type = pack_it->type;
					if (deduct <= pack_it->num)
					{
						pack_it->num -= deduct;
						pUseProperty->data[pUseProperty->deduct_num].num = deduct;
						deduct = 0;
					}
					else
					{
						deduct -= pack_it->num;
						pUseProperty->data[pUseProperty->deduct_num].num = pack_it->num;
						pack_it->num = 0;
					}
					++pUseProperty->deduct_num;
				}

				if (pack_it->num <= 0)
				{
					pack_it = packs.erase(pack_it);
				}
			}
			else
			{
				++pack_it;
			}
		}

		// 每次增加的数量
		UINT32 nVal = 0;
		GetGameCfgFileMan()->GetMaterialFuncValue(ESGS_MATERIAL_BACKPACK_EXTEND, nVal);
		it->second->packSize = SGSU8Add(it->second->packSize, nVal);
		if (it->second->packSize > GetGameCfgFileMan()->GetMaxBackpackSize())
		{
			it->second->packSize = GetGameCfgFileMan()->GetMaxBackpackSize();
		}

		retCode = MLS_OK;
	}

	pUseProperty->exp = it->second->exp;
	pUseProperty->gold = it->second->gold;
	pUseProperty->rpcoin = it->second->cash;
	pUseProperty->vit = it->second->vit;

	return retCode;
}

int RolesInfoMan::RoleSite::DelRole(UINT32 roleId)
{
	SRoleInfos* pInfos = NULL;
	{
		CXTAutoLock lock(m_rolesMapLocker);
		std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
		if (it == m_rolesMap.end())
			return MLS_OK;

		if (it->second->isOnline)
			return MLS_ROLE_IS_ONLINE;

		pInfos = it->second;
		m_rolesMap.erase(it);
	}

	ASSERT(pInfos != NULL);
	GetRoleInfosObjAllocator()->DeleteRoleInfosObj(pInfos);
	return MLS_OK;
}

/* zpy 成就系统新增 */
int RolesInfoMan::RoleSite::RemoveAllRole()
{
	std::set<SRoleInfos *> delete_set;

	{
		CXTAutoLock lock(m_rolesMapLocker);
		for (std::map<UINT32, SRoleInfos*>::iterator itr = m_rolesMap.begin(); itr != m_rolesMap.end(); ++itr)
		{
			delete_set.insert(itr->second);
		}
		m_rolesMap.clear();
	}

	for (std::set<SRoleInfos *>::iterator itr = delete_set.begin(); itr != delete_set.end(); ++itr)
	{
		ASSERT(*itr != NULL);
		GetAchievementMan()->ClearCache((*itr)->roleId);
		GetRoleInfosObjAllocator()->DeleteRoleInfosObj(*itr);
	}

	return MLS_OK;
}

int RolesInfoMan::RoleSite::EquipPosInlaidGem(UINT32 roleId, BYTE pos, UINT32 gemId, BYTE holeSeq)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	return it->second->EquipPosInlaidGem(pos, gemId, holeSeq);
}

int RolesInfoMan::RoleSite::EquipPosRemoveGem(UINT32 roleId, BYTE pos, BYTE holeSeq, SEquipPosRemoveGemRes* pRemoveRes)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	return it->second->EquipPosRemoveGem(pos, holeSeq, pRemoveRes);
}

int	RolesInfoMan::RoleSite::GemCompose(UINT32 roleId, BYTE num, const SConsumeProperty* pMaterial, SGemComposeRes* pComposeRes)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	return it->second->GemCompose(num, pMaterial, pComposeRes);
}

int RolesInfoMan::RoleSite::EnterInst(UINT32 roleId, UINT16 instId, SEnterInstRes* pEnterInstRes, int& dataLen, LTMSEL enterTTS, PlayerActionMan::SEnterInstItem& enterInstItem)
{
	BYTE boxNum = 0;
	BYTE brokenNum = 0;
	int offset = 0;
	char* p = (char*)pEnterInstRes->data;
	int maxBufferBytes = MAX_RES_USER_BYTES - member_offset(SEnterInstRes, restVit, data);

	//副本基本信息
	BYTE instType = 0;
	BYTE lvLimit = 0;
	UINT32 spendVit = 0;
	int retCode = GetGameCfgFileMan()->GetInstBasicInfo(instId, instType, lvLimit, spendVit);

	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	SRoleInfos* pRole = it->second;
	pRole->dropBox.boxMap.clear();
	pRole->dropProps.props.clear();
	pRole->brokenDrops.props.clear();

	enterInstItem.job = pRole->occuId;
	enterInstItem.level = pRole->level;
	enterInstItem.strNick = pRole->nick;
	enterInstItem.instId = instId;
	enterInstItem.roleId = roleId;
	enterInstItem.tts = enterTTS;

	if (retCode != MLS_OK)
		return retCode;

	//本限制角色等级判断
	if (pRole->level < lvLimit)
		return MLS_LOWER_INST_LIMIT_LEVEL;

	//体力判断
	if (pRole->vit < spendVit)
		return MLS_VIT_NOT_ENOUGH;

	//生成宝箱掉落
	retCode = GetGameCfgFileMan()->GenerateDropBox(instId, pRole->dropBox);
	if (retCode != MLS_OK)
		return retCode;

	//生成破碎物掉落
	retCode = GetGameCfgFileMan()->GenerateBrokenDrops(instId, pRole->brokenDrops);
	if (retCode != MLS_OK)
		return retCode;

	//是否有副本掉落邮件
	if (GetConfigFileMan()->IsCheckInstDropEmail() && GetEmailCleanMan()->IsHasInstDropEmail(roleId))
		return MLS_HAS_INST_DROP_EMAIL;

	//扣除体力
	pRole->vit = SGSU32Sub(pRole->vit, spendVit);

	//保存当前进入副本id
	pRole->pos.owncopy = instId;

	//复活次数清空
	pRole->reviveTimes = 0;

	//进入副本时间
	pRole->enterInstTTS = enterTTS;
	pRole->instDropGold = 0;
	pRole->instDropToken = 0;

	//填写掉落宝箱数据
	std::map<BYTE/*宝箱id*/, std::vector<SDropPropCfgItem> >& dropBox = pRole->dropBox.boxMap;
	std::map<BYTE/*宝箱id*/, std::vector<SDropPropCfgItem> >::iterator it2 = dropBox.begin();
	for (; it2 != dropBox.end(); ++it2)
	{
		std::vector<SDropPropCfgItem>& vec = it2->second;
		BYTE nCount = (BYTE)vec.size();
		if (nCount <= 0)
			continue;

		if (offset + sizeof(BYTE)* 2 + sizeof(SBoxDropPropItem)* nCount > maxBufferBytes)
			break;

		*(p + offset) = it2->first;	//宝箱id
		offset += sizeof(BYTE);

		*(p + offset) = nCount;		//宝箱中物品数量
		offset += sizeof(BYTE);
		for (BYTE i = 0; i < nCount; ++i)	//宝箱中物品
		{
			SBoxDropPropItem* pBoxDropItem = (SBoxDropPropItem*)(p + offset);
			pBoxDropItem->id = i + 1;	//物品编号
			pBoxDropItem->num = vec[i].num;
			pBoxDropItem->propId = vec[i].propId;
			pBoxDropItem->type = vec[i].type;
			pBoxDropItem->hton();
			offset += sizeof(SBoxDropPropItem);
		}
		++boxNum;
	}

	dataLen = offset;
	pEnterInstRes->boxNum = boxNum;
	pEnterInstRes->restVit = pRole->vit;

	if (offset >= maxBufferBytes)
		return MLS_OK;

	//填写破碎物掉落
	std::vector<SDropPropCfgItem>& brokenProps = pRole->brokenDrops.props;
	BYTE brokenCfgNum = (BYTE)brokenProps.size();
	for (BYTE i = 0; i < brokenCfgNum; ++i)
	{
		if (offset + sizeof(SMonsterDropPropItem) > maxBufferBytes)
			break;

		SMonsterDropPropItem* pBrokenDrop = (SMonsterDropPropItem*)(p + offset);
		pBrokenDrop->id = i;
		pBrokenDrop->num = brokenProps[i].num;
		pBrokenDrop->propId = brokenProps[i].propId;
		pBrokenDrop->type = brokenProps[i].type;
		pBrokenDrop->hton();

		++brokenNum;
		offset += sizeof(SMonsterDropPropItem);
	}
	dataLen = offset;
	pEnterInstRes->brokenNum = brokenNum;
	return MLS_OK;
}

int RolesInfoMan::RoleSite::FetchInstAreaMonsterDrops(UINT32 roleId, UINT16 instId, BYTE areaId, SFetchInstAreaDropsRes* pAreaDropsRes)
{
	BYTE idx = 0;
	BYTE maxDrops = (BYTE)((MAX_RES_USER_BYTES - member_offset(SFetchInstAreaDropsRes, monsterDrops, data)) / sizeof(SMonsterDropPropItem));

	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	SRoleInfos* pRole = it->second;
	if (pRole->pos.owncopy != instId)
		return MLS_NOT_ENTER_INST;

	pRole->dropProps.props.clear();
	pRole->brushMonster.monsterMap.clear();

	//生成掉落
	BYTE monsterDrops = 0;
	BYTE creamDrops = 0;
	BYTE bossDrops = 0;
	InstCfg::SInstDropProp& dropProps = pRole->dropProps;
	int retCode = GetGameCfgFileMan()->GenerateAreaMonsterDrops(instId, areaId, dropProps, monsterDrops, creamDrops, bossDrops);
	if (retCode != MLS_OK)
		return retCode;

	//记录刷怪表
	GetGameCfgFileMan()->GetBattleAreaMonster(instId, areaId, pRole->brushMonster);

	//记录刷怪区域id
	pRole->pos.monsterArea = areaId;

	// 日志信息
	std::stringstream log_ss;
	log_ss << "获取副本区域掉落 roleId:" << roleId << ",instId:" << instId << ",areaId:" << (int)areaId;

	//小怪掉落
	BYTE count = 0;
	BYTE n = monsterDrops;
	log_ss << " 小怪掉落";
	for (; idx < n; ++idx)
	{
		if (idx >= maxDrops)
			break;

		pAreaDropsRes->data[idx].id = idx;
		pAreaDropsRes->data[idx].num = dropProps.props[idx].num;
		pAreaDropsRes->data[idx].propId = dropProps.props[idx].propId;
		pAreaDropsRes->data[idx].type = dropProps.props[idx].type;
		pAreaDropsRes->data[idx].hton();
		log_ss << "type:" << (int)dropProps.props[idx].type << ",propId:" << dropProps.props[idx].propId << ",num:" << (int)dropProps.props[idx].num;
		++count;
	}
	pAreaDropsRes->monsterDrops = count;

	if (idx >= maxDrops)
		return MLS_OK;

	//精英怪掉落
	count = 0;
	n = SGSU8Add(monsterDrops, creamDrops);
	log_ss << " 精英掉落";
	for (; idx < n; ++idx)
	{
		if (idx >= maxDrops)
			break;

		pAreaDropsRes->data[idx].id = idx;
		pAreaDropsRes->data[idx].num = dropProps.props[idx].num;
		pAreaDropsRes->data[idx].propId = dropProps.props[idx].propId;
		pAreaDropsRes->data[idx].type = dropProps.props[idx].type;
		pAreaDropsRes->data[idx].hton();
		log_ss << "type:" << (int)dropProps.props[idx].type << ",propId:" << dropProps.props[idx].propId << ",num:" << (int)dropProps.props[idx].num;
		++count;
	}
	pAreaDropsRes->creamDrops = count;

	if (idx >= maxDrops)
		return MLS_OK;

	//boss掉落
	count = 0;
	n = SGSU8Add(n, bossDrops);
	log_ss << " boss掉落";
	for (; idx < n; ++idx)
	{
		if (idx >= maxDrops)
			break;

		pAreaDropsRes->data[idx].id = idx;
		pAreaDropsRes->data[idx].num = dropProps.props[idx].num;
		pAreaDropsRes->data[idx].propId = dropProps.props[idx].propId;
		pAreaDropsRes->data[idx].type = dropProps.props[idx].type;
		pAreaDropsRes->data[idx].hton();
		log_ss << "type:" << (int)dropProps.props[idx].type << ",propId:" << dropProps.props[idx].propId << ",num:" << (int)dropProps.props[idx].num;
		++count;
	}
	pAreaDropsRes->bossDrops = count;

	LogicLog(LOG_INFO, log_ss.str().c_str());

	return MLS_OK;
}

int RolesInfoMan::RoleSite::FinishInst(UINT32 userId, UINT32 roleId, UINT16 instId, UINT32 /*score*/, BYTE /*star*/, LTMSEL& enterTTS, const std::vector<SPropertyItem> &rewardVec, SFinishInstRes* pFinishInstRes, SReceiveRewardRefresh *pRefresh)
{
	std::string strNick;
	UINT64 toalGold = 0;
	UINT32 totalToken = 0;
	UINT32 instDropGold = 0;
	UINT32 instDropToken = 0;

	// zpy 发放限时活动奖励
	std::vector<SPropertyAttr> emailAttachmentsVec;
	{
		CXTAutoLock lock(m_rolesMapLocker);
		std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
		if (it == m_rolesMap.end())
			return MLS_ROLE_NOT_EXIST;

		SRoleInfos* pRole = it->second;
		pRole->pos.owncopy = 0;
		pRole->pos.monsterArea = 0;
		pRole->dropBox.boxMap.clear();
		pRole->dropProps.props.clear();
		pRole->brushMonster.monsterMap.clear();
		pRole->brokenDrops.props.clear();
		pRole->reviveTimes = 0;
		enterTTS = pRole->enterInstTTS;
		pRole->enterInstTTS = 0;
		instDropGold = pRole->instDropGold;
		instDropToken = pRole->instDropToken;
		pRole->instDropGold = 0;
		pRole->instDropToken = 0;
		strNick = pRole->nick;
		toalGold = BigNumberToU64(pRole->gold);
		totalToken = pRole->cash;

		//zpy 领取限时活动奖励
		it->second->ReceiveLimitedTimeActivityReward(rewardVec, pFinishInstRes, emailAttachmentsVec, pRefresh);
	}

	//zpy 装不下发邮件
	BYTE attachmentNum = (BYTE)emailAttachmentsVec.size();
	if (attachmentNum > 0)
	{
		EmailCleanMan::SSyncEmailSendContent email;
		email.type = ESGS_EMAIL_LIMITEDTIME_ACTIVITY_REWARD;
		email.receiverRoleId = roleId;
		email.receiverUserId = userId;
		email.attachmentsNum = attachmentNum;
		email.pAttachments = emailAttachmentsVec.data();
		email.pGBSenderNick = "系统";		//发送者昵称
		email.pGBCaption = "限时活动奖励";		//标题
		email.pGBBody = "限时活动奖励物品";		//正文
		GetEmailCleanMan()->AsyncSendEmail(email);
	}

	if (instDropGold > 0)
		GetConsumeProduceMan()->InputProduceRecord(ESGS_GOLD, roleId, strNick.c_str(), instDropGold, "副本掉落", toalGold);

	if (instDropToken > 0)
		GetConsumeProduceMan()->InputProduceRecord(ESGS_TOKEN, roleId, strNick.c_str(), instDropToken, "副本掉落", totalToken);

	return MLS_OK;
}

int RolesInfoMan::RoleSite::PickupInstMonsterDrop(UINT32 userId, UINT32 roleId, UINT16 instId, BYTE areaId, BYTE dropId, SPickupInstDropNotify* pNotify)
{
	std::vector<SPropertyAttr> emailAttachmentsVec;
	BYTE maxProps = (BYTE)((MAX_RES_USER_BYTES - member_offset(SBackpackUpdateNotify, propNum, props)) / sizeof(SPropertyAttr));

	{
		CXTAutoLock lock(m_rolesMapLocker);
		std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
		if (it == m_rolesMap.end())
			return MLS_ROLE_NOT_EXIST;

		SRoleInfos* pRole = it->second;
		InstCfg::SInstDropProp& dropProps = pRole->dropProps;
		BYTE si = (BYTE)dropProps.props.size();
		if (dropId >= si)
			return MLS_OK;

		//ASSERT(pRole->pos.owncopy == instId && pRole->pos.monsterArea == areaId);
		SDropPropCfgItem& item = dropProps.props[dropId];
		if (item.type == 0)
			return MLS_OK;

		switch (item.type)
		{
		case ESGS_PROP_GOLD:
			//SBigNumberAdd(pRole->gold, item.num);
			pRole->ProduceGold(item.num, "副本掉落", false);
			pRole->instDropGold = SGSU32Add(pRole->instDropGold, item.num);
			pNotify->pGold->gold = pRole->gold;
			break;
		case ESGS_PROP_TOKEN:
			//pRole->cash = SGSU32Add(pRole->cash, item.num);
			pRole->ProduceToken(item.num, "副本掉落", false);
			pRole->instDropToken = SGSU32Add(pRole->instDropToken, item.num);
			pNotify->pToken->rpcoin = pRole->cash;
			break;
		case ESGS_PROP_FRAGMENT:
			pNotify->pFragment->fragment.num = pRole->AddEquipFragment(item.propId, item.num);
			pNotify->pFragment->fragment.fragmentId = item.propId;
			break;
		case ESGS_PROP_CHAIN_SOUL_FRAGMENT:
			pNotify->pChainSoulFragment->fragment.num = pRole->AddChainSoulFragment(item.propId, item.num);
			pNotify->pChainSoulFragment->fragment.fragmentId = item.propId;
			break;
		case ESGS_PROP_EQUIP:
		case ESGS_PROP_GEM:
		case ESGS_PROP_MATERIAL:
		{
								   BYTE maxStack = GetGameCfgFileMan()->GetPropertyMaxStack(item.type, item.propId);
								   if (!pRole->CanPutIntoBackpack(item.propId, item.num, maxStack))
								   {
									   //背包放不下, 发邮件附件
									   SPropertyAttr attachmentProp;
									   memset(&attachmentProp, 0, sizeof(SPropertyAttr));
									   int retCode = GetGameCfgFileMan()->GetPropertyAttr(item.type, item.propId, attachmentProp, maxStack);
									   if (retCode == MLS_OK)
									   {
										   attachmentProp.num = (BYTE)item.num;
										   emailAttachmentsVec.push_back(attachmentProp);
									   }
								   }
								   else
								   {
									   BYTE retNum = 0;
									   pRole->PutBackpackProperty(item.type, item.propId, item.num, maxStack,
										   pNotify->pBackpack->props + pNotify->pBackpack->propNum,
										   SGSU8Sub(maxProps, pNotify->pBackpack->propNum), retNum);

									   pNotify->pBackpack->propNum = SGSU8Add(pNotify->pBackpack->propNum, retNum);
								   }
		}
			break;
		default:
			break;
		}

		item.type = 0;
	}

	//发送邮件
	BYTE attachmentNum = (BYTE)emailAttachmentsVec.size();
	if (attachmentNum > 0)
	{
		std::string instName;
		GetGameCfgFileMan()->GetInstName(instId, instName);
		instName += ", 背包满, 物品掉落";
		EmailCleanMan::SSyncEmailSendContent email;
		email.type = ESGS_EMAIL_INST_DROP;
		email.receiverRoleId = roleId;
		email.receiverUserId = userId;
		email.attachmentsNum = attachmentNum;
		email.pAttachments = emailAttachmentsVec.data();
		email.pGBSenderNick = "系统";		//发送者昵称
		email.pGBCaption = "副本怪掉落";		//标题
		email.pGBBody = instName.c_str();		//正文
		GetEmailCleanMan()->AsyncSendEmail(email);
	}
	return MLS_OK;
}

int RolesInfoMan::RoleSite::PickupInstBrokenDrop(UINT32 userId, UINT32 roleId, UINT16 instId, BYTE dropId, SPickupInstDropNotify* pNotify)
{
	std::vector<SPropertyAttr> emailAttachmentsVec;
	BYTE maxProps = (BYTE)((MAX_RES_USER_BYTES - member_offset(SBackpackUpdateNotify, propNum, props)) / sizeof(SPropertyAttr));

	{
		CXTAutoLock lock(m_rolesMapLocker);
		std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
		if (it == m_rolesMap.end())
			return MLS_ROLE_NOT_EXIST;

		SRoleInfos* pRole = it->second;
		InstCfg::SInstDropProp& dropProps = pRole->brokenDrops;
		BYTE si = (BYTE)dropProps.props.size();
		if (dropId >= si)
			return MLS_OK;

		//ASSERT(pRole->pos.owncopy == instId && pRole->pos.monsterArea == areaId);
		SDropPropCfgItem& item = dropProps.props[dropId];
		if (item.type == 0)
			return MLS_OK;

		switch (item.type)
		{
		case ESGS_PROP_GOLD:
			//SBigNumberAdd(pRole->gold, item.num);
			pRole->ProduceGold(item.num, "副本破碎物掉落", false);
			pRole->instDropGold = SGSU32Add(pRole->instDropGold, item.num);
			pNotify->pGold->gold = pRole->gold;
			break;
		case ESGS_PROP_TOKEN:
			pRole->ProduceToken(item.num, "副本破碎物掉落", false);
			pRole->instDropToken = SGSU32Add(pRole->instDropToken, item.num);
			//pRole->cash = SGSU32Add(pRole->cash, item.num);
			pNotify->pToken->rpcoin = pRole->cash;
			break;
		case ESGS_PROP_FRAGMENT:
			pNotify->pFragment->fragment.num = pRole->AddEquipFragment(item.propId, item.num);
			pNotify->pFragment->fragment.fragmentId = item.propId;
			break;
		case ESGS_PROP_CHAIN_SOUL_FRAGMENT:
			pNotify->pChainSoulFragment->fragment.num = pRole->AddChainSoulFragment(item.propId, item.num);
			pNotify->pChainSoulFragment->fragment.fragmentId = item.propId;
			break;
		case ESGS_PROP_EQUIP:
		case ESGS_PROP_GEM:
		case ESGS_PROP_MATERIAL:
		{
								   BYTE maxStack = GetGameCfgFileMan()->GetPropertyMaxStack(item.type, item.propId);
								   if (!pRole->CanPutIntoBackpack(item.propId, item.num, maxStack))
								   {
									   //背包放不下, 发邮件附件
									   SPropertyAttr attachmentProp;
									   memset(&attachmentProp, 0, sizeof(SPropertyAttr));
									   int retCode = GetGameCfgFileMan()->GetPropertyAttr(item.type, item.propId, attachmentProp, maxStack);
									   if (retCode == MLS_OK)
									   {
										   attachmentProp.num = (BYTE)item.num;
										   emailAttachmentsVec.push_back(attachmentProp);
									   }
								   }
								   else
								   {
									   BYTE retNum = 0;
									   pRole->PutBackpackProperty(item.type, item.propId, item.num, maxStack,
										   pNotify->pBackpack->props + pNotify->pBackpack->propNum,
										   SGSU8Sub(maxProps, pNotify->pBackpack->propNum), retNum);

									   pNotify->pBackpack->propNum = SGSU8Add(pNotify->pBackpack->propNum, retNum);
								   }
		}
			break;
		default:
			break;
		}

		item.type = 0;
	}

	//发送邮件
	BYTE attachmentNum = (BYTE)emailAttachmentsVec.size();
	if (attachmentNum > 0)
	{
		std::string instName;
		GetGameCfgFileMan()->GetInstName(instId, instName);
		instName += ", 背包满, 物品掉落";
		EmailCleanMan::SSyncEmailSendContent email;
		email.type = ESGS_EMAIL_INST_DROP;
		email.receiverRoleId = roleId;
		email.receiverUserId = userId;
		email.attachmentsNum = attachmentNum;
		email.pAttachments = emailAttachmentsVec.data();
		email.pGBSenderNick = "系统";		//发送者昵称
		email.pGBCaption = "副本破碎物掉落";		//标题
		email.pGBBody = instName.c_str();		//正文
		GetEmailCleanMan()->AsyncSendEmail(email);
	}
	return MLS_OK;
}

int RolesInfoMan::RoleSite::PickupInstBoxDrop(UINT32 userId, UINT32 roleId, UINT16 instId, BYTE boxId, BYTE id, SPickupInstDropNotify* pNotify)
{
	std::vector<SPropertyAttr> emailAttachmentsVec;
	BYTE maxProps = (BYTE)((MAX_RES_USER_BYTES - member_offset(SBackpackUpdateNotify, propNum, props)) / sizeof(SPropertyAttr));

	{
		CXTAutoLock lock(m_rolesMapLocker);
		std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
		if (it == m_rolesMap.end())
			return MLS_ROLE_NOT_EXIST;

		SRoleInfos* pRole = it->second;
		std::map<BYTE/*宝箱id*/, std::vector<SDropPropCfgItem> >& dropBox = it->second->dropBox.boxMap;
		//ASSERT(pRole->pos.owncopy == instId);
		std::map<BYTE/*宝箱id*/, std::vector<SDropPropCfgItem> >::iterator it2 = dropBox.find(boxId);
		if (it2 == dropBox.end())
			return MLS_OK;

		BYTE i = 0;
		BYTE si = 0;
		std::vector<SDropPropCfgItem>& itemVec = it2->second;
		if (id == 0)	//拾取宝箱全部物品
		{
			i = 0;
			si = (BYTE)itemVec.size();
		}
		else	//拾取宝箱某个物品
		{
			i = SGSU8Sub(id, 1);
			if (i >= itemVec.size())
				return MLS_OK;
			si = i + 1;
		}

		for (; i < si; ++i)
		{
			SDropPropCfgItem& item = itemVec[i];
			switch (item.type)
			{
			case ESGS_PROP_GOLD:
				//SBigNumberAdd(pRole->gold, item.num);
				pRole->ProduceGold(item.num, "副本宝箱掉落", false);
				pRole->instDropGold = SGSU32Add(pRole->instDropGold, item.num);
				pNotify->pGold->gold = pRole->gold;
				break;
			case ESGS_PROP_TOKEN:
				//pRole->cash = SGSU32Add(pRole->cash, item.num);
				pRole->ProduceToken(item.num, "副本宝箱掉落", false);
				pRole->instDropToken = SGSU32Add(pRole->instDropToken, item.num);
				pNotify->pToken->rpcoin = pRole->cash;
				break;
			case ESGS_PROP_FRAGMENT:
				pNotify->pFragment->fragment.num = pRole->AddEquipFragment(item.propId, item.num);
				pNotify->pFragment->fragment.fragmentId = item.propId;
				break;
			case ESGS_PROP_CHAIN_SOUL_FRAGMENT:
				pNotify->pChainSoulFragment->fragment.num = pRole->AddChainSoulFragment(item.propId, item.num);
				pNotify->pChainSoulFragment->fragment.fragmentId = item.propId;
				break;
			case ESGS_PROP_EQUIP:
			case ESGS_PROP_GEM:
			case ESGS_PROP_MATERIAL:
			{
									   BYTE maxStack = GetGameCfgFileMan()->GetPropertyMaxStack(item.type, item.propId);
									   if (!pRole->CanPutIntoBackpack(item.propId, item.num, maxStack))
									   {
										   //背包放不下, 发邮件附件
										   SPropertyAttr attachmentProp;
										   memset(&attachmentProp, 0, sizeof(SPropertyAttr));
										   int retCode = GetGameCfgFileMan()->GetPropertyAttr(item.type, item.propId, attachmentProp, maxStack);
										   if (retCode == MLS_OK)
										   {
											   attachmentProp.num = (BYTE)item.num;
											   emailAttachmentsVec.push_back(attachmentProp);
										   }
									   }
									   else
									   {
										   BYTE retNum = 0;
										   pRole->PutBackpackProperty(item.type, item.propId, item.num, maxStack,
											   pNotify->pBackpack->props + pNotify->pBackpack->propNum,
											   SGSU8Sub(maxProps, pNotify->pBackpack->propNum), retNum);

										   pNotify->pBackpack->propNum = SGSU8Add(pNotify->pBackpack->propNum, retNum);
									   }
			}
				break;
			default:
				break;
			}
			item.type = 0;
		}

		//移除打开的宝箱
		if (id == 0)
			dropBox.erase(it2);
	}

	//发送邮件
	BYTE attachmentNum = (BYTE)emailAttachmentsVec.size();
	if (attachmentNum > 0)
	{
		std::string instName;
		GetGameCfgFileMan()->GetInstName(instId, instName);
		instName += ", 背包满, 物品掉落";
		EmailCleanMan::SSyncEmailSendContent email;
		email.type = ESGS_EMAIL_INST_DROP;
		email.receiverRoleId = roleId;
		email.receiverUserId = userId;
		email.attachmentsNum = attachmentNum;
		email.pAttachments = emailAttachmentsVec.data();
		email.pGBSenderNick = "系统";		//发送者昵称
		email.pGBCaption = "副本宝箱掉落";		//标题
		email.pGBBody = instName.c_str();		//正文
		GetEmailCleanMan()->AsyncSendEmail(email);
	}
	return MLS_OK;
}

int RolesInfoMan::RoleSite::KillMonsterExp(UINT32 roleId, UINT16 instId, BYTE /*areaId*/, UINT16 monsterId, BYTE monsterLv, BOOL& isUpgrade)
{
	isUpgrade = false;
	INT64 monsterBasicExp = GetGameCfgFileMan()->GetMonsterExp(monsterId);
	if (monsterBasicExp == 0)
		return MLS_OK;

	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	SRoleInfos* pRole = it->second;

	//查找怪id
	std::map<UINT16/*怪id*/, InstCfg::SBrushMonster>::iterator it2 = pRole->brushMonster.monsterMap.find(monsterId);
	if (it2 == pRole->brushMonster.monsterMap.end())
		return MLS_NOT_EXIST_MONSTER;

	//查找怪等级
	InstCfg::SBrushMonster& monsterRef = it2->second;
	std::map<BYTE/*怪等级*/, UINT16/*数量*/>::iterator it3 = monsterRef.lvMonsterMap.find(monsterLv);
	if (it3 == monsterRef.lvMonsterMap.end())
		return MLS_NOT_EXIST_MONSTER;

	//杀完则清空怪数量
	it3->second = SGSU16Sub(it3->second, 1);
	if (it3->second == 0)
		monsterRef.lvMonsterMap.erase(it3);

	if (monsterRef.lvMonsterMap.empty())
		pRole->brushMonster.monsterMap.erase(it2);

	/* 2015.12.16郑一修改  */
	// monsterBasicExp = monsterBasicExp * monsterLv + GetGameCfgFileMan()->GetExpCorrectVal();
	InstCfg::SInstAttrCfg *config = NULL;
	if (MLS_OK == GetGameCfgFileMan()->GetInstConfig(instId, config))
	{
		monsterBasicExp = monsterBasicExp * config->limitRoleLevel + GetGameCfgFileMan()->GetExpCorrectVal();
		monsterBasicExp *= config->degree;
		if (monsterBasicExp > 0)
			pRole->AddRoleExp(monsterBasicExp);
	}

	//判断是否升级
	BYTE newLv = 0;
	GetGameCfgFileMan()->JudgeRoleUpgrade(pRole->occuId, pRole->level, BigNumberToU64(pRole->exp), newLv);
	if (newLv <= pRole->level)
		return MLS_OK;

	//开放新技能
	GetGameCfgFileMan()->GetRoleAllInitSkills(pRole->occuId, pRole->level, newLv, pRole->studySkills);

	//新等级角色hp/mp
	SRoleAttrPoint attr;
	if (MLS_OK == GetGameCfgFileMan()->GetRoleCfgBasicAttr(pRole->occuId, newLv, attr))
	{
		pRole->hp = attr.u32HP;
		pRole->mp = attr.u32MP;
		pRole->attack = attr.attack;
		pRole->def = attr.def;
	}
	//新等级
	pRole->level = newLv;
	isUpgrade = true;

	//重新计算战力
	pRole->CalcRoleCombatPower();

	return MLS_OK;
}

/* zpy 成就系统新增 */
int RolesInfoMan::RoleSite::GetMaxGemLevel(UINT32 roleId, BYTE &max_level)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	max_level = 0;

	// 遍历背包
	for (std::vector<SPropertyAttr>::const_iterator itr = it->second->packs.begin(); itr != it->second->packs.end(); ++itr)
	{
		if (itr->type == ESGS_PROP_GEM && itr->level > max_level)
		{
			max_level = itr->level;
		}
	}

	// 遍历装备位
	GemCfg::SGemAttrCfg gem_attr;
	for (std::vector<SEquipPosAttr>::const_iterator itr = it->second->equipPos.begin(); itr != it->second->equipPos.end(); ++itr)
	{
		for (int i = 0; i < MAX_GEM_HOLE_NUM; ++i)
		{
			if (GetGameCfgFileMan()->GetGemCfgAttr(itr->gem[i], gem_attr) != MLS_PROPERTY_NOT_EXIST)
			{
				if (gem_attr.level > max_level)
				{
					max_level = gem_attr.level;
				}
			}
		}
	}

	return MLS_OK;
}

/* zpy 成就系统新增 */
int RolesInfoMan::RoleSite::GetInlaidGemLevelSum(UINT32 roleId, UINT32 &sum)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	sum = 0;

	// 遍历装备位
	GemCfg::SGemAttrCfg gem_attr;
	for (std::vector<SEquipPosAttr>::const_iterator itr = it->second->equipPos.begin(); itr != it->second->equipPos.end(); ++itr)
	{
		for (int i = 0; i < MAX_GEM_HOLE_NUM; ++i)
		{
			if (GetGameCfgFileMan()->GetGemCfgAttr(itr->gem[i], gem_attr) != MLS_PROPERTY_NOT_EXIST)
			{
				sum += gem_attr.level;
			}
		}
	}

	return MLS_OK;
}

/* zpy 成就系统新增 */
int RolesInfoMan::RoleSite::GetEquipStarLevelSum(UINT32 roleId, UINT32 &sum)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	sum = 0;

	// 遍历装备位
	GemCfg::SGemAttrCfg gem_attr;
	for (std::vector<SEquipPosAttr>::const_iterator itr = it->second->equipPos.begin(); itr != it->second->equipPos.end(); ++itr)
	{
		sum += itr->star;
	}

	return MLS_OK;
}

/* zpy 成就系统新增 */
int RolesInfoMan::RoleSite::GetSkillLevelSum(UINT32 roleId, UINT32 &sum)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	sum = 0;

	// 遍历角色技能
	for (std::vector<SSkillMoveInfo>::const_iterator itr = it->second->studySkills.begin(); itr != it->second->studySkills.end(); ++itr)
	{
		if (GetGameCfgFileMan()->IsSkill(it->second->occuId, itr->id))
		{
			sum += itr->level;
		}
	}

	return MLS_OK;
}

int RolesInfoMan::RoleSite::CashProperty(UINT32 roleId, UINT16 cashId, SCashPropertyRes* pCashRes)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

#pragma compiling_remind("兑换道具从配置文件中获取暂略")
	return MLS_OK;
}

int RolesInfoMan::RoleSite::QueryRoleSkills(UINT32 roleId, SQuerySkillsRes* pSkills)
{
	BYTE maxSkills = (BYTE)((MAX_RES_USER_BYTES - member_offset(SQuerySkillsRes, skillNum, skills)) / sizeof(SSkillMoveInfo));

	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	SRoleInfos* pRoleInfos = it->second;
	pSkills->skillNum = min(maxSkills, (BYTE)pRoleInfos->studySkills.size());
	for (BYTE i = 0; i < pSkills->skillNum; ++i)
	{
		memcpy(&pSkills->skills[i], &pRoleInfos->studySkills[i], sizeof(SSkillMoveInfo));
		pSkills->skills[i].hton();
	}
	return MLS_OK;
}

int RolesInfoMan::RoleSite::SkillUpgrade(UINT32 roleId, UINT16 skillId, BYTE step, SSkillUpgradeRes* pUpgradeRes)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	return it->second->SkillUpgrade(skillId, pUpgradeRes);
}

int RolesInfoMan::RoleSite::ChangeFaction(UINT32 roleId, BYTE factionId, BOOL& needSyncDB)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	if (it->second->factionId == factionId)
		needSyncDB = false;
	else
	{
		it->second->factionId = factionId;
		needSyncDB = true;
	}

	return MLS_OK;
}

int RolesInfoMan::RoleSite::LoadSkill(UINT32 roleId, UINT16 skillId, BYTE key, BOOL& needSyncDB)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	std::vector<SSkillMoveInfo>& skills = it->second->studySkills;
	BYTE si = (BYTE)skills.size();
	for (BYTE i = 0; i < si; ++i)
	{
		if (skills[i].id == skillId)
		{
			needSyncDB = skills[i].key == key ? false : true;
			skills[i].key = key;
			return MLS_OK;
		}
	}
	return MLS_SKILL_ID_NOT_EXIST;
}

int RolesInfoMan::RoleSite::CancelSkill(UINT32 roleId, UINT16 skillId, BOOL& needSyncDB)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	std::vector<SSkillMoveInfo>& skills = it->second->studySkills;
	BYTE si = (BYTE)skills.size();
	for (BYTE i = 0; i < si; ++i)
	{
		if (skills[i].id == skillId)
		{
			needSyncDB = skills[i].key == 0 ? false : true;
			skills[i].key = 0;
			return MLS_OK;
		}
	}
	return MLS_SKILL_ID_NOT_EXIST;
}

int RolesInfoMan::RoleSite::GetDailyTasks(UINT32 roleId, SGetDailyTaskRes* pDailyTasks)
{
	BYTE maxTasks = (BYTE)((MAX_RES_USER_BYTES - member_offset(SGetDailyTaskRes, num, dailyTasks)) / sizeof(UINT16));

	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	return it->second->GetDailyTasks(roleId, pDailyTasks, maxTasks);
}

int RolesInfoMan::RoleSite::UpdateTaskTarget(UINT32 roleId, UINT16 taskId, const UINT32* pTarget, int n/* = MAX_TASK_TARGET_NUM*/)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	return it->second->UpdateTaskTarget(taskId, pTarget, n);
}

int RolesInfoMan::RoleSite::ReceiveTaskReward(UINT32 userId, UINT32 roleId, UINT16 taskId, SReceiveTaskRewardRes* pTaskRewardRes)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	return it->second->ReceiveTaskReward(userId, roleId, taskId, pTaskRewardRes);
}

int RolesInfoMan::RoleSite::ReceiveEmailAttachments(UINT32 roleId, BYTE emailType, const SPropertyAttr* pAttachmentsProp, BYTE attachmentsNum, SReceiveEmailAttachmentsRes* pEmailAttachmentsRes, BOOL& isUpgrade)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	return it->second->ReceiveEmailAttachments(emailType, pAttachmentsProp, attachmentsNum, pEmailAttachmentsRes, isUpgrade);
}

int RolesInfoMan::RoleSite::RechargeToken(UINT32 roleId, UINT16 cashId, SRechargeRes* pRechargeRes, char* pszRoleNick)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	RechargeCfg::SRechargeCfgAttr cashAttr;
	int retCode = GetGameCfgFileMan()->GetRechargeAttr(cashId, cashAttr);
	if (retCode != MLS_OK)
		return retCode;

	SRoleInfos* pRole = it->second;

	//添加代币
	//it->second->cash = SGSU32Add(it->second->cash, cashAttr.rpcoin);
	pRole->ProduceToken(cashAttr.rpcoin, "充值");

	//增加充值总额(RMB)
	pRole->cashcount = SGSU32Add(pRole->cashcount, cashAttr.rmb);

	pRechargeRes->rpcoin = pRole->cash;
	pRechargeRes->cashcount = pRole->cashcount;

	if (pszRoleNick != NULL)
		strcpy(pszRoleNick, pRole->nick);

	return MLS_OK;
}

int RolesInfoMan::RoleSite::SetRoleTitleId(UINT32 roleId, UINT16 titleId)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	SRoleInfos* pRole = it->second;

	//设置称号
	pRole->titleId = titleId;

	//重新计算战力
	pRole->CalcRoleCombatPower();
	return MLS_OK;
}

int RolesInfoMan::RoleSite::MallBuyGoods(UINT32 roleId, UINT32 buyId, UINT16 num, SMallBuyGoodsRes* pBuyGoodsRes, char* pszRoleNick, UINT32& spendToken)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	if (pszRoleNick != NULL)
		strcpy(pszRoleNick, it->second->nick);

	return it->second->MallBuyGoods(buyId, num, pBuyGoodsRes, spendToken);
}

int RolesInfoMan::RoleSite::EquipmentCompose(UINT32 roleId, UINT16 composeId, SEquipComposeRes* pComposeRes)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	return it->second->EquipmentCompose(composeId, pComposeRes);
}

int RolesInfoMan::RoleSite::QueryRoleDress(UINT32 roleId, SQueryRoleDressRes* pDressRes)
{
	int maxNum = (MAX_RES_USER_BYTES - member_offset(SQueryRoleDressRes, num, dressIds)) / sizeof(UINT16);

	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	std::vector<UINT16>& dressVec = it->second->dress;
	int si = min(maxNum, (int)dressVec.size());
	memcpy(pDressRes->dressIds, dressVec.data(), si * sizeof(UINT16));
	pDressRes->num = (UINT16)si;
	return MLS_OK;
}

int RolesInfoMan::RoleSite::WearDress(UINT32 roleId, UINT16 dressId)
{
	int retCode = MLS_DRESS_NOT_EXIST;

	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	std::vector<UINT16>& dressVec = it->second->dress;
#if 0
	if (dressId == 0)
	{
		if (it->second->dressId != 0)
			dressVec.push_back(it->second->dressId);

		it->second->dressId = 0;
	}
	else
	{
		std::vector<UINT16>::iterator it2 = dressVec.begin();
		for (; it2 != dressVec.end(); ++it2)
		{
			if (*it2 == dressId)
			{
				retCode = MLS_OK;
				break;
			}
		}

		if (retCode == MLS_OK)
		{
			dressVec.erase(it2);

			if (it->second->dressId != 0)
				dressVec.push_back(it->second->dressId);
			it->second->dressId = dressId;
		}
	}
#else
	if (dressId == 0)
	{
		it->second->dressId = 0;
		//重新计算战力
		it->second->CalcRoleCombatPower();
		return MLS_OK;
	}

	int si = (int)dressVec.size();
	for (int i = 0; i < si; ++i)
	{
		if (dressVec[i] == dressId)
		{
			retCode = MLS_OK;
			break;
		}
	}

	if (retCode == MLS_OK)
	{
		it->second->dressId = dressId;
		//重新计算战力
		it->second->CalcRoleCombatPower();
	}
#endif
	return retCode;
}

int RolesInfoMan::RoleSite::QueryEquipFragment(UINT32 roleId, SQueryEquipFragmentRes* pFragmentRes)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	std::vector<SEquipFragmentAttr>& fragmentVec = it->second->fragments;
	BYTE si = (BYTE)fragmentVec.size();
	for (BYTE i = 0; i < si; ++i)
	{
		pFragmentRes->data[i].fragmentId = fragmentVec[i].fragmentId;
		pFragmentRes->data[i].num = fragmentVec[i].num;
	}
	pFragmentRes->num = si;
	return MLS_OK;
}

/* zpy 查询炼魂碎片 */
int RolesInfoMan::RoleSite::QueryChainSoulFragment(UINT32 roleId, SQueryChainSoulFragmentRes* pFragmentRes)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	std::vector<SChainSoulFragmentAttr>& fragmentVec = it->second->chainSoulFragments;
	BYTE si = (BYTE)fragmentVec.size();
	for (BYTE i = 0; i < si; ++i)
	{
		pFragmentRes->data[i].fragmentId = fragmentVec[i].fragmentId;
		pFragmentRes->data[i].num = fragmentVec[i].num;
	}
	pFragmentRes->num = si;
	return MLS_OK;
}

/* zpy 查询炼魂部件 */
int RolesInfoMan::RoleSite::QueryChainSoulPos(UINT32 roleId, SQueryChainSoulPosRes* pChainSoulPos)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	ASSERT(it->second->chainSoulPos.size() == MAX_CHAIN_SOUL_HOLE_NUM);

	// 构造数据
	BYTE min_level = UCHAR_MAX;
	std::vector<SChainSoulPosAttr> &chainSoulPos = it->second->chainSoulPos;
	for (size_t i = 0; i < chainSoulPos.size(); ++i)
	{
		pChainSoulPos->pos[i].pos = chainSoulPos[i].pos;
		pChainSoulPos->pos[i].level = chainSoulPos[i].level;
		if (pChainSoulPos->pos[i].level < min_level)
		{
			min_level = pChainSoulPos->pos[i].level;
		}
	}

	// 计算圣物核心等级
	pChainSoulPos->core_level = GetGameCfgFileMan()->GetCoreLevelByConditionLevel(min_level);

	return MLS_OK;
}

/* zpy 升级炼魂部件 */
int RolesInfoMan::RoleSite::ChainSoulPosUpgrade(UINT32 roleId, BYTE pos, SChainSoulPosUpgradeRes* pChainSoulPosUpgrade)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	ASSERT(it->second->chainSoulPos.size() == MAX_CHAIN_SOUL_HOLE_NUM);

	// 升级部件是否合法
	if (pos < 1 || pos > 6)
	{
		return MLS_INVALID_CHAINSOUL_POS;
	}

	// 碎片是否充足
	BYTE index = pos - 1;
	BYTE level = it->second->chainSoulPos[index].level;
	GreedTheLiveCfg::ChainSoulPosConfig *pConfig = NULL;
	if (!GetGameCfgFileMan()->GetChainSoulPosConfig(pos, level + 1, pConfig))
	{
		return MLS_UPGRADE_CHAINSOUL_POS_FAIL;
	}

	for (size_t i = 0; i < pConfig->cost.size(); ++i)
	{
		if (it->second->GetChainSoulFragmentNum(pConfig->cost[i].id) < pConfig->cost[i].num)
		{
			return MLS_CHAINSOUL_FRAGMENT_NOT_ENOUGH;
		}
	}

	// 扣除碎片
	for (size_t i = 0; i < pConfig->cost.size(); ++i)
	{
		it->second->DropChainSoulFragment(pConfig->cost[i].id, pConfig->cost[i].num);
	}

	// 部件升级
	++it->second->chainSoulPos[index].level;

	// 计算炼魂核心等级
	BYTE min_level = UCHAR_MAX;
	std::vector<SChainSoulPosAttr> &chainSoulPos = it->second->chainSoulPos;
	for (size_t i = 0; i < chainSoulPos.size(); ++i)
	{
		if (chainSoulPos[i].level < min_level)
		{
			min_level = chainSoulPos[i].level;
		}
	}
	pChainSoulPosUpgrade->core_level = GetGameCfgFileMan()->GetCoreLevelByConditionLevel(min_level);

	// 构造返回数据
	pChainSoulPosUpgrade->attr.pos = pos;
	pChainSoulPosUpgrade->attr.level = it->second->chainSoulPos[index].level;
	std::vector<SChainSoulFragmentAttr>& fragmentVec = it->second->chainSoulFragments;
	BYTE size = (BYTE)fragmentVec.size();
	for (BYTE i = 0; i < size; ++i)
	{
		pChainSoulPosUpgrade->data[i].fragmentId = fragmentVec[i].fragmentId;
		pChainSoulPosUpgrade->data[i].num = fragmentVec[i].num;
	}
	pChainSoulPosUpgrade->num = size;

	return MLS_OK;
}

/* zpy 复活角色 */
int RolesInfoMan::RoleSite::ResurrectionInWorldBoss(UINT32 roleId, UINT16 reviveId, UINT32 consume_count, UINT32 &out_rpcoin)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	//获取vip增加次数
	UINT16 vipAddTimes = 0;
	/*BYTE vipLv = GetGameCfgFileMan()->CalcVIPLevel(it->second->cashcount);
	if(vipLv > 0)
	GetGameCfgFileMan()->GetVIPVitExchangeTimes(vipLv, vipAddTimes);*/

	UINT32 spendToken = 0;
	BYTE n = SGSU8Add(consume_count, 1);
	int retCode = GetGameCfgFileMan()->GetReviveConsume(reviveId, n, vipAddTimes, spendToken);
	if (retCode != MLS_OK)
		return retCode;

	if (it->second->cash < spendToken)
		return MLS_CASH_NOT_ENOUGH;

	//扣除代币
	it->second->ConsumeToken(spendToken, "世界Boss复活");
	out_rpcoin = it->second->cash;

	return MLS_OK;
}

/* zpy 购买竞技模式进入次数 */
int RolesInfoMan::RoleSite::BuyCompModeEnterNum(UINT32 roleId, int purchased, SBuyCompEnterNumRes *pEnterNum)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	/**
	* 策划文档原文：
	* GameType.z.Expend(OBJ) 字段中根据玩法取对应值，,"Buy":[10,20,30,40] 为购买额外挑战次数消耗
	*/
	UINT16 buyKOFTimes = 0;
	BYTE vipLv = GetGameCfgFileMan()->CalcVIPLevel(it->second->cashcount);
	GetGameCfgFileMan()->GetVIPBuyKOFTimes(vipLv, buyKOFTimes);
	if (purchased >= buyKOFTimes)
	{
		return MLS_VIP_BUY_ACHIEVE_UPPER;
	}

	// 获取价格
	UINT32 price = GetGameCfgFileMan()->GetGameTypeBuyPriceExpend(ESGS_INST_COMPETITIVEMODE, purchased + 1);
	if (it->second->cash < price)
		return MLS_CASH_NOT_ENOUGH;

	//扣除代币
	it->second->ConsumeToken(price, "购买竞技模式进入次数");
	pEnterNum->rpcoin = it->second->cash;
	pEnterNum->buy_num = purchased + 1;

	return MLS_OK;
}

int RolesInfoMan::RoleSite::BuyAllCompModeEnterNum(UINT32 roleId, int purchased, int upper_num, SBuyCompEnterNumRes *pEnterNum)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	/**
	* 策划文档原文：
	* GameType.z.Expend(OBJ) 字段中根据玩法取对应值，,"Buy":[10,20,30,40] 为购买额外挑战次数消耗
	*/
	UINT16 buyKOFTimes = 0;
	BYTE vipLv = GetGameCfgFileMan()->CalcVIPLevel(it->second->cashcount);
	GetGameCfgFileMan()->GetVIPBuyKOFTimes(vipLv, buyKOFTimes);
	if (purchased >= buyKOFTimes)
	{
		return MLS_VIP_BUY_ACHIEVE_UPPER;
	}

	// 取小者为上限
	if (buyKOFTimes < upper_num)
	{
		upper_num = buyKOFTimes;
	}

	// 获取价格
	UINT32 price_sum = 0;
	for (int i = purchased; i < upper_num; ++i)
	{
		price_sum += GetGameCfgFileMan()->GetGameTypeBuyPriceExpend(ESGS_INST_COMPETITIVEMODE, i + 1);
	}
	if (it->second->cash < price_sum)
		return MLS_CASH_NOT_ENOUGH;

	//扣除代币
	it->second->ConsumeToken(price_sum, "购买竞技模式进入次数");
	pEnterNum->rpcoin = it->second->cash;
	pEnterNum->buy_num = upper_num;

	return MLS_OK;
}

/* zpy 购买1vs1模式进入次数 */
int RolesInfoMan::RoleSite::BuyoneVsOneEnterNum(UINT32 roleId, int purchased, int num, SBuy1V1EnterNumRes *pBuyEnter)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	// 获取价格
	UINT32 price_sum = 0;
	for (int i = 0; i < num; ++i)
	{
		price_sum += GetGameCfgFileMan()->GetGameTypeBuyPriceExpend(ESGS_INST_ONE_VS_ONE, purchased + i + 1);
	}
	if (it->second->cash < price_sum)
		return MLS_CASH_NOT_ENOUGH;

	//扣除代币
	it->second->ConsumeToken(price_sum, "购买1V1模式进入次数");
	pBuyEnter->rpcoin = it->second->cash;
	pBuyEnter->buy_num = purchased + num;

	return MLS_OK;
}

int RolesInfoMan::RoleSite::RoleRevive(UINT32 roleId, UINT16 reviveId, SReviveRoleRes* pReviveRes)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	return it->second->RoleRevive(reviveId, pReviveRes);
}

int RolesInfoMan::RoleSite::QueryGoldExchange(UINT32 roleId, SQueryGoldExchangeRes* pExchangeRes)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	pExchangeRes->exchangedTimes = it->second->exchangedTimes;
	return MLS_OK;
}

int RolesInfoMan::RoleSite::GoldExchange(UINT32 roleId, SGoldExchangeRes* pExchangeRes)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	return it->second->GoldExchange(pExchangeRes);
}

/* zpy 体力兑换 */
int RolesInfoMan::RoleSite::QueryVitExchange(UINT32 roleId, SQueryVitExchangeRes* pExchangeRes)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	pExchangeRes->exchangedTimes = it->second->vitExchangedTimes;
	return MLS_OK;

}

/* zpy 体力兑换 */
int RolesInfoMan::RoleSite::VitExchange(UINT32 roleId, SVitExchangeRes* pExchangeRes)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	return it->second->VitExchange(pExchangeRes);
}

int RolesInfoMan::RoleSite::GetRecommendFriends(UINT32 userId, UINT32 roleId, SGetRecommendFriendsRes* pFriendsRes)
{
	BYTE level = 0;
	UINT32 nTimes = 0;
	{
		CXTAutoLock lock(m_rolesMapLocker);
		std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
		if (it == m_rolesMap.end())
			return MLS_ROLE_NOT_EXIST;
		level = it->second->level;
		nTimes = it->second->recommendFriendTimes;
		it->second->recommendFriendTimes = SGSU32Add(nTimes, 1);
	}

	return GetFriendMan()->GetRecommendFriends(level, nTimes, userId, pFriendsRes);
}

int RolesInfoMan::RoleSite::ReceiveVIPReward(UINT32 userId, UINT32 roleId, BYTE vipLv, SReceiveVIPRewardRes* pVIPRewardRes, SReceiveRewardRefresh* pRefresh)
{
	std::vector<SPropertyAttr> emailAttachmentsVec;
	{
		CXTAutoLock lock(m_rolesMapLocker);
		std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
		if (it == m_rolesMap.end())
			return MLS_ROLE_NOT_EXIST;

		it->second->ReceiveVIPReward(vipLv, pVIPRewardRes, emailAttachmentsVec, pRefresh);
	}

	//是否需要发送邮件
	BYTE attachmentNum = (BYTE)emailAttachmentsVec.size();
	if (attachmentNum > 0)
	{
		EmailCleanMan::SSyncEmailSendContent email;
		email.type = ESGS_EMAIL_VIP_REWARD;
		email.receiverRoleId = roleId;
		email.receiverUserId = userId;
		email.attachmentsNum = attachmentNum;
		email.pAttachments = emailAttachmentsVec.data();
		email.pGBSenderNick = "系统";		//发送者昵称
		email.pGBCaption = "VIP奖励";		//标题
		email.pGBBody = "VIP奖励物品";		//正文
		GetEmailCleanMan()->AsyncSendEmail(email);
	}

	return MLS_OK;
}

/* zpy 成就系统新增 */
int RolesInfoMan::RoleSite::ReceiveAchievementReward(UINT32 userId, UINT32 roleId, UINT32 achievement_id, SReceiveRewardRes *pAchievementReward, SReceiveRewardRefresh *pRefresh)
{
	std::vector<SPropertyAttr> emailAttachmentsVec;
	{
		CXTAutoLock lock(m_rolesMapLocker);
		std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
		if (it == m_rolesMap.end())
			return MLS_ROLE_NOT_EXIST;

		it->second->ReceiveAchievementReward(achievement_id, pAchievementReward, emailAttachmentsVec, pRefresh);
	}

	//是否需要发送邮件
	BYTE attachmentNum = (BYTE)emailAttachmentsVec.size();
	if (attachmentNum > 0)
	{
		EmailCleanMan::SSyncEmailSendContent email;
		email.type = ESGS_EMAIL_ACHIEVEMENT_REWARD;
		email.receiverRoleId = roleId;
		email.receiverUserId = userId;
		email.attachmentsNum = attachmentNum;
		email.pAttachments = emailAttachmentsVec.data();
		email.pGBSenderNick = "系统";		//发送者昵称
		email.pGBCaption = "成就奖励";		//标题
		email.pGBBody = "成就奖励物品";		//正文
		GetEmailCleanMan()->AsyncSendEmail(email);
	}

	return MLS_OK;
}

/* zpy 领取每日签到奖励 */
int RolesInfoMan::RoleSite::ReceiveDailySignReward(UINT32 userId, UINT32 roleId, BYTE days, bool continuous, SRoleDailySignRes *pRoleSign, SReceiveRewardRefresh *pRefresh)
{
	std::vector<SPropertyAttr> emailAttachmentsVec;
	{
		CXTAutoLock lock(m_rolesMapLocker);
		std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
		if (it == m_rolesMap.end())
			return MLS_ROLE_NOT_EXIST;

		it->second->ReceiveDailySignReward(days, continuous, pRoleSign, emailAttachmentsVec, pRefresh);
	}

	//是否需要发送邮件
	BYTE attachmentNum = (BYTE)emailAttachmentsVec.size();
	if (attachmentNum > 0)
	{
		EmailCleanMan::SSyncEmailSendContent email;
		email.type = ESGS_EMAIL_SIGN_IN_REWARD;
		email.receiverRoleId = roleId;
		email.receiverUserId = userId;
		email.attachmentsNum = attachmentNum;
		email.pAttachments = emailAttachmentsVec.data();
		email.pGBSenderNick = "系统";		//发送者昵称
		email.pGBCaption = "每日签到奖励";		//标题
		email.pGBBody = "每日签到奖励物品";		//正文
		GetEmailCleanMan()->AsyncSendEmail(email);
	}

	return MLS_OK;
}

//add by hxw 20151022

BOOL RolesInfoMan::RoleSite::IsGetStrongerReward(UINT32 roleId, UINT16 rewardid)
{
	std::vector<SPropertyAttr> emailAttachmentsVec;
	{
		CXTAutoLock lock(m_rolesMapLocker);
		std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
		if (it == m_rolesMap.end())
			return MLS_ROLE_NOT_EXIST;

		return it->second->IsGetStrongerReward(rewardid);
	}

}
BOOL RolesInfoMan::RoleSite::IsGetOnlineReward(UINT32 roleId, UINT16 rewardid)
{
	std::vector<SPropertyAttr> emailAttachmentsVec;
	{
		CXTAutoLock lock(m_rolesMapLocker);
		std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
		if (it == m_rolesMap.end())
			return MLS_ROLE_NOT_EXIST;

		return it->second->IsGetOnlineReward(rewardid);
	}

}

int RolesInfoMan::RoleSite::GetOnlineInfo(UINT32 roleId, SOnlineInfo* info, std::vector<UINT16>& vonline)
{
	std::vector<SPropertyAttr> emailAttachmentsVec;
	{
		CXTAutoLock lock(m_rolesMapLocker);
		std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
		if (it == m_rolesMap.end())
			return MLS_ROLE_NOT_EXIST;

		it->second->GetOnlineInfo(info, vonline);
	}
	return MLS_OK;
}
int RolesInfoMan::RoleSite::GetStrongerInfo(UINT32 roleId, std::vector<UINT16>& infov)
{
	std::vector<SPropertyAttr> emailAttachmentsVec;
	{
		CXTAutoLock lock(m_rolesMapLocker);
		std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
		if (it == m_rolesMap.end())
			return MLS_ROLE_NOT_EXIST;

		it->second->GetStrongerInfo(infov);
	}
	return MLS_OK;
}


int RolesInfoMan::RoleSite::InputStrongerReward(UINT32 roleId, UINT16 rewardid)
{
	std::vector<SPropertyAttr> emailAttachmentsVec;
	{
		CXTAutoLock lock(m_rolesMapLocker);
		std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
		if (it == m_rolesMap.end())
			return MLS_ROLE_NOT_EXIST;

		it->second->InputStrongerReward(rewardid);
	}
	return MLS_OK;
}

int RolesInfoMan::RoleSite::InputOnlineReward(UINT32 roleId, UINT16 rewardid)
{
	std::vector<SPropertyAttr> emailAttachmentsVec;
	{
		CXTAutoLock lock(m_rolesMapLocker);
		std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
		if (it == m_rolesMap.end())
			return MLS_ROLE_NOT_EXIST;

		it->second->InputOnlineReward(rewardid);
	}
	return MLS_OK;
}

int RolesInfoMan::RoleSite::SetOnlineInfo(UINT32 roleId, UINT32 id, UINT32 time, LTMSEL lasttime)
{
	std::vector<SPropertyAttr> emailAttachmentsVec;
	{
		CXTAutoLock lock(m_rolesMapLocker);
		std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
		if (it == m_rolesMap.end())
			return MLS_ROLE_NOT_EXIST;

		it->second->SetOnlineInfo(id, time, lasttime);
	}
	return MLS_OK;
}

//add by hxw 20151015
int RolesInfoMan::RoleSite::ReceiveRankReward(UINT32 userId, UINT32 roleId, BYTE type, UINT16 rewardid, \
	SGetRankRewardRes *pRoleSign, SReceiveRewardRefresh *pRefresh)
{
	std::vector<SPropertyAttr> emailAttachmentsVec;
	{
		CXTAutoLock lock(m_rolesMapLocker);
		std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
		if (it == m_rolesMap.end())
			return MLS_ROLE_NOT_EXIST;

		it->second->ReceiveRankReward(type, rewardid, pRoleSign, emailAttachmentsVec, pRefresh);
	}

	//是否需要发送邮件
	BYTE attachmentNum = (BYTE)emailAttachmentsVec.size();
	if (attachmentNum > 0)
	{
		EmailCleanMan::SSyncEmailSendContent email;
		email.type = ESGS_EMAIL_VIP_REWARD;
		email.receiverRoleId = roleId;
		email.receiverUserId = userId;
		email.attachmentsNum = attachmentNum;
		email.pAttachments = emailAttachmentsVec.data();
		email.pGBSenderNick = "系统";		//发送者昵称
		email.pGBCaption = "1111111111111";		//标题
		email.pGBBody = "1111111111111";		//正文
		GetEmailCleanMan()->AsyncSendEmail(email);
	}
	return MLS_OK;
}

//end 20151015

int RolesInfoMan::RoleSite::QueryRoleHelps(UINT32 roleId, SQueryFinishedHelpRes* pHelpRes)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	SRoleInfos* pRole = it->second;
	BYTE si = pRole->helps.size();
	if (si == 0)
	{
		GetPermanenDataMan()->QueryRoleFinishedHelps(roleId, pHelpRes);
		for (BYTE i = 0; i < pHelpRes->num; ++i)
			pRole->helps.push_back(pHelpRes->data[i]);
	}
	else
	{
		for (BYTE i = 0; i < si; ++i)
			pHelpRes->data[i] = pRole->helps[i];
		pHelpRes->num = si;
	}
	return MLS_OK;
}

int RolesInfoMan::RoleSite::FinishRoleHelp(UINT32 roleId, UINT16 helpId)
{
	BOOL isExist = false;
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	SRoleInfos* pRole = it->second;
	UINT16 si = (UINT16)pRole->helps.size();
	for (UINT16 i = 0; i < si; ++i)
	{
		if (pRole->helps[i] == helpId)
		{
			isExist = true;
			break;
		}
	}

	if (isExist)
		return MLS_ALREADY_FINISHED_HELP;

	pRole->helps.push_back(helpId);
	return MLS_OK;
}

int RolesInfoMan::RoleSite::QueryRoleMagics(UINT32 roleId, SQueryRoleMagicsRes* pMagicsRes)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	SRoleInfos* pRole = it->second;
	BYTE si = pRole->magics.size();
	if (si == 0)
	{
		GetPermanenDataMan()->QueryRoleMagics(roleId, pMagicsRes);
		for (BYTE i = 0; i < pMagicsRes->num; ++i)
			pRole->magics.push_back(pMagicsRes->data[i]);
	}
	else
	{
		for (BYTE i = 0; i < si; ++i)
			pMagicsRes->data[i] = pRole->magics[i];
		pMagicsRes->num = si;
	}
	return MLS_OK;
}

int RolesInfoMan::RoleSite::GainMagic(UINT32 roleId, UINT16 instId, UINT16 magicId)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	SRoleInfos* pRole = it->second;
	BYTE si = pRole->magics.size();
	for (BYTE i = 0; i < si; ++i)
	{
		if (pRole->magics[i] == magicId)
			return MLS_ERROR;
	}
	pRole->magics.push_back(magicId);
	return MLS_OK;
}

int RolesInfoMan::RoleSite::EquipPosStrengthen(UINT32 roleId, BYTE pos, SEquipPosStrengthenRes* pStrengthenRes)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	return it->second->EquipPosStrengthen(pos, pStrengthenRes);
}

int RolesInfoMan::RoleSite::EquipPosRiseStar(UINT32 roleId, BYTE pos, const SRiseStarAttachParam& param, SEquipPosRiseStarRes* pRiseStarRes)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	return it->second->EquipPosRiseStar(pos, param, pRiseStarRes);
}

int RolesInfoMan::RoleSite::QuerySignIn(UINT32 roleId, SQueryOnlineSignInRes* pSignInRes)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	std::vector<LTMSEL>& signInVec = it->second->signInVec;
	int si = (int)signInVec.size();
	if (si <= 0 || si >= 7)
	{
		pSignInRes->continueTimes = 0;
		pSignInRes->sameDayIsSign = 0;
		signInVec.clear();
	}
	else
	{
		int idx = si - 1;
		LTMSEL nowMSel = GetMsel();
		SYSTEMTIME nowTM = ToTime(nowMSel);
		SYSTEMTIME lastSignInTM = ToTime(signInVec[idx]);
		LTMSEL interval = nowMSel - signInVec[idx];
		if (interval >= 48 * 3600 * 1000 || interval < 0)	//2天或2天以上没来签到则从第一天开始
		{
			pSignInRes->continueTimes = 0;
			pSignInRes->sameDayIsSign = 0;
			signInVec.clear();
		}
		else if (nowTM.wYear == lastSignInTM.wYear && nowTM.wMonth == lastSignInTM.wMonth && nowTM.wDay == lastSignInTM.wDay)
		{
			pSignInRes->continueTimes = (BYTE)si;
			pSignInRes->sameDayIsSign = 1;
		}
		else
		{
			pSignInRes->continueTimes = (BYTE)si;
			pSignInRes->sameDayIsSign = 0;
		}
	}
	return MLS_OK;
}

int RolesInfoMan::RoleSite::SignIn(UINT32 userId, UINT32 roleId)
{
	LTMSEL nowMSel = GetMsel();
	UINT64 gold = 0;
	UINT32 rpcoin = 0;
	int nDays = 0;
	std::vector<SPropertyAttr> propVec;

	{
		CXTAutoLock lock(m_rolesMapLocker);
		std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
		if (it == m_rolesMap.end())
			return MLS_ROLE_NOT_EXIST;

		SRoleInfos* pRole = it->second;

		std::vector<LTMSEL>& signInVec = pRole->signInVec;
		int si = (int)signInVec.size();
		if (si <= 0 || si >= 7)
		{
			signInVec.clear();
		}
		else
		{
			int idx = si - 1;
			LTMSEL nowMSel = GetMsel();
			SYSTEMTIME nowTM = ToTime(nowMSel);
			SYSTEMTIME lastSignInTM = ToTime(signInVec[idx]);

			if (nowTM.wYear == lastSignInTM.wYear && nowTM.wMonth == lastSignInTM.wMonth && nowTM.wDay == lastSignInTM.wDay)
				return MLS_ALREADY_SIGN_IN;

			LTMSEL interval = nowMSel - signInVec[idx];
			if (interval >= 48 * 3600 * 1000 || interval < 0)	//2天或2天以上没来签到则从第一天开始
				signInVec.clear();
		}
		signInVec.push_back(nowMSel);

		nDays = (int)signInVec.size();
	}

	//发送签到奖励邮件
	SignInCfg::SSignInItem* pItem = NULL;
	int retCode = GetGameCfgFileMan()->GetSignInItem(nDays, pItem);
	if (retCode != MLS_OK)
		return retCode;

	int n = pItem->rewardVec.size();
	for (int i = 0; i < n; ++i)
	{
		if (pItem->rewardVec[i].type == ESGS_PROP_TOKEN)
			rpcoin = SGSU32Add(rpcoin, pItem->rewardVec[i].num);
		else if (pItem->rewardVec[i].type == ESGS_PROP_GOLD)
			gold = SGSU64Add(gold, pItem->rewardVec[i].num);
		else
		{
			SPropertyAttr prop;
			memset(&prop, 0, sizeof(SPropertyAttr));
			prop.propId = pItem->rewardVec[i].propId;
			prop.num = pItem->rewardVec[i].num;
			prop.type = pItem->rewardVec[i].type;
			propVec.push_back(prop);
		}
	}

	EmailCleanMan::SSyncEmailSendContent email;
	email.gold = gold;
	email.rpcoin = rpcoin;
	email.receiverUserId = userId;
	email.receiverRoleId = roleId;
	email.type = ESGS_EMAIL_SIGN_IN_REWARD;
	email.pGBSenderNick = "系统";
	email.pGBCaption = "签到奖励";
	email.pGBBody = "签到奖励";
	if (propVec.size() > 0)
	{
		email.attachmentsNum = propVec.size();
		email.pAttachments = propVec.data();
	}

	GetEmailCleanMan()->AsyncSendEmail(email);
	return MLS_OK;
}

int RolesInfoMan::RoleSite::InstSweep(UINT32 userId, UINT32 roleId, UINT16 instId, SInstSweepRes* pSweepRes, BOOL& isUpgrade)
{
	isUpgrade = false;
	std::vector<SPropertyAttr> emailAttachmentsVec;
	BYTE maxNum = (BYTE)((MAX_RES_USER_BYTES - member_offset(SInstSweepRes, remainVit, data)) / sizeof(SPropertyAttr));

	{
		CXTAutoLock lock(m_rolesMapLocker);
		std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
		if (it == m_rolesMap.end())
			return MLS_ROLE_NOT_EXIST;

		SRoleInfos* pRole = it->second;

		UINT32 costVit = 0;
		int retCode = GetGameCfgFileMan()->GetInstSpendVit(instId, costVit);
		if (retCode != MLS_OK)
			return retCode;

		if (pRole->vit < costVit)	//体力不足
			return MLS_VIT_NOT_ENOUGH;

		UINT16 num = pRole->NumberOfProperty(ESGS_MATERIAL_SWEEP_SCROLL, 1);
		if (num == 0)	//无扫荡卷轴
		{
			UINT32 unitPrice = 0;
			GetGameCfgFileMan()->GetMaterialFuncValue(ESGS_MATERIAL_SWEEP_SCROLL, unitPrice);
			if (pRole->cash < unitPrice)
				return MLS_CASH_NOT_ENOUGH;	//代币不足

			//扣除代币抵荡卷轴
			pRole->ConsumeToken(unitPrice, "副本扫荡");
			//pRole->cash = SGSU32Sub(pRole->cash, unitPrice);
		}
		else	//扣除扫荡卷轴
		{
			// 			SConsumeProperty consumeScroll[1];
			// 			BYTE retNum = 0;
			// 			pRole->DropBackpackPropEx(ESGS_MATERIAL_SWEEP_SCROLL, 1, consumeScroll, 1, retNum);
			// 			ASSERT(retNum == 1);
			// 			pSweepRes->data[0].id = consumeScroll[0].id;
			// 			pSweepRes->data[0].num = consumeScroll[0].num;
			// 			pSweepRes->data[0].propId = ESGS_MATERIAL_SWEEP_SCROLL;
			// 			pSweepRes->data[0].type = ESGS_PROP_MATERIAL;
			// 			pSweepRes->num = 1;
			pRole->DropBackpackPropEx(ESGS_MATERIAL_SWEEP_SCROLL, 1, pSweepRes->data, maxNum, pSweepRes->num);
		}

		//扣除体力
		pRole->vit = SGSU32Sub(pRole->vit, costVit);

		//得到经验
		UINT64 exp = GetGameCfgFileMan()->GetInstSweepExp(instId);
		pRole->AddRoleExp(exp);

		InstCfg::SInstDropProp drops;
		std::set<UINT16> addFragments;
		std::set<UINT16> addSoulFragments;
		GetGameCfgFileMan()->GenerateInstSweepDrops(instId, drops);
		int si = (int)drops.props.size();
		for (int i = 0; i < si; ++i)
		{
			if (drops.props[i].type == ESGS_PROP_GOLD)
				pRole->ProduceGold(drops.props[i].num, "副本扫荡");
			//SBigNumberAdd(pRole->gold, drops.props[i].num);
			else if (drops.props[i].type == ESGS_PROP_TOKEN)
				pRole->ProduceToken(drops.props[i].num, "副本扫荡");
			//pRole->cash = SGSU32Add(pRole->cash, drops.props[i].num);
			else if (drops.props[i].type == ESGS_PROP_FRAGMENT)
			{
				pRole->AddEquipFragment(drops.props[i].propId, drops.props[i].num);
				addFragments.insert(drops.props[i].propId);
			}
			else if (drops.props[i].type == ESGS_PROP_CHAIN_SOUL_FRAGMENT)
			{
				pRole->AddChainSoulFragment(drops.props[i].propId, drops.props[i].num);
				addSoulFragments.insert(drops.props[i].propId);
			}
			else if (drops.props[i].type == ESGS_PROP_EQUIP || drops.props[i].type == ESGS_PROP_GEM || drops.props[i].type == ESGS_PROP_MATERIAL)
			{
				BYTE maxStack = GetGameCfgFileMan()->GetPropertyMaxStack(drops.props[i].type, drops.props[i].propId);
				if (!pRole->CanPutIntoBackpack(drops.props[i].propId, drops.props[i].num, maxStack))
				{
					//背包放不下, 发邮件附件
					SPropertyAttr attachmentProp;
					memset(&attachmentProp, 0, sizeof(SPropertyAttr));
					int retCode = GetGameCfgFileMan()->GetPropertyAttr(drops.props[i].type, drops.props[i].propId, attachmentProp, maxStack);
					if (retCode == MLS_OK)
					{
						attachmentProp.num = (BYTE)drops.props[i].num;
						emailAttachmentsVec.push_back(attachmentProp);
					}
				}
				else
				{
					BYTE retNum = 0;
					pRole->PutBackpackProperty(drops.props[i].type, drops.props[i].propId, drops.props[i].num, maxStack,
						pSweepRes->data + pSweepRes->num, SGSU8Sub(maxNum, pSweepRes->num), retNum);

					pSweepRes->num = SGSU8Add(pSweepRes->num, retNum);
				}
			}
			else
				ASSERT(false);
		}

		/* zpy 2015.12.17新增，客戶端要求將碎片信息放入背包 */
		for (size_t i = 0; i < pRole->fragments.size(); ++i)
		{
			std::set<UINT16>::iterator itr = addFragments.find(pRole->fragments[i].fragmentId);
			if (itr != addFragments.end())
			{
				pSweepRes->data[pSweepRes->num].id = 0;
				pSweepRes->data[pSweepRes->num].propId = pRole->fragments[i].fragmentId;
				pSweepRes->data[pSweepRes->num].type = ESGS_PROP_FRAGMENT;
				pSweepRes->data[pSweepRes->num].suitId = 0;
				pSweepRes->data[pSweepRes->num].level = 0;
				pSweepRes->data[pSweepRes->num].grade = 0;
				pSweepRes->data[pSweepRes->num].star = 0;
				pSweepRes->data[pSweepRes->num].num = pRole->fragments[i].num;
				pSweepRes->data[pSweepRes->num].pos = 0;
				pSweepRes->num = SGSU8Add(pSweepRes->num, 1);
			}
		}
		for (size_t i = 0; i < pRole->chainSoulFragments.size(); ++i)
		{
			std::set<UINT16>::iterator itr = addSoulFragments.find(pRole->chainSoulFragments[i].fragmentId);
			if (itr != addSoulFragments.end())
			{
				pSweepRes->data[pSweepRes->num].id = 0;
				pSweepRes->data[pSweepRes->num].propId = pRole->chainSoulFragments[i].fragmentId;
				pSweepRes->data[pSweepRes->num].type = ESGS_PROP_CHAIN_SOUL_FRAGMENT;
				pSweepRes->data[pSweepRes->num].suitId = 0;
				pSweepRes->data[pSweepRes->num].level = 0;
				pSweepRes->data[pSweepRes->num].grade = 0;
				pSweepRes->data[pSweepRes->num].star = 0;
				pSweepRes->data[pSweepRes->num].num = pRole->chainSoulFragments[i].num;
				pSweepRes->data[pSweepRes->num].pos = 0;
				pSweepRes->num = SGSU8Add(pSweepRes->num, 1);
			}
		}

		pSweepRes->exp = pRole->exp;
		pSweepRes->remainVit = pRole->vit;
		pSweepRes->gold = pRole->gold;
		pSweepRes->rpcoin = pRole->cash;

		//判断是否升级
		BYTE newLv = 0;
		GetGameCfgFileMan()->JudgeRoleUpgrade(pRole->occuId, pRole->level, BigNumberToU64(pRole->exp), newLv);
		if (newLv <= pRole->level)
			return MLS_OK;

		//开放新技能
		GetGameCfgFileMan()->GetRoleAllInitSkills(pRole->occuId, pRole->level, newLv, pRole->studySkills);

		//新等级角色hp/mp
		SRoleAttrPoint attr;
		if (MLS_OK == GetGameCfgFileMan()->GetRoleCfgBasicAttr(pRole->occuId, newLv, attr))
		{
			pRole->hp = attr.u32HP;
			pRole->mp = attr.u32MP;
			pRole->attack = attr.attack;
			pRole->def = attr.def;
		}
		//新等级
		pRole->level = newLv;
		isUpgrade = true;

		//重新计算战力
		pRole->CalcRoleCombatPower();
	}


	//发送邮件
	BYTE attachmentNum = (BYTE)emailAttachmentsVec.size();
	if (attachmentNum > 0)
	{
		std::string instName;
		GetGameCfgFileMan()->GetInstName(instId, instName);
		instName += ", 背包满, 物品掉落";
		EmailCleanMan::SSyncEmailSendContent email;
		email.type = ESGS_EMAIL_INST_DROP;
		email.receiverRoleId = roleId;
		email.receiverUserId = userId;
		email.attachmentsNum = attachmentNum;
		email.pAttachments = emailAttachmentsVec.data();
		email.pGBSenderNick = "系统";		//发送者昵称
		email.pGBCaption = "副本扫荡掉落";		//标题
		email.pGBBody = instName.c_str();		//正文
		GetEmailCleanMan()->AsyncSendEmail(email);
	}

	return MLS_OK;
}

int RolesInfoMan::RoleSite::UnlockContent(UINT32 roleId, BYTE type, UINT16 unlockId)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	SRoleInfos* pRole = it->second;

	UnlockCfg::SUnlockItem* pUnloclItem = NULL;
	int retCode = GetGameCfgFileMan()->GetUnlockItem(type, unlockId, pUnloclItem);
	if (retCode != MLS_OK)
		return retCode;

	if (BigNumberToU64(pRole->gold) < pUnloclItem->gold)
		return MLS_GOLD_NOT_ENOUGH;

	if (pRole->cash < pUnloclItem->rpcoin)
		return MLS_CASH_NOT_ENOUGH;

	if (pUnloclItem->gold > 0)
		pRole->ConsumeGold(pUnloclItem->gold, "解锁消耗");
	//SBigNumberSub(pRole->gold, pUnloclItem->gold);	//扣除金币

	if (pUnloclItem->rpcoin > 0)
		pRole->ConsumeToken(pUnloclItem->rpcoin, "解锁消耗");
	//pRole->cash = SGSU32Sub(pRole->cash, pUnloclItem->rpcoin);	//扣除代币

	return MLS_OK;
}

int RolesInfoMan::RoleSite::ExtractRouletteReward(UINT32 userId, UINT32 roleId, const SRoulettePropItem& extractReward, SExtractRouletteRewardRes* pRewardRes, BOOL& isUpgrade)
{
	isUpgrade = false;
	std::vector<SPropertyAttr> emailAttachmentsVec;
	BYTE maxNum = (BYTE)((MAX_RES_USER_BYTES - member_offset(SExtractRouletteRewardRes, id, data)) / sizeof(SPropertyAttr));
	BYTE everydayGiveTimes = GetGameCfgFileMan()->GetEverydayGiveTimes();

	{
		CXTAutoLock lock(m_rolesMapLocker);
		std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
		if (it == m_rolesMap.end())
			return MLS_ROLE_NOT_EXIST;

		SRoleInfos* pRole = it->second;

		//超过每日赠送次数
		if (pRewardRes->getNum > everydayGiveTimes)
		{
			UINT32 consumeLuckyCoinNum = 0;
			GetGameCfgFileMan()->GetMaterialFuncValue(ESGS_MATERIAL_LUCKY_COIN, consumeLuckyCoinNum);

			if (!pRole->IsExistProperty(ESGS_MATERIAL_LUCKY_COIN, consumeLuckyCoinNum))
				return MLS_MATERIAL_NOT_ENOUGH;

			//扣除幸运币
			pRole->DropBackpackPropEx(ESGS_MATERIAL_LUCKY_COIN, consumeLuckyCoinNum, pRewardRes->data, maxNum, pRewardRes->num);
		}

		//发放奖品
		switch (extractReward.type)
		{
		case ESGS_PROP_EQUIP:			//装备
		case ESGS_PROP_GEM:				//宝石
		case ESGS_PROP_MATERIAL:		//材料
		{
											BYTE maxStack = GetGameCfgFileMan()->GetPropertyMaxStack(extractReward.type, extractReward.propId);
											if (!pRole->CanPutIntoBackpack(extractReward.propId, extractReward.num, maxStack))
											{
												//背包放不下, 发邮件附件
												SPropertyAttr attachmentProp;
												memset(&attachmentProp, 0, sizeof(SPropertyAttr));
												int retCode = GetGameCfgFileMan()->GetPropertyAttr(extractReward.type, extractReward.propId, attachmentProp, maxStack);
												if (retCode == MLS_OK)
												{
													attachmentProp.num = (BYTE)extractReward.num;
													emailAttachmentsVec.push_back(attachmentProp);
												}
											}
											else
											{
												BYTE retNum = 0;
												pRole->PutBackpackProperty(extractReward.type, extractReward.propId, extractReward.num,
													maxStack, pRewardRes->data + pRewardRes->num,
													SGSU8Sub(maxNum, pRewardRes->num), retNum);

												pRewardRes->num = SGSU8Add(pRewardRes->num, retNum);
											}
		}
			break;
		case ESGS_PROP_GOLD:		//金币
			pRole->ProduceGold(extractReward.num, "轮盘抽奖");
			//SBigNumberAdd(pRole->gold, itemRef.num);
			break;
		case ESGS_PROP_TOKEN:		//代币
			pRole->ProduceToken(extractReward.num, "轮盘抽奖");
			//pRole->cash = SGSU32Add(pRole->cash, itemRef.num);
			break;
		case ESGS_PROP_VIT:			//体力
			pRole->vit = SGSU32Add(pRole->vit, extractReward.num);
			break;
		case ESGS_PROP_EXP:			//经验
			pRole->AddRoleExp(extractReward.num);
			break;
		case ESGS_PROP_FRAGMENT:
			pRole->AddEquipFragment(extractReward.propId, extractReward.num);
			break;
		case ESGS_PROP_CHAIN_SOUL_FRAGMENT:
			pRole->AddChainSoulFragment(extractReward.propId, extractReward.num);
			break;
		case ESGS_PROP_MAGIC:		//魔导器
			pRole->magics.push_back(extractReward.propId);
			break;
		case ESGS_PROP_DRESS:
			pRole->dress.push_back(extractReward.propId);
			break;
		default:
			break;
		}

		// zpy 2016.1.6修改 客户端抽中幸运币时会出现bug，客户端汪生要求修改
		if (extractReward.type == ESGS_PROP_MATERIAL && extractReward.propId == ESGS_MATERIAL_LUCKY_COIN)
		{
			if (pRewardRes->num >= 2)
			{
				// 一个返回中 同一个UID只可以返回一次，不同UID就算其值为0 也要其数量num置为0返回 wbear 1/6
				if (pRewardRes->data[0].id == pRewardRes->data[1].id)
				{
					std::swap(pRewardRes->data[0], pRewardRes->data[pRewardRes->num - 1]);
					--pRewardRes->num;
				}
			}
		}

		pRewardRes->exp = pRole->exp;
		pRewardRes->gold = pRole->gold;
		pRewardRes->rpcoin = pRole->cash;
		pRewardRes->vit = pRole->vit;

		if (extractReward.type == ESGS_PROP_EXP)	//获取经验后是否升级
		{
			BYTE newLv = 0;
			GetGameCfgFileMan()->JudgeRoleUpgrade(pRole->occuId, pRole->level, BigNumberToU64(pRole->exp), newLv);
			if (newLv > pRole->level)
			{
				isUpgrade = true;
				GetGameCfgFileMan()->GetRoleAllInitSkills(pRole->occuId, pRole->level, newLv, pRole->studySkills);
				pRole->level = newLv;
			}
		}
	}

	//发送邮件
	BYTE attachmentNum = (BYTE)emailAttachmentsVec.size();
	if (attachmentNum > 0)
	{
		EmailCleanMan::SSyncEmailSendContent email;
		email.type = ESGS_EMAIL_EXTRACT_REWARD;
		email.receiverRoleId = roleId;
		email.receiverUserId = userId;
		email.attachmentsNum = attachmentNum;
		email.pAttachments = emailAttachmentsVec.data();
		email.pGBSenderNick = "系统";		//发送者昵称
		email.pGBCaption = "轮盘抽奖物品";		//标题
		email.pGBBody = "轮盘抽奖物品";		//正文
		GetEmailCleanMan()->AsyncSendEmail(email);
	}

	return MLS_OK;
}

int RolesInfoMan::RoleSite::FetchRouletteBigReward(UINT32 userId, UINT32 roleId, const SRoulettePropItem& bigReward, SFetchRouletteBigRewardRes* pRewardRes, BOOL& isUpgrade)
{
	std::vector<SPropertyAttr> emailAttachmentsVec;
	BYTE maxNum = (BYTE)((MAX_RES_USER_BYTES - member_offset(SFetchRouletteBigRewardRes, rpcoin, data)) / sizeof(SPropertyAttr));

	{
		CXTAutoLock lock(m_rolesMapLocker);
		std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
		if (it == m_rolesMap.end())
			return MLS_ROLE_NOT_EXIST;

		SRoleInfos* pRole = it->second;

		//发放大奖
		switch (bigReward.type)
		{
		case ESGS_PROP_EQUIP:			//装备
		case ESGS_PROP_GEM:				//宝石
		case ESGS_PROP_MATERIAL:		//材料
		{
											BYTE maxStack = GetGameCfgFileMan()->GetPropertyMaxStack(bigReward.type, bigReward.propId);
											if (!pRole->CanPutIntoBackpack(bigReward.propId, bigReward.num, maxStack))
											{
												//背包放不下, 发邮件附件
												SPropertyAttr attachmentProp;
												memset(&attachmentProp, 0, sizeof(SPropertyAttr));
												int retCode = GetGameCfgFileMan()->GetPropertyAttr(bigReward.type, bigReward.propId, attachmentProp, maxStack);
												if (retCode == MLS_OK)
												{
													attachmentProp.num = (BYTE)bigReward.num;
													emailAttachmentsVec.push_back(attachmentProp);
												}
											}
											else
											{
												BYTE retNum = 0;
												pRole->PutBackpackProperty(bigReward.type, bigReward.propId, bigReward.num, maxStack,
													pRewardRes->data + pRewardRes->num,
													SGSU8Sub(maxNum, pRewardRes->num), retNum);

												pRewardRes->num = SGSU8Add(pRewardRes->num, retNum);
											}
		}
			break;
		case ESGS_PROP_GOLD:		//金币
			pRole->ProduceGold(bigReward.num, "轮盘抽奖");
			//SBigNumberAdd(pRole->gold, itemRef.num);
			break;
		case ESGS_PROP_TOKEN:		//代币
			pRole->ProduceToken(bigReward.num, "轮盘抽奖");
			//pRole->cash = SGSU32Add(pRole->cash, itemRef.num);
			break;
		case ESGS_PROP_VIT:			//体力
			pRole->vit = SGSU32Add(pRole->vit, bigReward.num);
			break;
		case ESGS_PROP_EXP:			//经验
			pRole->AddRoleExp(bigReward.num);
			break;
		case ESGS_PROP_FRAGMENT:
			pRole->AddEquipFragment(bigReward.propId, bigReward.num);
			break;
		case ESGS_PROP_CHAIN_SOUL_FRAGMENT:
			pRole->AddChainSoulFragment(bigReward.propId, bigReward.num);
			break;
		case ESGS_PROP_MAGIC:		//魔导器
			pRole->magics.push_back(bigReward.propId);
			break;
		case ESGS_PROP_DRESS:
			pRole->dress.push_back(bigReward.propId);
			break;
		default:
			break;
		}

		pRewardRes->exp = pRole->exp;
		pRewardRes->gold = pRole->gold;
		pRewardRes->rpcoin = pRole->cash;
		pRewardRes->vit = pRole->vit;

		if (bigReward.type == ESGS_PROP_EXP)	//获取经验后是否升级
		{
			BYTE newLv = 0;
			GetGameCfgFileMan()->JudgeRoleUpgrade(pRole->occuId, pRole->level, BigNumberToU64(pRole->exp), newLv);
			if (newLv > pRole->level)
			{
				isUpgrade = true;
				GetGameCfgFileMan()->GetRoleAllInitSkills(pRole->occuId, pRole->level, newLv, pRole->studySkills);
				pRole->level = newLv;
			}
		}
	}

	//发送邮件
	BYTE attachmentNum = (BYTE)emailAttachmentsVec.size();
	if (attachmentNum > 0)
	{
		EmailCleanMan::SSyncEmailSendContent email;
		email.type = ESGS_EMAIL_EXTRACT_REWARD;
		email.receiverRoleId = roleId;
		email.receiverUserId = userId;
		email.attachmentsNum = attachmentNum;
		email.pAttachments = emailAttachmentsVec.data();
		email.pGBSenderNick = "系统";		//发送者昵称
		email.pGBCaption = "轮盘抽奖物品";		//标题
		email.pGBBody = "轮盘抽奖物品";		//正文
		GetEmailCleanMan()->AsyncSendEmail(email);
	}

	return MLS_OK;
}

int RolesInfoMan::RoleSite::UpdateRouletteBigReward(UINT32 roleId, UINT32 spendToken, UINT32& curToken)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	SRoleInfos* pRole = it->second;

	if (pRole->cash < spendToken)
		return MLS_CASH_NOT_ENOUGH;

	pRole->ConsumeToken(spendToken, "更新轮盘大奖");
	curToken = pRole->cash;
	return MLS_OK;
}

int RolesInfoMan::RoleSite::OnQueryRolesInfoByUserId(UINT32 userId)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select A.id, A.nick, A.sex, A.gold, A.level, A.occu, A.exp, A.cash, "
		"A.cashcount, A.actortype, A.hp, A.mp, A.attack, A.def, A.vit, "
		"A.cp, A.ap, A.packsize, A.waresize, A.guild, A.duplicate, A.coords, A.vist, A.lvtts, "
		"D.buffer, D.backpack, D.taskaccepted, D.ability, D.wearequipment, D.dress, A.faction, A.dressid, D.fragment, D.equipsock, D.lh_fragment, D.lh_pos "
		"from actor as A left join data as D on (A.id=D.actorid) where A.accountid=%u", userId);
	DBS_RESULT result = NULL;
	int ret = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (ret != DBS_OK)
		return ret;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols < 34)
	{
		DBS_FreeResult(result);
		return MLS_OK/*MLS_ROLE_NOT_EXIST*/;
	}

	DBS_ROW row = NULL;
	DBS_ROWLENGTH rowValLen = NULL;
	LTMSEL nowMSel = GetMsel();
	while ((row = DBS_FetchRow(result, &rowValLen)) != NULL)
	{
		AutoRoleInfosObj pRoleInfo;
		ASSERT(pRoleInfo.IsValid());
		UINT32 roleId = _atoi64(row[0]);
		pRoleInfo->roleId = roleId;				//角色id
		strcpy(pRoleInfo->nick, row[1]);		//昵称
		pRoleInfo->sex = atoi(row[2]);			//性别
		U64ToBigNumber(_atoi64(row[3]), pRoleInfo->gold);	//游戏币
		pRoleInfo->level = atoi(row[4]);	//等级
		pRoleInfo->titleId = atoi(row[5]);		//职业称号
		U64ToBigNumber(_atoi64(row[6]), pRoleInfo->exp);		//经验

		//mid_cash add by hxw 20151117 如果代币是在角色身上
		if (GetConfigFileMan()->GetCashSaveType() == CASH_SAVE_TYPE_TO_ACTOR)
		{
			pRoleInfo->cash = _atoi64(row[7]);		//现金
			pRoleInfo->cashcount = _atoi64(row[8]);	//充值总额
		}
		else //if(GetConfigFileMan()->GetCashSaveType()==CASH_SAVE_TYPE_TO_CASHDB)
		{
			GetCashDBMan()->GetCash(userId, pRoleInfo->cash, pRoleInfo->cashcount);		//现金
		}
		//end 1117


		pRoleInfo->occuId = atoi(row[9]);			//角色类型

		pRoleInfo->hp = _atoi64(row[10]);		//生命
		pRoleInfo->mp = _atoi64(row[11]);		//魔力
		pRoleInfo->attack = _atoi64(row[12]);	//攻击
		pRoleInfo->def = _atoi64(row[13]);		//防御

		pRoleInfo->vit = _atoi64(row[14]);		//体力
		pRoleInfo->cp = _atoi64(row[15]);		//战力
		pRoleInfo->ap = atoi(row[16]);				//剩余属性点数
		pRoleInfo->packSize = atoi(row[17]);		//背包大小
		pRoleInfo->wareSize = atoi(row[18]);		//仓库大小
		pRoleInfo->guildId = _atoi64(row[19]);		//所属公会id
		pRoleInfo->pos.owncopy = atoi(row[20]);		//所在副本
		if (rowValLen[21] > 0)
			memcpy(&pRoleInfo->pos, row[21], sizeof(pRoleInfo->pos));	//位置
		pRoleInfo->vist = _atoi64(row[22]);		//访问次数
		strcpy(pRoleInfo->lastEnterTime, row[23]);	//最后一次进入游戏时间

		//初始化buffer
		if (rowValLen[24] > 0)
			pRoleInfo->InitBuffer((const BYTE*)row[24], rowValLen[24]);

		//初始化背包
		if (rowValLen[25] > 0)
			pRoleInfo->InitBackpack((const BYTE*)row[25], rowValLen[25]);

		//初始化已接收任务
		if (rowValLen[26] > 0)
			pRoleInfo->InitAcceptedTasks((const BYTE*)row[26], rowValLen[26], nowMSel);

		//初始化技能
		if (rowValLen[27] > 0)
			pRoleInfo->InitSkills((const BYTE*)row[27], rowValLen[27]);

		//初始化已穿戴装备
		if (rowValLen[28] > 0)
			pRoleInfo->InitWearEquipment((const BYTE*)row[28], rowValLen[28]);

		//初始化拥有时装
		if (rowValLen[29] > 0)
			pRoleInfo->InitHasDress((const BYTE*)row[29], rowValLen[29]);

		//所在阵营id
		pRoleInfo->factionId = (BYTE)atoi(row[30]);

		//当前穿戴时装
		pRoleInfo->dressId = (UINT16)atoi(row[31]);

		//装备碎片
		if (rowValLen[32] > 0)
			pRoleInfo->InitHasEquipFragment((const BYTE*)row[32], rowValLen[32]);

		//装备位
		if (rowValLen[33] > 0)
			pRoleInfo->InitEquipPos((const BYTE*)row[33], rowValLen[33]);

		//zpy 炼魂碎片
		if (rowValLen[34] > 0)
			pRoleInfo->InitHasChainSoulFragment((const BYTE*)row[34], rowValLen[34]);

		//zpy 炼魂部件
		if (rowValLen[35] > 0)
			pRoleInfo->InitChainSoulPos((const BYTE*)row[35], rowValLen[35]);

		//zpy 天梯信息
		UINT16 win_in_row = 0;
		UINT16 refresh_count = 0;
		UINT16 eliminate_count = 0;
		UINT16 challenge_count = 0;
		LTMSEL last_finish_time = 0;
		UINT32 matching_rank[LADDER_MATCHTING_MAX_NUM];
		memset(matching_rank, 0, sizeof(matching_rank));
		GetLadderRankMan()->QueryRoleLadderInfoFromDB(roleId, &win_in_row, &refresh_count, &eliminate_count, &challenge_count, &last_finish_time, matching_rank);
		pRoleInfo->InitLadderInfo(win_in_row, refresh_count, eliminate_count, challenge_count, last_finish_time, matching_rank);

		//查询金币兑换信息
		GetGoldExchangeMan()->QueryRoleGoldExchange(roleId, pRoleInfo->exchangedTimes);

		/* zpy 体力兑换次数查询 */
		GetVitExchangeMan()->QueryRoleVitExchange(roleId, pRoleInfo->vitExchangedTimes);

		//查询签到信息
		GetOnlineSingnInMan()->QueryRoleSignIn(roleId, pRoleInfo->signInVec);

		/* zpy 成就系统新增 */
		if (!GetAchievementMan()->CreateCache(roleId, ret))
		{
			DBS_FreeResult(result);
			return ret;
		}

		//保存角色信息
		m_rolesMap.insert(std::make_pair(roleId, pRoleInfo.Drop()));
	}
	DBS_FreeResult(result);
	return MLS_OK;
}

RolesInfoMan::RoleSite* RolesInfoMan::NewRoleSite()
{
	RoleSite* site = NULL;
#if USE_MEM_POOL
	site = m_roleSitePool.NewObj();
#else
	site = new RoleSite();
#endif
	site->SetParent(this);
	return site;
}

void RolesInfoMan::DeleteRoleSite(RoleSite* site)
{
	if (site == NULL)
		return;

#if USE_MEM_POOL
	m_roleSitePool.DeleteObj(site);
#else
	delete site;
#endif
}

RolesInfoMan::RoleSite* RolesInfoMan::GetRoleSite(UINT32 userId)
{
	RoleSite* pSite = NULL;
	CXTAutoLock lock(m_userRolesMapLocker);
	std::map<UINT32/*userId*/, RoleSite*>::iterator it = m_userRolesMap.find(userId);
	if (it != m_userRolesMap.end())
	{
		pSite = it->second;
		pSite->AddRef();
	}

	return pSite;
}

RolesInfoMan::RoleSite* RolesInfoMan::FetchRoleSite(UINT32 userId)
{
	RoleSite* pSite = NULL;
	CXTAutoLock lock(m_userRolesMapLocker);
	std::map<UINT32/*userId*/, RoleSite*>::iterator it = m_userRolesMap.find(userId);
	if (it != m_userRolesMap.end())
	{
		pSite = it->second;
		pSite->AddRef();
	}
	else
	{
		pSite = NewRoleSite();
		ASSERT(pSite != NULL);
		pSite->AddRef();
		m_userRolesMap.insert(std::make_pair(userId, pSite));
	}
	return pSite;
}

int RolesInfoMan::GetRoleBackpack(UINT32 userId, UINT32 roleId, std::vector<SPropertyAttr>& vec)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GetRoleBackpack(roleId, vec);
}

int RolesInfoMan::GetRoleAcceptTask(UINT32 userId, UINT32 roleId, std::vector<SAcceptTaskInfo>& vec)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GetRoleAcceptTask(roleId, vec);
}

int RolesInfoMan::GetRoleStorageSpace(UINT32 userId, UINT32 roleId, BYTE& packSize, BYTE& wareSize)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GetRoleStorageSpace(roleId, packSize, wareSize);
}

int RolesInfoMan::GetRoleMoney(UINT32 userId, UINT32 roleId, UINT64& gold, UINT32& cash, UINT32& cashcount)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GetRoleMoney(roleId, gold, cash, cashcount);
}

int RolesInfoMan::GetRoleVistInfo(UINT32 userId, UINT32 roleId, UINT32& vistCount, UINT32& vit, char* lastTime)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GetRoleVistInfo(roleId, vistCount, vit, lastTime);
}

int RolesInfoMan::GetRoleSkills(UINT32 userId, UINT32 roleId, std::vector<SSkillMoveInfo>& skillsVec)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GetRoleSkills(roleId, skillsVec);
}

int RolesInfoMan::GetRoleNick(UINT32 userId, UINT32 roleId, char* pszNick)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GetRoleNick(roleId, pszNick);
}

int RolesInfoMan::GetRoleInfo(UINT32 userId, UINT32 roleId, SRoleInfos* pRoleInfo)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GetRoleInfo(roleId, pRoleInfo);
}

int RolesInfoMan::GetRoleCurrentVit(UINT32 userId, UINT32 roleId, UINT32& vit)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GetRoleCurrentVit(roleId, vit);
}

int RolesInfoMan::GetRoleFaction(UINT32 userId, UINT32 roleId, BYTE& factionId)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GetRoleFaction(roleId, factionId);
}

int RolesInfoMan::GetRoleWearBackpack(UINT32 userId, UINT32 roleId, std::vector<SPropertyAttr>& wear, std::vector<SPropertyAttr>& backpack)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GetRoleWearBackpack(roleId, wear, backpack);
}

int RolesInfoMan::GetRoleWearEquip(UINT32 userId, UINT32 roleId, std::vector<SPropertyAttr>& wear)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GetRoleWearEquip(roleId, wear);
}

int RolesInfoMan::GetUserEnteredRoles(UINT32 userId, std::vector<UINT32>& rolesVec)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GetUserEnteredRoles(rolesVec);
}

int RolesInfoMan::RecoveryOnlineRoleVit(UINT32 userId, UINT32 roleId, UINT32& vit, BOOL& needNotify)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->RecoveryOnlineRoleVit(roleId, vit, needNotify);
}

int RolesInfoMan::GetRoleTitleId(UINT32 userId, UINT32 roleId, UINT16& titleId)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GetRoleTitleId(roleId, titleId);
}

int RolesInfoMan::GetRoleWearDressId(UINT32 userId, UINT32 roleId, UINT16& dressId, std::vector<UINT16>* pDressVec)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GetRoleWearDressId(roleId, dressId, pDressVec);
}

int RolesInfoMan::CalcRoleCompatPower(UINT32 userId, UINT32 roleId, UINT32* pNewPower)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->CalcRoleCompatPower(roleId, pNewPower);
}

int RolesInfoMan::GetRoleCompatPower(UINT32 userId, UINT32 roleId, UINT32& cp)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GetRoleCompatPower(roleId, cp);
}

int RolesInfoMan::GetRoleEquipFragment(UINT32 userId, UINT32 roleId, std::vector<SEquipFragmentAttr>& fragmentVec)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GetRoleEquipFragment(roleId, fragmentVec);
}

//zpy 获取炼魂碎片
int RolesInfoMan::GetRoleChainSoulFragment(UINT32 userId, UINT32 roleId, std::vector<SChainSoulFragmentAttr>& fragmentVec)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GetRoleChainSoulFragment(roleId, fragmentVec);
}

//zpy 获取炼魂部件
int RolesInfoMan::GetRoleChainSoulPos(UINT32 userId, UINT32 roleId, std::vector<SChainSoulPosAttr>& posAttr)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GetRoleChainSoulPos(roleId, posAttr);
}

// 天梯匹配
int RolesInfoMan::LadderMatching(UINT32 userId, UINT32 roleId, UINT32 matching_rank[LADDER_MATCHTING_MAX_NUM], UINT16 *refresh_count, UINT32 *rpcoin)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->LadderMatching(roleId, matching_rank, refresh_count, rpcoin);
}

//zpy 刷新天梯排行范围
int RolesInfoMan::RefreshLadderRankScope(UINT32 userId, UINT32 roleId, UINT32 matching_rank[LADDER_MATCHTING_MAX_NUM])
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->RefreshLadderRankScope(roleId, matching_rank);
}

//zpy 执行天梯挑战
int RolesInfoMan::DoLadderChallenge(UINT32 userId, UINT32 roleId, UINT16 *ret_challenge_count)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->DoLadderChallenge(roleId, ret_challenge_count);
}

//zpy 消除冷却时间
int RolesInfoMan::EliminateLadderCoolingTime(UINT32 userId, UINT32 roleId, SEliminateLadderCDTimeRes *pEliminate)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->EliminateLadderCoolingTime(roleId, pEliminate);
}

//zpy 完成天梯挑战
int RolesInfoMan::FinishLadderChallenge(UINT32 userId, UINT32 roleId, BYTE finishCode/*, SFinishLadderChallengeRes *pFinishLadder*/)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->FinishLadderChallenge(roleId, finishCode/*, pFinishLadder*/);
}

//zpy 重置被挑战者天梯排行范围
int RolesInfoMan::ResetChallengerLadderRankScope(UINT32 userId, UINT32 roleId)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->ResetChallengerLadderRankScope(roleId);
}

//zpy 添加机器人
int RolesInfoMan::AddRobot(UINT32 userId, UINT32 roleId, UINT32 robotId)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->AddRobot(roleId, robotId);
}

//zpy 是否存在机器人
int RolesInfoMan::HasRobot(UINT32 userId, UINT32 roleId, UINT32 robotId, bool *ret)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->HasRobot(roleId, robotId, ret);
}

//zpy 清理机器人
int RolesInfoMan::ClearRobot(UINT32 userId, UINT32 roleId)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->ClearRobot(roleId);
}

/* zpy 购买竞技模式进入次数 */
int RolesInfoMan::BuyCompModeEnterNum(UINT32 userId, UINT32 roleId, int purchased, SBuyCompEnterNumRes *pEnterNum)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->BuyCompModeEnterNum(roleId, purchased, pEnterNum);
}

int RolesInfoMan::BuyAllCompModeEnterNum(UINT32 userId, UINT32 roleId, int purchased, int upper_num, SBuyCompEnterNumRes *pEnterNum)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->BuyAllCompModeEnterNum(roleId, purchased, upper_num, pEnterNum);
}

/* zpy 购买1vs1模式进入次数 */
int RolesInfoMan::BuyoneVsOneEnterNum(UINT32 userId, UINT32 roleId, int purchased, int num, SBuy1V1EnterNumRes *pBuyEnter)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->BuyoneVsOneEnterNum(roleId, purchased, num, pBuyEnter);
}

int RolesInfoMan::GetRolePosition(UINT32 userId, UINT32 roleId, SGSPosition* pos)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GetRolePosition(roleId, pos);
}

int RolesInfoMan::GetRoleExp(UINT32 userId, UINT32 roleId, SBigNumber& exp)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GetRoleExp(roleId, exp);
}

int RolesInfoMan::GetRoleUpgradeNotify(UINT32 userId, UINT32 roleId, SRoleUpgradeNotify* pUpgradeNotify)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GetRoleUpgradeNotify(roleId, pUpgradeNotify);
}

int RolesInfoMan::GetRoleHelps(UINT32 userId, UINT32 roleId, std::vector<UINT16>& helpsVec)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GetRoleHelps(roleId, helpsVec);
}

int RolesInfoMan::GetRoleMagics(UINT32 userId, UINT32 roleId, std::vector<UINT16>& magicsVec)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GetRoleMagics(roleId, magicsVec);
}

int RolesInfoMan::GetRoleEquipPosAttr(UINT32 userId, UINT32 roleId, std::vector<SEquipPosAttr>& equipPosVec)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GetRoleEquipPosAttr(roleId, equipPosVec);
}

int RolesInfoMan::GetRoleSignInInfo(UINT32 userId, UINT32 roleId, std::vector<LTMSEL>& signVec)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->GetRoleSignInInfo(roleId, signVec);
}

int RolesInfoMan::CheckPlayerRechargeOrderStatus(UINT32 userId, UINT32 roleId, BOOL& hasNotReceiveOrder)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->CheckPlayerRechargeOrderStatus(userId, roleId, hasNotReceiveOrder);
}

/* zpy 领取每日签到奖励 */
int RolesInfoMan::ReceiveDailySignReward(UINT32 userId, UINT32 roleId, BYTE days, bool continuous, SRoleDailySignRes *pRoleSign, SReceiveRewardRefresh *pRefresh)
{
	RoleSite* pSite = GetRoleSite(userId);
	if (pSite == NULL)
		return MLS_ACCOUNT_NOT_EXIST;

	ASSERT(pSite != NULL);
	AutoRefCountObjTmpl<RoleSite> pAutoObj(pSite);
	return pAutoObj->ReceiveDailySignReward(userId, roleId, days, continuous, pRoleSign, pRefresh);
}

//CREAT_GENERAL_ROLES_MAN_FUN(CleanOnlineData)

//add by hxw 20151124
void RolesInfoMan::CleanOnlineData()
{
	CXTAutoLock lock(m_userRolesMapLocker);
	std::map<UINT32, RoleSite*>::iterator it = m_userRolesMap.begin();
	for (; it != m_userRolesMap.end(); ++it)
		it->second->CleanOnlineData();
	GetRankRewardMan()->RefreshOnlineOnZero();
}

void RolesInfoMan::CleanEverydayData()
{
	CXTAutoLock lock(m_userRolesMapLocker);
	std::map<UINT32/*userId*/, RoleSite*>::iterator it = m_userRolesMap.begin();
	for (; it != m_userRolesMap.end(); ++it)
		it->second->CleanEverydayData();
}

int RolesInfoMan::RoleSite::GetRoleBackpack(UINT32 roleId, std::vector<SPropertyAttr>& vec)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	vec.assign(it->second->packs.begin(), it->second->packs.end());
	return MLS_OK;
}

int RolesInfoMan::RoleSite::GetRoleAcceptTask(UINT32 roleId, std::vector<SAcceptTaskInfo>& vec)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;
	vec.assign(it->second->acceptTasks.begin(), it->second->acceptTasks.end());
	return MLS_OK;
}

int RolesInfoMan::RoleSite::GetRoleStorageSpace(UINT32 roleId, BYTE& packSize, BYTE& wareSize)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	packSize = it->second->packSize;
	wareSize = it->second->wareSize;
	return MLS_OK;
}

int RolesInfoMan::RoleSite::GetRoleMoney(UINT32 roleId, UINT64& gold, UINT32& cash, UINT32& cashcount)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	gold = BigNumberToU64(it->second->gold);
	cash = it->second->cash;
	cashcount = it->second->cashcount;
	return MLS_OK;
}

int RolesInfoMan::RoleSite::GetRoleVistInfo(UINT32 roleId, UINT32& vistCount, UINT32& vit, char* lastTime)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	vistCount = it->second->vist;
	vit = SGSU32Sub(it->second->vit, it->second->onlineRecoveryVit);
	if (lastTime != NULL)
		strcpy(lastTime, it->second->lastEnterTime);
	return MLS_OK;
}

int RolesInfoMan::RoleSite::GetRoleSkills(UINT32 roleId, std::vector<SSkillMoveInfo>& skillsVec)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	skillsVec.assign(it->second->studySkills.begin(), it->second->studySkills.end());
	return MLS_OK;
}

int RolesInfoMan::RoleSite::GetRoleNick(UINT32 roleId, char* pszNick)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	strcpy(pszNick, it->second->nick);
	return MLS_OK;
}

int RolesInfoMan::RoleSite::GetRoleInfo(UINT32 roleId, SRoleInfos* pRoleInfo)
{
	int n = member_offset(SRoleInfos, roleId, packs);

	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	SRoleInfos* pInfo = it->second;
	memcpy(pRoleInfo, pInfo, n);
	return MLS_OK;
}

int RolesInfoMan::RoleSite::GetRoleCurrentVit(UINT32 roleId, UINT32& vit)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	//vit = it->second->vit;
	vit = SGSU32Sub(it->second->vit, it->second->onlineRecoveryVit);
	return MLS_OK;
}

int RolesInfoMan::RoleSite::GetRoleFaction(UINT32 roleId, BYTE& factionId)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;
	factionId = it->second->factionId;
	return MLS_OK;
}

int RolesInfoMan::RoleSite::GetRoleWearBackpack(UINT32 roleId, std::vector<SPropertyAttr>& wear, std::vector<SPropertyAttr>& backpack)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	wear.assign(it->second->wearEquipment.begin(), it->second->wearEquipment.end());
	backpack.assign(it->second->packs.begin(), it->second->packs.end());
	return MLS_OK;
}

int RolesInfoMan::RoleSite::GetRoleWearEquip(UINT32 roleId, std::vector<SPropertyAttr>& wear)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	wear.assign(it->second->wearEquipment.begin(), it->second->wearEquipment.end());
	return MLS_OK;
}

int RolesInfoMan::RoleSite::GetRoleTitleId(UINT32 roleId, UINT16& titleId)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	titleId = it->second->titleId;
	return MLS_OK;
}

int RolesInfoMan::RoleSite::GetRoleWearDressId(UINT32 roleId, UINT16& dressId, std::vector<UINT16>* pDressVec)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	dressId = it->second->dressId;
	if (pDressVec != NULL)
		pDressVec->assign(it->second->dress.begin(), it->second->dress.end());

	return MLS_OK;
}

int RolesInfoMan::RoleSite::CalcRoleCompatPower(UINT32 roleId, UINT32* pNewPower)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	UINT32 newPower = it->second->CalcRoleCombatPower();
	if (pNewPower != NULL)
		*pNewPower = newPower;

	return MLS_OK;
}

int RolesInfoMan::RoleSite::GetRoleCompatPower(UINT32 roleId, UINT32& cp)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	cp = it->second->cp;
	return MLS_OK;
}

int RolesInfoMan::RoleSite::GetRoleEquipFragment(UINT32 roleId, std::vector<SEquipFragmentAttr>& fragmentVec)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	fragmentVec.assign(it->second->fragments.begin(), it->second->fragments.end());
	return MLS_OK;
}

//zpy 获取炼魂碎片
int RolesInfoMan::RoleSite::GetRoleChainSoulFragment(UINT32 roleId, std::vector<SChainSoulFragmentAttr>& fragmentVec)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	fragmentVec.assign(it->second->chainSoulFragments.begin(), it->second->chainSoulFragments.end());
	return MLS_OK;
}

int RolesInfoMan::RoleSite::GetRolePosition(UINT32 roleId, SGSPosition* pos)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	memcpy(pos, &it->second->pos, sizeof(SGSPosition));
	return MLS_OK;
}

int RolesInfoMan::RoleSite::GetRoleExp(UINT32 roleId, SBigNumber& exp)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	exp = it->second->exp;
	return MLS_OK;
}

int RolesInfoMan::RoleSite::GetRoleUpgradeNotify(UINT32 roleId, SRoleUpgradeNotify* pUpgradeNotify)
{
	BYTE idx = 0;
	BYTE maxSkillNum = (BYTE)((MAX_RES_USER_BYTES - member_offset(SRoleUpgradeNotify, roleLevel, data)) / sizeof(SSkillMoveInfo));

	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	SRoleInfos* pRole = it->second;
	pUpgradeNotify->roleLevel = pRole->level;		//角色等级
	pUpgradeNotify->gold = pRole->gold;				//游戏币
	pUpgradeNotify->exp = pRole->exp;				//经验
	pUpgradeNotify->hp = pRole->hp;					//生命
	pUpgradeNotify->mp = pRole->mp;					//魔力
	pUpgradeNotify->vit = pRole->vit;				//体力
	pUpgradeNotify->ap = pRole->ap;					//剩余属性点数
	pUpgradeNotify->attack = pRole->attack;			//攻击
	pUpgradeNotify->def = pRole->def;				//防御
	pUpgradeNotify->cp = pRole->cp;					//当前战斗力;
	memcpy(&pUpgradeNotify->pos, &pRole->pos, sizeof(SGSPosition));	//所在位置

	BYTE si = (BYTE)pRole->studySkills.size();
	for (BYTE i = 0; i < si; ++i)
	{
		memcpy(&pUpgradeNotify->data[idx], &pRole->studySkills[i], sizeof(SSkillMoveInfo));
		pUpgradeNotify->data[idx].hton();
		if (++idx >= maxSkillNum)
			break;
	}
	pUpgradeNotify->skillNum = idx;
	return MLS_OK;
}

int RolesInfoMan::RoleSite::GetRoleHelps(UINT32 roleId, std::vector<UINT16>& helpsVec)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	helpsVec.assign(it->second->helps.begin(), it->second->helps.end());
	return MLS_OK;
}

int RolesInfoMan::RoleSite::GetRoleMagics(UINT32 roleId, std::vector<UINT16>& magicsVec)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	magicsVec.assign(it->second->magics.begin(), it->second->magics.end());
	return MLS_OK;
}

int RolesInfoMan::RoleSite::GetRoleEquipPosAttr(UINT32 roleId, std::vector<SEquipPosAttr>& equipPosVec)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	equipPosVec.assign(it->second->equipPos.begin(), it->second->equipPos.end());
	return MLS_OK;
}

//zpy 获取炼魂部件
int RolesInfoMan::RoleSite::GetRoleChainSoulPos(UINT32 roleId, std::vector<SChainSoulPosAttr>& posAttr)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	posAttr.assign(it->second->chainSoulPos.begin(), it->second->chainSoulPos.end());
	return MLS_OK;
}

//zpy 天梯匹配
int RolesInfoMan::RoleSite::LadderMatching(UINT32 roleId, UINT32 matching_rank[LADDER_MATCHTING_MAX_NUM], UINT16 *refresh_count, UINT32 *rpcoin)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	//是否可免费匹配
	for (int i = 0; i < LADDER_MATCHTING_MAX_NUM; ++i)
	{
		if (it->second->ladder_matching_rank[i] != 0)
		{
			//代币是否充足
			UINT32 cost = GetGameCfgFileMan()->GetGameTypeRefurbishExpend(ESGS_INST_HIGH_LADDER, it->second->ladder_refresh_count);
			if (cost > it->second->cash)
			{
				return MLS_CASH_NOT_ENOUGH;
			}

			//扣除代币
			it->second->ConsumeToken(cost, "天梯匹配");
			*rpcoin = it->second->cash;
			*refresh_count = ++it->second->ladder_refresh_count;
			memcpy(it->second->ladder_matching_rank, matching_rank, sizeof(it->second->ladder_matching_rank));
			return MLS_OK;
		}
	}
	*rpcoin = it->second->cash;
	*refresh_count = it->second->ladder_refresh_count;
	memcpy(it->second->ladder_matching_rank, matching_rank, sizeof(it->second->ladder_matching_rank));
	return MLS_OK;
}

//zpy 刷新天梯排行范围
int RolesInfoMan::RoleSite::RefreshLadderRankScope(UINT32 roleId, UINT32 matching_rank[LADDER_MATCHTING_MAX_NUM])
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	memcpy(it->second->ladder_matching_rank, matching_rank, sizeof(it->second->ladder_matching_rank));
	return MLS_OK;
}

//zpy 执行天梯挑战
int RolesInfoMan::RoleSite::DoLadderChallenge(UINT32 roleId, UINT16 *ret_challenge_count)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	// 检查挑战次数
	UINT32 maxNum = GetGameCfgFileMan()->GetLadderMaxChallengeNum();
	if (maxNum <= it->second->ladder_challenge_count)
	{
		return MLS_LADDER_CHALLENGE_UPPER_LIMIT;
	}

	// 检查cd时间
	LTMSEL interval = (GetMsel() - it->second->ladder_last_finish_time) / 1000;
	if (GetGameCfgFileMan()->GetLadderChallengeCDTime() >= interval)
	{
		return MLS_LADDER_CHALLENGE_COOLING;
	}

	*ret_challenge_count = ++it->second->ladder_challenge_count;

	return MLS_OK;
}

//zpy 消除冷却时间
int RolesInfoMan::RoleSite::EliminateLadderCoolingTime(UINT32 roleId, SEliminateLadderCDTimeRes *pEliminate)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	// 检查cd时间
	LTMSEL interval = (GetMsel() - it->second->ladder_last_finish_time) / 1000;
	if (GetGameCfgFileMan()->GetLadderChallengeCDTime() >= interval)
	{
		const UINT32 cost = GetGameCfgFileMan()->GetGameTypeEliminateExpend(ESGS_INST_HIGH_LADDER, it->second->ladder_eliminate_count + 1);
		if (cost > it->second->cash)
		{
			return MLS_CASH_NOT_ENOUGH;
		}

		//扣除代币
		it->second->ConsumeToken(cost, "消除天梯挑战CD时间");
		++it->second->ladder_eliminate_count;
		it->second->ladder_last_finish_time = 0;
	}

	pEliminate->rpcoin = it->second->cash;
	pEliminate->eliminate_count = it->second->ladder_eliminate_count;

	return MLS_OK;
}

//zpy 完成天梯挑战
int RolesInfoMan::RoleSite::FinishLadderChallenge(UINT32 roleId, BYTE finishCode/*, SFinishLadderChallengeRes *pFinishLadder*/)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	if (finishCode == MLS_OK)
	{
		++it->second->ladder_win_in_row;
	}
	else
	{
		it->second->ladder_win_in_row = 0;
	}
	it->second->ladder_last_finish_time = GetMsel();
	//pFinishLadder->win_in_row = it->second->ladder_win_in_row;
	//pFinishLadder->cooling_time = GetGameCfgFileMan()->GetLadderChallengeCDTime();
	memset(it->second->ladder_matching_rank, 0, sizeof(it->second->ladder_matching_rank));
	return MLS_OK;
}

//zpy 重置被挑战者天梯排行范围
int RolesInfoMan::RoleSite::ResetChallengerLadderRankScope(UINT32 roleId)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;
	memset(it->second->ladder_matching_rank, 0, sizeof(it->second->ladder_matching_rank));
	return MLS_OK;
}

//zpy 添加机器人
int RolesInfoMan::RoleSite::AddRobot(UINT32 roleId, UINT32 robotId)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;
	it->second->robotSet.insert(robotId);
	return MLS_OK;
}

//zpy 是否存在机器人
int RolesInfoMan::RoleSite::HasRobot(UINT32 roleId, UINT32 robotId, bool *ret)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;
	*ret = it->second->robotSet.find(robotId) != it->second->robotSet.end();
	return MLS_OK;
}

//zpy 清理机器人
int RolesInfoMan::RoleSite::ClearRobot(UINT32 roleId)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;
	it->second->robotSet.clear();
	return MLS_OK;
}

//zpy 查询所有角色的ID
int RolesInfoMan::RoleSite::QueryAllRoles(UINT32 userId, SQueryAllRolesRes* pAllRoles)
{
	pAllRoles->num = 0;
	CXTAutoLock lock(m_rolesMapLocker);
	for (std::map<UINT32, SRoleInfos*>::iterator itr = m_rolesMap.begin(); itr != m_rolesMap.end(); ++itr)
	{
		pAllRoles->data[pAllRoles->num].roleId = itr->first;
		pAllRoles->data[pAllRoles->num].occuId = itr->second->occuId;
		++pAllRoles->num;
	}
	return MLS_OK;
}

int RolesInfoMan::RoleSite::GetRoleSignInInfo(UINT32 roleId, std::vector<LTMSEL>& signVec)
{
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	signVec.assign(it->second->signInVec.begin(), it->second->signInVec.end());
	return MLS_OK;
}
int RolesInfoMan::RoleSite::CheckPlayerRechargeOrderStatus(UINT32 userId, UINT32 roleId, BOOL& hasNotReceiveOrder)
{
	hasNotReceiveOrder = false;
	CXTAutoLock lock(m_rolesMapLocker);
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.find(roleId);
	if (it == m_rolesMap.end())
		return MLS_ROLE_NOT_EXIST;

	SRoleInfos* pRole = it->second;
	std::vector<RechargeMan::SOrderItem> orderItemVec;
	int retCode = GetRechargeMan()->GetNotReceiveTokenRechargeId(roleId, orderItemVec);
	if (retCode != MLS_OK)
		return retCode;

	int si = (int)orderItemVec.size();
	for (int i = 0; i < si; ++i)
	{
		RechargeCfg::SRechargeCfgAttr cashAttr;
		int retCode = GetGameCfgFileMan()->GetRechargeAttr(orderItemVec[i].rechargeId, cashAttr);
		if (retCode != MLS_OK)
			continue;

		UINT32 rmb = 0;
		UINT16 rechargeId = 0;
		BYTE status = 0;
		retCode = GetRechargeMan()->GetRechargeInfo(userId, roleId, orderItemVec[i].orderid.c_str(), rechargeId, rmb, status);
		if (retCode != MLS_OK)
			continue;

		hasNotReceiveOrder = true;

		//更新充值总额(RMB)
		pRole->cashcount = SGSU32Add(pRole->cashcount, cashAttr.rmb);

		//发送邮件
		EmailCleanMan::SSyncEmailSendContent email;
		email.pGBCaption = "购买钻石";
		char szBody[128] = { 0 };
		sprintf(szBody, "购买钻石, 购%u返%u", cashAttr.rpcoin, cashAttr.firstGive);
		email.pGBBody = szBody;
		email.pGBSenderNick = "系统";
		email.receiverRoleId = roleId;
		email.receiverUserId = userId;
		email.type = ESGS_EMAIL_RECHARGE_RETURN;
		email.rpcoin = SGSU32Add(cashAttr.rpcoin, cashAttr.firstGive);
		GetEmailCleanMan()->AsyncSendEmail(email);

		//更新状态
		GetRechargeMan()->UpdateLocalPartOrderInfo(orderItemVec[i].orderid.c_str(), status, cashAttr.rpcoin, cashAttr.firstGive);
	}
	return retCode;
}

//add by hxw 20151124
void RolesInfoMan::RoleSite::CleanOnlineData()
{
	if (m_rolesMap.size() <= 0)
		return;
	LTMSEL ontime = GetMsel();
	char ctime[33] = { 0 };
	MselToStr(ontime, ctime);

	//Add by hxw 20151024	
	SRoleInfos* pRole = NULL;
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.begin();
	for (; it != m_rolesMap.end(); ++it)
	{
		pRole = it->second;
		if (pRole->onlineVec.id <= 0)
			continue;

		GetRankRewardMan()->UpdateOnlineTime(pRole->onlineVec.id, (ontime - pRole->onlineVec.lasttime) / 1000);
		pRole->onlineVec.OnClear();
		pRole->onlineVec.lasttime = ontime;
	}

	//GetRankRewardMan()->QueryOnlineInfoForDB(it->first,it->second,ctime);
	//end
}

void RolesInfoMan::RoleSite::CleanEverydayData()
{
	std::map<UINT32/*角色id*/, SRoleInfos*>::iterator it = m_rolesMap.begin();
	for (; it != m_rolesMap.end(); ++it)
	{
		SRoleInfos* pRole = it->second;
		pRole->exchangedTimes = 0;
		pRole->vitExchangedTimes = 0;
		pRole->ladder_refresh_count = 0;
		pRole->ladder_challenge_count = 0;
		pRole->ladder_eliminate_count = 0;
		pRole->ladder_last_finish_time = 0;

	}
}