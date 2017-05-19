#ifndef __REFRESH_DB_MAN_H__
#define __REFRESH_DB_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
ˢ�����ݿ�
*/

#include <map>
#include <vector>
#include "cyaCoreTimer.h"

//add by hxw 20151116
const int CASHDB_TABLE_NUM = 5;
//end

enum ERefreshRoleDataType
{
	REFRESH_ROLE_CP = 1,	//ˢ��ս��
	REFRESH_ROLE_BACKPACK = 2,	//ˢ�±���
	REFRESH_ROLE_TASK = 3,	//ˢ���ѽ�������
	REFRESH_ROLE_STORAGE = 4,	//ˢ�´洢�ռ�
	REFRESH_ROLE_FRAGMENT = 5,	//ˢ�½�ɫװ����Ƭ
	REFRESH_ROLE_GOLD = 6,	//ˢ�½��
	REFRESH_ROLE_VIST_INFO = 7,	//ˢ�½�ɫ������Ϣ
	REFRESH_ROLE_SKILLS = 8,	//ˢ�½�ɫ��ѧ����
	REFRESH_ROLE_FIN_INST = 9,	//ˢ�½�ɫ��ɸ���
	REFRESH_ROLE_VIT = 10,	//ˢ�½�ɫ����
	REFRESH_ROLE_UPGRADE = 11,	//ˢ�½�ɫ����
	REFRESH_ROLE_FACTION = 12,	//ˢ�½�ɫ��Ӫ
	REFRESH_ROLE_WEAR_BACKPACK = 13,	//ˢ�½�ɫװ������, ����
	REFRESH_ROLE_WEAR = 14,	//ˢ�½�ɫ����
	REFRESH_ROLE_TITLE_ID = 15,	//ˢ�½�ɫְҵ�ƺ�id
	REFRESH_ROLE_CASH = 16,	//ˢ�½�ɫ�ֽ�/����
	REFRESH_ROLE_DRESS = 17,	//ˢ�½�ɫʱװ
	REFRESH_ROLE_DRESS_WEAR = 18,	//ˢ�½�ɫʱװ����
	REFRESH_ROLE_GOLD_EXCHANGE = 19,	//ˢ�½�ɫ��Ҷһ�
	REFRESH_ROLE_FIN_TASK = 20,	//ˢ�½�ɫ�������
	REFRESH_ROLE_EXP = 21,	//ˢ�½�ɫ����
	REFRESH_ROLE_EQUIP_POS = 22,	//ˢ�½�ɫװ��λ

	/* zpy �ɾ�ϵͳ���� */
	REFRESH_ROLE_KILL_MONSTER = 23, //ˢ�½�ɫɱ������
	REFRESH_ROLE_PICKUP_BOX_SUM = 24, //ˢ�½�ɫ�򿪱�������
	REFRESH_ROLE_COMBAT_PERFORMANCE = 25, //ˢ�½�ɫս������

	/* zpy ����ϵͳ���� */
	REFRESH_ROLE_CHAINSOUL_FRAGMENT = 26,	//ˢ��������Ƭ
	REFRESH_ROLE_CHAINSOUL_POS = 27,		//ˢ�����겿��

	/* zpy ����ϵͳ���� */
	REFRESH_ROLE_LADDER_INFO = 28,			//ˢ��������Ϣ
	REFRESH_ROLE_LADDER_MATCHING_INFO = 29,	//ˢ������ƥ����Ϣ
	REFRESH_ROLE_RESET_LADDER_RANK_VIEW = 30,	//����������ͼ
};

#define REFRESH_ROLE_ID_START	REFRESH_ROLE_CP
#define REFRESH_ROLE_ID_END		REFRESH_ROLE_RESET_LADDER_RANK_VIEW

class RefreshDBMan
{
	RefreshDBMan(const RefreshDBMan&);
	RefreshDBMan& operator = (const RefreshDBMan&);

	struct SUpdateDBInfo	//��Ҫ���µĽ�ɫ
	{
		UINT32 userId;	//�û�id
		UINT32 roleId;	//��ɫid
		UINT16 type;	//��Ҫ����״̬

		SUpdateDBInfo()
		{
			userId = 0;
			roleId = 0;
			type = 0;
		}

