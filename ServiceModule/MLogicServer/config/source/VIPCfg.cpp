#include "VIPCfg.h"

VIPCfg::VIPCfg()
{

}

VIPCfg::~VIPCfg()
{

}

BOOL VIPCfg::Load(const char* filename)
{
	Json::Value rootValue;
	if (!OpenCfgFile(filename, rootValue))
		return false;

	if (!rootValue.isMember("Vip") || !rootValue["Vip"].isArray())
		return false;

	int n = rootValue["Vip"].size();
	if (n <= 0)
		return true;

	SVIPAttrCfg vipAttr;
	for (int i = 0; i < n; ++i)
	{
		BYTE vip = (BYTE)rootValue["Vip"][i]["VipLv"].asUInt();
		std::map<BYTE/*VIP等级*/, SVIPAttrCfg>::iterator it = m_vipMap.find(vip);
		if (it != m_vipMap.end())
		{
			ASSERT(false);
			continue;
		}

		m_vipMap.insert(std::make_pair(vip, vipAttr));
		SVIPAttrCfg& vipAttrRef = m_vipMap[vip];
		vipAttrRef.buyVitNum = rootValue["Vip"][i]["BuyVitNum"].asUInt();
		vipAttrRef.deadReviveNum = rootValue["Vip"][i]["DeadReviveNum"].asUInt();
		vipAttrRef.goldExchangeNum = rootValue["Vip"][i]["GoldExchangeNum"].asUInt();
		vipAttrRef.instSweepNum = rootValue["Vip"][i]["InstSweepNum"].asUInt();
		vipAttrRef.mallBuyDiscount = rootValue["Vip"][i]["MallBuyPropDiscount"].asUInt();
		vipAttrRef.maxVit = rootValue["Vip"][i]["MaxVit"].asUInt();
		vipAttrRef.cashcount = rootValue["Vip"][i]["RechargeTotal"].asUInt();
		vipAttrRef.resourceInstChalNum = rootValue["Vip"][i]["ResourceInstChalNum"].asUInt();
		vipAttrRef.signReward = rootValue["Vip"][i]["SignUpRewardDouble"].asUInt();
		vipAttrRef.buyKOFnum = rootValue["Vip"][i]["KOFNum"].asUInt();
		vipAttrRef.fightNum = rootValue["Vip"][i]["FightNum"].asUInt();

		int nProps = rootValue["Vip"][i]["GivingProp"].size();
		for (int j = 0; j < nProps; ++j)
		{
			SVIPGiveProp prop;
			prop.type = rootValue["Vip"][i]["GivingProp"][j]["Type"].asUInt();
			prop.propId = rootValue["Vip"][i]["GivingProp"][j]["PropID"].asUInt();
			prop.num = rootValue["Vip"][i]["GivingProp"][j]["Num"].asUInt();
			vipAttrRef.givePropVec.push_back(prop);
		}
	}

	return true;
}

BYTE VIPCfg::CalcVIPLevel(UINT32 cashcount)
{
	if (cashcount == 0 || m_vipMap.empty())
		return 0;

	std::map<BYTE/*VIP等级*/, SVIPAttrCfg>::iterator it = m_vipMap.begin();
	std::map<BYTE/*VIP等级*/, SVIPAttrCfg>::iterator nextIt = it;
	while (it != m_vipMap.end())
	{
		if (cashcount < it->second.cashcount)
			return 0;

		nextIt = it;
		UINT32 n = it->second.cashcount;
		if (++nextIt == m_vipMap.end())
			return it->first;

		UINT32 m = nextIt->second.cashcount;
		if (cashcount >= n && cashcount < m)
			return it->first;
		it = nextIt;
	}

	return 0;
}

void VIPCfg::GetVIPGiveProps(BYTE vipLv, std::vector<SVIPGiveProp>*& pGivePropVec)
{
	std::map<BYTE/*VIP等级*/, SVIPAttrCfg>::iterator it = m_vipMap.find(vipLv);
	if (it != m_vipMap.end())
		pGivePropVec = &it->second.givePropVec;
}

void VIPCfg::GetVIPMaxVit(BYTE vipLv, UINT32& maxVit)
{
	std::map<BYTE/*VIP等级*/, SVIPAttrCfg>::iterator it = m_vipMap.find(vipLv);
	if (it != m_vipMap.end())
		maxVit = it->second.maxVit;
}

void VIPCfg::GetVIPBuyVitTimes(BYTE vipLv, UINT16& buyVitTimes)
{
	std::map<BYTE/*VIP等级*/, SVIPAttrCfg>::iterator it = m_vipMap.find(vipLv);
	if (it != m_vipMap.end())
		buyVitTimes = it->second.buyVitNum;
}

void VIPCfg::GetVIPGoldExchangeTimes(BYTE vipLv, UINT16& exchangeTimes)
{
	std::map<BYTE/*VIP等级*/, SVIPAttrCfg>::iterator it = m_vipMap.find(vipLv);
	if (it != m_vipMap.end())
		exchangeTimes = it->second.goldExchangeNum;
}

void VIPCfg::GetVIPDeadReviveTimes(BYTE vipLv, BYTE& reviveTimes)
{
	std::map<BYTE/*VIP等级*/, SVIPAttrCfg>::iterator it = m_vipMap.find(vipLv);
	if (it != m_vipMap.end())
		reviveTimes = it->second.deadReviveNum;
}

void VIPCfg::GetVIPInstSweepTimes(BYTE vipLv, UINT16& sweepTimes)
{
	std::map<BYTE/*VIP等级*/, SVIPAttrCfg>::iterator it = m_vipMap.find(vipLv);
	if (it != m_vipMap.end())
		sweepTimes = it->second.instSweepNum;
}

void VIPCfg::GetVIPResourceInstChalNum(BYTE vipLv, UINT16& chalTimes)
{
	std::map<BYTE/*VIP等级*/, SVIPAttrCfg>::iterator it = m_vipMap.find(vipLv);
	if (it != m_vipMap.end())
		chalTimes = it->second.resourceInstChalNum;
}

void VIPCfg::GetVIPMallBuyDiscount(BYTE vipLv, BYTE& discount)
{
	std::map<BYTE/*VIP等级*/, SVIPAttrCfg>::iterator it = m_vipMap.find(vipLv);
	if (it != m_vipMap.end())
		discount = it->second.mallBuyDiscount;
}

void VIPCfg::GetVIPSignReward(BYTE vipLv, BYTE& signReward)
{
	std::map<BYTE/*VIP等级*/, SVIPAttrCfg>::iterator it = m_vipMap.find(vipLv);
	if (it != m_vipMap.end())
		signReward = it->second.signReward;
}

// 获取VIP购买竞技模式次数
void VIPCfg::GetVIPBuyKOFTimes(BYTE vipLv, UINT16& buyKOFTimes)
{
	std::map<BYTE/*VIP等级*/, SVIPAttrCfg>::iterator it = m_vipMap.find(vipLv);
	if (it != m_vipMap.end())
		buyKOFTimes = it->second.buyKOFnum;
}

//获取1v1进入上限
void VIPCfg::GetOneVsOneEnterTimes(BYTE vipLv, UINT16& enterTimes)
{
	std::map<BYTE/*VIP等级*/, SVIPAttrCfg>::iterator it = m_vipMap.find(vipLv);
	if (it != m_vipMap.end())
		enterTimes = it->second.fightNum;
}