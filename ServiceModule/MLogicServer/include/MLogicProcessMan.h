#ifndef __MLOGIC_PROCESS_MAN_H__
#define __MLOGIC_PROCESS_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
逻辑处理类
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

private:	//用户请求处理
	//查询角色信息
	int OnQueryRoles(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//创建角色
	int OnCreateRole(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//删除角色
	int OnDelRole(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//进入角色
	int OnEnterRoles(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//退出角色
	int OnLeaveRoles(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//账号退出
	int OnUserExit(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//查询背包
	int OnQueryBackpack(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//背包整理
	int OnBackpackCleanup(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//查询角色外形信息
	int OnQueryRoleApperance(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//查询角色详细信息
	int OnQueryRoleExplicit(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//购买背包或仓库存储空间
	int OnBuyStorageSpace(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//查询公会名称
	int OnQueryGuildName(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//查询邮件内容
	int OnQueryEmailContent(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//收取邮件附件
	int OnReceiveEmailAttachments(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//查询邮件列表
	int OnQueryEmailList(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//删除邮件
	int OnDelEmails(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//查询好友
	int OnQueryFriends(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//查询好友申请列表
	int OnQueryFriendsApply(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//添加好友
	int OnAddFriend(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//好友申请确认
	int OnFriendApplyConfirm(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//删除好友
	int OnDelFriend(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//获取推荐好友列表
	int OnGetRecommendFriends(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//搜索玩家
	int OnSearchPlayer(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//查询已完成新手引导信息
	int OnQueryFinishedHelp(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//完成某新手引导信息
	int OnFinishNewPlayerHelp(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//查询角色魔导器
	int OnQueryMagics(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//获得魔导器
	int OnGainMagic(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//出售物品
	int OnSellProperty(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//丢弃物品
	int OnDropProperty(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//使用道具
	int OnUseProperty(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//兑换道具
	int OnCashProperty(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//物品回购
	int OnBuyBackProperty(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//查询物品出售记录
	int OnQueryPropSellRcd(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//进入副本
	int OnEnterInst(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//获取副本区域怪掉落
	int OnFetchInstAreaMonsterDrops(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//查询已完成副本信息
	int OnQueryFinishedInst(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//拾取副本怪掉落物品
	int OnPickupInstMonsterDrop(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//拾取副本宝箱
	int OnPickupInstBox(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//拾取破碎物掉落
	int OnPickupBrokenDrop(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//杀怪得经验
	int OnKillMonsterExp(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//完成副本
	int OnFinishInst(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//宝石合成
	int OnGemCompose(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//载入装备
	int OnLoadEquipment(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//卸载装备
	int OnUnLoadEquipment(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//装备合成
	int OnEquipmentCompose(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//装备位强化
	int OnEquipPosStrengthen(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//装备位升星
	int OnEquipPosRiseStar(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//装备位宝石镶嵌
	int OnEquipPosInlaidGem(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//装备位宝石取出
	int OnEquipPosRemoveGem(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//获取装备位属性
	int OnGetEquipPosAttr(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//领取成就奖励
	int OnReceiveAchievementReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//查询成就
	int OnQueryAchievement(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//查询角色技能
	int OnQueryRoleSkills(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//技能升级
	int OnSkillUpgrade(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//改变阵营
	int OnChangeFaction(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//装载技能
	int OnLoadSkill(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//取消技能
	int OnCancelSkill(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//接受任务
	int OnAcceptTask(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//定时任务请求
	int OnReqTimerTask(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//查询已接受任务
	int OnQueryAcceptedTasks(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//放弃任务
	int OnDropTask(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//提交任务完成
	int OnCommitTaskFinish(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//查询已完成任务历史记录
	int OnQueryFinTaskHistory(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//获取日常任务
	int OnGetDailyTask(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//更新已接任务目标
	int OnUpdateTaskTarget(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//领取任务奖励
	int OnReceiveTaskReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//充值
	int OnRechargeToken(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//获取角色占领副本
	int OnGetRoleOccupyInst(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//设置角色职业称号
	int OnSetRoleTitleId(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//商城购买物品
	int OnMallBuyGoods(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//查询角色时装
	int OnQueryRoleDress(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//角色时装穿戴
	int OnWearDress(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//获取机器人数据
	int OnFetchRobot(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//获取单人随机副本传送阵进入人数
	int OnGetPersonalTransferEnter(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//获取已打开单人随机副本
	int OnGetOpenPersonalRandInst(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//查询装备碎片
	int OnQueryEquipFragment(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//复活
	int OnRoleRevive(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//查询金币兑换
	int OnQueryGoldExchange(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//金币兑换
	int OnGoldExchange(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//查询未领取奖励的VIP等级
	int OnQueryNotReceiveVIPReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//领取VIP奖励
	int OnReceiveVIPReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//查询已充值且有首充赠送的充值id
	int OnQueryRechargeFirstGive(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//查询签到
	int OnQuerySignIn(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//上线签到
	int OnlineSignIn(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//查询角色已领取活动
	int OnQueryReceivedActivity(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//领取活动
	int OnReceivedActivity(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//查询当前开放章节
	int OnQueryCurOpenChapter(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//副本扫荡
	int OnInstSweep(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//领取兑换码奖励
	int OnReceiveCashCodeReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//查询账号解锁内容
	int OnQueryUserUnlockContent(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//解锁内容
	int OnUnlockContent(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//验证服务器查询账号解锁内容
	int OnLcsQueryUserUnlockContent(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//获取轮盘剩余奖品
	int OnGetRouletteRemainReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//抽取轮盘奖品
	int OnExtractRouletteReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//刷新轮盘大奖
	int OnUpdateRouletteBigReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//获取轮盘大奖
	int OnFetchRouletteBigReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//获取充值订单
	int OnFetchRechargeOrder(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//获取充值代币
	int OnGetRechargeToken(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//获取活动副本挑战次数
	int OnGetActiveInstChallengeTimes(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	/* zpy 体力兑换 */
	//查询体力兑换
	int OnQueryVitExchange(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//体力兑换
	int OnVitExchange(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	/* zpy 查询每日签到 */
	int OnQueryRoleDailySign(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	/* zpy 每日签到 */
	int OnRoleDailySign(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	//add by hxw 20151028
	//查询等级排行的排行榜信息
	int QueryLvRankInfo(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//end
	//add by hxw 20151006
	//查询等级排行奖励信息
	int QueryLvRank(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	// 查询是否领取副本推图排名奖励
	int QueryInsnumRank(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//查询副本推图排行的排行榜
	int QueryInsnumRankInfo(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//获取等级排行奖励
	int GetLvRankReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//获取副本挑战次数排行奖励
	int GetInsnumRankReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//查询强者奖励（战力)领取情况
	int QueryStrongerInfo(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//领取强者奖励
	int GetStrongerReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//查询在线奖励
	int QueryOnlineInfo(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	int GetOnlineReward(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//end

	/* zpy 查询炼魂碎片 */
	int OnQueryChainSoulFragment(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	/* zpy 查询炼魂部件 */
	int OnQueryChainSoulPos(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	/* zpy 升级炼魂部件 */
	int OnChainSoulPosUpgrade(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	/* zpy 攻击世界Boos */
	int OnAttackWorldBoss(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	/* zpy 查询世界Boos排行榜 */
	int OnQueryWorldBossHurtRank(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	/* zpy 被世界Boss杀死 */
	int OnWasKilledByWorldBoss(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	/* zpy 在世界Boss副本中复活 */
	int OnResurrectionInWorldBoss(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	/* zpy 查询世界Boss信息 */
	int OnQueryWorldBossInfo(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	/* zpy 获取天梯信息 */
	int OnGetLadderInfo(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	/* zpy 天梯匹配玩家 */
	int OnLadderMatchingPlayer(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	/* zpy 刷新天梯排行信息 */
	int OnRefreshLadderRankScope(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	/* zpy 天梯挑战玩家 */
	int OnLadderChallengePlayer(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	/* zpy 消除天梯CD时间 */
	int OnEliminateLadderCoolingTime(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	/* zpy 完成天梯挑战 */
	//int OnFinishLadderChallenge(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	/* zpy 查询所有角色 */
	int OnQueryAllRoles(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	/* zpy 查询拥有的活动副本类型 */
	int OnQueryHasAcivityInstType(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	/* zpy 查询周免角色类型 */
	int OnQueryWeekFreeActorType(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	/* zpy 查询竞技模式进入次数 */
	int OnQueryEnterKOFNum(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	/* zpy 购买竞技模式进入次数 */
	int OnBuyEnterKOFNum(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

	/* zpy 查询1v1副本进入次数 */
	int OnQueryEnterOneVsOneNum(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	/* zpy 购买1v1副本进入次数 */
	int OnBuyEnterOneVsOneNum(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

private:
	typedef int (MLogicProcessMan::*fnMsgProcessHandler)(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	std::map<UINT16, fnMsgProcessHandler> m_msgProcHandler;
};

void InitMLogicProcessMan();
MLogicProcessMan* GetMLogicProcessMan();
void DestroyMLogicProcessMan();

#endif