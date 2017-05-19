#ifndef __VIP_MAN_H__
#define __VIP_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
VIP管理
*/

#include <vector>
#include "ServiceMLogic.h"

class VIPMan
{
	VIPMan(const VIPMan&);
	VIPMan& operator = (const VIPMan&);

public:
	VIPMan();
	~VIPMan();

public:
	//查询角色未领取奖励VIP等级
	int QueryRoleNotReceiveVIPReward(UINT32 roleId, BYTE vipLv, SQueryNotReceiveVIPRewardRes* pVIPLv);
	//记录VIP领取奖励
	void RecordRoleReceiveVIPReward(UINT32 roleId, BYTE vipLv);
	//是否领取过该VIP等级奖励
	BOOL IsReceivedVIPReward(UINT32 roleId, BYTE vipLv);
};

void InitVIPMan();
VIPMan* GetVIPMan();
void DestroyVIPMan();

#endif	//_VIPMan_h__