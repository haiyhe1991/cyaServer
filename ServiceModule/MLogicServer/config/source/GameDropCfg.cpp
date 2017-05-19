#include "GameDropCfg.h"

#define DROP_CHANCE_PERCENT	1000		//µÙ¬‰±»

GameDropCfg::GameDropCfg()
{

}

GameDropCfg::~GameDropCfg()
{

}

BOOL GameDropCfg::Load(const char* pszFile)
{
	Json::Value rootValue;
	if (!OpenCfgFile(pszFile, rootValue))
		return false;

	if (!rootValue.isMember("GameDrop") || !rootValue["GameDrop"].isArray())
		return false;

	int si = (int)rootValue["GameDrop"].size();
	if (si <= 0)
		return false;

	std::vector<SGameDropItem> drops;
	for (int i = 0; i < si; ++i)
	{
		UINT32 dropId = rootValue["GameDrop"][i]["DropID"].asUInt();
		std::map<UINT32/*µÙ¬‰id*/, std::vector<SGameDropItem> >::iterator it = m_gameDropMap.find(dropId);
		if (it != m_gameDropMap.end())
		{
			ASSERT(false);
			continue;
		}
		m_gameDropMap.insert(std::make_pair(dropId, drops));
		std::vector<SGameDropItem>& dropsRef = m_gameDropMap[dropId];

		UINT16 chanceVal = 0;
		int n = rootValue["GameDrop"][i]["Items"].size();
		for (int j = 0; j < n; ++j)
		{
			SGameDropItem item;
			item.monster = (BYTE)rootValue["GameDrop"][i]["Items"][j]["LV"].asUInt();
			item.num = rootValue["GameDrop"][i]["Items"][j]["Num"].asUInt();
			item.propId = (UINT16)rootValue["GameDrop"][i]["Items"][j]["ID"].asUInt();
			item.type = (BYTE)rootValue["GameDrop"][i]["Items"][j]["Type"].asUInt();
			item.repeatDrop = (BYTE)rootValue["GameDrop"][i]["Items"][j]["IsReGet"].asUInt();
			UINT16 nChance = rootValue["GameDrop"][i]["Items"][j]["Chance"].asUInt();
			if (nChance == 0)
			{
				ASSERT(false);
				continue;
			}
			chanceVal += nChance;
			ASSERT(chanceVal <= DROP_CHANCE_PERCENT);
			item.chanceVal = chanceVal;
			dropsRef.push_back(item);
		}
	}

	return true;
}

void GameDropCfg::GenerateDrops(UINT32 dropId, BYTE dropNum, std::vector<SDropPropCfgItem>& dropVec)
{
	std::map<UINT32/*µÙ¬‰id*/, std::vector<SGameDropItem> >::iterator it = m_gameDropMap.find(dropId);
	if (it == m_gameDropMap.end())
		return;

	std::vector<SGameDropItem>& dropItems = it->second;
	int si = (int)dropItems.size();
	if (si <= 0)
		return;

	for (BYTE i = 0; i < dropNum; ++i)
	{
		UINT16 randVal = rand() % DROP_CHANCE_PERCENT + 1;
		for (int j = 0; j < si; ++j)
		{
			if (randVal > dropItems[j].chanceVal)
				continue;

			if (!dropItems[j].repeatDrop)	//≤ªƒ‹÷ÿ∏¥µÙ¬‰
			{
				BOOL bExist = false;
				BYTE n = (BYTE)dropVec.size();
				for (BYTE k = 0; k < n; ++k)
				{
					if (dropVec[k].propId == dropItems[j].propId)
					{
						bExist = true;
						break;
					}
				}
				if (bExist)
					break;
			}

			SDropPropCfgItem item;
			item.num = dropItems[j].num;
			item.propId = dropItems[j].propId;
			item.type = dropItems[j].type;

			if (item.type == ESGS_PROP_DROP_ID)	//«∂Ã◊¡ÀµÙ¬‰id(µ›πÈ)
				GenerateDrops(item.propId, item.num, dropVec);
			else
				dropVec.push_back(item);

			break;
		}
	}
}

