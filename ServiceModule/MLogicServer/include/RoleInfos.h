#ifndef __ROLE_INFOS_H__
#define __ROLE_INFOS_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
缓存角色信息数据结构
*/
#include <map>
#include <vector>
#include "ServiceMLogic.h"
#include "EquipCfg.h"
#include "InstCfg.h"
#include "StrengthenCfg.h"

#define USE_MEM_POOL 1

#if USE_MEM_POOL
	#include "ssmempool.h"
#endif

#define GENERIC_SQL_BUF_LEN		1024
#define MAX_SQL_BUF_LEN			64*1024
#define MAX_PUT_ITEM_RET_NUM	255	//0xFF

#pragma pack(push, 1)

//时拾取副本掉落通知
struct SPickupInstDropNotify
{
	SGoldUpdateNotify* pGold;
	STokenUpdateNotify* pToken;
	SVitUpdateNotify* pVit;
	SBackpackUpdateNotify* pBackpack;
	SFragmentUpdateNotify* pFragment;
	SChainSoulFragmentNotify* pChainSoulFragment;
};

//领取奖励需更新数据
struct SReceiveRewardRefresh
{
	BOOL gold;
	BOOL rpcoin;
	BOOL fragment;
	BOOL prop;
	BOOL chainSoulFragment;
	BOOL exp;
	BOOL vit;
	BOOL dress;
	BOOL magics;
	BOOL upgrade;
};


struct SOnlineInfo
{
	UINT32 id;	 //数据ID	
	UINT32 itmes;//已在线时间
	LTMSEL lasttime;//最后一次进入游戏时间

	SOnlineInfo()
	{
		id = 0;
		itmes = 0;
		lasttime = 0;
	}
	void OnClear()
	{
		id = 0;
		itmes = 0;
		lasttime = 0;

	}
};



//角色属性数据
struct SRoleInfos
{
	UINT32 roleId;			//角色id
	char nick[33];			//昵称[33];
	UINT16 titleId;			//称号id;
	BYTE level;				//角色等级
	SBigNumber gold;		//游戏币
	SBigNumber exp;			//经验
	UINT32	cash;			//现金
	BYTE	occuId;			//职业id
	BYTE	sex;			//性别
	UINT32	cashcount;		//充值总额
	UINT32	hp;				//生命
	UINT32	mp;				//魔力
	UINT32	attack;			//攻击
	UINT32	def;			//防御
	UINT32	vit;			//体力
	UINT16	ap;				//剩余属性点数
	BYTE    packSize;		//背包大小
	BYTE    wareSize;		//仓库大小
	UINT32  guildId;		//所属公会id
	UINT32	cp;				//战力;
	SGSPosition	pos;		//所在位置, 暂时忽略
	BYTE   newPlayerState;	//新手引导状态 未完成 已完成
	UINT32 vist;			//访问次数
	BYTE   factionId;		//阵营id
	UINT16 dressId;			//时装id
	char   lastEnterTime[SGS_DATE_TIME_LEN];	//最后一次进入游戏时间
	LTMSEL enterInstTTS;	//进入副本时间

	BYTE   isOnline;	//是否在线
	BYTE   isEnterGame;	//是否进入过游戏
	UINT32 uid;			//装备唯一id
	UINT32 onlineRecoveryVit;	//在线恢复体力点数
	BYTE   reviveTimes;			//复活次数
	UINT16 exchangedTimes;		//当前金币已兑换次数
	UINT32 recommendFriendTimes;	//推荐好友次数
	UINT32 instDropGold;		//副本掉落金币
	UINT32 instDropToken;		//副本掉落代币

	UINT16 vitExchangedTimes;		//当前体力兑换次数

	UINT16 ladder_win_in_row;			//天梯连胜次数
	UINT16 ladder_refresh_count;		//天梯刷新次数
	UINT16 ladder_challenge_count;		//天梯挑战次数
	UINT16 ladder_eliminate_count;		//天梯消除CD次数
	LTMSEL ladder_last_finish_time;		//上次完成天梯时间
	UINT32 ladder_matching_rank[LADDER_MATCHTING_MAX_NUM];	//天梯匹配排名

