#ifndef __MONSTER_CFG_H__
#define __MONSTER_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
������
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
		UINT32 exp;			//����
		BYTE type;			//����
		UINT32 difficulty;	//�����Ѷ�ϵ
	};

public:
	//���������ļ�
	BOOL Load(const char* filename);
	//��ȡ�־���
	UINT32 GetMonsterExp(UINT16 monsterId);
	//��ȡ������
	BYTE GetMonsterType(UINT16 monsterId);
	//��ȡ���Ѷ�ϵ��
	UINT32 GetMonsterDifficulty(UINT16 monsterId);

private:
	std::map<UINT16/*��id*/, MonsterAttr/*�ֻ�������*/> m_monsterExpMap;
};

#endif