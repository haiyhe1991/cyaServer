#include "InstCfg.h"
#include "ServiceErrorCode.h"
#include "ServiceProtocol.h"
#include "GameDropCfg.h"
#include "cyaMaxMin.h"
#include "MonsterCfg.h"

InstCfg::InstCfg(GameDropCfg* pDropCfg, MonsterCfg* pMonsterCfg)
: m_gameDropCfg(pDropCfg)
, m_pMonsterCfg(pMonsterCfg)
{

}

InstCfg::~InstCfg()
{

}

void InstCfg::SetParamValue(UINT32 goldBranchThrehold, BYTE goldBranchMaxPercent, BYTE dropLv1ToBossPercent, BYTE dropLv1ToCreamPercent)
{
	m_goldBranchThrehold = goldBranchThrehold;
	m_goldBranchMaxPercent = goldBranchMaxPercent;
	m_dropLv1ToBossPercent = dropLv1ToBossPercent;
	m_dropLv1ToCreamPercent = dropLv1ToCreamPercent;
}

BOOL InstCfg::LoadInst(const char* filename)
{
	Json::Value rootValue;
	if (!OpenCfgFile(filename, rootValue))
		return false;

	if (!rootValue.isMember("Stage") || !rootValue["Stage"].isArray())
		return false;

	int si = rootValue["Stage"].size();
	if (si <= 0)
		return false;

	SInstAttrCfg inst;
	for (int i = 0; i < si; ++i)
	{
		//基本信息
		SInstAttrCfg* pInstCfg = NULL;
		UINT16 instId = rootValue["Stage"][i]["ID"].asUInt();
		BYTE   instType = rootValue["Stage"][i]["Type"].asUInt();
		UINT16 enterNum = rootValue["Stage"][i]["EnterNum"].asUInt();

		if (/*zpy 注释 instType == ESGS_INST_PERSONAL_RAND*/false)	//随机副本
		{
			std::map<UINT16/*关卡id*/, SInstAttrCfg>::iterator it = m_randInstCfg.find(instId);
			if (it != m_randInstCfg.end())
			{

#if defined(WIN32)
				throw(new ExConfig(instId, "副本ID已经存在"));
#endif
				ASSERT(false);
				continue;
			}
			m_randInstCfg.insert(std::make_pair(instId, inst));
			pInstCfg = &m_randInstCfg[instId];
		}
		else
		{
			std::map<UINT16/*关卡id*/, SInstAttrCfg>::iterator it = m_instCfg.find(instId);
			if (it != m_instCfg.end())
			{

#if defined(WIN32)
				throw(new ExConfig(instId, "副本ID已经存在"));
#endif
				ASSERT(false);
				continue;
			}
			m_instCfg.insert(std::make_pair(instId, inst));
			pInstCfg = &m_instCfg[instId];

			/* zpy 新增 */
			m_instByType[instType].insert(instId);
		}

#if defined(WIN32)	
		std::string strsign;
#define STR_SIGN(parm)	strsign = #parm;
#endif

		try{

#if defined(WIN32)	
			STR_SIGN(Name);
#endif
			pInstCfg->type = instType;
			pInstCfg->enterNum = enterNum;
			pInstCfg->name = rootValue["Stage"][i]["Name"].asCString();

#if defined(WIN32)	
			STR_SIGN(Vit);
#endif
			pInstCfg->spendVit = rootValue["Stage"][i]["Vit"].asUInt();

#if defined(WIN32)	
			STR_SIGN(LvLimit);
#endif
			pInstCfg->limitRoleLevel = rootValue["Stage"][i]["LvLimit"].asUInt();

#if defined(WIN32)	
			STR_SIGN(Degree);
#endif
			pInstCfg->degree = rootValue["Stage"][i]["Degree"].asDouble();

#if defined(WIN32)	
			STR_SIGN(ReviveID);
#endif
			pInstCfg->reviveId = rootValue["Stage"][i]["ReviveID"].asUInt();

#if defined(WIN32)	
			STR_SIGN(FrontID);
#endif
			pInstCfg->frontInstId = rootValue["Stage"][i]["FrontID"].asUInt();

#if defined(WIN32)	
			STR_SIGN(ChapterID);
#endif
			pInstCfg->chapterId = rootValue["Stage"][i]["ChapterID"].asUInt();

#if defined(WIN32)	
			STR_SIGN(EXP);
#endif
			pInstCfg->sweepExp = rootValue["Stage"][i]["EXP"].asUInt();

		}
		catch (...)
		{
#if defined(WIN32)
			std::string str = "出错字段";
			str += strsign;
			throw(new ExConfig(instId, str.c_str()));
#endif
		}

		//破碎物品
		try{
			pInstCfg->brokenDrops.dropId = rootValue["Stage"][i]["BrokenObjects"]["DropID"].asUInt();
			pInstCfg->brokenDrops.gold = rootValue["Stage"][i]["BrokenObjects"]["Gold"].asUInt();
			pInstCfg->brokenDrops.dropNum = (BYTE)rootValue["Stage"][i]["BrokenObjects"]["MaxDrop"].asUInt();
		}
		catch (...)
		{
			throw(new ExConfig(instId, "破碎物错误"));
		}

		//宝箱
		try
		{
			if (rootValue["Stage"][i].isMember("ChestBoxs") && rootValue["Stage"][i]["ChestBoxs"].isArray())
			{
				SStorageAreaBox box;
				int boxNum = rootValue["Stage"][i]["ChestBoxs"].size();
				for (int j = 0; j < boxNum; ++j)
				{
					BYTE boxId = rootValue["Stage"][i]["ChestBoxs"][j]["ID"].asUInt();
					pInstCfg->boxDrops.insert(std::make_pair(boxId, box));
					SStorageAreaBox& boxRef = pInstCfg->boxDrops[boxId];

					boxRef.dropId = rootValue["Stage"][i]["ChestBoxs"][j]["DropID"].asUInt();
					boxRef.dropNum = (BYTE)rootValue["Stage"][i]["ChestBoxs"][j]["MaxDrop"].asUInt();
					boxRef.gold = rootValue["Stage"][i]["ChestBoxs"][j]["Gold"].asUInt();
				}
			}
		}
		catch (...)
		{
			throw(new ExConfig(instId, "宝箱配置错误"));
		}

		//战斗区域怪
		try
		{
			if (rootValue["Stage"][i].isMember("BattleArea") && rootValue["Stage"][i]["BattleArea"].isArray())
			{
				SBattleAreaDrop battleDrop;
				int battleAreaNum = rootValue["Stage"][i]["BattleArea"].size();
				for (int j = 0; j < battleAreaNum; ++j)
				{
					BYTE battleAreaId = (BYTE)rootValue["Stage"][i]["BattleArea"][j]["ID"].asUInt();
					std::map<BYTE/*战斗区域id*/, SBattleAreaDrop>::iterator it2 = pInstCfg->battleArea.find(battleAreaId);
					if (it2 != pInstCfg->battleArea.end())
					{

#if defined(WIN32)
						char cc[128] = { 0 };
						sprintf(cc, "战斗区域ID=%d重复配置", (int)battleAreaId);
						throw(new ExConfig(instId, cc));
#endif
						ASSERT(false);
						continue;
					}
					pInstCfg->battleArea.insert(std::make_pair(battleAreaId, battleDrop));
					SBattleAreaDrop& battleDropRef = pInstCfg->battleArea[battleAreaId];

					int battleMonsterArrNum = rootValue["Stage"][i]["BattleArea"][j]["Battle"].size();
					for (int k = 0; k < battleMonsterArrNum; ++k)
					{
						BYTE appearTimes = (BYTE)rootValue["Stage"][i]["BattleArea"][j]["Battle"][k]["Trun"].asUInt();
						int monsterNum = rootValue["Stage"][i]["BattleArea"][j]["Battle"][k]["Monster"].size();
						for (int n = 0; n < monsterNum; ++n)
						{
							BYTE type = (BYTE)rootValue["Stage"][i]["BattleArea"][j]["Battle"][k]["Monster"][n]["Type"].asUInt();
							UINT16 monserId = (UINT16)rootValue["Stage"][i]["BattleArea"][j]["Battle"][k]["Monster"][n]["ID"].asUInt();
							BYTE monsterLv = (BYTE)rootValue["Stage"][i]["BattleArea"][j]["Battle"][k]["Monster"][n]["Lv"].asUInt();
							UINT16 monsterNum = (UINT16)rootValue["Stage"][i]["BattleArea"][j]["Battle"][k]["Monster"][n]["Num"].asUInt();
							monsterNum = monsterNum * appearTimes;

							if (type == ESGS_MONSTER_CREAM)
								battleDropRef.hasCream = 1;
							else if (type == ESGS_MONSTER_SMALL_BOSS || type == ESGS_MONSTER_BIG_BOSS)
								battleDropRef.hasBoss = 1;
							else
								battleDropRef.hasMonster = 1;

							std::map<UINT16/*怪id*/, SBrushMonster>::iterator it = battleDropRef.brushMonster.find(monserId);
							if (it == battleDropRef.brushMonster.end())
							{
								SBrushMonster monster;
								battleDropRef.brushMonster.insert(std::make_pair(monserId, monster));

								SBrushMonster& monsterRef = battleDropRef.brushMonster[monserId];
								monsterRef.lvMonsterMap.insert(std::make_pair(monsterLv, monsterNum));
							}
							else
							{
								SBrushMonster& monsterRef = battleDropRef.brushMonster[monserId];
								std::map<BYTE/*怪等级*/, UINT16/*数量*/>::iterator it = monsterRef.lvMonsterMap.find(monsterLv);
								if (it == monsterRef.lvMonsterMap.end())
									monsterRef.lvMonsterMap.insert(std::make_pair(monsterLv, monsterNum));
								else
									it->second = SGSU16Add(it->second, monsterNum);
							}
						}
					}
				}
			}
		}
		catch (...)
		{
			throw(new ExConfig(instId, "战斗区域配置错误"));
		}
		//战斗区域掉落
		try
		{
			if (rootValue["Stage"][i].isMember("DropArea") && rootValue["Stage"][i]["DropArea"].isArray())
			{
				if (!rootValue["Stage"][i]["DropArea"].isNull())
				{
					int battleAreaDropNum = rootValue["Stage"][i]["DropArea"].size();
					for (int j = 0; j < battleAreaDropNum; ++j)
					{
						BYTE battleAreaId = (BYTE)rootValue["Stage"][i]["DropArea"][j]["ID"].asUInt();
						std::map<BYTE/*战斗区域id*/, SBattleAreaDrop>::iterator it = pInstCfg->battleArea.find(battleAreaId);
						if (it == pInstCfg->battleArea.end())
						{
#if defined(WIN32)
							char cc[128] = { 0 };
							sprintf(cc, "区域掉落ID=%d重复配置", battleAreaId);
							throw(new ExConfig(instId, cc));
#endif
							continue;
						}
						it->second.dropId = rootValue["Stage"][i]["DropArea"][j]["DropID"].asUInt();
						it->second.dropNum = rootValue["Stage"][i]["DropArea"][j]["MaxDrop"].asUInt();
						it->second.gold = rootValue["Stage"][i]["DropArea"][j]["Gold"].asUInt();
					}
				}

			}
		}
		catch (...)
		{
			throw(new ExConfig(instId, "战斗区域掉落配置错误"));
		}
	}

	// 	SInstDropProp dropProps;
	// 	BYTE monsterDrops = 0;
	// 	BYTE creamDrops = 0;
	// 	BYTE bossDrops = 0;
	// 	GenerateAreaMonsterDrops(1101, 7, dropProps, monsterDrops, creamDrops, bossDrops);
	return true;
}

