#ifndef __GUARD_ICON_MAN_H__
#define __GUARD_ICON_MAN_H__

#include "ServiceMLogic.h"

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
守卫圣像
*/

class GuardIconMan
{
public:
	GuardIconMan();
	~GuardIconMan();

	// 进入守卫圣像副本
	int EnterGuardIconManInst(UINT32 userId, UINT32 roleId);
};

void InitGuardIconMan();
GuardIconMan* GetGuardIconMan();
void DestroyGuardIconMan();

#endif