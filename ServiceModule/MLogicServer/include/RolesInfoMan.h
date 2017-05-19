#ifndef __ROLES_INFO_MAN_H__
#define __ROLES_INFO_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
�û���ɫ��Ϣ������
*/
#include <map>
#include <vector>
#include "cyaTypes.h"
#include "cyaLocker.h"
#include "cyaRefCount.h"
#include "RoleInfos.h"
#include "ServiceProtocol.h"
#include "PlayerActionMan.h"

class RolesInfoMan
{
	class RoleSite : public BaseRefCount1
	{
	public:
		RoleSite();
		~RoleSite();

		virtual void DeleteObj();

		void SetLinkerId(BYTE linkerId);
		BYTE GetLinkerId() const;
		void SetParent(RolesInfoMan* parent);

		//��ѯ�û���ɫ��Ϣ
		int QueryUserRoles(UINT32 userId, SQueryRolesRes* pRoles);
		//��ѯӵ�еĽ�ɫ���� add by hxw 20151210
		int QueryUserRolesNum();

		//��ѯ��ɫ������Ϣ
		int QueryRoleAppearance(UINT32 roleId, SQueryRoleAppearanceRes* pRoleRes);
		//��ѯ��ɫ��ϸ��Ϣ
		int QueryRoleExplicit(UINT32 roleId, SQueryRoleExplicitRes* pRoleRes);
		//�����ɫ //20151121 ����userid ��Ϊ��ʯͬ�� by hxw
		int EnterRole(UINT32 userId, UINT32 roleId, SEnterRoleRes* pEnterRes);
		//�˳���ɫ
		int LeaveRole(UINT32 roleId);
		//������ɫ
		int CreateRole(UINT32 userId, BYTE occuId, BYTE sex, const char* roleNick, SCreateRoleRes* pCreateRes);
		//ɾ����ɫ
		int DelRole(UINT32 roleId);
		//�ı���Ӫ
		int ChangeFaction(UINT32 roleId, BYTE factionId, BOOL& needSyncDB);

		//zpy ��ѯ���н�ɫ��ID
		int QueryAllRoles(UINT32 userId, SQueryAllRolesRes* pAllRoles);

		/* zpy �ɾ�ϵͳ���� */
		int RemoveAllRole();

		//��ѯ������Ϣ
		int QueryBackpack(UINT32 roleId, BYTE from, BYTE nums, SQueryBackpackRes* pBackpackRes);
		//��������
		int CleanupBackpack(UINT32 roleId, BOOL& isCleanup);
		//������������
		int DropBackpackProperty(UINT32 roleId, UINT32 id, BYTE num);
		//ʹ�õ���
		int UseProperty(UINT32 roleId, UINT32 id, UINT16& propId, SUsePropertyRes *pUseProperty);
		//�һ�����
		int CashProperty(UINT32 roleId, UINT16 cashId, SCashPropertyRes* pCashRes);
		//���򱳰���ֿ�洢�ռ�
		int BuyStorageSpace(UINT32 roleId, BYTE buyType, BYTE num, UINT32& spend, BYTE& newNum);
		//������Ʒ
		int SellProperty(UINT32 roleId, UINT32 id, BYTE num, SSellPropertyRes* pSellRes, SPropertyAttr& propAttr);
		//��Ʒ�ع�
		int BuyBackProperty(UINT32 roleId, SPropertyAttr& buyProp, BYTE num, SPropBuyBackRes* pBuyBackRes);

		//װ��װ��
		int LoadEquipment(UINT32 roleId, UINT32 id, BYTE pos);
		//ж��װ��
		int UnLoadEquipment(UINT32 roleId, UINT32 id);

		//װ���ϳ�
		int EquipmentCompose(UINT32 roleId, UINT16 composeId, SEquipComposeRes* pComposeRes);

		//���븱��
		int EnterInst(UINT32 roleId, UINT16 instId, SEnterInstRes* pEnterInstRes, int& dataLen, LTMSEL enterTTS, PlayerActionMan::SEnterInstItem& enterInstItem);
		//��ȡ��������ֵ���
		int FetchInstAreaMonsterDrops(UINT32 roleId, UINT16 instId, BYTE areaId, SFetchInstAreaDropsRes* pAreaDropsRes);
		//ʰȡ�����ֵ���
		int PickupInstMonsterDrop(UINT32 userId, UINT32 roleId, UINT16 instId, BYTE areaId, BYTE dropId, SPickupInstDropNotify* pNotify);
		//ʰȡ�������������
		int PickupInstBrokenDrop(UINT32 userId, UINT32 roleId, UINT16 instId, BYTE dropId, SPickupInstDropNotify* pNotify);
		//ʰȡ��������
		int PickupInstBoxDrop(UINT32 userId, UINT32 roleId, UINT16 instId, BYTE boxId, BYTE id, SPickupInstDropNotify* pNotify);
		//ɱ�ֵþ���
		int KillMonsterExp(UINT32 roleId, UINT16 instId, BYTE areaId, UINT16 monsterId, BYTE monsterLv, BOOL& isUpgrade);
		//��ɸ���
		int FinishInst(UINT32 userId, UINT32 roleId, UINT16 instId, UINT32 score, BYTE star, LTMSEL& enterTTS, const std::vector<SPropertyItem> &rewardVec, SFinishInstRes* pFinishInstRes, SReceiveRewardRefresh *pRefresh);

