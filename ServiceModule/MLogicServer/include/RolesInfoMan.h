#ifndef __ROLES_INFO_MAN_H__
#define __ROLES_INFO_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
用户角色信息管理类
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

		//查询用户角色信息
		int QueryUserRoles(UINT32 userId, SQueryRolesRes* pRoles);
		//查询拥有的角色数量 add by hxw 20151210
		int QueryUserRolesNum();

		//查询角色外形信息
		int QueryRoleAppearance(UINT32 roleId, SQueryRoleAppearanceRes* pRoleRes);
		//查询角色详细信息
		int QueryRoleExplicit(UINT32 roleId, SQueryRoleExplicitRes* pRoleRes);
		//进入角色 //20151121 新增userid 作为钻石同步 by hxw
		int EnterRole(UINT32 userId, UINT32 roleId, SEnterRoleRes* pEnterRes);
		//退出角色
		int LeaveRole(UINT32 roleId);
		//创建角色
		int CreateRole(UINT32 userId, BYTE occuId, BYTE sex, const char* roleNick, SCreateRoleRes* pCreateRes);
		//删除角色
		int DelRole(UINT32 roleId);
		//改变阵营
		int ChangeFaction(UINT32 roleId, BYTE factionId, BOOL& needSyncDB);

		//zpy 查询所有角色的ID
		int QueryAllRoles(UINT32 userId, SQueryAllRolesRes* pAllRoles);

		/* zpy 成就系统新增 */
		int RemoveAllRole();

		//查询背包信息
		int QueryBackpack(UINT32 roleId, BYTE from, BYTE nums, SQueryBackpackRes* pBackpackRes);
		//背包整理
		int CleanupBackpack(UINT32 roleId, BOOL& isCleanup);
		//丢弃背包道具
		int DropBackpackProperty(UINT32 roleId, UINT32 id, BYTE num);
		//使用道具
		int UseProperty(UINT32 roleId, UINT32 id, UINT16& propId, SUsePropertyRes *pUseProperty);
		//兑换道具
		int CashProperty(UINT32 roleId, UINT16 cashId, SCashPropertyRes* pCashRes);
		//购买背包或仓库存储空间
		int BuyStorageSpace(UINT32 roleId, BYTE buyType, BYTE num, UINT32& spend, BYTE& newNum);
		//出售物品
		int SellProperty(UINT32 roleId, UINT32 id, BYTE num, SSellPropertyRes* pSellRes, SPropertyAttr& propAttr);
		//物品回购
		int BuyBackProperty(UINT32 roleId, SPropertyAttr& buyProp, BYTE num, SPropBuyBackRes* pBuyBackRes);

		//装备装备
		int LoadEquipment(UINT32 roleId, UINT32 id, BYTE pos);
		//卸载装备
		int UnLoadEquipment(UINT32 roleId, UINT32 id);

		//装备合成
		int EquipmentCompose(UINT32 roleId, UINT16 composeId, SEquipComposeRes* pComposeRes);

		//进入副本
		int EnterInst(UINT32 roleId, UINT16 instId, SEnterInstRes* pEnterInstRes, int& dataLen, LTMSEL enterTTS, PlayerActionMan::SEnterInstItem& enterInstItem);
		//获取副本区域怪掉落
		int FetchInstAreaMonsterDrops(UINT32 roleId, UINT16 instId, BYTE areaId, SFetchInstAreaDropsRes* pAreaDropsRes);
		//拾取副本怪掉落
		int PickupInstMonsterDrop(UINT32 userId, UINT32 roleId, UINT16 instId, BYTE areaId, BYTE dropId, SPickupInstDropNotify* pNotify);
		//拾取副本破碎物掉落
		int PickupInstBrokenDrop(UINT32 userId, UINT32 roleId, UINT16 instId, BYTE dropId, SPickupInstDropNotify* pNotify);
		//拾取副本宝箱
		int PickupInstBoxDrop(UINT32 userId, UINT32 roleId, UINT16 instId, BYTE boxId, BYTE id, SPickupInstDropNotify* pNotify);
		//杀怪得经验
		int KillMonsterExp(UINT32 roleId, UINT16 instId, BYTE areaId, UINT16 monsterId, BYTE monsterLv, BOOL& isUpgrade);
		//完成副本
		int FinishInst(UINT32 userId, UINT32 roleId, UINT16 instId, UINT32 score, BYTE star, LTMSEL& enterTTS, const std::vector<SPropertyItem> &rewardVec, SFinishInstRes* pFinishInstRes, SReceiveRewardRefresh *pRefresh);

		/* zpy 成就系统新增 */
		int GetMaxGemLevel(UINT32 roleId, BYTE &max_level);
		int GetInlaidGemLevelSum(UINT32 roleId, UINT32 &sum);
		int GetEquipStarLevelSum(UINT32 roleId, UINT32 &sum);
		int GetSkillLevelSum(UINT32 roleId, UINT32 &sum);

		//查询角色技能
		int QueryRoleSkills(UINT32 roleId, SQuerySkillsRes* pSkills);
		//技能升级
		int SkillUpgrade(UINT32 roleId, UINT16 skillId, BYTE step, SSkillUpgradeRes* pUpgradeRes);
		//装载技能
		int LoadSkill(UINT32 roleId, UINT16 skillId, BYTE key, BOOL& needSyncDB);
		//取消技能
		int CancelSkill(UINT32 roleId, UINT16 skillId, BOOL& needSyncDB);

		//接受任务
		int AcceptTask(UINT32 roleId, UINT16 taskId);
		//查询已接受任务
		int QueryAcceptedTasks(UINT32 roleId, SQueryAcceptTasksRes* pTaskRes);
		//放弃任务
		int DropTask(UINT32 roleId, UINT16 taskId);
		//提交任务完成
		int CommitTaskFinish(UINT32 roleId, UINT16 taskId, SFinishTaskFailNotify* pFinTaskNotify);
		//获取日常任务
		int GetDailyTasks(UINT32 roleId, SGetDailyTaskRes* pDailyTasks);
		//更新任务目标
		int UpdateTaskTarget(UINT32 roleId, UINT16 taskId, const UINT32* pTarget, int n = MAX_TASK_TARGET_NUM);
		//领取任务奖励
		int ReceiveTaskReward(UINT32 userId, UINT32 roleId, UINT16 taskId, SReceiveTaskRewardRes* pTaskRewardRes);

		//装备位镶嵌宝石
		int EquipPosInlaidGem(UINT32 roleId, BYTE pos, UINT32 gemId, BYTE holeSeq);
		//装备位取出宝石
		int EquipPosRemoveGem(UINT32 roleId, BYTE pos, BYTE holeSeq, SEquipPosRemoveGemRes* pRemoveRes);
		//宝石合成
		int	GemCompose(UINT32 roleId, BYTE num, const SConsumeProperty* pMaterial, SGemComposeRes* pComposeRes);

		//收取邮件附件
		int ReceiveEmailAttachments(UINT32 roleId, BYTE emailType, const SPropertyAttr* pAttachmentsProp, BYTE attachmentsNum, SReceiveEmailAttachmentsRes* pEmailAttachments, BOOL& isUpgrade);
		//充值
		int RechargeToken(UINT32 roleId, UINT16 cashId, SRechargeRes* pRechargeRes, char* pszRoleNick);
		//设置角色称号id
		int SetRoleTitleId(UINT32 roleId, UINT16 titleId);
		//商城购买物品
		int MallBuyGoods(UINT32 roleId, UINT32 buyId, UINT16 num, SMallBuyGoodsRes* pBuyGoodsRes, char* pszRoleNick, UINT32& spendToken);

		//查询角色时装
		int QueryRoleDress(UINT32 roleId, SQueryRoleDressRes* pDressRes);
		//角色时装穿戴
		int WearDress(UINT32 roleId, UINT16 dressId);
		//查询角色装备碎片
		int QueryEquipFragment(UINT32 roleId, SQueryEquipFragmentRes* pFragmentRes);

		//复活
		int RoleRevive(UINT32 roleId, UINT16 reviveId, SReviveRoleRes* pReviveRes);
		//查询金币兑换
		int QueryGoldExchange(UINT32 roleId, SQueryGoldExchangeRes* pExchangeRes);
		//金币兑换
		int GoldExchange(UINT32 roleId, SGoldExchangeRes* pExchangeRes);
		//推荐好友
		int GetRecommendFriends(UINT32 userId, UINT32 roleId, SGetRecommendFriendsRes* pFriendsRes);

		//领取VIP奖励
		int ReceiveVIPReward(UINT32 userId, UINT32 roleId, BYTE vipLv, SReceiveVIPRewardRes* pVIPRewardRes, SReceiveRewardRefresh* pRefresh);

		/* zpy 成就系统新增 */
		int ReceiveAchievementReward(UINT32 userId, UINT32 roleId, UINT32 achievement_id, SReceiveRewardRes *pAchievementReward, SReceiveRewardRefresh *pRefresh);

		//查询新手引导
		int QueryRoleHelps(UINT32 roleId, SQueryFinishedHelpRes* pHelpRes);
		//完成新手引导
		int FinishRoleHelp(UINT32 roleId, UINT16 helpId);
		//查询角色拥有魔导器
		int QueryRoleMagics(UINT32 roleId, SQueryRoleMagicsRes* pMagicsRes);
		//获得魔导器
		int GainMagic(UINT32 roleId, UINT16 instId, UINT16 magicId);

		//装备位强化
		int EquipPosStrengthen(UINT32 roleId, BYTE pos, SEquipPosStrengthenRes* pStrengthenRes);
		//装备位升星
		int EquipPosRiseStar(UINT32 roleId, BYTE pos, const SRiseStarAttachParam& param, SEquipPosRiseStarRes* pRiseStarRes);

		//查询签到
		int QuerySignIn(UINT32 roleId, SQueryOnlineSignInRes* pSignInRes);
		//签到
		int SignIn(UINT32 userId, UINT32 roleId);
		//副本扫荡
		int InstSweep(UINT32 userId, UINT32 roleId, UINT16 instId, SInstSweepRes* pSweepRes, BOOL& isUpgrade);
		//解锁内容
		int UnlockContent(UINT32 roleId, BYTE type, UINT16 unlockId);

		//抽取轮盘奖品
		int ExtractRouletteReward(UINT32 userId, UINT32 roleId, const SRoulettePropItem& extractReward, SExtractRouletteRewardRes* pRewardRes, BOOL& isUpgrade);
		//获取轮盘大奖
		int FetchRouletteBigReward(UINT32 userId, UINT32 roleId, const SRoulettePropItem& bigReward, SFetchRouletteBigRewardRes* pRewardRes, BOOL& isUpgrade);
		//刷新轮盘大奖
		int UpdateRouletteBigReward(UINT32 roleId, UINT32 spendToken, UINT32& curToken);

		//查询体力兑换
		int QueryVitExchange(UINT32 roleId, SQueryVitExchangeRes* pExchangeRes);
		//体力兑换
		int VitExchange(UINT32 roleId, SVitExchangeRes* pExchangeRes);

		/* zpy 领取每日签到奖励 */
		int ReceiveDailySignReward(UINT32 userId, UINT32 roleId, BYTE days, bool continuous, SRoleDailySignRes *pRoleSign, SReceiveRewardRefresh *pRefresh);

		/* zpy 查询炼魂碎片 */
		int QueryChainSoulFragment(UINT32 roleId, SQueryChainSoulFragmentRes* pFragmentRes);
		/* zpy 查询炼魂部件 */
		int QueryChainSoulPos(UINT32 roleId, SQueryChainSoulPosRes* pChainSoulPos);
		/* zpy 升级炼魂部件 */
		int ChainSoulPosUpgrade(UINT32 roleId, BYTE pos, SChainSoulPosUpgradeRes* pChainSoulPosUpgrade);

		/* zpy 复活角色 */
		int ResurrectionInWorldBoss(UINT32 roleId, UINT16 reviveId, UINT32 consume_count, UINT32 &out_rpcoin);

		/* zpy 购买竞技模式进入次数 */
		int BuyCompModeEnterNum(UINT32 roleId, int purchased, SBuyCompEnterNumRes *pEnterNum);
		int BuyAllCompModeEnterNum(UINT32 roleId, int purchased, int upper_num, SBuyCompEnterNumRes *pEnterNum);

		/* zpy 购买1vs1模式进入次数 */
		int BuyoneVsOneEnterNum(UINT32 roleId, int purchased, int num, SBuy1V1EnterNumRes *pBuyEnter);

	public:
		//角色是否在线
		BOOL IsRoleOnline(UINT32 roleId);
		//获取角色背包数据
		int GetRoleBackpack(UINT32 roleId, std::vector<SPropertyAttr>& vec);
		//获取角色已接收任务
		int GetRoleAcceptTask(UINT32 roleId, std::vector<SAcceptTaskInfo>& vec);
		//角色背包/仓库大小
		int GetRoleStorageSpace(UINT32 roleId, BYTE& packSize, BYTE& wareSize);
		//获取角色金币
		int GetRoleMoney(UINT32 roleId, UINT64& gold, UINT32& cash, UINT32& cashcount);
		//获取角色访问信息
		int GetRoleVistInfo(UINT32 roleId, UINT32& vistCount, UINT32& vit, char* lastTime = NULL);
		//获取角色已经学技能
		int GetRoleSkills(UINT32 roleId, std::vector<SSkillMoveInfo>& skillsVec);
		//获取角色昵称
		int GetRoleNick(UINT32 roleId, char* pszNick);
		//获取角色所有信息
		int GetRoleInfo(UINT32 roleId, SRoleInfos* pRoleInfo);
		//获取角色当前体力
		int GetRoleCurrentVit(UINT32 roleId, UINT32& vit);
		//获取角色阵营
		int GetRoleFaction(UINT32 roleId, BYTE& factionId);
		//获取角色背包,穿戴
		int GetRoleWearBackpack(UINT32 roleId, std::vector<SPropertyAttr>& wear, std::vector<SPropertyAttr>& backpack);
		//获取角色穿戴
		int GetRoleWearEquip(UINT32 roleId, std::vector<SPropertyAttr>& wear);
		//获取当前用户进入过游戏角色id
		int GetUserEnteredRoles(std::vector<UINT32>& rolesVec);
		//恢复在线角色体力
		int RecoveryOnlineRoleVit(UINT32 roleId, UINT32& vit, BOOL& needNotify);
		//获取角色职业称号id
		int GetRoleTitleId(UINT32 roleId, UINT16& titleId);
		//获取角色穿戴时装id
		int GetRoleWearDressId(UINT32 roleId, UINT16& dressId, std::vector<UINT16>* pDressVec);
		//计算角色战力
		int CalcRoleCompatPower(UINT32 roleId, UINT32* pNewPower);
		//获取角色战力
		int GetRoleCompatPower(UINT32 roleId, UINT32& cp);
		//获取角色装备碎片
		int GetRoleEquipFragment(UINT32 roleId, std::vector<SEquipFragmentAttr>& fragmentVec);
		//获取角色位置
		int GetRolePosition(UINT32 roleId, SGSPosition* pos);
		//获取角色经验
		int GetRoleExp(UINT32 roleId, SBigNumber& exp);
		//获取角色升级通知信息
		int GetRoleUpgradeNotify(UINT32 roleId, SRoleUpgradeNotify* pUpgradeNotify);
		//获取角色新手引导
		int GetRoleHelps(UINT32 roleId, std::vector<UINT16>& helpsVec);
		//获取角色拥有魔导器
		int GetRoleMagics(UINT32 roleId, std::vector<UINT16>& magicsVec);
		//获取装备位属性
		int GetRoleEquipPosAttr(UINT32 roleId, std::vector<SEquipPosAttr>& equipPosVec);
		//获取角色签到信息
		int GetRoleSignInInfo(UINT32 roleId, std::vector<LTMSEL>& signVec);
		//检查玩家充值订单状态
		int CheckPlayerRechargeOrderStatus(UINT32 userId, UINT32 roleId, BOOL& hasNotReceiveOrder);
		//清理每日数据
		void CleanEverydayData();

		//add by hxw 20151124
		//清理每日在线信息的数据，因为涉及到重新写入，所以单独提出来实现
		void CleanOnlineData();

		//zpy 获取炼魂碎片
		int GetRoleChainSoulFragment(UINT32 roleId, std::vector<SChainSoulFragmentAttr>& fragmentVec);
		//zpy 获取炼魂部件
		int GetRoleChainSoulPos(UINT32 roleId, std::vector<SChainSoulPosAttr>& posAttr);

		//zpy 天梯匹配
		int LadderMatching(UINT32 roleId, UINT32 matching_rank[LADDER_MATCHTING_MAX_NUM], UINT16 *refresh_count, UINT32 *rpcoin);
		//zpy 刷新天梯排行范围
		int RefreshLadderRankScope(UINT32 roleId, UINT32 matching_rank[LADDER_MATCHTING_MAX_NUM]);
		//zpy 执行天梯挑战
		int DoLadderChallenge(UINT32 roleId, UINT16 *ret_challenge_count);
		//zpy 消除冷却时间
		int EliminateLadderCoolingTime(UINT32 roleId, SEliminateLadderCDTimeRes *pEliminate);
		//zpy 完成天梯挑战
		int FinishLadderChallenge(UINT32 roleId, BYTE finishCode/*, SFinishLadderChallengeRes *pFinishLadder*/);
		//zpy 重置被挑战者天梯排行范围
		int ResetChallengerLadderRankScope(UINT32 roleId);

		//zpy 添加机器人
		int AddRobot(UINT32 roleId, UINT32 robotId);
		//zpy 是否存在机器人
		int HasRobot(UINT32 roleId, UINT32 robotId, bool *ret);
		//zpy 清理机器人
		int ClearRobot(UINT32 roleId);

		//add by hxw 20151015 获取通用排行奖励接口	

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
		//根据账号id查询所有角色信息
		int OnQueryRolesInfoByUserId(UINT32 userId);

	private:
		BYTE m_linkerId;
		RolesInfoMan* m_parent;
		CXTLocker m_rolesMapLocker;
		std::map<UINT32/*角色id*/, SRoleInfos*> m_rolesMap;	//角色表
	};

