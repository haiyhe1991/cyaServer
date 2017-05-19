#ifndef __LIMITED_TIME_ACTIVITY_CFG_H__
#define __LIMITED_TIME_ACTIVITY_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include <set>
#include <map>
#include "CfgCom.h"

class LimitedTimeActivityCfg
{
public:
	enum ActivityType
	{
		GUARD_ICON = 5002,			// 守卫圣像
		WORLD_BOSS = 5004,			// 世界Boss
		WORLD_BOSS_NOTICE = 1,		// 世界Boss公告
	};

	struct SActivityConfig
	{
		bool IsOpen;
		BYTE Type;
		BYTE StartTimeHour;
		BYTE StartTimeMinute;
		BYTE EndTimeHour;
		BYTE EndTimeHourMinute;
		UINT16 Interval;
		UINT16 Chance;
		UINT16 LastTime;
		UINT16 LimitNum;
		UINT16 MaxOpenNum;
		UINT16 MinOpenNum;
		UINT16 TransferNum;
		std::string Desc;
		//add hxw 20151203
		SActivityConfig()
		{
			IsOpen = FALSE;
			Type = 0;
			StartTimeHour = 0;
			StartTimeMinute = 0;
			EndTimeHour = 0;
			EndTimeHourMinute = 0;
			Interval = 0;
			Chance = 0;
			LastTime = 0;
			LimitNum = 0;
			MaxOpenNum = 0;
			MinOpenNum = 0;
			TransferNum = 0;
		}
	};

public:
	LimitedTimeActivityCfg();
	~LimitedTimeActivityCfg();

public:
	//载入配置
	BOOL Load(const char* pszFile);

	//获取限时活动配置
	bool GetLimitedTimeActivityCfg(ActivityType type, SActivityConfig &out);

	//生成活动副本
	bool GenerateActivityInst(int level, std::set<UINT16> passInsts, SActivityInstItem &item);

private:
	std::vector<int> m_activityInst;
	std::map<int, SActivityConfig> m_activityCfg;
};

#endif