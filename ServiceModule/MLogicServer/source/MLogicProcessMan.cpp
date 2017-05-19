#include <math.h>
#include "MLogicProcessMan.h"
#include "DBSClient.h"
#include "GWSClient.h"
#include "MLogicProtocol.h"
#include "ConfigFileMan.h"
#include "GameCfgFileMan.h"
#include "PersonalTimerTaskMan.h"
#include "FunModuleMan.h"
#include "VIPMan.h"
#include "cyaLog.h"
#include "cyaMaxMin.h"
#include "AchievementMan.h"
#include "RankRewardMan.h"

#define ADD_MLGC_PROC_HANDLER(cmdCode, fun) \
do \
		{	\
		m_msgProcHandler.insert(std::make_pair(cmdCode, &MLogicProcessMan::fun));	\
		} while (0)

static MLogicProcessMan* sg_pMlogicMan = NULL;
void InitMLogicProcessMan()
{
	ASSERT(sg_pMlogicMan == NULL);
	sg_pMlogicMan = new MLogicProcessMan();
	ASSERT(sg_pMlogicMan != NULL);
}

MLogicProcessMan* GetMLogicProcessMan()
{
	return sg_pMlogicMan;
}

void DestroyMLogicProcessMan()
{
	MLogicProcessMan* pMlogicMan = sg_pMlogicMan;
	sg_pMlogicMan = NULL;
	if (pMlogicMan != NULL)
		delete pMlogicMan;
}

void ReceiveRewardRefresh(UINT32 userId, UINT32 roleId, const SReceiveRewardRefresh &refresh_info)
{
	//ˢ�±���
	if (refresh_info.prop)
		GetRefreshDBMan()->InputUpdateInfo(userId, roleId, REFRESH_ROLE_BACKPACK);

	//ˢ�½��
	if (refresh_info.gold)
		GetRefreshDBMan()->InputUpdateInfo(userId, roleId, REFRESH_ROLE_GOLD);

	//ˢ�´���
	if (refresh_info.rpcoin)
		GetRefreshDBMan()->InputUpdateInfo(userId, roleId, REFRESH_ROLE_CASH);

	//ˢ����Ƭ
	if (refresh_info.fragment)
		GetRefreshDBMan()->InputUpdateInfo(userId, roleId, REFRESH_ROLE_FRAGMENT);

	//ˢ��������Ƭ
	if (refresh_info.chainSoulFragment)
		GetRefreshDBMan()->InputUpdateInfo(userId, roleId, REFRESH_ROLE_CHAINSOUL_FRAGMENT);

	// ˢ�½�ɫʱװ
	if (refresh_info.dress)
		GetRefreshDBMan()->InputUpdateInfo(userId, roleId, REFRESH_ROLE_DRESS);

	// ˢ�½�ɫ����
	if (refresh_info.vit)
		GetRefreshDBMan()->InputUpdateInfo(userId, roleId, REFRESH_ROLE_VIT);

	// ˢ�½�ɫ����
	if (refresh_info.upgrade)
	{
		/* zpy �ɾ�ϵͳ���� */
		GetAchievementMan()->OnRoleUpgrade(userId, roleId);

		GetRefreshDBMan()->InputUpdateInfo(userId, roleId, REFRESH_ROLE_UPGRADE);
	}

	// ˢ�½�ɫ����
	if (refresh_info.exp)
		GetRefreshDBMan()->InputUpdateInfo(userId, roleId, REFRESH_ROLE_EXP);
}

MLogicProcessMan::MLogicProcessMan()
{
	ADD_MLGC_PROC_HANDLER(MLS_QUERY_ROLES, OnQueryRoles);
	ADD_MLGC_PROC_HANDLER(MLS_ENTER_ROLE, OnEnterRoles);
	ADD_MLGC_PROC_HANDLER(MLS_QUERY_BACKPACK, OnQueryBackpack);
	ADD_MLGC_PROC_HANDLER(MLS_QUERY_EMAIL_CONTENT, OnQueryEmailContent);

	ADD_MLGC_PROC_HANDLER(MLS_CREATE_ROLE, OnCreateRole);
	ADD_MLGC_PROC_HANDLER(MLS_QUERY_FRIEND, OnQueryFriends);
	ADD_MLGC_PROC_HANDLER(MLS_ADD_FRIEND, OnAddFriend);
	ADD_MLGC_PROC_HANDLER(MLS_DEL_FRIEND, OnDelFriend);

	ADD_MLGC_PROC_HANDLER(MLS_QUERY_FINISHED_HELP, OnQueryFinishedHelp);
	ADD_MLGC_PROC_HANDLER(MLS_FINISH_ROLE_HELP, OnFinishNewPlayerHelp);
	ADD_MLGC_PROC_HANDLER(MLS_QUERY_MAGICS, OnQueryMagics);
	ADD_MLGC_PROC_HANDLER(MLS_GAIN_MAGIC, OnGainMagic);

	ADD_MLGC_PROC_HANDLER(MLS_ENTER_INST, OnEnterInst);
	ADD_MLGC_PROC_HANDLER(MLS_FETCH_INST_AREA_DROPS, OnFetchInstAreaMonsterDrops);
	ADD_MLGC_PROC_HANDLER(MLS_KILL_MONSTER_EXP, OnKillMonsterExp);
	ADD_MLGC_PROC_HANDLER(MLS_QUERY_FINISHED_INST, OnQueryFinishedInst);
	ADD_MLGC_PROC_HANDLER(MLS_PICKUP_INST_MONSTER_DROP, OnPickupInstMonsterDrop);
	ADD_MLGC_PROC_HANDLER(MLS_PICKUP_INST_BOX, OnPickupInstBox);
	ADD_MLGC_PROC_HANDLER(MLS_FINISH_INST, OnFinishInst);
	ADD_MLGC_PROC_HANDLER(MLS_PICKUP_BROKEN_DROP, OnPickupBrokenDrop);

	ADD_MLGC_PROC_HANDLER(MLS_SELL_PROPERTY, OnSellProperty);
	ADD_MLGC_PROC_HANDLER(MLS_DROP_PROPERTY, OnDropProperty);
	ADD_MLGC_PROC_HANDLER(MLS_BUY_STORAGE_SPACE, OnBuyStorageSpace);

	ADD_MLGC_PROC_HANDLER(MLS_ACCEPT_TASK, OnAcceptTask);
	ADD_MLGC_PROC_HANDLER(MLS_DROP_TASK, OnDropTask);
	ADD_MLGC_PROC_HANDLER(MLS_QUERY_ACCEPTED_TASK, OnQueryAcceptedTasks);
	ADD_MLGC_PROC_HANDLER(MLS_COMMIT_TASK_FINISH, OnCommitTaskFinish);
	ADD_MLGC_PROC_HANDLER(MLS_QUERY_FIN_TASK_HISTORY, OnQueryFinTaskHistory);
	ADD_MLGC_PROC_HANDLER(MLS_GET_DAILY_TASK, OnGetDailyTask);
	ADD_MLGC_PROC_HANDLER(MLS_UPDATE_TASK_TARGET, OnUpdateTaskTarget);
	ADD_MLGC_PROC_HANDLER(MLS_RECEIVE_TASK_REWARD, OnReceiveTaskReward);

	ADD_MLGC_PROC_HANDLER(MLS_DEL_EMAIL, OnDelEmails);
	ADD_MLGC_PROC_HANDLER(MLS_QUERY_EMAIL_LIST, OnQueryEmailList);
	ADD_MLGC_PROC_HANDLER(MLS_RECEIVE_EMAIL_ATTACHMENT, OnReceiveEmailAttachments);

	ADD_MLGC_PROC_HANDLER(MLS_GEM_COMPOSE, OnGemCompose);
	ADD_MLGC_PROC_HANDLER(MLS_LOAD_EQUIPMENT, OnLoadEquipment);
	ADD_MLGC_PROC_HANDLER(MLS_UNLOAD_EQUIPMENT, OnUnLoadEquipment);
	ADD_MLGC_PROC_HANDLER(MLS_QUERY_ROLE_APPEARANCE, OnQueryRoleApperance);

	ADD_MLGC_PROC_HANDLER(MLS_QUERY_ROLE_EXPLICIT, OnQueryRoleExplicit);
	ADD_MLGC_PROC_HANDLER(MLS_USE_PROPERTY, OnUseProperty);
	ADD_MLGC_PROC_HANDLER(MLS_RECEIVE_REWARD, OnReceiveAchievementReward);

	ADD_MLGC_PROC_HANDLER(MLS_CASH_PROPERTY, OnCashProperty);
	ADD_MLGC_PROC_HANDLER(MLS_QUERY_ACHIEVEMENT, OnQueryAchievement);
	ADD_MLGC_PROC_HANDLER(MLS_DEL_ROLE, OnDelRole);
	ADD_MLGC_PROC_HANDLER(MLS_QUERY_GUILD_NAME, OnQueryGuildName);

	ADD_MLGC_PROC_HANDLER(MLS_LEAVE_ROLE, OnLeaveRoles);
	ADD_MLGC_PROC_HANDLER(MLS_USER_EXIT, OnUserExit);
	ADD_MLGC_PROC_HANDLER(MLS_PERSONAL_TIMER_TASK, OnReqTimerTask);

	//ADD_MLGC_PROC_HANDLER(MLS_INST_SWEEP, OnInstSweep);
	ADD_MLGC_PROC_HANDLER(MLS_QUERY_ROLE_SKILLS, OnQueryRoleSkills);
	ADD_MLGC_PROC_HANDLER(MLS_SKILL_UPGRADE, OnSkillUpgrade);
	ADD_MLGC_PROC_HANDLER(MLS_CHANGE_FACTION, OnChangeFaction);

	ADD_MLGC_PROC_HANDLER(MLS_LOAD_SKILL, OnLoadSkill);
	ADD_MLGC_PROC_HANDLER(MLS_CANCEL_SKILL, OnCancelSkill);

	ADD_MLGC_PROC_HANDLER(MLS_EQUIPMENT_COMPOSE, OnEquipmentCompose);
	ADD_MLGC_PROC_HANDLER(MLS_EQUIP_POS_STRENGTHEN, OnEquipPosStrengthen);
	ADD_MLGC_PROC_HANDLER(MLS_EQUIP_POS_RISE_STAR, OnEquipPosRiseStar);
	ADD_MLGC_PROC_HANDLER(MLS_EQUIP_POS_INLAID_GEM, OnEquipPosInlaidGem);
	ADD_MLGC_PROC_HANDLER(MLS_EQUIP_POS_REMOVE_GEM, OnEquipPosRemoveGem);
	ADD_MLGC_PROC_HANDLER(MLS_GET_EQUIP_POS_ATTR, OnGetEquipPosAttr);

	ADD_MLGC_PROC_HANDLER(MLS_PROP_BUY_BACK, OnBuyBackProperty);
	ADD_MLGC_PROC_HANDLER(MLS_QUERY_ROLE_SELL_PROP_RCD, OnQueryPropSellRcd);
	ADD_MLGC_PROC_HANDLER(MLS_DELTA_TOKEN, OnRechargeToken);

	ADD_MLGC_PROC_HANDLER(MLS_GET_ROLE_OCCUPY_INST, OnGetRoleOccupyInst);
	ADD_MLGC_PROC_HANDLER(MLS_SET_ROLE_TITLE_ID, OnSetRoleTitleId);
	ADD_MLGC_PROC_HANDLER(MLS_MALL_BUY_GOODS, OnMallBuyGoods);

	ADD_MLGC_PROC_HANDLER(MLS_QUERY_ROLE_DRESS, OnQueryRoleDress);
	ADD_MLGC_PROC_HANDLER(MLS_WEAR_DRESS, OnWearDress);
	ADD_MLGC_PROC_HANDLER(MLS_FETCH_ROBOT, OnFetchRobot);
	ADD_MLGC_PROC_HANDLER(MLS_GET_PERSONAL_TRANSFER_ENTER, OnGetPersonalTransferEnter);

	ADD_MLGC_PROC_HANDLER(MLS_QUERY_EQUIP_FRAGMENT, OnQueryEquipFragment);
	ADD_MLGC_PROC_HANDLER(MLS_ROLE_REVIVE, OnRoleRevive);
	ADD_MLGC_PROC_HANDLER(MLS_GET_OPEN_PERSONAL_RAND_INST, OnGetOpenPersonalRandInst);

	ADD_MLGC_PROC_HANDLER(MLS_QUERY_GOLD_EXCHANGE, OnQueryGoldExchange);
	ADD_MLGC_PROC_HANDLER(MLS_GOLD_EXCHANGE, OnGoldExchange);
	ADD_MLGC_PROC_HANDLER(MLS_FRIEND_APPLY_CONFIRM, OnFriendApplyConfirm);
	ADD_MLGC_PROC_HANDLER(MLS_QUERY_FRIEND_APPLY, OnQueryFriendsApply);

	ADD_MLGC_PROC_HANDLER(MLS_GET_RECOMMEND_FRIEND_LIST, OnGetRecommendFriends);
	ADD_MLGC_PROC_HANDLER(MLS_SEARCH_PLAYER, OnSearchPlayer);
	ADD_MLGC_PROC_HANDLER(MLS_NOT_RECEIVE_VIP_REWARD, OnQueryNotReceiveVIPReward);
	ADD_MLGC_PROC_HANDLER(MLS_RECEIVE_VIP_REWARD, OnReceiveVIPReward);

	ADD_MLGC_PROC_HANDLER(MLS_QUERY_FIRST_RECHARGE_GIVE, OnQueryRechargeFirstGive);
	ADD_MLGC_PROC_HANDLER(MLS_QUERY_ONLINE_SIGN_IN, OnQuerySignIn);
	ADD_MLGC_PROC_HANDLER(MLS_ONLINE_SIGN_IN, OnlineSignIn);
	ADD_MLGC_PROC_HANDLER(MLS_QUERY_RECEIVED_ACTIVITY, OnQueryReceivedActivity);
	ADD_MLGC_PROC_HANDLER(MLS_RECEIVED_ACTIVITY, OnReceivedActivity);

	ADD_MLGC_PROC_HANDLER(MLS_INST_SWEEP, OnInstSweep);
	ADD_MLGC_PROC_HANDLER(MLS_RECEIVE_CASH_CODE_REWARD, OnReceiveCashCodeReward);
	ADD_MLGC_PROC_HANDLER(MLS_QUERY_UNLOCK_CONTENT, OnQueryUserUnlockContent);
	ADD_MLGC_PROC_HANDLER(MLS_CONTENT_UNLOCK, OnUnlockContent);

	ADD_MLGC_PROC_HANDLER(LICENCE_QUERY_UNLOCK_JOBS, OnLcsQueryUserUnlockContent);
	ADD_MLGC_PROC_HANDLER(MLS_GET_ROULETTE_REMAIN_REWARD, OnGetRouletteRemainReward);
	ADD_MLGC_PROC_HANDLER(MLS_EXTRACT_ROULETTE_REWARD, OnExtractRouletteReward);
	ADD_MLGC_PROC_HANDLER(MLS_FETCH_RECHARGE_ORDER, OnFetchRechargeOrder);

	ADD_MLGC_PROC_HANDLER(MLS_GET_RECHARGE_TOKEN, OnGetRechargeToken);
	ADD_MLGC_PROC_HANDLER(MLS_UPDATE_ROULETTE_BIG_REWARD, OnUpdateRouletteBigReward);
	ADD_MLGC_PROC_HANDLER(MLS_FETCH_ROULETTE_BIG_REWARD, OnFetchRouletteBigReward);
	ADD_MLGC_PROC_HANDLER(MLS_GET_ACTIVE_INST_CHALLENGE_TIMES, OnGetActiveInstChallengeTimes);

	ADD_MLGC_PROC_HANDLER(MLS_QUERY_VIT_EXCHANGE, OnQueryVitExchange);
	ADD_MLGC_PROC_HANDLER(MLS_VIT_EXCHANGE, OnVitExchange);

	ADD_MLGC_PROC_HANDLER(MLS_QUERY_ROLE_DAILY_SIGN, OnQueryRoleDailySign);
	ADD_MLGC_PROC_HANDLER(MLS_ROLE_DAILY_SIGN, OnRoleDailySign);

	ADD_MLGC_PROC_HANDLER(MLS_QUERY_CHAIN_SOUL_FRAGMENT, OnQueryChainSoulFragment);
	ADD_MLGC_PROC_HANDLER(MLS_QUERY_CHAIN_SOUL_POS, OnQueryChainSoulPos);
	ADD_MLGC_PROC_HANDLER(MLS_CHAIN_SOUL_POS_UPGRADE, OnChainSoulPosUpgrade);

	ADD_MLGC_PROC_HANDLER(MLS_ATTACK_WORLD_BOOS, OnAttackWorldBoss);
	ADD_MLGC_PROC_HANDLER(MLS_QUERY_WORLD_BOSS_RANK, OnQueryWorldBossHurtRank);
	ADD_MLGC_PROC_HANDLER(MLS_WASKILLED_BY_WORLDBOSS, OnWasKilledByWorldBoss);
	ADD_MLGC_PROC_HANDLER(MLS_RESURRECTION_IN_WORLDBOSS, OnResurrectionInWorldBoss);
	ADD_MLGC_PROC_HANDLER(MLS_QUERY_WORLDBOSS_INFO, OnQueryWorldBossInfo);

	//by add hxw 20151021
	//20151028 hxw �޸�MSL_QUERY_LV_RANK_INFO ΪMSL_QUERY_LV_RANK
	ADD_MLGC_PROC_HANDLER(MSL_QUERY_LV_RANK, QueryLvRank);
	ADD_MLGC_PROC_HANDLER(MSL_QUERY_INNUM_RANK, QueryInsnumRank);
	ADD_MLGC_PROC_HANDLER(MSL_QUERY_INNUM_RANK_INFO, QueryInsnumRankInfo);
	ADD_MLGC_PROC_HANDLER(MSL_GET_LV_RANK_REWARD, GetLvRankReward);
	ADD_MLGC_PROC_HANDLER(MSL_GET_INNUM_REWARD, GetInsnumRankReward);
	ADD_MLGC_PROC_HANDLER(MSL_QUERY_STRONGER_INFO, QueryStrongerInfo);
	ADD_MLGC_PROC_HANDLER(MSL_GET_STRONGER_REWARD, GetStrongerReward);
	ADD_MLGC_PROC_HANDLER(MSL_QUERY_ONLINE_INFO, QueryOnlineInfo);
	ADD_MLGC_PROC_HANDLER(MSL_GET_ONLINE_REWARD, GetOnlineReward);
	//end
	//by add hxw 20151028
	ADD_MLGC_PROC_HANDLER(MSL_QUERY_LV_RANK_INFO, QueryLvRankInfo);
	//end

	ADD_MLGC_PROC_HANDLER(MLS_GET_LADDER_ROLE_INFO, OnGetLadderInfo);
	ADD_MLGC_PROC_HANDLER(MLS_MATCHING_LADDER, OnLadderMatchingPlayer);
	ADD_MLGC_PROC_HANDLER(MLS_REFRESH_LADDER_RANK, OnRefreshLadderRankScope);
	ADD_MLGC_PROC_HANDLER(MLS_CHALLENGE_LADDER, OnLadderChallengePlayer);
	ADD_MLGC_PROC_HANDLER(MLS_ELIMINATE_LADDER_CD, OnEliminateLadderCoolingTime);
	//ADD_MLGC_PROC_HANDLER(MLS_FINISH_LADDER_CHALLENGE,OnFinishLadderChallenge);

	ADD_MLGC_PROC_HANDLER(MLS_QUERY_ALL_ROLES, OnQueryAllRoles);

	ADD_MLGC_PROC_HANDLER(MLS_QUERY_HAS_ACTIVITY_INST_TYPE, OnQueryHasAcivityInstType);

	ADD_MLGC_PROC_HANDLER(MLS_QUERY_WEEKFREE_ACTOR, OnQueryWeekFreeActorType);

	ADD_MLGC_PROC_HANDLER(MLS_QUERY_COMPMODE_ENTER_NUM, OnQueryEnterKOFNum);
	ADD_MLGC_PROC_HANDLER(MLS_BUY_COMPMODE_ENTER_NUM, OnBuyEnterKOFNum);

	ADD_MLGC_PROC_HANDLER(MLS_QUERY_1V1_ENTER_NUM, OnQueryEnterOneVsOneNum);
	ADD_MLGC_PROC_HANDLER(MLS_BUY_1V1_ENTER_NUM, OnBuyEnterOneVsOneNum);
}

MLogicProcessMan::~MLogicProcessMan()
{

}

int MLogicProcessMan::ProcessLogic(GWSClient* gwsSession, UINT16 cmdCode, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	int retCode = MLS_OK;
	std::map<UINT16, fnMsgProcessHandler>::iterator it = m_msgProcHandler.find(cmdCode);
	if (it != m_msgProcHandler.end())
		retCode = (this->*(it->second))(gwsSession, pHead, pData, nDatalen);
	else
		gwsSession->SendBasicResponse(pHead, MLS_UNKNOWN_CMD_CODE, cmdCode);
	return retCode;
}

int MLogicProcessMan::OnQueryRoles(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SQueryRolesReq) == nDatalen);
	if (sizeof(SQueryRolesReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_ROLES);

	SQueryRolesReq* req = (SQueryRolesReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQueryRolesRes* pRoles = (SQueryRolesRes*)(buf + nLen);

	int retCode = GetRolesInfoMan()->QueryUserRoles(req->userId, pRoles);
	LogInfo(("��ѯ�û�[%u]��ɫ��Ϣ, ������[%d]!", req->userId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_QUERY_ROLES);

	nLen += sizeof(SQueryRolesRes)+(pRoles->rolesCount - 1) * sizeof(SQueryRoleAttr);
	return gwsSession->SendResponse(pHead, retCode, MLS_QUERY_ROLES, buf, nLen);
}

int MLogicProcessMan::OnEnterRoles(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerEnterRoleReq) == nDatalen);
	if (sizeof(SLinkerEnterRoleReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_ENTER_ROLE);

	SLinkerEnterRoleReq* req = (SLinkerEnterRoleReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SEnterRoleRes* pEnterRes = (SEnterRoleRes*)(buf + nLen);
	pEnterRes->userId = req->userId;
	pEnterRes->roleId = req->roleId;
	int retCode = GetRolesInfoMan()->UserEnterRole(req->userId, req->roleId, pHead->sourceId, pEnterRes);
	LogInfo(("�û�[%u]��ɫ[%u]������Ϸ, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_ENTER_ROLE);

	//ˢ��ս��
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_CP);

	//���·�����Ϣ
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_VIST_INFO);

	//�ָ�����
	GetRecoveryVitMan()->InsertRecoveryVitRole(req->userId, req->roleId, pHead->sourceId);

	nLen += sizeof(SEnterRoleRes)+(pEnterRes->skillNum - 1) * sizeof(SSkillMoveInfo);
	pEnterRes->hton();

	// zpy���� ����������Ϣ��ʾ
	{
		char message_buf[MAX_BUF_LEN] = { 0 };
		int nMessageLen = GWS_COMMON_REPLY_LEN;
		SMessageTipsNotify *pMessageTips = (SMessageTipsNotify*)(message_buf + nMessageLen);
		int result = GetMessageTipsMan()->FetchAllOfflineMessage(req->roleId, pMessageTips, &nMessageLen);
		if (result == MLS_OK && pMessageTips->num > 0)
		{
			gwsSession->SendResponse(pHead->sourceId, req->userId, MLS_OK, MLS_MESSAGETIPS_NOTIFY, message_buf, nMessageLen);
		}
	}

	//���ͻظ�
	gwsSession->SendResponse(pHead, retCode, MLS_ENTER_ROLE, buf, nLen);

	//֪ͨ��������
	//GetFriendMan()->NotifyFriendOnOffLine(req->userId, req->roleId, true);

	//����ֵ
	if (GetConfigFileMan()->EnableCheckActorOrder())
		GetPlayerInfoMan()->InputPlayerMsg(PlayerInfoMan::E_HAS_CASH_TOKEN, req->userId, req->roleId);
	return retCode;
}

int MLogicProcessMan::OnLeaveRoles(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerLeaveRoleReq) == nDatalen);
	if (sizeof(SLinkerLeaveRoleReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_LEAVE_ROLE);

	SLinkerLeaveRoleReq* req = (SLinkerLeaveRoleReq*)pData;
	req->ntoh();
	int retCode = GetRolesInfoMan()->UserLeaveRole(req->userId, req->roleId);
	LogInfo(("�û�[%u]��ɫ[%u]�뿪��Ϸ!", req->userId, req->roleId));

	//�ظ�
	gwsSession->SendBasicResponse(pHead, retCode, MLS_LEAVE_ROLE);

	//����ָ�����
	GetRecoveryVitMan()->RemoveRecoveryVitRole(req->roleId);

	//֪ͨ��������
	//GetFriendMan()->NotifyFriendOnOffLine(req->userId, req->roleId, false);
	return retCode;
}

int MLogicProcessMan::OnQueryBackpack(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerQueryBackpackReq) == nDatalen);
	if (sizeof(SLinkerQueryBackpackReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_BACKPACK);

	SLinkerQueryBackpackReq* req = (SLinkerQueryBackpackReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQueryBackpackRes* pBackpackRes = (SQueryBackpackRes*)(buf + nLen);
	int retCode = GetRolesInfoMan()->QueryBackpack(req->userId, req->roleId, req->from, req->num, pBackpackRes);
	LogInfo(("�û�[%u]��ɫ[%u]��ѯ����,from[%d]num[%d] ������[%d]!", req->userId, req->roleId, req->from, req->num, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_QUERY_BACKPACK);

	nLen += sizeof(SQueryBackpackRes)+(pBackpackRes->curNum - 1) * sizeof(SPropertyAttr);
	return gwsSession->SendResponse(pHead, retCode, MLS_QUERY_BACKPACK, buf, nLen);
}

int MLogicProcessMan::OnBackpackCleanup(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerBackpackCleanupReq) == nDatalen);
	if (sizeof(SLinkerBackpackCleanupReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_BACKPACK_CLEAN_UP);

	SLinkerBackpackCleanupReq* req = (SLinkerBackpackCleanupReq*)pData;
	req->ntoh();

	BOOL isCleanup = false;
	GetRolesInfoMan()->CleanupBackpack(req->userId, req->roleId, isCleanup);
	gwsSession->SendBasicResponse(pHead, MLS_OK, MLS_BACKPACK_CLEAN_UP);

	if (isCleanup)
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_BACKPACK);

	return MLS_OK;
}