		/* zpy �ɾ�ϵͳ���� */
		int GetMaxGemLevel(UINT32 roleId, BYTE &max_level);
		int GetInlaidGemLevelSum(UINT32 roleId, UINT32 &sum);
		int GetEquipStarLevelSum(UINT32 roleId, UINT32 &sum);
		int GetSkillLevelSum(UINT32 roleId, UINT32 &sum);

		//��ѯ��ɫ����
		int QueryRoleSkills(UINT32 roleId, SQuerySkillsRes* pSkills);
		//��������
		int SkillUpgrade(UINT32 roleId, UINT16 skillId, BYTE step, SSkillUpgradeRes* pUpgradeRes);
		//װ�ؼ���
		int LoadSkill(UINT32 roleId, UINT16 skillId, BYTE key, BOOL& needSyncDB);
		//ȡ������
		int CancelSkill(UINT32 roleId, UINT16 skillId, BOOL& needSyncDB);

		//��������
		int AcceptTask(UINT32 roleId, UINT16 taskId);
		//��ѯ�ѽ�������
		int QueryAcceptedTasks(UINT32 roleId, SQueryAcceptTasksRes* pTaskRes);
		//��������
		int DropTask(UINT32 roleId, UINT16 taskId);
		//�ύ�������
		int CommitTaskFinish(UINT32 roleId, UINT16 taskId, SFinishTaskFailNotify* pFinTaskNotify);
		//��ȡ�ճ�����
		int GetDailyTasks(UINT32 roleId, SGetDailyTaskRes* pDailyTasks);
		//��������Ŀ��
		int UpdateTaskTarget(UINT32 roleId, UINT16 taskId, const UINT32* pTarget, int n = MAX_TASK_TARGET_NUM);
		//��ȡ������
		int ReceiveTaskReward(UINT32 userId, UINT32 roleId, UINT16 taskId, SReceiveTaskRewardRes* pTaskRewardRes);

		//װ��λ��Ƕ��ʯ
		int EquipPosInlaidGem(UINT32 roleId, BYTE pos, UINT32 gemId, BYTE holeSeq);
		//װ��λȡ����ʯ
		int EquipPosRemoveGem(UINT32 roleId, BYTE pos, BYTE holeSeq, SEquipPosRemoveGemRes* pRemoveRes);
		//��ʯ�ϳ�
		int	GemCompose(UINT32 roleId, BYTE num, const SConsumeProperty* pMaterial, SGemComposeRes* pComposeRes);

		//��ȡ�ʼ�����
		int ReceiveEmailAttachments(UINT32 roleId, BYTE emailType, const SPropertyAttr* pAttachmentsProp, BYTE attachmentsNum, SReceiveEmailAttachmentsRes* pEmailAttachments, BOOL& isUpgrade);
		//��ֵ
		int RechargeToken(UINT32 roleId, UINT16 cashId, SRechargeRes* pRechargeRes, char* pszRoleNick);
		//���ý�ɫ�ƺ�id
		int SetRoleTitleId(UINT32 roleId, UINT16 titleId);
		//�̳ǹ�����Ʒ
		int MallBuyGoods(UINT32 roleId, UINT32 buyId, UINT16 num, SMallBuyGoodsRes* pBuyGoodsRes, char* pszRoleNick, UINT32& spendToken);

		//��ѯ��ɫʱװ
		int QueryRoleDress(UINT32 roleId, SQueryRoleDressRes* pDressRes);
		//��ɫʱװ����
		int WearDress(UINT32 roleId, UINT16 dressId);
		//��ѯ��ɫװ����Ƭ
		int QueryEquipFragment(UINT32 roleId, SQueryEquipFragmentRes* pFragmentRes);

		//����
		int RoleRevive(UINT32 roleId, UINT16 reviveId, SReviveRoleRes* pReviveRes);
		//��ѯ��Ҷһ�
		int QueryGoldExchange(UINT32 roleId, SQueryGoldExchangeRes* pExchangeRes);
		//��Ҷһ�
		int GoldExchange(UINT32 roleId, SGoldExchangeRes* pExchangeRes);
		//�Ƽ�����
		int GetRecommendFriends(UINT32 userId, UINT32 roleId, SGetRecommendFriendsRes* pFriendsRes);

		//��ȡVIP����
		int ReceiveVIPReward(UINT32 userId, UINT32 roleId, BYTE vipLv, SReceiveVIPRewardRes* pVIPRewardRes, SReceiveRewardRefresh* pRefresh);

		/* zpy �ɾ�ϵͳ���� */
		int ReceiveAchievementReward(UINT32 userId, UINT32 roleId, UINT32 achievement_id, SReceiveRewardRes *pAchievementReward, SReceiveRewardRefresh *pRefresh);

		//��ѯ��������
		int QueryRoleHelps(UINT32 roleId, SQueryFinishedHelpRes* pHelpRes);
		//�����������
		int FinishRoleHelp(UINT32 roleId, UINT16 helpId);
		//��ѯ��ɫӵ��ħ����
		int QueryRoleMagics(UINT32 roleId, SQueryRoleMagicsRes* pMagicsRes);
		//���ħ����
		int GainMagic(UINT32 roleId, UINT16 instId, UINT16 magicId);

		//װ��λǿ��
		int EquipPosStrengthen(UINT32 roleId, BYTE pos, SEquipPosStrengthenRes* pStrengthenRes);
		//װ��λ����
		int EquipPosRiseStar(UINT32 roleId, BYTE pos, const SRiseStarAttachParam& param, SEquipPosRiseStarRes* pRiseStarRes);

