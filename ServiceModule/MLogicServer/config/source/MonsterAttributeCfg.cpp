#include "MonsterAttributeCfg.h"


MonsterAttributeCfg::MonsterAttributeCfg()
{

}

MonsterAttributeCfg::~MonsterAttributeCfg()
{

}

//载入怪物属性配置
BOOL MonsterAttributeCfg::Load(const char* filename)
{
	Json::Value rootValue;
	if (!OpenCfgFile(filename, rootValue))
		return false;

	if (!rootValue.isMember("MonterAttri") || !rootValue["MonterAttri"].isArray())
		return false;

	for (int i = 0; i < rootValue["MonterAttri"].size(); ++i)
	{
		const Json::Value &json_object = rootValue["MonterAttri"][i];
		UINT16 level = json_object["Level"].asUInt();
		UINT32 blood = json_object["HP"].asUInt();
		m_bloodCfg.insert(std::make_pair(level, blood));
	}

	return TRUE;
}

//获取怪物血量
UINT32 MonsterAttributeCfg::GetMonsterBlood(UINT16 level)
{
	std::map<UINT16/*等级*/, UINT32/*血量*/>::iterator itr = m_bloodCfg.find(level);
	if (itr == m_bloodCfg.end())
	{
		return 0;
	}
	return itr->second;
}