BOOL InstCfg::LoadRandInst(const char* filename)
{
	return true;
}

int InstCfg::GetInstSpendVit(UINT16 instId, UINT32& spendVit)
{
	std::map<UINT16/*关卡id*/, SInstAttrCfg>::iterator it = m_instCfg.find(instId);
	if (it == m_instCfg.end())
	{
		it = m_randInstCfg.find(instId);
		if (it == m_randInstCfg.end())
			return MLS_NOT_EXIST_INST;
	}

	spendVit = it->second.spendVit;
	return MLS_OK;
}

int InstCfg::GetInstLvLimit(UINT16 instId, BYTE& lvLimit)
{
	std::map<UINT16/*关卡id*/, SInstAttrCfg>::iterator it = m_instCfg.find(instId);
	if (it == m_instCfg.end())
	{
		it = m_randInstCfg.find(instId);
		if (it == m_randInstCfg.end())
			return MLS_NOT_EXIST_INST;
	}

	lvLimit = it->second.limitRoleLevel;
	return MLS_OK;
}

//获取副本配置
int InstCfg::GetInstConfig(UINT16 instId, SInstAttrCfg *&config)
{
	config = NULL;
	std::map<UINT16/*关卡id*/, SInstAttrCfg>::iterator it = m_instCfg.find(instId);
	if (it == m_instCfg.end())
	{
		it = m_randInstCfg.find(instId);
		if (it == m_randInstCfg.end())
			return MLS_NOT_EXIST_INST;
	}

	config = &it->second;
	return MLS_OK;
}

