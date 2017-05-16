#ifndef __DAILY_SIGN_CFG_H__
#define __DAILY_SIGN_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include <map>
#include <vector>
#include "CfgCom.h"

//每日签到

class DailySignCfg
{
public:
	// 道具奖励
	struct SDailySignAward
	{
		UINT32 id;									//物品ID
		BYTE type;									//物品类型
		UINT32 num;									//物品数量
	};

	struct SDailySignItem
	{
		BYTE double_vip_level;						//双倍奖励所需vip等级
		bool continuous;							//是否连续
		std::vector<SDailySignAward> award;			//奖励
	};

	// 每日签到配置
	struct SDailySignConfig
	{
		bool is_open;								//是否开启
		LTMSEL start_time;							//开始时间
		LTMSEL end_time;							//结束时间
		std::map<BYTE, SDailySignItem> data;
	};


public:
	DailySignCfg();
	~DailySignCfg();

public:
	//载入配置
	BOOL Load(const char* pszFile);

	//是否有效
	bool IsDailySignValid() const;

	//获取奖励
	bool GetDailySignConfig(BYTE days, const SDailySignItem *&config);

private:
	SDailySignConfig m_signConfig;
};

#endif