#ifndef __GAME_CFG_FILE_MAN_H__
#define __GAME_CFG_FILE_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
游戏配置文件管理
*/

#include "RoleBasicCfg.h"
#include "SkillCfg.h"
#include "TaskCfg.h"
#include "MaterialCfg.h"
#include "InstCfg.h"
#include "EquipCfg.h"
#include "RiseStarCfg.h"
#include "QualityCfg.h"
#include "GemCfg.h"
#include "FragmentCfg.h"
#include "MallCfg.h"
#include "RechargeCfg.h"
#include "TitleCfg.h"
#include "EmailCfg.h"
#include "RandInstSysCfg.h"
#include "FashionCfg.h"
#include "SuitCfg.h"
#include "ReviveCfg.h"
#include "GoldExchangeCfg.h"
#include "VIPCfg.h"
#include "MonsterCfg.h"
#include "StrengthenCfg.h"
#include "GameDropCfg.h"
#include "EquipComposeCfg.h"
#include "ChapterCfg.h"
#include "SignInCfg.h"
#include "ActivityCfg.h"
#include "NoticeCfg.h"
#include "UnlockCfg.h"
#include "GiftBagCfg.h"
#include "InitRoleGiveCfg.h"
#include "RouletteCfg.h"
#include "AchievementCfg.h"
#include "VitExchangeCfg.h"
#include "DailySignCfg.h"
#include "ChainSoulFragmentCfg.h"
#include "GreedTheLiveCfg.h"
#include "MonsterAttributeCfg.h"
#include "MonsterTypeConfig.h"
#include "StrengthenAttriCfg.h"
#include "PlayerCfg.h"
#include "ActivityStageCfg.h"

//add by hxw 20151013 增加排行奖励配置
#include "RankRewardCfg.h"
//end

#include "LimitedTimeActivityCfg.h"
#include "GameTypeCfg.h"

class GameCfgFileMan
{
	GameCfgFileMan(const GameCfgFileMan&);
	GameCfgFileMan& operator = (const GameCfgFileMan&);

public:
	GameCfgFileMan();
	~GameCfgFileMan();

#pragma pack(push, 1)
	//战力平衡值
	struct SCPBalanceAttr
	{
		float HPBalance;
		float MPBalance;
		float ATKBalance;
		float DEFBalance;
		float CTBalance;
		float SGBalance;
		float SKBalance;
		float CritHurt;
		float CriticalRating;
	};
	//强化消耗
	struct SEquipStrengthenSpend
	{
		UINT64 gold;		//金币
		UINT16 stones;		//强化石材料数量
		SEquipStrengthenSpend()
		{
			gold = 0;
			stones = 0;
		}
	};
#pragma pack(pop)

	//载入游戏配置文件
	BOOL LoadGameCfgFile();

	//获取最大创建角色数
	BYTE GetMaxCreateRoles() const
	{
		return m_maxCreateRoles;
	}

	//获取完成单个副本最大排行数
	BYTE GetMaxInstRankNum() const
	{
		return m_maxInstRankNum;
	}

	//该道具最多可镶嵌宝石数量
	BYTE GetMaxInlaidStones(UINT16 /*propId*/, BYTE /*propGrade*/)
	{
		return 4;
	}

	//获取体力恢复时间间隔
	UINT16 GetVitRecoveryInterval() const
	{
		return m_vitRecoveryInterval;
	}

	//获取单位时间内恢复体力值
	UINT16 GetVitRecoveryVal() const
	{
		return m_vitRecoveryVal;
	}

	//自动恢复体力最大值
	UINT32 GetMaxAutoRecoveryVit() const
	{
		return m_maxAutoRecoveryVit;
	}

	//获取最大接收任务数量
	BYTE GetMaxAcceptTasks() const
	{
		return m_maxAcceptTasks;
	}

	//获取装备最大星级
	BYTE GetEquipMaxStarLevel() const
	{
		return m_equipMaxStarLv;
	}