int MLogicProcessMan::OnCreateRole(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerCreateRoleReq) == nDatalen);
	if (sizeof(SLinkerCreateRoleReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_CREATE_ROLE);

	SLinkerCreateRoleReq* req = (SLinkerCreateRoleReq*)pData;
	req->ntoh();

	if (strlen(req->nick) <= 0)
		return gwsSession->SendBasicResponse(pHead, MLS_NICK_NOT_ALLOW_EMPTY, MLS_CREATE_ROLE);

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SCreateRoleRes* pCreateRes = (SCreateRoleRes*)(buf + nLen);
	int retCode = GetRolesInfoMan()->CreateRole(req->userId, req->occuId, req->sex, req->nick, pCreateRes);
	LogInfo(("�û�[%u]������ɫ[%s : %u], ������[%d]!", req->userId, req->nick, pCreateRes->id, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_CREATE_ROLE);

	nLen += sizeof(SCreateRoleRes);
	pCreateRes->hton();
	return gwsSession->SendResponse(pHead, retCode, MLS_CREATE_ROLE, buf, nLen);
}

int MLogicProcessMan::OnQueryAcceptedTasks(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerQueryAcceptTaskReq) == nDatalen);
	if (sizeof(SLinkerQueryAcceptTaskReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_ACCEPTED_TASK);

	SLinkerQueryAcceptTaskReq* req = (SLinkerQueryAcceptTaskReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQueryAcceptTasksRes* pTaskRes = (SQueryAcceptTasksRes*)(buf + nLen);
	int retCode = GetRolesInfoMan()->QueryAcceptedTasks(req->userId, req->roleId, pTaskRes);
	LogInfo(("�û�[%u]��ɫ[%u]��ѯ�ѽ�������, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_QUERY_ACCEPTED_TASK);

	nLen += sizeof(SQueryAcceptTasksRes)+(pTaskRes->num - 1) * sizeof(SAcceptTaskInfo);
	return gwsSession->SendResponse(pHead, retCode, MLS_QUERY_ACCEPTED_TASK, buf, nLen);
}

int MLogicProcessMan::OnQueryFriends(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	//select B.accountid, B.id, B.actortype, B.nick, B.level, B.guild from friend as A,actor as B, guild as C where A.friendid=B.id and A.actorid=1;
	//select A.accountid, F.friendid, A.actortype, A.nick, A.level, G.id, G.name from friend as F join actor as A on (F.friendid=A.id) join guild as G on (G.id=A.guild) where F.actorid=1;
	ASSERT(sizeof(SLinkerQueryFriendsReq) == nDatalen);
	if (sizeof(SLinkerQueryFriendsReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_FRIEND);

	SLinkerQueryFriendsReq* req = (SLinkerQueryFriendsReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQueryFriendsRes* pFriendsRes = (SQueryFriendsRes*)(buf + nLen);
	pFriendsRes->total = 0;
	pFriendsRes->num = 0;

	int retCode = GetFriendMan()->QueryFriends(req->roleId, req->from, req->num, pFriendsRes);
	LogInfo(("��ѯ��ɫ[%u]����, ������[%d]!", req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_QUERY_FRIEND);

	nLen += sizeof(SQueryFriendsRes)+(pFriendsRes->num - 1) * sizeof(SFriendInfo);
	return gwsSession->SendResponse(pHead, retCode, MLS_QUERY_FRIEND, buf, nLen);
}

int MLogicProcessMan::OnQueryFriendsApply(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerQueryFriendApplyReq) == nDatalen);
	if (sizeof(SLinkerQueryFriendApplyReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_FRIEND_APPLY);

	SLinkerQueryFriendApplyReq* req = (SLinkerQueryFriendApplyReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQueryFriendApplyRes* pFriendApplyRes = (SQueryFriendApplyRes*)(buf + nLen);
	pFriendApplyRes->total = 0;
	pFriendApplyRes->num = 0;

	int retCode = GetFriendMan()->QueryFriendApply(req->roleId, req->from, req->num, pFriendApplyRes);
	LogInfo(("��ѯ��ɫ[%u]���������б�, ������[%d]!", req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_QUERY_FRIEND_APPLY);

	nLen += sizeof(SQueryFriendApplyRes)+(pFriendApplyRes->num - 1) * sizeof(SFriendInfo);
	return gwsSession->SendResponse(pHead, retCode, MLS_QUERY_FRIEND_APPLY, buf, nLen);
}

int MLogicProcessMan::OnAddFriend(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(nDatalen == sizeof(SLinkerAddFriendReq));
	if (sizeof(SLinkerAddFriendReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_ADD_FRIEND);

	SLinkerAddFriendReq* req = (SLinkerAddFriendReq*)pData;
	req->ntoh();

	int retCode = GetFriendMan()->AddFriend(req->userId, req->roleId, req->nick);
	LogInfo(("��ɫ[%u]��Ӻ���[%s], ������[%d]!", req->roleId, req->nick, retCode));
	return gwsSession->SendBasicResponse(pHead, retCode, MLS_ADD_FRIEND);
}

int MLogicProcessMan::OnFriendApplyConfirm(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(nDatalen == sizeof(SLinkerFriendApplyConfirmReq));
	if (sizeof(SLinkerFriendApplyConfirmReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_FRIEND_APPLY_CONFIRM);

	SLinkerFriendApplyConfirmReq* req = (SLinkerFriendApplyConfirmReq*)pData;
	req->ntoh();

	int retCode = GetFriendMan()->ConfirmFriendApply(req->userId, req->roleId, req->friendUserId, req->friendRoleId, req->status);
	LogInfo(("��ɫ[%u]%s����[%u][%u]����, ������[%d]!", req->roleId, req->status == 1 ? "ͬ��" : "�ܾ�", req->friendUserId, req->friendRoleId, retCode));
	return gwsSession->SendBasicResponse(pHead, retCode, MLS_FRIEND_APPLY_CONFIRM);
}

int MLogicProcessMan::OnDelFriend(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(nDatalen == sizeof(SLinkerDelFriendReq));
	if (sizeof(SLinkerDelFriendReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_DEL_FRIEND);

	SLinkerDelFriendReq* req = (SLinkerDelFriendReq*)pData;
	req->ntoh();

	int retCode = GetFriendMan()->DelFriend(req->userId, req->roleId, req->nick);
	LogInfo(("�û�[%u]��ɫ[%u]ɾ������[%s], ������[%d]!", req->userId, req->roleId, req->nick, retCode));
	return gwsSession->SendBasicResponse(pHead, retCode, MLS_DEL_FRIEND);
}

int MLogicProcessMan::OnGetRecommendFriends(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(nDatalen == sizeof(SLinkerGetRecommendFriendsReq));
	if (sizeof(SLinkerGetRecommendFriendsReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_GET_RECOMMEND_FRIEND_LIST);

	SLinkerGetRecommendFriendsReq* req = (SLinkerGetRecommendFriendsReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SGetRecommendFriendsRes* pRecommendFriendRes = (SGetRecommendFriendsRes*)(buf + nLen);

	GetRolesInfoMan()->GetRecommendFriends(req->userId, req->roleId, pRecommendFriendRes);
	LogInfo(("��ɫ[%u]��ȡ�Ƽ�����!", req->roleId));

	nLen += sizeof(SGetRecommendFriendsRes)+(pRecommendFriendRes->num - 1) * sizeof(SFriendInfo);
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_GET_RECOMMEND_FRIEND_LIST, buf, nLen);
}

int MLogicProcessMan::OnSearchPlayer(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(nDatalen == sizeof(SLinkerPlayerSearchReq));
	if (sizeof(SLinkerPlayerSearchReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_SEARCH_PLAYER);

	SLinkerPlayerSearchReq* req = (SLinkerPlayerSearchReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SPlayerSearchRes* pSearchRes = (SPlayerSearchRes*)(buf + nLen);

	int retCode = GetFriendMan()->SearchPlayer(req->playerNick, pSearchRes);
	LogInfo(("��ɫ[%u]��ѯ���[%s]��Ϣ, ������[%d]!", req->roleId, req->playerNick, retCode));

	nLen += sizeof(SPlayerSearchRes);
	pSearchRes->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_SEARCH_PLAYER, buf, nLen);
}

int MLogicProcessMan::OnFinishNewPlayerHelp(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerFinishHelpReq) == nDatalen);
	if (sizeof(SLinkerFinishHelpReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_FINISH_ROLE_HELP);

	SLinkerFinishHelpReq* req = (SLinkerFinishHelpReq*)pData;
	req->ntoh();

	int retCode = GetRolesInfoMan()->FinishRoleHelp(req->userId, req->roleId, req->helpId);
	LogInfo(("�û�[%u]��ɫ[%u]�����������[%d]!", req->userId, req->roleId, req->helpId));
	if (retCode == MLS_OK)
		GetPermanenDataMan()->InputUpdatePermanenDataInfo(req->userId, req->roleId, PERMANENT_HELP);
	return gwsSession->SendBasicResponse(pHead, MLS_OK, MLS_FINISH_ROLE_HELP);
}

int MLogicProcessMan::OnQueryFinishedHelp(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerQueryFinishedHelpReq) == nDatalen);
	if (sizeof(SLinkerQueryFinishedHelpReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_FINISHED_HELP);

	SLinkerQueryFinishedHelpReq* req = (SLinkerQueryFinishedHelpReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQueryFinishedHelpRes* pHelpRes = (SQueryFinishedHelpRes*)(buf + nLen);
	GetRolesInfoMan()->QueryRoleHelps(req->userId, req->roleId, pHelpRes);
	LogInfo(("�û�[%u]��ɫ[%u]��ѯ��������!", req->userId, req->roleId));
	nLen += sizeof(SQueryFinishedHelpRes)+(pHelpRes->num - 1) * sizeof(UINT16);
	pHelpRes->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_QUERY_FINISHED_HELP, buf, nLen);
}

int MLogicProcessMan::OnQueryMagics(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerQueryRoleMagicsReq) == nDatalen);
	if (sizeof(SLinkerQueryRoleMagicsReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_MAGICS);

	SLinkerQueryRoleMagicsReq* req = (SLinkerQueryRoleMagicsReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQueryRoleMagicsRes* pMagicsRes = (SQueryRoleMagicsRes*)(buf + nLen);
	GetRolesInfoMan()->QueryRoleMagics(req->userId, req->roleId, pMagicsRes);
	LogInfo(("�û�[%u]��ɫ[%u]��ѯӵ��ħ����!", req->userId, req->roleId));
	nLen += sizeof(SQueryRoleMagicsRes)+(pMagicsRes->num - 1) * sizeof(UINT16);
	pMagicsRes->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_QUERY_MAGICS, buf, nLen);
}

int MLogicProcessMan::OnGainMagic(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerGainMagicReq) == nDatalen);
	if (sizeof(SLinkerGainMagicReq) != nDatalen)
		return MLS_OK;

	SLinkerGainMagicReq* req = (SLinkerGainMagicReq*)pData;
	req->ntoh();

	int retCode = GetRolesInfoMan()->GainMagic(req->userId, req->roleId, req->instId, req->magicId);
	LogInfo(("�û�[%u]��ɫ[%u]���ħ����[%d]!", req->userId, req->roleId, req->magicId));
	if (retCode != MLS_OK)
		return retCode;

	//����ħ��������
	GetPermanenDataMan()->InputUpdatePermanenDataInfo(req->userId, req->roleId, PERMANENT_MAGIC);

	//֪ͨ�û�����µ�ħ����
	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SGainMagicNotify* pMagicNotify = (SGainMagicNotify*)(buf + nLen);
	pMagicNotify->magicId = req->magicId;
	nLen += sizeof(SGainMagicNotify);
	pMagicNotify->hton();
	gwsSession->SendResponse(pHead, MLS_OK, MLS_GAIN_MAGIC_NOTIFY, buf, nLen);
	return retCode;
}

int MLogicProcessMan::OnEnterInst(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerEnterInstReq) == nDatalen);
	if (sizeof(SLinkerEnterInstReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_ENTER_INST);

	SLinkerEnterInstReq* req = (SLinkerEnterInstReq*)pData;
	req->hton();

	LTMSEL enterTTS = GetMsel();
	PlayerActionMan::SEnterInstItem enterInstItem;
	BYTE instType = GetGameCfgFileMan()->GetInstType(req->instId);

	switch (instType)
	{
		// �����
	case ESGS_INST_FIND_CHEST:
	case ESGS_INST_GUARD_ICON:
	case ESGS_INST_ACTIVITY_INST:
	{
									int retCode = GetActitvityInstMan()->EnterActitvityInst(req->userId, req->roleId, req->instId);
									if (retCode != MLS_OK)
										return retCode;
									break;
	}
		// ��սģʽ
	case ESGS_INST_COMPETITIVEMODE:
	{
									  int retCode = GetCompetitiveModeMan()->EnterCompetitiveMode(req->userId, req->roleId);
									  if (retCode != MLS_OK)
										  return retCode;
									  break;
	}
		// 1v1ģʽ
	case ESGS_INST_ONE_VS_ONE:
	{
								 int retCode = GetOneVsOneMan()->EnterOneVsOneInst(req->userId, req->roleId);
								 if (retCode != MLS_OK)
									 return retCode;
								 break;
	}
	}

	/* zpy 2015.10.14ע��
	//�Ƿ��������
	BYTE instType = GetGameCfgFileMan()->GetInstType(req->instId);
	if(instType == ESGS_INST_PERSONAL_RAND)
	{
	int retCode = GetPersonalRandInstMan()->EnterRandInstId(req->instId);
	if(retCode != MLS_OK)
	{
	gwsSession->SendBasicResponse(pHead, retCode, MLS_ENTER_INST);
	}
	*/

	//zpy�޸� ��¼���븱����Ϣ
	SRoleInfos roleInfo;
	GetRolesInfoMan()->GetRoleInfo(req->userId, req->roleId, &roleInfo);
	enterInstItem.instId = req->instId;
	enterInstItem.job = roleInfo.occuId;
	enterInstItem.level = roleInfo.level;
	enterInstItem.result = 0;
	enterInstItem.strNick = roleInfo.nick;
	enterInstItem.tts = enterTTS;
	GetPlayerActionMan()->InputEnterInstLog(enterInstItem);

	/* zpy 2015.11.11ע�ͣ��߻��������
	//zpy ��ʱ�����
	switch (instType)
	{
	case ESGS_INST_GUARD_ICON:					//����ʥ��
	{
	int retCode = GetGuardIconMan()->EnterGuardIconManInst(req->userId, req->roleId);
	if (retCode != MLS_OK)
	{
	return gwsSession->SendBasicResponse(pHead, retCode, MLS_ENTER_INST);
	}
	break;
	}
	case ESGS_INST_WORLD_BOSS:					//����Boss
	{
	int retCode = GetWorldBossMan()->EnterWorldBossInst(req->userId, req->roleId);
	if (retCode != MLS_OK)
	{
	return gwsSession->SendBasicResponse(pHead, retCode, MLS_ENTER_INST);
	}
	break;
	}
	}
	*/

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SEnterInstRes* pEnterInstRes = (SEnterInstRes*)(buf + nLen);

	int dataLen = 0;
	int retCode = GetRolesInfoMan()->EnterInst(req->userId, req->roleId, req->instId, pEnterInstRes, dataLen, enterTTS, enterInstItem);

	//��¼���븱����Ϣ
	enterInstItem.result = retCode;
	GetPlayerActionMan()->InputEnterInstLog(enterInstItem);
	LogInfo(("�û�[%u]��ɫ[%u]���븱��[%d], ������[%d]!", req->userId, req->roleId, req->instId, retCode));
	if (retCode != MLS_OK)
	{
		/*  zpy 2015.10.14ע��
		//�뿪�������
		if(instType == ESGS_INST_PERSONAL_RAND)
		GetPersonalRandInstMan()->LeaveRandInstId(req->instId);*/

		return gwsSession->SendBasicResponse(pHead, retCode, MLS_ENTER_INST);
	}

	//ˢ������
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_VIT);

	nLen += sizeof(SEnterInstRes)-sizeof(char)+dataLen;
	pEnterInstRes->hton();
	gwsSession->SendResponse(pHead, retCode, MLS_ENTER_INST, buf, nLen);

	/* zpy 2015.11.11ע�ͣ��߻��������
	//���»��������
	if(instType == ESGS_INST_FIND_CHEST)
	GetInstFinHistoryMan()->UpdateAvtiveInstFinTimes(req->roleId, req->instId);
	*/

	return MLS_OK;
}

int MLogicProcessMan::OnFetchInstAreaMonsterDrops(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerFetchInstAreaDropsReq) == nDatalen);
	if (sizeof(SLinkerFetchInstAreaDropsReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_FETCH_INST_AREA_DROPS);

	SLinkerFetchInstAreaDropsReq* req = (SLinkerFetchInstAreaDropsReq*)pData;
	req->hton();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SFetchInstAreaDropsRes* pAreaDropsRes = (SFetchInstAreaDropsRes*)(buf + nLen);
	int retCode = GetRolesInfoMan()->FetchInstAreaMonsterDrops(req->userId, req->roleId, req->instId, req->areaId, pAreaDropsRes);
	LogInfo(("�û�[%u]��ɫ[%u]��ȡ����[%d]����[%d]�ֵ���, ������[%d]!", req->userId, req->roleId, req->instId, req->areaId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_FETCH_INST_AREA_DROPS);

	nLen += sizeof(SFetchInstAreaDropsRes)+(pAreaDropsRes->monsterDrops + pAreaDropsRes->creamDrops + pAreaDropsRes->bossDrops - 1) * sizeof(SMonsterDropPropItem);
	return gwsSession->SendResponse(pHead, retCode, MLS_FETCH_INST_AREA_DROPS, buf, nLen);
}

int MLogicProcessMan::OnPickupInstMonsterDrop(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerPickupInstMonsterDropReq) == nDatalen);
	if (sizeof(SLinkerPickupInstMonsterDropReq) != nDatalen)
		return MLS_OK;

	SLinkerPickupInstMonsterDropReq* req = (SLinkerPickupInstMonsterDropReq*)pData;
	req->hton();

	char propBuf[MAX_BUF_LEN] = { 0 };
	char goldBuf[GWS_COMMON_REPLY_LEN + sizeof(SBigNumber)] = { 0 };
	char tokenBuf[GWS_COMMON_REPLY_LEN + sizeof(UINT32)] = { 0 };
	char vitBuf[GWS_COMMON_REPLY_LEN + sizeof(UINT32)] = { 0 };
	char fragmentBuf[GWS_COMMON_REPLY_LEN + sizeof(SEquipFragmentAttr)] = { 0 };
	char chainSoulFragmentBuf[GWS_COMMON_REPLY_LEN + sizeof(SChainSoulFragmentAttr)] = { 0 };

	SPickupInstDropNotify notify;
	notify.pGold = (SGoldUpdateNotify*)(goldBuf + GWS_COMMON_REPLY_LEN);
	notify.pToken = (STokenUpdateNotify*)(tokenBuf + GWS_COMMON_REPLY_LEN);
	notify.pVit = (SVitUpdateNotify*)(vitBuf + GWS_COMMON_REPLY_LEN);
	notify.pBackpack = (SBackpackUpdateNotify*)(propBuf + GWS_COMMON_REPLY_LEN);
	notify.pFragment = (SFragmentUpdateNotify*)(fragmentBuf + GWS_COMMON_REPLY_LEN);
	notify.pChainSoulFragment = (SChainSoulFragmentNotify*)(chainSoulFragmentBuf + GWS_COMMON_REPLY_LEN);

	int retCode = GetRolesInfoMan()->PickupInstMonsterDrop(req->userId, req->roleId, req->instId, req->areaId, req->dropId, &notify);
	LogInfo(("�û�[%u]��ɫ[%u]ʰȡ����[%d]����[%d]������Ʒ[%d], ������[%d]!", req->userId, req->roleId, req->instId, req->areaId, req->dropId, retCode));
	if (retCode != MLS_OK)
		return retCode;

	BYTE linkerId = pHead->sourceId;

	//ˢ�½��
	if (BigNumberToU64(notify.pGold->gold) > 0)
	{
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_GOLD);
		gwsSession->SendResponse(linkerId, req->userId, MLS_OK, MLS_GOLD_UPDATE_NOTIFY, goldBuf, GWS_COMMON_REPLY_LEN + sizeof(SBigNumber));
	}

	//ˢ�´���
	if (notify.pToken->rpcoin > 0)
	{
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_CASH);
		notify.pToken->hton();
		gwsSession->SendResponse(linkerId, req->userId, MLS_OK, MLS_TOKEN_UPDATE_NOTIFY, tokenBuf, GWS_COMMON_REPLY_LEN + sizeof(UINT32));
	}

	//ˢ������
	if (notify.pVit->vit > 0)
	{
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_VIT);
		notify.pVit->hton();
		gwsSession->SendResponse(linkerId, req->userId, MLS_OK, MLS_VIT_UPDATE_NOTIFY, vitBuf, GWS_COMMON_REPLY_LEN + sizeof(UINT32));
	}

	//ˢ�±���
	if (notify.pBackpack->propNum > 0)
	{
		int nLen = sizeof(SBackpackUpdateNotify)+(notify.pBackpack->propNum - 1) * sizeof(SPropertyAttr);
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_BACKPACK);
		for (BYTE i = 0; i < notify.pBackpack->propNum; ++i)
			notify.pBackpack->props[i].hton();
		gwsSession->SendResponse(linkerId, req->userId, MLS_OK, MLS_BACKPACK_UPDATE_NOTIFY, propBuf, GWS_COMMON_REPLY_LEN + nLen);
	}

	//ˢ����Ƭ
	if (notify.pFragment->fragment.num > 0)
	{
		int nLen = sizeof(SFragmentUpdateNotify);
		notify.pFragment->hton();
		gwsSession->SendResponse(linkerId, req->userId, MLS_OK, MLS_FRAGMENT_UPDATE_NOTIFY, fragmentBuf, GWS_COMMON_REPLY_LEN + nLen);
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_FRAGMENT);
	}

	//ˢ��������Ƭ
	if (notify.pChainSoulFragment->fragment.num > 0)
	{
		int nLen = sizeof(SChainSoulFragmentNotify);
		notify.pChainSoulFragment->hton();
		gwsSession->SendResponse(linkerId, req->userId, MLS_OK, MLS_CHAIN_SOUL_FRAGMENT_UPDATE_NOTIFY, chainSoulFragmentBuf, GWS_COMMON_REPLY_LEN + nLen);
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_CHAINSOUL_FRAGMENT);
	}

	return MLS_OK;
}

int MLogicProcessMan::OnPickupInstBox(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerPickupInstBoxReq) == nDatalen);
	if (sizeof(SLinkerPickupInstBoxReq) != nDatalen)
		return MLS_OK;

	SLinkerPickupInstBoxReq* req = (SLinkerPickupInstBoxReq*)pData;
	req->hton();

	char propBuf[MAX_BUF_LEN] = { 0 };
	char goldBuf[GWS_COMMON_REPLY_LEN + sizeof(SBigNumber)] = { 0 };
	char tokenBuf[GWS_COMMON_REPLY_LEN + sizeof(UINT32)] = { 0 };
	char vitBuf[GWS_COMMON_REPLY_LEN + sizeof(UINT32)] = { 0 };
	char fragmentBuf[GWS_COMMON_REPLY_LEN + sizeof(SEquipFragmentAttr)] = { 0 };
	char chainSoulFragmentBuf[GWS_COMMON_REPLY_LEN + sizeof(SChainSoulFragmentAttr)] = { 0 };

	SPickupInstDropNotify notify;
	notify.pGold = (SGoldUpdateNotify*)(goldBuf + GWS_COMMON_REPLY_LEN);
	notify.pToken = (STokenUpdateNotify*)(tokenBuf + GWS_COMMON_REPLY_LEN);
	notify.pVit = (SVitUpdateNotify*)(vitBuf + GWS_COMMON_REPLY_LEN);
	notify.pBackpack = (SBackpackUpdateNotify*)(propBuf + GWS_COMMON_REPLY_LEN);
	notify.pFragment = (SFragmentUpdateNotify*)(fragmentBuf + GWS_COMMON_REPLY_LEN);
	notify.pChainSoulFragment = (SChainSoulFragmentNotify*)(chainSoulFragmentBuf + GWS_COMMON_REPLY_LEN);

	int retCode = GetRolesInfoMan()->PickupInstBoxDrop(req->userId, req->roleId, req->instId, req->boxId, req->id, &notify);
	LogInfo(("�û�[%u]��ɫ[%u]ʰȡ����[%d]����[%d]��Ʒ[%d], ������[%d]!", req->userId, req->roleId, req->instId, req->boxId, req->id, retCode));
	if (retCode != MLS_OK)
		return retCode;

	/* zpy �ɾ�ϵͳ���� */
	GetAchievementMan()->OnPickupInstBoxDrop(req->userId, req->roleId);
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_PICKUP_BOX_SUM);

	BYTE linkerId = pHead->sourceId;

	//ˢ�½��
	if (BigNumberToU64(notify.pGold->gold) > 0)
	{
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_GOLD);
		gwsSession->SendResponse(linkerId, req->userId, MLS_OK, MLS_GOLD_UPDATE_NOTIFY, goldBuf, GWS_COMMON_REPLY_LEN + sizeof(SBigNumber));
	}

	//ˢ�´���
	if (notify.pToken->rpcoin > 0)
	{
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_CASH);
		notify.pToken->hton();
		gwsSession->SendResponse(linkerId, req->userId, MLS_OK, MLS_TOKEN_UPDATE_NOTIFY, tokenBuf, GWS_COMMON_REPLY_LEN + sizeof(UINT32));
	}

	//ˢ������
	if (notify.pVit->vit > 0)
	{
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_VIT);
		notify.pVit->hton();
		gwsSession->SendResponse(linkerId, req->userId, MLS_OK, MLS_VIT_UPDATE_NOTIFY, vitBuf, GWS_COMMON_REPLY_LEN + sizeof(UINT32));
	}

	//ˢ�±���
	if (notify.pBackpack->propNum > 0)
	{
		int nLen = sizeof(SBackpackUpdateNotify)+(notify.pBackpack->propNum - 1) * sizeof(SPropertyAttr);
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_BACKPACK);
		for (BYTE i = 0; i < notify.pBackpack->propNum; ++i)
			notify.pBackpack->props[i].hton();
		gwsSession->SendResponse(linkerId, req->userId, MLS_OK, MLS_BACKPACK_UPDATE_NOTIFY, propBuf, GWS_COMMON_REPLY_LEN + nLen);
	}

	//ˢ����Ƭ
	if (notify.pFragment->fragment.num > 0)
	{
		int nLen = sizeof(SFragmentUpdateNotify);
		notify.pFragment->hton();
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_FRAGMENT);
		gwsSession->SendResponse(linkerId, req->userId, MLS_OK, MLS_FRAGMENT_UPDATE_NOTIFY, fragmentBuf, GWS_COMMON_REPLY_LEN + nLen);
	}

	//ˢ��������Ƭ
	if (notify.pChainSoulFragment->fragment.num > 0)
	{
		int nLen = sizeof(SChainSoulFragmentNotify);
		notify.pChainSoulFragment->hton();
		gwsSession->SendResponse(linkerId, req->userId, MLS_OK, MLS_CHAIN_SOUL_FRAGMENT_UPDATE_NOTIFY, chainSoulFragmentBuf, GWS_COMMON_REPLY_LEN + nLen);
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_CHAINSOUL_FRAGMENT);
	}

	return MLS_OK;
}

