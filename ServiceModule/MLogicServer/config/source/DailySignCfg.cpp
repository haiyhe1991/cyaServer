#include "cyaTime.h"
#include "DailySignCfg.h"

DailySignCfg::DailySignCfg()
{

}

DailySignCfg::~DailySignCfg()
{

}

BOOL DailySignCfg::Load(const char* pszFile)
{
	Json::Value rootValue;
	if (!OpenCfgFile(pszFile, rootValue))
		return false;

	if (!rootValue["EDSignIn"].isArray())
		return false;

	const int size = rootValue["EDSignIn"].size();
	if (size <= 0)
		return false;

	m_signConfig.is_open = rootValue["IsOpen"].asInt() != 0;
	m_signConfig.start_time = StrToMsel(rootValue["StartTime"].asString().c_str());
	m_signConfig.end_time = StrToMsel(rootValue["EndTime"].asString().c_str());

	SDailySignItem item;
	SDailySignAward award;
	for (int i = 0; i < size; ++i)
	{
		BYTE days = rootValue["EDSignIn"][i]["Days"].asInt();
		item.continuous = rootValue["EDSignIn"][i]["Continuous"].asInt() != 0;
		item.double_vip_level = rootValue["EDSignIn"][i]["DoubleVipLv"].asInt();

		const int reward_num = rootValue["EDSignIn"][i]["Rewards"].size();
		for (int j = 0; j < reward_num; ++j)
		{
			award.id = rootValue["EDSignIn"][i]["Rewards"][j]["ID"].asInt();
			award.num = rootValue["EDSignIn"][i]["Rewards"][j]["NUM"].asInt();
			award.type = rootValue["EDSignIn"][i]["Rewards"][j]["TYPE"].asInt();
			item.award.push_back(award);
		}
		m_signConfig.data.insert(std::make_pair(days, item));
		item.award.resize(0);
	}

	return true;
}

//是否有效
bool DailySignCfg::IsDailySignValid() const
{
	LTMSEL current_time = GetMsel();
	return m_signConfig.is_open && current_time >= m_signConfig.start_time && current_time < m_signConfig.end_time;
}

//获取奖励
bool DailySignCfg::GetDailySignConfig(BYTE days, const SDailySignItem *&config)
{
	config = NULL;
	std::map<BYTE, SDailySignItem>::const_iterator itr = m_signConfig.data.find(days);
	if (itr != m_signConfig.data.end())
	{
		config = &itr->second;
		return true;
	}
	return false;
}