	std::vector<SPropertyAttr>	packs;			//背包数据
	std::vector<SPropertyAttr>  wearEquipment;	//已穿戴装备
	std::vector<SSkillMoveInfo> studySkills;	//已经学技能数据
	std::vector<SAcceptTaskInfo> acceptTasks;	//已接受任务
	std::vector<SBufferInfo> buffers;			//Buff数据
	std::vector<UINT16> dress;					//拥有时装
	std::vector<SEquipFragmentAttr> fragments;	//装备碎片
	std::vector<UINT16> helps;					//新手引导
	std::vector<UINT16> magics;					//魔导器
	std::vector<SEquipPosAttr> equipPos;		//装备位属性
	std::vector<LTMSEL> signInVec;				//签到时间
	std::vector<SChainSoulFragmentAttr> chainSoulFragments;	//炼魂碎片
	std::vector<SChainSoulPosAttr> chainSoulPos;//炼魂部件属性
	std::set<UINT32> robotSet;					//机器人集合					

	//add by hxw 20151020 	
	SOnlineInfo onlineVec;						//在线奖励领取数组
	std::vector<UINT16> onlines;				//在线奖励领取数组
	std::vector<UINT16> strongerVec;			//强者奖励领取状况
	//end

	InstCfg::SInstDropBox dropBox;		//副本掉落宝箱
	InstCfg::SInstDropProp brokenDrops;		//副本破碎物掉落
	InstCfg::SInstDropProp dropProps;	//副本区域怪掉落物品
	InstCfg::SInstBrushMonster brushMonster;	//副本区域刷怪表

	SRoleInfos()
	{
		roleId = 0;			 //角色id
		titleId = 0;		 //职业称号等级;
		level = 0;			 //角色等级
		gold.hi = 0;
		gold.lo = 0;
		exp.hi = 0;
		exp.lo = 0;
		cash = 0;			//现金
		occuId = 0;			//角色类型
		sex = 0;
		cashcount = 0;		//充值总额
		hp = 0;				//生命
		mp = 0;				//魔力
		attack = 0;			//攻击
		def = 0;			//防御
		vit = 0;			//体力
		ap = 0;				//剩余属性点数
		packSize = 0;		//背包大小
		wareSize = 0;		//仓库大小
		guildId = 0;		//所属公会id
		cp = 0;				//战斗力;
		newPlayerState = 0;	//新手引导状态 未完成 已完成
		vist = 0;			//访问次数
		isOnline = 0;		//是否在线
		factionId = 0;		//阵营id
		dressId = 0;		//时装id
		enterInstTTS = 0;	//进入副本时间

		isEnterGame = 0;	//是否进入过游戏
		uid = 1;			//装备唯一id
		onlineRecoveryVit = 0;
		reviveTimes = 0;	//复活次数
		exchangedTimes = 0;	//金币兑换次数
		recommendFriendTimes = 0;	//推荐好友次数
		instDropGold = 0;		//副本掉落金币
		instDropToken = 0;		//副本掉落代币

		memset(nick, 0, sizeof(nick));
		memset(lastEnterTime, 0, sizeof(lastEnterTime));
		memset(&pos, 0, sizeof(SGSPosition));

		vitExchangedTimes = 0;		//体力兑换次数

		ladder_win_in_row = 0;			//天梯连胜次数
		ladder_refresh_count = 0;		//当天刷新天梯次数	
		ladder_challenge_count = 0;		//天梯挑战次数
		ladder_eliminate_count = 0;		//天梯消除CD次数
		ladder_last_finish_time = 0;	//上次完成天梯时间
		memset(ladder_matching_rank, 0, sizeof(ladder_matching_rank));

		for (BYTE pos = ESGS_EQUIP_WEAPON; pos <= ESGS_EQUIP_NECKLACE; ++pos)
		{
			SEquipPosAttr item;
			memset(&item, 0, sizeof(SEquipPosAttr));
			item.pos = pos;
			equipPos.push_back(item);
		}

		for (BYTE idx = 0; idx < MAX_CHAIN_SOUL_HOLE_NUM; ++idx)
		{
			SChainSoulPosAttr attr;
			attr.pos = idx + 1;
			attr.level = 0;
			chainSoulPos.push_back(attr);
		}

		packs.clear();
		wearEquipment.clear();
		studySkills.clear();
		acceptTasks.clear();
		buffers.clear();
		dress.clear();
		fragments.clear();
		helps.clear();
		chainSoulFragments.clear();

		dropBox.boxMap.clear();
		dropProps.props.clear();
		brushMonster.monsterMap.clear();
		brokenDrops.props.clear();

		memset(&onlineVec, 0, sizeof(SOnlineInfo));
		onlines.clear();
		strongerVec.clear();
	}

