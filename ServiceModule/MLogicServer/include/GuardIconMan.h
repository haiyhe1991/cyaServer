#ifndef __GUARD_ICON_MAN_H__
#define __GUARD_ICON_MAN_H__

#include "ServiceMLogic.h"

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
����ʥ��
*/

class GuardIconMan
{
public:
	GuardIconMan();
	~GuardIconMan();

	// ��������ʥ�񸱱�
	int EnterGuardIconManInst(UINT32 userId, UINT32 roleId);
};

void InitGuardIconMan();
GuardIconMan* GetGuardIconMan();
void DestroyGuardIconMan();

#endif