	//获取装备最大强化等级
	BYTE GetEquipMaxStrengthenLevel() const
	{
		return m_equipMaxStrengthenLv;
	}

	//获取最大保存物品出售记录
	BYTE GetMaxSaveSellProps() const
	{
		return m_maxSaveSellProps;
	}

	//获取邮件最大附件数量
	BYTE GetMaxEmailAttachments() const
	{
		return m_maxEmailAttachments;
	}

	//获取最大好友数量
	BYTE GetMaxFriendNum() const
	{
		return m_maxFriendNum;
	}

	//获取角色最大等级
	BYTE GetMaxRoleLevel() const
	{
		return m_maxRoleLevel;
	}

	//获取推荐好友数量
	BYTE GetRecommendFriendNum() const
	{
		return m_recommendFriendNum;
	}

	//获取推荐好友等级
	BYTE GetRecommendFriendLv() const
	{
		return m_recommendFriendLv;
	}

	//获取初始背包大小
	BYTE GetInitBackpackSize() const
	{
		return m_initBackpackSize;
	}

	//获取初始仓库大小
	BYTE GetInitWarehouseSize() const
	{
		return m_initWarehouseSize;
	}

	//获取最大背包大小
	BYTE GetMaxBackpackSize() const
	{
		return m_maxBackpackSize;
	}

	//获取背包系数
	BYTE GetPackParam() const
	{
		return m_packParam;
	}

	//获取最大仓库大小
	BYTE GetMaxWarehouseSize() const
	{
		return m_maxWarehouseSize;
	}

	//获取战力平衡值
	void GetCPBalanceAttr(SCPBalanceAttr& cpBalanceAttr)
	{
		cpBalanceAttr = m_CPBanlance;
	}

	//获取回购价格倍数
	float GetBuybackPriceTimes() const
	{
		return m_buybackPriceTimes;
	}

	//获取金币掉落基值
	UINT32 GetDropGoldBasicVal() const
	{
		return m_dropGoldBasicVal;
	}

	//获取金币掉落基值浮动百分比
	BYTE GetDropGoldFloat() const
	{
		return m_dropGoldFloat;
	}

	//升星最大成功率
	BYTE GetRiseStarMaxSuccessChance() const
	{
		return m_riseStarMaxSuccessChance;
	}

	//升星最小破损率
	BYTE GetRiseStarMinDamageChance() const
	{
		return m_riseStarMinDamageChance;
	}

	//获取经验修正值
	int GetExpCorrectVal() const
	{
		return m_expCorrectVal;
	}

	//获取战力修正范围值
	UINT32 GetPowerCorrectVal() const
	{
		return m_powerCorrectVal;
	}

	//获取删除角色时间限制
	UINT32 GetDelRoleTimeLimit() const
	{
		return m_delRoleTimeLimit;
	}

	//获取活动副本上限
	UINT32 GetActivityInstUpperLimit() const
	{
		return m_activityInstUpperLimit;
	}

	//获取强化花费
	int GetEquipStrengthenSpend(BYTE pos, BYTE lv, SEquipStrengthenSpend& spend);

	////测试任务链 add by hxw 20151212
	////只对测试用，服务器不调用
	//int TestTask()
	//{
	//	return m_taskCfg.TestTask();
	//}
	////end

	private:
		BYTE m_maxCreateRoles;	//最多创建的角色数量
		BYTE m_maxInstRankNum;	//副本排行数(前几名)

		UINT16 m_vitRecoveryInterval;	//体力恢复时间间隔(秒)
		UINT16 m_vitRecoveryVal;		//体力恢复时间间隔内恢复值
		UINT32 m_maxAutoRecoveryVit;	//自动恢复体力最大值(超过该值则不处理,若通过购买或其他方式则做相应累加,不受该值的影响)

		BYTE m_initBackpackSize;	//初始背包大小
		BYTE m_initWarehouseSize;	//初始仓库大小
		BYTE m_maxBackpackSize;		//最大背包大小
		BYTE m_maxWarehouseSize;	//最大仓库大小
		BYTE m_packParam;			//背包系数

