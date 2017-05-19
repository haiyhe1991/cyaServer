#ifndef __RECHARGE_CFG_H__
#define __RECHARGE_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
充值配置
*/

#include <map>
#include "CfgCom.h"

class RechargeCfg
{
public:
	RechargeCfg();
	~RechargeCfg();

#pragma pack(push, 1)
	struct SRechargeCfgAttr
	{
		UINT32 rmb;			//人民币
		UINT32 rpcoin;		//代币
		UINT32 firstGive;	//首次充值赠送代币
		UINT32 everydayGive;	//每日赠送代币
		UINT16 giveDays;		//赠送天数

		SRechargeCfgAttr()
		{
			rmb = 0;
			rpcoin = 0;
			firstGive = 0;
			everydayGive = 0;
			giveDays = 0;
		}
	};
#pragma pack(pop)

public:
	//载入充值配置
	BOOL Load(const char* filename);
	//获取充值属性
	int GetRechargeAttr(UINT16 cashId, SRechargeCfgAttr& cashAttr);
	//获取充值配置
	const std::map<UINT16, SRechargeCfgAttr>& Get() const;

private:
	std::map<UINT16/*充值id*/, SRechargeCfgAttr> m_rechargeMap;
};


#endif