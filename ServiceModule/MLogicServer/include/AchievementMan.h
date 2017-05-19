#ifndef __ACHIEVEMENT_MAN_H__
#define __ACHIEVEMENT_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/**
* 成就管理
*/

#include <set>
#include <map>
#include <vector>
#include "cyaTypes.h"
#include "cyaLocker.h"
#include "cyaCoreTimer.h"
#include "GameCfgFileMan.h"

class AchievementMan
{
	AchievementMan(const AchievementMan&);
	AchievementMan& operator = (const AchievementMan&);

#pragma pack(push, 1)
	struct SDBAchievementItem
	{
		UINT16 id;				//成就ID
		bool receive;			//是否领取
	};
#pragma pack(pop)
	typedef std::map<UINT16, bool> achievement_map;
	typedef std::vector<SDBAchievementItem> achievement_array;

public:
	enum ComparisonType
	{
		EQUAL,						// 等于
		EQUAL_OR_GREATER,			// 大于或者等于
	};

	struct SAchievementRecord
	{
		UINT32 kill_common;			//杀死普通怪总量
		UINT32 kill_cream;			//杀死精英怪总量
		UINT32 kill_boss;			//杀死boss总量
		UINT32 pickup_box_sum;		//打开宝箱总量
		UINT32 continuous;			//连击次数
		UINT32 speed_evaluation;	//速度评价
		UINT32 hurt_evaluation;		//伤害评价
		UINT32 star_evaluation;		//三星评价
		UINT32 complatednum;		//角色通关总次数

		SAchievementRecord()
			: kill_common(0)
			, kill_cream(0)
			, kill_boss(0)
			, pickup_box_sum(0)
			, continuous(0)
			, speed_evaluation(0)
			, hurt_evaluation(0)
			, star_evaluation(0)
			, complatednum(0)
		{

		}
	};

public:
	AchievementMan();
	~AchievementMan();

	// 查询成就点数
	int QueryAchievementPoint(UINT32 roleId, SQueryAchievementRes *pAchievementInfo);

	// 查询成长之路
	int QueryRoadToGrowth(UINT32 userId, UINT32 roleId, UINT16 start_pos, SQueryAchievementRes *pAchievementInfo);

	// 查询冒险历程
	int QueryAdventureCourse(UINT32 userId, UINT32 roleId, UINT16 start_pos, SQueryAchievementRes *pAchievementInfo);

	// 查询完美技巧
	int QueryPerfectSkill(UINT32 userId, UINT32 roleId, UINT16 start_pos, SQueryAchievementRes *pAchievementInfo);

	// 领取成就奖励
	int ReceiveAchievementReward(UINT32 userId, UINT32 roleId, UINT32 achievement_id, SReceiveRewardRes *pAchievementReward, SReceiveRewardRefresh* pRefresh);

public:
	// 创建缓存
	bool CreateCache(UINT32 roleId, int &error_code);

	// 清理缓存
	bool ClearCache(UINT32 roleId);

public:
	// 战斗力更新(不可在此函数查询角色相关信息)
	int OnUpdateCombatPower(UINT32 roleId, UINT32 cp);

	// 获取宝石(不可在此函数查询角色相关信息)
	int OnGotGem(UINT32 roleId, BYTE gem_level);

public:
	// 角色升级
	int OnRoleUpgrade(UINT32 userId, UINT32 roleId);

	// 镶嵌宝石
	int OnInlaidGem(UINT32 userId, UINT32 roleId);

	// 装备位升星
	int OnRiseStar(UINT32 userId, UINT32 roleId);

	// 拾取宝箱
	int OnPickupInstBoxDrop(UINT32 userId, UINT32 roleId);

	// 击杀怪物
	int OnKillMonsterRecord(UINT32 userId, UINT32 roleId, UINT16 monsterId);

	// 完成副本
	int OnFinishInst(UINT32 userId, UINT32 roleId, UINT32 continuous, bool speed_evaluation, bool hurt_evaluation, bool star_evaluation);

	// 技能升级
	int OnSkillUpgrade(UINT32 userId, UINT32 roleId);

public:
	// 获取成就记录
	int GetAchievementRecord(UINT32 userId, UINT32 roleId, SAchievementRecord &record);

private:
	static void RefreshRoleTimer(void* param, TTimerID id);
	void OnRefreshRoleInfo();

public:
	// 写入成就记录到数据库
	bool WriteAchievementRecordToDB(UINT32 roleId, achievement_map &record);

	// 从数据库中读取成就记录
	bool ReadAchievementRecordFromDB(UINT32 roleId, achievement_map &record);

	// 激活成就
	bool ActivateAchievement(UINT32 roleId, AchievementCfg::TargetType target_type, UINT32 point, ComparisonType comparison, achievement_map &achievement);

private:
	CXTLocker m_total_locker;
	CXTLocker m_record_locker;
	CXTLocker m_update_locker;

	std::set<UINT32> m_updateSet;
	std::map<UINT32, achievement_map> m_achievement_record;
	std::map<UINT32, SAchievementRecord> m_achievement_total;

	CyaCoreTimer m_refreshDBTimer;
};

void InitAchievementMan();
AchievementMan* GetAchievementMan();
void DestroyAchievementMan();


#endif