		//��ѯǩ��
		int QuerySignIn(UINT32 roleId, SQueryOnlineSignInRes* pSignInRes);
		//ǩ��
		int SignIn(UINT32 userId, UINT32 roleId);
		//����ɨ��
		int InstSweep(UINT32 userId, UINT32 roleId, UINT16 instId, SInstSweepRes* pSweepRes, BOOL& isUpgrade);
		//��������
		int UnlockContent(UINT32 roleId, BYTE type, UINT16 unlockId);

		//��ȡ���̽�Ʒ
		int ExtractRouletteReward(UINT32 userId, UINT32 roleId, const SRoulettePropItem& extractReward, SExtractRouletteRewardRes* pRewardRes, BOOL& isUpgrade);
		//��ȡ���̴�
		int FetchRouletteBigReward(UINT32 userId, UINT32 roleId, const SRoulettePropItem& bigReward, SFetchRouletteBigRewardRes* pRewardRes, BOOL& isUpgrade);
		//ˢ�����̴�
		int UpdateRouletteBigReward(UINT32 roleId, UINT32 spendToken, UINT32& curToken);

		//��ѯ�����һ�
		int QueryVitExchange(UINT32 roleId, SQueryVitExchangeRes* pExchangeRes);
		//�����һ�
		int VitExchange(UINT32 roleId, SVitExchangeRes* pExchangeRes);

		/* zpy ��ȡÿ��ǩ������ */
		int ReceiveDailySignReward(UINT32 userId, UINT32 roleId, BYTE days, bool continuous, SRoleDailySignRes *pRoleSign, SReceiveRewardRefresh *pRefresh);

		/* zpy ��ѯ������Ƭ */
		int QueryChainSoulFragment(UINT32 roleId, SQueryChainSoulFragmentRes* pFragmentRes);
		/* zpy ��ѯ���겿�� */
		int QueryChainSoulPos(UINT32 roleId, SQueryChainSoulPosRes* pChainSoulPos);
		/* zpy �������겿�� */
		int ChainSoulPosUpgrade(UINT32 roleId, BYTE pos, SChainSoulPosUpgradeRes* pChainSoulPosUpgrade);

		/* zpy �����ɫ */
		int ResurrectionInWorldBoss(UINT32 roleId, UINT16 reviveId, UINT32 consume_count, UINT32 &out_rpcoin);

		/* zpy ���򾺼�ģʽ������� */
		int BuyCompModeEnterNum(UINT32 roleId, int purchased, SBuyCompEnterNumRes *pEnterNum);
		int BuyAllCompModeEnterNum(UINT32 roleId, int purchased, int upper_num, SBuyCompEnterNumRes *pEnterNum);

		/* zpy ����1vs1ģʽ������� */
		int BuyoneVsOneEnterNum(UINT32 roleId, int purchased, int num, SBuy1V1EnterNumRes *pBuyEnter);

	public:
		//��ɫ�Ƿ�����
		BOOL IsRoleOnline(UINT32 roleId);
		//��ȡ��ɫ��������
		int GetRoleBackpack(UINT32 roleId, std::vector<SPropertyAttr>& vec);
		//��ȡ��ɫ�ѽ�������
		int GetRoleAcceptTask(UINT32 roleId, std::vector<SAcceptTaskInfo>& vec);
		//��ɫ����/�ֿ��С
		int GetRoleStorageSpace(UINT32 roleId, BYTE& packSize, BYTE& wareSize);
		//��ȡ��ɫ���
		int GetRoleMoney(UINT32 roleId, UINT64& gold, UINT32& cash, UINT32& cashcount);
		//��ȡ��ɫ������Ϣ
		int GetRoleVistInfo(UINT32 roleId, UINT32& vistCount, UINT32& vit, char* lastTime = NULL);
		//��ȡ��ɫ�Ѿ�ѧ����
		int GetRoleSkills(UINT32 roleId, std::vector<SSkillMoveInfo>& skillsVec);
		//��ȡ��ɫ�ǳ�
		int GetRoleNick(UINT32 roleId, char* pszNick);
		//��ȡ��ɫ������Ϣ
		int GetRoleInfo(UINT32 roleId, SRoleInfos* pRoleInfo);
		//��ȡ��ɫ��ǰ����
		int GetRoleCurrentVit(UINT32 roleId, UINT32& vit);
		//��ȡ��ɫ��Ӫ
		int GetRoleFaction(UINT32 roleId, BYTE& factionId);
		//��ȡ��ɫ����,����
		int GetRoleWearBackpack(UINT32 roleId, std::vector<SPropertyAttr>& wear, std::vector<SPropertyAttr>& backpack);
		//��ȡ��ɫ����
		int GetRoleWearEquip(UINT32 roleId, std::vector<SPropertyAttr>& wear);
		//��ȡ��ǰ�û��������Ϸ��ɫid
		int GetUserEnteredRoles(std::vector<UINT32>& rolesVec);
		//�ָ����߽�ɫ����
		int RecoveryOnlineRoleVit(UINT32 roleId, UINT32& vit, BOOL& needNotify);
		//��ȡ��ɫְҵ�ƺ�id
		int GetRoleTitleId(UINT32 roleId, UINT16& titleId);
		//��ȡ��ɫ����ʱװid
		int GetRoleWearDressId(UINT32 roleId, UINT16& dressId, std::vector<UINT16>* pDressVec);
		//�����ɫս��
		int CalcRoleCompatPower(UINT32 roleId, UINT32* pNewPower);
		//��ȡ��ɫս��
		int GetRoleCompatPower(UINT32 roleId, UINT32& cp);
		//��ȡ��ɫװ����Ƭ
		int GetRoleEquipFragment(UINT32 roleId, std::vector<SEquipFragmentAttr>& fragmentVec);
		//��ȡ��ɫλ��
		int GetRolePosition(UINT32 roleId, SGSPosition* pos);
		//��ȡ��ɫ����
		int GetRoleExp(UINT32 roleId, SBigNumber& exp);
		//��ȡ��ɫ����֪ͨ��Ϣ
		int GetRoleUpgradeNotify(UINT32 roleId, SRoleUpgradeNotify* pUpgradeNotify);
		//��ȡ��ɫ��������
		int GetRoleHelps(UINT32 roleId, std::vector<UINT16>& helpsVec);
		//��ȡ��ɫӵ��ħ����
		int GetRoleMagics(UINT32 roleId, std::vector<UINT16>& magicsVec);
		//��ȡװ��λ����
		int GetRoleEquipPosAttr(UINT32 roleId, std::vector<SEquipPosAttr>& equipPosVec);
		//��ȡ��ɫǩ����Ϣ
		int GetRoleSignInInfo(UINT32 roleId, std::vector<LTMSEL>& signVec);
		//�����ҳ�ֵ����״̬
		int CheckPlayerRechargeOrderStatus(UINT32 userId, UINT32 roleId, BOOL& hasNotReceiveOrder);
		//����ÿ������
		void CleanEverydayData();

