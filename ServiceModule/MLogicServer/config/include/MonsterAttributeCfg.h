#ifndef __MONSTER_ATTRIBUTE_CFG_H__
#define __MONSTER_ATTRIBUTE_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
������������
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
	//���������������
	BOOL Load(const char* filename);

	//��ȡ����Ѫ��
	UINT32 GetMonsterBlood(UINT16 level);

private:
	std::map<UINT16/*�ȼ�*/, UINT32/*Ѫ��*/> m_bloodCfg;
};


#endif