#include "ReviveCfg.h"
#include "ServiceErrorCode.h"

ReviveCfg::ReviveCfg()
{

}

ReviveCfg::~ReviveCfg()
{

}

BOOL ReviveCfg::Load(const char* filename)
{
	Json::Value rootValue;
	if (!OpenCfgFile(filename, rootValue))
		return false;

	if (!rootValue.isMember("Revive") || !rootValue["Revive"].isArray())
		return false;

	int si = rootValue["Revive"].size();
	if (si <= 0)
		return true;

	SReviveAttr revive;
	for (int i = 0; i < si; ++i)
	{
		UINT16 id = (UINT16)rootValue["Revive"][i]["ID"].asUInt();
		std::map<UINT16, SReviveAttr>::iterator it = m_reviveCfgMap.find(id);
		if (it != m_reviveCfgMap.end())
		{
			ASSERT(false);
			continue;
		}
		m_reviveCfgMap.insert(std::make_pair(id, revive));
		SReviveAttr& reviveRef = m_reviveCfgMap[id];

		reviveRef.cdTimes = rootValue["Revive"][i]["CD"].asUInt();
		reviveRef.maxTimes = (BYTE)rootValue["Revive"][i]["MaxNum"].asUInt();
		int n = rootValue["Revive"][i]["ReviveCoinCost"].size();
		for (int j = 0; j < n; ++j)
			reviveRef.reviveCoinVec.push_back(rootValue["Revive"][i]["ReviveCoinCost"][j].asUInt());
	}

	return true;
}

int ReviveCfg::GetReviveConsume(UINT16 reviveId, UINT32 times, UINT16 vipAddTimes, UINT32& reviveCoin)
{
	std::map<UINT16, SReviveAttr>::iterator it = m_reviveCfgMap.find(reviveId);
	if (it == m_reviveCfgMap.end())
		return MLS_INVALID_REVIVE;

	if (it->second.maxTimes != 0)
	{
		BYTE maxReviveTimes = SGSU8Add(it->second.maxTimes, vipAddTimes);
		if (times > maxReviveTimes)
			return MLS_OVER_MAX_REVIVE_TIMES;
	}

	int si = (int)it->second.reviveCoinVec.size();
	if (si <= 0)
	{
		reviveCoin = 0;
		return MLS_OK;
	}

	reviveCoin = it->second.reviveCoinVec[times > si ? si - 1 : times - 1];
	return MLS_OK;
}

//ªÒ»°∏¥ªÓCD
int ReviveCfg::GetReviveCDTimes(UINT16 reviveId)
{
	std::map<UINT16, SReviveAttr>::iterator it = m_reviveCfgMap.find(reviveId);
	if (it == m_reviveCfgMap.end())
		return 0;
	return it->second.cdTimes;
}