int MLogicProcessMan::OnPickupBrokenDrop(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerPickupBrokenDropReq) == nDatalen);
	if (sizeof(SLinkerPickupBrokenDropReq) != nDatalen)
		return MLS_OK;

	SLinkerPickupBrokenDropReq* req = (SLinkerPickupBrokenDropReq*)pData;
	req->hton();

	char propBuf[MAX_BUF_LEN] = { 0 };
	char goldBuf[GWS_COMMON_REPLY_LEN + sizeof(SBigNumber)] = { 0 };
	char tokenBuf[GWS_COMMON_REPLY_LEN + sizeof(UINT32)] = { 0 };
	char vitBuf[GWS_COMMON_REPLY_LEN + sizeof(UINT32)] = { 0 };
	char fragmentBuf[GWS_COMMON_REPLY_LEN + sizeof(SEquipFragmentAttr)] = { 0 };
	char chainSoulFragmentBuf[GWS_COMMON_REPLY_LEN + sizeof(SChainSoulFragmentAttr)] = { 0 };

	SPickupInstDropNotify notify;
	notify.pGold = (SGoldUpdateNotify*)(goldBuf + GWS_COMMON_REPLY_LEN);
	notify.pToken = (STokenUpdateNotify*)(tokenBuf + GWS_COMMON_REPLY_LEN);
	notify.pVit = (SVitUpdateNotify*)(vitBuf + GWS_COMMON_REPLY_LEN);
	notify.pBackpack = (SBackpackUpdateNotify*)(propBuf + GWS_COMMON_REPLY_LEN);
	notify.pFragment = (SFragmentUpdateNotify*)(fragmentBuf + GWS_COMMON_REPLY_LEN);
	notify.pChainSoulFragment = (SChainSoulFragmentNotify*)(chainSoulFragmentBuf + GWS_COMMON_REPLY_LEN);

	int retCode = GetRolesInfoMan()->PickupInstBrokenDrop(req->userId, req->roleId, req->instId, req->dropId, &notify);
	LogInfo(("�û�[%u]��ɫ[%u]ʰȡ����[%d]���������[%d], ������[%d]!", req->userId, req->roleId, req->instId, req->dropId, retCode));
	if (retCode != MLS_OK)
		return retCode;

	BYTE linkerId = pHead->sourceId;

	//ˢ�½��
	if (BigNumberToU64(notify.pGold->gold) > 0)
	{
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_GOLD);
		gwsSession->SendResponse(linkerId, req->userId, MLS_OK, MLS_GOLD_UPDATE_NOTIFY, goldBuf, GWS_COMMON_REPLY_LEN + sizeof(SBigNumber));
	}

	//ˢ�´���
	if (notify.pToken->rpcoin > 0)
	{
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_CASH);
		notify.pToken->hton();
		gwsSession->SendResponse(linkerId, req->userId, MLS_OK, MLS_TOKEN_UPDATE_NOTIFY, tokenBuf, GWS_COMMON_REPLY_LEN + sizeof(UINT32));
	}

	//ˢ������
	if (notify.pVit->vit > 0)
	{
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_VIT);
		notify.pVit->hton();
		gwsSession->SendResponse(linkerId, req->userId, MLS_OK, MLS_VIT_UPDATE_NOTIFY, vitBuf, GWS_COMMON_REPLY_LEN + sizeof(UINT32));
	}

	//ˢ�±���
	if (notify.pBackpack->propNum > 0)
	{
		int nLen = sizeof(SBackpackUpdateNotify)+(notify.pBackpack->propNum - 1) * sizeof(SPropertyAttr);
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_BACKPACK);
		for (BYTE i = 0; i < notify.pBackpack->propNum; ++i)
			notify.pBackpack->props[i].hton();
		gwsSession->SendResponse(linkerId, req->userId, MLS_OK, MLS_BACKPACK_UPDATE_NOTIFY, propBuf, GWS_COMMON_REPLY_LEN + nLen);
	}

	//ˢ����Ƭ
	if (notify.pFragment->fragment.num > 0)
	{
		int nLen = sizeof(SFragmentUpdateNotify);
		notify.pFragment->hton();
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_FRAGMENT);
		gwsSession->SendResponse(linkerId, req->userId, MLS_OK, MLS_FRAGMENT_UPDATE_NOTIFY, fragmentBuf, GWS_COMMON_REPLY_LEN + nLen);
	}

	//ˢ��������Ƭ
	if (notify.pChainSoulFragment->fragment.num > 0)
	{
		int nLen = sizeof(SChainSoulFragmentNotify);
		notify.pChainSoulFragment->hton();
		gwsSession->SendResponse(linkerId, req->userId, MLS_OK, MLS_CHAIN_SOUL_FRAGMENT_UPDATE_NOTIFY, chainSoulFragmentBuf, GWS_COMMON_REPLY_LEN + nLen);
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_CHAINSOUL_FRAGMENT);
	}

	return MLS_OK;
}

int MLogicProcessMan::OnKillMonsterExp(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerKillMonsterExpReq) == nDatalen);
	if (sizeof(SLinkerKillMonsterExpReq) != nDatalen)
		return MLS_OK;

	SLinkerKillMonsterExpReq* req = (SLinkerKillMonsterExpReq*)pData;
	req->ntoh();

	BOOL isUpgrade = false;
	int retCode = GetRolesInfoMan()->KillMonsterExp(req->userId, req->roleId, req->instId, req->areaId, req->monsterId, req->monsterLv, isUpgrade);
	LogInfo(("�û�[%u]��ɫ[%u]����[%d]����[%d]ɱ��[%d]�ȼ�[%d], ������[%d]!", req->userId, req->roleId, req->instId, req->areaId, req->monsterId, req->monsterLv, retCode));
	if (retCode != MLS_OK)
		return retCode;

	/* zpy �ɾ�ϵͳ���� */
	GetAchievementMan()->OnKillMonsterRecord(req->userId, req->roleId, req->monsterId);
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_KILL_MONSTER);

	if (isUpgrade)
	{
		/* zpy �ɾ�ϵͳ���� */
		GetAchievementMan()->OnRoleUpgrade(req->userId, req->roleId);

		//ˢ�½�ɫ����
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_UPGRADE);

		//����֪ͨ
		char buf[MAX_BUF_LEN] = { 0 };
		int nLen = GWS_COMMON_REPLY_LEN;
		SRoleUpgradeNotify* pUpgradeNotify = (SRoleUpgradeNotify*)(buf + nLen);
		retCode = GetRolesInfoMan()->GetRoleUpgradeNotify(req->userId, req->roleId, pUpgradeNotify);
		if (retCode == MLS_OK)
		{
			nLen += sizeof(SRoleUpgradeNotify)+(pUpgradeNotify->skillNum - 1) * sizeof(SSkillMoveInfo);
			pUpgradeNotify->hton();
			gwsSession->SendResponse(pHead, retCode, MLS_ROLE_UPGRADE_NOTIFY, buf, nLen);
		}
	}
	else
	{
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_EXP);
	}

	return MLS_OK;
}

// zpy ��ȡ��������
void GetInstReward(SLinkerFinishInstReq *req, BYTE instType, std::vector<SPropertyItem> &rewardVec)
{
	rewardVec.clear();

	if ((instType != ESGS_INST_WORLD_BOSS && instType != ESGS_INST_HIGH_LADDER) &&
		(req->finCode == 0 || instType == ESGS_INST_ONE_VS_ONE))
	{
		std::vector<GameTypeCfg::SRewardCondition> *conditionVec_ptr = NULL;
		if (GetGameCfgFileMan()->GetConditionAndReward(instType, conditionVec_ptr))
		{
			std::vector<GameTypeCfg::SRewardCondition> &conditionVec = *conditionVec_ptr;
			for (std::vector<GameTypeCfg::SRewardCondition>::const_iterator itr = conditionVec.begin(); itr != conditionVec.end(); ++itr)
			{
				bool have = false;
				switch (itr->type)
				{
				case ESGS_CONDITION_SURPLUS_TIME:						//ʣ��ʱ��
				{
																			have = req->surplus_time >= itr->num;
																			break;
				}
				case ESGS_CONDITION_RESIST_NUMBER:						//�ֵ�����
				{
																			have = req->resist_number >= itr->num;
																			break;
				}
				case ESGS_CONDITION_KILL_NUMBER:						//��ɱ����
				{
																			have = req->kill_number >= itr->num;
																			break;
				}
				case ESGS_CONDITION_CHEST:								//����
				{
																			if (itr->num == ESGS_CONDITION_CHEST_SENIOR)
																			{
																				for (int i = 0; i < req->chest_num; ++i)
																				{
																					if (req->data[i].type == ESGS_CONDITION_CHEST_SENIOR)
																					{
																						have = true;
																						break;
																					}
																				}
																			}
																			else if (itr->num == ESGS_CONDITION_CHEST_LOWER)
																			{
																				for (int i = 0; i < req->chest_num; ++i)
																				{
																					if (req->data[i].type == ESGS_CONDITION_CHEST_LOWER)
																					{
																						have = true;
																						break;
																					}
																				}
																			}
																			break;
				}
				case ESGS_CONDITION_LEVEL_GAP:							//�ȼ���
				{
																			have = req->level_gap >= itr->num;
																			break;
				}
				case ESGS_CONDITION_CP_GAP:								//ս����
				{
																			have = req->cp_gap >= itr->num;
																			break;
				}
				case ESGS_CONDITION_HURT_EVERY:							//ÿ����˺�
				{
																			have = req->hurt_sum >= itr->num;
																			break;
				}
				case ESGS_CONDITION_ONE_ON_ONE:							//1v1
				{
																			have = true;
																			break;
				}
				case ESGS_CONDITION_RANKING:							//����
				case ESGS_CONDITION_LAST_STRAW:							//���һ��
				case ESGS_CONDITION_HURT_EXTREMELY:						//����˺���ֱ�
				case ESGS_CONDITION_HIGH_LADDER_RANK:					//��������
				case ESGS_CONDITION_MAN_MACHINE_TO_WAR:					//�˻���ս
				case ESGS_CONDITION_HIGH_LADDER_CONTINUOUS_VICTORY_COUNT://������ʤ����
				{
																			 break;
				}
				}

				if (have && !itr->reward.empty())
				{
					std::vector<GameTypeCfg::SRewardInfo> rewards;
					GetGameCfgFileMan()->UnlockRewardNesting(itr->reward, &rewards);
					rewardVec.reserve(rewardVec.size() + rewards.size());
					for (size_t i = 0; i < rewards.size(); ++i)
					{
						if (req->finCode == 0 || (itr->type == ESGS_CONDITION_ONE_ON_ONE && rewards[i].type == ESGS_PROP_GOLD))
						{
							SPropertyItem prop;
							memset(&prop, 0, sizeof(SPropertyItem));
							prop.propId = rewards[i].id;
							prop.type = rewards[i].type;
							prop.num = rewards[i].num;
							if (itr->type == ESGS_CONDITION_ONE_ON_ONE && prop.type == ESGS_PROP_GOLD)
							{
								BYTE ratio_value = GetGameCfgFileMan()->GetOneOnOneRewardRatio(req->finCode);
								// ����ֵ*(1+Min(0.1*Max(�Է��ȼ�-�����ȼ�,0),0.5))*ʤ�������1V1ƥ�����5������
								//	prop.num = (BYTE)(prop.num * (1 + std::min<float>(0.1f * std::max<INT32>(req->level_gap, 0), 0.5f)) * (UINT8)ratio_value);
								prop.num = prop.num * (1 + min(0.1f * max(req->level_gap, 0), 0.5f)) * (ratio_value / 100.0f) + (req->hurt_sum / 10);
							}
							rewardVec.push_back(prop);
						}
					}
				}
			}
		}
	}
}



int MLogicProcessMan::OnFinishInst(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerFinishInstReq)-sizeof(SChestItem) <= nDatalen);

	if (sizeof(SLinkerFinishInstReq)-sizeof(SChestItem) > nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_FINISH_INST);

	SLinkerFinishInstReq* req = (SLinkerFinishInstReq*)pData;
	req->ntoh();

	//zpy ��������˼�¼
	GetRolesInfoMan()->ClearRobot(req->userId, req->roleId);

	UINT16 finCode = req->finCode;
	PlayerActionMan::SFinishInstItem finInstItem;

	/* zpy 2015.10.14ע��
	//�뿪�������
	BYTE instType = GetGameCfgFileMan()->GetInstType(req->instId);
	if(instType == ESGS_INST_PERSONAL_RAND)
	GetPersonalRandInstMan()->LeaveRandInstId(req->instId); */

	BYTE instType = GetGameCfgFileMan()->GetInstType(req->instId);

	// ��������ݸ���
	if (instType == ESGS_INST_HIGH_LADDER)
	{
		int retCode = GetLadderRankMan()->FinishChallenge(req->userId, req->roleId, req->finCode);
		if (retCode != MLS_OK)
			return gwsSession->SendBasicResponse(pHead, retCode, MLS_FINISH_INST);
	}

	// ����Ǿ���ģʽ
	if (instType == ESGS_INST_COMPETITIVEMODE && finCode == 0)
	{
		int retCode = GetCompetitiveModeMan()->FinishCompetitiveMode(req->userId, req->roleId);
		if (retCode != MLS_OK)
			return gwsSession->SendBasicResponse(pHead, retCode, MLS_FINISH_INST);
	}

	// zpy ��ȡ��������
	std::vector<SPropertyItem> rewardVec;
	GetInstReward(req, instType, rewardVec);

	SReceiveRewardRefresh refresh;
	memset(&refresh, 0, sizeof(SReceiveRewardRefresh));

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SFinishInstRes* pFinishInstRes = (SFinishInstRes*)(buf + nLen);
	int retCode = GetRolesInfoMan()->FinishInst(req->userId, req->roleId, req->instId, req->score, req->star, finInstItem.enter_tts, rewardVec, pFinishInstRes, &refresh);
	LogInfo(("�û�[%u]��ɫ[%u]��ɸ���[%d], ������[%d]!", req->userId, req->roleId, req->instId, retCode));
	if (retCode != MLS_OK)
		gwsSession->SendBasicResponse(pHead, retCode, MLS_FINISH_INST);

	// ˢ�����ݿ�
	ReceiveRewardRefresh(req->userId, req->roleId, refresh);

	//��¼���������Ϣ
	finInstItem.fin_tts = GetMsel();
	finInstItem.result = finCode;
	finInstItem.roleId = req->roleId;
	finInstItem.instId = req->instId;
	GetPlayerActionMan()->InputFinishInstLog(finInstItem);

	if (instType == ESGS_INST_MAIN && finCode == 0)
	{
		ASSERT(printf("role=%d,instid=%d,type=%d\n", req->roleId, req->instId, instType));
		GetInstFinHistoryMan()->InputInstFinRecord(req->roleId, req->instId, req->star);

		/* zpy �ɾ�ϵͳ���� */
		GetAchievementMan()->OnFinishInst(req->userId, req->roleId, req->continuous, req->speed_evaluation, req->hurt_evaluation, req->star >= 3);
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_COMBAT_PERFORMANCE);
	}

	nLen += sizeof(SFinishInstRes);
	pFinishInstRes->hton();
	return gwsSession->SendResponse(pHead, retCode, MLS_FINISH_INST, buf, nLen);
}

int MLogicProcessMan::OnQueryFinishedInst(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerQueryFinishedInstReq) == nDatalen);
	if (sizeof(SLinkerQueryFinishedInstReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_FINISHED_INST);

	SLinkerQueryFinishedInstReq* req = (SLinkerQueryFinishedInstReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQueryFinishedInstRes* pFinInstRes = (SQueryFinishedInstRes*)(buf + nLen);
	pFinInstRes->total = 0;
	pFinInstRes->curNum = 0;

	BYTE maxNum = (MAX_RES_USER_BYTES - member_offset(SQueryFinishedInstRes, total, instInfos)) / sizeof(SFinishedInstInfo);
	int retCode = GetInstFinHistoryMan()->QueryFinishedInst(req->roleId, req->from, req->num, pFinInstRes, maxNum);
	LogInfo(("��ɫ[%u]��ѯ����ɸ�����Ϣ, ������[%d]!", req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_QUERY_FINISHED_INST);

	nLen += sizeof(SQueryFinishedInstRes)+(pFinInstRes->curNum - 1) * sizeof(SFinishedInstInfo);
	pFinInstRes->hton();
	return gwsSession->SendResponse(pHead, retCode, MLS_QUERY_FINISHED_INST, buf, nLen);
}

int MLogicProcessMan::OnSellProperty(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerSellPropertyReq) == nDatalen);
	if (sizeof(SLinkerSellPropertyReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_SELL_PROPERTY);

	SLinkerSellPropertyReq* req = (SLinkerSellPropertyReq*)pData;
	req->ntoh();

	if (req->num <= 0)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_SELL_PROPERTY);

	//������Ʒ��Ϣ
	SPropSellInfo sellInfo;
	memset(&sellInfo, 0, sizeof(SPropSellInfo));

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SSellPropertyRes* pSellRes = (SSellPropertyRes*)(buf + nLen);
	int retCode = GetRolesInfoMan()->SellProperty(req->userId, req->roleId, req->id, req->num, pSellRes, sellInfo.prop);
	LogInfo(("�û�[%u]��ɫ[%u]������Ʒ, ������[%d]", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_SELL_PROPERTY);

	//ˢ�±���
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_BACKPACK);

	//ˢ�½��
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_GOLD);

	//��¼���ۼ�¼(��ɫ����ɫ����װ���ű�����ۼ�¼)
	if (sellInfo.prop.type == ESGS_PROP_EQUIP)
	{
		if (sellInfo.prop.grade >= ESGS_EQUIP_QUALITY_PRUPLE)
		{
			GetLocalStrTime(sellInfo.sellDate);
			strcpy(pSellRes->szSellDate, sellInfo.sellDate);
			GetPropSellMan()->InputPropSellRecord(req->roleId, sellInfo);
			pSellRes->sellId = sellInfo.prop.id;
		}
	}
	else	//����
	{
		GetLocalStrTime(sellInfo.sellDate);
		strcpy(pSellRes->szSellDate, sellInfo.sellDate);
		GetPropSellMan()->InputPropSellRecord(req->roleId, sellInfo);
		pSellRes->sellId = sellInfo.prop.id;
	}

	//��������
	for (BYTE i = 0; i < pSellRes->num; ++i)
		pSellRes->data[i].ntoh();
	nLen += sizeof(SSellPropertyRes)+(pSellRes->num - 1) * sizeof(SPropertyAttr);
	pSellRes->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_SELL_PROPERTY, buf, nLen);
}

int MLogicProcessMan::OnDropProperty(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerDropPropertyReq) == nDatalen);
	if (sizeof(SLinkerDropPropertyReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_DROP_PROPERTY);

	SLinkerDropPropertyReq* req = (SLinkerDropPropertyReq*)pData;
	req->ntoh();

	int retCode = MLS_OK;
	if (req->from == 0)
	{
		//������������
		retCode = GetRolesInfoMan()->DropBackpackProperty(req->userId, req->roleId, req->id, req->num);

		//�������ݿ�
		if (retCode == MLS_OK)
			GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_BACKPACK);
	}
	else
	{
		//�����ֿ����
	}

	LogInfo(("�û�[%u]��ɫ[%u]��������, ������[%d]!", req->userId, req->roleId, retCode));
	return gwsSession->SendBasicResponse(pHead, retCode, MLS_DROP_PROPERTY);
}

int MLogicProcessMan::OnDropTask(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerDropTaskReq) == nDatalen);
	if (sizeof(SLinkerDropTaskReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_DROP_TASK);

	SLinkerDropTaskReq* req = (SLinkerDropTaskReq*)pData;
	req->ntoh();
	int retCode = GetRolesInfoMan()->DropTask(req->userId, req->roleId, req->taskId);
	LogInfo(("�û�[%u]��ɫ[%u]��������[%d], ������[%d]!", req->userId, req->roleId, req->taskId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_DROP_TASK);

	//ˢ�����ݿ�
	if (retCode == MLS_OK)
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_TASK);

	//���ͻظ�
	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SDropTaskRes* pDropRes = (SDropTaskRes*)(buf + nLen);
	pDropRes->taskId = req->taskId;
	pDropRes->hton();
	nLen += sizeof(SDropTaskRes);
	return gwsSession->SendResponse(pHead, retCode, MLS_DROP_TASK, buf, nLen);
}

int MLogicProcessMan::OnCommitTaskFinish(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerFinishTaskReq) == nDatalen);
	if (sizeof(SLinkerFinishTaskReq) != nDatalen)
		return MLS_OK;

	SLinkerFinishTaskReq* req = (SLinkerFinishTaskReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;

	SFinishTaskFailNotify* pFailNotify = (SFinishTaskFailNotify*)(buf + nLen);
	int retCode = GetRolesInfoMan()->CommitTaskFinish(req->userId, req->roleId, req->taskId, pFailNotify);
	LogInfo(("�û�[%u]��ɫ[%u]�ύ�������[%d], ������[%d]!", req->userId, req->roleId, req->taskId, retCode));

	//ˢ�½�ɫ�ѽ�����
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_TASK);

	//�������ʧ��֪ͨ
	if (retCode != MLS_OK)
	{
		nLen += sizeof(SFinishTaskFailNotify);
		pFailNotify->hton();
		gwsSession->SendResponse(pHead, MLS_OK, MLS_FINISH_TASK_FAIL_NOTIFY, buf, nLen);
	}

	return retCode;
}

int MLogicProcessMan::OnQueryFinTaskHistory(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerQueryFinishedTaskReq) == nDatalen);
	if (sizeof(SLinkerQueryFinishedTaskReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_FIN_TASK_HISTORY);

	SLinkerQueryFinishedTaskReq* req = (SLinkerQueryFinishedTaskReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQueryFinishedTaskRes* pFinTasks = (SQueryFinishedTaskRes*)(buf + nLen);
	int retCode = GetTaskFinHistoryMan()->QueryRoleFinTaskHistory(req->roleId, req->from, req->num, pFinTasks);
	LogInfo(("�û�[%u]��ѯ��ɫ[%u]���������ʷ��¼, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_QUERY_FIN_TASK_HISTORY);

	nLen += sizeof(SQueryFinishedTaskRes)+(pFinTasks->num - 1) * sizeof(UINT16);
	pFinTasks->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_QUERY_FIN_TASK_HISTORY, buf, nLen);
}

int MLogicProcessMan::OnGetDailyTask(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerGetDailyTaskReq) == nDatalen);
	if (sizeof(SLinkerGetDailyTaskReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_GET_DAILY_TASK);

	SLinkerGetDailyTaskReq* req = (SLinkerGetDailyTaskReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SGetDailyTaskRes* pDailyTasks = (SGetDailyTaskRes*)(buf + nLen);
	int retCode = GetRolesInfoMan()->GetDailyTasks(req->userId, req->roleId, pDailyTasks);
	LogInfo(("�û�[%u]��ѯ��ɫ[%u]�ճ�����, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_GET_DAILY_TASK);

	nLen += sizeof(SGetDailyTaskRes)+(pDailyTasks->num - 1) * sizeof(UINT16);
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_GET_DAILY_TASK, buf, nLen);
}

int MLogicProcessMan::OnUpdateTaskTarget(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerUpdateTaskTargetReq) == nDatalen);
	if (sizeof(SLinkerUpdateTaskTargetReq) != nDatalen)
		return MLS_OK;

	SLinkerUpdateTaskTargetReq* req = (SLinkerUpdateTaskTargetReq*)pData;
	req->ntoh();

	int retCode = GetRolesInfoMan()->UpdateTaskTarget(req->userId, req->roleId, req->taskId, req->target);
	LogInfo(("�û�[%u]��ɫ[%u]��������[%d]Ŀ��!", req->userId, req->roleId, req->taskId));
	if (retCode == MLS_OK)
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_TASK);

	return MLS_OK;
}

int MLogicProcessMan::OnReceiveTaskReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerReceiveTaskRewardReq) == nDatalen);
	if (sizeof(SLinkerReceiveTaskRewardReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_RECEIVE_TASK_REWARD);

	SLinkerReceiveTaskRewardReq* req = (SLinkerReceiveTaskRewardReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SReceiveTaskRewardRes* pTaskRewardRes = (SReceiveTaskRewardRes*)(buf + nLen);
	int retCode = GetRolesInfoMan()->ReceiveTaskReward(req->userId, req->roleId, req->taskId, pTaskRewardRes);
	LogInfo(("�û�[%u]��ɫ[%u]��ȡ����[%d]����, ������[%d]!", req->userId, req->roleId, req->taskId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_RECEIVE_TASK_REWARD);

	//ˢ������
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_FIN_TASK);

	//���ͻظ�
	BYTE linkerId = pHead->sourceId;
	nLen += sizeof(SReceiveTaskRewardRes)+(pTaskRewardRes->modifyNum - 1) * sizeof(SPropertyAttr);
	pTaskRewardRes->hton();
	gwsSession->SendResponse(pHead, MLS_OK, MLS_RECEIVE_TASK_REWARD, buf, nLen);

	//�Ƿ�����
	if (pTaskRewardRes->isUpgrade)
	{
		nLen = GWS_COMMON_REPLY_LEN;
		SRoleUpgradeNotify* pUpgradeNotify = (SRoleUpgradeNotify*)(buf + nLen);
		retCode = GetRolesInfoMan()->GetRoleUpgradeNotify(req->userId, req->roleId, pUpgradeNotify);
		if (retCode == MLS_OK)
		{
			nLen += sizeof(SRoleUpgradeNotify)+(pUpgradeNotify->skillNum - 1) * sizeof(SSkillMoveInfo);
			pUpgradeNotify->hton();
			gwsSession->SendResponse(linkerId, req->userId, MLS_OK, MLS_ROLE_UPGRADE_NOTIFY, buf, nLen);
		}

		/* zpy �ɾ�ϵͳ���� */
		GetAchievementMan()->OnRoleUpgrade(req->userId, req->roleId);

		//ˢ�½�ɫ��������
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_UPGRADE);
	}

	//����������ɼ�¼
	TaskCfg::STaskAttrCfg* pTaskInfo = NULL;
	retCode = GetGameCfgFileMan()->GetTaskInfo(req->taskId, pTaskInfo);
	if (retCode != MLS_OK)
		return MLS_OK;

	BYTE sysTaskType = pTaskInfo->type;
	if (sysTaskType == ESGS_SYS_MAIN_TASK || sysTaskType == ESGS_SYS_BRANCH_TASK)
		GetTaskFinHistoryMan()->InputRoleFinTask(req->roleId, req->taskId, sysTaskType, pTaskInfo->chapterId);
	else if (sysTaskType == ESGS_SYS_DAILY_TASK)
		GetDailyTaskMan()->FinishedDailyTask(req->roleId, req->taskId);

	return retCode;
}

