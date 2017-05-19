#ifndef __MONSTER_TYPE_CONFIG_H__
#define __MONSTER_TYPE_CONFIG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
怪物类型配置
*/

#include <map>
#include "CfgCom.h"
#include "ServiceMLogic.h"

class MonsterTypeConfig
{
public:
	MonsterTypeConfig();
	~MonsterTypeConfig();

public:
	//载入类型属性配置
	BOOL Load(const char* filename);

	//获取怪物血比例
	UINT32 GetMonsterBloodProportion(UINT16 id);

private:
	std::map<UINT16, UINT32> m_proportionCfg;
};


#endif