#ifndef __ACTIVITY_CFG_H__
#define __ACTIVITY_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
活动配置
*/

#include <map>
#include <vector>
#include "CfgCom.h"

class ActivityCfg
{
public:
	ActivityCfg();
	~ActivityCfg();

#pragma pack(push, 1)
	struct SActivityItem
	{
		LTMSEL startMSel;
		LTMSEL endMSel;
		std::vector<SDropPropCfgItem> rewardVec;
	};
#pragma pack(pop)

public:
	//载入配置文件
	BOOL Load(const char* filename);
	//获取活动信息
	int GetActivityItem(UINT32 activityId, SActivityItem& activityItem);

private:
	std::string m_filename;
};

#endif