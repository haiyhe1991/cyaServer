#include "RiseStarCfg.h"
#include "ServiceErrorCode.h"
#include "ServiceMLogic.h"

RiseStarCfg::RiseStarCfg()
{

}

RiseStarCfg::~RiseStarCfg()
{

}

BOOL RiseStarCfg::Load(const char* filename)
{
	Json::Value rootValue;
	if (!OpenCfgFile(filename, rootValue))
		return false;

	if (!rootValue.isMember("StartUpgrade") || !rootValue["StartUpgrade"].isArray())
		return false;

	int si = rootValue["StartUpgrade"].size();
	if (si <= 0)
		return false;

	for (int i = 0; i < si; ++i)
	{
		SRiseStarCfg starCfg;
		BYTE star = (BYTE)rootValue["StartUpgrade"][i]["Level"].asUInt();
		m_riseStarMap.insert(std::make_pair(star, starCfg));

		SRiseStarCfg& starCfgRef = m_riseStarMap[star];
		starCfgRef.extraEffect = rootValue["StartUpgrade"][i]["ExtraEffect"].asDouble();
		starCfgRef.damagedChance = (BYTE)rootValue["StartUpgrade"][i]["BreakageRate"].asUInt();
		starCfgRef.succedChance = (BYTE)rootValue["StartUpgrade"][i]["SuccessRate"].asUInt();
		starCfgRef.limitLevel = (BYTE)rootValue["StartUpgrade"][i]["PlayerLevelLimit"].asUInt();
		starCfgRef.spend.gold = rootValue["StartUpgrade"][i]["GoldCost"].asUInt();
		starCfgRef.spend.stones = (UINT16)rootValue["StartUpgrade"][i]["StarStone"].asUInt();
	}

	return true;
}

int RiseStarCfg::GetRiseStarSpend(BYTE starLv, SRiseStarSpend& spend)
{
	std::map<BYTE/*星级*/, SRiseStarCfg>::iterator it = m_riseStarMap.find(starLv);
	if (it == m_riseStarMap.end())
		return MLS_MAX_STAR_LEVEL;

	spend = it->second.spend;
	return MLS_OK;
}

int RiseStarCfg::GetRiseStarLimitLevel(BYTE starLv, BYTE& limitLevel)
{
	std::map<BYTE/*星级*/, SRiseStarCfg>::iterator it = m_riseStarMap.find(starLv);
	if (it == m_riseStarMap.end())
		return MLS_MAX_STAR_LEVEL;

	limitLevel = it->second.limitLevel;
	return MLS_OK;
}

int RiseStarCfg::GetStarTotalSpend(BYTE starLv, SRiseStarSpend& totalSpend)
{
	totalSpend.gold = 0;
	totalSpend.stones = 0;

	std::map<BYTE/*星级*/, SRiseStarCfg>::iterator it = m_riseStarMap.begin();
	for (; it != m_riseStarMap.end(); ++it)
	{
		totalSpend.stones = SGSU16Add(totalSpend.stones, it->second.spend.stones);
		totalSpend.gold = SGSU64Add(totalSpend.gold, it->second.spend.gold);

		if (starLv == it->first)
			break;
	}
	return MLS_OK;
}

int RiseStarCfg::GetRiseStarCfg(BYTE starLv, SRiseStarCfg& cfg)
{
	std::map<BYTE/*星级*/, SRiseStarCfg>::iterator it = m_riseStarMap.find(starLv);
	if (it == m_riseStarMap.end())
		return MLS_MAX_STAR_LEVEL;

	cfg = it->second;
	return MLS_OK;
}