#ifndef __ACHIEVEMENT_CFG_H__
#define __ACHIEVEMENT_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include <map>
#include <vector>
#include "CfgCom.h"

class AchievementCfg
{
public:
	// 目标类型枚举
	enum TargetType
	{
		ZZCZ = 11,			// 茁壮成长
		SQLZ = 12,			// 身强力壮
		SSYY = 13,			// 闪闪耀眼
		SYDM = 14,			// 闪耀夺目
		XXZB = 15,			// 星星装备
		YQDQ = 21,			// 一骑当千
		SBKD = 22,			// 势不可挡
		YMWD = 23,			// 勇猛无敌
		BZSC = 24,			// 百战沙场
		DBQB = 25,			// 夺宝奇兵
		LJDR = 31,			// 连击达人
		JSXF = 32,			// 急速先锋
		YYWZ = 33,			// 优雅舞者
		SXQZ = 34,			// 三星强者
		WDZZ = 35,			// 武道至尊
	};

	// 成就类型
	enum AchievementType
	{
		RoadToGrowth = 1,		//成长之路
		AdventureCourse = 2,	//冒险历程
		PerfectSkill = 3,		//完美技巧
	};

	// 成就条件
	struct SAchievementCondition
	{
		UINT32 condition;							//达成条件
		UINT32 premise_id;							//前置成就ID
	};

	// 成就道具奖励
	struct SAchievementPropAward
	{
		UINT32 id;									//物品ID
		BYTE type;									//物品类型
		BYTE num;									//物品数量
	};

	// 成就奖励
	struct SAchievementReward
	{
		UINT32 achievement_count;					//成就点奖励
		std::vector<SAchievementPropAward> reward;	//道具奖励
	};

	// 成就杂项
	struct SAchievementMisc
	{
		BYTE target_type;							//目标类型
		SAchievementReward reward;					//成就奖励
	};

public:
	AchievementCfg();
	~AchievementCfg();

public:
	//载入配置
	BOOL Load(const char* pszFile);

	//获取成就目标类型
	bool GetAchievementTargetType(UINT32 id, TargetType &type) const;

	//获取成就奖励
	bool GetAchievementReward(UINT32 id, SAchievementReward &reward) const;

	//获取成就条件
	bool GetAchievementCondition(UINT32 id, SAchievementCondition &condition) const;

	//根据成就类型获取成就
	bool GetAchievementFromType(AchievementType type, const std::vector<UINT32> *&out) const;

	//根据目标类型获取成就
	bool GetAchievementFromTargetType(TargetType type, const std::vector<UINT32> *&out) const;

private:
	std::map<UINT32, std::vector<UINT32> > m_targetType;
	std::map<UINT32, SAchievementMisc> m_achievementMisc;
	std::map<UINT32, std::vector<UINT32> > m_achievementType;
	std::map<UINT32, SAchievementCondition> m_achievementCondition;
};

#endif