int InstCfg::GetInstBasicInfo(UINT16 instId, BYTE&instType, BYTE& lvLimit, UINT32& spendVit)
{
	std::map<UINT16/*关卡id*/, SInstAttrCfg>::iterator it = m_instCfg.find(instId);
	if (it == m_instCfg.end())
	{
		it = m_randInstCfg.find(instId);
		if (it == m_randInstCfg.end())
			return MLS_NOT_EXIST_INST;
	}

	lvLimit = it->second.limitRoleLevel;
	instType = it->second.type;
	spendVit = it->second.spendVit;
	return MLS_OK;
}

int InstCfg::GetInstName(UINT16 instId, std::string& instName)
{
	std::map<UINT16/*关卡id*/, SInstAttrCfg>::iterator it = m_instCfg.find(instId);
	if (it == m_instCfg.end())
	{
		it = m_randInstCfg.find(instId);
		if (it == m_randInstCfg.end())
			return MLS_NOT_EXIST_INST;
	}

	instName = it->second.name.c_str();
	return MLS_OK;
}

UINT16 InstCfg::GenerateRandInstId()
{
	UINT16 id = 0;
	if (m_randInstCfg.empty())
		return id;

	id = m_randInstCfg.begin()->first;
	return id;
}

int InstCfg::GenerateDropBox(UINT16 instId, SInstDropBox& dropBox)
{
	std::map<UINT16/*关卡id*/, SInstAttrCfg>::iterator it = m_instCfg.find(instId);
	if (it == m_instCfg.end())
	{
		it = m_randInstCfg.find(instId);
		if (it == m_randInstCfg.end())
			return MLS_NOT_EXIST_INST;
	}

	std::map<BYTE/*宝箱id*/, SStorageAreaBox>& boxDropsCfg = it->second.boxDrops;
	std::map<BYTE/*宝箱id*/, SStorageAreaBox>::iterator it2 = boxDropsCfg.begin();
	for (; it2 != boxDropsCfg.end(); ++it2)
	{
		BYTE boxId = it2->first;
		std::vector<SDropPropCfgItem> vec;
		dropBox.boxMap.insert(std::make_pair(boxId, vec));

		m_gameDropCfg->GenerateDrops(it2->second.dropId, it2->second.dropNum, dropBox.boxMap[boxId]);
		GoldBranch(it2->second.gold, dropBox.boxMap[boxId]);
	}

	return MLS_OK;
}

