#ifndef __GOLD_EXCHANGE_CFG_H__
#define __GOLD_EXCHANGE_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
金币兑换
*/

#include <map>
#include "CfgCom.h"
#include <vector>
class GoldExchangeCfg
{
public:
	GoldExchangeCfg();
	~GoldExchangeCfg();

#pragma pack(push, 1)//by hxw 20150902 
	struct SGoldExchangeAttr
	{
		UINT64 getGold;		//获得金币
		std::vector<UINT32> spendToken;	//花费代币数组
		UINT32 goldIncreament;	//金币每次增量
		//	UINT32 tokenIncreament;	//代币每次增量
		UINT16 everydayMaxTimes;	//每天最大兑换次数
		BYTE   vipLimitLv;			//vip等级限制


		SGoldExchangeAttr()
		{
			getGold = 0;
			goldIncreament = 0;
			//	tokenIncreament = 0;
			everydayMaxTimes = 0;
			vipLimitLv = 0;
		}
	};
#pragma pack(pop)

public:
	//载入兑换配置
	BOOL Load(const char* filename);
	//获取兑换数据
	int GetGoldExchange(UINT16 exchangeTimes, UINT16 vipAddTimes, UINT64& getGold, UINT32& spendToken);

private:
	SGoldExchangeAttr m_goldExchange;
};

#endif