		BYTE  m_maxAcceptTasks;			//最多接受任务数量
		BYTE  m_equipMaxStarLv;			//装备最大星级
		BYTE  m_equipMaxStrengthenLv;	//装备最大强化等级
		BYTE  m_maxSaveSellProps;		//最大保存物品出售记录
		BYTE  m_maxEmailAttachments;	//邮件最大附件数量
		BYTE  m_maxRoleLevel;			//角色最大等级

		BYTE  m_maxFriendNum;			//最大好友数量
		BYTE  m_recommendFriendNum;		//推荐好友数量
		BYTE  m_recommendFriendLv;		//推荐好友等级

		SCPBalanceAttr m_CPBanlance;	//战力平衡

		float m_strengthenStoneModulus;	//强化石系数
		float m_strengthenGoldModulus;	//强化金币系数
		int m_stoneConsumeCorrectVal;	//石头消耗修正值
		int m_goldConsumeCorrectVal;	//金币消耗修正值
		std::vector<UINT32> m_strengthenBasicConsumeStoneVec;	//强化基础消耗石头
		std::vector<UINT32> m_strengthenBasicConsumeGoldVec;	//强化基础消耗金币

		float m_buybackPriceTimes;		//回购价格倍数
		UINT32 m_dropGoldBasicVal;		//金币掉落基值
		BYTE   m_dropGoldFloat;			//金币掉落基值浮动百分比

		UINT32 m_goldBranchThrehold;	//金币分堆阀值
		BYTE   m_goldBranchMaxPercent;	//金币分堆最大百分比
		BYTE   m_dropLv1ToBossPercent;	//普通物品分到boss百分比
		BYTE   m_dropLv1ToCreamPercent;	//普通物品分到精英百分比
		int    m_expCorrectVal;			//经验修正值

		BYTE m_riseStarMaxSuccessChance;	//升星最大成功率
		BYTE m_riseStarMinDamageChance;		//升星最小破损率
		UINT32 m_powerCorrectVal;			//战力修正范围值
		UINT32 m_activityInstUpperLimit;	//活动副本上限
		UINT32 m_delRoleTimeLimit;			//删除角色时间限制

		std::map<UINT32, UINT32> m_ladderMatching;	//天梯匹配配置

		RoleBasicCfg m_roleCfg;		//角色配置
		InstCfg m_instCfg;			//副本配置
		SkillCfg m_skillCfg;		//技能配置
		MaterialCfg m_materialCfg;	//材料配置
		TaskCfg m_taskCfg;			//任务配置
		EquipCfg m_equipCfg;		//装备配置
		RiseStarCfg m_riseStarCfg;	//装备升星配置
		QualityCfg m_qualityCfg;	//装备品质配置
		GemCfg m_gemCfg;			//宝石配置
		FragmentCfg m_fragmentCfg;	//碎片配置
		MallCfg m_mallCfg;			//商城配置
		FashionCfg m_fashionCfg;	//时装配置
		RechargeCfg m_rechargeCfg;	//充值配置
		TitleCfg m_titleCfg;		//称号配置
		EmailCfg m_emailCfg;		//邮件赠送配置
		RandInstSysCfg m_randInstSysCfg;	//单人随机副本配置
		SuitCfg m_suitCfg;			//套装配置
		ReviveCfg m_reviveCfg;		//复活配置
		GoldExchangeCfg m_goldExchangeCfg;	//金币兑换配置
		VIPCfg m_vipCfg;			//VIP配置
		MonsterCfg m_monsterCfg;	//怪配置
		StrengthenCfg m_strengthenCfg;	//强化配置
		GameDropCfg m_gameDropCfg;	//游戏掉落配置
		EquipComposeCfg m_equipComposeCfg;	//碎片合成配置
		ChapterCfg m_chapterCfg;	//章节配置
		SignInCfg m_signInCfg;		//签到配置
		ActivityCfg m_activityCfg;	//活动配置
		NoticeCfg m_noticeCfg;		//公告配置
		UnlockCfg m_unlockCfg;		//解锁配置
		GiftBagCfg m_giftBagCfg;	//礼包配置
		InitRoleGiveCfg m_newRoleGiveCfg;	//新角色赠送
		RouletteCfg m_rouletteCfg;	//轮盘配置
		LimitedTimeActivityCfg m_limitedTimeActivity;	//限时活动配置
		GameTypeCfg m_gameTypeCfg;						//关卡玩法配置
		MonsterAttributeCfg m_monsterAttributeCfg;		//怪物属性配置
		MonsterTypeConfig m_monsterTypeCfg;				//怪物类型配置
		StrengthenAttriCfg m_strengthenAttriCfg;		//装备位属性配置
		PlayerCfg m_playerCfg;							//角色配置
		ActivityStageCfg m_activityStageCfg;			//活动副本配置