int InstCfg::GenerateAreaMonsterDrops(UINT16 instId, BYTE areaId, SInstDropProp& dropProps, BYTE& monsterDrops, BYTE& creamDrops, BYTE& bossDrops)
{
	creamDrops = 0;
	bossDrops = 0;
	monsterDrops = 0;

	std::map<UINT16/*关卡id*/, SInstAttrCfg>::iterator it = m_instCfg.find(instId);
	if (it == m_instCfg.end())
	{
		it = m_randInstCfg.find(instId);
		if (it == m_randInstCfg.end())
			return MLS_NOT_EXIST_INST;
	}

	std::map<BYTE/*战斗区域id*/, SBattleAreaDrop>& battleAreaCfg = it->second.battleArea;	//战斗区域掉落
	std::map<BYTE/*战斗区域id*/, SBattleAreaDrop>::iterator it2 = battleAreaCfg.find(areaId);
	if (it2 == battleAreaCfg.end())
		return MLS_BATTLE_AREA_NOT_EXIST;

	SBattleAreaDrop& areaDrop = it2->second;
	if (!areaDrop.hasBoss && !areaDrop.hasCream && !areaDrop.hasMonster)
		return MLS_OK;

	std::vector<SDropPropCfgItem> monster;
	std::vector<SDropPropCfgItem> cream;
	std::vector<SDropPropCfgItem> boss;
	std::vector<SDropPropCfgItem> gold;
	m_gameDropCfg->GenerateAreaMonsterDrops(areaDrop.dropId, areaDrop.dropNum, monster, cream, boss);
	GoldBranch(areaDrop.gold, gold);
	//GoldBranch(areaDrop.gold, monster);
	//std::random_shuffle(monster.begin(), monster.end());
	BYTE goldStack = (BYTE)gold.size();

#if 1
	//分配掉落
	if (!areaDrop.hasBoss && !areaDrop.hasCream)
	{
		for (BYTE i = 0; i < goldStack; ++i)
			monster.push_back(gold[i]);
	}
	else
	{
		BYTE idx = 0;
		UINT32 n = goldStack;
		BYTE allocBossNum = min(goldStack, (n * m_dropLv1ToBossPercent) / 100);
		BYTE allocCreamNum = min(SGSU8Sub(goldStack, allocBossNum), (n * m_dropLv1ToCreamPercent) / 100);
		BYTE allocMonster = goldStack - allocBossNum - allocCreamNum;

		//分小怪
		if (areaDrop.hasMonster)
		{
			while (allocMonster-- > 0)
				monster.push_back(gold[idx++]);
		}

		//分boss
		if (areaDrop.hasBoss)
		{
			while (allocBossNum-- > 0)
				boss.push_back(gold[idx++]);
		}

		//分精英
		if (areaDrop.hasCream)
		{
			while (allocCreamNum-- > 0)
				cream.push_back(gold[idx++]);
		}

		//未分完
		if (goldStack > idx)
		{
			if (areaDrop.hasBoss)
			{
				for (; idx < goldStack; ++idx)
					boss.push_back(gold[idx]);
			}

			if (areaDrop.hasCream)
			{
				for (; idx < goldStack; ++idx)
					cream.push_back(gold[idx]);
			}
		}
	}
	monsterDrops = (BYTE)monster.size();
	creamDrops = (BYTE)cream.size();
	bossDrops = (BYTE)boss.size();

	if (monsterDrops > 0)
		dropProps.props.assign(monster.begin(), monster.end());

	for (BYTE i = 0; i < creamDrops; ++i)
		dropProps.props.push_back(cream[i]);

	for (BYTE i = 0; i < bossDrops; ++i)
		dropProps.props.push_back(boss[i]);
#else
	monsterDrops = (BYTE)monster.size();
	if (monsterDrops > 0)
		dropProps.props.assign(monster.begin(), monster.end());

	if (it2->second.hasBoss)
	{
		bossDrops = (BYTE)boss.size();
		for (BYTE i = 0; i < bossDrops; ++i)
			dropProps.props.push_back(boss[i]);
	}

	if (it2->second.hasCream)
	{
		creamDrops = (BYTE)cream.size();
		for (BYTE i = 0; i < creamDrops; ++i)
			dropProps.props.push_back(cream[i]);
	}
#endif
	return MLS_OK;
}