int MLogicProcessMan::OnBuyStorageSpace(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerBuyStorageSpaceReq) == nDatalen);
	if (sizeof(SLinkerBuyStorageSpaceReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_BUY_STORAGE_SPACE);

	SLinkerBuyStorageSpaceReq* req = (SLinkerBuyStorageSpaceReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SBuyStorageSpaceRes* pBuyRes = (SBuyStorageSpaceRes*)(buf + nLen);
	pBuyRes->buyType = req->buyType;
	int retCode = GetRolesInfoMan()->BuyStorageSpace(req->userId, req->roleId, req->buyType, req->num, pBuyRes->spend, pBuyRes->newNum);
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_BUY_STORAGE_SPACE);

	LogInfo(("�û�[%u]��ɫ[%u]����洢�ռ�!", req->userId, req->roleId));

	//�������ݿ�
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_STORAGE);

	nLen += sizeof(SBuyStorageSpaceRes);
	pBuyRes->hton();
	return gwsSession->SendResponse(pHead, retCode, MLS_BUY_STORAGE_SPACE, buf, nLen);
}

int MLogicProcessMan::OnAcceptTask(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerAcceptTaskReq) == nDatalen);
	if (sizeof(SLinkerAcceptTaskReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_ACCEPT_TASK);

	SLinkerAcceptTaskReq* req = (SLinkerAcceptTaskReq*)pData;
	req->ntoh();
	int retCode = GetRolesInfoMan()->AcceptTask(req->userId, req->roleId, req->taskId);
	LogInfo(("�û�[%u]��ɫ[%u]��������[%d], ������[%d]!", req->userId, req->roleId, req->taskId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_ACCEPT_TASK);

	//ˢ�����ݿ�
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_TASK);

	//���ͻظ�
	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SAcceptTaskRes* pAcceptRes = (SAcceptTaskRes*)(buf + nLen);
	pAcceptRes->taskId = req->taskId;
	nLen += sizeof(SAcceptTaskRes);
	pAcceptRes->hton();
	return gwsSession->SendResponse(pHead, retCode, MLS_ACCEPT_TASK, buf, nLen);
}

int MLogicProcessMan::OnGemCompose(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(nDatalen >= sizeof(SLinkerGemComposeReq));
	if (nDatalen < sizeof(SLinkerGemComposeReq))
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_GEM_COMPOSE);

	SLinkerGemComposeReq* req = (SLinkerGemComposeReq*)pData;
	req->ntoh();

	if (req->num <= 0)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_GEM_COMPOSE);

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SGemComposeRes* pComposeRes = (SGemComposeRes*)(buf + nLen);
	int retCode = GetRolesInfoMan()->GemCompose(req->userId, req->roleId, req->num, req->material, pComposeRes);
	LogInfo(("�û�[%u]��ɫ[%u]�ϳɱ�ʯ, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_GEM_COMPOSE);

	//ˢ�±���
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_BACKPACK);

	//ˢ�½��
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_GOLD);

	nLen += sizeof(SGemComposeRes);
	pComposeRes->hton();
	return gwsSession->SendResponse(pHead, retCode, MLS_GEM_COMPOSE, buf, nLen);
}

int MLogicProcessMan::OnQueryEmailList(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerQueryEmailListReq) == nDatalen);
	if (sizeof(SLinkerQueryEmailListReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_EMAIL_LIST);

	SLinkerQueryEmailListReq* req = (SLinkerQueryEmailListReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQueryEmailListRes* pEmailListRes = (SQueryEmailListRes*)(buf + nLen);
	int retCode = GetEmailCleanMan()->QueryEmailList(req->roleId, req->from, req->num, pEmailListRes);
	LogInfo(("�û�[%u]��ɫ[%u]��ѯ�ʼ��б��[%d]��ʼ[%d]��, ������[%d]!", req->userId, req->roleId, req->from, req->num, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_QUERY_EMAIL_LIST);

	nLen += sizeof(SQueryEmailListRes)+(pEmailListRes->retNum - 1) * sizeof(SEmailListGenInfo);
	return gwsSession->SendResponse(pHead, retCode, MLS_QUERY_EMAIL_LIST, buf, nLen);
}

int MLogicProcessMan::OnDelEmails(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(nDatalen >= sizeof(SLinkerDelEmailReq));
	if (nDatalen < sizeof(SLinkerDelEmailReq))
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_DEL_EMAIL);

	SLinkerDelEmailReq* req = (SLinkerDelEmailReq*)pData;
	req->ntoh();

	int retCode = GetEmailCleanMan()->DelEmails(req->roleId, req->emailIds, req->num);
	LogInfo(("�û�[%u]��ɫ[%u]ɾ���ʼ�, ������[%d]!", req->userId, req->roleId, retCode));
	return gwsSession->SendBasicResponse(pHead, retCode, MLS_DEL_EMAIL);
}

int MLogicProcessMan::OnReceiveEmailAttachments(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerReceiveEmailAttachmentsReq) == nDatalen);
	if (sizeof(SLinkerReceiveEmailAttachmentsReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_RECEIVE_EMAIL_ATTACHMENT);

	SLinkerReceiveEmailAttachmentsReq* req = (SLinkerReceiveEmailAttachmentsReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SReceiveEmailAttachmentsRes* pAttachmentsRes = (SReceiveEmailAttachmentsRes*)(buf + nLen);

	BYTE retNum = 0;
	BYTE attachmentsBuf[MAX_BUF_LEN] = { 0 };
	BYTE maxNum = min(GetGameCfgFileMan()->GetMaxEmailAttachments(), MAX_BUF_LEN / sizeof(SPropertyAttr));
	SPropertyAttr* pEmailAttachments = (SPropertyAttr*)attachmentsBuf;

	BYTE emailType = 0;
	EmailCleanMan::SEmailAttachmentsProp hasPropType;
	int retCode = GetEmailCleanMan()->ReceiveEmailAttachments(req->emailId, emailType, pAttachmentsRes->gold,
		pAttachmentsRes->rpcoin, pAttachmentsRes->exp,
		pAttachmentsRes->vit, hasPropType,
		pEmailAttachments, maxNum, retNum);


	LogInfo(("�û�[%u]��ɫ[%u]��ȡ�ʼ�[%u]����, ������[%d]!", req->userId, req->roleId, req->emailId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_RECEIVE_EMAIL_ATTACHMENT);

	BOOL needUpGold = BigNumberToU64(pAttachmentsRes->gold) > 0 ? true : false;
	BOOL needUpRPCoin = pAttachmentsRes->rpcoin > 0 ? true : false;
	BOOL needUpExp = BigNumberToU64(pAttachmentsRes->exp) > 0 ? true : false;
	BOOL needUpVit = pAttachmentsRes->vit > 0 ? true : false;

	BOOL isUpgrade = false;
	retCode = GetRolesInfoMan()->ReceiveEmailAttachments(req->userId, req->roleId, emailType, pEmailAttachments, retNum, pAttachmentsRes, isUpgrade);
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_RECEIVE_EMAIL_ATTACHMENT);

	//���ø���Ϊ����ȡ״̬
	GetEmailCleanMan()->SetEmailAttachmentsStatus(req->emailId, true);

	//ˢ�±���
	if (hasPropType.hasPackProp)
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_BACKPACK);

	//ˢ����Ƭ
	if (hasPropType.hasFragment)
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_FRAGMENT);

	//ˢ��������Ƭ
	if (hasPropType.hasChainSoulFragment)
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_CHAINSOUL_FRAGMENT);

	//ˢ��ʱװ
	if (hasPropType.hasDress)
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_DRESS);

	//ˢ��ħ����
	if (hasPropType.hasDress)
		GetPermanenDataMan()->InputUpdatePermanenDataInfo(req->userId, req->roleId, PERMANENT_MAGIC);

	//ˢ�´���
	if (needUpRPCoin)
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_CASH);

	//ˢ�½��
	if (needUpGold)
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_GOLD);

	//ˢ������
	if (needUpVit)
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_VIT);

	// 	{
	// 		SRoleInfos roleInfo;
	// 		int retCode2 = GetRolesInfoMan()->GetRoleInfo(req->userId, req->roleId, &roleInfo);
	// 		if(retCode2!=MLS_OK)
	// 		{		
	// 			return -1;
	// 		}
	// 		pAttachmentsRes->exp = roleInfo.exp;
	// 		pAttachmentsRes->vit += roleInfo.vit;
	// 	}

	BYTE linkerId = pHead->sourceId;
	pAttachmentsRes->emailId = req->emailId;
	pAttachmentsRes->hton();
	nLen += sizeof(SReceiveEmailAttachmentsRes)+(pAttachmentsRes->propsNum - 1) * sizeof(SPropertyAttr);
	gwsSession->SendResponse(pHead, retCode, MLS_RECEIVE_EMAIL_ATTACHMENT, buf, nLen);

	//�Ƿ�����
	if (isUpgrade)
	{
		/* zpy �ɾ�ϵͳ���� */
		GetAchievementMan()->OnRoleUpgrade(req->userId, req->roleId);

		//ˢ�½�ɫ����
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_UPGRADE);

		//����֪ͨ
		char buf[MAX_BUF_LEN] = { 0 };
		int nLen = GWS_COMMON_REPLY_LEN;
		SRoleUpgradeNotify* pUpgradeNotify = (SRoleUpgradeNotify*)(buf + nLen);
		retCode = GetRolesInfoMan()->GetRoleUpgradeNotify(req->userId, req->roleId, pUpgradeNotify);
		if (retCode == MLS_OK)
		{
			nLen += sizeof(SRoleUpgradeNotify)+(pUpgradeNotify->skillNum - 1) * sizeof(SSkillMoveInfo);
			pUpgradeNotify->hton();
			gwsSession->SendResponse(linkerId, req->userId, MLS_OK, MLS_ROLE_UPGRADE_NOTIFY, buf, nLen);
		}
	}
	else if (needUpExp)
	{
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_EXP);
	}

	return MLS_OK;
}

int MLogicProcessMan::OnQueryEmailContent(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerQueryEmailContentReq) == nDatalen);
	if (sizeof(SLinkerQueryEmailContentReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_EMAIL_CONTENT);

	SLinkerQueryEmailContentReq* req = (SLinkerQueryEmailContentReq*)pData;
	req->hton();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQueryEmailContentRes* pContentRes = (SQueryEmailContentRes*)(buf + nLen);
	int nBytes = 0;
	int retCode = GetEmailCleanMan()->ReadEmailContent(req->emailId, pContentRes, nBytes);
	LogInfo(("�û�[%u]��ɫ[%u]��ѯ�ʼ�[%u]����, ������[%d]!", req->userId, req->roleId, req->emailId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_QUERY_EMAIL_CONTENT);

	//���ʼ�״̬��Ϊ�Ѷ�
	GetEmailCleanMan()->SetEmailReadStatus(req->emailId, true);

	pContentRes->emailId = req->emailId;
	pContentRes->hton();
	nLen += nBytes;
	return gwsSession->SendResponse(pHead, retCode, MLS_QUERY_EMAIL_CONTENT, buf, nLen);
}

int MLogicProcessMan::OnLoadEquipment(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerLoadEquipmentReq) == nDatalen);
	if (sizeof(SLinkerLoadEquipmentReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_LOAD_EQUIPMENT);

	SLinkerLoadEquipmentReq* req = (SLinkerLoadEquipmentReq*)pData;
	req->ntoh();

	//����λ�ò���
	if (req->pos == 0)
		return gwsSession->SendBasicResponse(pHead, MLS_WEAR_POS_NOT_MATCH, MLS_LOAD_EQUIPMENT);

	int retCode = GetRolesInfoMan()->LoadEquipment(req->userId, req->roleId, req->id, req->pos);
	LogInfo(("�û�[%u]��ɫ[%u]����װ��[%u]λ��[%d], ������[%d]!", req->userId, req->roleId, req->id, req->pos, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_LOAD_EQUIPMENT);

	//ˢ�´���װ��
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_WEAR_BACKPACK);

	return gwsSession->SendBasicResponse(pHead, retCode, MLS_LOAD_EQUIPMENT);
}

int MLogicProcessMan::OnUnLoadEquipment(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerUnLoadEquipmentReq) == nDatalen);
	if (sizeof(SLinkerUnLoadEquipmentReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_UNLOAD_EQUIPMENT);

	SLinkerUnLoadEquipmentReq* req = (SLinkerUnLoadEquipmentReq*)pData;
	req->ntoh();

	int retCode = GetRolesInfoMan()->UnLoadEquipment(req->userId, req->roleId, req->id);
	LogInfo(("�û�[%u]��ɫ[%u]ж��װ��[%u], ������[%d]!", req->userId, req->roleId, req->id, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_UNLOAD_EQUIPMENT);

	//ˢ�´���װ��
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_WEAR_BACKPACK);

	return gwsSession->SendBasicResponse(pHead, retCode, MLS_UNLOAD_EQUIPMENT);
}

int MLogicProcessMan::OnEquipPosStrengthen(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerEquipPosStrengthenReq) == nDatalen);
	if (sizeof(SLinkerEquipPosStrengthenReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_EQUIP_POS_STRENGTHEN);

	SLinkerEquipPosStrengthenReq* req = (SLinkerEquipPosStrengthenReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SEquipPosStrengthenRes* pStrengthenRes = (SEquipPosStrengthenRes*)(buf + nLen);
	int retCode = GetRolesInfoMan()->EquipPosStrengthen(req->userId, req->roleId, req->pos, pStrengthenRes);
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_EQUIP_POS_STRENGTHEN);

	//ˢ��ս��
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_CP);

	//ˢ��װ��λ
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_EQUIP_POS);

	//ˢ�±���
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_BACKPACK);

	//ˢ�½��
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_GOLD);

	nLen += sizeof(SEquipPosStrengthenRes)+(pStrengthenRes->num - 1) * sizeof(SConsumeProperty);
	pStrengthenRes->hton();
	return gwsSession->SendResponse(pHead, retCode, MLS_EQUIP_POS_STRENGTHEN, buf, nLen);
}

int MLogicProcessMan::OnEquipPosRiseStar(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerEquipPosRiseStarReq) == nDatalen);
	if (sizeof(SLinkerEquipPosRiseStarReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_EQUIP_POS_RISE_STAR);

	SLinkerEquipPosRiseStarReq* req = (SLinkerEquipPosRiseStarReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SEquipPosRiseStarRes* pRiseStarRes = (SEquipPosRiseStarRes*)(buf + nLen);
	int retCode = GetRolesInfoMan()->EquipPosRiseStar(req->userId, req->roleId, req->pos, req->param, pRiseStarRes);
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_EQUIP_POS_RISE_STAR);

	/* zpy �ɾ�ϵͳ���� */
	GetAchievementMan()->OnRiseStar(req->userId, req->roleId);

	//ˢ��װ��λ
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_EQUIP_POS);

	//ˢ�±���
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_BACKPACK);

	//ˢ�½��
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_GOLD);

	nLen += sizeof(SEquipPosRiseStarRes)+(pRiseStarRes->num - 1) * sizeof(SConsumeProperty);
	pRiseStarRes->hton();
	return gwsSession->SendResponse(pHead, retCode, MLS_EQUIP_POS_RISE_STAR, buf, nLen);
}

int MLogicProcessMan::OnEquipPosInlaidGem(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerEquipPosInlaidGemReq) == nDatalen);
	if (sizeof(SLinkerEquipPosInlaidGemReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_EQUIP_POS_INLAID_GEM);

	SLinkerEquipPosInlaidGemReq* req = (SLinkerEquipPosInlaidGemReq*)pData;
	req->ntoh();

	int retCode = GetRolesInfoMan()->EquipPosInlaidGem(req->userId, req->roleId, req->pos, req->gemId, req->holeSeq);
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_EQUIP_POS_INLAID_GEM);

	/* zpy �ɾ�ϵͳ���� */
	GetAchievementMan()->OnInlaidGem(req->userId, req->roleId);

	//ˢ��ս��
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_CP);

	//ˢ��װ��λ
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_EQUIP_POS);

	//ˢ�±���
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_BACKPACK);

	return gwsSession->SendBasicResponse(pHead, retCode, MLS_EQUIP_POS_INLAID_GEM);
}

int MLogicProcessMan::OnEquipPosRemoveGem(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerEquipPosRemoveGemReq) == nDatalen);
	if (sizeof(SLinkerEquipPosRemoveGemReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_EQUIP_POS_REMOVE_GEM);

	SLinkerEquipPosRemoveGemReq* req = (SLinkerEquipPosRemoveGemReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SEquipPosRemoveGemRes* pRemoveGemRes = (SEquipPosRemoveGemRes*)(buf + nLen);
	int retCode = GetRolesInfoMan()->EquipPosRemoveGem(req->userId, req->roleId, req->pos, req->holeSeq, pRemoveGemRes);
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_EQUIP_POS_REMOVE_GEM);

	//ˢ��ս��
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_CP);

	//ˢ��װ��λ
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_EQUIP_POS);

	//ˢ�±���
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_BACKPACK);

	nLen += sizeof(SEquipPosRemoveGemRes);
	pRemoveGemRes->hton();
	return gwsSession->SendResponse(pHead, retCode, MLS_EQUIP_POS_REMOVE_GEM, buf, nLen);
}

int MLogicProcessMan::OnGetEquipPosAttr(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerGetEquipPosAttrReq) == nDatalen);
	if (sizeof(SLinkerGetEquipPosAttrReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_GET_EQUIP_POS_ATTR);

	SLinkerGetEquipPosAttrReq* req = (SLinkerGetEquipPosAttrReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SGetEquipPosAttrRes* pAttrRes = (SGetEquipPosAttrRes*)(buf + nLen);

	std::vector<SEquipPosAttr> equipPosVec;
	int retCode = GetRolesInfoMan()->GetRoleEquipPosAttr(req->userId, req->roleId, equipPosVec);
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_GET_EQUIP_POS_ATTR);

	pAttrRes->num = (BYTE)equipPosVec.size();
	memcpy(pAttrRes->data, equipPosVec.data(), pAttrRes->num * sizeof(SEquipPosAttr));

	pAttrRes->hton();
	nLen += sizeof(SGetEquipPosAttrRes)+(pAttrRes->num - 1) * sizeof(SEquipPosAttr);
	return gwsSession->SendResponse(pHead, retCode, MLS_GET_EQUIP_POS_ATTR, buf, nLen);
}

int MLogicProcessMan::OnEquipmentCompose(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(nDatalen >= sizeof(SLinkerEquipComposeReq)-sizeof(UINT32));
	if (nDatalen < sizeof(SLinkerEquipComposeReq)-sizeof(UINT32))
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_EQUIPMENT_COMPOSE);

	SLinkerEquipComposeReq* req = (SLinkerEquipComposeReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SEquipComposeRes* pComposeRes = (SEquipComposeRes*)(buf + nLen);
	int retCode = GetRolesInfoMan()->EquipmentCompose(req->userId, req->roleId, req->composeId, pComposeRes);
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_EQUIPMENT_COMPOSE);

	//ˢ���Լ�����װ��
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_WEAR_BACKPACK);

	//ˢ�½��
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_GOLD);

	//ˢ����Ƭ
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_FRAGMENT);

	pComposeRes->hton();
	nLen += sizeof(SEquipComposeRes)+(pComposeRes->num - 1) * sizeof(SPropertyAttr);
	return gwsSession->SendResponse(pHead, retCode, MLS_EQUIPMENT_COMPOSE, buf, nLen);
}

int MLogicProcessMan::OnQueryRoleApperance(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerQueryRoleAppearanceReq) == nDatalen);
	if (sizeof(SLinkerQueryRoleAppearanceReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_ROLE_APPEARANCE);

	SLinkerQueryRoleAppearanceReq* req = (SLinkerQueryRoleAppearanceReq*)pData;
	req->ntoh();

	int retCode = MLS_OK;
	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQueryRoleAppearanceRes* pAppearanceRes = (SQueryRoleAppearanceRes*)(buf + nLen);

	if (req->roleId == req->playerId)
	{
		retCode = GetRolesInfoMan()->QueryRoleAppearance(req->userId, req->playerId, pAppearanceRes);
		LogInfo(("�û�[%u]��ɫ[%u]��ѯ�Լ�������Ϣ, ������[%d]!", req->userId, req->playerId, retCode));
	}
	else
	{
		retCode = GetPlayerInfoMan()->QueryPlayerAppearance(req->playerId, pAppearanceRes);
		LogInfo(("�û�[%u]��ɫ[%u]��ѯ���[%u]������Ϣ, ������[%d]!", req->userId, req->roleId, req->playerId, retCode));
	}

	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_QUERY_ROLE_APPEARANCE);

	pAppearanceRes->hton();
	nLen += sizeof(SQueryRoleAppearanceRes)+(pAppearanceRes->num - 1) * sizeof(SPropertyAttr);
	return gwsSession->SendResponse(pHead, retCode, MLS_QUERY_ROLE_APPEARANCE, buf, nLen);
}

int MLogicProcessMan::OnQueryRoleExplicit(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerQueryRoleExplicitReq) == nDatalen);
	if (sizeof(SLinkerQueryRoleExplicitReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_ROLE_EXPLICIT);

	SLinkerQueryRoleExplicitReq* req = (SLinkerQueryRoleExplicitReq*)pData;
	req->ntoh();

	int retCode = MLS_OK;
	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQueryRoleExplicitRes* pExplicitRes = (SQueryRoleExplicitRes*)(buf + nLen);

	if (req->roleId == req->playerId)
	{
		retCode = GetRolesInfoMan()->QueryRoleExplicit(req->userId, req->playerId, pExplicitRes);
		LogInfo(("�û�[%u]��ɫ[%u]��ѯ�Լ���ϸ��Ϣ, ������[%d]!", req->userId, req->playerId, retCode));
	}
	else
	{
		retCode = GetPlayerInfoMan()->QueryPlayerExplicit(req->playerId, pExplicitRes);
		LogInfo(("�û�[%u]��ɫ[%u]��ѯ���[%u]��ϸ��Ϣ, ������[%d]!", req->userId, req->roleId, req->playerId, retCode));
	}

	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_QUERY_ROLE_EXPLICIT);

	pExplicitRes->hton();
	nLen += sizeof(SQueryRoleExplicitRes)+(pExplicitRes->num - 1) * sizeof(SSkillMoveInfo);
	return gwsSession->SendResponse(pHead, retCode, MLS_QUERY_ROLE_EXPLICIT, buf, nLen);
}

int MLogicProcessMan::OnUseProperty(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerUsePropertyReq) == nDatalen);
	if (sizeof(SLinkerUsePropertyReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_USE_PROPERTY);

	SLinkerUsePropertyReq* req = (SLinkerUsePropertyReq*)pData;
	req->ntoh();

	UINT16 propId = 0;
	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SUsePropertyRes* pUseProperty = (SUsePropertyRes*)(buf + nLen);

	int retCode = GetRolesInfoMan()->UseProperty(req->userId, req->roleId, req->id, propId, pUseProperty);
	LogInfo(("�û�[%u]��ɫ[%u]ʹ�õ���, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode == MLS_OK)
	{
		if (IS_VIT_POSION(propId))	//ˢ������
			GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_VIT);

		if (propId == ESGS_MATERIAL_BACKPACK_EXTEND)
		{
			//ˢ�±�������
			GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_CASH);
			GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_STORAGE);
		}

		//ˢ�±���
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_BACKPACK);
	}

	nLen += sizeof(SUsePropertyRes)+(pUseProperty->deduct_num - 1) * sizeof(SPropertyDeduct);
	pUseProperty->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_USE_PROPERTY, buf, nLen);
}

int MLogicProcessMan::OnReceiveAchievementReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerReceiveRewardReq) == nDatalen);
	if (sizeof(SLinkerReceiveRewardReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_RECEIVE_REWARD);

	SLinkerReceiveRewardReq* req = (SLinkerReceiveRewardReq*)pData;
	req->ntoh();

	/* zpy �ɾ�ϵͳ���� */
	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SReceiveRewardRes* pRewardInfo = (SReceiveRewardRes*)(buf + nLen);

	SReceiveRewardRefresh refresh_info;
	memset(&refresh_info, 0, sizeof(SReceiveRewardRefresh));
	int retCode = GetAchievementMan()->ReceiveAchievementReward(req->userId, req->roleId, req->achievement_id, pRewardInfo, &refresh_info);
	LogInfo(("�û�[%u]��ɫ[%d]��ȡ�ɾͽ���, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_RECEIVE_REWARD);

	// ˢ�����ݿ�
	ReceiveRewardRefresh(req->userId, req->roleId, refresh_info);

	nLen += sizeof(SReceiveRewardRes)+(pRewardInfo->num - 1) * sizeof(SPropertyAttr);
	pRewardInfo->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_RECEIVE_REWARD, buf, nLen);
}

int MLogicProcessMan::OnCashProperty(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerCashPropertyReq) == nDatalen);
	if (sizeof(SLinkerCashPropertyReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_CASH_PROPERTY);

	SLinkerCashPropertyReq* req = (SLinkerCashPropertyReq*)pData;
	req->ntoh();

	return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_CASH_PROPERTY);
}

