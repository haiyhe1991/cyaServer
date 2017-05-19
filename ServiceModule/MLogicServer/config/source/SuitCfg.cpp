#include "SuitCfg.h"
#include "ServiceErrorCode.h"

SuitCfg::SuitCfg()
{

}

SuitCfg::~SuitCfg()
{

}

BOOL SuitCfg::Load(const char* filename)
{
	Json::Value rootValue;
	if (!OpenCfgFile(filename, rootValue))
		return false;

	if (!rootValue.isMember("Suit") || !rootValue["Suit"].isArray())
		return false;

	int si = rootValue["Suit"].size();
	if (si <= 0)
		return false;

	SSuitAttrObj suit;
	for (int i = 0; i < si; ++i)
	{
		UINT16 suitId = rootValue["Suit"][i]["ID"].asUInt();
		m_suitAttrMap.insert(std::make_pair(suitId, suit));

		SSuitAttrObj& suitRef = m_suitAttrMap[suitId];
		suitRef.lv = rootValue["Suit"][i]["SuitLV"].asUInt();
		suitRef.quality = rootValue["Suit"][i]["Quality"].asUInt();

		SSuitAttrItem item;
		int n = rootValue["Suit"][i]["Attributes"].size();
		for (int j = 0; j < n; ++j)
		{
			item.type = rootValue["Suit"][i]["Attributes"][j]["Type"].asUInt();
			item.val = rootValue["Suit"][i]["Attributes"][j]["Value"].asUInt();
			suitRef.attrVec.push_back(item);
		}
	}
	return true;
}

int SuitCfg::GetSuitAttr(UINT16 suitId, SSuitAttrObj*& pAttrObj)
{
	std::map<UINT16, SSuitAttrObj>::iterator it = m_suitAttrMap.find(suitId);
	if (it == m_suitAttrMap.end())
		return -1;
	pAttrObj = &it->second;
	return 0;
}

//获取套装附加值
bool SuitCfg::GetSuitAttrValue(UINT16 suitId, BYTE type, UINT32 *value)
{
	*value = 0;
	std::map<UINT16, SSuitAttrObj>::iterator it = m_suitAttrMap.find(suitId);
	if (it == m_suitAttrMap.end())
		return false;

	for (size_t i = 0; i < it->second.attrVec.size(); ++i)
	{
		if (it->second.attrVec[i].type == type)
		{
			*value = it->second.attrVec[i].val;
			return true;
		}
	}
	return false;
}