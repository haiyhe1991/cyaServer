#include "RouletteCfg.h"
#include "cyaTime.h"
#include "ServiceProtocol.h"
#include "cyaMaxMin.h"

#define SGS_ROULETTE_REWARD_CHANCE 1000

RouletteCfg::RouletteCfg()
{

}

RouletteCfg::~RouletteCfg()
{

}

BOOL RouletteCfg::Load(const char* filename)
{
	m_filename = filename;
	Json::Value rootValue;
	if (!OpenCfgFile(m_filename.c_str(), rootValue))
		return false;

	m_everydayGiveTimes = rootValue["Roulette"]["GiveNum"].asUInt();

	// 	char buf[4096] = { 0 };
	// 	std::vector<SRoulettePropCfgItem> saveItemVec;
	// 	SGetRoulettePropRes* p = (SGetRoulettePropRes*)buf;
	// 	GenerateRouletteReward(p, saveItemVec);
	// 	std::vector<SRoulettePropCfgItem> itemVec;
	// 	GenerateRouletteReward(itemVec);
	return true;
}

BYTE RouletteCfg::GetEverydayGiveTimes() const
{
	return m_everydayGiveTimes;
}

BOOL RouletteCfg::GenerateRouletteReward(SGetRoulettePropRes* pRoulettePropRes, std::vector<SRoulettePropCfgItem>& saveItemVec)
{
	Json::Value rootValue;
	if (!OpenCfgFile(m_filename.c_str(), rootValue))
		return false;

	m_everydayGiveTimes = rootValue["Roulette"]["GiveNum"].asUInt();
	BYTE maxNum = (BYTE)((MAX_RES_USER_BYTES - member_offset(SGetRoulettePropRes, num, data)) / sizeof(SRoulettePropItem));

	const char* startDate = rootValue["Roulette"]["StartTime"].asCString();
	const char* endDate = rootValue["Roulette"]["EndTime"].asCString();
	LTMSEL startMSel = StrToMsel(startDate);
	LTMSEL endMSel = StrToMsel(endDate);
	LTMSEL nowMSel = GetMsel();
	if (nowMSel < startMSel || nowMSel > endMSel)
		return false;

	//生成大奖
	BYTE idx = 0;
	UINT32 spendToken = 0;
	SRoulettePropCfgItem item;
	GenerateRouletteBigReward(item, spendToken);
	pRoulettePropRes->data[idx].id = item.id;
	pRoulettePropRes->data[idx].num = item.num;
	pRoulettePropRes->data[idx].propId = item.propId;
	pRoulettePropRes->data[idx].type = item.type;
	saveItemVec.push_back(item);
	++idx;

	int si = rootValue["Roulette"]["Rewards"].size();
	for (int i = 0; i < si; ++i)
	{
		pRoulettePropRes->data[idx].num = rootValue["Roulette"]["Rewards"][i]["Num"].asUInt();
		pRoulettePropRes->data[idx].propId = rootValue["Roulette"]["Rewards"][i]["ID"].asUInt();
		pRoulettePropRes->data[idx].type = rootValue["Roulette"]["Rewards"][i]["Type"].asUInt();
		pRoulettePropRes->data[idx].id = idx;

		item.id = pRoulettePropRes->data[idx].id;
		item.chance = rootValue["Roulette"]["Rewards"][i]["Chance"].asUInt();
		item.num = pRoulettePropRes->data[idx].num;
		item.propId = pRoulettePropRes->data[idx].propId;
		item.type = pRoulettePropRes->data[idx].type;
		saveItemVec.push_back(item);

		if (++idx >= maxNum)
			break;
	}
	pRoulettePropRes->num = idx;

	return true;
}

BOOL RouletteCfg::GenerateRouletteReward(std::vector<SRoulettePropCfgItem>& itemVec)
{
	Json::Value rootValue;
	if (!OpenCfgFile(m_filename.c_str(), rootValue))
		return false;

	m_everydayGiveTimes = rootValue["Roulette"]["GiveNum"].asUInt();
	const char* startDate = rootValue["Roulette"]["StartTime"].asCString();
	const char* endDate = rootValue["Roulette"]["EndTime"].asCString();
	LTMSEL startMSel = StrToMsel(startDate);
	LTMSEL endMSel = StrToMsel(endDate);
	LTMSEL nowMSel = GetMsel();
	if (nowMSel < startMSel || nowMSel > endMSel)
		return false;

	//生成大奖
	UINT32 spendToken = 0;
	SRoulettePropCfgItem item;
	GenerateRouletteBigReward(item, spendToken);
	itemVec.push_back(item);

	BYTE idx = 1;
	int si = rootValue["Roulette"]["Rewards"].size();
	for (int i = 0; i < si; ++i)
	{
		item.num = rootValue["Roulette"]["Rewards"][i]["Num"].asUInt();
		item.propId = rootValue["Roulette"]["Rewards"][i]["ID"].asUInt();
		item.type = rootValue["Roulette"]["Rewards"][i]["Type"].asUInt();
		item.chance = rootValue["Roulette"]["Rewards"][i]["Chance"].asUInt();
		item.id = idx++;
		itemVec.push_back(item);
	}
	return true;
}

BOOL RouletteCfg::GenerateRouletteBigReward(SRoulettePropCfgItem& bigReward, UINT32& spendToken)
{
	Json::Value rootValue;
	if (!OpenCfgFile(m_filename.c_str(), rootValue))
		return false;

	BYTE awardNum = rootValue["Roulette"]["AwardsNum"].asUInt();
	if (awardNum == 0)
		return true;
	m_everydayGiveTimes = rootValue["Roulette"]["GiveNum"].asUInt();
	spendToken = rootValue["Roulette"]["RefreshSpend"].asUInt();

	UINT16 chance = 0;
	std::map<UINT16, SDropPropCfgItem> bigRewardMap;
	int si = rootValue["Roulette"]["Awards"].size();
	for (int i = 0; i < si; ++i)
	{
		SDropPropCfgItem item;
		UINT16 nChance = rootValue["Roulette"]["Awards"][i]["Chance"].asUInt();
		chance = SGSU16Add(chance, nChance);

		bigRewardMap.insert(std::make_pair(chance, item));
		SDropPropCfgItem& itemRef = bigRewardMap[chance];
		itemRef.num = rootValue["Roulette"]["Awards"][i]["Num"].asUInt();
		itemRef.type = rootValue["Roulette"]["Awards"][i]["Type"].asUInt();
		itemRef.propId = rootValue["Roulette"]["Awards"][i]["ID"].asUInt();
	}

	int n = rand() % SGS_ROULETTE_REWARD_CHANCE;
	std::map<UINT16, SDropPropCfgItem>::iterator it = bigRewardMap.begin();
	for (; it != bigRewardMap.end(); ++it)
	{
		if (n < it->first)
		{
			bigReward.id = 0;
			bigReward.num = it->second.num;
			bigReward.propId = it->second.propId;
			bigReward.type = it->second.type;
			bigReward.chance = 0;
			break;
		}
	}

	return true;
}