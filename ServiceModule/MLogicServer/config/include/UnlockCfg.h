#ifndef __UNLOCK_CFG_H__
#define __UNLOCK_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
解锁配置
*/

#include <map>
#include "CfgCom.h"

class UnlockCfg
{
public:
	UnlockCfg();
	~UnlockCfg();

#pragma pack(push, 1)
	struct SUnlockItem
	{
		BYTE isLock;	//是否上锁
		UINT32 gold;	//解锁金币消耗
		UINT32 rpcoin;	//解锁代币消耗
		BYTE   lvLimit;	//等级限制
	};
	struct SUnlockObj
	{
		std::map<UINT16/*解锁id*/, SUnlockItem> unlockItemMap;
	};
#pragma pack(pop)

public:
	//载入配置文件
	BOOL Load(const char* filename);
	//获取解锁参数
	int GetUnlockItem(BYTE type, UINT16 unlockId, SUnlockItem*& pUnlockItem);
	//获取未上锁id
	int GetUnlockedItems(BYTE type, std::vector<UINT16>& itemsVec);

private:
	std::map<BYTE/*解锁类型*/, SUnlockObj> m_unlockCfgMap;
};


#endif