		/* zpy 成就系统新增 */
		AchievementCfg m_achievementCfg;

		/* zpy 体力兑换 */
		VitExchangeCfg m_vitExchangeCfg;

		/* zpy 每日签到 */
		DailySignCfg m_dailySignCfg;

		/* zpy 炼魂碎片 */
		ChainSoulFragmentCfg m_chainSoulFragmentCfg;
		/* zpy 炼魂系统配置 */
		GreedTheLiveCfg m_greedTheLiveCfg;

		//add by hxw 20151013 排行奖励配置
		RankRewardCfg m_rankRewardCfg;
		//end

		private:
			BOOL LoadGameGlobalCfg(const char* filename);
			BOOL OpenCfgFile(const char* filename, Json::Value& rootValue);

			public:
				//获取角色基本配置
				int GetRoleCfgBasicAttr(BYTE jobId, BYTE roleLevel, SRoleAttrPoint& attr);
				//判断角色升级
				int JudgeRoleUpgrade(UINT16 jobId, BYTE curLevel, UINT64 curExp, BYTE& newLevel);
				//根据经验获得人物等级
				BYTE GetRoleLevelByExp(BYTE jobId, UINT64 exp);

				public:
					//获取某角色从from级到to级拥有所有技能
					int GetRoleAllInitSkills(BYTE jobId, BYTE from, BYTE to, std::vector<SSkillMoveInfo>& allSkills);
					//技能升级(skillId->当前要升级的技能id)
					int GetUpgradeAnySkillAttr(BYTE jobId, UINT16 skillId, BYTE newSkillLevel, SkillCfg::SSkillAttrCfg& skillAttr);
					//是否是普通技能
					bool IsSkill(BYTE occuId, UINT16 skillID);
					/* zpy 新增 获取普通技能配置 */
					bool GetSkillAttr(BYTE jobId, UINT16 skillID, SkillCfg::SSkillAttrCfg *&item);