	// zpy 2015.9.29注释 拷贝构造函数和赋值运算符重载函数实现里面并未实现完全拷贝，不知何意
	//SRoleInfos(const SRoleInfos& role)
	//{
	//	roleId = role.roleId;			//角色id
	//	titleId = role.titleId;			//职业称号等级;
	//	level = role.level;				//角色等级
	//	gold.hi = role.gold.hi;
	//	gold.lo = role.gold.lo;
	//	exp.hi = role.exp.hi;
	//	exp.lo = role.exp.lo;
	//	cash = role.cash;			//现金
	//	occuId = role.occuId;			//角色类型
	//	sex = role.sex;
	//	cashcount = role.cashcount;		//充值总额
	//	hp = role.hp;				//生命
	//	mp = role.mp;				//魔力
	//	attack = role.attack;			//攻击
	//	def = role.def;			//防御
	//	vit = role.vit;			//体力
	//	ap = role.ap;				//剩余属性点数
	//	packSize = role.packSize;		//背包大小
	//	wareSize = role.wareSize;		//仓库大小
	//	guildId = role.guildId;		//所属公会id
	//	cp = role.cp;				//战斗力;
	//	newPlayerState = role.newPlayerState;	//新手引导状态 未完成 已完成
	//	vist = role.vist;			//访问次数
	//	isOnline = role.isOnline;			//是否在线
	//	factionId = role.factionId;	//阵营id
	//	dressId = role.dressId;		//时装id
	//	reviveTimes = role.reviveTimes;			//复活次数
	//	exchangedTimes = role.exchangedTimes;
	//	recommendFriendTimes = role.recommendFriendTimes;	//推荐好友次数
	//	instDropGold = role.instDropGold;		//副本掉落金币
	//	instDropToken = role.instDropToken;		//副本掉落代币
	//	enterInstTTS = role.enterInstTTS;	//进入副本时间
	//	strcpy(nick, role.nick);
	//	strcpy(lastEnterTime, role.lastEnterTime);
	//	memcpy(&pos, &role.pos, sizeof(SGSPosition));

	//	vitExchangedTimes = role.vitExchangedTimes;			//体力兑换次数

	//	packs.assign(role.packs.begin(), role.packs.end());
	//	wearEquipment.assign(role.wearEquipment.begin(), role.wearEquipment.end());
	//	studySkills.assign(role.studySkills.begin(), role.studySkills.end());
	//	acceptTasks.assign(role.acceptTasks.begin(), role.acceptTasks.end());
	//	buffers.assign(role.buffers.begin(), role.buffers.end());
	//	dress.assign(role.dress.begin(), role.dress.end());
	//	fragments.assign(role.fragments.begin(), role.fragments.end());
	//	chainSoulFragments.assign(role.chainSoulFragments.begin(), role.chainSoulFragments.end());
	//}
	//SRoleInfos& operator = (const SRoleInfos& role)
	//{
	//	roleId = role.roleId;			//角色id
	//	titleId = role.titleId;			//称号id;
	//	level = role.level;				//角色等级
	//	gold.hi = role.gold.hi;
	//	gold.lo = role.gold.lo;
	//	exp.hi = role.exp.hi;
	//	exp.lo = role.exp.lo;
	//	cash = role.cash;			//现金
	//	occuId = role.occuId;			//角色类型
	//	sex = role.sex;
	//	cashcount = role.cashcount;		//充值总额
	//	hp = role.hp;				//生命
	//	mp = role.mp;				//魔力
	//	attack = role.attack;	//攻击
	//	def = role.def;			//防御
	//	vit = role.vit;			//体力
	//	ap = role.ap;				//剩余属性点数
	//	packSize = role.packSize;		//背包大小
	//	wareSize = role.wareSize;		//仓库大小
	//	guildId = role.guildId;		//所属公会id
	//	cp = role.cp;				//战斗力;
	//	newPlayerState = role.newPlayerState;	//新手引导状态 未完成 已完成
	//	vist = role.vist;			//访问次数
	//	isOnline = role.isOnline;			//是否在线
	//	factionId = role.factionId;	//阵营id
	//	dressId = role.dressId;		//时装id
	//	reviveTimes = role.reviveTimes;			//复活次数
	//	exchangedTimes = role.exchangedTimes;
	//	recommendFriendTimes = role.recommendFriendTimes;	//推荐好友次数
	//	enterInstTTS = role.enterInstTTS;	//进入副本时间
	//	instDropGold = role.instDropGold;		//副本掉落金币
	//	instDropToken = role.instDropToken;		//副本掉落代币
	//	strcpy(nick, role.nick);
	//	strcpy(lastEnterTime, role.lastEnterTime);
	//	memcpy(&pos, &role.pos, sizeof(SGSPosition));

