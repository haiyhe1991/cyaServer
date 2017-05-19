#include "PlayerCfg.h"
#include "ServiceErrorCode.h"


PlayerCfg::PlayerCfg()
{

}

PlayerCfg::~PlayerCfg()
{

}

BOOL PlayerCfg::Load(const char* filename)
{
	Json::Value rootValue;
	if (!OpenCfgFile(filename, rootValue))
		return FALSE;

	if (!rootValue["PlayerConfig"].isArray())
		return FALSE;

	for (int i = 0; i < rootValue["PlayerConfig"].size(); ++i)
	{
		const Json::Value &json_object = rootValue["PlayerConfig"][i];
		BYTE actor_id = json_object["ID"].asInt();
		UINT32 chance = json_object["Chance"].asInt();
		m_playerInfo.insert(std::make_pair(actor_id, chance));
	}

	return TRUE;
}

// 生成周免角色
BYTE PlayerCfg::GeneraWeekFreeActorType()
{
	UINT32 sum = 0;
	for (std::map<BYTE, UINT32>::iterator itr = m_playerInfo.begin(); itr != m_playerInfo.end(); ++itr)
	{
		sum += itr->second;
	}

	UINT32 value = 0;
	UINT32 rand_value = 1 + rand() % sum;
	for (std::map<BYTE, UINT32>::iterator itr = m_playerInfo.begin(); itr != m_playerInfo.end(); ++itr)
	{
		value += itr->second;
		if (rand_value <= value)
		{
			return itr->first;
		}
	}
	return 0;
}