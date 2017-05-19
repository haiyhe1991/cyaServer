#include "GameTypeCfg.h"
#include "GameCfgFileMan.h"

GameTypeCfg::GameTypeCfg()
: m_worldBossBlood(0)
, m_ladderMaxNum(0)
, m_ladderCD(0)
{

}

GameTypeCfg::~GameTypeCfg()
{

}

//载入配置
BOOL GameTypeCfg::Load(const char* pszFile)
{
	Json::Value rootValue;
	if (!OpenCfgFile(pszFile, rootValue))
		return false;

	if (!rootValue.isMember("GameType") || !rootValue["GameType"].isArray())
		return false;

	for (int i = 0; i < rootValue["GameType"].size(); ++i)
	{
		int type = rootValue["GameType"][i]["ID"].asInt();
		if (type == ESGS_INST_WORLD_BOSS)
		{
			m_worldBossBlood = atoi(rootValue["GameType"][i]["GameIcon"].asString().c_str());
		}

		std::vector<SRewardCondition> &vec = m_rewardCondition[type];
		const Json::Value &condition = rootValue["GameType"][i]["Condition"];
		for (int j = 0; j < condition.size(); ++j)
		{
			vec.resize(vec.size() + 1);
			vec.back().num = condition[j]["Num"].asInt();
			vec.back().type = condition[j]["Type"].asInt();

			const Json::Value &reward = condition[j]["Reward"];
			for (int f = 0; f < reward.size(); ++f)
			{
				vec.back().reward.resize(vec.back().reward.size() + 1);
				vec.back().reward.back().id = 0;
				vec.back().reward.back().type = reward[f]["Type"].asInt();
				vec.back().reward.back().num = reward[f]["Num"].asInt();
				if (reward[f].isMember("ID"))
				{
					vec.back().reward.back().id = reward[f]["ID"].asInt();
				}
			}
		}

		// 玩法消耗
		SEexpendInfo &expend = m_expend[type];
		const Json::Value &json_expend = rootValue["GameType"][i]["Expend"];
		if (json_expend.isMember("Eliminate"))
		{
			const Json::Value &json_array = json_expend["Eliminate"];
			for (int i = 0; i < json_array.size(); ++i)
			{
				expend.eliminate.push_back(json_array[i].asUInt());
			}
		}

		if (json_expend.isMember("Refurbish"))
		{
			const Json::Value &json_array = json_expend["Refurbish"];
			for (int i = 0; i < json_array.size(); ++i)
			{
				expend.refurbish.push_back(json_array[i].asUInt());
			}
		}

		if (json_expend.isMember("Buy"))
		{
			const Json::Value &json_array = json_expend["Buy"];
			for (int i = 0; i < json_array.size(); ++i)
			{
				expend.buyPrice.push_back(json_array[i].asUInt());
			}
		}

		// 1v1
		if (type == ESGS_INST_ONE_VS_ONE)
		{
			m_oneOnOne.resize(3);
			for (int idx = 0; idx < m_oneOnOne.size(); ++idx)
			{
				m_oneOnOne[idx] = rootValue["GameType"][i]["Objective"][idx].asUInt();
			}
		}
		// 天梯cd
		else if (type == ESGS_INST_HIGH_LADDER)
		{
			int idx = 0;
			m_ladderMaxNum = rootValue["GameType"][i]["Objective"][idx].asUInt();
			++idx;
			m_ladderCD = rootValue["GameType"][i]["Objective"][idx].asUInt();
		}

		// 复活规则
		BYTE &rule = m_reviveRule[type];
		rule = rootValue["GameType"][i]["ReviveID"].asInt();
	}

	return TRUE;
}

//获取条件和奖励
bool GameTypeCfg::GetConditionAndReward(BYTE type, std::vector<SRewardCondition> *&ret)
{
	condition_map::iterator itr = m_rewardCondition.find(type);
	if (itr == m_rewardCondition.end())
	{
		return false;
	}

	ret = &itr->second;
	return true;
}

//获取cd消耗
UINT32 GameTypeCfg::GetGameTypeEliminateExpend(BYTE type, UINT16 times)
{
	if (times == 0)
	{
		return 0;
	}

	std::map<BYTE, SEexpendInfo>::const_iterator itr = m_expend.find(type);
	if (itr == m_expend.end())
	{
		return 0;
	}

	if (times > itr->second.eliminate.size())
	{
		times = itr->second.eliminate.size();
	}
	return itr->second.eliminate[times - 1];
}

//获取刷新消耗
UINT32 GameTypeCfg::GetGameTypeRefurbishExpend(BYTE type, UINT16 times)
{
	if (times == 0)
	{
		return 0;
	}

	std::map<BYTE, SEexpendInfo>::const_iterator itr = m_expend.find(type);
	if (itr == m_expend.end())
	{
		return 0;
	}

	if (times > itr->second.refurbish.size())
	{
		times = itr->second.refurbish.size();
	}
	return itr->second.refurbish[times - 1];
}

//获取购买价格
UINT32 GameTypeCfg::GetGameTypeBuyPriceExpend(BYTE type, UINT16 times)
{
	if (times == 0)
	{
		return 0;
	}

	std::map<BYTE, SEexpendInfo>::const_iterator itr = m_expend.find(type);
	if (itr == m_expend.end())
	{
		return 0;
	}

	if (times > itr->second.buyPrice.size())
	{
		times = itr->second.buyPrice.size();
	}
	return itr->second.buyPrice[times - 1];
}

//获取复活规则
BYTE GameTypeCfg::GetGameTypeReviveRule(BYTE type)
{
	std::map<BYTE, BYTE>::const_iterator itr = m_reviveRule.find(type);
	if (itr == m_reviveRule.end())
	{
		return 0;
	}
	return itr->second;
}

//获取1v1奖励比率
BYTE GameTypeCfg::GetOneOnOneRewardRatio(BYTE finCode) const
{
	return finCode == 0 ? m_oneOnOne[0] : finCode == 1 ? m_oneOnOne[2] : m_oneOnOne[1];
}

//解除奖励嵌套
void GameTypeCfg::UnlockNesting(const std::vector<SRewardInfo> &source, std::vector<SRewardInfo> *ret)
{
	ret->clear();
	for (size_t i = 0; i < source.size(); ++i)
	{
		if (source[i].type == ESGS_PROP_DROP_ID)
		{
			std::vector<SDropPropCfgItem> props;
			GetGameCfgFileMan()->SpreadDropGroups(source[i].id, source[i].num, &props);
			ret->reserve(ret->size() + props.size());
			for (size_t i = 0; i < props.size(); ++i)
			{
				ret->resize(ret->size() + 1);
				ret->back().num = props[i].num;
				ret->back().type = props[i].type;
				ret->back().id = props[i].propId;
			}
		}
		else
		{
			ret->push_back(source[i]);
		}
	}
}