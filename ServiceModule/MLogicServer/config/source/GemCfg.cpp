#include "GemCfg.h"
#include "moduleInfo.h"
#include "ServiceErrorCode.h"

GemCfg::GemCfg()
{

}

GemCfg::~GemCfg()
{

}

BOOL GemCfg::Load(const char* szGemFile, const char* szGemCostFile, const char* szGemHoleFile)
{
	if (!LoadGemCfg(szGemFile))
		return false;

	if (!LoadGemConsumeCfg(szGemCostFile))
		return false;

	if (!LoadEquipGemCfg(szGemHoleFile))
		return false;

	return true;
}

BOOL GemCfg::LoadGemCfg(const char* filename)
{
	Json::Value rootValue;
	if (!OpenCfgFile(filename, rootValue))
		return false;

	if (!rootValue.isMember("Gems") || !rootValue["Gems"].isArray())
		return false;

	int si = rootValue["Gems"].size();
	if (si <= 0)
		return false;

	for (int i = 0; i < si; ++i)
	{
		SGemAttrCfg gemAttr;
		UINT16 id = (UINT16)rootValue["Gems"][i]["ID"].asUInt();
		m_gemAttrMap.insert(std::make_pair(id, gemAttr));

		SGemAttrCfg& gemAttrRef = m_gemAttrMap[id];
		gemAttrRef.type = rootValue["Gems"][i]["Type"].asUInt();
		gemAttrRef.level = rootValue["Gems"][i]["Level"].asUInt();
		//gemAttrRef.shopSellToken = rootValue["Gems"][i]["TokenValue"].asUInt();
		gemAttrRef.playerSellGold = rootValue["Gems"][i]["GoldValue"].asUInt();
		//gemAttrRef.playerBuyBackGold = rootValue["Gem"][i]["BuyBack"].asUInt();
		gemAttrRef.attrVal = rootValue["Gems"][i]["AttrVal"].asUInt();
		gemAttrRef.maxStack = rootValue["Gems"][i]["MaxStack"].asUInt();
	}
	return true;
}

BOOL GemCfg::LoadGemHoleLimitCfg(const char* filename)
{
	Json::Value rootValue;
	if (!OpenCfgFile(filename, rootValue))
		return false;

	if (!rootValue.isMember("GemHoleLimit") || !rootValue["GemHoleLimit"].isArray())
		return false;

	int si = rootValue["Gem"].size();
	if (si <= 0)
		return false;

	for (int i = 0; i < si; ++i)
	{
		BYTE equipLv = (BYTE)rootValue["Gem"][i]["LevelLimit"].asUInt();
		BYTE holsIdx = (BYTE)rootValue["Gem"][i]["HoleIndex"].asUInt();
		m_equipLimitHolesMap.insert(std::make_pair(equipLv, holsIdx));
	}

	return true;
}

BOOL GemCfg::LoadGemConsumeCfg(const char* filename)
{
	Json::Value rootValue;
	if (!OpenCfgFile(filename, rootValue))
		return false;

	if (!rootValue.isMember("GemInlayCost") || !rootValue["GemInlayCost"].isArray())
		return false;

	int si = rootValue["GemInlayCost"].size();
	if (si <= 0)
		return false;

	for (int i = 0; i < si; ++i)
	{
		BYTE gemLv = (BYTE)rootValue["GemInlayCost"][i]["GemLevel"].asUInt();
		SGemConsumeCfg consumeCfg;
		m_gemConsumeMap.insert(std::make_pair(gemLv, consumeCfg));

		SGemConsumeCfg& consumeCfgRef = m_gemConsumeMap[gemLv];
		consumeCfgRef.composeGold = rootValue["GemInlayCost"][i]["CombineCost"].asUInt();
		consumeCfgRef.decomposeGold = rootValue["GemInlayCost"][i]["UnCombineCost"].asUInt();
		consumeCfgRef.loadGold = rootValue["GemInlayCost"][i]["InlayCost"].asUInt();
		consumeCfgRef.unloadGold = rootValue["GemInlayCost"][i]["UnlayCost"].asUInt();
	}

	return true;
}