int InstCfg::GetBattleAreaMonster(UINT16 instId, BYTE areaId, SInstBrushMonster& brushMonster)
{
	std::map<UINT16/*关卡id*/, SInstAttrCfg>::iterator it = m_instCfg.find(instId);
	if (it == m_instCfg.end())
	{
		it = m_randInstCfg.find(instId);
		if (it == m_randInstCfg.end())
			return MLS_NOT_EXIST_INST;
	}

	std::map<BYTE/*战斗区域id*/, SBattleAreaDrop>& battleAreaCfg = it->second.battleArea;	//战斗区域掉落
	std::map<BYTE/*战斗区域id*/, SBattleAreaDrop>::iterator it2 = battleAreaCfg.find(areaId);
	if (it2 == battleAreaCfg.end())
		return MLS_BATTLE_AREA_NOT_EXIST;

	brushMonster.monsterMap.insert(it2->second.brushMonster.begin(), it2->second.brushMonster.end());
	return MLS_OK;
}

int InstCfg::GenerateBrokenDrops(UINT16 instId, SInstDropProp& brokenDrops)
{
	std::map<UINT16/*关卡id*/, SInstAttrCfg>::iterator it = m_instCfg.find(instId);
	if (it == m_instCfg.end())
	{
		it = m_randInstCfg.find(instId);
		if (it == m_randInstCfg.end())
			return MLS_NOT_EXIST_INST;
	}

	SBrokenPropDrop& brokenCfgDrops = it->second.brokenDrops;
	m_gameDropCfg->GenerateDrops(brokenCfgDrops.dropId, brokenCfgDrops.dropNum, brokenDrops.props);
	GoldBranch(brokenCfgDrops.gold, brokenDrops.props);

	return MLS_OK;
}

