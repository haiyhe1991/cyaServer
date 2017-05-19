#ifndef __MLOGIC_PROCESS_MAN_H__
#define __MLOGIC_PROCESS_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
�߼�������
*/

#include <map>
#include "ServiceProtocol.h"
#include "GWSProtocol.h"

class GWSClient;
class MLogicProcessMan
{
	MLogicProcessMan(const MLogicProcessMan&);
	MLogicProcessMan& operator = (const MLogicProcessMan&);
public:
	MLogicProcessMan();
	~MLogicProcessMan();

	int ProcessLogic(GWSClient* gwsSession, UINT16 cmdCode, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

private:	//�û�������
	//��ѯ��ɫ��Ϣ
	int OnQueryRoles(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//������ɫ
	int OnCreateRole(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//ɾ����ɫ
	int OnDelRole(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//�����ɫ
	int OnEnterRoles(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//�˳���ɫ
	int OnLeaveRoles(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//�˺��˳�
	int OnUserExit(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//��ѯ����
	int OnQueryBackpack(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��������
	int OnBackpackCleanup(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ѯ��ɫ������Ϣ
	int OnQueryRoleApperance(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ѯ��ɫ��ϸ��Ϣ
	int OnQueryRoleExplicit(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//���򱳰���ֿ�洢�ռ�
	int OnBuyStorageSpace(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ѯ��������
	int OnQueryGuildName(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//��ѯ�ʼ�����
	int OnQueryEmailContent(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ȡ�ʼ�����
	int OnReceiveEmailAttachments(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ѯ�ʼ��б�
	int OnQueryEmailList(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//ɾ���ʼ�
	int OnDelEmails(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//��ѯ����
	int OnQueryFriends(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ѯ���������б�
	int OnQueryFriendsApply(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��Ӻ���
	int OnAddFriend(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��������ȷ��
	int OnFriendApplyConfirm(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//ɾ������
	int OnDelFriend(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ȡ�Ƽ������б�
	int OnGetRecommendFriends(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//�������
	int OnSearchPlayer(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//��ѯ���������������Ϣ
	int OnQueryFinishedHelp(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//���ĳ����������Ϣ
	int OnFinishNewPlayerHelp(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ѯ��ɫħ����
	int OnQueryMagics(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//���ħ����
	int OnGainMagic(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//������Ʒ
	int OnSellProperty(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//������Ʒ
	int OnDropProperty(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//ʹ�õ���
	int OnUseProperty(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//�һ�����
	int OnCashProperty(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��Ʒ�ع�
	int OnBuyBackProperty(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ѯ��Ʒ���ۼ�¼
	int OnQueryPropSellRcd(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//���븱��
	int OnEnterInst(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ȡ��������ֵ���
	int OnFetchInstAreaMonsterDrops(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ѯ����ɸ�����Ϣ
	int OnQueryFinishedInst(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//ʰȡ�����ֵ�����Ʒ
	int OnPickupInstMonsterDrop(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//ʰȡ��������
	int OnPickupInstBox(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//ʰȡ���������
	int OnPickupBrokenDrop(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//ɱ�ֵþ���
	int OnKillMonsterExp(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ɸ���
	int OnFinishInst(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//��ʯ�ϳ�
	int OnGemCompose(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//����װ��
	int OnLoadEquipment(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//ж��װ��
	int OnUnLoadEquipment(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//װ���ϳ�
	int OnEquipmentCompose(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//װ��λǿ��
	int OnEquipPosStrengthen(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//װ��λ����
	int OnEquipPosRiseStar(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//װ��λ��ʯ��Ƕ
	int OnEquipPosInlaidGem(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//װ��λ��ʯȡ��
	int OnEquipPosRemoveGem(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ȡװ��λ����
	int OnGetEquipPosAttr(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//��ȡ�ɾͽ���
	int OnReceiveAchievementReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ѯ�ɾ�
	int OnQueryAchievement(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//��ѯ��ɫ����
	int OnQueryRoleSkills(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��������
	int OnSkillUpgrade(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//�ı���Ӫ
	int OnChangeFaction(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//װ�ؼ���
	int OnLoadSkill(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//ȡ������
	int OnCancelSkill(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//��������
	int OnAcceptTask(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ʱ��������
	int OnReqTimerTask(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ѯ�ѽ�������
	int OnQueryAcceptedTasks(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��������
	int OnDropTask(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//�ύ�������
	int OnCommitTaskFinish(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ѯ�����������ʷ��¼
	int OnQueryFinTaskHistory(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ȡ�ճ�����
	int OnGetDailyTask(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//�����ѽ�����Ŀ��
	int OnUpdateTaskTarget(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ȡ������
	int OnReceiveTaskReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//��ֵ
	int OnRechargeToken(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ȡ��ɫռ�츱��
	int OnGetRoleOccupyInst(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//���ý�ɫְҵ�ƺ�
	int OnSetRoleTitleId(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//�̳ǹ�����Ʒ
	int OnMallBuyGoods(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ѯ��ɫʱװ
	int OnQueryRoleDress(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ɫʱװ����
	int OnWearDress(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ȡ����������
	int OnFetchRobot(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//��ȡ������������������������
	int OnGetPersonalTransferEnter(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ȡ�Ѵ򿪵����������
	int OnGetOpenPersonalRandInst(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//��ѯװ����Ƭ
	int OnQueryEquipFragment(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//����
	int OnRoleRevive(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ѯ��Ҷһ�
	int OnQueryGoldExchange(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��Ҷһ�
	int OnGoldExchange(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//��ѯδ��ȡ������VIP�ȼ�
	int OnQueryNotReceiveVIPReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ȡVIP����
	int OnReceiveVIPReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ѯ�ѳ�ֵ�����׳����͵ĳ�ֵid
	int OnQueryRechargeFirstGive(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//��ѯǩ��
	int OnQuerySignIn(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//����ǩ��
	int OnlineSignIn(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ѯ��ɫ����ȡ�
	int OnQueryReceivedActivity(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ȡ�
	int OnReceivedActivity(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//��ѯ��ǰ�����½�
	int OnQueryCurOpenChapter(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//����ɨ��
	int OnInstSweep(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ȡ�һ��뽱��
	int OnReceiveCashCodeReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ѯ�˺Ž�������
	int OnQueryUserUnlockContent(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��������
	int OnUnlockContent(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��֤��������ѯ�˺Ž�������
	int OnLcsQueryUserUnlockContent(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//��ȡ����ʣ�ཱƷ
	int OnGetRouletteRemainReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ȡ���̽�Ʒ
	int OnExtractRouletteReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//ˢ�����̴�
	int OnUpdateRouletteBigReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ȡ���̴�
	int OnFetchRouletteBigReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ȡ��ֵ����
	int OnFetchRechargeOrder(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ȡ��ֵ����
	int OnGetRechargeToken(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//��ȡ�������ս����
	int OnGetActiveInstChallengeTimes(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	/* zpy �����һ� */
	//��ѯ�����һ�
	int OnQueryVitExchange(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//�����һ�
	int OnVitExchange(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	/* zpy ��ѯÿ��ǩ�� */
	int OnQueryRoleDailySign(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	/* zpy ÿ��ǩ�� */
	int OnRoleDailySign(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//add by hxw 20151028
	//��ѯ�ȼ����е����а���Ϣ
	int QueryLvRankInfo(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//end
	//add by hxw 20151006
	//��ѯ�ȼ����н�����Ϣ
	int QueryLvRank(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	// ��ѯ�Ƿ���ȡ������ͼ��������
	int QueryInsnumRank(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ѯ������ͼ���е����а�
	int QueryInsnumRankInfo(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ȡ�ȼ����н���
	int GetLvRankReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ȡ������ս�������н���
	int GetInsnumRankReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ѯǿ�߽�����ս��)��ȡ���
	int QueryStrongerInfo(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ȡǿ�߽���
	int GetStrongerReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��ѯ���߽���
	int QueryOnlineInfo(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	int GetOnlineReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//end

	/* zpy ��ѯ������Ƭ */
	int OnQueryChainSoulFragment(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	/* zpy ��ѯ���겿�� */
	int OnQueryChainSoulPos(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	/* zpy �������겿�� */
	int OnChainSoulPosUpgrade(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	/* zpy ��������Boos */
	int OnAttackWorldBoss(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	/* zpy ��ѯ����Boos���а� */
	int OnQueryWorldBossHurtRank(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	/* zpy ������Bossɱ�� */
	int OnWasKilledByWorldBoss(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	/* zpy ������Boss�����и��� */
	int OnResurrectionInWorldBoss(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	/* zpy ��ѯ����Boss��Ϣ */
	int OnQueryWorldBossInfo(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	/* zpy ��ȡ������Ϣ */
	int OnGetLadderInfo(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	/* zpy ����ƥ����� */
	int OnLadderMatchingPlayer(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	/* zpy ˢ������������Ϣ */
	int OnRefreshLadderRankScope(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	/* zpy ������ս��� */
	int OnLadderChallengePlayer(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	/* zpy ��������CDʱ�� */
	int OnEliminateLadderCoolingTime(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	/* zpy ���������ս */
	//int OnFinishLadderChallenge(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	/* zpy ��ѯ���н�ɫ */
	int OnQueryAllRoles(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	/* zpy ��ѯӵ�еĻ�������� */
	int OnQueryHasAcivityInstType(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	/* zpy ��ѯ�����ɫ���� */
	int OnQueryWeekFreeActorType(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	/* zpy ��ѯ����ģʽ������� */
	int OnQueryEnterKOFNum(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	/* zpy ���򾺼�ģʽ������� */
	int OnBuyEnterKOFNum(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	/* zpy ��ѯ1v1����������� */
	int OnQueryEnterOneVsOneNum(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	/* zpy ����1v1����������� */
	int OnBuyEnterOneVsOneNum(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

private:
	typedef int (MLogicProcessMan::*fnMsgProcessHandler)(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	std::map<UINT16, fnMsgProcessHandler> m_msgProcHandler;
};

void InitMLogicProcessMan();
MLogicProcessMan* GetMLogicProcessMan();
void DestroyMLogicProcessMan();

#endif