#ifndef __UNLOCK_CONTENT_MAN_H__
#define __UNLOCK_CONTENT_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
��������
*/

#include "cyaTypes.h"
#include "ServiceMLogic.h"

class UnlockContentMan
{
	UnlockContentMan(const UnlockContentMan&);
	UnlockContentMan& operator = (const UnlockContentMan&);

public:
	UnlockContentMan();
	~UnlockContentMan();

	//��ѯ�ѽ�������
	int QueryUnlockContent(UINT32 userId, UINT32 roleId, BYTE type, SQueryUnlockContentRes* pUnlockRes);
	//��������
	int UnlockContent(UINT32 userId, UINT32 roleId, BYTE type, UINT16 unlockId);
};

void InitUnlockContentMan();
UnlockContentMan* GetUnlockContentMan();
void DestroyUnlockContentMan();

#endif	//_UnlockContentMan_h__