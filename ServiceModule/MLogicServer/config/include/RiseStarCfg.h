#ifndef __RISE_STAR_CFG_H__
#define __RISE_STAR_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
装备升星配置
*/

#include <map>
#include "CfgCom.h"

class RiseStarCfg
{
public:
	RiseStarCfg();
	~RiseStarCfg();

#pragma pack(push, 1)
	struct SRiseStarSpend
	{
		UINT16 stones;	//需要升星石数量
		UINT64 gold;	//花费金币
		SRiseStarSpend()
		{
			stones = 0;
			gold = 0;
		}
	};

	struct SRiseStarCfg
	{
		BYTE succedChance;	//成功率
		BYTE damagedChance;	//破损率
		BYTE limitLevel;	//人物等级限制
		float extraEffect;
		SRiseStarSpend spend;
		SRiseStarCfg()
		{
			succedChance = 0;
			damagedChance = 0;
			limitLevel = 0;
		}
	};
#pragma pack(pop)

public:
	//载入升星配置
	BOOL Load(const char* filename);
	//获取升星花费
	int GetRiseStarSpend(BYTE starLv, SRiseStarSpend& spend);
	//获取该星级人物等级限制
	int GetRiseStarLimitLevel(BYTE starLv, BYTE& limitLevel);
	//获取某星级消耗(升星石数量,金币)
	int GetStarTotalSpend(BYTE starLv, SRiseStarSpend& totalSpend);
	//获取升星配置
	int GetRiseStarCfg(BYTE starLv, SRiseStarCfg& cfg);

private:
	std::map<BYTE/*星级*/, SRiseStarCfg> m_riseStarMap;
};


#endif