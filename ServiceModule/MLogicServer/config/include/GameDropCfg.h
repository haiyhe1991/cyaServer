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
	//��������
	BOOL Load(const char* pszFile);
	//������Ʒ����
	void GenerateDrops(UINT32 dropId, BYTE dropNum, std::vector<SDropPropCfgItem>& dropVec);
	//��������ֵ���
	void GenerateAreaMonsterDrops(UINT32 dropId, BYTE dropNum, std::vector<SDropPropCfgItem>& monster, std::vector<SDropPropCfgItem>& cream, std::vector<SDropPropCfgItem>& boss);

	/* zpy 2015.12.22���� չ�������� */
	void SpreadDropGroups(UINT32 dropId, BYTE dropNum, std::vector<SDropPropCfgItem> *ret);

private:
	std::map<UINT32/*����id*/, std::vector<SGameDropItem> > m_gameDropMap;
};


#endif