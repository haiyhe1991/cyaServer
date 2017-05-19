#ifndef __GAME_DROP_CFG_H__
#define __GAME_DROP_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include <map>
#include <vector>
#include "CfgCom.h"

#include <map>
#include <vector>
#include "CfgCom.h"

class GameDropCfg
{
public:
	GameDropCfg();
	~GameDropCfg();

#pragma pack(push, 1)
	struct SGameDropItem
	{
		UINT16 propId;
		BYTE   type;
		UINT32 num;
		BYTE   monster;
		UINT16 chanceVal;
		BYTE   repeatDrop;
	};
#pragma pack(pop)

public:
	//载入配置
	BOOL Load(const char* pszFile);
	//生成物品掉落
	void GenerateDrops(UINT32 dropId, BYTE dropNum, std::vector<SDropPropCfgItem>& dropVec);
	//生成区域怪掉落
	void GenerateAreaMonsterDrops(UINT32 dropId, BYTE dropNum, std::vector<SDropPropCfgItem>& monster, std::vector<SDropPropCfgItem>& cream, std::vector<SDropPropCfgItem>& boss);

	/* zpy 2015.12.22新增 展开掉落组 */
	void SpreadDropGroups(UINT32 dropId, BYTE dropNum, std::vector<SDropPropCfgItem> *ret);

private:
	std::map<UINT32/*掉落id*/, std::vector<SGameDropItem> > m_gameDropMap;
};


#endif