public:
	RolesInfoMan();
	~RolesInfoMan();

	//获取用户角色信息
	int QueryUserRoles(UINT32 userId, SQueryRolesRes* pRoles);
	//查询拥有的角色数量 add by hxw 20151210
	int QueryUserRolesNum(UINT32 userId);

	//进入角色
	int UserEnterRole(UINT32 userId, UINT32 roleId, BYTE linkerId, SEnterRoleRes* pEnterRes);
	//退出角色
	int UserLeaveRole(UINT32 userId, UINT32 roleId);
	//移除账号角色
	void RemoveUserRoles(UINT32 userId);

	//创建角色
	int CreateRole(UINT32 userId, BYTE occuId, BYTE sex, const char* roleNick, SCreateRoleRes* pCreateRes);
	//删除角色
	int DelRole(UINT32 userId, UINT32 roleId, SDelRoleRes *pDelRes);
	//查询角色外形信息
	int QueryRoleAppearance(UINT32 userId, UINT32 roleId, SQueryRoleAppearanceRes* pRoleRes);
	//查询角色详细信息
	int QueryRoleExplicit(UINT32 userId, UINT32 roleId, SQueryRoleExplicitRes* pRoleRes);
	//改变阵营
	int ChangeFaction(UINT32 userId, UINT32 roleId, BYTE factionId, BOOL& needSyncDB);
	//zpy 查询所有角色的ID
	int QueryAllRoles(UINT32 userId, SQueryAllRolesRes* pAllRoles);

	//查询背包信息
	int QueryBackpack(UINT32 userId, UINT32 roleId, BYTE from, BYTE nums, SQueryBackpackRes* pBackpackRes);
	//背包整理
	int CleanupBackpack(UINT32 userId, UINT32 roleId, BOOL& isCleanup);
	//丢弃背包道具
	int DropBackpackProperty(UINT32 userId, UINT32 roleId, UINT32 id, BYTE num);
	//使用道具
	int UseProperty(UINT32 userId, UINT32 roleId, UINT32 id, UINT16& propId, SUsePropertyRes *pUseProperty);
	//兑换道具
	int CashProperty(UINT32 userId, UINT32 roleId, UINT16 cashId, SCashPropertyRes* pCashRes);
	//购买背包或仓库存储空间
	int BuyStorageSpace(UINT32 userId, UINT32 roleId, BYTE buyType, BYTE num, UINT32& spend, BYTE& newNum);
	//出售物品
	int SellProperty(UINT32 userId, UINT32 roleId, UINT32 id, BYTE num, SSellPropertyRes* pSellRes, SPropertyAttr& propAttr);
	//物品回购
	int BuyBackProperty(UINT32 userId, UINT32 roleId, SPropertyAttr& buyProp, BYTE num, SPropBuyBackRes* pBuyBackRes);

	//装备装备
	int LoadEquipment(UINT32 userId, UINT32 roleId, UINT32 id, BYTE pos);
	//卸载装备
	int UnLoadEquipment(UINT32 userId, UINT32 roleId, UINT32 id);
	//装备合成
	int EquipmentCompose(UINT32 userId, UINT32 roleId, UINT16 composeId, SEquipComposeRes* pComposeRes);

	//进入副本
	int EnterInst(UINT32 userId, UINT32 roleId, UINT16 instId, SEnterInstRes* pEnterInstRes, int& dataLen, LTMSEL enterTTS, PlayerActionMan::SEnterInstItem& enterInstItem);
	//获取副本区域怪掉落
	int FetchInstAreaMonsterDrops(UINT32 userId, UINT32 roleId, UINT16 instId, BYTE areaId, SFetchInstAreaDropsRes* pAreaDropsRes);
	//拾取副本怪掉落
	int PickupInstMonsterDrop(UINT32 userId, UINT32 roleId, UINT16 instId, BYTE areaId, BYTE dropId, SPickupInstDropNotify* pNotify);
	//拾取副本破碎物掉落
	int PickupInstBrokenDrop(UINT32 userId, UINT32 roleId, UINT16 instId, BYTE dropId, SPickupInstDropNotify* pNotify);
	//拾取副本宝箱
	int PickupInstBoxDrop(UINT32 userId, UINT32 roleId, UINT16 instId, BYTE boxId, BYTE id, SPickupInstDropNotify* pNotify);
	//杀怪得经验
	int KillMonsterExp(UINT32 userId, UINT32 roleId, UINT16 instId, BYTE areaId, UINT16 monsterId, BYTE monsterLv, BOOL& isUpgrade);
	//完成副本
	int FinishInst(UINT32 userId, UINT32 roleId, UINT16 instId, UINT32 score, BYTE star, LTMSEL& enterTTS, const std::vector<SPropertyItem> &rewardVec, SFinishInstRes* pFinishInstRes, SReceiveRewardRefresh *pRefresh);

	/* zpy 成就系统新增 */
	int GetMaxGemLevel(UINT32 userId, UINT32 roleId, BYTE &max_level);
	int GetInlaidGemLevelSum(UINT32 userId, UINT32 roleId, UINT32 &sum);
	int GetEquipStarLevelSum(UINT32 userId, UINT32 roleId, UINT32 &sum);
	int GetSkillLevelSum(UINT32 userId, UINT32 roleId, UINT32 &sum);

	//查询角色技能
	int QueryRoleSkills(UINT32 userId, UINT32 roleId, SQuerySkillsRes* pSkills);
	//技能升级
	int SkillUpgrade(UINT32 userId, UINT32 roleId, UINT16 skillId, BYTE step, SSkillUpgradeRes* pUpgradeRes);
	//装载技能
	int LoadSkill(UINT32 userId, UINT32 roleId, UINT16 skillId, BYTE key, BOOL& needSyncDB);
	//取消技能
	int CancelSkill(UINT32 userId, UINT32 roleId, UINT16 skillId, BOOL& needSyncDB);

	//接受任务
	int AcceptTask(UINT32 userId, UINT32 roleId, UINT16 taskId);
	//查询已接受任务
	int QueryAcceptedTasks(UINT32 userId, UINT32 roleId, SQueryAcceptTasksRes* pTaskRes);
	//放弃任务
	int DropTask(UINT32 userId, UINT32 roleId, UINT16 taskId);
	//提交任务完成
	int CommitTaskFinish(UINT32 userId, UINT32 roleId, UINT16 taskId, SFinishTaskFailNotify* pFinTaskRes);
	//获取日常任务
	int GetDailyTasks(UINT32 userId, UINT32 roleId, SGetDailyTaskRes* pDailyTasks);
	//更新任务目标
	int UpdateTaskTarget(UINT32 userId, UINT32 roleId, UINT16 taskId, const UINT32* pTarget, int n = MAX_TASK_TARGET_NUM);
	//领取任务奖励
	int ReceiveTaskReward(UINT32 userId, UINT32 roleId, UINT16 taskId, SReceiveTaskRewardRes* pTaskRewardRes);

	//装备位镶嵌宝石
	int EquipPosInlaidGem(UINT32 userId, UINT32 roleId, BYTE pos, UINT32 gemId, BYTE holeSeq);
	//装备位取出宝石
	int EquipPosRemoveGem(UINT32 userId, UINT32 roleId, BYTE pos, BYTE holeSeq, SEquipPosRemoveGemRes* pRemoveRes);
	//宝石合成
	int	GemCompose(UINT32 userId, UINT32 roleId, BYTE num, const SConsumeProperty* pMaterial, SGemComposeRes* pComposeRes);

	//收取邮件附件
	int ReceiveEmailAttachments(UINT32 userId, UINT32 roleId, BYTE emailType, const SPropertyAttr* pAttachmentsProp, BYTE attachmentsNum, SReceiveEmailAttachmentsRes* pEmailAttachmentsRes, BOOL& isUpgrade);
	//充值
	int RechargeToken(UINT32 userId, UINT32 roleId, UINT16 cashId, SRechargeRes* pRechargeRes, char* pszRoleNick);
	//设置角色称号id
	int SetRoleTitleId(UINT32 userId, UINT32 roleId, UINT16 titleId);
	//商城购买物品
	int MallBuyGoods(UINT32 userId, UINT32 roleId, UINT32 buyId, UINT16 num, SMallBuyGoodsRes* pBuyGoodsRes, char* pszRoleNick, UINT32& spendToken);

	//查询角色时装
	int QueryRoleDress(UINT32 userId, UINT32 roleId, SQueryRoleDressRes* pDressRes);
	//角色时装穿戴
	int WearDress(UINT32 userId, UINT32 roleId, UINT16 dressId);
	//查询角色装备碎片
	int QueryEquipFragment(UINT32 userId, UINT32 roleId, SQueryEquipFragmentRes* pFragmentRes);

	//复活
	int RoleRevive(UINT32 userId, UINT32 roleId, UINT16 reviveId, SReviveRoleRes* pReviveRes);
	//查询金币兑换
	int QueryGoldExchange(UINT32 userId, UINT32 roleId, SQueryGoldExchangeRes* pExchangeRes);
	//金币兑换
	int GoldExchange(UINT32 userId, UINT32 roleId, SGoldExchangeRes* pExchangeRes);
	//推荐好友
	int GetRecommendFriends(UINT32 userId, UINT32 roleId, SGetRecommendFriendsRes* pFriendsRes);

	//领取VIP奖励
	int ReceiveVIPReward(UINT32 userId, UINT32 roleId, BYTE vipLv, SReceiveVIPRewardRes* pVIPRewardRes, SReceiveRewardRefresh* pRefresh);

	/* zpy 成就系统新增 */
	int ReceiveAchievementReward(UINT32 userId, UINT32 roleId, UINT32 achievement_id, SReceiveRewardRes *pAchievementReward, SReceiveRewardRefresh *pRefresh);

	//查询新手引导
	int QueryRoleHelps(UINT32 userId, UINT32 roleId, SQueryFinishedHelpRes* pHelpRes);
	//完成新手引导
	int FinishRoleHelp(UINT32 userId, UINT32 roleId, UINT16 helpId);
	//查询角色拥有魔导器
	int QueryRoleMagics(UINT32 userId, UINT32 roleId, SQueryRoleMagicsRes* pMagicsRes);
	//获得魔导器
	int GainMagic(UINT32 userId, UINT32 roleId, UINT16 instId, UINT16 magicId);

	//装备位强化
	int EquipPosStrengthen(UINT32 userId, UINT32 roleId, BYTE pos, SEquipPosStrengthenRes* pStrengthenRes);
	//装备位升星
	int EquipPosRiseStar(UINT32 userId, UINT32 roleId, BYTE pos, const SRiseStarAttachParam& param, SEquipPosRiseStarRes* pRiseStarRes);

	//查询签到
	int QuerySignIn(UINT32 userId, UINT32 roleId, SQueryOnlineSignInRes* pSignInRes);
	//签到
	int SignIn(UINT32 userId, UINT32 roleId);
	//副本扫荡
	int InstSweep(UINT32 userId, UINT32 roleId, UINT16 instId, SInstSweepRes* pSweepRes, BOOL& isUpgrade);
	//解锁内容
	int UnlockContent(UINT32 userId, UINT32 roleId, BYTE type, UINT16 unlockId);

	//抽取轮盘奖品
	int ExtractRouletteReward(UINT32 userId, UINT32 roleId, const SRoulettePropItem& extractReward, SExtractRouletteRewardRes* pRewardRes, BOOL& isUpgrade);
	//获取轮盘大奖
	int FetchRouletteBigReward(UINT32 userId, UINT32 roleId, const SRoulettePropItem& bigReward, SFetchRouletteBigRewardRes* pRewardRes, BOOL& isUpgrade);
	//刷新轮盘大奖
	int UpdateRouletteBigReward(UINT32 userId, UINT32 roleId, UINT32 spendToken, UINT32& curToken);

	/* zpy 体力兑换 */
	//查询体力兑换
	int QueryVitExchange(UINT32 userId, UINT32 roleId, SQueryVitExchangeRes* pExchangeRes);
	//体力兑换
	int VitExchange(UINT32 userId, UINT32 roleId, SVitExchangeRes* pExchangeRes);

	/* zpy 领取每日签到奖励 */
	int ReceiveDailySignReward(UINT32 userId, UINT32 roleId, BYTE days, bool continuous, SRoleDailySignRes *pRoleSign, SReceiveRewardRefresh *pRefresh);

	/* zpy 查询炼魂碎片 */
	int QueryChainSoulFragment(UINT32 userId, UINT32 roleId, SQueryChainSoulFragmentRes* pFragmentRes);
	/* zpy 查询炼魂部件 */
	int QueryChainSoulPos(UINT32 userId, UINT32 roleId, SQueryChainSoulPosRes* pChainSoulPos);
	/* zpy 升级炼魂部件 */
	int ChainSoulPosUpgrade(UINT32 userId, UINT32 roleId, BYTE pos, SChainSoulPosUpgradeRes* pChainSoulPosUpgrade);

	/* zpy 复活角色 */
	int ResurrectionInWorldBoss(UINT32 userId, UINT32 roleId, UINT16 reviveId, UINT32 consume_count, UINT32 &out_rpcoin);