		//add by hxw 20151124
		//����ÿ��������Ϣ�����ݣ���Ϊ�漰������д�룬���Ե��������ʵ��
		void CleanOnlineData();

		//zpy ��ȡ������Ƭ
		int GetRoleChainSoulFragment(UINT32 roleId, std::vector<SChainSoulFragmentAttr>& fragmentVec);
		//zpy ��ȡ���겿��
		int GetRoleChainSoulPos(UINT32 roleId, std::vector<SChainSoulPosAttr>& posAttr);

		//zpy ����ƥ��
		int LadderMatching(UINT32 roleId, UINT32 matching_rank[LADDER_MATCHTING_MAX_NUM], UINT16 *refresh_count, UINT32 *rpcoin);
		//zpy ˢ���������з�Χ
		int RefreshLadderRankScope(UINT32 roleId, UINT32 matching_rank[LADDER_MATCHTING_MAX_NUM]);
		//zpy ִ��������ս
		int DoLadderChallenge(UINT32 roleId, UINT16 *ret_challenge_count);
		//zpy ������ȴʱ��
		int EliminateLadderCoolingTime(UINT32 roleId, SEliminateLadderCDTimeRes *pEliminate);
		//zpy ���������ս
		int FinishLadderChallenge(UINT32 roleId, BYTE finishCode/*, SFinishLadderChallengeRes *pFinishLadder*/);
		//zpy ���ñ���ս���������з�Χ
		int ResetChallengerLadderRankScope(UINT32 roleId);

		//zpy ��ӻ�����
		int AddRobot(UINT32 roleId, UINT32 robotId);
		//zpy �Ƿ���ڻ�����
		int HasRobot(UINT32 roleId, UINT32 robotId, bool *ret);
		//zpy ���������
		int ClearRobot(UINT32 roleId);

		//add by hxw 20151015 ��ȡͨ�����н����ӿ�	

		int ReceiveRankReward(UINT32 userId, UINT32 roleId, BYTE type, UINT16 rewardid, SGetRankRewardRes *pRoleSign, SReceiveRewardRefresh *pRefresh);
		//add 20151022
		int GetOnlineInfo(UINT32 roleId, SOnlineInfo* info, std::vector<UINT16>& vonline);
		int GetStrongerInfo(UINT32 roleId, std::vector<UINT16>& infov);
		BOOL IsGetStrongerReward(UINT32 roleId, UINT16 rewardid);
		BOOL IsGetOnlineReward(UINT32 roleId, UINT16 rewardid);
		int InputStrongerReward(UINT32 roleId, UINT16 rewardid);
		int InputOnlineReward(UINT32 roleId, UINT16 rewardid);
		int SetOnlineInfo(UINT32 roleId, UINT32 id, UINT32 time, LTMSEL lasttime = 0);

		//end 20151015;


	private:
		//�����˺�id��ѯ���н�ɫ��Ϣ
		int OnQueryRolesInfoByUserId(UINT32 userId);

	private:
		BYTE m_linkerId;
		RolesInfoMan* m_parent;
		CXTLocker m_rolesMapLocker;
		std::map<UINT32/*��ɫid*/, SRoleInfos*> m_rolesMap;	//��ɫ��
	};

public:
	RolesInfoMan();
	~RolesInfoMan();

	//��ȡ�û���ɫ��Ϣ
	int QueryUserRoles(UINT32 userId, SQueryRolesRes* pRoles);
	//��ѯӵ�еĽ�ɫ���� add by hxw 20151210
	int QueryUserRolesNum(UINT32 userId);

	//�����ɫ
	int UserEnterRole(UINT32 userId, UINT32 roleId, BYTE linkerId, SEnterRoleRes* pEnterRes);
	//�˳���ɫ
	int UserLeaveRole(UINT32 userId, UINT32 roleId);
	//�Ƴ��˺Ž�ɫ
	void RemoveUserRoles(UINT32 userId);

