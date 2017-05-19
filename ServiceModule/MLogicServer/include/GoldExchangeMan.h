#ifndef __GOLD_EXCHANGE_MAN_H__
#define __GOLD_EXCHANGE_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
金币兑换
*/

#include <map>
#include "cyaTypes.h"
#include "cyaLocker.h"
#include "cyaCoreTimer.h"

class GoldExchangeMan
{
#pragma pack(push, 1)
	struct SGoldExchangeUpdate
	{
		UINT16 nTimes;	//当前兑换次数
		LTMSEL tts;		//最后一次兑换时间
	};
#pragma pack(pop)

public:
	GoldExchangeMan();
	~GoldExchangeMan();

public:
	void Start();
	void Stop();

	int  QueryRoleGoldExchange(UINT32 roleId, UINT16& exchangedTimes);
	void RefeshRoleGoldExchange(UINT32 roleId, UINT16 nTimes);
	void ClearGoldExchangeRecord();

private:
	static void GoldExchangeTimer(void* param, TTimerID id);
	void OnGoldExchange();
	void OnRefreshDBGoldExchange(UINT32 roleId, UINT16 nTimes, LTMSEL tts);

private:
	CXTLocker m_locker;
	CyaCoreTimer m_timer;
	std::map<UINT32/*角色id*/, SGoldExchangeUpdate> m_goldExchangeMap;
};

void InitGoldExchangeMan();
GoldExchangeMan* GetGoldExchangeMan();
void DestroyGoldExchangeMan();

#endif	//_GoldExchangeMan_h__