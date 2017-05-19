#include "moduleInfo.h"
#include "MaterialCfg.h"
#include "ServiceErrorCode.h"
#include "cyaMaxMin.h"

MaterialCfg::MaterialCfg()
{

}

MaterialCfg::~MaterialCfg()
{

}

BOOL MaterialCfg::Load(const char* filename)
{
	Json::Value rootValue;
	if (!OpenCfgFile(filename, rootValue))
		return false;

	if (!rootValue.isMember("Material") || !rootValue["Material"].isArray())
		return false;

	int si = rootValue["Material"].size();
	for (int i = 0; i < si; ++i)
	{
		SMaterialAttr attr;
		UINT16 propId = rootValue["Material"][i]["ID"].asUInt();
		m_materialAttrCfg.insert(std::make_pair(propId, attr));

		SMaterialAttr& attrRef = m_materialAttrCfg[propId];
		attrRef.level = rootValue["Material"][i]["Level"].asUInt();
		attrRef.limitRoleLevel = (BYTE)rootValue["Material"][i]["LvLimit"].asUInt();
		attrRef.sellPrice = rootValue["Material"][i]["Price"].asUInt();
		//attrRef.backPrice = rootValue["Material"][i]["BackPrice"].asUInt();
		//attrRef.shopSellPrice = rootValue["Material"][i]["MallSellPrice"].asUInt();
		attrRef.bindMode = rootValue["Material"][i]["BindMode"].asUInt();
		attrRef.maxStackCount = rootValue["Material"][i]["StackCounts"].asUInt();
		attrRef.quality = rootValue["Material"][i]["Quality"].asUInt();

		if (!rootValue["Material"][i].isMember("Value") || !rootValue["Material"][i]["Value"].isArray())
			continue;

		int n = rootValue["Material"][i]["Value"].size();
		for (int j = 0; j < n; ++j)
			attrRef.funcValVec.push_back(rootValue["Material"][i]["Value"][j].asUInt());
	}

	return true;
}

int MaterialCfg::GetMaterialPropAttr(UINT16 materiaId, SPropertyAttr& propAttr, BYTE& maxStackCount)
{
	std::map<UINT16/*物品id*/, SMaterialAttr>::iterator it = m_materialAttrCfg.find(materiaId);
	if (it == m_materialAttrCfg.end())
		return MLS_PROPERTY_NOT_EXIST;

	propAttr.type = ESGS_PROP_MATERIAL;
	maxStackCount = it->second.maxStackCount;
	propAttr.propId = materiaId;
	propAttr.suitId = 0;
	propAttr.level = it->second.level;		//材料等级
	propAttr.grade = it->second.quality;	//材料品级;
	return MLS_OK;
}

BYTE MaterialCfg::GetMaterialMaxStack(UINT16 materiaId)
{
	BYTE maxStack = 1;
	std::map<UINT16/*物品id*/, SMaterialAttr>::iterator it = m_materialAttrCfg.find(materiaId);
	if (it != m_materialAttrCfg.end())
		maxStack = it->second.maxStackCount;

	return maxStack;
}

int MaterialCfg::GetMaterialUserSellPrice(UINT16 materiaId, UINT64& price)
{
	std::map<UINT16/*物品id*/, SMaterialAttr>::iterator it = m_materialAttrCfg.find(materiaId);
	if (it == m_materialAttrCfg.end())
		return MLS_PROPERTY_NOT_EXIST;

	price = it->second.sellPrice;
	return MLS_OK;
}

int MaterialCfg::GetMaterialFuncValue(UINT16 materiaId, UINT32& funcVal)
{
	std::map<UINT16/*物品id*/, SMaterialAttr>::iterator it = m_materialAttrCfg.find(materiaId);
	if (it == m_materialAttrCfg.end())
		return MLS_PROPERTY_NOT_EXIST;

	std::vector<UINT32>& valVec = it->second.funcValVec;
	funcVal = valVec.empty() ? 0 : valVec[0];
	return MLS_OK;
}

int MaterialCfg::GetMaterialFuncValueVec(UINT16 materiaId, std::vector<UINT32>& funcValVec)
{
	std::map<UINT16/*物品id*/, SMaterialAttr>::iterator it = m_materialAttrCfg.find(materiaId);
	if (it == m_materialAttrCfg.end())
		return MLS_PROPERTY_NOT_EXIST;

	funcValVec.assign(it->second.funcValVec.begin(), it->second.funcValVec.end());
	return MLS_OK;
}