					public:
						//获取副本需要消耗的体力点
						int GetInstSpendVit(UINT16 instId, UINT32& spendVit);
						//获取副本名字
						int GetInstName(UINT16 instId, std::string& instName);
						//获取副本等级限制
						int GetInstLvLimit(UINT16 instId, BYTE& lvLimit);
						//获取副本配置
						int GetInstConfig(UINT16 instId, InstCfg::SInstAttrCfg *&config);
						//获取副本基本信息
						int GetInstBasicInfo(UINT16 instId, BYTE&instType, BYTE& lvLimit, UINT32& spendVit);
						//生成随机副本id
						UINT16 GenerateRandInstId();
						//生成宝箱掉落
						int GenerateDropBox(UINT16 instId, InstCfg::SInstDropBox& dropBox);
						//生成区域怪掉落
						int GenerateAreaMonsterDrops(UINT16 instId, BYTE areaId, InstCfg::SInstDropProp& dropProps, BYTE& monsterDrops, BYTE& creamDrops, BYTE& bossDrops);
						/* zpy 2015.12.22新增 展开掉落组 */
						void SpreadDropGroups(UINT32 dropId, BYTE dropNum, std::vector<SDropPropCfgItem> *ret);
						//获取战斗区域怪
						int GetBattleAreaMonster(UINT16 instId, BYTE areaId, InstCfg::SInstBrushMonster& brushMonster);
						//生成破碎物掉落
						int GenerateBrokenDrops(UINT16 instId, InstCfg::SInstDropProp& brokenDrops);
						//获取副本扫荡经验
						UINT64 GetInstSweepExp(UINT16 instId);
						//生成扫荡掉落
						int GenerateInstSweepDrops(UINT16 instId, InstCfg::SInstDropProp& drops);
						//获取副本所在章节
						int GetInstOwnerChapter(UINT16 instId, UINT16& chapterId);
						//获取副本类型
						BYTE GetInstType(UINT16 instId);
						//获取进入次数
						UINT16 GetInstEnterNum(UINT16 instId);
						//通过类型获取副本id
						bool GetInstByInstType(BYTE type, std::set<UINT16> *&ret);

						public:
							//获取物品属性
							int GetPropertyAttr(BYTE propType, UINT16 propId, SPropertyAttr& propAttr, BYTE& maxStackCount);
							//获取物品最大堆叠数
							BYTE GetPropertyMaxStack(BYTE propType, UINT16 propId);
							//获取物品用户出售价格
							int GetPropertyUserSellPrice(BYTE propType, UINT16 propId, UINT64& price);
							//获取物品用户回购价格
							int GetPropertyUserBuyBackPrice(BYTE propType, UINT16 propId, UINT64& price);
							//获取物品名称
							void GetPropertyName(BYTE propType, UINT16 propId, std::string& name);

							public:
								//获取角色日常任务
								int GetDailyTasks(BYTE roleLv, TaskCfg::SDailyTasks& dailyTasks);
								//获取任务信息
								int GetTaskInfo(UINT16 taskId, TaskCfg::STaskAttrCfg*& pTaskInfo);
								//获取任务奖励
								int GetTaskReard(UINT16 taskId, BYTE roleLv, TaskCfg::STaskReward& taskReward);

								public:
									//获取装备信息
									int GetEquipPropAttr(UINT16 equipId, SPropertyAttr& propAttr, BYTE& maxStack);
									//获取装备属性点
									int GetEquipAttrPoint(UINT16 equipId, std::vector<EquipCfg::SEquipAddPoint>*& addPoint);

									public:
										//获取宝石消耗
										int GetGemSpendByLevel(BYTE gemlv, GemCfg::SGemConsumeCfg& spend);
										//根据宝石id获取宝石消耗
										int GetGemSpendById(UINT16 gemId, GemCfg::SGemConsumeCfg& spend);
										//根据装备类型,宝石孔获取对应宝石类型,装备等级限制
										int GetEquipInlaidGemAttr(BYTE equipType, BYTE holeSeq, GemCfg::SEquipGemAttr& attr);
										//获取宝石配置属性
										int GetGemCfgAttr(UINT16 gemPropId, GemCfg::SGemAttrCfg& gemAttr);
										//根据宝石类型,等级获取宝石id,最大堆叠数量
										int GetGemIdMaxStackByTypeLevel(BYTE gemType, BYTE gemLv, UINT16& gemId, BYTE& maxStack);
										//获取宝石配置属性
										int GetGemPropAttr(UINT16 gemPropId, SPropertyAttr& propAttr, BYTE& maxStack);
										//获取宝石合成消耗宝石数量
										UINT16 GetComposeGemConsumeGems() const;

										public:
											//获取材料功能值
											int GetMaterialFuncValue(UINT16 materiaId, UINT32& funcVal);
											//获取材料功能值
											int GetMaterialFuncValueVec(UINT16 materiaId, std::vector<UINT32>& funcValVec);

