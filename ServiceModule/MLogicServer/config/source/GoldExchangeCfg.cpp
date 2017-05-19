#include "GoldExchangeCfg.h"
#include "ServiceErrorCode.h"

GoldExchangeCfg::GoldExchangeCfg()
{

}

GoldExchangeCfg::~GoldExchangeCfg()
{

}

BOOL GoldExchangeCfg::Load(const char* filename)
{
	Json::Value rootValue;
	if (!OpenCfgFile(filename, rootValue))
		return false;

	if (!rootValue.isMember("GoldExchange"))
		return false;

	m_goldExchange.getGold = rootValue["GoldExchange"]["BaseGold"].asUInt();
	m_goldExchange.goldIncreament = rootValue["GoldExchange"]["GoldIncrement"].asUInt();

	int len = rootValue["GoldExchange"]["TokenExpend"].size();
	for (int i = 0; i < len; i++)
		m_goldExchange.spendToken.push_back(rootValue["GoldExchange"]["TokenExpend"][i].asUInt());

	m_goldExchange.everydayMaxTimes = (UINT16)rootValue["GoldExchange"]["BaseNum"].asUInt();
	m_goldExchange.vipLimitLv = (BYTE)rootValue["GoldExchange"]["VIPLvLimit"].asUInt();

	return true;
}

int GoldExchangeCfg::GetGoldExchange(UINT16 exchangeTimes, UINT16 vipAddTimes, UINT64& getGold, UINT32& spendToken)
{
	UINT16 maxTimes = SGSU16Add(m_goldExchange.everydayMaxTimes, vipAddTimes);
	if (exchangeTimes >= maxTimes)
		return MLS_OVER_DAY_EXCHANGE_TIMES;

	int si = (int)m_goldExchange.spendToken.size();
	if (si <= 0)
		return MLS_OVER_DAY_EXCHANGE_TIMES;

	int idx = exchangeTimes >= si ? si - 1 : exchangeTimes;
	getGold = m_goldExchange.getGold + m_goldExchange.goldIncreament * idx;
	spendToken = m_goldExchange.spendToken[idx];
	return MLS_OK;
}