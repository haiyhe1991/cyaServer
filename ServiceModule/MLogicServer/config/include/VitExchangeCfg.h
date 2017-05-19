#ifndef __VIT_EXCHANGE_CFG_H__
#define __VIT_EXCHANGE_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
�����һ�
*/

#include <map>
#include <vector>
#include "CfgCom.h"

class VitExchangeCfg
{
public:
	VitExchangeCfg();
	~VitExchangeCfg();

#pragma pack(push, 1)
	struct SVitExchangeAttr
	{
		std::vector<UINT32> base_power;		// �һ�����
		std::vector<UINT32> token_expend;	// �һ�����
		UINT16 base_num;					// ÿ�նһ�����
		BYTE vip_level_limit;				// Vip�ȼ�����
	};
#pragma pack(pop)

public:
	//����һ�����
	BOOL Load(const char* filename);
	//��ȡ�һ�����
	int GetVitExchangeExchange(UINT16 exchangeTimes, UINT16 vipAddTimes, UINT64& getVit, UINT32& spendToken);

private:
	SVitExchangeAttr m_vitExchange;
};


#endif