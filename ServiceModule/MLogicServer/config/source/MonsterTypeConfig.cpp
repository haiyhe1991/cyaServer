#include "MonsterTypeConfig.h"


MonsterTypeConfig::MonsterTypeConfig()
{

}

MonsterTypeConfig::~MonsterTypeConfig()
{

}

//���������������
BOOL MonsterTypeConfig::Load(const char* filename)
{
	Json::Value rootValue;
	if (!OpenCfgFile(filename, rootValue))
		return false;

	if (!rootValue.isMember("MonsterType") || !rootValue["MonsterType"].isArray())
		return false;

	for (int i = 0; i < rootValue["MonsterType"].size(); ++i)
	{
		const Json::Value &json_object = rootValue["MonsterType"][i];
		UINT16 id = json_object["ID"].asUInt();
		UINT32 proportion = json_object["HP"].asUInt();
		m_proportionCfg.insert(std::make_pair(id, proportion));
	}

	return TRUE;
}

//��ȡ����Ѫ����
UINT32 MonsterTypeConfig::GetMonsterBloodProportion(UINT16 id)
{
	std::map<UINT16, UINT32>::iterator itr = m_proportionCfg.find(id);
	if (itr == m_proportionCfg.end())
	{
		return 0;
	}
	return itr->second;
}