	//	vitExchangedTimes = role.vitExchangedTimes;			//体力兑换次数

	//	packs.assign(role.packs.begin(), role.packs.end());
	//	wearEquipment.assign(role.wearEquipment.begin(), role.wearEquipment.end());
	//	studySkills.assign(role.studySkills.begin(), role.studySkills.end());
	//	acceptTasks.assign(role.acceptTasks.begin(), role.acceptTasks.end());
	//	buffers.assign(role.buffers.begin(), role.buffers.end());
	//	dress.assign(role.dress.begin(), role.dress.end());
	//	fragments.assign(role.fragments.begin(), role.fragments.end());
	//	chainSoulFragments.assign(role.chainSoulFragments.begin(), role.chainSoulFragments.end());
	//	return *this;
	//}

	//自动恢复体力
	void AutoRecoveryVit(UINT32 maxAutoVit, LTMSEL nowMsel);
	//初始化背包
	void InitBackpack(const BYTE* pData, int nDataLen);
	//初始化buffer
	void InitBuffer(const BYTE* pData, int nDataLen);
	//初始化已接受任务
	void InitAcceptedTasks(const BYTE* pData, int nDataLen, LTMSEL nowMsel);
	//初始化技能
	void InitSkills(const BYTE* pData, int nDataLen);
	//初始化已穿戴装备
	void InitWearEquipment(const BYTE* pData, int nDataLen);
	//初始化已拥有时装
	void InitHasDress(const BYTE* pData, int nDataLen);
	//初始化已拥有装备碎片
	void InitHasEquipFragment(const BYTE* pData, int nDataLen);
	//初始化装备位
	void InitEquipPos(const BYTE* pData, int nDataLen);

	//技能升级
	int SkillUpgrade(UINT16 skillId, SSkillUpgradeRes* pUpgradeRes);
	//获取技能
	int GetStudySkills(SSkillMoveInfo* pSkills, BYTE maxNum);
	//计算角色战力
	UINT32 CalcRoleCombatPower();
	//获取角色基础属性点
	void GetRoleBasicAttrPoint(SRoleAttrPoint& attrPoint);
	//获取角色已穿戴装备属性点
	void GetRoleWearEquipAttrPoint(SRoleAttrPoint& attrPoint);
	//获取角色已穿戴称号属性点
	void GetRoleWearTitleAttrPoint(SRoleAttrPoint& attrPoint);
	//获取角色穿戴时装属性点
	void GetRoleWearFashionAttrPoint(SRoleAttrPoint& attrPoint);
	/* zpy 获取炼魂属性点 */
	void GetRoleChainSoulAttrPoint(SRoleAttrPoint& attrPoint);
	//计算属性战力
	UINT32 ConvertAttrToPower(const SRoleAttrPoint& attrPoint);

	//获取背包物品所在下标
	int GetBackpackPropIdx(UINT32 id);
	//获取已穿戴装备所在下标
	int GetWearEquipIdx(UINT32 id);

