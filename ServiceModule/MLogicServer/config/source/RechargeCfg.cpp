#include "RechargeCfg.h"
#include "ServiceErrorCode.h"

RechargeCfg::RechargeCfg()
{

}

RechargeCfg::~RechargeCfg()
{

}

BOOL RechargeCfg::Load(const char* filename)
{
	Json::Value rootValue;
	if (!OpenCfgFile(filename, rootValue))
		return false;

	if (!rootValue.isMember("Recharge") || !rootValue["Recharge"].isArray())
		return false;

	int si = rootValue["Recharge"].size();
	if (si <= 0)
		return false;

	for (int i = 0; i < si; ++i)
	{
		SRechargeCfgAttr attr;
		UINT16 id = (UINT16)rootValue["Recharge"][i]["ID"].asUInt();
		std::map<UINT16/*³äÖµid*/, SRechargeCfgAttr>::iterator it = m_rechargeMap.find(id);
		if (it != m_rechargeMap.end())
		{
			ASSERT(false);
			continue;
		}

		attr.rmb = rootValue["Recharge"][i]["RMB"].asUInt();
		attr.rpcoin = rootValue["Recharge"][i]["DiamondsNum"].asUInt();
		attr.firstGive = rootValue["Recharge"][i]["FirstGiving"].asUInt();
		attr.everydayGive = rootValue["Recharge"][i]["EverydayGiving"].asUInt();
		attr.giveDays = (UINT16)rootValue["Recharge"][i]["GivingTimeLimit"].asUInt();
		m_rechargeMap.insert(std::make_pair(id, attr));
	}

	return true;
}

int RechargeCfg::GetRechargeAttr(UINT16 cashId, SRechargeCfgAttr& cashAttr)
{
	std::map<UINT16/*³äÖµid*/, SRechargeCfgAttr>::iterator it = m_rechargeMap.find(cashId);
	if (it == m_rechargeMap.end())
		return MLS_INVALID_CHASH_ID;

	cashAttr = it->second;
	return MLS_OK;
}

const std::map<UINT16, RechargeCfg::SRechargeCfgAttr>& RechargeCfg::Get() const
{
	return m_rechargeMap;
}