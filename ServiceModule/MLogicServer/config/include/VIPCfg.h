#ifndef __VIP_CFG_H__
#define __VIP_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include <map>
#include <vector>
#include "CfgCom.h"

class VIPCfg
{
public:
	VIPCfg();
	~VIPCfg();

#pragma pack(push, 1)
	struct SVIPGiveProp
	{
		BYTE type;
		UINT16 propId;
		UINT32 num;

		SVIPGiveProp()
		{
			type = 0;
			propId = 0;
			num = 0;
		}
	};
	struct SVIPAttrCfg
	{
		UINT32 cashcount;	//达到充值总额
		UINT32 maxVit;		//最大体力
		UINT16 buyVitNum;	//购买体力次数
		UINT16 goldExchangeNum;	//金币兑换次数
		UINT16 buyKOFnum;			//竞技模式购买次数
		UINT16 fightNum;			//1v1进入次数上限
		BYTE   deadReviveNum;	//死亡复活次数
		UINT16 instSweepNum;	//副本扫荡次数
		UINT16 resourceInstChalNum;	//资源副本挑战次数
		BYTE   mallBuyDiscount;	//商城购买道具折扣
		BYTE   signReward;		//签到奖励倍数
		std::vector<SVIPGiveProp> givePropVec;	//赠送物品

		SVIPAttrCfg()
		{
			buyKOFnum = 0;
			cashcount = 0;
			maxVit = 0;
			buyVitNum = 0;
			goldExchangeNum = 0;
			deadReviveNum = 0;
			instSweepNum = 0;
			resourceInstChalNum = 0;
			mallBuyDiscount = 0;
			signReward = 0;
			givePropVec.clear();
		}
	};
#pragma pack(pop)

public:
	//载入配置
	BOOL Load(const char* filename);
	//计算VIP等级
	BYTE CalcVIPLevel(UINT32 cashcount);
	//获取VIP赠送物品
	void GetVIPGiveProps(BYTE vipLv, std::vector<SVIPGiveProp>*& pGivePropVec);
	//获取VIP最大体力
	void GetVIPMaxVit(BYTE vipLv, UINT32& maxVit);
	//获取VIP购买体力次数
	void GetVIPBuyVitTimes(BYTE vipLv, UINT16& buyVitTimes);
	//获取VIP金币兑换次数
	void GetVIPGoldExchangeTimes(BYTE vipLv, UINT16& exchangeTimes);
	//获取VIP死亡复活次数
	void GetVIPDeadReviveTimes(BYTE vipLv, BYTE& reviveTimes);
	//获取VIP副本扫荡次数
	void GetVIPInstSweepTimes(BYTE vipLv, UINT16& sweepTimes);
	//获取VIP资源副本挑战次数
	void GetVIPResourceInstChalNum(BYTE vipLv, UINT16& chalTimes);
	//获取VIP商城购买道具折扣
	void GetVIPMallBuyDiscount(BYTE vipLv, BYTE& discount);
	//获取VIP签到奖励倍数
	void GetVIPSignReward(BYTE vipLv, BYTE& signReward);
	//获取VIP购买竞技模式次数
	void GetVIPBuyKOFTimes(BYTE vipLv, UINT16& buyKOFTimes);
	//获取1v1进入上限
	void GetOneVsOneEnterTimes(BYTE vipLv, UINT16& enterTimes);

private:
	std::map<BYTE/*VIP等级*/, SVIPAttrCfg> m_vipMap;
};

#endif