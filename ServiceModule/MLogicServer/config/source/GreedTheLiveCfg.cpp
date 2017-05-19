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

	//�Ƿ�������ϵͳ
	m_is_open = rootValue["IsOpen"].asInt() != 0;

	//��ȡ��������
	for (size_t i = 0; i < rootValue["GreedTheLive"].size(); ++i)
	{
		UINT16 id = rootValue["GreedTheLive"][i]["ID"].asInt();
		BYTE type = rootValue["GreedTheLive"][i]["Type"].asInt();
		ChainSoulPosConfig &config = m_chainSoulPosConfig[type][id];
		config.next_id = rootValue["GreedTheLive"][i]["NextID"].asInt();

		//��ȡ��������
		for (size_t j = 0; j < rootValue["GreedTheLive"][i]["UpCost"].size(); ++j)
		{
			SUpCost cost;
			cost.id = rootValue["GreedTheLive"][i]["UpCost"][j]["ID"].asInt();
			cost.num = rootValue["GreedTheLive"][i]["UpCost"][j]["Num"].asInt();
			cost.type = rootValue["GreedTheLive"][i]["UpCost"][j]["Type"].asInt();
			config.cost.push_back(cost);
		}

		//��ȡ��������
		for (size_t j = 0; j < rootValue["GreedTheLive"][i]["Attributes"].size(); ++j)
		{
			SAttributes attribute;
			attribute.type = rootValue["GreedTheLive"][i]["Attributes"][j]["Type"].asInt();
			attribute.attrVal = rootValue["GreedTheLive"][i]["Attributes"][j]["AttrVal"].asInt();
			config.attr_plus.push_back(attribute);
		}

		// id����
		m_typeSet[type].push_back(id);
	}

	//��ȡʥ���������
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

		// �����ȼ�����ĵȼ����Ӧ
		m_conditionCorrespondsCore.insert(std::make_pair(core_config.condition_level, level));
	}

	return true;
}

//�Ƿ�����
bool GreedTheLiveCfg::IsOpen() const
{
	return m_is_open;
}

//��ȡ��������
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

//��ȡʥ���������
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

//����������ȡ���ĵȼ�
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