	//背包是否存在某物品
	BOOL IsExistProperty(UINT16 propId, UINT16 leastNum);
	//背包中拥有物品数量(>=leastNum返回leastNum否则返回实际数量)
	UINT16 NumberOfProperty(UINT16 propId);
	UINT16 NumberOfProperty(UINT16 propId, UINT16 leastNum);
	//根据唯一id获取背包物品信息
	int GetBackpackPropAttrById(UINT32 id, SPropertyAttr& propAttr);
	//放入单个物品放入背包(prop.num=1, maxStack-被放入物品最大堆叠数)
	int PutBackpackSingleNumProperty(SPropertyAttr& prop, BYTE maxStack);
	//获取背包中所有物品信息
	int GetBackpackProps(SPropertyAttr* pProps, BYTE maxNum, BYTE from = 0, BYTE* totals = NULL);
	//丢弃背包物品(<=num则全部丢弃)
	int DropBackpackProp(UINT32 id, BYTE num);
	//丢弃背包道具(<=num则全部丢弃)
	int DropBackpackPropEx(UINT16 propId, UINT16 num);
	//丢弃背包道具(<=num则全部丢弃)
	int DropBackpackPropEx(UINT16 propId, UINT16 num, SConsumeProperty* pConsumeItem, BYTE maxItemNum, BYTE& retNum);
	int DropBackpackPropEx(UINT16 propId, UINT16 num, SPropertyAttr* pBackpackMod, BYTE maxModNum, BYTE& retNum);
	//能否放入物品
	BOOL CanPutIntoBackpack(UINT16 propId, UINT16 num, BYTE maxStack);
	//放入物品(能放下)
	int PutBackpackProperty(BYTE propType, UINT16 propId, UINT16 num, BYTE maxStack, SConsumeProperty* pPutItem, BYTE maxItemNum, BYTE& retNum);
	//放入物品(能放下)
	int PutBackpackProperty(BYTE propType, UINT16 propId, UINT16 num, BYTE maxStack, SPropertyAttr* pBackpackMod, BYTE maxModNum, BYTE& retNum);
	//是否拥有魔导器
	BOOL IsHasMagic(UINT16 magicId);
	//获取碎片数量
	UINT16 GetFragmentNum(UINT16 fragId);
	//是否拥有时装
	BOOL IsHasDress(UINT16 dressId);
	//扣除碎片
	void DropFragment(UINT16 fragId, UINT16 num);
	//扣除魔导器
	void DropMagic(UINT16 magicId);
	//扣除时装
	void DropDress(UINT16 dressId);

	//出售物品
	int SellProperty(UINT32 id, BYTE num, SPropertyAttr& propAttr, SSellPropertyRes* pSellRes);
	//回购物品
	int BuyBackProperty(SPropertyAttr& buyProp, BYTE num, SPropBuyBackRes* pBuyBackRes);

	//初始化新角色赠送
	void InitNewRoleGiveItems(UINT32 userid = 0);
	//初始新角色邮件
	void InitNewRoleEmail(UINT32 userId, UINT32 roleId);

	//获取所有已穿戴装备信息
	int GetWearEquipmentProps(SPropertyAttr* pProps, BYTE maxNum, BYTE from = 0, BYTE* totals = NULL);
	//获取已穿戴装备
	int GetWearEquipmentProp(UINT32 id, SPropertyAttr& propAttr);

	//装备合成
	int EquipmentCompose(UINT16 composeId, SEquipComposeRes* pComposeRes);
	//穿戴装备
	int LoadEquipment(UINT32 id, BYTE pos);
	//卸载装备
	int UnLoadEquipment(UINT32 id);
	//zpy 替换装备
	int ReplaceEquipment(UINT32 id, const SPropertyAttr& propAttr);

	//装备位镶嵌宝石
	int EquipPosInlaidGem(BYTE pos, UINT32 gemId, BYTE holeSeq);
	//装备位移除宝石
	int EquipPosRemoveGem(BYTE pos, BYTE holeSeq, SEquipPosRemoveGemRes* pRemoveRes);
	//宝石合成
	int GemCompose(BYTE num, const SConsumeProperty* pMaterial, SGemComposeRes* pComposeRes);

	//接收任务
	int AcceptTask(UINT16 taskId);
	//放弃任务
	int DropTask(UINT16 taskId);
	//判断任务完成
	int JudgeTaskFinish(UINT16 taskId, SFinishTaskFailNotify* pFinTaskNotify);
	//获取日常任务
	int GetDailyTasks(UINT32 roleId, SGetDailyTaskRes* pDailyTasks, BYTE maxNum);
	//已接受任务中是否已存在
	BOOL IsExistAcceptedTasks(UINT16 taskId);
	//领取任务奖励
	int ReceiveTaskReward(UINT32 userId, UINT32 roleId, UINT16 taskId, SReceiveTaskRewardRes* pTaskRewardRes);
	//更新任务目标
	int UpdateTaskTarget(UINT16 taskId, const UINT32* pTarget, int n = MAX_TASK_TARGET_NUM);

	//收取邮件附件
	int ReceiveEmailAttachments(BYTE emailType, const SPropertyAttr* pAttachmentsProp, BYTE attachmentsNum, SReceiveEmailAttachmentsRes* pEmailAttachmentsRes, BOOL& isUpgrade);
	//商城购买物品
	int MallBuyGoods(UINT32 buyId, UINT16 num, SMallBuyGoodsRes* pBuyGoodsRes, UINT32& spendToken);
	//增加装备碎片
	UINT16 AddEquipFragment(UINT16 fragmentId, UINT16 num);

