#ifndef __ROULETTE_CFG_H__
#define __ROULETTE_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
��������
*/

#include <vector>
#include "CfgCom.h"
#include "ServiceMLogic.h"

class RouletteCfg
{
public:
	RouletteCfg();
	~RouletteCfg();

#pragma pack(push, 1)
	struct SRoulettePropCfgItem
	{
		BYTE id;		//���̸���id
		BYTE type;		//����(ESGSPropType)
		UINT16 propId;	//��Ʒid
		UINT32 num;		//��Ʒ����
		UINT16 chance;	//����
	};
#pragma pack(pop)

public:
	//���������ļ�
	BOOL Load(const char* filename);
	//��ȡÿ�����ʹ���
	BYTE GetEverydayGiveTimes() const;
	//�������̽�Ʒ
	BOOL GenerateRouletteReward(SGetRoulettePropRes* pRoulettePropRes, std::vector<SRoulettePropCfgItem>& saveItemVec);
	BOOL GenerateRouletteReward(std::vector<SRoulettePropCfgItem>& itemVec);
	//�������̴�
	BOOL GenerateRouletteBigReward(SRoulettePropCfgItem& bigReward, UINT32& spendToken);

private:
	std::string m_filename;
	BYTE m_everydayGiveTimes;
};

#endif