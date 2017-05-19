#ifndef __MONSTER_CFG_H__
#define __MONSTER_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
怪配置
*/

#include <map>
#include "CfgCom.h"

class MonsterCfg
{
public:
	MonsterCfg();
	~MonsterCfg();

	struct MonsterAttr
	{
		UINT32 exp;			//经验
		BYTE type;			//类型
		UINT32 difficulty;	//怪物难度系
	};

public:
	//载入配置文件
	BOOL Load(const char* filename);
	//获取怪经验
	UINT32 GetMonsterExp(UINT16 monsterId);
	//获取怪类型
	BYTE GetMonsterType(UINT16 monsterId);
	//获取怪难度系数
	UINT32 GetMonsterDifficulty(UINT16 monsterId);

private:
	std::map<UINT16/*怪id*/, MonsterAttr/*怪基础属性*/> m_monsterExpMap;
};

#endif