	//复活
	int RoleRevive(UINT16 reviveId, SReviveRoleRes* pReviveRes);
	//金币兑换
	int GoldExchange(SGoldExchangeRes* pExchangeRes);

	//领取VIP奖励
	int ReceiveVIPReward(BYTE vipLv, SReceiveVIPRewardRes* pVIPRewardRes, std::vector<SPropertyAttr>& emailAttachmentsVec, SReceiveRewardRefresh* pRefresh);

	/* zpy 成就系统新增 */
	int ReceiveAchievementReward(UINT32 achievement_id, SReceiveRewardRes *pAchievementReward, std::vector<SPropertyAttr>& emailAttachmentsVec, SReceiveRewardRefresh *pRefresh);

	/* zpy 限时活动奖励 */
	int ReceiveLimitedTimeActivityReward(const std::vector<SPropertyItem> &rewardVec, SFinishInstRes* pFinishInstRes, std::vector<SPropertyAttr> emailAttachmentsVec, SReceiveRewardRefresh *pRefresh);

	//装备位强化
	int EquipPosStrengthen(BYTE pos, SEquipPosStrengthenRes* pStrengthenRes);
	//装备位升星
	int EquipPosRiseStar(BYTE pos, const SRiseStarAttachParam& param, SEquipPosRiseStarRes* pRiseStarRes);

	//产出金币
	void ProduceGold(UINT64 goldNum, const char* pszAction, BOOL recordLog = true);
	//消耗金币
	void ConsumeGold(UINT64 goldNum, const char* pszAction, BOOL recordLog = true);

	//产出代币
	void ProduceToken(UINT32 tokenNum, const char* pszAction, BOOL recordLog = true);
	//消耗代币
	void ConsumeToken(UINT32 tokenNum, const char* pszAction, BOOL recordLog = true);

	/* zpy 体力兑换 */
	int VitExchange(SVitExchangeRes* pExchangeRes);

	/* zpy 领取每日签到奖励 */
	int ReceiveDailySignReward(BYTE days, bool continuous, SRoleDailySignRes *pRoleSign, std::vector<SPropertyAttr>& emailAttachmentsVec, SReceiveRewardRefresh *pRefresh);

	// zpy 获取炼魂碎片数量
	UINT16 GetChainSoulFragmentNum(UINT16 fragId);
	// zpy 扣除炼魂碎片
	void DropChainSoulFragment(UINT16 fragId, UINT16 num);
	// zpy 增加炼魂碎片
	UINT16 AddChainSoulFragment(UINT16 fragmentId, UINT16 num);
	// zpy 初始化已拥有炼魂碎片
	void InitHasChainSoulFragment(const BYTE* pData, int nDataLen);
	// zpy 初始化炼魂部件
	void InitChainSoulPos(const BYTE* pData, int nDataLen);

	// zpy 背包和已穿戴装备装备中是否存在某物品
	BOOL IsExistInbackpackAdnWearEquipment(UINT16 propId, UINT16 leastNum);

	// zpy 初始化天梯信息
	void InitLadderInfo(UINT16 win_in_row, UINT16 refresh_count, UINT16 eliminate_count, UINT16 challenge_count, LTMSEL last_finish_time, UINT32 matching_rank[LADDER_MATCHTING_MAX_NUM]);

	// zpy 增加角色经验
	SBigNumber AddRoleExp(UINT64 add_exp);
	SBigNumber AddRoleExp(SBigNumber add_exp);

	//add by hxw 20151015 Rank奖励通用领取
	int ReceiveRankReward(BYTE type, UINT16 rewardid, SGetRankRewardRes *pRoleSign, std::vector<SPropertyAttr>& emailAttachmentsVec, SReceiveRewardRefresh *pRefresh);
	//add 20151022
	BOOL IsGetStrongerReward(UINT16 rewardid);
	BOOL IsGetOnlineReward(UINT16 rewardid);
	int GetOnlineInfo(SOnlineInfo* info, std::vector<UINT16>& vonline);
	int GetStrongerInfo(std::vector<UINT16>& infov);
	int InputStrongerReward(UINT16 rewardid);
	int InputOnlineReward(UINT16 rewardid);
	int SetOnlineInfo(UINT32 id, UINT32 time, LTMSEL lasttime = 0);
	//end 20151015


};