int MLogicProcessMan::OnBuyBackProperty(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerPropBuyBackReq) == nDatalen);
	if (sizeof(SLinkerPropBuyBackReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_PROP_BUY_BACK);

	SLinkerPropBuyBackReq* req = (SLinkerPropBuyBackReq*)pData;
	req->ntoh();

	//��ѯ���ۼ�¼
	SPropertyAttr buyPropAttr;
	std::vector<SPropSellInfo> sellVec;
	int retCode = GetPropSellMan()->GetPropSellInfo(req->roleId, req->sellId, buyPropAttr, sellVec);
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_PROP_BUY_BACK);

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SPropBuyBackRes* pBuyBackRes = (SPropBuyBackRes*)(buf + nLen);
	retCode = GetRolesInfoMan()->BuyBackProperty(req->userId, req->roleId, buyPropAttr, req->num, pBuyBackRes);
	LogInfo(("�û�[%u]��ɫ[%u]�ع���Ʒ[%d]����[%d], ������[%d]!", req->userId, req->roleId, buyPropAttr.propId, buyPropAttr.num, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_PROP_BUY_BACK);

	//ˢ�±���
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_BACKPACK);

	//ˢ�½��
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_GOLD);

	//ɾ�����ۼ�¼
	GetPropSellMan()->DelPropSellInfo(req->roleId, req->sellId, req->num, sellVec);

	nLen += sizeof(SPropBuyBackRes)+(pBuyBackRes->propNum - 1) * sizeof(SPropertyAttr);
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_PROP_BUY_BACK, buf, nLen);
}

int MLogicProcessMan::OnQueryPropSellRcd(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerQueryRoleSellPropRcdReq) == nDatalen);
	if (sizeof(SLinkerQueryRoleSellPropRcdReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_ROLE_SELL_PROP_RCD);

	SLinkerQueryRoleSellPropRcdReq* req = (SLinkerQueryRoleSellPropRcdReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQueryRoleSellPropRcdRes* pSellRcdRes = (SQueryRoleSellPropRcdRes*)(buf + nLen);

	BYTE maxNum = (MAX_RES_USER_BYTES - member_offset(SQueryRoleSellPropRcdRes, total, sells)) / sizeof(SPropSellInfo);
	int retCode = GetPropSellMan()->QueryPropSellRecord(req->roleId, req->from, req->num, pSellRcdRes, maxNum);
	LogInfo(("�û�[%u]��ɫ[%u]��ѯ��Ʒ���ۼ�¼from[%u]����[%d], ������[%d]!", req->userId, req->roleId, req->from, req->num, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_QUERY_ROLE_SELL_PROP_RCD);

	nLen += sizeof(SQueryRoleSellPropRcdRes)+(pSellRcdRes->num - 1) * sizeof(SPropSellInfo);
	pSellRcdRes->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_QUERY_ROLE_SELL_PROP_RCD, buf, nLen);
}

int MLogicProcessMan::OnQueryAchievement(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerQueryAchievementReq) == nDatalen);
	if (sizeof(SLinkerQueryAchievementReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_ACHIEVEMENT);

	SLinkerQueryAchievementReq* req = (SLinkerQueryAchievementReq*)pData;
	req->ntoh();

	/* zpy �ɾ�ϵͳ���� */
	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQueryAchievementRes* pAchievementInfo = (SQueryAchievementRes*)(buf + nLen);

	if (req->type == 1)				// �ɳ�֮·
	{
		int retCode = GetAchievementMan()->QueryRoadToGrowth(req->userId, req->roleId, req->start_pos, pAchievementInfo);
		LogInfo(("�û�[%u]��ɫ[%d]��ѯ�ɳ�֮·�ɾ�", req->userId, req->roleId));
		if (retCode != MLS_OK)
			return gwsSession->SendBasicResponse(pHead, retCode, MLS_QUERY_ACHIEVEMENT);
	}
	else if (req->type == 2)		// ð������
	{
		int retCode = GetAchievementMan()->QueryAdventureCourse(req->userId, req->roleId, req->start_pos, pAchievementInfo);
		LogInfo(("�û�[%u]��ɫ[%d]��ѯð�����̳ɾ�", req->userId, req->roleId));
		if (retCode != MLS_OK)
			return gwsSession->SendBasicResponse(pHead, retCode, MLS_QUERY_ACHIEVEMENT);
	}
	else if (req->type == 3)		// ��������
	{
		int retCode = GetAchievementMan()->QueryPerfectSkill(req->userId, req->roleId, req->start_pos, pAchievementInfo);
		LogInfo(("�û�[%u]��ɫ[%d]��ѯ�������ɳɾ�", req->userId, req->roleId));
		if (retCode != MLS_OK)
			return gwsSession->SendBasicResponse(pHead, retCode, MLS_QUERY_ACHIEVEMENT);
	}
	else
	{
		return gwsSession->SendResponse(pHead, MLS_INVALID_ACHIEVEMENT_TYPE, MLS_QUERY_ACHIEVEMENT, buf, nLen);
	}

	// ��ѯ�ɾ͵���
	int retCode = GetAchievementMan()->QueryAchievementPoint(req->roleId, pAchievementInfo);
	LogInfo(("�û�[%u]��ɫ[%d]��ѯ�ɾ͵���", req->userId, req->roleId));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_QUERY_ACHIEVEMENT);

	nLen += sizeof(SQueryAchievementRes)+(pAchievementInfo->num - 1) * sizeof(SAchievementInfo);
	pAchievementInfo->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_QUERY_ACHIEVEMENT, buf, nLen);
}

int MLogicProcessMan::OnDelRole(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerDelRoleReq) == nDatalen);
	if (sizeof(SLinkerDelRoleReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_DEL_ROLE);

	SLinkerDelRoleReq* req = (SLinkerDelRoleReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SDelRoleRes* pDelRes = (SDelRoleRes*)(buf + nLen);
	int retCode = GetRolesInfoMan()->DelRole(req->userId, req->roleId, pDelRes);
	LogInfo(("�û�[%u]ɾ����ɫ[%u], ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_DEL_ROLE);

	if (pDelRes->success)
	{
		//�����ɫ��������
		GetLadderRankMan()->DelRole(req->roleId);

		//�����ɫ��������
		GetInstRankInfoMan()->DelRoleInstRank(req->roleId);

		/* zpy �ɾ�ϵͳ���� */
		GetAchievementMan()->ClearCache(req->roleId);
	}

	pDelRes->ntoh();
	nLen += sizeof(SDelRoleRes);
	return gwsSession->SendResponse(pHead, retCode, MLS_DEL_ROLE, buf, nLen);
}

int MLogicProcessMan::OnQueryGuildName(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SQueryGuildNameReq) == nDatalen);
	if (sizeof(SQueryGuildNameReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_GUILD_NAME);

	SQueryGuildNameReq* req = (SQueryGuildNameReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQueryGuildNameRes* pNameRes = (SQueryGuildNameRes*)(buf + nLen);

	int retCode = GetGuildMan()->QueryGuildName(req->guildId, pNameRes);
	nLen += sizeof(SQueryGuildNameRes);
	pNameRes->hton();
	return gwsSession->SendResponse(pHead, retCode, MLS_QUERY_GUILD_NAME, buf, nLen);
}

int MLogicProcessMan::OnUserExit(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerUserExitReq) == nDatalen);
	if (sizeof(SLinkerUserExitReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_USER_EXIT);

	SLinkerUserExitReq* req = (SLinkerUserExitReq*)pData;
	req->ntoh();

	//�������Ϸ��ɫ
	std::vector<UINT32> rolesVec;
	GetRolesInfoMan()->GetUserEnteredRoles(req->userId, rolesVec);
	if (rolesVec.empty())
	{
		GetRolesInfoMan()->RemoveUserRoles(req->userId);
		return gwsSession->SendBasicResponse(pHead, MLS_OK, MLS_USER_EXIT);
	}

	//ͬ�����ݿ��û�����
	GetRefreshDBMan()->ExitSyncUserRoleInfo(req->userId, rolesVec);

	//ͬ����������,ħ������Ϣ
	GetPermanenDataMan()->ExitSyncUserPermanenData(req->userId, rolesVec);

	//�����û�����
	GetRolesInfoMan()->RemoveUserRoles(req->userId);

	LogInfo(("�û�[%u]�˳�, ������ʱ����!", req->userId));

	return gwsSession->SendBasicResponse(pHead, MLS_OK, MLS_USER_EXIT);
}

// int MLogicProcessMan::OnQueryInstRankInfo(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
// {
// 	ASSERT(sizeof(SQueryInstRankReq) == nDatalen);
// 	if(sizeof(SQueryInstRankReq) != nDatalen)
// 		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_INST_RANK);
// 
// 	SQueryInstRankReq* req = (SQueryInstRankReq*)pData;
// 	req->ntoh();
// 
// 	char buf[MAX_BUF_LEN] = { 0 };
// 	int nLen = GWS_COMMON_REPLY_LEN;
// 	SQueryInstRankRes* pInstRankRes = (SQueryInstRankRes*)(buf + nLen);
// 
// 	BYTE rankNum = (MAX_RES_USER_BYTES - member_offset(SQueryInstRankRes, instId, ranks)) / sizeof(SInstRankInfo);
// 	pInstRankRes->num = GetInstRankInfoMan()->QueryInstRankInfo(req->instId, pInstRankRes->ranks, rankNum);
// 	pInstRankRes->instId = req->instId;
// 	pInstRankRes->hton();
// 	nLen += sizeof(SQueryInstRankRes) + (pInstRankRes->num - 1) * sizeof(SInstRankInfo);
// 
// 	LogInfo(("��ѯ��ɸ���[%d]������Ϣ!", req->instId));
// 
// 	return gwsSession->SendResponse(pHead, MLS_OK, MLS_QUERY_INST_RANK, buf, nLen);
// }

int MLogicProcessMan::OnReqTimerTask(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerPersonalTimerTaskReq) == nDatalen);
	if (sizeof(SLinkerPersonalTimerTaskReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_PERSONAL_TIMER_TASK);

	SLinkerPersonalTimerTaskReq* req = (SLinkerPersonalTimerTaskReq*)pData;
	req->ntoh();

	UINT32 newId = 0;
	int retCode = GetPersonalTimerTaskMan()->JoinPersonalTimerTaskQ(req->roleId, req->taskId, pHead->targetId, pHead->userId, newId);
	LogInfo(("�յ���ʱ�����ɫ[%u],����[%d], ������[%d]!", req->roleId, req->taskId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_PERSONAL_TIMER_TASK);

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SPersonalTimerTaskRes* pTimerTaskRes = (SPersonalTimerTaskRes*)(buf + nLen);
	pTimerTaskRes->taskId = req->taskId;
	pTimerTaskRes->newId = newId;

	nLen += sizeof(SPersonalTimerTaskRes);
	pTimerTaskRes->hton();

	return gwsSession->SendResponse(pHead, MLS_OK, MLS_PERSONAL_TIMER_TASK, buf, nLen);
}

int MLogicProcessMan::OnQueryRoleSkills(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerQuerySkillsReq) == nDatalen);
	if (sizeof(SLinkerQuerySkillsReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_ROLE_SKILLS);

	SLinkerQuerySkillsReq* req = (SLinkerQuerySkillsReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQuerySkillsRes* pSkillsRes = (SQuerySkillsRes*)(buf + nLen);

	int retCode = GetRolesInfoMan()->QueryRoleSkills(req->userId, req->roleId, pSkillsRes);
	LogInfo(("�û�[%u]��ѯ��ɫ[%u]����, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_QUERY_ROLE_SKILLS);

	nLen += sizeof(SQuerySkillsRes)+(pSkillsRes->skillNum - 1) * sizeof(SSkillMoveInfo);
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_QUERY_ROLE_SKILLS, buf, nLen);
}

int MLogicProcessMan::OnSkillUpgrade(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerSkillUpgradeReq) == nDatalen);
	if (sizeof(SLinkerSkillUpgradeReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_SKILL_UPGRADE);

	SLinkerSkillUpgradeReq* req = (SLinkerSkillUpgradeReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SSkillUpgradeRes* pUpgradeRes = (SSkillUpgradeRes*)(buf + nLen);
	int retCode = GetRolesInfoMan()->SkillUpgrade(req->userId, req->roleId, req->skillId, req->step, pUpgradeRes);
	LogInfo(("�û�[%u]��ɫ[%u]��������[%d], ������[%d]!", req->userId, req->roleId, req->skillId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_SKILL_UPGRADE);

	/* zpy �ɾ�ϵͳ���� */
	GetAchievementMan()->OnSkillUpgrade(req->userId, req->roleId);

	//ˢ�����ݿ�����
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_SKILLS);

	nLen += sizeof(SSkillUpgradeRes);
	pUpgradeRes->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_SKILL_UPGRADE, buf, nLen);
}

int MLogicProcessMan::OnChangeFaction(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerChangeFactionReq) == nDatalen);
	if (sizeof(SLinkerChangeFactionReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_CHANGE_FACTION);

	SLinkerChangeFactionReq* req = (SLinkerChangeFactionReq*)pData;
	req->ntoh();

	BOOL needSyncDB = false;
	int retCode = GetRolesInfoMan()->ChangeFaction(req->userId, req->roleId, req->factionId, needSyncDB);
	LogInfo(("�û�[%u]��ɫ[%u]��Ӫ�ı�Ϊ[%d], ������[%d]!", req->userId, req->roleId, req->factionId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_CHANGE_FACTION);

	//ˢ�����ݿ�
	if (needSyncDB)
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_FACTION);

	return gwsSession->SendBasicResponse(pHead, MLS_OK, MLS_CHANGE_FACTION);
}

int MLogicProcessMan::OnLoadSkill(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerLoadSkillReq) == nDatalen);
	if (sizeof(SLinkerLoadSkillReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_LOAD_SKILL);

	SLinkerLoadSkillReq* req = (SLinkerLoadSkillReq*)pData;
	req->ntoh();

	BOOL needSyncDB = false;
	int retCode = GetRolesInfoMan()->LoadSkill(req->userId, req->roleId, req->skillId, req->key, needSyncDB);
	LogInfo(("�û�[%u]��ɫ[%u]װ�ؼ���[%d], key[%d], ������[%d]!", req->userId, req->roleId, req->skillId, req->key, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_LOAD_SKILL);

	//ˢ�����ݿ�
	if (needSyncDB)
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_SKILLS);

	//������Ӧ
	return gwsSession->SendBasicResponse(pHead, retCode, MLS_LOAD_SKILL);
}

int MLogicProcessMan::OnCancelSkill(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerCancelSkillReq) == nDatalen);
	if (sizeof(SLinkerCancelSkillReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_CANCEL_SKILL);

	SLinkerCancelSkillReq* req = (SLinkerCancelSkillReq*)pData;
	req->ntoh();

	BOOL needSyncDB = false;
	int retCode = GetRolesInfoMan()->CancelSkill(req->userId, req->roleId, req->skillId, needSyncDB);
	LogInfo(("�û�[%u]��ɫ[%u]ж�ؼ���[%d], ������[%d]!", req->userId, req->roleId, req->skillId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_CANCEL_SKILL);

	//ˢ�����ݿ�
	if (needSyncDB)
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_SKILLS);

	//������Ӧ
	return gwsSession->SendBasicResponse(pHead, retCode, MLS_CANCEL_SKILL);
}

int MLogicProcessMan::OnRechargeToken(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerRechargeReq) == nDatalen);
	if (sizeof(SLinkerRechargeReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_DELTA_TOKEN);

	SLinkerRechargeReq* req = (SLinkerRechargeReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SRechargeRes* pRechargeRes = (SRechargeRes*)(buf + nLen);
	char szRoleNick[33] = { 0 };

	int retCode = GetRolesInfoMan()->RechargeToken(req->userId, req->roleId, req->cashId, pRechargeRes, szRoleNick);
	LogInfo(("�û�[%u]��ɫ[%u]��ֵRMB[%u], ������[%d]!", req->userId, req->roleId, req->cashId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_DELTA_TOKEN);

	//ˢ��money
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_CASH);

	pRechargeRes->hton();
	nLen += sizeof(SRechargeRes);
	gwsSession->SendResponse(pHead, MLS_OK, MLS_DELTA_TOKEN, buf, nLen);

	//�Ƿ��׳䲢��������
	RechargeCfg::SRechargeCfgAttr cashAttr;
	GetGameCfgFileMan()->GetRechargeAttr(req->cashId, cashAttr);
	if (cashAttr.firstGive > 0)
	{
		if (GetRechargeMan()->IsFirstRecharge(req->userId, req->roleId, req->cashId))
		{
			EmailCleanMan::SSyncEmailSendContent email;
			email.type = ESGS_EMAIL_FIRST_RECHARGE;
			email.receiverRoleId = req->roleId;	//���ս�ɫid
			email.receiverUserId = req->userId;	//�����˺�id
			email.pGBSenderNick = "ϵͳ";	//�������ǳ�
			email.pGBCaption = "�׳�����";	//����
			email.rpcoin = cashAttr.firstGive;	//�׳����ʹ���
			char szBody[64] = { 0 };
			sprintf(szBody, "�״γ�ֵ, ��%u��%u", cashAttr.rpcoin, cashAttr.firstGive);
			email.pGBBody = szBody;
			GetEmailCleanMan()->AsyncSendEmail(email);
		}
		else
			cashAttr.firstGive = 0;
	}

	//��¼��ֵ����
	//by hxw 20151125 ����userid
	GetRechargeMan()->InputRechargeRecord(req->userId, req->roleId, szRoleNick, req->cashId, cashAttr.rmb, cashAttr.rpcoin, cashAttr.firstGive);

	return retCode;
}

int MLogicProcessMan::OnGetRoleOccupyInst(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerGetRoleOccupyInstReq) == nDatalen);
	if (sizeof(SLinkerGetRoleOccupyInstReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_GET_ROLE_OCCUPY_INST);

	SLinkerGetRoleOccupyInstReq* req = (SLinkerGetRoleOccupyInstReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SGetRoleOccupyInstRes* pOccupyInstRes = (SGetRoleOccupyInstRes*)(buf + nLen);
	int maxRetNum = (MAX_RES_USER_BYTES - member_offset(SGetRoleOccupyInstRes, num, occupyInstIds)) / sizeof(UINT16);

	GetInstRankInfoMan()->GetRoleOccupyInst(req->userId, req->roleId, pOccupyInstRes, maxRetNum);
	LogInfo(("�û�[%u]��ɫ[%u]��ѯ��ռ�츱��, ������[0]", req->userId, req->roleId));

	nLen += sizeof(SGetRoleOccupyInstRes)+(pOccupyInstRes->num - 1) * sizeof(UINT16);
	pOccupyInstRes->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_GET_ROLE_OCCUPY_INST, buf, nLen);
}

int MLogicProcessMan::OnSetRoleTitleId(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerSetRoleTitleReq) == nDatalen);
	if (sizeof(SLinkerSetRoleTitleReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_SET_ROLE_TITLE_ID);

	SLinkerSetRoleTitleReq* req = (SLinkerSetRoleTitleReq*)pData;
	req->ntoh();

	int retCode = GetRolesInfoMan()->SetRoleTitleId(req->userId, req->roleId, req->titleId);
	LogInfo(("�û�[%u]��ɫ[%u]���óƺ�id[%d], ������[%d]!", req->userId, req->roleId, req->titleId, retCode));

	//ˢ�³ƺ�id
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_TITLE_ID);

	return gwsSession->SendBasicResponse(pHead, retCode, MLS_SET_ROLE_TITLE_ID);
}

int MLogicProcessMan::OnMallBuyGoods(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerMallBuyGoodsReq) == nDatalen);
	if (sizeof(SLinkerMallBuyGoodsReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_MALL_BUY_GOODS);

	SLinkerMallBuyGoodsReq* req = (SLinkerMallBuyGoodsReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SMallBuyGoodsRes* pBuyGoodsRes = (SMallBuyGoodsRes*)(buf + nLen);
	char szRoleNick[33] = { 0 };
	UINT32 spendToken = 0;

	int retCode = GetRolesInfoMan()->MallBuyGoods(req->userId, req->roleId, req->buyId, req->num, pBuyGoodsRes, szRoleNick, spendToken);
	LogInfo(("�û�[%u]��ɫ[%u]�̳ǹ�����Ʒ[%d]����[%d], ������[%d]!", req->userId, req->roleId, req->buyId, req->num, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_MALL_BUY_GOODS);

	//ˢ�´���
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_CASH);

	if (pBuyGoodsRes->backpackNum > 0)	//ˢ�±���
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_BACKPACK);
	else	//ˢ��ӵ��ʱװ
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_DRESS);

	nLen += sizeof(SMallBuyGoodsRes)+(pBuyGoodsRes->backpackNum - 1) * sizeof(SPropertyAttr);
	pBuyGoodsRes->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_MALL_BUY_GOODS, buf, nLen);
}

int MLogicProcessMan::OnQueryRoleDress(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerQueryRoleDressReq) == nDatalen);
	if (sizeof(SLinkerQueryRoleDressReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_ROLE_DRESS);

	SLinkerQueryRoleDressReq* req = (SLinkerQueryRoleDressReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQueryRoleDressRes* pDressRes = (SQueryRoleDressRes*)(buf + nLen);
	int retCode = GetRolesInfoMan()->QueryRoleDress(req->userId, req->roleId, pDressRes);
	LogInfo(("�û�[%u]��ɫ[%u]��ѯӵ��ʱװ, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_QUERY_ROLE_DRESS);

	nLen += sizeof(SQueryRoleDressRes)+(pDressRes->num - 1) * sizeof(UINT16);
	pDressRes->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_QUERY_ROLE_DRESS, buf, nLen);
}

int MLogicProcessMan::OnWearDress(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerWearDressReq) == nDatalen);
	if (sizeof(SLinkerWearDressReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_WEAR_DRESS);

	SLinkerWearDressReq* req = (SLinkerWearDressReq*)pData;
	req->ntoh();

	int retCode = GetRolesInfoMan()->WearDress(req->userId, req->roleId, req->dressId);
	LogInfo(("�û�[%u]��ɫ[%u]����ʱװ[%d], ������[%d]!", req->userId, req->roleId, req->dressId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_WEAR_DRESS);

	//ˢ��ʱװ
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_DRESS_WEAR);

	return gwsSession->SendBasicResponse(pHead, retCode, MLS_WEAR_DRESS);
}

int MLogicProcessMan::OnFetchRobot(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerFetchRobotReq) == nDatalen);
	if (sizeof(SLinkerFetchRobotReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_FETCH_ROBOT);

	SLinkerFetchRobotReq* req = (SLinkerFetchRobotReq*)pData;
	req->ntoh();

	SFetchRobotReq* pRobotReq = (SFetchRobotReq*)((BYTE*)req + sizeof(UINT32)* 2);

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SFetchRobotRes* pRobotRes = (SFetchRobotRes*)(buf + nLen);
	int retCode = GetPlayerInfoMan()->FetchRobotData(req->userId, req->roleId, pRobotReq, pRobotRes);
	LogInfo(("�û�[%u]��ɫ[%u]��ȡ����������, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_FETCH_ROBOT);

	int nSize = sizeof(SFetchRobotRes)-sizeof(char);
	nLen += nSize + (pRobotRes->wearEquipNum) * sizeof(SPropertyAttr)+(pRobotRes->skillNum) * sizeof(UINT16)+(pRobotRes->equipPosNum) * sizeof(SEquipPosAttr)+(pRobotRes->chainSoulPosNum) * sizeof(SChainSoulPosAttr);
	pRobotRes->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_FETCH_ROBOT, buf, nLen);
}

int MLogicProcessMan::OnGetPersonalTransferEnter(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerGetPersonalTransferEnterReq) == nDatalen);
	if (sizeof(SLinkerGetPersonalTransferEnterReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_GET_PERSONAL_TRANSFER_ENTER);

	SLinkerGetPersonalTransferEnterReq* req = (SLinkerGetPersonalTransferEnterReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SGetPersonalTransferEnterRes* pEnterRes = (SGetPersonalTransferEnterRes*)(buf + nLen);
	int retCode = GetPersonalRandInstMan()->GetTransferEnterNum(req->transferId, pEnterRes->enterNum, pEnterRes->leftSec);
	LogInfo(("�û�[%u]��ɫ[%u]��ȡ��������������������, ������[%d]!", req->userId, req->roleId, retCode));
	// 	if(retCode != MLS_OK)
	// 		return gwsSession->SendBasicResponse(pHead, retCode, MLS_GET_PERSONAL_TRANSFER_ENTER);

	nLen += sizeof(SGetPersonalTransferEnterRes);
	pEnterRes->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_GET_PERSONAL_TRANSFER_ENTER, buf, nLen);
}

int MLogicProcessMan::OnGetOpenPersonalRandInst(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerGetOpenPersonalRandInstReq) == nDatalen);
	if (sizeof(SLinkerGetOpenPersonalRandInstReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_GET_OPEN_PERSONAL_RAND_INST);

	SLinkerGetOpenPersonalRandInstReq* req = (SLinkerGetOpenPersonalRandInstReq*)pData;
	req->ntoh();

	return gwsSession->SendBasicResponse(pHead, MLS_ERROR, MLS_GET_OPEN_PERSONAL_RAND_INST);

	/*char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SGetOpenPersonalRandInstRes* pOpenedRes = (SGetOpenPersonalRandInstRes*)(buf + nLen);
	GetPersonalRandInstMan()->GetOpenedTransfers(pOpenedRes);
	LogInfo(("�û�[%u]��ɫ[%u]��ȡ�Ѵ򿪵������������Ϣ, ������[%d]!", req->userId, req->roleId, 0));
	nLen += sizeof(SGetOpenPersonalRandInstRes) + (pOpenedRes->num - 1) * sizeof(SPersonalRandInstInfo);
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_GET_OPEN_PERSONAL_RAND_INST, buf, nLen);*/
}

