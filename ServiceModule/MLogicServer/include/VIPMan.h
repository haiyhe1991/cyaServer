#ifndef __VIP_MAN_H__
#define __VIP_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
VIP����
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
	//��ѯ��ɫδ��ȡ����VIP�ȼ�
	int QueryRoleNotReceiveVIPReward(UINT32 roleId, BYTE vipLv, SQueryNotReceiveVIPRewardRes* pVIPLv);
	//��¼VIP��ȡ����
	void RecordRoleReceiveVIPReward(UINT32 roleId, BYTE vipLv);
	//�Ƿ���ȡ����VIP�ȼ�����
	BOOL IsReceivedVIPReward(UINT32 roleId, BYTE vipLv);
};

void InitVIPMan();
VIPMan* GetVIPMan();
void DestroyVIPMan();

#endif	//_VIPMan_h__