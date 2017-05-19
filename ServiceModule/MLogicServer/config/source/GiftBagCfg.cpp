#include "GiftBagCfg.h"
#include "ServiceErrorCode.h"

GiftBagCfg::GiftBagCfg()
{

}

GiftBagCfg::~GiftBagCfg()
{

}

BOOL GiftBagCfg::Load(const char* filename, BOOL loadInMem/* = false*/)
{
	m_filename = filename;
	m_loadInMem = loadInMem;
	if (!loadInMem)
		return true;

	Json::Value rootValue;
	if (!OpenCfgFile(filename, rootValue))
		return false;

	int si = rootValue["GiftBag"].size();
	for (int i = 0; i < si; ++i)
	{
		SGiftBagItem item;
		UINT16 id = rootValue["GiftBag"][i]["ID"].asUInt();
		std::map<UINT16, SGiftBagItem>::iterator it = m_giftBagMap.find(id);
		if (it != m_giftBagMap.end())
		{
			ASSERT(false);
			continue;
		}
		//add by hxw 20151222
		strcpy(item.name, rootValue["GiftBag"][i]["Name"].asCString());
		strcpy(item.desc, rootValue["GiftBag"][i]["Desc"].asCString());
		//end
		m_giftBagMap.insert(std::make_pair(id, item));
		SGiftBagItem& itemRef = m_giftBagMap[id];
		itemRef.exp = rootValue["GiftBag"][i]["EXP"].asUInt();
		itemRef.gold = rootValue["GiftBag"][i]["Golds"].asUInt();
		itemRef.rpcoin = rootValue["GiftBag"][i]["token"].asUInt();
		itemRef.vit = 0;
		int n = rootValue["GiftBag"][i]["Items"].size();
		for (int j = 0; j < n; ++j)
		{
			SDropPropCfgItem prop;
			prop.propId = rootValue["GiftBag"][i]["Items"][j]["ID"].asUInt();
			prop.num = rootValue["GiftBag"][i]["Items"][j]["NUM"].asUInt();
			prop.type = rootValue["GiftBag"][i]["Items"][j]["TYPE"].asUInt();
			itemRef.propVec.push_back(prop);
		}
	}

	return true;
}

int GiftBagCfg::GetGiftBagItem(UINT16 id, SGiftBagItem& item)
{
	if (m_loadInMem)
	{
		std::map<UINT16, SGiftBagItem>::iterator it = m_giftBagMap.find(id);
		if (it == m_giftBagMap.end())
			return MLS_INVALID_GIFTBAG_ID;

		item.exp = it->second.exp;
		item.gold = it->second.gold;
		item.rpcoin = it->second.rpcoin;
		item.vit = it->second.vit;
		item.propVec.assign(it->second.propVec.begin(), it->second.propVec.end());

		//2016.01.12 zpy 新增
		strcpy(item.name, it->second.name);
		strcpy(item.desc, it->second.desc);
	}
	else
	{
		Json::Value rootValue;
		if (!OpenCfgFile(m_filename.c_str(), rootValue))
			return false;

		int si = rootValue["GiftBag"].size();
		for (int i = 0; i < si; ++i)
		{
			UINT16 giftBagId = rootValue["GiftBag"][i]["ID"].asUInt();
			if (id != giftBagId)
				continue;

			//2016.01.12 zpy 新增
			strcpy(item.name, rootValue["GiftBag"][i]["Name"].asCString());
			strcpy(item.desc, rootValue["GiftBag"][i]["Desc"].asCString());

			item.exp = rootValue["GiftBag"][i]["EXP"].asUInt();
			item.gold = rootValue["GiftBag"][i]["Golds"].asUInt();
			item.rpcoin = rootValue["GiftBag"][i]["token"].asUInt();
			item.vit = 0;
			int n = rootValue["GiftBag"][i]["Items"].size();
			for (int j = 0; j < n; ++j)
			{
				SDropPropCfgItem prop;
				prop.propId = rootValue["GiftBag"][i]["Items"][j]["ID"].asUInt();
				prop.num = rootValue["GiftBag"][i]["Items"][j]["NUM"].asUInt();
				prop.type = rootValue["GiftBag"][i]["Items"][j]["TYPE"].asUInt();
				item.propVec.push_back(prop);
			}
			break;
		}
	}

	return MLS_OK;
}