int MLogicProcessMan::OnQueryEquipFragment(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerQueryEquipFragmentReq) == nDatalen);
	if (sizeof(SLinkerQueryEquipFragmentReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_EQUIP_FRAGMENT);

	SLinkerQueryEquipFragmentReq* req = (SLinkerQueryEquipFragmentReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQueryEquipFragmentRes * pFragmentRes = (SQueryEquipFragmentRes*)(buf + nLen);
	int retCode = GetRolesInfoMan()->QueryEquipFragment(req->userId, req->roleId, pFragmentRes);
	LogInfo(("�û�[%u]��ɫ[%u]��ѯװ����Ƭ, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_QUERY_EQUIP_FRAGMENT);

	for (BYTE i = 0; i < pFragmentRes->num; ++i)
		pFragmentRes->data[i].hton();
	nLen += sizeof(SQueryEquipFragmentRes)+(pFragmentRes->num - 1) * sizeof(SEquipFragmentAttr);
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_QUERY_EQUIP_FRAGMENT, buf, nLen);
}

int MLogicProcessMan::OnRoleRevive(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerReviveRoleReq) == nDatalen);
	if (sizeof(SLinkerReviveRoleReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_ROLE_REVIVE);

	SLinkerReviveRoleReq* req = (SLinkerReviveRoleReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SReviveRoleRes* pReviveRes = (SReviveRoleRes*)(buf + nLen);

	int retCode = GetRolesInfoMan()->RoleRevive(req->userId, req->roleId, req->reviveId, pReviveRes);
	LogInfo(("�û�[%u]��ɫ[%u]����[%d], ������[%d]!", req->userId, req->roleId, req->reviveId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_ROLE_REVIVE);

	//ˢ�±���
	if (pReviveRes->num > 0)
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_BACKPACK);

	//ˢ�´���
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_CASH);

	for (BYTE i = 0; i < pReviveRes->num; ++i)
		pReviveRes->data[i].hton();
	nLen += sizeof(SReviveRoleRes)+(pReviveRes->num - 1) * sizeof(SConsumeProperty);
	pReviveRes->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_ROLE_REVIVE, buf, nLen);
}

int MLogicProcessMan::OnQueryGoldExchange(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerQueryGoldExchangeReq) == nDatalen);
	if (sizeof(SLinkerQueryGoldExchangeReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_GOLD_EXCHANGE);

	SLinkerQueryGoldExchangeReq* req = (SLinkerQueryGoldExchangeReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQueryGoldExchangeRes* pExchangeRes = (SQueryGoldExchangeRes*)(buf + nLen);

	int retCode = GetRolesInfoMan()->QueryGoldExchange(req->userId, req->roleId, pExchangeRes);
	LogInfo(("�û�[%u]��ɫ[%u]��ѯ��Ҷһ�, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_QUERY_GOLD_EXCHANGE);

	nLen += sizeof(SQueryGoldExchangeRes);
	pExchangeRes->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_QUERY_GOLD_EXCHANGE, buf, nLen);
}

int MLogicProcessMan::OnGoldExchange(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerGoldExchangeReq) == nDatalen);
	if (sizeof(SLinkerGoldExchangeReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_GOLD_EXCHANGE);

	SLinkerGoldExchangeReq* req = (SLinkerGoldExchangeReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SGoldExchangeRes* pExchangeRes = (SGoldExchangeRes*)(buf + nLen);

	int retCode = GetRolesInfoMan()->GoldExchange(req->userId, req->roleId, pExchangeRes);
	LogInfo(("�û�[%u]��ɫ[%u]��Ҷһ�, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_GOLD_EXCHANGE);

	//ˢ�½��/����
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_GOLD_EXCHANGE);

	//ˢ�¶һ�����
	GetGoldExchangeMan()->RefeshRoleGoldExchange(req->roleId, pExchangeRes->exchangedTimes);

	nLen += sizeof(SGoldExchangeRes);
	pExchangeRes->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_GOLD_EXCHANGE, buf, nLen);
}

int MLogicProcessMan::OnQueryNotReceiveVIPReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerQueryNotReceiveVIPRewardReq) == nDatalen);
	if (sizeof(SLinkerQueryNotReceiveVIPRewardReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_NOT_RECEIVE_VIP_REWARD);

	SLinkerQueryNotReceiveVIPRewardReq* req = (SLinkerQueryNotReceiveVIPRewardReq*)pData;
	req->ntoh();

	UINT64 gold = 0;
	UINT32 cash = 0;
	UINT32 cashcount = 0;
	int retCode = GetRolesInfoMan()->GetRoleMoney(req->userId, req->roleId, gold, cash, cashcount);
	LogInfo(("�û�[%u]��ɫ[%u]��ѯδ��ȡ����VIP�ȼ�, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_NOT_RECEIVE_VIP_REWARD);

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQueryNotReceiveVIPRewardRes* pVIPRes = (SQueryNotReceiveVIPRewardRes*)(buf + nLen);

	BYTE vipLv = GetGameCfgFileMan()->CalcVIPLevel(cashcount);
	if (vipLv > 0)
		GetVIPMan()->QueryRoleNotReceiveVIPReward(req->roleId, vipLv, pVIPRes);

	nLen += sizeof(SQueryNotReceiveVIPRewardRes)+(pVIPRes->num - 1) * sizeof(BYTE);
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_NOT_RECEIVE_VIP_REWARD, buf, nLen);
}

int MLogicProcessMan::OnReceiveVIPReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerReceiveVIPRewardReq) == nDatalen);
	if (sizeof(SLinkerReceiveVIPRewardReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_RECEIVE_VIP_REWARD);

	SLinkerReceiveVIPRewardReq* req = (SLinkerReceiveVIPRewardReq*)pData;
	req->ntoh();

	//VIP0������ȡ
	if (req->vipLv == 0)
		return gwsSession->SendBasicResponse(pHead, MLS_VIP_LV_NOT_ENOUGH, MLS_RECEIVE_VIP_REWARD);

	//�Ƿ���ȡ����VIP�ȼ�
	if (GetVIPMan()->IsReceivedVIPReward(req->roleId, req->vipLv))
		return gwsSession->SendBasicResponse(pHead, MLS_ALREADY_RECEIVED_VIP, MLS_RECEIVE_VIP_REWARD);

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SReceiveVIPRewardRes* pVIPRewardRes = (SReceiveVIPRewardRes*)(buf + nLen);
	SReceiveRewardRefresh refresh_info;
	memset(&refresh_info, 0, sizeof(SReceiveRewardRefresh));

	int retCode = GetRolesInfoMan()->ReceiveVIPReward(req->userId, req->roleId, req->vipLv, pVIPRewardRes, &refresh_info);
	LogInfo(("�û�[%u]��ɫ[%u]��ȡ����VIP%d����, ������[%d]!", req->userId, req->roleId, req->vipLv, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_RECEIVE_VIP_REWARD);

	for (BYTE i = 0; i < pVIPRewardRes->num; ++i)
		pVIPRewardRes->data[i].hton();
	nLen += sizeof(SReceiveVIPRewardRes)+(pVIPRewardRes->num - 1) * sizeof(SPropertyAttr);
	pVIPRewardRes->hton();
	gwsSession->SendResponse(pHead, MLS_OK, MLS_RECEIVE_VIP_REWARD, buf, nLen);

	// ˢ�����ݿ�
	ReceiveRewardRefresh(req->userId, req->roleId, refresh_info);

	//��¼��ȡ
	GetVIPMan()->RecordRoleReceiveVIPReward(req->roleId, req->vipLv);

	return retCode;
}

int MLogicProcessMan::OnQueryRechargeFirstGive(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerQueryRechargeFirstGiveReq) == nDatalen);
	if (sizeof(SLinkerQueryRechargeFirstGiveReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_FIRST_RECHARGE_GIVE);

	SLinkerQueryRechargeFirstGiveReq* req = (SLinkerQueryRechargeFirstGiveReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQueryRechargeFirstGiveRes* pRechargeRes = (SQueryRechargeFirstGiveRes*)(buf + nLen);
	GetRechargeMan()->QueryRechargeFirstGive(req->userId, req->roleId, pRechargeRes);
	LogInfo(("�û�[%u]��ɫ[%u]��ѯ�׳�����id!", req->userId, req->roleId));

	nLen += sizeof(SQueryRechargeFirstGiveRes)+(pRechargeRes->num - 1) * sizeof(UINT16);
	pRechargeRes->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_QUERY_FIRST_RECHARGE_GIVE, buf, nLen);
}

int MLogicProcessMan::OnQuerySignIn(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerQueryOnlineSignInReq) == nDatalen);
	if (sizeof(SLinkerQueryOnlineSignInReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_ONLINE_SIGN_IN);

	SLinkerQueryOnlineSignInReq* req = (SLinkerQueryOnlineSignInReq*)pData;
	req->ntoh();

	LogInfo(("�û�[%u]��ɫ[%u]��ѯ����ǩ��", req->userId, req->roleId));

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQueryOnlineSignInRes* pSignInRes = (SQueryOnlineSignInRes*)(buf + nLen);
	GetRolesInfoMan()->QuerySignIn(req->userId, req->roleId, pSignInRes);

	nLen += sizeof(SQueryOnlineSignInRes);
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_QUERY_ONLINE_SIGN_IN, buf, nLen);
}

int MLogicProcessMan::OnlineSignIn(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerOnlineSignInReq) == nDatalen);
	if (sizeof(SLinkerOnlineSignInReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_ONLINE_SIGN_IN);

	SLinkerOnlineSignInReq* req = (SLinkerOnlineSignInReq*)pData;
	req->ntoh();

	int retCode = GetRolesInfoMan()->SignIn(req->userId, req->roleId);
	LogInfo(("�û�[%u]��ɫ[%u]����ǩ��, ������[%d]", req->userId, req->roleId, retCode));
	if (retCode == MLS_OK)
		GetOnlineSingnInMan()->RefreshSignInInfo(req->userId, req->roleId);

	return gwsSession->SendBasicResponse(pHead, retCode, MLS_ONLINE_SIGN_IN);
}

int MLogicProcessMan::OnQueryReceivedActivity(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerQueryReceivedActivityReq) == nDatalen);
	if (sizeof(SLinkerQueryReceivedActivityReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_RECEIVED_ACTIVITY);

	SLinkerQueryReceivedActivityReq* req = (SLinkerQueryReceivedActivityReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQueryReceivedActivityRes* pActivityRes = (SQueryReceivedActivityRes*)(buf + nLen);
	int retCode = GetActitvityMan()->QueryReceivedActivity(req->roleId, pActivityRes);
	LogInfo(("�û�[%u]��ɫ[%u]��ѯ����ȡ�����, ������[%d]", req->userId, req->roleId, retCode));
	if (retCode != retCode)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_RECEIVED_ACTIVITY);

	nLen += sizeof(SQueryReceivedActivityRes)+(pActivityRes->num - 1) * sizeof(UINT32);
	pActivityRes->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_QUERY_RECEIVED_ACTIVITY, buf, nLen);
}

int MLogicProcessMan::OnReceivedActivity(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerReceiveActivityReq) == nDatalen);
	if (sizeof(SLinkerReceiveActivityReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_RECEIVED_ACTIVITY);

	SLinkerReceiveActivityReq* req = (SLinkerReceiveActivityReq*)pData;
	req->ntoh();

	LogInfo(("�û�[%u]��ɫ[%u]��ȡ�[%u]����", req->userId, req->roleId, req->activityId));

	//�жϻ����
	ActivityCfg::SActivityItem activityItem;
	int retCode = GetGameCfgFileMan()->GetActivityItem(req->activityId, activityItem);
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_RECEIVED_ACTIVITY);

	if (GetMsel() > activityItem.endMSel)
		return gwsSession->SendBasicResponse(pHead, MLS_ACTIVITY_OUT_DATE, MLS_RECEIVED_ACTIVITY);

	//�Ƿ�μӹ��Ļ
	if (!GetActitvityMan()->IsReceivedActivity(req->roleId, req->activityId))
		retCode = MLS_ALREADY_RECEIVED_ACTIVITY;

	gwsSession->SendBasicResponse(pHead, retCode, MLS_RECEIVED_ACTIVITY);

	//���ͻ�����ʼ�
	UINT32 rpcoin = 0;
	UINT64 gold = 0;
	UINT32 vit = 0;
	UINT64 exp = 0;
	std::vector<SPropertyAttr> propVec;
	int n = activityItem.rewardVec.size();
	for (int i = 0; i < n; ++i)
	{
		if (activityItem.rewardVec[i].type == ESGS_PROP_TOKEN)
			rpcoin = SGSU32Add(rpcoin, activityItem.rewardVec[i].num);

		else if (activityItem.rewardVec[i].type == ESGS_PROP_GOLD)
			gold = SGSU64Add(gold, activityItem.rewardVec[i].num);

		else if (activityItem.rewardVec[i].type == ESGS_PROP_EXP)
			exp = SGSU64Add(exp, activityItem.rewardVec[i].num);

		else if (activityItem.rewardVec[i].type == ESGS_PROP_VIT)
			vit = SGSU32Add(vit, activityItem.rewardVec[i].num);

		else if (activityItem.rewardVec[i].type == ESGS_PROP_EQUIP ||
			activityItem.rewardVec[i].type == ESGS_PROP_GEM ||
			activityItem.rewardVec[i].type == ESGS_PROP_MATERIAL)
		{
			SPropertyAttr prop;
			BYTE maxStack = 0;
			memset(&prop, 0, sizeof(SPropertyAttr));
			if (MLS_OK == GetGameCfgFileMan()->GetPropertyAttr(activityItem.rewardVec[i].type, activityItem.rewardVec[i].propId, prop, maxStack))
			{
				prop.num = activityItem.rewardVec[i].num;
				propVec.push_back(prop);
			}
		}
		else
		{
			SPropertyAttr prop;
			memset(&prop, 0, sizeof(SPropertyAttr));
			prop.propId = activityItem.rewardVec[i].propId;
			prop.num = activityItem.rewardVec[i].num;
			prop.type = activityItem.rewardVec[i].type;
			propVec.push_back(prop);
		}
	}

	EmailCleanMan::SSyncEmailSendContent email;
	email.gold = gold;
	email.rpcoin = rpcoin;
	email.exp = exp;
	email.vit = vit;
	email.receiverUserId = req->userId;
	email.receiverRoleId = req->roleId;
	email.type = ESGS_EMAIL_ACTIVITY_REWARD;
	email.pGBSenderNick = "ϵͳ";
	email.pGBCaption = "�����";
	email.pGBBody = "�����";
	if (propVec.size() > 0)
	{
		email.attachmentsNum = propVec.size();
		email.pAttachments = propVec.data();
	}
	GetEmailCleanMan()->AsyncSendEmail(email);

	//ˢ������ȡ�
	GetActitvityMan()->RefreshReceivedActivity(req->roleId, req->activityId);
	return retCode;
}

int MLogicProcessMan::OnQueryCurOpenChapter(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerQueryCurOpenChapterReq) == nDatalen);
	if (sizeof(SLinkerQueryCurOpenChapterReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_CUR_OPEN_CHAPTER);

	SLinkerQueryCurOpenChapterReq* req = (SLinkerQueryCurOpenChapterReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQueryCurOpenChapterRes* pChapterRes = (SQueryCurOpenChapterRes*)(buf + nLen);
	pChapterRes->chapterId = GetTaskFinHistoryMan()->GetCurrentOpenChapter(req->roleId);
	LogInfo(("�û�[%u]��ɫ[%u]��ȡ��ǰ�����½�[%d]", req->userId, req->roleId, pChapterRes->chapterId));

	pChapterRes->hton();
	nLen += sizeof(SQueryCurOpenChapterRes);
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_QUERY_CUR_OPEN_CHAPTER, buf, nLen);
}

int MLogicProcessMan::OnReceiveCashCodeReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerReceiveCashCodeRewardReq) == nDatalen);
	if (sizeof(SLinkerReceiveCashCodeRewardReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_RECEIVE_CASH_CODE_REWARD);

	SLinkerReceiveCashCodeRewardReq* req = (SLinkerReceiveCashCodeRewardReq*)pData;
	req->ntoh();

	UINT16 rewardId = 0;
	int retCode = GetActitvityMan()->VerifyCashCode(req->cashCode, req->roleId, rewardId);
	LogInfo(("�û�[%u]��ɫ[%u]��ȡ�һ���[%s]����, ������[%d]", req->userId, req->roleId, req->cashCode, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_RECEIVE_CASH_CODE_REWARD);

	//��ȡ������Ʒ
	GiftBagCfg::SGiftBagItem giftItem;
	retCode = GetGameCfgFileMan()->GetGiftBagItem(rewardId, giftItem);
	gwsSession->SendBasicResponse(pHead, retCode, MLS_RECEIVE_CASH_CODE_REWARD);
	if (retCode != MLS_OK)
		return retCode;

	//�����ʼ�
	std::vector<SPropertyAttr> propVec;
	BYTE nSize = (BYTE)giftItem.propVec.size();
	for (BYTE i = 0; i < nSize; ++i)
	{
		BYTE maxStack = 0;
		SPropertyAttr propAttr;
		memset(&propAttr, 0, sizeof(SPropertyAttr));
		int retCode = GetGameCfgFileMan()->GetPropertyAttr(giftItem.propVec[i].type, giftItem.propVec[i].propId, propAttr, maxStack);
		if (retCode != MLS_OK)
			continue;

		propAttr.num = giftItem.propVec[i].num;
		propVec.push_back(propAttr);
	}

	EmailCleanMan::SSyncEmailSendContent email;
	email.attachmentsNum = propVec.size();
	email.exp = giftItem.exp;
	email.gold = giftItem.gold;
	email.pAttachments = propVec.data();
	email.pGBBody = giftItem.desc;//"�һ��뽱��"; 20151222 hxw �޸�Ϊ�����Ϣ
	email.pGBCaption = giftItem.name;//"�һ��뽱��"; 20151222 hxw �޸�Ϊ�����Ϣ
	email.pGBSenderNick = "ϵͳ";
	email.receiverRoleId = req->roleId;
	email.receiverUserId = req->userId;
	email.rpcoin = giftItem.rpcoin;
	email.senderId = 0;
	email.type = ESGS_EMAIL_GIFTBAG_REWARD;
	email.vit = giftItem.vit;
	GetEmailCleanMan()->AsyncSendEmail(email);
	return retCode;
}

int MLogicProcessMan::OnInstSweep(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerInstSweepReq) == nDatalen);
	if (sizeof(SLinkerInstSweepReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_INST_SWEEP);

	SLinkerInstSweepReq* req = (SLinkerInstSweepReq*)pData;
	req->ntoh();

	//�Ƿ��и��������ʼ�
	if (GetConfigFileMan()->IsCheckInstDropEmail() && GetEmailCleanMan()->IsHasInstDropEmail(req->roleId))
		return MLS_HAS_INST_DROP_EMAIL;

	BOOL isUpgrade = false;
	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SInstSweepRes* pSweepRes = (SInstSweepRes*)(buf + nLen);
	int retCode = GetRolesInfoMan()->InstSweep(req->userId, req->roleId, req->instId, pSweepRes, isUpgrade);
	LogInfo(("�û�[%u]��ɫ[%u]ɨ������[%d]����, ������[%d]", req->userId, req->roleId, req->instId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_INST_SWEEP);

	BYTE linkerId = pHead->sourceId;
	nLen += sizeof(SInstSweepRes)+(pSweepRes->num - 1) * sizeof(SPropertyAttr);
	pSweepRes->hton();
	gwsSession->SendResponse(pHead, MLS_OK, MLS_INST_SWEEP, buf, nLen);

	//ˢ����ɸ���
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_FIN_INST);

	//����ˢ��
	if (!isUpgrade)
		return retCode;

	/* zpy �ɾ�ϵͳ���� */
	GetAchievementMan()->OnRoleUpgrade(req->userId, req->roleId);
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_UPGRADE);

	//����֪ͨ
	nLen = GWS_COMMON_REPLY_LEN;
	SRoleUpgradeNotify* pUpgradeNotify = (SRoleUpgradeNotify*)(buf + nLen);
	retCode = GetRolesInfoMan()->GetRoleUpgradeNotify(req->userId, req->roleId, pUpgradeNotify);
	if (retCode == MLS_OK)
	{
		nLen += sizeof(SRoleUpgradeNotify)+(pUpgradeNotify->skillNum - 1) * sizeof(SSkillMoveInfo);
		pUpgradeNotify->hton();
		gwsSession->SendResponse(linkerId, req->userId, MLS_OK, MLS_ROLE_UPGRADE_NOTIFY, buf, nLen);
	}

	return retCode;
}

int MLogicProcessMan::OnQueryUserUnlockContent(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerQueryUnlockContentReq) == nDatalen);
	if (sizeof(SLinkerQueryUnlockContentReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_UNLOCK_CONTENT);

	SLinkerQueryUnlockContentReq* req = (SLinkerQueryUnlockContentReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQueryUnlockContentRes* pUnlockContentRes = (SQueryUnlockContentRes*)(buf + nLen);
	int retCode = GetUnlockContentMan()->QueryUnlockContent(req->userId, req->roleId, req->type, pUnlockContentRes);
	LogInfo(("�û�[%u]��ɫ[%u]��ѯ�ѽ�������, ������[%d]", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_QUERY_UNLOCK_CONTENT);

	//��ȡδ��������
	std::vector<UINT16> unlockedItemsVec;
	GetGameCfgFileMan()->GetUnlockedItems(req->type, unlockedItemsVec);
	UINT16 maxNum = (MAX_RES_USER_BYTES - member_offset(SQueryUnlockContentRes, num, data)) / sizeof(UINT16);
	int n = (int)unlockedItemsVec.size();
	for (int i = 0; i < n; ++i)
	{
		if (pUnlockContentRes->num >= maxNum)
			break;
		pUnlockContentRes->data[pUnlockContentRes->num] = unlockedItemsVec[i];
		++pUnlockContentRes->num;
	}

	pUnlockContentRes->userId = req->userId;
	pUnlockContentRes->roleId = req->roleId;
	nLen += sizeof(SQueryUnlockContentRes)+(pUnlockContentRes->num - 1) * sizeof(UINT16);
	pUnlockContentRes->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_QUERY_UNLOCK_CONTENT, buf, nLen);
}

int MLogicProcessMan::OnLcsQueryUserUnlockContent(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerQueryUnlockContentReq) == nDatalen);
	if (sizeof(SLinkerQueryUnlockContentReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, LICENCE_QUERY_UNLOCK_JOBS);

	SLinkerQueryUnlockContentReq* req = (SLinkerQueryUnlockContentReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQueryUnlockContentRes* pUnlockContentRes = (SQueryUnlockContentRes*)(buf + nLen);
	int retCode = GetUnlockContentMan()->QueryUnlockContent(req->userId, req->roleId, req->type, pUnlockContentRes);
	LogInfo(("��֤�������û�[%u]��ɫ[%u]��ѯ�ѽ�������, ������[%d]", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, LICENCE_QUERY_UNLOCK_JOBS);

	//��ȡδ��������
	std::vector<UINT16> unlockedItemsVec;
	GetGameCfgFileMan()->GetUnlockedItems(req->type, unlockedItemsVec);
	UINT16 maxNum = (MAX_RES_USER_BYTES - member_offset(SQueryUnlockContentRes, num, data)) / sizeof(UINT16);
	int n = (int)unlockedItemsVec.size();
	for (int i = 0; i < n; ++i)
	{
		if (pUnlockContentRes->num >= maxNum)
			break;
		pUnlockContentRes->data[pUnlockContentRes->num] = unlockedItemsVec[i];
		++pUnlockContentRes->num;
	}

	pUnlockContentRes->userId = req->userId;
	pUnlockContentRes->roleId = req->roleId;
	nLen += sizeof(SQueryUnlockContentRes)+(pUnlockContentRes->num - 1) * sizeof(UINT16);
	pUnlockContentRes->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, LICENCE_QUERY_UNLOCK_JOBS, buf, nLen);
}

int MLogicProcessMan::OnUnlockContent(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerContentUnlockReq) == nDatalen);
	if (sizeof(SLinkerContentUnlockReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_CONTENT_UNLOCK);

	SLinkerContentUnlockReq* req = (SLinkerContentUnlockReq*)pData;
	req->ntoh();

	UnlockCfg::SUnlockItem* pUnloclItem = NULL;
	int retCode = GetGameCfgFileMan()->GetUnlockItem(req->type, req->unlockId, pUnloclItem);
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_CONTENT_UNLOCK);

	//�Ƿ�����
	if (!pUnloclItem->isLock)
		return gwsSession->SendBasicResponse(pHead, MLS_OK, MLS_CONTENT_UNLOCK);

	retCode = GetRolesInfoMan()->UnlockContent(req->userId, req->roleId, req->type, req->unlockId);
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_CONTENT_UNLOCK);

	retCode = GetUnlockContentMan()->UnlockContent(req->userId, req->roleId, req->type, req->unlockId);
	LogInfo(("�û�[%u]��ɫ[%u]��������[%d : %d], ������[%d]", req->userId, req->roleId, req->type, req->unlockId, retCode));

	if (pUnloclItem->gold > 0)
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_GOLD);

	if (pUnloclItem->rpcoin > 0)
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_CASH);

	return gwsSession->SendBasicResponse(pHead, retCode, MLS_CONTENT_UNLOCK);
}

int MLogicProcessMan::OnGetRouletteRemainReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerGetRoulettePropReq) == nDatalen);
	if (sizeof(SLinkerGetRoulettePropReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_GET_ROULETTE_REMAIN_REWARD);

	SLinkerGetRoulettePropReq* req = (SLinkerGetRoulettePropReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SGetRoulettePropRes* pRoulettePropRes = (SGetRoulettePropRes*)(buf + nLen);
	GetActitvityMan()->GetRouletteReward(req->roleId, pRoulettePropRes);
	LogInfo(("�û�[%u]��ɫ[%u]��ȡ����ʣ�ཱƷ", req->userId, req->roleId));

	nLen += sizeof(SGetRoulettePropRes)+(pRoulettePropRes->num - 1) * sizeof(SRoulettePropItem);
	pRoulettePropRes->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_GET_ROULETTE_REMAIN_REWARD, buf, nLen);
}

