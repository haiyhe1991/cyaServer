#include "GreedTheLiveCfg.h"


GreedTheLiveCfg::GreedTheLiveCfg()
{

}

GreedTheLiveCfg::~GreedTheLiveCfg()
{

}

BOOL GreedTheLiveCfg::Load(const char* pszFile)
{
	Json::Value rootValue;
	if (!OpenCfgFile(pszFile, rootValue))
		return false;

	if (!rootValue["Core"].isArray())
		return false;

	if (!rootValue["GreedTheLive"].isArray())
		return false;

	//是否开启炼魂系统
	m_is_open = rootValue["IsOpen"].asInt() != 0;

	//读取部件配置
	for (size_t i = 0; i < rootValue["GreedTheLive"].size(); ++i)
	{
		UINT16 id = rootValue["GreedTheLive"][i]["ID"].asInt();
		BYTE type = rootValue["GreedTheLive"][i]["Type"].asInt();
		ChainSoulPosConfig &config = m_chainSoulPosConfig[type][id];
		config.next_id = rootValue["GreedTheLive"][i]["NextID"].asInt();

		//读取升级消耗
		for (size_t j = 0; j < rootValue["GreedTheLive"][i]["UpCost"].size(); ++j)
		{
			SUpCost cost;
			cost.id = rootValue["GreedTheLive"][i]["UpCost"][j]["ID"].asInt();
			cost.num = rootValue["GreedTheLive"][i]["UpCost"][j]["Num"].asInt();
			cost.type = rootValue["GreedTheLive"][i]["UpCost"][j]["Type"].asInt();
			config.cost.push_back(cost);
		}

		//读取升级属性
		for (size_t j = 0; j < rootValue["GreedTheLive"][i]["Attributes"].size(); ++j)
		{
			SAttributes attribute;
			attribute.type = rootValue["GreedTheLive"][i]["Attributes"][j]["Type"].asInt();
			attribute.attrVal = rootValue["GreedTheLive"][i]["Attributes"][j]["AttrVal"].asInt();
			config.attr_plus.push_back(attribute);
		}

		// id归类
		m_typeSet[type].push_back(id);
	}

	//读取圣物核心配置
	for (size_t i = 0; i < rootValue["Core"].size(); ++i)
	{
		BYTE level = rootValue["Core"][i]["LV"].asInt();
		ChainSoulCoreConfig &core_config = m_chainSoulCoreConfig[level];
		core_config.condition_level = rootValue["Core"][i]["ConditionLV"].asInt();
		for (size_t j = 0; j < rootValue["Core"][i]["Attributes"].size(); ++j)
		{
			SAttributes attribute;
			attribute.type = rootValue["Core"][i]["Attributes"][j]["Type"].asInt();
			attribute.attrVal = rootValue["Core"][i]["Attributes"][j]["AttrVal"].asInt();
			core_config.attr_plus.push_back(attribute);
		}

		// 条件等级与核心等级相对应
		m_conditionCorrespondsCore.insert(std::make_pair(core_config.condition_level, level));
	}

	return true;
}

//是否启用
bool GreedTheLiveCfg::IsOpen() const
{
	return m_is_open;
}

//获取部件配置
bool GreedTheLiveCfg::GetChainSoulPosConfig(BYTE type, BYTE level, ChainSoulPosConfig *&out_config)
{
	out_config = NULL;

	std::map<BYTE, std::vector<UINT16> >::iterator itr = m_typeSet.find(type);
	if (itr == m_typeSet.end()) return false;

	for (size_t i = 0; i < itr->second.size(); ++i)
	{
		if (i + 1 == level)
		{
			UINT16 id = itr->second[i];
			cspc_map &config_map = m_chainSoulPosConfig[type];
			out_config = &config_map[id];
			return true;
		}
	}

	return false;
}

//获取圣物核心配置
bool GreedTheLiveCfg::GetChainSoulCoreConfig(BYTE level, ChainSoulCoreConfig *&out_config)
{
	out_config = NULL;

	std::map<BYTE, ChainSoulCoreConfig>::iterator itr = m_chainSoulCoreConfig.find(level);
	if (itr != m_chainSoulCoreConfig.end())
	{
		out_config = &itr->second;
		return true;
	}

	return false;
}

//根据条件获取核心等级
BYTE GreedTheLiveCfg::GetCoreLevelByConditionLevel(BYTE condition_level)
{
	std::map<BYTE, BYTE>::iterator itr = m_conditionCorrespondsCore.find(condition_level);
	if (itr != m_conditionCorrespondsCore.end())
	{
		return itr->second;
	}
	else
	{
		BYTE core_level = 0;
		for (itr = m_conditionCorrespondsCore.begin(); itr != m_conditionCorrespondsCore.end(); ++itr)
		{
			if (itr->first <= condition_level)
			{
				core_level = itr->second;
			}
		}
		return core_level;
	}
}