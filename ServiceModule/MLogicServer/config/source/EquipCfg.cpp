#include "EquipCfg.h"
#include "moduleInfo.h"
#include "ServiceErrorCode.h"
#include "ServiceProtocol.h"
#include "cyaMaxMin.h"

EquipCfg::EquipCfg()
{

}

EquipCfg::~EquipCfg()
{

}

BYTE EquipCfg::GetEquipMaxStack(UINT16 /*equipId*/)
{
	return 1;
}

int EquipCfg::GetEquipPropAttr(UINT16 equipId, SPropertyAttr& propAttr, BYTE& maxStack)
{
	maxStack = 1;
	std::map<UINT16/*装备id*/, SEquitAttr>::iterator it = m_equipMap.find(equipId);
	if (it == m_equipMap.end())
		return MLS_PROPERTY_NOT_EXIST;

	propAttr.type = ESGS_PROP_EQUIP;
	propAttr.propId = it->first;			//id
	propAttr.grade = it->second.quality;	//品质
	propAttr.pos = it->second.pos;			//装备位置
	propAttr.suitId = it->second.suitId;	//套装id

	return MLS_OK;
}

BOOL EquipCfg::Load(const char* filename)
{
	Json::Value rootValue;
	if (!OpenCfgFile(filename, rootValue))
		return false;

	if (!rootValue.isMember("Equipment") || !rootValue["Equipment"].isArray())
		return false;

	int si = rootValue["Equipment"].size();
	if (si <= 0)
		return false;

	for (int i = 0; i < si; ++i)
	{
		UINT16 id = rootValue["Equipment"][i]["ID"].asUInt();
		std::map<UINT16/*装备id*/, SEquitAttr>::iterator it = m_equipMap.find(id);
		if (it != m_equipMap.end())
		{

#if defined(WIN32)
			throw(id);
#endif
			ASSERT(false);
			continue;
		}
		SEquitAttr attr;
		m_equipMap.insert(std::make_pair(id, attr));
		SEquitAttr& attrRef = m_equipMap[id];

		attrRef.pos = rootValue["Equipment"][i]["EquipSocket"].asUInt();
		attrRef.name = rootValue["Equipment"][i]["Name"].asCString();
		attrRef.quality = rootValue["Equipment"][i]["QualityID"].asUInt();
		attrRef.sellPrice = rootValue["Equipment"][i]["Price"].asUInt();
		attrRef.suitId = rootValue["Equipment"][i]["SuitId"].asUInt();
		if (attrRef.suitId > 0)
		{
			std::map<UINT16/*套装id*/, std::set<UINT16>/*装备id*/ >::iterator it = m_suitEquipMap.find(attrRef.suitId);
			if (it == m_suitEquipMap.end())
			{
				std::set<UINT16> equitSet;
				m_suitEquipMap.insert(std::make_pair(attrRef.suitId, equitSet));
			}
			m_suitEquipMap[attrRef.suitId].insert(id);
		}

		if (!rootValue["Equipment"][i].isMember("Attributes") || !rootValue["Equipment"][i]["Attributes"].isArray())
			continue;

		int n = rootValue["Equipment"][i]["Attributes"].size();
		for (int j = 0; j < n; ++j)
		{
			SEquipAddPoint pt;
			pt.val = rootValue["Equipment"][i]["Attributes"][j]["AttrVal"].asUInt();
			pt.master = rootValue["Equipment"][i]["Attributes"][j]["IsMainAttr"].asUInt();
			pt.type = rootValue["Equipment"][i]["Attributes"][j]["Type"].asUInt();
			attrRef.equipPointVec.push_back(pt);
		}
	}

	return true;
}

int EquipCfg::GetEquipUserSellPrice(UINT16 equipId, UINT64& price)
{
	std::map<UINT16/*装备id*/, SEquitAttr>::iterator it = m_equipMap.find(equipId);
	if (it == m_equipMap.end())
		return MLS_PROPERTY_NOT_EXIST;

	price = it->second.sellPrice;
	return MLS_OK;
}

void EquipCfg::GetEquipName(UINT16 equipId, std::string& name)
{
	std::map<UINT16/*装备id*/, SEquitAttr>::iterator it = m_equipMap.find(equipId);
	if (it == m_equipMap.end())
		return;

	name = it->second.name.c_str();
}


int EquipCfg::GetEquipAttrPoint(UINT16 equipId, std::vector<SEquipAddPoint>*& addPoint)
{
	std::map<UINT16/*装备id*/, SEquitAttr>::iterator it = m_equipMap.find(equipId);
	if (it == m_equipMap.end())
		return MLS_PROPERTY_NOT_EXIST;

	addPoint = &it->second.equipPointVec;
	return MLS_OK;
}