int MLogicProcessMan::OnExtractRouletteReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerExtractRouletteRewardReq) == nDatalen);
	if (sizeof(SLinkerExtractRouletteRewardReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_EXTRACT_ROULETTE_REWARD);

	SLinkerExtractRouletteRewardReq* req = (SLinkerExtractRouletteRewardReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SExtractRouletteRewardRes* pExtractPropRes = (SExtractRouletteRewardRes*)(buf + nLen);

	//��ȡ����ʣ�ཱƷ
	UINT16 extractNum = 0;
	std::vector<RouletteCfg::SRoulettePropCfgItem> itemVec;
	GetActitvityMan()->GetRouletteReward(req->roleId, extractNum, itemVec);
	pExtractPropRes->getNum = SGSU16Add(extractNum, 1);
	if (itemVec.empty())
		return gwsSession->SendBasicResponse(pHead, MLS_ALREADY_FIN_EXTRACT, MLS_EXTRACT_ROULETTE_REWARD);

	//��ȡ��Ʒ
	int si = itemVec.size();
	if (si <= 1)
		return gwsSession->SendBasicResponse(pHead, MLS_ALREADY_FIN_EXTRACT, MLS_EXTRACT_ROULETTE_REWARD);

	//�����ܸ���
	std::vector<UINT16> chanceVec;
	UINT16 nTotalChance = 0;
	for (int i = 1; i < si; ++i)
	{
		nTotalChance = SGSU16Add(nTotalChance, itemVec[i].chance);
		chanceVec.push_back(nTotalChance);
	}
	//�������
	int nIdx = 0;
	int nRandChance = rand() % nTotalChance;
	for (int i = 0; i < si - 1; ++i)
	{
		if (nRandChance < chanceVec[i])
		{
			nIdx = i + 1;
			break;
		}
	}
	ASSERT(nIdx >= 1 && nIdx < itemVec.size());
	if (nIdx <= 0)
		return gwsSession->SendBasicResponse(pHead, MLS_ALREADY_FIN_EXTRACT, MLS_EXTRACT_ROULETTE_REWARD);

	BOOL isUpgrade = false;
	BYTE linkerId = pHead->sourceId;
	SRoulettePropItem rewardItem;
	rewardItem.id = itemVec[nIdx].id;
	rewardItem.num = itemVec[nIdx].num;
	rewardItem.propId = itemVec[nIdx].propId;
	rewardItem.type = itemVec[nIdx].type;
	int retCode = GetRolesInfoMan()->ExtractRouletteReward(req->userId, req->roleId, rewardItem, pExtractPropRes, isUpgrade);
	LogInfo(("�û�[%u]��ɫ[%u]���̳齱[%d], ������[%d]", req->userId, req->roleId, rewardItem.id, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, MLS_ALREADY_FIN_EXTRACT, MLS_EXTRACT_ROULETTE_REWARD);

	pExtractPropRes->id = rewardItem.id;
	UINT16 getNum = pExtractPropRes->getNum;
	nLen += sizeof(SExtractRouletteRewardRes)+(pExtractPropRes->num - 1) * sizeof(SPropertyAttr);
	pExtractPropRes->hton();
	gwsSession->SendResponse(pHead, MLS_OK, MLS_EXTRACT_ROULETTE_REWARD, buf, nLen);

	//ȥ�������ϱ���ȡ��Ʒ
	std::vector<RouletteCfg::SRoulettePropCfgItem>::iterator it = itemVec.begin();
	for (; it != itemVec.end(); ++it)
	{
		if (rewardItem.id != it->id)
			continue;

		itemVec.erase(it);
		break;
	}

	//ˢ�½�ɫ����
	switch (rewardItem.type)
	{
	case ESGS_PROP_EQUIP:			//װ��
	case ESGS_PROP_GEM:				//��ʯ
	case ESGS_PROP_MATERIAL:		//����
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_BACKPACK);
		break;
	case ESGS_PROP_GOLD:		//��Ϸ��
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_GOLD);
		break;
	case ESGS_PROP_TOKEN:		//����
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_CASH);
		break;
	case ESGS_PROP_VIT:			//����
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_VIT);
		break;
	case ESGS_PROP_FRAGMENT:	//��Ƭ
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_FRAGMENT);
		break;
	case ESGS_PROP_CHAIN_SOUL_FRAGMENT:	//������Ƭ
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_CHAINSOUL_FRAGMENT);
		break;
	case ESGS_PROP_MAGIC:		//ħ����
		GetPermanenDataMan()->InputUpdatePermanenDataInfo(req->userId, req->roleId, PERMANENT_MAGIC);
		break;
	case ESGS_PROP_DRESS:		//ʱװ
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_DRESS);
		break;
	default:
		break;
	}

	//�жϻ�ȡ������Ƿ�����
	if (rewardItem.type == ESGS_PROP_EXP)
	{
		if (!isUpgrade)
			GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_EXP);		//���¾���
		else
		{
			/* zpy �ɾ�ϵͳ���� */
			GetAchievementMan()->OnRoleUpgrade(req->userId, req->roleId);

			//ˢ�½�ɫ����
			GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_UPGRADE);

			//����֪ͨ
			nLen = GWS_COMMON_REPLY_LEN;
			SRoleUpgradeNotify* pUpgradeNotify = (SRoleUpgradeNotify*)(buf + nLen);
			retCode = GetRolesInfoMan()->GetRoleUpgradeNotify(req->userId, req->roleId, pUpgradeNotify);
			if (retCode == MLS_OK)
			{
				nLen += sizeof(SRoleUpgradeNotify)+(pUpgradeNotify->skillNum - 1) * sizeof(SSkillMoveInfo);
				pUpgradeNotify->hton();
				gwsSession->SendResponse(linkerId, req->userId, MLS_OK, MLS_GOLD_UPDATE_NOTIFY, buf, nLen);
			}
		}
	}

	ASSERT(itemVec.size() >= 1);
	GetActitvityMan()->UpdateRoleRouletteReward(req->roleId, getNum, itemVec);
	return MLS_OK;
}

int MLogicProcessMan::OnUpdateRouletteBigReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerUpdateRouletteBigRewardReq) == nDatalen);
	if (sizeof(SLinkerUpdateRouletteBigRewardReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_UPDATE_ROULETTE_BIG_REWARD);

	SLinkerFetchRouletteBigRewardReq* req = (SLinkerFetchRouletteBigRewardReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SUpdateRouletteBigRewardRes* pBigRewardRes = (SUpdateRouletteBigRewardRes*)(buf + nLen);

	UINT32 spendToken = 0;
	RouletteCfg::SRoulettePropCfgItem bigReward;
	GetGameCfgFileMan()->GenerateRouletteBigReward(bigReward, spendToken);
	pBigRewardRes->bigReward.id = bigReward.id;
	pBigRewardRes->bigReward.num = bigReward.num;
	pBigRewardRes->bigReward.propId = bigReward.propId;
	pBigRewardRes->bigReward.type = bigReward.type;

	int retCode = GetRolesInfoMan()->UpdateRouletteBigReward(req->userId, req->roleId, spendToken, pBigRewardRes->rpcoin);
	LogInfo(("�û�[%u]��ɫ[%u]�������̴�, ������[%d]", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_UPDATE_ROULETTE_BIG_REWARD);

	//�������ݿ�
	GetActitvityMan()->UpdateRouletteBigReward(req->roleId, pBigRewardRes->bigReward);

	//ˢ�´���
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_CASH);

	nLen += sizeof(SUpdateRouletteBigRewardRes);
	pBigRewardRes->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_UPDATE_ROULETTE_BIG_REWARD, buf, nLen);
}

int MLogicProcessMan::OnFetchRouletteBigReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerFetchRouletteBigRewardReq) == nDatalen);
	if (sizeof(SLinkerFetchRouletteBigRewardReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_FETCH_ROULETTE_BIG_REWARD);

	SLinkerFetchRouletteBigRewardReq* req = (SLinkerFetchRouletteBigRewardReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SFetchRouletteBigRewardRes* pRewardRes = (SFetchRouletteBigRewardRes*)(buf + nLen);

	//��ȡ���̽�Ʒ
	UINT16 extractTimes = 0;
	SRoulettePropItem bigReward;
	int retCode = GetActitvityMan()->FetchRouletteBigReward(req->roleId, extractTimes, bigReward);
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_FETCH_ROULETTE_BIG_REWARD);

	BOOL isUpgrade = false;
	BYTE linkerId = pHead->sourceId;
	retCode = GetRolesInfoMan()->FetchRouletteBigReward(req->userId, req->roleId, bigReward, pRewardRes, isUpgrade);
	LogInfo(("�û�[%u]��ɫ[%u]��ȡ���̴�, ������[%d]", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_FETCH_ROULETTE_BIG_REWARD);

	//ˢ�½�ɫ����
	switch (bigReward.type)
	{
	case ESGS_PROP_EQUIP:			//װ��
	case ESGS_PROP_GEM:				//��ʯ
	case ESGS_PROP_MATERIAL:		//����
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_BACKPACK);
		break;
	case ESGS_PROP_GOLD:		//��Ϸ��
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_GOLD);
		break;
	case ESGS_PROP_TOKEN:		//����
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_CASH);
		break;
	case ESGS_PROP_VIT:			//����
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_VIT);
		break;
	case ESGS_PROP_FRAGMENT:	//��Ƭ
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_FRAGMENT);
		break;
	case ESGS_PROP_CHAIN_SOUL_FRAGMENT:	//������Ƭ
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_CHAINSOUL_FRAGMENT);
		break;
	case ESGS_PROP_MAGIC:		//ħ����
		GetPermanenDataMan()->InputUpdatePermanenDataInfo(req->userId, req->roleId, PERMANENT_MAGIC);
		break;
	case ESGS_PROP_DRESS:		//ʱװ
		GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_DRESS);
		break;
	default:
		break;
	}

	//�жϻ�ȡ������Ƿ�����
	if (bigReward.type == ESGS_PROP_EXP)
	{
		if (!isUpgrade)
			GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_EXP);		//���¾���
		else
		{
			/* zpy �ɾ�ϵͳ���� */
			GetAchievementMan()->OnRoleUpgrade(req->userId, req->roleId);

			//ˢ�½�ɫ����
			GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_UPGRADE);

			//����֪ͨ
			nLen = GWS_COMMON_REPLY_LEN;
			SRoleUpgradeNotify* pUpgradeNotify = (SRoleUpgradeNotify*)(buf + nLen);
			retCode = GetRolesInfoMan()->GetRoleUpgradeNotify(req->userId, req->roleId, pUpgradeNotify);
			if (retCode == MLS_OK)
			{
				nLen += sizeof(SRoleUpgradeNotify)+(pUpgradeNotify->skillNum - 1) * sizeof(SSkillMoveInfo);
				pUpgradeNotify->hton();
				gwsSession->SendResponse(linkerId, req->userId, MLS_OK, MLS_GOLD_UPDATE_NOTIFY, buf, nLen);
			}
		}
	}

	//�����������̽�Ʒ
	std::vector<RouletteCfg::SRoulettePropCfgItem> itemVec;
	GetGameCfgFileMan()->GenerateRouletteReward(itemVec);
	GetActitvityMan()->UpdateRoleRouletteReward(req->roleId, extractTimes, itemVec);

	//�����������
	nLen += sizeof(SFetchRouletteBigRewardRes)+(pRewardRes->num - 1) * sizeof(SPropertyAttr);
	pRewardRes->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_FETCH_ROULETTE_BIG_REWARD, buf, nLen);
}

int MLogicProcessMan::OnFetchRechargeOrder(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerFetchRechargeOrderReq) == nDatalen);
	if (sizeof(SLinkerFetchRechargeOrderReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_FETCH_RECHARGE_ORDER);

	SLinkerFetchRechargeOrderReq* req = (SLinkerFetchRechargeOrderReq*)pData;
	req->ntoh();

	RechargeCfg::SRechargeCfgAttr cashAttr;
	int retCode = GetGameCfgFileMan()->GetRechargeAttr(req->rechargeId, cashAttr);
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_FETCH_RECHARGE_ORDER);

	//��ֵid������Ҳ�ƥ��
	if (req->rmb != cashAttr.rmb)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_CHASH_ID, MLS_FETCH_RECHARGE_ORDER);

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SFetchRechargeOrderRes* pOrderRes = (SFetchRechargeOrderRes*)(buf + nLen);
	retCode = GetRechargeMan()->GenerateRechargeOrder(req->userId, req->roleId, req->partId, req->rechargeId, cashAttr.rmb, pOrderRes->orderNO);
	LogInfo(("�û�[%u]��ɫ[%u]��ȡ��ֵ����[%s], ������[%d]", req->userId, req->roleId, pOrderRes->orderNO, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_FETCH_RECHARGE_ORDER);

	nLen += sizeof(SFetchRechargeOrderRes);
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_FETCH_RECHARGE_ORDER, buf, nLen);
}

int MLogicProcessMan::OnGetRechargeToken(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerGetRechargeTokenReq) == nDatalen);
	if (sizeof(SLinkerGetRechargeTokenReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_GET_RECHARGE_TOKEN);

	SLinkerGetRechargeTokenReq* req = (SLinkerGetRechargeTokenReq*)pData;
	req->ntoh();

	BYTE status = 0;
	UINT32 rmb = 0;
	UINT16 rechargeId = 0;
	int retCode = GetRechargeMan()->GetRechargeInfo(req->userId, req->roleId, req->orderNO, rechargeId, rmb, status);
	LogInfo(("�û�[%u]��ɫ[%u]��ȡ����[%s]��ֵ����, ������[%d]", req->userId, req->roleId, req->orderNO, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_GET_RECHARGE_TOKEN);

	//��ȡ��ֵid��Ϣ
	RechargeCfg::SRechargeCfgAttr cashAttr;
	GetGameCfgFileMan()->GetRechargeAttr(rechargeId, cashAttr);
	ASSERT(rmb == cashAttr.rmb);

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SGetRechargeTokenRes* pRechargeRes = (SGetRechargeTokenRes*)(buf + nLen);
	char szRoleNick[33] = { 0 };
	SRechargeRes rechargeRes;
	retCode = GetRolesInfoMan()->RechargeToken(req->userId, req->roleId, rechargeId, &rechargeRes, szRoleNick);
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_GET_RECHARGE_TOKEN);

	//ˢ��money
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_CASH);

	pRechargeRes->rpcoin = rechargeRes.rpcoin;
	pRechargeRes->hton();
	nLen += sizeof(SGetRechargeTokenRes);
	gwsSession->SendResponse(pHead, MLS_OK, MLS_GET_RECHARGE_TOKEN, buf, nLen);

	//�Ƿ��׳䲢��������
	if (cashAttr.firstGive > 0)
	{
		if (GetRechargeMan()->IsFirstRechargeId(req->userId, req->roleId, rechargeId))
		{
			EmailCleanMan::SSyncEmailSendContent email;
			email.type = ESGS_EMAIL_FIRST_RECHARGE;
			email.receiverRoleId = req->roleId;	//���ս�ɫid
			email.receiverUserId = req->userId;	//�����˺�id
			email.pGBSenderNick = "ϵͳ";		//�������ǳ�
			email.pGBCaption = "�׳�����";		//����
			email.rpcoin = cashAttr.firstGive;	//�׳����ʹ���
			char szBody[64] = { 0 };
			sprintf(szBody, "�״γ�ֵ, ��%u��%u", cashAttr.rpcoin, cashAttr.firstGive);
			email.pGBBody = szBody;
			GetEmailCleanMan()->AsyncSendEmail(email);
		}
		else
			cashAttr.firstGive = 0;
	}

	//���±��ض���״̬
	GetRechargeMan()->UpdateLocalPartOrderInfo(req->orderNO, status, cashAttr.rpcoin, cashAttr.firstGive);
	return retCode;
}

int MLogicProcessMan::OnGetActiveInstChallengeTimes(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerGetActiveInstChallengeTimesReq) == nDatalen);
	if (sizeof(SLinkerGetActiveInstChallengeTimesReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_GET_ACTIVE_INST_CHALLENGE_TIMES);

	SLinkerGetActiveInstChallengeTimesReq* req = (SLinkerGetActiveInstChallengeTimesReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SGetActiveInstChallengeTimesRes* pChallengeTimesRes = (SGetActiveInstChallengeTimesRes*)(buf + nLen);
	GetInstFinHistoryMan()->GetAvtiveInstChallengeTimes(req->roleId, pChallengeTimesRes);
	LogInfo(("�û�[%u]��ɫ[%d]��ȡ�������ս����", req->userId, req->roleId));

	nLen += sizeof(SGetActiveInstChallengeTimesRes)+(pChallengeTimesRes->num - 1) * sizeof(SActiveInstItem);
	pChallengeTimesRes->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_GET_ACTIVE_INST_CHALLENGE_TIMES, buf, nLen);
}

int MLogicProcessMan::OnQueryVitExchange(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerQueryVitExchangeReq) == nDatalen);
	if (sizeof(SLinkerQueryVitExchangeReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_VIT_EXCHANGE);

	SLinkerQueryVitExchangeReq* req = (SLinkerQueryVitExchangeReq*)pData;
	req->ntoh();

	/* zpy �����һ� */
	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQueryVitExchangeRes* pVitExchange = (SQueryVitExchangeRes*)(buf + nLen);
	int retCode = GetRolesInfoMan()->QueryVitExchange(req->userId, req->roleId, pVitExchange);
	LogInfo(("�û�[%u]��ɫ[%d]��ѯ�����һ�����", req->userId, req->roleId));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_QUERY_VIT_EXCHANGE);

	nLen += sizeof(SQueryVitExchangeRes);
	pVitExchange->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_QUERY_VIT_EXCHANGE, buf, nLen);
}

int MLogicProcessMan::OnVitExchange(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerVitExchangeReq) == nDatalen);
	if (sizeof(SLinkerVitExchangeReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_VIT_EXCHANGE);

	SLinkerVitExchangeReq* req = (SLinkerVitExchangeReq*)pData;
	req->ntoh();

	/* zpy �����һ� */
	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SVitExchangeRes* pVitExchange = (SVitExchangeRes*)(buf + nLen);
	int retCode = GetRolesInfoMan()->VitExchange(req->userId, req->roleId, pVitExchange);
	LogInfo(("�û�[%u]��ɫ[%u]�����һ�, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_VIT_EXCHANGE);

	//ˢ�´���
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_CASH);

	//ˢ������
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_VIT);

	//ˢ�¶һ�����
	GetVitExchangeMan()->RefeshRoleVitExchange(req->roleId, pVitExchange->exchangedTimes);

	nLen += sizeof(SVitExchangeRes);
	pVitExchange->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_VIT_EXCHANGE, buf, nLen);
}

