#ifndef __GOLD_EXCHANGE_CFG_H__
#define __GOLD_EXCHANGE_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
��Ҷһ�
*/

#include <map>
#include "CfgCom.h"
#include <vector>
class GoldExchangeCfg
{
public:
	GoldExchangeCfg();
	~GoldExchangeCfg();

#pragma pack(push, 1)//by hxw 20150902 
	struct SGoldExchangeAttr
	{
		UINT64 getGold;		//��ý��
		std::vector<UINT32> spendToken;	//���Ѵ�������
		UINT32 goldIncreament;	//���ÿ������
		//	UINT32 tokenIncreament;	//����ÿ������
		UINT16 everydayMaxTimes;	//ÿ�����һ�����
		BYTE   vipLimitLv;			//vip�ȼ�����


		SGoldExchangeAttr()
		{
			getGold = 0;
			goldIncreament = 0;
			//	tokenIncreament = 0;
			everydayMaxTimes = 0;
			vipLimitLv = 0;
		}
	};
#pragma pack(pop)

public:
	//����һ�����
	BOOL Load(const char* filename);
	//��ȡ�һ�����
	int GetGoldExchange(UINT16 exchangeTimes, UINT16 vipAddTimes, UINT64& getGold, UINT32& spendToken);

private:
	SGoldExchangeAttr m_goldExchange;
};

#endif