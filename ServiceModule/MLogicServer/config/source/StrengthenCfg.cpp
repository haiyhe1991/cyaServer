#include "StrengthenCfg.h"
#include "ServiceErrorCode.h"

StrengthenCfg::StrengthenCfg()
{

}

StrengthenCfg::~StrengthenCfg()
{

}

BOOL StrengthenCfg::Load(const char* filename)
{
	Json::Value rootValue;
	if (!OpenCfgFile(filename, rootValue))
		return false;

	if (!rootValue.isMember("Strengthens") || !rootValue["Strengthens"].isArray())
		return false;

	int si = rootValue["Strengthens"].size();
	if (si <= 0)
		return false;

	for (int i = 0; i < si; ++i)
	{
		BYTE pos = (BYTE)rootValue["Strengthens"][i]["Type"].asUInt();
		SEquipAttrInc inc;
		m_strengthenCfg.insert(std::make_pair(pos, inc));
		SEquipAttrInc& incRef = m_strengthenCfg[pos];

		incRef.masterInc = rootValue["Strengthens"][i]["MasterAttrIncre"].asUInt();
		incRef.depuInc = rootValue["Strengthens"][i]["DepuAttrIncre"].asUInt();
	}

	return true;
}

int StrengthenCfg::GetEquipAttrIncreament(BYTE pos, BYTE /*quality*/, SEquipAttrInc& attrInc)
{
	std::map<BYTE/*×°±¸Î»ÖÃ*/, SEquipAttrInc>::iterator it = m_strengthenCfg.find(pos);
	if (it == m_strengthenCfg.end())
		return MLS_PROPERTY_NOT_EXIST;

	attrInc = it->second;
	return MLS_OK;
}