/* zpy ��ѯÿ��ǩ�� */
int MLogicProcessMan::OnQueryRoleDailySign(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerQueryRoleDailySignReq) == nDatalen);
	if (sizeof(SLinkerQueryRoleDailySignReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_ROLE_DAILY_SIGN);

	SLinkerQueryRoleDailySignReq* req = (SLinkerQueryRoleDailySignReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQueryRoleDailySignRes* pQueryRoleDailySign = (SQueryRoleDailySignRes*)(buf + nLen);
	int retCode = GetActitvityMan()->QueryRoleDailySign(req->roleId, pQueryRoleDailySign);
	LogInfo(("�û�[%u]��ɫ[%u]��ѯÿ��ǩ��, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_QUERY_ROLE_DAILY_SIGN);

	nLen += sizeof(SQueryRoleDailySignRes)-sizeof(BYTE)+pQueryRoleDailySign->num * sizeof(BYTE);
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_QUERY_ROLE_DAILY_SIGN, buf, nLen);
}

/* zpy ÿ��ǩ�� */
int MLogicProcessMan::OnRoleDailySign(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerRoleDailySignReq) == nDatalen);
	if (sizeof(SLinkerRoleDailySignReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_ROLE_DAILY_SIGN);

	SLinkerRoleDailySignReq* req = (SLinkerRoleDailySignReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SReceiveRewardRefresh refresh_info;
	memset(&refresh_info, 0, sizeof(SReceiveRewardRefresh));
	SRoleDailySignRes* pQueryRoleDailySign = (SRoleDailySignRes*)(buf + nLen);
	int retCode = GetActitvityMan()->OnRoleDailySign(req->userId, req->roleId, pQueryRoleDailySign, &refresh_info);
	LogInfo(("�û�[%u]��ɫ[%u]ÿ��ǩ��, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_ROLE_DAILY_SIGN);

	// ˢ�����ݿ�
	ReceiveRewardRefresh(req->userId, req->roleId, refresh_info);

	nLen += sizeof(SRoleDailySignRes)+(pQueryRoleDailySign->num - 1) * sizeof(SPropertyAttr);
	pQueryRoleDailySign->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_ROLE_DAILY_SIGN, buf, nLen);
}

//add by hxw 20151028
//��ѯ�ȼ����а���Ϣ
int MLogicProcessMan::QueryLvRankInfo(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	const UINT32 _CMD_TYPE_ = MSL_QUERY_LV_RANK_INFO;
	ASSERT(sizeof(SLinkerQueryLvInfoReq) == nDatalen);
	if (sizeof(SLinkerQueryLvInfoReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, _CMD_TYPE_);

	SLinkerQueryLvInfoReq* req = (SLinkerQueryLvInfoReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;

	SQueryInsnumRanksInfo* pQueryRank = (SQueryInsnumRanksInfo*)(buf + nLen);

	int retCode = GetRankRewardMan()->QueryLvRankInfo(pQueryRank);
	LogInfo(("�û�[%u]��ɫ[%u]��ѯ�ȼ��������а�, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, _CMD_TYPE_);
	//SLinkerQueryRankRewardReq

	nLen += sizeof(SQueryInsnumRanksInfo);

	pQueryRank->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, _CMD_TYPE_, buf, nLen);
}
//end

//add by hxw 20151006
//��ѯ�ȼ����н�����Ϣ
int MLogicProcessMan::QueryLvRank(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	const UINT32 _CMD_TYPE_ = MSL_QUERY_LV_RANK;
	ASSERT(sizeof(SLinkerQueryLvRankRewardReq) == nDatalen);
	if (sizeof(SLinkerQueryLvRankRewardReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, _CMD_TYPE_);

	SLinkerQueryLvRankRewardReq* req = (SLinkerQueryLvRankRewardReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;

	SQueryRankRewardGet* pQueryRank = (SQueryRankRewardGet*)(buf + nLen);

	int retCode = GetRankRewardMan()->QueryLvRank(req->roleId, pQueryRank);
	LogInfo(("�û�[%u]��ɫ[%u]��ѯ�弶����, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, _CMD_TYPE_);
	//SLinkerQueryRankRewardReq

	nLen += sizeof(SQueryRankRewardGet);

	pQueryRank->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, _CMD_TYPE_, buf, nLen);
}


// ��ѯ�Ƿ���ȡ������ͼ��������
int MLogicProcessMan::QueryInsnumRank(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	const UINT32 _CMD_TYPE_ = MSL_QUERY_INNUM_RANK;
	ASSERT(sizeof(SLinkerQueryInsnumRankRewardReq) == nDatalen);
	if (sizeof(SLinkerQueryInsnumRankRewardReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, _CMD_TYPE_);

	SLinkerQueryInsnumRankRewardReq* req = (SLinkerQueryInsnumRankRewardReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;

	SQueryRankRewardGet* pQueryRank = (SQueryRankRewardGet*)(buf + nLen);

	int retCode = GetRankRewardMan()->QueryInsnumRank(req->roleId, pQueryRank);
	LogInfo(("�û�[%u]��ɫ[%u]��ѯˢͼ������ȡ���, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, _CMD_TYPE_);
	//SLinkerQueryRankRewardReq

	nLen += sizeof(SQueryRankRewardGet);

	pQueryRank->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, _CMD_TYPE_, buf, nLen);
}


//��ѯ������ͼ���а���Ϣ
int MLogicProcessMan::QueryInsnumRankInfo(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	const UINT32 _CMD_TYPE_ = MSL_QUERY_INNUM_RANK_INFO;
	ASSERT(sizeof(SLinkerQueryInsnumRankInfoReq) == nDatalen);
	if (sizeof(SLinkerQueryInsnumRankInfoReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, _CMD_TYPE_);

	SLinkerQueryInsnumRankInfoReq* req = (SLinkerQueryInsnumRankInfoReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;

	SQueryInsnumRanksInfo* pQueryRank = (SQueryInsnumRanksInfo*)(buf + nLen);

	int retCode = GetRankRewardMan()->QueryInsnumRankInfo(pQueryRank);
	LogInfo(("�û�[%u]��ɫ[%u]��ѯˢͼ�������а�, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, _CMD_TYPE_);
	//SLinkerQueryRankRewardReq

	nLen += sizeof(SQueryInsnumRanksInfo);

	pQueryRank->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, _CMD_TYPE_, buf, nLen);
}


//add by hxw 20151006
// ��ȡ�ȼ����н���
int MLogicProcessMan::GetLvRankReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	const UINT32 _CMD_TYPE_ = MSL_GET_LV_RANK_REWARD;
	ASSERT(sizeof(SLinkerGetLvRankRewardReq) == nDatalen);
	if (sizeof(SLinkerGetLvRankRewardReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, _CMD_TYPE_);
	SLinkerGetLvRankRewardReq* req = (SLinkerGetLvRankRewardReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SReceiveRewardRefresh refresh_info;
	memset(&refresh_info, 0, sizeof(SReceiveRewardRefresh));

	SGetRankRewardRes* rankres = (SGetRankRewardRes*)(buf + nLen);

	int retCode = GetRankRewardMan()->GetLvRankReward(req->userId, req->roleId, rankres, &refresh_info);
	std::string str;
	GetGameCfgFileMan()->GetRankLogs(str, __Stronger_Reward_Type);
	LogInfo(("�û�[%u]��ɫ[%u]  %s, ������[%d]!", req->userId, req->roleId, str.c_str(), retCode));

	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, _CMD_TYPE_);

	// ˢ�����ݿ�
	ReceiveRewardRefresh(req->userId, req->roleId, refresh_info);

	nLen += sizeof(SGetRankRewardRes)+(rankres->num - 1) * sizeof(SPropertyAttr);
	rankres->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, _CMD_TYPE_, buf, nLen);
}

int MLogicProcessMan::GetInsnumRankReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	const UINT32 _CMD_TYPE_ = MSL_GET_INNUM_REWARD;
	ASSERT(sizeof(SLinkerGetInsnumRankRewardReq) == nDatalen);
	if (sizeof(SLinkerGetInsnumRankRewardReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, _CMD_TYPE_);
	SLinkerGetInsnumRankRewardReq* req = (SLinkerGetInsnumRankRewardReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SReceiveRewardRefresh refresh_info;
	memset(&refresh_info, 0, sizeof(SReceiveRewardRefresh));

	SGetRankRewardRes* rankres = (SGetRankRewardRes*)(buf + nLen);

	int retCode = GetRankRewardMan()->GetInsnumRankReward(req->userId, req->roleId, rankres, &refresh_info);
	std::string str;
	GetGameCfgFileMan()->GetRankLogs(str, __Stronger_Reward_Type);
	LogInfo(("�û�[%u]��ɫ[%u]  %s, ������[%d]!", req->userId, req->roleId, str.c_str(), retCode));

	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, _CMD_TYPE_);

	// ˢ�����ݿ�
	ReceiveRewardRefresh(req->userId, req->roleId, refresh_info);

	nLen += sizeof(SGetRankRewardRes)+(rankres->num - 1) * sizeof(SPropertyAttr);
	rankres->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, _CMD_TYPE_, buf, nLen);
}


//��ѯǿ�߽�����ս��)��ȡ���
int MLogicProcessMan::QueryStrongerInfo(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	const UINT32 _CMD_TYPE_ = MSL_QUERY_STRONGER_INFO;
	ASSERT(sizeof(SLinkerQueryStrongerInfoReq) == nDatalen);
	if (sizeof(SLinkerQueryStrongerInfoReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, _CMD_TYPE_);

	SLinkerQueryStrongerInfoReq* req = (SLinkerQueryStrongerInfoReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;

	SQueryStrongerInfos* pQueryRank = (SQueryStrongerInfos*)(buf + nLen);

	int retCode = GetRankRewardMan()->QueryStrongerInfo(req->userId, req->roleId, pQueryRank);
	LogInfo(("�û�[%u]��ɫ[%u]��ѯǿ�߽�����ȡ���, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, _CMD_TYPE_);
	//SLinkerQueryRankRewardReq

	nLen += sizeof(SQueryStrongerInfos);

	pQueryRank->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, _CMD_TYPE_, buf, nLen);
}
//��ȡǿ�߽���
int MLogicProcessMan::GetStrongerReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	const UINT32 _CMD_TYPE_ = MSL_GET_STRONGER_REWARD;
	ASSERT(sizeof(SLinkerGetStrongerReq) == nDatalen);
	if (sizeof(SLinkerGetStrongerReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, _CMD_TYPE_);

	SLinkerGetStrongerReq* req = (SLinkerGetStrongerReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SReceiveRewardRefresh refresh_info;
	memset(&refresh_info, 0, sizeof(SReceiveRewardRefresh));

	SGetRankRewardRes* rankres = (SGetRankRewardRes*)(buf + nLen);

	int retCode = GetRankRewardMan()->GetStrongerReward(req->userId, req->roleId, req->rewardId, rankres, &refresh_info);
	std::string str;
	GetGameCfgFileMan()->GetRankLogs(str, __Stronger_Reward_Type);
	LogInfo(("�û�[%u]��ɫ[%u]  %s, ������[%d]!", req->userId, req->roleId, str.c_str(), retCode));

	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, _CMD_TYPE_);

	// ˢ�����ݿ�
	ReceiveRewardRefresh(req->userId, req->roleId, refresh_info);

	nLen += sizeof(SGetRankRewardRes)+(rankres->num - 1) * sizeof(SPropertyAttr);
	rankres->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, _CMD_TYPE_, buf, nLen);
}
//end=======

//add by hxw 20151020

int MLogicProcessMan::QueryOnlineInfo(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	const UINT32 _CMD_TYPE_ = MSL_QUERY_ONLINE_INFO;
	ASSERT(sizeof(SLinkerQueryOnlineInfoReq) == nDatalen);
	if (sizeof(SLinkerQueryOnlineInfoReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, _CMD_TYPE_);

	SLinkerQueryOnlineInfoReq* req = (SLinkerQueryOnlineInfoReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;

	SQueryOnlineInfos* qInfo = (SQueryOnlineInfos*)(buf + nLen);

	int retCode = GetRankRewardMan()->QueryOnlineInfo(req->userId, req->roleId, qInfo);
	LogInfo(("�û�[%u]��ɫ[%u]��ѯ���߽�����ȡ���, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, _CMD_TYPE_);
	//SLinkerQueryRankRewardReq

	nLen += sizeof(SQueryOnlineInfos);

	qInfo->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, _CMD_TYPE_, buf, nLen);

}

//1020 ��ȡ���߽���
int MLogicProcessMan::GetOnlineReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	const UINT32 _CMD_TYPE_ = MSL_GET_ONLINE_REWARD;
	ASSERT(sizeof(SLinkerGetOnlineRewardReq) == nDatalen);
	if (sizeof(SLinkerGetOnlineRewardReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, _CMD_TYPE_);

	SLinkerGetOnlineRewardReq* req = (SLinkerGetOnlineRewardReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SReceiveRewardRefresh refresh_info;
	memset(&refresh_info, 0, sizeof(SReceiveRewardRefresh));

	SGetRankRewardRes* rankres = (SGetRankRewardRes*)(buf + nLen);

	int retCode = GetRankRewardMan()->GetOnlineReward(req->userId, req->roleId, req->rewardId, rankres, &refresh_info);
	std::string str;
	GetGameCfgFileMan()->GetRankLogs(str, __Online_Reward_Type);
	LogInfo(("�û�[%u]��ɫ[%u]  %s, ������[%d]!", req->userId, req->roleId, str.c_str(), retCode));

	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, _CMD_TYPE_);

	// ˢ�����ݿ�
	ReceiveRewardRefresh(req->userId, req->roleId, refresh_info);

	nLen += sizeof(SGetRankRewardRes)+(rankres->num - 1) * sizeof(SPropertyAttr);
	rankres->hton();
	return gwsSession->SendResponse(pHead, MLS_OK, _CMD_TYPE_, buf, nLen);
}
//end


/* zpy ��ѯ������Ƭ */
int MLogicProcessMan::OnQueryChainSoulFragment(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerQueryChainSoulFragmentReq) == nDatalen);
	if (sizeof(SLinkerQueryChainSoulFragmentReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_CHAIN_SOUL_FRAGMENT);

	SLinkerQueryChainSoulFragmentReq* req = (SLinkerQueryChainSoulFragmentReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQueryChainSoulFragmentRes *pChainSoulFragment = (SQueryChainSoulFragmentRes*)(buf + nLen);
	int retCode = GetRolesInfoMan()->QueryChainSoulFragment(req->userId, req->roleId, pChainSoulFragment);
	LogInfo(("�û�[%u]��ɫ[%u]��ѯ������Ƭ, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_QUERY_CHAIN_SOUL_FRAGMENT);

	pChainSoulFragment->hton();
	nLen += sizeof(SQueryChainSoulFragmentRes)+(pChainSoulFragment->num - 1) * sizeof(SChainSoulFragmentAttr);
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_QUERY_CHAIN_SOUL_FRAGMENT, buf, nLen);
}

/* zpy ��ѯ���겿�� */
int MLogicProcessMan::OnQueryChainSoulPos(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerQueryChainSoulPosReq) == nDatalen);
	if (sizeof(SLinkerQueryChainSoulPosReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_CHAIN_SOUL_POS);

	SLinkerQueryChainSoulPosReq* req = (SLinkerQueryChainSoulPosReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQueryChainSoulPosRes *pChainSoulPos = (SQueryChainSoulPosRes*)(buf + nLen);
	int retCode = GetRolesInfoMan()->QueryChainSoulPos(req->userId, req->roleId, pChainSoulPos);
	LogInfo(("�û�[%u]��ɫ[%u]��ѯ���겿��, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_QUERY_CHAIN_SOUL_POS);

	nLen += sizeof(SQueryChainSoulPosRes);
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_QUERY_CHAIN_SOUL_POS, buf, nLen);
}

/* zpy �������겿�� */
int MLogicProcessMan::OnChainSoulPosUpgrade(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerChainSoulPosUpgradeReq) == nDatalen);
	if (sizeof(SLinkerChainSoulPosUpgradeReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_CHAIN_SOUL_POS_UPGRADE);

	SLinkerChainSoulPosUpgradeReq* req = (SLinkerChainSoulPosUpgradeReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SChainSoulPosUpgradeRes *pChainSoulPosUpgrade = (SChainSoulPosUpgradeRes*)(buf + nLen);
	int retCode = GetRolesInfoMan()->ChainSoulPosUpgrade(req->userId, req->roleId, req->pos, pChainSoulPosUpgrade);
	LogInfo(("�û�[%u]��ɫ[%u]�������겿��, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_CHAIN_SOUL_POS_UPGRADE);

	//ˢ�����겿��
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_CHAINSOUL_POS);

	//ˢ��������Ƭ����
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_CHAINSOUL_FRAGMENT);

	pChainSoulPosUpgrade->hton();
	nLen += sizeof(SChainSoulPosUpgradeRes)+(pChainSoulPosUpgrade->num - 1) * sizeof(SChainSoulFragmentAttr);
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_CHAIN_SOUL_POS_UPGRADE, buf, nLen);
}

/* zpy ��������Boos */
int MLogicProcessMan::OnAttackWorldBoss(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerAttackWorldBossReq) == nDatalen);
	if (sizeof(SLinkerAttackWorldBossReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_ATTACK_WORLD_BOOS);

	SLinkerAttackWorldBossReq* req = (SLinkerAttackWorldBossReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SAttackWorldBossRes *pAttackWorldBoss = (SAttackWorldBossRes*)(buf + nLen);
	int retCode = GetWorldBossMan()->AttackWorldBoss(req->userId, req->roleId, req->hurt, pAttackWorldBoss);
	LogInfo(("�û�[%u]��ɫ[%u]��������Boss, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_ATTACK_WORLD_BOOS);

	pAttackWorldBoss->hton();
	nLen += sizeof(SAttackWorldBossRes);
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_ATTACK_WORLD_BOOS, buf, nLen);
}

/* zpy ��ѯ����Boos���а� */
int MLogicProcessMan::OnQueryWorldBossHurtRank(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerQueryWorldBossRankReq) == nDatalen);
	if (sizeof(SLinkerQueryWorldBossRankReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_WORLD_BOSS_RANK);

	SLinkerQueryWorldBossRankReq* req = (SLinkerQueryWorldBossRankReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQueryWorldBossRankRes *pWorldBossRank = (SQueryWorldBossRankRes*)(buf + nLen);
	int retCode = GetWorldBossMan()->QueryWorldBossHurtRank(req->userId, req->roleId, pWorldBossRank);
	LogInfo(("�û�[%u]��ɫ[%u]��ѯ����Boss����, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_QUERY_WORLD_BOSS_RANK);

	pWorldBossRank->hton();
	nLen += sizeof(SQueryWorldBossRankRes)+(pWorldBossRank->num - 1) * sizeof(SWorldBossRankItem);
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_QUERY_WORLD_BOSS_RANK, buf, nLen);
}

/* zpy ������Bossɱ�� */
int MLogicProcessMan::OnWasKilledByWorldBoss(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerWasKilledByWorldBossReq) == nDatalen);
	if (sizeof(SLinkerWasKilledByWorldBossReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_WASKILLED_BY_WORLDBOSS);

	SLinkerWasKilledByWorldBossReq* req = (SLinkerWasKilledByWorldBossReq*)pData;
	req->ntoh();

	int retCode = GetWorldBossMan()->WasKilledByWorldBoss(req->userId, req->roleId);
	LogInfo(("�û�[%u]��ɫ[%u]������Bossɱ��, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_WASKILLED_BY_WORLDBOSS);

	return MLS_OK;
}

/* zpy ������Boss�����и��� */
int MLogicProcessMan::OnResurrectionInWorldBoss(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerResurrectionInWorldBossReq) == nDatalen);
	if (sizeof(SLinkerResurrectionInWorldBossReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_RESURRECTION_IN_WORLDBOSS);

	SLinkerResurrectionInWorldBossReq* req = (SLinkerResurrectionInWorldBossReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SResurrectionInWorldBossRes *pResurrectionInWorldBoss = (SResurrectionInWorldBossRes*)(buf + nLen);
	int retCode = GetWorldBossMan()->ResurrectionInWorldBoss(req->userId, req->roleId, pResurrectionInWorldBoss);
	LogInfo(("�û�[%u]��ɫ[%u]������Boss�����Ѹ���, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_RESURRECTION_IN_WORLDBOSS);

	// ˢ�´���
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_CASH);

	pResurrectionInWorldBoss->hton();
	nLen += sizeof(SResurrectionInWorldBossRes);
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_RESURRECTION_IN_WORLDBOSS, buf, nLen);
}

/* zpy ��ѯ����Boss��Ϣ */
int MLogicProcessMan::OnQueryWorldBossInfo(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerQueryWorldBossInfoReq) == nDatalen);
	if (sizeof(SLinkerQueryWorldBossInfoReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_WORLDBOSS_INFO);

	SLinkerQueryWorldBossInfoReq* req = (SLinkerQueryWorldBossInfoReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQueryWorldBossInfoRes *pQueryWorldBossInfo = (SQueryWorldBossInfoRes*)(buf + nLen);
	int retCode = GetWorldBossMan()->QueryWorldBossInfo(req->userId, req->roleId, pQueryWorldBossInfo);
	LogInfo(("�û�[%u]��ɫ[%u]��ѯ����Boss��Ϣ, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_QUERY_WORLDBOSS_INFO);

	pQueryWorldBossInfo->hton();
	nLen += sizeof(SQueryWorldBossInfoRes);
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_QUERY_WORLDBOSS_INFO, buf, nLen);
}

/* zpy ��ȡ������Ϣ */
int MLogicProcessMan::OnGetLadderInfo(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerLadderRoleInfoReq) == nDatalen);
	if (sizeof(SLinkerLadderRoleInfoReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_GET_LADDER_ROLE_INFO);

	SLinkerLadderRoleInfoReq* req = (SLinkerLadderRoleInfoReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SLadderRoleInfoRes *pLadderRoleInfo = (SLadderRoleInfoRes*)(buf + nLen);
	int retCode = GetLadderRankMan()->GetChallengeInfo(req->userId, req->roleId, pLadderRoleInfo);
	LogInfo(("�û�[%u]��ɫ[%u]��ѯ������Ϣ, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_GET_LADDER_ROLE_INFO);

	pLadderRoleInfo->hton();
	nLen += sizeof(SLadderRoleInfoRes);
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_GET_LADDER_ROLE_INFO, buf, nLen);
}

/* zpy ����ƥ����� */
int MLogicProcessMan::OnLadderMatchingPlayer(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerMatchingLadderReq) == nDatalen);
	if (sizeof(SLinkerMatchingLadderReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_MATCHING_LADDER);

	SLinkerMatchingLadderReq* req = (SLinkerMatchingLadderReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SMatchingLadderRes *pMatchingLadder = (SMatchingLadderRes*)(buf + nLen);
	int retCode = GetLadderRankMan()->MatchingPlayer(req->userId, req->roleId, pMatchingLadder);
	LogInfo(("�û�[%u]��ɫ[%u]ƥ���������, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_MATCHING_LADDER);

	//ˢ�´���
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_CASH);

	//ˢ��������Ϣ
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_LADDER_INFO);

	//ˢ������ƥ����Ϣ
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_LADDER_MATCHING_INFO);

	pMatchingLadder->hton();
	nLen += sizeof(SMatchingLadderRes)+(pMatchingLadder->num - 1) * sizeof(SMatchingLadderItem);
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_MATCHING_LADDER, buf, nLen);
}

/* zpy ˢ������������Ϣ */
int MLogicProcessMan::OnRefreshLadderRankScope(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerRefreshLadderRankReq) == nDatalen);
	if (sizeof(SLinkerRefreshLadderRankReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_REFRESH_LADDER_RANK);

	SLinkerRefreshLadderRankReq* req = (SLinkerRefreshLadderRankReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SRefreshLadderRankRes *pLadderRank = (SRefreshLadderRankRes*)(buf + nLen);
	int retCode = GetLadderRankMan()->RefreshLadderRankScope(req->userId, req->roleId, pLadderRank);
	LogInfo(("�û�[%u]��ɫ[%u]ˢ����������, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_REFRESH_LADDER_RANK);



	//ˢ��������Ϣ
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_LADDER_INFO);

	//ˢ������ƥ����Ϣ
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_LADDER_MATCHING_INFO);

	pLadderRank->hton();
	nLen += sizeof(SRefreshLadderRankRes)+(pLadderRank->num - 1) * sizeof(SMatchingLadderItem);
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_REFRESH_LADDER_RANK, buf, nLen);
}

/* zpy ������ս��� */
int MLogicProcessMan::OnLadderChallengePlayer(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerChallengeLadderReq) == nDatalen);
	if (sizeof(SLinkerChallengeLadderReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_CHALLENGE_LADDER);

	SLinkerChallengeLadderReq* req = (SLinkerChallengeLadderReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SChallengeLadderRes *pChallengeLadder = (SChallengeLadderRes*)(buf + nLen);
	int retCode = GetLadderRankMan()->ChallengePlayer(req->userId, req->roleId, req->target_rank, pChallengeLadder);
	LogInfo(("�û�[%u]��ɫ[%u]��ս��������, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_CHALLENGE_LADDER);

	//ˢ��������Ϣ
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_LADDER_INFO);

	pChallengeLadder->hton();
	nLen += sizeof(SChallengeLadderRes);
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_CHALLENGE_LADDER, buf, nLen);
}

/* zpy ��������CDʱ�� */
int MLogicProcessMan::OnEliminateLadderCoolingTime(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerEliminateLadderCDTimeReq) == nDatalen);
	if (sizeof(SLinkerEliminateLadderCDTimeReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_ELIMINATE_LADDER_CD);

	SLinkerEliminateLadderCDTimeReq* req = (SLinkerEliminateLadderCDTimeReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SEliminateLadderCDTimeRes *pEliminateLadderCD = (SEliminateLadderCDTimeRes*)(buf + nLen);
	int retCode = GetLadderRankMan()->EliminateCoolingTime(req->userId, req->roleId, pEliminateLadderCD);
	LogInfo(("�û�[%u]��ɫ[%u]��������CD, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_ELIMINATE_LADDER_CD);

	//ˢ�´���
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_CASH);

	//ˢ��������Ϣ
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_LADDER_INFO);

	pEliminateLadderCD->hton();
	nLen += sizeof(SEliminateLadderCDTimeRes);
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_ELIMINATE_LADDER_CD, buf, nLen);
}

/* zpy ���������ս */
//int MLogicProcessMan::OnFinishLadderChallenge(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
//{
//	ASSERT(sizeof(SLinkerFinishLadderChallengeReq) == nDatalen);
//	if(sizeof(SLinkerFinishLadderChallengeReq) != nDatalen)
//		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_FINISH_LADDER_CHALLENGE);
//
//	SLinkerFinishLadderChallengeReq* req = (SLinkerFinishLadderChallengeReq*)pData;
//	req->ntoh();
//
//	char buf[MAX_BUF_LEN] = { 0 };
//	int nLen = GWS_COMMON_REPLY_LEN;
//	SFinishLadderChallengeRes *pFinishLadder = (SFinishLadderChallengeRes*)(buf + nLen);
//	int retCode = GetLadderRankMan()->FinishChallenge(req->userId, req->roleId, req->finishCode, pFinishLadder);
//	LogInfo(("�û�[%u]��ɫ[%u]���������ս, ������[%d]!", req->userId, req->roleId, retCode));
//	if(retCode != MLS_OK)
//		return gwsSession->SendBasicResponse(pHead, retCode, MLS_FINISH_LADDER_CHALLENGE);
//
//	//ˢ��������Ϣ
//	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_LADDER_INFO);
//
//	//ˢ������ƥ����Ϣ
//	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_LADDER_MATCHING_INFO);
//
//	pFinishLadder->hton();
//	nLen += sizeof(SFinishLadderChallengeRes);
//	return gwsSession->SendResponse(pHead, MLS_OK, MLS_FINISH_LADDER_CHALLENGE, buf, nLen);
//}

/* zpy ��ѯ���н�ɫ */
int MLogicProcessMan::OnQueryAllRoles(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerQueryAllRolesReq) == nDatalen);
	if (sizeof(SLinkerQueryAllRolesReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_ALL_ROLES);

	SLinkerQueryAllRolesReq* req = (SLinkerQueryAllRolesReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQueryAllRolesRes *pAllRoles = (SQueryAllRolesRes*)(buf + nLen);
	int retCode = GetRolesInfoMan()->QueryAllRoles(req->userId, pAllRoles);
	LogInfo(("�û�[%u]��ɫ[%u]��ѯ���н�ɫ, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_QUERY_ALL_ROLES);

	pAllRoles->hton();
	nLen += sizeof(SQueryAllRolesRes)+(pAllRoles->num - 1) * sizeof(SRoleOccu);
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_QUERY_ALL_ROLES, buf, nLen);
}

/* zpy ��ѯӵ�еĻ�������� */
int MLogicProcessMan::OnQueryHasAcivityInstType(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerHasActivityInstTypeReq) == nDatalen);
	if (sizeof(SLinkerHasActivityInstTypeReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_HAS_ACTIVITY_INST_TYPE);

	SLinkerHasActivityInstTypeReq* req = (SLinkerHasActivityInstTypeReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQueryHasActivityInstTypeRes *pActivityInstType = (SQueryHasActivityInstTypeRes*)(buf + nLen);
	int retCode = GetActitvityInstMan()->QueryHasActitvityInstType(req->userId, req->roleId, pActivityInstType);
	LogInfo(("�û�[%u]��ɫ[%u]��ѯӵ�еĻ��������, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_QUERY_HAS_ACTIVITY_INST_TYPE);

	pActivityInstType->hton();
	nLen += sizeof(SQueryHasActivityInstTypeRes)+(pActivityInstType->num - 1) * sizeof(SActivityInstItem);
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_QUERY_HAS_ACTIVITY_INST_TYPE, buf, nLen);
}

/* zpy ��ѯ�����ɫ���� */
int MLogicProcessMan::OnQueryWeekFreeActorType(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerQueryQueryWeekFreeActorReq) == nDatalen);
	if (sizeof(SLinkerQueryQueryWeekFreeActorReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_WEEKFREE_ACTOR);

	SLinkerQueryQueryWeekFreeActorReq* req = (SLinkerQueryQueryWeekFreeActorReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQueryWeekFreeActorRes *pWeekFreeActor = (SQueryWeekFreeActorRes*)(buf + nLen);
	pWeekFreeActor->actorType = GetCompetitiveModeMan()->GetWeekFreeActorType();
	LogInfo(("�û�[%u]��ɫ[%u]��ѯ�����ɫ!", req->userId, req->roleId));

	nLen += sizeof(SQueryWeekFreeActorRes);
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_QUERY_WEEKFREE_ACTOR, buf, nLen);
}

/* zpy ��ѯ����ģʽ������� */
int MLogicProcessMan::OnQueryEnterKOFNum(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerQueryCompEnterNumReq) == nDatalen);
	if (sizeof(SLinkerQueryCompEnterNumReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_COMPMODE_ENTER_NUM);

	SLinkerQueryCompEnterNumReq* req = (SLinkerQueryCompEnterNumReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQueryCompEnterNumRes *pEnterNum = (SQueryCompEnterNumRes*)(buf + nLen);
	int retCode = GetCompetitiveModeMan()->QueryEnterNum(req->userId, req->roleId, pEnterNum);
	LogInfo(("�û�[%u]��ɫ[%u]��ѯ����ģʽ��������, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_QUERY_COMPMODE_ENTER_NUM);

	pEnterNum->hton();
	nLen += sizeof(SQueryCompEnterNumRes);
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_QUERY_COMPMODE_ENTER_NUM, buf, nLen);
}

/* zpy ���򾺼�ģʽ������� */
int MLogicProcessMan::OnBuyEnterKOFNum(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerBuyCompEnterNumReq) == nDatalen);
	if (sizeof(SLinkerBuyCompEnterNumReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_BUY_COMPMODE_ENTER_NUM);

	SLinkerBuyCompEnterNumReq* req = (SLinkerBuyCompEnterNumReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SBuyCompEnterNumRes *pEnterNum = (SBuyCompEnterNumRes*)(buf + nLen);
	int retCode = GetCompetitiveModeMan()->BuyEnterNum(req->userId, req->roleId, req->buy_mode, pEnterNum);
	LogInfo(("�û�[%u]��ɫ[%u]���򾺼�ģʽ��������, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_BUY_COMPMODE_ENTER_NUM);

	//ˢ�´���
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_CASH);

	pEnterNum->hton();
	nLen += sizeof(SBuyCompEnterNumRes);
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_BUY_COMPMODE_ENTER_NUM, buf, nLen);
}

/* zpy ��ѯ1v1����������� */
int MLogicProcessMan::OnQueryEnterOneVsOneNum(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerQuery1V1EnterNumReq) == nDatalen);
	if (sizeof(SLinkerQuery1V1EnterNumReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_QUERY_1V1_ENTER_NUM);

	SLinkerQuery1V1EnterNumReq* req = (SLinkerQuery1V1EnterNumReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SQuery1V1EnterNumRes *pEnterNum = (SQuery1V1EnterNumRes*)(buf + nLen);
	int retCode = GetOneVsOneMan()->QueryEnterOneVsOneNum(req->userId, req->roleId, pEnterNum);
	LogInfo(("�û�[%u]��ɫ[%u]��ѯ1v1ģʽ��������, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_QUERY_1V1_ENTER_NUM);

	pEnterNum->hton();
	nLen += sizeof(SQuery1V1EnterNumRes);
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_QUERY_1V1_ENTER_NUM, buf, nLen);
}

/* zpy ����1v1����������� */
int MLogicProcessMan::OnBuyEnterOneVsOneNum(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerBuy1V1EnterNumReq) == nDatalen);
	if (sizeof(SLinkerBuy1V1EnterNumReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MLS_INVALID_PACKET, MLS_BUY_1V1_ENTER_NUM);

	SLinkerBuy1V1EnterNumReq* req = (SLinkerBuy1V1EnterNumReq*)pData;
	req->ntoh();

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SBuy1V1EnterNumRes *pBuyEnterNum = (SBuy1V1EnterNumRes*)(buf + nLen);
	int retCode = GetOneVsOneMan()->BuyEnterOneVsOneNum(req->userId, req->roleId, req->buy_num, pBuyEnterNum);
	LogInfo(("�û�[%u]��ɫ[%u]����1v1ģʽ��������, ������[%d]!", req->userId, req->roleId, retCode));
	if (retCode != MLS_OK)
		return gwsSession->SendBasicResponse(pHead, retCode, MLS_BUY_1V1_ENTER_NUM);

	//ˢ�´���
	GetRefreshDBMan()->InputUpdateInfo(req->userId, req->roleId, REFRESH_ROLE_CASH);

	pBuyEnterNum->hton();
	nLen += sizeof(SBuy1V1EnterNumRes);
	return gwsSession->SendResponse(pHead, MLS_OK, MLS_BUY_1V1_ENTER_NUM, buf, nLen);
}