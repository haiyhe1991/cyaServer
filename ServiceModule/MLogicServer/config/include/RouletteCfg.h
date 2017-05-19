#ifndef __ROULETTE_CFG_H__
#define __ROULETTE_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
轮盘配置
*/

#include <vector>
#include "CfgCom.h"
#include "ServiceMLogic.h"

class RouletteCfg
{
public:
	RouletteCfg();
	~RouletteCfg();

#pragma pack(push, 1)
	struct SRoulettePropCfgItem
	{
		BYTE id;		//轮盘格子id
		BYTE type;		//类型(ESGSPropType)
		UINT16 propId;	//物品id
		UINT32 num;		//物品数量
		UINT16 chance;	//概率
	};
#pragma pack(pop)

public:
	//载入配置文件
	BOOL Load(const char* filename);
	//获取每天赠送次数
	BYTE GetEverydayGiveTimes() const;
	//生成轮盘奖品
	BOOL GenerateRouletteReward(SGetRoulettePropRes* pRoulettePropRes, std::vector<SRoulettePropCfgItem>& saveItemVec);
	BOOL GenerateRouletteReward(std::vector<SRoulettePropCfgItem>& itemVec);
	//生成轮盘大奖
	BOOL GenerateRouletteBigReward(SRoulettePropCfgItem& bigReward, UINT32& spendToken);

private:
	std::string m_filename;
	BYTE m_everydayGiveTimes;
};

#endif