BOOL GemCfg::LoadEquipGemCfg(const char* filename)
{
	Json::Value rootValue;
	if (!OpenCfgFile(filename, rootValue))
		return false;

	if (!rootValue.isMember("EquipmentGemHole") || !rootValue["EquipmentGemHole"].isArray())
		return false;

	int si = rootValue["EquipmentGemHole"].size();
	if (si <= 0)
		return false;

	for (int i = 0; i < si; ++i)
	{
		std::vector<SEquipGemAttr> vec;
		BYTE equipType = (BYTE)rootValue["EquipmentGemHole"][i]["EquipmentType"].asUInt();
		m_equipGemHolesMap.insert(std::make_pair(equipType, vec));

		int n = rootValue["EquipmentGemHole"][i]["HoleAttr"].size();
		std::vector<SEquipGemAttr>& vecRef = m_equipGemHolesMap[equipType];
		for (int j = 0; j < n; ++j)
		{
			SEquipGemAttr gemAttr;
			gemAttr.equipLimitlv = rootValue["EquipmentGemHole"][i]["HoleAttr"][j]["LevelLimit"].asUInt();
			gemAttr.gemType = rootValue["EquipmentGemHole"][i]["HoleAttr"][j]["GemType"].asUInt();
			vecRef.push_back(gemAttr);
		}
	}

	return true;
}

int GemCfg::GetEquipInlaidGemAttr(BYTE equipType, BYTE holeSeq, SEquipGemAttr& attr)
{
	std::map<BYTE/*装备类型*/, std::vector<SEquipGemAttr>/*孔镶嵌宝石类型*/ >::iterator it = m_equipGemHolesMap.find(equipType);
	if (it == m_equipGemHolesMap.end())
		return MLS_PROPERTY_NOT_EXIST;

	std::vector<SEquipGemAttr>& vec = it->second;
	if (holeSeq >= vec.size())
		return MLS_GEM_HOLE_INVALID;

	attr = vec[holeSeq];
	return MLS_OK;
}

int GemCfg::GetGemSpendByLevel(BYTE gemLv, SGemConsumeCfg& spend)
{
	std::map<BYTE/*宝石等级*/, SGemConsumeCfg>::iterator it = m_gemConsumeMap.find(gemLv);
	if (it == m_gemConsumeMap.end())
		return MLS_PROPERTY_NOT_EXIST;

	spend = it->second;
	return MLS_OK;
}

int GemCfg::GetGemCfgAttr(UINT16 gemPropId, SGemAttrCfg& gemAttr)
{
	std::map<UINT16/*宝石id*/, SGemAttrCfg>::iterator it = m_gemAttrMap.find(gemPropId);
	if (it == m_gemAttrMap.end())
		return MLS_PROPERTY_NOT_EXIST;

	gemAttr = it->second;
	return MLS_OK;
}

int GemCfg::GetGemPropAttr(UINT16 gemPropId, SPropertyAttr& propAttr, BYTE& maxStack)
{
	std::map<UINT16/*宝石id*/, SGemAttrCfg>::iterator it = m_gemAttrMap.find(gemPropId);
	if (it == m_gemAttrMap.end())
		return MLS_PROPERTY_NOT_EXIST;

	propAttr.type = ESGS_PROP_GEM;
	propAttr.propId = gemPropId;
	propAttr.level = it->second.level;
	propAttr.grade = it->second.type;
	maxStack = it->second.maxStack;
	return MLS_OK;
}

int GemCfg::GetGemIdMaxStackByTypeLevel(BYTE gemType, BYTE gemLv, UINT16& gemId, BYTE& maxStack)
{
	std::map<UINT16/*宝石id*/, SGemAttrCfg>::iterator it = m_gemAttrMap.begin();
	for (; it != m_gemAttrMap.end(); ++it)
	{
		if (it->second.type == gemType && it->second.level == gemLv)
		{
			gemId = it->first;
			maxStack = it->second.maxStack;
			return MLS_OK;
		}
	}

	gemId = 0;
	maxStack = 0;
	return MLS_PROPERTY_NOT_EXIST;
}

BYTE GemCfg::GetGemMaxStack(UINT16 gemId)
{
	BYTE maxStack = 1;
	std::map<UINT16/*宝石id*/, SGemAttrCfg>::iterator it = m_gemAttrMap.find(gemId);
	if (it != m_gemAttrMap.end())
		maxStack = it->second.maxStack;
	return maxStack;
}

int GemCfg::GetGemUserSellPrice(UINT16 gemId, UINT64& price)
{
	std::map<UINT16/*宝石id*/, SGemAttrCfg>::iterator it = m_gemAttrMap.find(gemId);
	if (it == m_gemAttrMap.end())
		return MLS_PROPERTY_NOT_EXIST;

	price = it->second.playerSellGold;
	return MLS_OK;
}

int GemCfg::GetGemSpendById(UINT16 gemId, SGemConsumeCfg& spend)
{
	std::map<UINT16/*宝石id*/, SGemAttrCfg>::iterator it = m_gemAttrMap.find(gemId);
	if (it == m_gemAttrMap.end())
		return MLS_PROPERTY_NOT_EXIST;

	return GetGemSpendByLevel(it->second.level, spend);
}

UINT16 GemCfg::GetComposeGemConsumeGems() const
{
	return 4;
}