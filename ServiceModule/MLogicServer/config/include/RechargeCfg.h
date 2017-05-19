#ifndef __RECHARGE_CFG_H__
#define __RECHARGE_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
��ֵ����
*/

#include <map>
#include "CfgCom.h"

class RechargeCfg
{
public:
	RechargeCfg();
	~RechargeCfg();

#pragma pack(push, 1)
	struct SRechargeCfgAttr
	{
		UINT32 rmb;			//�����
		UINT32 rpcoin;		//����
		UINT32 firstGive;	//�״γ�ֵ���ʹ���
		UINT32 everydayGive;	//ÿ�����ʹ���
		UINT16 giveDays;		//��������

		SRechargeCfgAttr()
		{
			rmb = 0;
			rpcoin = 0;
			firstGive = 0;
			everydayGive = 0;
			giveDays = 0;
		}
	};
#pragma pack(pop)

public:
	//�����ֵ����
	BOOL Load(const char* filename);
	//��ȡ��ֵ����
	int GetRechargeAttr(UINT16 cashId, SRechargeCfgAttr& cashAttr);
	//��ȡ��ֵ����
	const std::map<UINT16, SRechargeCfgAttr>& Get() const;

private:
	std::map<UINT16/*��ֵid*/, SRechargeCfgAttr> m_rechargeMap;
};


#endif