	//������ɫ
	int CreateRole(UINT32 userId, BYTE occuId, BYTE sex, const char* roleNick, SCreateRoleRes* pCreateRes);
	//ɾ����ɫ
	int DelRole(UINT32 userId, UINT32 roleId, SDelRoleRes *pDelRes);
	//��ѯ��ɫ������Ϣ
	int QueryRoleAppearance(UINT32 userId, UINT32 roleId, SQueryRoleAppearanceRes* pRoleRes);
	//��ѯ��ɫ��ϸ��Ϣ
	int QueryRoleExplicit(UINT32 userId, UINT32 roleId, SQueryRoleExplicitRes* pRoleRes);
	//�ı���Ӫ
	int ChangeFaction(UINT32 userId, UINT32 roleId, BYTE factionId, BOOL& needSyncDB);
	//zpy ��ѯ���н�ɫ��ID
	int QueryAllRoles(UINT32 userId, SQueryAllRolesRes* pAllRoles);

	//��ѯ������Ϣ
	int QueryBackpack(UINT32 userId, UINT32 roleId, BYTE from, BYTE nums, SQueryBackpackRes* pBackpackRes);
	//��������
	int CleanupBackpack(UINT32 userId, UINT32 roleId, BOOL& isCleanup);
	//������������
	int DropBackpackProperty(UINT32 userId, UINT32 roleId, UINT32 id, BYTE num);
	//ʹ�õ���
	int UseProperty(UINT32 userId, UINT32 roleId, UINT32 id, UINT16& propId, SUsePropertyRes *pUseProperty);
	//�һ�����
	int CashProperty(UINT32 userId, UINT32 roleId, UINT16 cashId, SCashPropertyRes* pCashRes);
	//���򱳰���ֿ�洢�ռ�
	int BuyStorageSpace(UINT32 userId, UINT32 roleId, BYTE buyType, BYTE num, UINT32& spend, BYTE& newNum);
	//������Ʒ
	int SellProperty(UINT32 userId, UINT32 roleId, UINT32 id, BYTE num, SSellPropertyRes* pSellRes, SPropertyAttr& propAttr);
	//��Ʒ�ع�
	int BuyBackProperty(UINT32 userId, UINT32 roleId, SPropertyAttr& buyProp, BYTE num, SPropBuyBackRes* pBuyBackRes);

	//װ��װ��
	int LoadEquipment(UINT32 userId, UINT32 roleId, UINT32 id, BYTE pos);
	//ж��װ��
	int UnLoadEquipment(UINT32 userId, UINT32 roleId, UINT32 id);
	//װ���ϳ�
	int EquipmentCompose(UINT32 userId, UINT32 roleId, UINT16 composeId, SEquipComposeRes* pComposeRes);

	//���븱��
	int EnterInst(UINT32 userId, UINT32 roleId, UINT16 instId, SEnterInstRes* pEnterInstRes, int& dataLen, LTMSEL enterTTS, PlayerActionMan::SEnterInstItem& enterInstItem);
	//��ȡ��������ֵ���
	int FetchInstAreaMonsterDrops(UINT32 userId, UINT32 roleId, UINT16 instId, BYTE areaId, SFetchInstAreaDropsRes* pAreaDropsRes);
	//ʰȡ�����ֵ���
	int PickupInstMonsterDrop(UINT32 userId, UINT32 roleId, UINT16 instId, BYTE areaId, BYTE dropId, SPickupInstDropNotify* pNotify);
	//ʰȡ�������������
	int PickupInstBrokenDrop(UINT32 userId, UINT32 roleId, UINT16 instId, BYTE dropId, SPickupInstDropNotify* pNotify);
	//ʰȡ��������
	int PickupInstBoxDrop(UINT32 userId, UINT32 roleId, UINT16 instId, BYTE boxId, BYTE id, SPickupInstDropNotify* pNotify);
	//ɱ�ֵþ���
	int KillMonsterExp(UINT32 userId, UINT32 roleId, UINT16 instId, BYTE areaId, UINT16 monsterId, BYTE monsterLv, BOOL& isUpgrade);
	//��ɸ���
	int FinishInst(UINT32 userId, UINT32 roleId, UINT16 instId, UINT32 score, BYTE star, LTMSEL& enterTTS, const std::vector<SPropertyItem> &rewardVec, SFinishInstRes* pFinishInstRes, SReceiveRewardRefresh *pRefresh);

	/* zpy �ɾ�ϵͳ���� */
	int GetMaxGemLevel(UINT32 userId, UINT32 roleId, BYTE &max_level);
	int GetInlaidGemLevelSum(UINT32 userId, UINT32 roleId, UINT32 &sum);
	int GetEquipStarLevelSum(UINT32 userId, UINT32 roleId, UINT32 &sum);
	int GetSkillLevelSum(UINT32 userId, UINT32 roleId, UINT32 &sum);

	//��ѯ��ɫ����
	int QueryRoleSkills(UINT32 userId, UINT32 roleId, SQuerySkillsRes* pSkills);
	//��������
	int SkillUpgrade(UINT32 userId, UINT32 roleId, UINT16 skillId, BYTE step, SSkillUpgradeRes* pUpgradeRes);
	//װ�ؼ���
	int LoadSkill(UINT32 userId, UINT32 roleId, UINT16 skillId, BYTE key, BOOL& needSyncDB);
	//ȡ������
	int CancelSkill(UINT32 userId, UINT32 roleId, UINT16 skillId, BOOL& needSyncDB);