											public:
												//获取商城物品属性
												int GetMallBuyGoodsAttr(UINT32 buyId, MallCfg::SMallGoodsAttr*& pGoodsAttr);
												int GetMallBuyGoodsAttrByPropId(UINT16 propId, MallCfg::SMallGoodsAttr *&pGoodsAttr);

public:
	//时装和职业是否匹配
	int IsJobFashionMatch(BYTE jobId, UINT16 fashionId);
	//获取时装属性
	int GetFashionAttr(UINT16 fashionId, std::vector<FashionCfg::SFashionAttr>*& pAttrVec);

	public:
		//获取充值属性
		int GetRechargeAttr(UINT16 cashId, RechargeCfg::SRechargeCfgAttr& cashAttr);

		//获取充值配置
		const std::map<UINT16, RechargeCfg::SRechargeCfgAttr>& GetRechargeConfig() const;

public:
	//获取称号属性
	int GetRoleTitleAttr(BYTE jobId, UINT16 titleId, std::vector<TitleCfg::STitleAttr>& attrVec);

	public:
		//邮件赠送配置
		int GetNewPlayerEmailGiveProps(std::vector<EmailCfg::SNewPlayerEmailItem>& giveVec);

public:
	//单人随机副本
	BOOL GetRandInstSysParam(RandInstSysCfg::SRandInstSysParam& param);

	public:
		//获取碎片属性
		int GetFragmentAttr(UINT16 fragmentId, FragmentCfg::SFragmentCfgAttr& fragmentAttr);
		//获取赠送碎片
		int GetGiveFragment(std::vector<FragmentCfg::SGiveFragmentInfo>& giveVec);

public:
	//装备合成
	int GetEquipComposeItem(UINT16 composeId, EquipComposeCfg::SEquipComposeItem*& pComposeItem);
	//获取升星花费
	int GetRiseStarSpend(BYTE starLv, RiseStarCfg::SRiseStarSpend& spend);
	//获取升星配置
	int GetRiseStarCfg(BYTE starLv, RiseStarCfg::SRiseStarCfg& cfg);
	//获取装备属性增量
	int GetEquipAttrIncreament(BYTE pos, BYTE quality, StrengthenCfg::SEquipAttrInc& attrInc);


	public:
		//获取复活CD
		int GetReviveCDTimes(UINT16 reviveId);

		//获取复活消耗
		int GetReviveConsume(UINT16 reviveId, UINT32 times, UINT16 vipAddTimes, UINT32& reviveCoin);

public:
	//获取金币兑换
	int GetGoldExchange(UINT16 exchangeTimes, UINT16 vipAddTimes, UINT64& getGold, UINT32& spendToken);

	/* zpy 兑换体力 */
	int GetVitExchange(UINT16 exchangeTimes, UINT16 vipAddTimes, UINT64& getVit, UINT32& spendToken);

	public:
		//计算VIP等级
		BYTE CalcVIPLevel(UINT32 cashcount);
		//获取VIP赠送物品
		void GetVIPGiveProps(BYTE vipLv, std::vector<VIPCfg::SVIPGiveProp>*& pGivePropVec);
		//获取VIP最大体力
		void GetVIPMaxVit(BYTE vipLv, UINT32& maxVit);
		//获取VIP购买体力次数
		void GetVIPBuyVitTimes(BYTE vipLv, UINT16& buyVitTimes);
		//获取VIP金币兑换次数
		void GetVIPGoldExchangeTimes(BYTE vipLv, UINT16& exchangeTimes);
		//获取VIP死亡复活次数
		void GetVIPDeadReviveTimes(BYTE vipLv, BYTE& reviveTimes);
		//获取VIP副本扫荡次数
		void GetVIPInstSweepTimes(BYTE vipLv, UINT16& sweepTimes);
		//获取VIP资源副本挑战次数
		void GetVIPResourceInstChalNum(BYTE vipLv, UINT16& chalTimes);
		//获取VIP商城购买道具折扣
		void GetVIPMallBuyDiscount(BYTE vipLv, BYTE& discount);
		//获取VIP签到奖励倍数
		void GetVIPSignReward(BYTE vipLv, BYTE& signReward);
		/* zpy 体力兑换 */
		void GetVIPVitExchangeTimes(BYTE vipLv, UINT16& exchangeTimes);
		//获取VIP购买竞技模式次数
		void GetVIPBuyKOFTimes(BYTE vipLv, UINT16& buyKOFTimes);
		//获取1v1进入上限
		void GetOneVsOneEnterTimes(BYTE vipLv, UINT16& enterTimes);