void InstCfg::GoldBranch(UINT32 gold, std::vector<SDropPropCfgItem>& vec)
{
	if (gold <= 0)
		return;

	if (gold <= m_goldBranchThrehold)
	{
		SDropPropCfgItem item;
		item.num = gold;
		item.propId = 0;
		item.type = ESGS_PROP_GOLD;
		vec.push_back(item);
	}
	else
	{
		UINT32 total = gold;
		UINT32 maxVal = max(1, ((UINT64)gold * m_goldBranchMaxPercent) / 100);
		UINT32 minVal = max(1, ((UINT64)gold * m_goldBranchMaxPercent) / 200);

		while (total > 0)
		{
			SDropPropCfgItem item;
			item.propId = 0;
			item.type = ESGS_PROP_GOLD;
			int val = rand() % maxVal + 1;
			item.num = max(minVal, val);
			ASSERT(item.num >= minVal);
			vec.push_back(item);
			total = SGSU32Sub(total, item.num);

			if (total > 0 && total <= minVal)
			{
				item.num = total;
				vec.push_back(item);
				break;
			}
		}
	}
}

UINT64 InstCfg::GetInstSweepExp(UINT16 instId)
{
	UINT64 exp = 0;
	std::map<UINT16/*关卡id*/, SInstAttrCfg>::iterator it = m_instCfg.find(instId);
	if (it == m_instCfg.end())
		return exp;

	exp = it->second.sweepExp;
	// 	SInstAttrCfg& inst = it->second;
	// 	std::map<BYTE/*战斗区域id*/, SBattleAreaDrop>::iterator it2 = inst.battleArea.begin();
	// 	for(; it2 != inst.battleArea.end(); ++it2)
	// 	{
	// 		std::map<UINT16/*怪id*/, SBrushMonster>& monster = it2->second.brushMonster;
	// 		std::map<UINT16/*怪id*/, SBrushMonster>::iterator it3 = monster.begin();
	// 		for(; it3 != monster.end(); ++it3)
	// 		{
	// 			UINT64 expVal = m_pMonsterCfg->GetMonsterExp(it3->first);
	// 			std::map<BYTE/*怪等级*/, UINT16/*数量*/>& lvMonster = it3->second.lvMonsterMap;
	// 			std::map<BYTE/*怪等级*/, UINT16/*数量*/>::iterator it4 = lvMonster.begin();
	// 			for(; it4 != lvMonster.end(); ++it4)
	// 				exp += expVal * it4->first * it4->second;
	// 		}
	// 	}

	return exp;
}

