#ifndef __VIT_EXCHANGE_MAN_H__
#define __VIT_EXCHANGE_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
#pragma once
#endif

/*
体力兑换
*/

#include <map>
#include "cyaTypes.h"
#include "cyaLocker.h"
#include "cyaCoreTimer.h"

class VitExchangeMan
{
#pragma pack(push, 1)
	struct SVitExchangeUpdate
	{
		UINT16 nTimes;	//当前兑换次数
		LTMSEL tts;		//最后一次兑换时间
	};
#pragma pack(pop)

public:
	VitExchangeMan();
	~VitExchangeMan();

public:
	void Start();
	void Stop();

	int  QueryRoleVitExchange(UINT32 roleId, UINT16& exchangedTimes);
	void RefeshRoleVitExchange(UINT32 roleId, UINT16 nTimes);
	void ClearVitExchangeRecord();

private:
	static void VitExchangeTimer(void* param, TTimerID id);
	void OnVitExchange();
	void OnRefreshDBVitExchange(UINT32 roleId, UINT16 nTimes, LTMSEL tts);

private:
	CXTLocker m_locker;
	CyaCoreTimer m_timer;
	std::map<UINT32/*角色id*/, SVitExchangeUpdate> m_vitExchangeMap;
};

void InitVitExchangeMan();
VitExchangeMan* GetVitExchangeMan();
void DestroyVitExchangeMan();

#endif