void GameDropCfg::GenerateAreaMonsterDrops(UINT32 dropId, BYTE dropNum, std::vector<SDropPropCfgItem>& monster, std::vector<SDropPropCfgItem>& cream, std::vector<SDropPropCfgItem>& boss)
{
	std::map<UINT32/*µÙ¬‰id*/, std::vector<SGameDropItem> >::iterator it = m_gameDropMap.find(dropId);
	if (it == m_gameDropMap.end())
		return;

	std::vector<SGameDropItem>& dropItems = it->second;
	int si = (int)dropItems.size();
	if (si <= 0)
		return;

	for (BYTE i = 0; i < dropNum; ++i)
	{
		int randVal = rand() % DROP_CHANCE_PERCENT + 1;
		for (int j = 0; j < si; ++j)
		{
			if (randVal > dropItems[j].chanceVal)
				continue;

			if (!dropItems[j].repeatDrop)	//≤ªƒ‹÷ÿ∏¥µÙ¬‰
			{
				BOOL bExist = false;
				BYTE n = (BYTE)monster.size();
				for (BYTE k = 0; k < n; k++)
				{
					if (monster[k].propId == dropItems[j].propId)
					{
						bExist = true;
						break;
					}
				}
				if (bExist)
					break;

				bExist = false;
				n = (BYTE)cream.size();
				for (BYTE k = 0; k < n; k++)
				{
					if (cream[k].propId == dropItems[j].propId)
					{
						bExist = true;
						break;
					}
				}
				if (bExist)
					break;

				bExist = false;
				n = (BYTE)boss.size();
				for (BYTE k = 0; k < n; k++)
				{
					if (boss[k].propId == dropItems[j].propId)
					{
						bExist = true;
						break;
					}
				}
				if (bExist)
					break;
			}

			SDropPropCfgItem item;
			item.num = dropItems[j].num;
			item.propId = dropItems[j].propId;
			item.type = dropItems[j].type;

			if (item.type == ESGS_PROP_DROP_ID)	//«∂Ã◊¡ÀµÙ¬‰id(µ›πÈ)
			{
				GenerateAreaMonsterDrops(item.propId, item.num, monster, cream, boss);
			}
			else
			{
				if (dropItems[j].monster == ESGS_MONSTER_CREAM)
					cream.push_back(item);
				else if (dropItems[j].monster == ESGS_MONSTER_SMALL_BOSS || dropItems[j].monster == ESGS_MONSTER_BIG_BOSS)
					boss.push_back(item);
				else
					monster.push_back(item);
			}

			break;
		}
	}
}

//’πø™µÙ¬‰◊È
void GameDropCfg::SpreadDropGroups(UINT32 dropId, BYTE dropNum, std::vector<SDropPropCfgItem> *ret)
{
	std::map<UINT32/*µÙ¬‰id*/, std::vector<SGameDropItem> >::iterator it = m_gameDropMap.find(dropId);
	if (it == m_gameDropMap.end())
		return;

	std::vector<SGameDropItem>& dropItems = it->second;
	int si = (int)dropItems.size();
	if (si <= 0)
		return;

	for (BYTE i = 0; i < dropNum; ++i)
	{
		int randVal = rand() % DROP_CHANCE_PERCENT + 1;
		for (int j = 0; j < si; ++j)
		{
			if (randVal > dropItems[j].chanceVal)
				continue;

			if (!dropItems[j].repeatDrop)	//≤ªƒ‹÷ÿ∏¥µÙ¬‰
			{
				BOOL bExist = false;
				BYTE n = (BYTE)ret->size();
				for (BYTE k = 0; k < n; k++)
				{
					if ((*ret)[k].propId == dropItems[j].propId)
					{
						bExist = true;
						break;
					}
				}
				if (bExist)
					break;
			}

			SDropPropCfgItem item;
			item.num = dropItems[j].num;
			item.propId = dropItems[j].propId;
			item.type = dropItems[j].type;
			if (item.type == ESGS_PROP_DROP_ID)	//«∂Ã◊¡ÀµÙ¬‰id(µ›πÈ)
			{
				SpreadDropGroups(item.propId, item.num, ret);
			}
			else
			{
				ret->push_back(item);
			}
			break;
		}
	}
}