int InstCfg::GenerateInstSweepDrops(UINT16 instId, SInstDropProp& drops)
{
	std::map<UINT16/*关卡id*/, SInstAttrCfg>::iterator it = m_instCfg.find(instId);
	if (it == m_instCfg.end())
		return MLS_NOT_EXIST_INST;

	//破碎物
	// 	SInstDropProp brokens;
	// 	GenerateBrokenDrops(instId, brokens);
	// 	//宝箱
	// 	SInstDropBox chestBox;
	// 	GenerateDropBox(instId, chestBox);
	//怪
	std::vector<SDropPropCfgItem> monster;
	std::vector<SDropPropCfgItem> cream;
	std::vector<SDropPropCfgItem> boss;
	std::map<BYTE/*战斗区域id*/, SBattleAreaDrop>& battleAreaCfg = it->second.battleArea;	//战斗区域掉落
	std::map<BYTE/*战斗区域id*/, SBattleAreaDrop>::iterator it2 = battleAreaCfg.begin();
	for (; it2 != battleAreaCfg.end(); ++it2)
	{
		SBattleAreaDrop& areaDrop = it2->second;
		SDropPropCfgItem goldItem;
		goldItem.propId = 0;
		goldItem.num = areaDrop.gold;
		goldItem.type = ESGS_PROP_GOLD;
		monster.push_back(goldItem);
		m_gameDropCfg->GenerateAreaMonsterDrops(areaDrop.dropId, areaDrop.dropNum, monster, cream, boss);

		// 		std::vector<SDropPropCfgItem> creamVec;
		// 		std::vector<SDropPropCfgItem> bossVec;
		// 		m_gameDropCfg->GenerateAreaMonsterDrops(areaDrop.dropId, areaDrop.dropNum, monster, creamVec, bossVec);
		// 		if(areaDrop.hasBoss)
		// 		{
		// 			int n = (int)bossVec.size();
		// 			for(int i = 0; i < n; ++i)
		// 				boss.push_back(bossVec[i]);
		// 		}
		// 		if(areaDrop.hasCream)
		// 		{
		// 			int n = (int)creamVec.size();
		// 			for(int i = 0; i < n; ++i)
		// 				cream.push_back(creamVec[i]);
		// 		}
	}

	//物品归类
	// 	int si = (int)brokens.props.size();	//破碎物
	// 	for(int i = 0; i < si; ++i)
	// 	{
	// 		BOOL exist = false;
	// 		int n = (int)drops.props.size();
	// 		for(int j = 0; j < n; ++j)
	// 		{
	// 			if(drops.props[j].propId == brokens.props[i].propId)
	// 			{
	// 				drops.props[j].num = SGSU32Add(drops.props[j].num, brokens.props[i].num);
	// 				exist = true;
	// 				break;
	// 			}
	// 		}
	// 
	// 		if(!exist)
	// 			drops.props.push_back(brokens.props[i]);
	// 	}
	// 
	// 	std::map<BYTE/*宝箱id*/, std::vector<SDropPropCfgItem> >::iterator boxIt = chestBox.boxMap.begin();	//宝箱
	// 	for(; boxIt != chestBox.boxMap.end(); ++boxIt)
	// 	{
	// 		std::vector<SDropPropCfgItem>& boxPropVec = boxIt->second;
	// 		si = (int)boxPropVec.size();
	// 		for(int i = 0; i < si; ++i)
	// 		{
	// 			BOOL exist = false;
	// 			int n = (int)drops.props.size();
	// 			for(int j = 0; j < n; ++j)
	// 			{
	// 				if(drops.props[j].propId == boxPropVec[i].propId)
	// 				{
	// 					drops.props[j].num = SGSU32Add(drops.props[j].num, boxPropVec[i].num);
	// 					exist = true;
	// 					break;
	// 				}
	// 			}
	// 
	// 			if(!exist)
	// 				drops.props.push_back(boxPropVec[i]);
	// 		}
	// 	}

	int si = (int)monster.size();	//小怪
	for (int i = 0; i < si; ++i)
	{
		BOOL exist = false;
		int n = (int)drops.props.size();
		for (int j = 0; j < n; ++j)
		{
			if (drops.props[j].propId == monster[i].propId)
			{
				drops.props[j].num = SGSU32Add(drops.props[j].num, monster[i].num);
				exist = true;
				break;
			}
		}

		if (!exist)
			drops.props.push_back(monster[i]);
	}

	si = (int)cream.size();	//精英怪
	for (int i = 0; i < si; ++i)
	{
		BOOL exist = false;
		int n = (int)drops.props.size();
		for (int j = 0; j < n; ++j)
		{
			if (drops.props[j].propId == cream[i].propId)
			{
				drops.props[j].num = SGSU32Add(drops.props[j].num, cream[i].num);
				exist = true;
				break;
			}
		}

		if (!exist)
			drops.props.push_back(cream[i]);
	}

	si = (int)boss.size();	//boss怪
	for (int i = 0; i < si; ++i)
	{
		BOOL exist = false;
		int n = (int)drops.props.size();
		for (int j = 0; j < n; ++j)
		{
			if (drops.props[j].propId == boss[i].propId)
			{
				drops.props[j].num = SGSU32Add(drops.props[j].num, boss[i].num);
				exist = true;
				break;
			}
		}

		if (!exist)
			drops.props.push_back(boss[i]);
	}

	return MLS_OK;
}

