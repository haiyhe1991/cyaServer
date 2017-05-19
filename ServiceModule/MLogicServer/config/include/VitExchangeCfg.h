#ifndef __VIT_EXCHANGE_CFG_H__
#define __VIT_EXCHANGE_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
体力兑换
*/

#include <map>
#include <vector>
#include "CfgCom.h"

class VitExchangeCfg
{
public:
	VitExchangeCfg();
	~VitExchangeCfg();

#pragma pack(push, 1)
	struct SVitExchangeAttr
	{
		std::vector<UINT32> base_power;		// 兑换数量
		std::vector<UINT32> token_expend;	// 兑换条件
		UINT16 base_num;					// 每日兑换上限
		BYTE vip_level_limit;				// Vip等级限制
	};
#pragma pack(pop)

public:
	//载入兑换配置
	BOOL Load(const char* filename);
	//获取兑换数据
	int GetVitExchangeExchange(UINT16 exchangeTimes, UINT16 vipAddTimes, UINT64& getVit, UINT32& spendToken);

private:
	SVitExchangeAttr m_vitExchange;
};


#endif