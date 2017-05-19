#ifndef __MONSTER_ATTRIBUTE_CFG_H__
#define __MONSTER_ATTRIBUTE_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
怪物属性配置
*/

#include <map>
#include "CfgCom.h"
#include "ServiceMLogic.h"

class MonsterAttributeCfg
{
public:
	MonsterAttributeCfg();
	~MonsterAttributeCfg();

public:
	//载入怪物属性配置
	BOOL Load(const char* filename);

	//获取怪物血量
	UINT32 GetMonsterBlood(UINT16 level);

private:
	std::map<UINT16/*等级*/, UINT32/*血量*/> m_bloodCfg;
};


#endif