public:
	//用户角色是否在线
	BOOL IsUserRoleOnline(UINT32 userId, UINT32 roleId);
	//用户是否在线
	BOOL IsUserOnline(UINT32 userId);
	//获取用户所在linker
	int GetUserLinkerId(UINT32 userId, BYTE& linkerId);
	//设置用户所在linker
	int SetUserLinkerId(UINT32 userId, BYTE linkerId);
	//获取角色背包数据
	int GetRoleBackpack(UINT32 userId, UINT32 roleId, std::vector<SPropertyAttr>& vec);
	//获取角色已接收任务
	int GetRoleAcceptTask(UINT32 userId, UINT32 roleId, std::vector<SAcceptTaskInfo>& vec);
	//获取角色背包/仓库大小
	int GetRoleStorageSpace(UINT32 userId, UINT32 roleId, BYTE& packSize, BYTE& wareSize);
	//获取角色money
	int GetRoleMoney(UINT32 userId, UINT32 roleId, UINT64& gold, UINT32& cash, UINT32& cashcount);
	//获取角色访问信息
	int GetRoleVistInfo(UINT32 userId, UINT32 roleId, UINT32& vistCount, UINT32& vit, char* lastTime);
	//获取角色技能
	int GetRoleSkills(UINT32 userId, UINT32 roleId, std::vector<SSkillMoveInfo>& skillsVec);
	//获取角色昵称
	int GetRoleNick(UINT32 userId, UINT32 roleId, char* pszNick);
	//获取角色所有信息
	int GetRoleInfo(UINT32 userId, UINT32 roleId, SRoleInfos* pRoleInfo);
	//获取角色当前体力
	int GetRoleCurrentVit(UINT32 userId, UINT32 roleId, UINT32& vit);
	//获取角色阵营
	int GetRoleFaction(UINT32 userId, UINT32 roleId, BYTE& factionId);
	//获取角色背包,穿戴
	int GetRoleWearBackpack(UINT32 userId, UINT32 roleId, std::vector<SPropertyAttr>& wear, std::vector<SPropertyAttr>& backpack);
	//获取角色已经穿戴装备
	int GetRoleWearEquip(UINT32 userId, UINT32 roleId, std::vector<SPropertyAttr>& wear);
	//获取当前用户进入过游戏角色id
	int GetUserEnteredRoles(UINT32 userId, std::vector<UINT32>& rolesVec);
	//恢复在线角色体力
	int RecoveryOnlineRoleVit(UINT32 userId, UINT32 roleId, UINT32& vit, BOOL& needNotify);
	//获取角色职业称号id
	int GetRoleTitleId(UINT32 userId, UINT32 roleId, UINT16& titleId);
	//获取角色穿戴时装id
	int GetRoleWearDressId(UINT32 userId, UINT32 roleId, UINT16& dressId, std::vector<UINT16>* pDressVec);
	//计算角色战力
	int CalcRoleCompatPower(UINT32 userId, UINT32 roleId, UINT32* pNewPower = NULL);
	//获取角色战力
	int GetRoleCompatPower(UINT32 userId, UINT32 roleId, UINT32& cp);
	//获取角色装备碎片
	int GetRoleEquipFragment(UINT32 userId, UINT32 roleId, std::vector<SEquipFragmentAttr>& fragmentVec);
	//获取角色位置
	int GetRolePosition(UINT32 userId, UINT32 roleId, SGSPosition* pos);
	//获取角色经验
	int GetRoleExp(UINT32 userId, UINT32 roleId, SBigNumber& exp);
	//获取角色升级推送信息
	int GetRoleUpgradeNotify(UINT32 userId, UINT32 roleId, SRoleUpgradeNotify* pUpgradeNotify);
	//获取角色新手引导
	int GetRoleHelps(UINT32 userId, UINT32 roleId, std::vector<UINT16>& helpsVec);
	//获取角色拥有魔导器
	int GetRoleMagics(UINT32 userId, UINT32 roleId, std::vector<UINT16>& magicsVec);
	//获取装备位属性
	int GetRoleEquipPosAttr(UINT32 userId, UINT32 roleId, std::vector<SEquipPosAttr>& equipPosVec);
	//获取角色签到信息
	int GetRoleSignInInfo(UINT32 userId, UINT32 roleId, std::vector<LTMSEL>& signVec);
	//检查玩家充值订单状态
	int CheckPlayerRechargeOrderStatus(UINT32 userId, UINT32 roleId, BOOL& hasNotReceiveOrder);
	//清理每日数据
	void CleanEverydayData();

	//add by hxw 20151124
	//清理每天online reward 记录
	void CleanOnlineData();

	//zpy 获取炼魂碎片
	int GetRoleChainSoulFragment(UINT32 userId, UINT32 roleId, std::vector<SChainSoulFragmentAttr>& fragmentVec);
	//zpy 获取炼魂部件
	int GetRoleChainSoulPos(UINT32 userId, UINT32 roleId, std::vector<SChainSoulPosAttr>& posAttr);

	//zpy 天梯匹配
	int LadderMatching(UINT32 userId, UINT32 roleId, UINT32 matching_rank[LADDER_MATCHTING_MAX_NUM], UINT16 *refresh_count, UINT32 *rpcoin);
	//zpy 刷新天梯排行范围
	int RefreshLadderRankScope(UINT32 userId, UINT32 roleId, UINT32 matching_rank[LADDER_MATCHTING_MAX_NUM]);
	//zpy 执行天梯挑战
	int DoLadderChallenge(UINT32 userId, UINT32 roleId, UINT16 *ret_challenge_count);
	//zpy 消除冷却时间
	int EliminateLadderCoolingTime(UINT32 userId, UINT32 roleId, SEliminateLadderCDTimeRes *pEliminate);
	//zpy 完成天梯挑战
	int FinishLadderChallenge(UINT32 userId, UINT32 roleId, BYTE finishCode/*, SFinishLadderChallengeRes *pFinishLadder*/);
	//zpy 重置被挑战者天梯排行范围
	int ResetChallengerLadderRankScope(UINT32 userId, UINT32 roleId);

	//zpy 添加机器人
	int AddRobot(UINT32 userId, UINT32 roleId, UINT32 robotId);
	//zpy 是否存在机器人
	int HasRobot(UINT32 userId, UINT32 roleId, UINT32 robotId, bool *ret);
	//zpy 清理机器人
	int ClearRobot(UINT32 userId, UINT32 roleId);

	/* zpy 购买竞技模式进入次数 */
	int BuyCompModeEnterNum(UINT32 userId, UINT32 roleId, int purchased, SBuyCompEnterNumRes *pEnterNum);
	int BuyAllCompModeEnterNum(UINT32 userId, UINT32 roleId, int purchased, int upper_num, SBuyCompEnterNumRes *pEnterNum);

	/* zpy 购买1vs1模式进入次数 */
	int BuyoneVsOneEnterNum(UINT32 userId, UINT32 roleId, int purchased, int num, SBuy1V1EnterNumRes *pBuyEnter);

	//BY add hxw 20151016
	//获取排行通用奖励
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
	UINT32 m_totalRoles;	//所有角色数量
	CXTLocker m_userRolesMapLocker;
	std::map<UINT32/*账号id*/, RoleSite*> m_userRolesMap;	//用户角色表
};

void InitRolesInfoMan();
RolesInfoMan* GetRolesInfoMan();
void DestroyRolesInfoMan();

#endif