	//��������
	int AcceptTask(UINT32 userId, UINT32 roleId, UINT16 taskId);
	//��ѯ�ѽ�������
	int QueryAcceptedTasks(UINT32 userId, UINT32 roleId, SQueryAcceptTasksRes* pTaskRes);
	//��������
	int DropTask(UINT32 userId, UINT32 roleId, UINT16 taskId);
	//�ύ�������
	int CommitTaskFinish(UINT32 userId, UINT32 roleId, UINT16 taskId, SFinishTaskFailNotify* pFinTaskRes);
	//��ȡ�ճ�����
	int GetDailyTasks(UINT32 userId, UINT32 roleId, SGetDailyTaskRes* pDailyTasks);
	//��������Ŀ��
	int UpdateTaskTarget(UINT32 userId, UINT32 roleId, UINT16 taskId, const UINT32* pTarget, int n = MAX_TASK_TARGET_NUM);
	//��ȡ������
	int ReceiveTaskReward(UINT32 userId, UINT32 roleId, UINT16 taskId, SReceiveTaskRewardRes* pTaskRewardRes);

	//װ��λ��Ƕ��ʯ
	int EquipPosInlaidGem(UINT32 userId, UINT32 roleId, BYTE pos, UINT32 gemId, BYTE holeSeq);
	//װ��λȡ����ʯ
	int EquipPosRemoveGem(UINT32 userId, UINT32 roleId, BYTE pos, BYTE holeSeq, SEquipPosRemoveGemRes* pRemoveRes);
	//��ʯ�ϳ�
	int	GemCompose(UINT32 userId, UINT32 roleId, BYTE num, const SConsumeProperty* pMaterial, SGemComposeRes* pComposeRes);

	//��ȡ�ʼ�����
	int ReceiveEmailAttachments(UINT32 userId, UINT32 roleId, BYTE emailType, const SPropertyAttr* pAttachmentsProp, BYTE attachmentsNum, SReceiveEmailAttachmentsRes* pEmailAttachmentsRes, BOOL& isUpgrade);
	//��ֵ
	int RechargeToken(UINT32 userId, UINT32 roleId, UINT16 cashId, SRechargeRes* pRechargeRes, char* pszRoleNick);
	//���ý�ɫ�ƺ�id
	int SetRoleTitleId(UINT32 userId, UINT32 roleId, UINT16 titleId);
	//�̳ǹ�����Ʒ
	int MallBuyGoods(UINT32 userId, UINT32 roleId, UINT32 buyId, UINT16 num, SMallBuyGoodsRes* pBuyGoodsRes, char* pszRoleNick, UINT32& spendToken);

	//��ѯ��ɫʱװ
	int QueryRoleDress(UINT32 userId, UINT32 roleId, SQueryRoleDressRes* pDressRes);
	//��ɫʱװ����
	int WearDress(UINT32 userId, UINT32 roleId, UINT16 dressId);
	//��ѯ��ɫװ����Ƭ
	int QueryEquipFragment(UINT32 userId, UINT32 roleId, SQueryEquipFragmentRes* pFragmentRes);

	//����
	int RoleRevive(UINT32 userId, UINT32 roleId, UINT16 reviveId, SReviveRoleRes* pReviveRes);
	//��ѯ��Ҷһ�
	int QueryGoldExchange(UINT32 userId, UINT32 roleId, SQueryGoldExchangeRes* pExchangeRes);
	//��Ҷһ�
	int GoldExchange(UINT32 userId, UINT32 roleId, SGoldExchangeRes* pExchangeRes);
	//�Ƽ�����
	int GetRecommendFriends(UINT32 userId, UINT32 roleId, SGetRecommendFriendsRes* pFriendsRes);

	//��ȡVIP����
	int ReceiveVIPReward(UINT32 userId, UINT32 roleId, BYTE vipLv, SReceiveVIPRewardRes* pVIPRewardRes, SReceiveRewardRefresh* pRefresh);

	/* zpy �ɾ�ϵͳ���� */
	int ReceiveAchievementReward(UINT32 userId, UINT32 roleId, UINT32 achievement_id, SReceiveRewardRes *pAchievementReward, SReceiveRewardRefresh *pRefresh);

	//��ѯ��������
	int QueryRoleHelps(UINT32 userId, UINT32 roleId, SQueryFinishedHelpRes* pHelpRes);
	//�����������
	int FinishRoleHelp(UINT32 userId, UINT32 roleId, UINT16 helpId);
	//��ѯ��ɫӵ��ħ����
	int QueryRoleMagics(UINT32 userId, UINT32 roleId, SQueryRoleMagicsRes* pMagicsRes);
	//���ħ����
	int GainMagic(UINT32 userId, UINT32 roleId, UINT16 instId, UINT16 magicId);

	//װ��λǿ��
	int EquipPosStrengthen(UINT32 userId, UINT32 roleId, BYTE pos, SEquipPosStrengthenRes* pStrengthenRes);
	//װ��λ����
	int EquipPosRiseStar(UINT32 userId, UINT32 roleId, BYTE pos, const SRiseStarAttachParam& param, SEquipPosRiseStarRes* pRiseStarRes);

	//��ѯǩ��
	int QuerySignIn(UINT32 userId, UINT32 roleId, SQueryOnlineSignInRes* pSignInRes);
	//ǩ��
	int SignIn(UINT32 userId, UINT32 roleId);
	//����ɨ��
	int InstSweep(UINT32 userId, UINT32 roleId, UINT16 instId, SInstSweepRes* pSweepRes, BOOL& isUpgrade);
	//��������
	int UnlockContent(UINT32 userId, UINT32 roleId, BYTE type, UINT16 unlockId);

