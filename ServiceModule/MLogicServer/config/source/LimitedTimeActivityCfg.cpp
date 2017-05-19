#include "LimitedTimeActivityCfg.h"
#include "GameCfgFileMan.h"

LimitedTimeActivityCfg::LimitedTimeActivityCfg()
{

}

LimitedTimeActivityCfg::~LimitedTimeActivityCfg()
{

}

//载入配置
BOOL LimitedTimeActivityCfg::Load(const char* pszFile)
{
	Json::Value rootValue;
	if (!OpenCfgFile(pszFile, rootValue))
		return false;

	if (!rootValue["SStageSystem"].isArray())
		return false;

	for (int i = 0; i < rootValue["SStageSystem"].size(); ++i)
	{
		int id = rootValue["SStageSystem"][i]["ID"].asInt();
		SActivityConfig &config = m_activityCfg[id];

		config.IsOpen = rootValue["SStageSystem"][i]["IsOpen"].asInt() == 1;
		config.Type = rootValue["SStageSystem"][i]["Type"].asInt();
		config.Interval = rootValue["SStageSystem"][i]["Interval"].asInt();
		config.Chance = rootValue["SStageSystem"][i]["Chance"].asInt();
		config.LastTime = rootValue["SStageSystem"][i]["LastTime"].asInt();
		config.LimitNum = rootValue["SStageSystem"][i]["LimitNum"].asInt();
		config.MaxOpenNum = rootValue["SStageSystem"][i]["MaxOpenNum"].asInt();
		config.MinOpenNum = rootValue["SStageSystem"][i]["MinOpenNum"].asInt();
		config.TransferNum = rootValue["SStageSystem"][i]["TransferNum"].asInt();

		config.Desc = rootValue["SStageSystem"][i]["Desc"].asString();
		std::string StartTime = rootValue["SStageSystem"][i]["StartTime"].asString();
		std::string EndTime = rootValue["SStageSystem"][i]["EndTime"].asString();
		if (StartTime.size() != 4 || EndTime.size() != 4)
		{
			return false;
		}

		char buffer[3];
		memset(buffer, 0, sizeof(buffer));
		memcpy(buffer, StartTime.c_str(), 2);
		config.StartTimeHour = atoi(buffer);
		memcpy(buffer, StartTime.c_str() + 2, 2);
		config.StartTimeMinute = atoi(buffer);
		memcpy(buffer, EndTime.c_str(), 2);
		config.EndTimeHour = atoi(buffer);
		memcpy(buffer, EndTime.c_str() + 2, 2);
		config.EndTimeHourMinute = atoi(buffer);

		if (config.Type == 3)
		{
			m_activityInst.push_back(id);
		}
	}

	return true;
}

//获取限时活动配置
bool LimitedTimeActivityCfg::GetLimitedTimeActivityCfg(ActivityType type, SActivityConfig &out)
{
	std::map<int, SActivityConfig>::const_iterator itr = m_activityCfg.find(type);
	if (itr == m_activityCfg.end())
	{
		return false;
	}

	out = itr->second;
	return true;
}

//生成活动副本
bool LimitedTimeActivityCfg::GenerateActivityInst(int level, std::set<UINT16> passInsts, SActivityInstItem &item)
{
	// 初始化
	{
		item.num = 0;
		item.instId = 0;
	}

	std::vector<int> insts;
	UINT32 chanceUpperLimit = 0;
	ActivityStageCfg::Condition condition;
	for (size_t i = 0; i < m_activityInst.size(); ++i)
	{
		if (GetGameCfgFileMan()->GetActivityStageOpenCondition(m_activityInst[i], condition))
		{
			if (condition.type == ActivityStageCfg::Level)
			{
				if (level >= condition.num)
				{
					insts.push_back(m_activityInst[i]);
					chanceUpperLimit += m_activityCfg[m_activityInst[i]].Chance;
				}
			}
			else if (condition.type == ActivityStageCfg::PassInst)
			{
				if (passInsts.find(condition.num) != passInsts.end())
				{
					insts.push_back(m_activityInst[i]);
					chanceUpperLimit += m_activityCfg[m_activityInst[i]].Chance;
				}
			}
		}
		else
		{
			ASSERT(false);
		}
	}

	if (chanceUpperLimit > 0)
	{
		UINT32 value = 0;
		UINT32 rand_value = 1 + rand() % chanceUpperLimit;
		for (size_t i = 0; i < insts.size(); ++i)
		{
			UINT32 instId = insts[i];
			value += m_activityCfg[instId].Chance;
			if (rand_value <= value)
			{
				item.instId = instId;
				item.num = GetGameCfgFileMan()->GetInstEnterNum(instId);
				break;
			}
		}
	}

	return item.instId != 0 ? item.num > 0 : false;
}