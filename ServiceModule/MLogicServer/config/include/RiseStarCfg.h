#ifndef __RISE_STAR_CFG_H__
#define __RISE_STAR_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
װ����������
*/

#include <map>
#include "CfgCom.h"

class RiseStarCfg
{
public:
	RiseStarCfg();
	~RiseStarCfg();

#pragma pack(push, 1)
	struct SRiseStarSpend
	{
		UINT16 stones;	//��Ҫ����ʯ����
		UINT64 gold;	//���ѽ��
		SRiseStarSpend()
		{
			stones = 0;
			gold = 0;
		}
	};

	struct SRiseStarCfg
	{
		BYTE succedChance;	//�ɹ���
		BYTE damagedChance;	//������
		BYTE limitLevel;	//����ȼ�����
		float extraEffect;
		SRiseStarSpend spend;
		SRiseStarCfg()
		{
			succedChance = 0;
			damagedChance = 0;
			limitLevel = 0;
		}
	};
#pragma pack(pop)

public:
	//������������
	BOOL Load(const char* filename);
	//��ȡ���ǻ���
	int GetRiseStarSpend(BYTE starLv, SRiseStarSpend& spend);
	//��ȡ���Ǽ�����ȼ�����
	int GetRiseStarLimitLevel(BYTE starLv, BYTE& limitLevel);
	//��ȡĳ�Ǽ�����(����ʯ����,���)
	int GetStarTotalSpend(BYTE starLv, SRiseStarSpend& totalSpend);
	//��ȡ��������
	int GetRiseStarCfg(BYTE starLv, SRiseStarCfg& cfg);

private:
	std::map<BYTE/*�Ǽ�*/, SRiseStarCfg> m_riseStarMap;
};


#endif