#include "MonsterCfg.h"
#include "ServiceErrorCode.h"
#include "cyaMaxMin.h"

MonsterCfg::MonsterCfg()
{

}

MonsterCfg::~MonsterCfg()
{

}

BOOL MonsterCfg::Load(const char* filename)
{
	Json::Value rootValue;
	if (!OpenCfgFile(filename, rootValue))
		return false;

	if (!rootValue.isMember("Monster") || !rootValue["Monster"].isArray())
		return false;

	MonsterAttr attr;
	int nSize = rootValue["Monster"].size();
	for (int i = 0; i < nSize; ++i)
	{
		UINT16 monsterId = rootValue["Monster"][i]["ID"].asUInt();
		attr.exp = rootValue["Monster"][i]["Exp"].asUInt();
		attr.type = rootValue["Monster"][i]["Type"].asUInt();
		attr.difficulty = rootValue["Monster"][i]["Difficulty"].asUInt();
		m_monsterExpMap.insert(std::make_pair(monsterId, attr));
	}
	return true;
}

UINT32 MonsterCfg::GetMonsterExp(UINT16 monsterId)
{
	UINT32 exp = 0;
	std::map<UINT16, MonsterAttr>::iterator it = m_monsterExpMap.find(monsterId);
	if (it != m_monsterExpMap.end())
		exp = it->second.exp;
	return exp;
}

BYTE MonsterCfg::GetMonsterType(UINT16 monsterId)
{
	std::map<UINT16, MonsterAttr>::iterator it = m_monsterExpMap.find(monsterId);
	if (it != m_monsterExpMap.end())
		return it->second.type;
	return 0;
}

UINT32 MonsterCfg::GetMonsterDifficulty(UINT16 monsterId)
{
	std::map<UINT16, MonsterAttr>::iterator it = m_monsterExpMap.find(monsterId);
	if (it != m_monsterExpMap.end())
		return it->second.difficulty;
	return 0;
}