	//��ȡ���̽�Ʒ
	int ExtractRouletteReward(UINT32 userId, UINT32 roleId, const SRoulettePropItem& extractReward, SExtractRouletteRewardRes* pRewardRes, BOOL& isUpgrade);
	//��ȡ���̴�
	int FetchRouletteBigReward(UINT32 userId, UINT32 roleId, const SRoulettePropItem& bigReward, SFetchRouletteBigRewardRes* pRewardRes, BOOL& isUpgrade);
	//ˢ�����̴�
	int UpdateRouletteBigReward(UINT32 userId, UINT32 roleId, UINT32 spendToken, UINT32& curToken);

	/* zpy �����һ� */
	//��ѯ�����һ�
	int QueryVitExchange(UINT32 userId, UINT32 roleId, SQueryVitExchangeRes* pExchangeRes);
	//�����һ�
	int VitExchange(UINT32 userId, UINT32 roleId, SVitExchangeRes* pExchangeRes);

	/* zpy ��ȡÿ��ǩ������ */
	int ReceiveDailySignReward(UINT32 userId, UINT32 roleId, BYTE days, bool continuous, SRoleDailySignRes *pRoleSign, SReceiveRewardRefresh *pRefresh);

	/* zpy ��ѯ������Ƭ */
	int QueryChainSoulFragment(UINT32 userId, UINT32 roleId, SQueryChainSoulFragmentRes* pFragmentRes);
	/* zpy ��ѯ���겿�� */
	int QueryChainSoulPos(UINT32 userId, UINT32 roleId, SQueryChainSoulPosRes* pChainSoulPos);
	/* zpy �������겿�� */
	int ChainSoulPosUpgrade(UINT32 userId, UINT32 roleId, BYTE pos, SChainSoulPosUpgradeRes* pChainSoulPosUpgrade);

	/* zpy �����ɫ */
	int ResurrectionInWorldBoss(UINT32 userId, UINT32 roleId, UINT16 reviveId, UINT32 consume_count, UINT32 &out_rpcoin);

public:
	//�û���ɫ�Ƿ�����
	BOOL IsUserRoleOnline(UINT32 userId, UINT32 roleId);
	//�û��Ƿ�����
	BOOL IsUserOnline(UINT32 userId);
	//��ȡ�û�����linker
	int GetUserLinkerId(UINT32 userId, BYTE& linkerId);
	//�����û�����linker
	int SetUserLinkerId(UINT32 userId, BYTE linkerId);
	//��ȡ��ɫ��������
	int GetRoleBackpack(UINT32 userId, UINT32 roleId, std::vector<SPropertyAttr>& vec);
	//��ȡ��ɫ�ѽ�������
	int GetRoleAcceptTask(UINT32 userId, UINT32 roleId, std::vector<SAcceptTaskInfo>& vec);
	//��ȡ��ɫ����/�ֿ��С
	int GetRoleStorageSpace(UINT32 userId, UINT32 roleId, BYTE& packSize, BYTE& wareSize);
	//��ȡ��ɫmoney
	int GetRoleMoney(UINT32 userId, UINT32 roleId, UINT64& gold, UINT32& cash, UINT32& cashcount);
	//��ȡ��ɫ������Ϣ
	int GetRoleVistInfo(UINT32 userId, UINT32 roleId, UINT32& vistCount, UINT32& vit, char* lastTime);
	//��ȡ��ɫ����
	int GetRoleSkills(UINT32 userId, UINT32 roleId, std::vector<SSkillMoveInfo>& skillsVec);
	//��ȡ��ɫ�ǳ�
	int GetRoleNick(UINT32 userId, UINT32 roleId, char* pszNick);
	//��ȡ��ɫ������Ϣ
	int GetRoleInfo(UINT32 userId, UINT32 roleId, SRoleInfos* pRoleInfo);
	//��ȡ��ɫ��ǰ����
	int GetRoleCurrentVit(UINT32 userId, UINT32 roleId, UINT32& vit);
	//��ȡ��ɫ��Ӫ
	int GetRoleFaction(UINT32 userId, UINT32 roleId, BYTE& factionId);
	//��ȡ��ɫ����,����
	int GetRoleWearBackpack(UINT32 userId, UINT32 roleId, std::vector<SPropertyAttr>& wear, std::vector<SPropertyAttr>& backpack);
	//��ȡ��ɫ�Ѿ�����װ��
	int GetRoleWearEquip(UINT32 userId, UINT32 roleId, std::vector<SPropertyAttr>& wear);
	//��ȡ��ǰ�û��������Ϸ��ɫid
	int GetUserEnteredRoles(UINT32 userId, std::vector<UINT32>& rolesVec);
	//�ָ����߽�ɫ����
	int RecoveryOnlineRoleVit(UINT32 userId, UINT32 roleId, UINT32& vit, BOOL& needNotify);
	//��ȡ��ɫְҵ�ƺ�id
	int GetRoleTitleId(UINT32 userId, UINT32 roleId, UINT16& titleId);
	//��ȡ��ɫ����ʱװid
	int GetRoleWearDressId(UINT32 userId, UINT32 roleId, UINT16& dressId, std::vector<UINT16>* pDressVec);
	//�����ɫս��
	int CalcRoleCompatPower(UINT32 userId, UINT32 roleId, UINT32* pNewPower = NULL);
	//��ȡ��ɫս��
	int GetRoleCompatPower(UINT32 userId, UINT32 roleId, UINT32& cp);
	//��ȡ��ɫװ����Ƭ
	int GetRoleEquipFragment(UINT32 userId, UINT32 roleId, std::vector<SEquipFragmentAttr>& fragmentVec);
	//��ȡ��ɫλ��
	int GetRolePosition(UINT32 userId, UINT32 roleId, SGSPosition* pos);
	//��ȡ��ɫ����
	int GetRoleExp(UINT32 userId, UINT32 roleId, SBigNumber& exp);
	//��ȡ��ɫ����������Ϣ
	int GetRoleUpgradeNotify(UINT32 userId, UINT32 roleId, SRoleUpgradeNotify* pUpgradeNotify);
	//��ȡ��ɫ��������
	int GetRoleHelps(UINT32 userId, UINT32 roleId, std::vector<UINT16>& helpsVec);
	//��ȡ��ɫӵ��ħ����
	int GetRoleMagics(UINT32 userId, UINT32 roleId, std::vector<UINT16>& magicsVec);
	//��ȡװ��λ����
	int GetRoleEquipPosAttr(UINT32 userId, UINT32 roleId, std::vector<SEquipPosAttr>& equipPosVec);
	//��ȡ��ɫǩ����Ϣ
	int GetRoleSignInInfo(UINT32 userId, UINT32 roleId, std::vector<LTMSEL>& signVec);
	//�����ҳ�ֵ����״̬
	int CheckPlayerRechargeOrderStatus(UINT32 userId, UINT32 roleId, BOOL& hasNotReceiveOrder);
	//����ÿ������
	void CleanEverydayData();