		public:
			//获取怪经验
			UINT32 GetMonsterExp(UINT16 momsterId);
			/* zpy 成就系统新增 */
			BYTE GetMonsterType(UINT16 momsterId);
			//获取怪难度系数
			UINT32 GetMonsterDifficulty(UINT16 monsterId);

public:
	//获取签到信息
	int GetSignInItem(BYTE nTimes, SignInCfg::SSignInItem*& pItem);

public:
	//活动信息
	int GetActivityItem(UINT32 activityId, ActivityCfg::SActivityItem& activityItem);

public:
	//公告
	int GetNoticeItems(std::vector<NoticeCfg::SNoticeItem>& noticeVec, UINT32& timeInterval);
	//获取时间间隔
	UINT32 GetNoticeInterval();

public:
	//获取解锁参数
	int GetUnlockItem(BYTE type, UINT16 unlockId, UnlockCfg::SUnlockItem*& pUnlockItem);
	//获取未上锁id
	int GetUnlockedItems(BYTE type, std::vector<UINT16>& itemsVec);

public:
	//获取礼包物品
	int GetGiftBagItem(UINT16 id, GiftBagCfg::SGiftBagItem& item);

public:
	//获取章节数据
	int GetChapterItem(UINT16 chapterId, ChapterCfg::SChapterItem*& pChapterItem);

public:
	//获取角色初始赠送
	BOOL GetRoleInitGiveItem(InitRoleGiveCfg::SInitRoleGiveItem& giveItem);

public:
	//生成轮盘奖品
	BOOL GenerateRouletteReward(SGetRoulettePropRes* pRoulettePropRes, std::vector<RouletteCfg::SRoulettePropCfgItem>& saveDBItemVec);
	BOOL GenerateRouletteReward(std::vector<RouletteCfg::SRoulettePropCfgItem>& itemVec);
	//生成轮盘大奖
	BOOL GenerateRouletteBigReward(RouletteCfg::SRoulettePropCfgItem& bigReward, UINT32& spendToken);
	//获取每日赠送抽奖次数
	BYTE GetEverydayGiveTimes() const;

public:
	//获取邮件类型描述
	const char* GetEmailTypeDesc(BYTE type);

public:
	//获取成就目标类型
	bool GetAchievementTargetType(UINT32 id, AchievementCfg::TargetType &type) const;
	//获取成就奖励
	bool GetAchievementReward(UINT32 id, AchievementCfg::SAchievementReward &reward) const;
	//获取成就条件
	bool GetAchievementCondition(UINT32 id, AchievementCfg::SAchievementCondition &condition);
	//根据成就类型获取成就
	bool GetAchievementFromType(AchievementCfg::AchievementType type, const std::vector<UINT32> *&out) const;
	//根据目标类型获取成就
	bool GetAchievementFromTargetType(AchievementCfg::TargetType type, const std::vector<UINT32> *&out) const;

public:
	//是否开启每日签到
	bool IsDailySignValid() const;

