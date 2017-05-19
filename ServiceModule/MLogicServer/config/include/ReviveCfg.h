#ifndef __REVIVE_CFG_H__
#define __REVIVE_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
��������
*/

#include <map>
#include <vector>
#include "CfgCom.h"

class ReviveCfg
{
#pragma pack(push, 1)
	struct SReviveAttr
	{
		BYTE   maxTimes;		//��󸴻����
		UINT32 cdTimes;			//cdʱ��
		std::vector<UINT32> reviveCoinVec;	//�����
		SReviveAttr()
		{
			maxTimes = 0;
			reviveCoinVec.clear();
		}
	};
#pragma pack(pop)

public:
	ReviveCfg();
	~ReviveCfg();

public:
	//���븴������
	BOOL Load(const char* filename);
	//��ȡ��������
	int GetReviveConsume(UINT16 reviveId, UINT32 times, UINT16 vipAddTimes, UINT32& reviveCoin);
	//��ȡ����CD
	int GetReviveCDTimes(UINT16 reviveId);

private:
	std::map<UINT16, SReviveAttr> m_reviveCfgMap;
};

#endif