	//add by hxw 20151124
	//����ÿ��online reward ��¼
	void CleanOnlineData();

	//zpy ��ȡ������Ƭ
	int GetRoleChainSoulFragment(UINT32 userId, UINT32 roleId, std::vector<SChainSoulFragmentAttr>& fragmentVec);
	//zpy ��ȡ���겿��
	int GetRoleChainSoulPos(UINT32 userId, UINT32 roleId, std::vector<SChainSoulPosAttr>& posAttr);

	//zpy ����ƥ��
	int LadderMatching(UINT32 userId, UINT32 roleId, UINT32 matching_rank[LADDER_MATCHTING_MAX_NUM], UINT16 *refresh_count, UINT32 *rpcoin);
	//zpy ˢ���������з�Χ
	int RefreshLadderRankScope(UINT32 userId, UINT32 roleId, UINT32 matching_rank[LADDER_MATCHTING_MAX_NUM]);
	//zpy ִ��������ս
	int DoLadderChallenge(UINT32 userId, UINT32 roleId, UINT16 *ret_challenge_count);
	//zpy ������ȴʱ��
	int EliminateLadderCoolingTime(UINT32 userId, UINT32 roleId, SEliminateLadderCDTimeRes *pEliminate);
	//zpy ���������ս
	int FinishLadderChallenge(UINT32 userId, UINT32 roleId, BYTE finishCode/*, SFinishLadderChallengeRes *pFinishLadder*/);
	//zpy ���ñ���ս���������з�Χ
	int ResetChallengerLadderRankScope(UINT32 userId, UINT32 roleId);

	//zpy ��ӻ�����
	int AddRobot(UINT32 userId, UINT32 roleId, UINT32 robotId);
	//zpy �Ƿ���ڻ�����
	int HasRobot(UINT32 userId, UINT32 roleId, UINT32 robotId, bool *ret);
	//zpy ���������
	int ClearRobot(UINT32 userId, UINT32 roleId);

	/* zpy ���򾺼�ģʽ������� */
	int BuyCompModeEnterNum(UINT32 userId, UINT32 roleId, int purchased, SBuyCompEnterNumRes *pEnterNum);
	int BuyAllCompModeEnterNum(UINT32 userId, UINT32 roleId, int purchased, int upper_num, SBuyCompEnterNumRes *pEnterNum);

	/* zpy ����1vs1ģʽ������� */
	int BuyoneVsOneEnterNum(UINT32 userId, UINT32 roleId, int purchased, int num, SBuy1V1EnterNumRes *pBuyEnter);

	//BY add hxw 20151016
	//��ȡ����ͨ�ý���
	int ReceiveRankReward(UINT32 userId, UINT32 roleId, BYTE type, UINT16 rewardid, SGetRankRewardRes *pRoleSign, SReceiveRewardRefresh *pRefresh);
	//add by hxw 20151022 
	BOOL IsGetStrongerReward(UINT32 userId, UINT32 roleId, UINT16 rewardid);
	BOOL IsGetOnlineReward(UINT32 userId, UINT32 roleId, UINT16 rewardid);
	BOOL GetOnlineInfo(UINT32 userId, UINT32 roleId, SOnlineInfo* info, std::vector<UINT16>& vonline);
	BOOL GetStrongerInfo(UINT32 userId, UINT32 roleId, std::vector<UINT16>& infov);
	int InputStrongerReward(UINT32 userId, UINT32 roleId, UINT16 rewardid);
	int InputOnlineReward(UINT32 userId, UINT32 roleId, UINT16 rewardid);
	int SetOnlineInfo(UINT32 userId, UINT32 roleId, UINT32 id, UINT32 time, LTMSEL lastTime = 0);
	//end 

private:
	RoleSite* NewRoleSite();
	void DeleteRoleSite(RoleSite* site);

	RoleSite* GetRoleSite(UINT32 userId);
	RoleSite* FetchRoleSite(UINT32 userId);

private:
#if USE_MEM_POOL
	CMemPoolObj<RoleSite> m_roleSitePool;
#endif
	UINT32 m_totalRoles;	//���н�ɫ����
	CXTLocker m_userRolesMapLocker;
	std::map<UINT32/*�˺�id*/, RoleSite*> m_userRolesMap;	//�û���ɫ��
};

void InitRolesInfoMan();
RolesInfoMan* GetRolesInfoMan();
void DestroyRolesInfoMan();

#endif