	//获取每日签到配置
	bool GetDailySignConfig(BYTE days, const DailySignCfg::SDailySignItem *&config);

public:
	//获取炼魂碎片出售价格(单价)
	int GetFragmentUserSellPrice(UINT32 fragmentId, UINT64& price);
	//获取炼魂碎片属性
	bool GetChainSoulFragmentAttr(UINT32 fragmentId, ChainSoulFragmentCfg::ChainSoulFragmentAttr &out_attr);
	//通过类型获取炼魂碎片ID
	bool GetFragmentIdByType(BYTE type, UINT32 &out_id);

public:
	//是否开启炼魂系统
	bool IsOpenChainSoulSystem() const;
	//获取部件配置
	bool GetChainSoulPosConfig(BYTE type, BYTE level, GreedTheLiveCfg::ChainSoulPosConfig *&out_config);
	//获取圣物核心配置
	bool GetChainSoulCoreConfig(BYTE level, GreedTheLiveCfg::ChainSoulCoreConfig *&out_config);
	//根据条件等级获取核心等级
	BYTE GetCoreLevelByConditionLevel(BYTE condition_level);


	//add by hxw 20151014 排行奖励配置信息
public:
	//获取排行奖励玩家数量
	INT GetRewardPlayNum(const BYTE type) const;
	BOOL GetRankReward(const BYTE type, const UINT16 rankid, CRankReward::SRkRewardRes*& items, const int iValue = 0);
	//是否是活动时间，返回TRUE表示活动继续，FLASE表示无法活动
	BOOL IsValidityTime(const BYTE type);
	//是否开启
	BOOL IsOpen(const BYTE type) const;
	void SetOpen(const BYTE type, const BOOL bOpen);
	BOOL IsCanGet(const BYTE type, const UINT16 id, const UINT32 value) const;
	int GetRankLogs(std::string& str, const BYTE type, const UINT16 rewardid = 0);


	//end


public:
	//获取限时活动配置
	bool GetLimitedTimeActivityCfg(LimitedTimeActivityCfg::ActivityType type, LimitedTimeActivityCfg::SActivityConfig &out);

	//生成活动副本
	bool GenerateActivityInst(int level, std::set<UINT16> passInsts, SActivityInstItem &item);

public:
	//获取世界Boss血量
	UINT32 GetWorldBossBlood() const;

	//获取cd消耗
	UINT32 GetGameTypeEliminateExpend(BYTE type, BYTE times);

	//获取刷新消耗
	UINT32 GetGameTypeRefurbishExpend(BYTE type, BYTE times);

	//获取购买价格
	UINT32 GetGameTypeBuyPriceExpend(BYTE type, UINT16 times);

	//获取复活规则
	BYTE GetGameTypeReviveRule(BYTE type);

	//获取条件和奖励
	bool GetConditionAndReward(BYTE type, std::vector<GameTypeCfg::SRewardCondition> *&out);

	//获取天梯最大挑战次数
	UINT32 GetLadderMaxChallengeNum() const;

	//获取天梯挑战CD
	UINT32 GetLadderChallengeCDTime() const;

	//获取1v1奖励比率
	BYTE GetOneOnOneRewardRatio(BYTE finCode) const;

	//解除奖励嵌套
	void UnlockRewardNesting(const std::vector<GameTypeCfg::SRewardInfo> &source, std::vector<GameTypeCfg::SRewardInfo> *ret);

public:
	//获取怪物血量
	UINT32 GetMonsterBlood(UINT16 level);

	//获取怪物血比例
	UINT32 GetMonsterBloodProportion(UINT16 id);

public:
	//获取装备位强化属性
	BOOL GetStrengthenAttri(BYTE level, StrengthenAttriCfg::SStrengthenAttr &ret);

public:
	//获取天梯匹配范围
	UINT32 GetLadderMatchingScope(UINT32 rank);

public:
	// 生成周免角色
	BYTE GeneraWeekFreeActorType();

public:
	//获取活动副本开启条件
	bool GetActivityStageOpenCondition(UINT16 instId, ActivityStageCfg::Condition &ret);
};

BOOL InitGameCfgFileMan();
GameCfgFileMan* GetGameCfgFileMan();
void DestroyGameCfgFileMan();

#endif