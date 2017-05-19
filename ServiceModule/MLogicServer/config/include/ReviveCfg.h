#ifndef __REVIVE_CFG_H__
#define __REVIVE_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
复活配置
*/

#include <map>
#include <vector>
#include "CfgCom.h"

class ReviveCfg
{
#pragma pack(push, 1)
	struct SReviveAttr
	{
		BYTE   maxTimes;		//最大复活次数
		UINT32 cdTimes;			//cd时间
		std::vector<UINT32> reviveCoinVec;	//复活币
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
	//载入复活配置
	BOOL Load(const char* filename);
	//获取复活消耗
	int GetReviveConsume(UINT16 reviveId, UINT32 times, UINT16 vipAddTimes, UINT32& reviveCoin);
	//获取复活CD
	int GetReviveCDTimes(UINT16 reviveId);

private:
	std::map<UINT16, SReviveAttr> m_reviveCfgMap;
};

#endif