int InstCfg::GetInstOwnerChapter(UINT16 instId, UINT16& chapterId)
{
	std::map<UINT16/*关卡id*/, SInstAttrCfg>::iterator it = m_instCfg.find(instId);
	if (it == m_instCfg.end())
		return MLS_NOT_EXIST_INST;

	chapterId = it->second.chapterId;
	return MLS_OK;
}

BYTE InstCfg::GetInstType(UINT16 instId)
{
	BYTE type = 0;
	std::map<UINT16/*关卡id*/, SInstAttrCfg>::iterator it = m_instCfg.find(instId);
	if (it != m_instCfg.end())
		type = it->second.type;
	else
	{
		it = m_randInstCfg.find(instId);
		if (it != m_randInstCfg.end())
			type = it->second.type;
	}

	return type;
}

UINT16 InstCfg::GetInstEnterNum(UINT16 instId)
{
	UINT16 enterNum = 0;
	std::map<UINT16/*关卡id*/, SInstAttrCfg>::iterator it = m_instCfg.find(instId);
	if (it != m_instCfg.end())
		enterNum = it->second.enterNum;
	else
	{
		it = m_randInstCfg.find(instId);
		if (it != m_randInstCfg.end())
			enterNum = it->second.enterNum;
	}

	return enterNum;
}

//通过类型获取副本id
bool InstCfg::GetInstByInstType(BYTE type, std::set<UINT16> *&ret)
{
	ret = NULL;
	std::map<BYTE, std::set<UINT16> >::iterator itr = m_instByType.find(type);
	if (itr == m_instByType.end())
		return false;
	ret = &itr->second;
	return true;
}

/* zpy 2015.12.22新增 展开掉落组 */
void InstCfg::SpreadDropGroups(UINT32 dropId, BYTE dropNum, std::vector<SDropPropCfgItem> *ret)
{
	m_gameDropCfg->SpreadDropGroups(dropId, dropNum, ret);
}