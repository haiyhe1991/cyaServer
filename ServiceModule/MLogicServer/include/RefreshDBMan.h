#ifndef __REFRESH_DB_MAN_H__
#define __REFRESH_DB_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
刷新数据库
*/

#include <map>
#include <vector>
#include "cyaCoreTimer.h"

//add by hxw 20151116
const int CASHDB_TABLE_NUM = 5;
//end

enum ERefreshRoleDataType
{
	REFRESH_ROLE_CP = 1,	//刷新战力
	REFRESH_ROLE_BACKPACK = 2,	//刷新背包
	REFRESH_ROLE_TASK = 3,	//刷新已接受任务
	REFRESH_ROLE_STORAGE = 4,	//刷新存储空间
	REFRESH_ROLE_FRAGMENT = 5,	//刷新角色装备碎片
	REFRESH_ROLE_GOLD = 6,	//刷新金币
	REFRESH_ROLE_VIST_INFO = 7,	//刷新角色访问信息
	REFRESH_ROLE_SKILLS = 8,	//刷新角色已学技能
	REFRESH_ROLE_FIN_INST = 9,	//刷新角色完成副本
	REFRESH_ROLE_VIT = 10,	//刷新角色体力
	REFRESH_ROLE_UPGRADE = 11,	//刷新角色升级
	REFRESH_ROLE_FACTION = 12,	//刷新角色阵营
	REFRESH_ROLE_WEAR_BACKPACK = 13,	//刷新角色装备穿戴, 背包
	REFRESH_ROLE_WEAR = 14,	//刷新角色穿戴
	REFRESH_ROLE_TITLE_ID = 15,	//刷新角色职业称号id
	REFRESH_ROLE_CASH = 16,	//刷新角色现金/代币
	REFRESH_ROLE_DRESS = 17,	//刷新角色时装
	REFRESH_ROLE_DRESS_WEAR = 18,	//刷新角色时装穿戴
	REFRESH_ROLE_GOLD_EXCHANGE = 19,	//刷新角色金币兑换
	REFRESH_ROLE_FIN_TASK = 20,	//刷新角色完成任务
	REFRESH_ROLE_EXP = 21,	//刷新角色经验
	REFRESH_ROLE_EQUIP_POS = 22,	//刷新角色装备位

	/* zpy 成就系统新增 */
	REFRESH_ROLE_KILL_MONSTER = 23, //刷新角色杀怪数量
	REFRESH_ROLE_PICKUP_BOX_SUM = 24, //刷新角色打开宝箱数量
	REFRESH_ROLE_COMBAT_PERFORMANCE = 25, //刷新角色战斗表现

	/* zpy 炼魂系统新增 */
	REFRESH_ROLE_CHAINSOUL_FRAGMENT = 26,	//刷新炼魂碎片
	REFRESH_ROLE_CHAINSOUL_POS = 27,		//刷新炼魂部件

	/* zpy 天梯系统新增 */
	REFRESH_ROLE_LADDER_INFO = 28,			//刷新天梯信息
	REFRESH_ROLE_LADDER_MATCHING_INFO = 29,	//刷新天梯匹配信息
	REFRESH_ROLE_RESET_LADDER_RANK_VIEW = 30,	//重置天梯视图
};

#define REFRESH_ROLE_ID_START	REFRESH_ROLE_CP
#define REFRESH_ROLE_ID_END		REFRESH_ROLE_RESET_LADDER_RANK_VIEW

class RefreshDBMan
{
	RefreshDBMan(const RefreshDBMan&);
	RefreshDBMan& operator = (const RefreshDBMan&);

	struct SUpdateDBInfo	//需要更新的角色
	{
		UINT32 userId;	//用户id
		UINT32 roleId;	//角色id
		UINT16 type;	//需要更新状态

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

	//需要更新的角色信息
	void InputUpdateInfo(UINT32 userId, UINT32 roleId, UINT16 type);
	//用户退出时同步
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

	/* zpy 成就系统新增 */
	void OnUpdateRoleKillMonster(UINT32 userId, UINT32 roleId);
	void OnUpdateRolePickupBoxSum(UINT32 userId, UINT32 roleId);
	void OnUpdateRoleCombatPerformance(UINT32 userId, UINT32 roleId);

	/* zpy 炼魂系统新增 */
	void OnUpdateRoleChainSoulFragment(UINT32 userId, UINT32 roleId);
	void OnUpdateRoleChainSoulPos(UINT32 userId, UINT32 roleId);

	/* zpy 天梯系统新增 */
	void OnUpdateLadderInfo(UINT32 userId, UINT32 roleId);
	void OnUpdateLadderMatchingInfo(UINT32 userId, UINT32 roleId);
	void OnResetLadderView(UINT32 userId, UINT32 roleId);

	//mid_cash add by hxw 20151116 
	//写入代币到角色数据库
	INT UpdateCashToActorDB(UINT32 roleId, UINT32 cash, UINT32 cashcount = -1);
	//写入代币到代币数据库
	INT UpdateCashToCashDB(UINT32 userID, UINT32 cash, UINT32 cashcount = -1);

	//end
private:
	CyaCoreTimer m_refreshDBTimer;

	//角色数据信息
	CXTLocker m_locker;
	std::map<std::string/*key*/, SUpdateDBInfo> m_updateMap;
};

void InitRefreshDBMan();
RefreshDBMan* GetRefreshDBMan();
void DestroyRefreshDBMan();

#endif	//_RefreshDBMan_h__