		SUpdateDBInfo(UINT32 user, UINT32 role, UINT16 nType)
		{
			userId = user;
			roleId = role;
			type = nType;
		}
	};

public:
	RefreshDBMan();
	~RefreshDBMan();

	//��Ҫ���µĽ�ɫ��Ϣ
	void InputUpdateInfo(UINT32 userId, UINT32 roleId, UINT16 type);
	//�û��˳�ʱͬ��
	void ExitSyncUserRoleInfo(UINT32 userId, const std::vector<UINT32>& rolesVec);

private:
	static void RefreshRoleTimer(void* param, TTimerID id);
	void OnRefreshRoleInfo();
	void OnRefreshRoleInfo(const SUpdateDBInfo& upInfo);

private:
	void OnUpdateRoleCompatPower(UINT32 userId, UINT32 roleId);
	void OnUpdateRoleBackpack(UINT32 userId, UINT32 roleId);
	void OnUpdateRoleAcceptTask(UINT32 userId, UINT32 roleId);
	void OnUpdateRoleStorageSpace(UINT32 userId, UINT32 roleId);
	void OnUpdateRoleGold(UINT32 userId, UINT32 roleId);
	void OnUpdateRoleVistInfo(UINT32 userId, UINT32 roleId);
	void OnUpdateRoleSkills(UINT32 userId, UINT32 roleId);
	void OnUpdateRoleFinTask(UINT32 userId, UINT32 roleId);
	void OnUpdateRoleVit(UINT32 userId, UINT32 roleId);
	void OnUpdateRoleUpgrade(UINT32 userId, UINT32 roleId);
	void OnUpdateRoleFaction(UINT32 userId, UINT32 roleId);
	void OnUpdateRoleWearBackpack(UINT32 userId, UINT32 roleId);
	void OnUpdateRoleWear(UINT32 userId, UINT32 roleId);
	void OnUpdateRoleTitleId(UINT32 userId, UINT32 roleId);
	void OnUpdateRoleCash(UINT32 userId, UINT32 roleId);
	void OnUpdateRoleDress(UINT32 userId, UINT32 roleId);
	void OnUpdateRoleDressWear(UINT32 userId, UINT32 roleId);
	void OnUpdateRoleFragment(UINT32 userId, UINT32 roleId);
	void OnUpdateRoleGoldExchange(UINT32 userId, UINT32 roleId);
	void OnUpdateRoleFinInst(UINT32 userId, UINT32 roleId);
	void OnUpdateRoleExp(UINT32 userId, UINT32 roleId);
	void OnUpdateRoleEquipPos(UINT32 userId, UINT32 roleId);

	/* zpy �ɾ�ϵͳ���� */
	void OnUpdateRoleKillMonster(UINT32 userId, UINT32 roleId);
	void OnUpdateRolePickupBoxSum(UINT32 userId, UINT32 roleId);
	void OnUpdateRoleCombatPerformance(UINT32 userId, UINT32 roleId);

	/* zpy ����ϵͳ���� */
	void OnUpdateRoleChainSoulFragment(UINT32 userId, UINT32 roleId);
	void OnUpdateRoleChainSoulPos(UINT32 userId, UINT32 roleId);

	/* zpy ����ϵͳ���� */
	void OnUpdateLadderInfo(UINT32 userId, UINT32 roleId);
	void OnUpdateLadderMatchingInfo(UINT32 userId, UINT32 roleId);
	void OnResetLadderView(UINT32 userId, UINT32 roleId);

	//mid_cash add by hxw 20151116 
	//д����ҵ���ɫ���ݿ�
	INT UpdateCashToActorDB(UINT32 roleId, UINT32 cash, UINT32 cashcount = -1);
	//д����ҵ��������ݿ�
	INT UpdateCashToCashDB(UINT32 userID, UINT32 cash, UINT32 cashcount = -1);

	//end
private:
	CyaCoreTimer m_refreshDBTimer;

	//��ɫ������Ϣ
	CXTLocker m_locker;
	std::map<std::string/*key*/, SUpdateDBInfo> m_updateMap;
};

void InitRefreshDBMan();
RefreshDBMan* GetRefreshDBMan();
void DestroyRefreshDBMan();

#endif	//_RefreshDBMan_h__