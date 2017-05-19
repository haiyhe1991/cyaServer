#include "ServiceErrorCode.h"
#include "VitExchangeCfg.h"

VitExchangeCfg::VitExchangeCfg()
{

}

VitExchangeCfg::~VitExchangeCfg()
{

}

BOOL VitExchangeCfg::Load(const char* filename)
{
	Json::Value rootValue;
	if (!OpenCfgFile(filename, rootValue))
		return false;

	if (!rootValue.isMember("PowerExchange"))
		return false;

	m_vitExchange.base_num = rootValue["PowerExchange"]["BaseNum"].asInt();
	m_vitExchange.vip_level_limit = rootValue["PowerExchange"]["VIPLvLimit"].asInt();

	for (size_t i = 0; i < rootValue["PowerExchange"]["BasePower"].size(); ++i)
	{
		m_vitExchange.base_power.push_back(rootValue["PowerExchange"]["BasePower"][i].asInt());
	}

	for (size_t i = 0; i < rootValue["PowerExchange"]["TokenExpend"].size(); ++i)
	{
		m_vitExchange.token_expend.push_back(rootValue["PowerExchange"]["TokenExpend"][i].asInt());
	}

	ASSERT(m_vitExchange.token_expend.size() == m_vitExchange.base_power.size());

	return true;
}

int VitExchangeCfg::GetVitExchangeExchange(UINT16 exchangeTimes, UINT16 vipAddTimes, UINT64& getVit, UINT32& spendToken)
{
	UINT16 maxTimes = SGSU16Add(m_vitExchange.base_num, vipAddTimes);
	if (exchangeTimes >= maxTimes)
		return MLS_OVER_DAY_EXCHANGE_TIMES;

	size_t size = m_vitExchange.token_expend.size();
	if (size <= 0)
		return MLS_OVER_DAY_EXCHANGE_TIMES;

	int idx = exchangeTimes >= size ? size - 1 : exchangeTimes;
	getVit = m_vitExchange.base_power[idx];
	spendToken = m_vitExchange.token_expend[idx];
	return MLS_OK;
}