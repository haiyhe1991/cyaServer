#include "EquipComposeCfg.h"
#include "cyaTime.h"
#include "ServiceErrorCode.h"

EquipComposeCfg::EquipComposeCfg()
{

}

EquipComposeCfg::~EquipComposeCfg()
{
	std::map<UINT16, SEquipComposeItem>::iterator it = m_equipComposeMap.begin();
	for (; it != m_equipComposeMap.end(); ++it)
	{
		int n = (int)it->second.conditionVec.size();
		for (int i = 0; i < n; ++i)
		{
			if (it->second.conditionVec[i].type == ESGS_EQUIP_COMPOSE_PROP)
			{
				ASSERT(it->second.conditionVec[i].item.prop.pHasProp != NULL);
				delete[] it->second.conditionVec[i].item.prop.pHasProp;
			}
		}
	}
	m_equipComposeMap.clear();
}

BOOL EquipComposeCfg::Load(const char* pszFile)
{
	Json::Value rootValue;
	if (!OpenCfgFile(pszFile, rootValue))
		return false;

	if (!rootValue.isMember("Compound") || !rootValue["Compound"].isArray())
		return false;

	int n = rootValue["Compound"].size();
	if (n <= 0)
		return false;

	SEquipComposeItem composeItem;
	for (int i = 0; i < n; ++i)
	{
		UINT16 id = (UINT16)rootValue["Compound"][i]["ID"].asUInt();
		std::map<UINT16, SEquipComposeItem>::iterator it = m_equipComposeMap.find(id);
		if (it != m_equipComposeMap.end())
		{
			;
#if defined(WIN32)
			throw(id);
#endif
			ASSERT(false);
			continue;
		}
		m_equipComposeMap.insert(std::make_pair(id, composeItem));
		SEquipComposeItem& composeItemRef = m_equipComposeMap[id];

		composeItemRef.chance = (BYTE)rootValue["Compound"][i]["Probability"].asUInt();
		composeItemRef.consumeGold = rootValue["Compound"][i]["Golds"].asUInt();

		int si = rootValue["Compound"][i]["CompoundArticle"].size();
		for (int j = 0; j < si; ++j)
		{
			SDropPropCfgItem propItem;
			propItem.propId = rootValue["Compound"][i]["CompoundArticle"][j]["ID"].asUInt();
			propItem.type = rootValue["Compound"][i]["CompoundArticle"][j]["Type"].asUInt();
			propItem.num = rootValue["Compound"][i]["CompoundArticle"][j]["Num"].asUInt();
			composeItemRef.compsoePropVec.push_back(propItem);
		}

		si = rootValue["Compound"][i]["BaseArticle"].size();
		for (int j = 0; j < si; ++j)
		{
			SDropPropCfgItem propItem;
			propItem.propId = rootValue["Compound"][i]["BaseArticle"][j]["ID"].asUInt();
			propItem.type = rootValue["Compound"][i]["BaseArticle"][j]["Type"].asUInt();
			propItem.num = rootValue["Compound"][i]["BaseArticle"][j]["Num"].asUInt();
			composeItemRef.basicConsumePropVec.push_back(propItem);
		}

		si = rootValue["Compound"][i]["AdditionArticle"].size();
		for (int j = 0; j < si; ++j)
		{
			SDropPropCfgItem propItem;
			propItem.propId = rootValue["Compound"][i]["AdditionArticle"][j]["ID"].asUInt();
			propItem.type = rootValue["Compound"][i]["AdditionArticle"][j]["Type"].asUInt();
			propItem.num = rootValue["Compound"][i]["AdditionArticle"][j]["Num"].asUInt();
			composeItemRef.additionalConsumePropVec.push_back(propItem);
		}

		if (!rootValue["Compound"][i].isMember("Condition") || !rootValue["Compound"][i]["Condition"].isArray())
			continue;

		si = rootValue["Compound"][i]["Condition"].size();
		for (int j = 0; j < si; ++j)
		{
			int idx = 0;
			SComposeConditionItem condItem;
			condItem.type = rootValue["Compound"][i]["Condition"][j]["Type"].asUInt();
			int condNum = rootValue["Compound"][i]["Condition"][j]["Value"].size();
			if (condItem.type == ESGS_EQUIP_COMPOSE_ROLE_LEVEL)
			{
				ASSERT(condNum >= 2);
				condItem.item.lv.minLv = rootValue["Compound"][i]["Condition"][j]["Value"][idx++].asUInt();
				condItem.item.lv.maxLv = rootValue["Compound"][i]["Condition"][j]["Value"][idx++].asUInt();
			}
			else if (condItem.type == ESGS_EQUIP_COMPOSE_DATE_TIME)
			{
				ASSERT(condNum >= 2);
				condItem.item.tm.start = StrToMsel(rootValue["Compound"][i]["Condition"][j]["Value"][idx++].asCString());
				condItem.item.tm.end = StrToMsel(rootValue["Compound"][i]["Condition"][j]["Value"][idx++].asCString());
			}
			else if (condItem.type == ESGS_EQUIP_COMPOSE_MAGIC)
			{
				ASSERT(condNum >= 1);

				condItem.item.magic.magicId = rootValue["Compound"][i]["Condition"][j]["Value"][idx++].asUInt();
			}
			else if (condItem.type == ESGS_EQUIP_COMPOSE_PROP)
			{
				condItem.item.prop.num = condNum;
				condItem.item.prop.pHasProp = new SDropPropCfgItem[condNum];
				ASSERT(condItem.item.prop.pHasProp != NULL);

				for (; idx < condNum; ++idx)
				{
					condItem.item.prop.pHasProp[idx].type = rootValue["Compound"][i]["Condition"][j]["Value"][idx]["Type"].asUInt();
					condItem.item.prop.pHasProp[idx].propId = rootValue["Compound"][i]["Condition"][j]["Value"][idx]["ID"].asUInt();
					condItem.item.prop.pHasProp[idx].num = rootValue["Compound"][i]["Condition"][j]["Value"][idx]["Num"].asUInt();
				}
			}
			else
			{

#if defined(WIN32)
				throw(id);
#endif
				ASSERT(false);
			}
		}
	}
	return true;
}

int EquipComposeCfg::GetEquipComposeItem(UINT16 composeId, SEquipComposeItem*& pComposeItem)
{
	std::map<UINT16, SEquipComposeItem>::iterator it = m_equipComposeMap.find(composeId);
	if (it == m_equipComposeMap.end())
		return MLS_NOT_EXIST_COMPOSE_ID;

	pComposeItem = &it->second;
	return MLS_OK;
}