inline void CalcEquipAttachAttrPoint(BYTE type, UINT32 val, SRoleAttrPoint& equipAttr)
{
	switch (type)
	{
	case ESGS_EQUIP_ATTACH_HP:			//hp
		equipAttr.u32HP += val;
		break;
	case ESGS_EQUIP_ATTACH_MP:			//mp
		equipAttr.u32MP += val;
		break;
	case ESGS_EQUIP_ATTACH_ATTACK:		//攻击
		equipAttr.attack += val;
		break;
	case ESGS_EQUIP_ATTACH_DEF:			//防御
		equipAttr.def += val;
		break;
	case ESGS_EQUIP_ATTACH_CRIT:		//暴击
		equipAttr.crit += val;
		break;
	case ESGS_EQUIP_ATTACH_TENACITY:	//韧性
		equipAttr.tenacity += val;
		break;
	default:
		break;
	}
}

inline void CalcEquipGemAttrPoint(BYTE type, UINT32 val, SRoleAttrPoint& equipAttr)
{
	switch (type)
	{
	case ESGS_GEM_HP:			//hp
		equipAttr.u32HP += val;
		break;
	case ESGS_GEM_MP:			//mp
		equipAttr.u32MP += val;
		break;
	case ESGS_GEM_ATTACK:		//攻击
		equipAttr.attack += val;
		break;
	case ESGS_GEM_DEF:			//防御
		equipAttr.def += val;
		break;
	case ESGS_GEM_CRIT:			//暴击
		equipAttr.crit += val;
		break;
	case ESGS_GEM_TENACITY:		//韧性
		equipAttr.tenacity += val;
		break;
	default:
		break;
	}
}

//副本占领类型
enum EInstOccupyType
{
	EINST_OCCUPY_BY_NEW_ROLE = 1,		//被新的玩家占领
	EINST_OCCUPY_REFRESH_BY_SELF = 2,	//被自己刷新
	EINST_OUT_RANK = 3,					//被挤出排行
	EINST_FIRST_OCCUPY = 4				//首次占领
};

//副本占领发送邮件
struct SInstOccupyEmail
{
	EInstOccupyType type;
	UINT16 instId;
	UINT32 occupierRoleId;		//占领者角色id
	UINT32 occupierUserId;		//占领者账号id
	std::string occupierNick;	//占领者角色昵称
	UINT32 occupiedRoleId;		//被占领者角色id
	UINT32 occupiedUserId;		//被占领者账号id
	std::string occupiedNick;	//被占领者角色昵称
};

#pragma pack(pop)

class RoleInfosObjAllocator
{
	RoleInfosObjAllocator(const RoleInfosObjAllocator&);
	RoleInfosObjAllocator& operator = (const RoleInfosObjAllocator&);

public:
	RoleInfosObjAllocator();
	~RoleInfosObjAllocator();

	SRoleInfos* NewRoleInfosObj();
	void DeleteRoleInfosObj(SRoleInfos* pInfo);

private:
#if USE_MEM_POOL
	CMemPoolObj<SRoleInfos> m_roleAttrPool;
#endif
};

void InitRoleInfosObjAllocator();
RoleInfosObjAllocator* GetRoleInfosObjAllocator();
void DestroyRoleInfosObjAllocator();

class AutoRoleInfosObj
{
public:
	AutoRoleInfosObj(SRoleInfos* pObj)
		: m_roleObj(pObj)
	{

	}

	AutoRoleInfosObj()
		: m_roleObj(NULL)
	{
		m_roleObj = GetRoleInfosObjAllocator()->NewRoleInfosObj();
	}

	~AutoRoleInfosObj()
	{
		if (m_roleObj != NULL)
		{
			GetRoleInfosObjAllocator()->DeleteRoleInfosObj(m_roleObj);
			m_roleObj = NULL;
		}
	}

	BOOL IsValid()
	{
		return m_roleObj != NULL ? true : false;
	}

	operator SRoleInfos*()
	{
		return m_roleObj;
	}

	SRoleInfos* operator -> ()
	{
		return m_roleObj;
	}

	SRoleInfos* Get()
	{
		return m_roleObj;
	}

	SRoleInfos* Drop()
	{
		SRoleInfos* pObj = m_roleObj;
		m_roleObj = NULL;
		return pObj;
	}

private:
	SRoleInfos* m_roleObj;
};

#endif