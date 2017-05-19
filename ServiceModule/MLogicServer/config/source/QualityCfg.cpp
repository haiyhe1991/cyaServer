#include "QualityCfg.h"
#include "ServiceErrorCode.h"

QualityCfg::QualityCfg()
{

}

QualityCfg::~QualityCfg()
{

}

BOOL QualityCfg::Load(const char* filename)
{
	Json::Value rootValue;
	if (!OpenCfgFile(filename, rootValue))
		return false;

	if (!rootValue.isMember("Quality") || !rootValue["Quality"].isArray())
		return false;

	int si = rootValue["Quality"].size();
	if (si <= 0)
		return false;

	for (int i = 0; i < si; ++i)
	{
		BYTE qualityId = (BYTE)rootValue["Quality"][i]["ID"].asUInt();
		BYTE maxLv = (BYTE)rootValue["Quality"][i]["MaxLevel"].asUInt();
		m_qualityCfgMap.insert(std::make_pair(qualityId, maxLv));
	}
	return true;
}

int QualityCfg::GetMaxStrengthenLevel(BYTE qualityId, BYTE& maxStrengthenLv)
{
	std::map<BYTE/*品质id*/, BYTE/*最高强化等级*/>::iterator it = m_qualityCfgMap.find(qualityId);
	if (it == m_qualityCfgMap.end())
		return MLS_PROPERTY_NOT_EXIST;

	maxStrengthenLv = it->second;
	return MLS_OK;
}