#ifndef __LV_RANKING_CFG_H__
#define __LV_RANKING_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
等级排行奖励活动 add by hxw 20150929
*/

#include <vector>
#include "CfgCom.h"
#include "ServiceErrorCode.h"

class LvRankingCfg
{
public:
	LvRankingCfg(void);
	virtual ~LvRankingCfg(void);


	struct SLvRanking
	{
		BOOL IsOpen;
		LTMSEL StartTime;
		int  EndTime;
		std::vector<SDropPropCfgItem> propVec;
		SLvRanking()
		{
			IsOpen = false;
			EndTime = 0;
		}
	};

	struct SLvRankingCV
	{
		std::vector<SDropPropCfgItem> rewardvs;
		int		  cvalue;
		SLvRankingCV(){
			cvalue = 0;
		}
	};

public:
	//载入配置文件
	BOOL Load(const char* filename);
	//获取等级排行奖励
	BOOL GetLvRanking(const BYTE rank, const int Lv, std::vector<SDropPropCfgItem>*& items);
	//是否是活动时间，返回TRUE表示活动继续，FLASE表示无法活动
	BOOL IsValidityTime();
	//是否开启
	BOOL IsOpen() const { return m_IsOpen; }

	void SetOpen(const BOOL bOpen) { m_IsOpen = bOpen; }

private:
	std::string m_filename;
	BOOL m_IsOpen;
	//是否有效时间内
	BOOL m_IsValidity;
	LTMSEL m_StartTime;
	int  m_EndTime;  //单位小时
	std::map<BYTE/*排名*/, SLvRankingCV> m_LvRewardsMap;
};

#endif