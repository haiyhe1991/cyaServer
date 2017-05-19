#ifndef __ACTIVITY_INST_MAN_H__
#define __ACTIVITY_INST_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
活动副本管理
*/

#include <vector>
#include "cyaTypes.h"
#include "ServiceMLogic.h"

class ActitvityInstMan
{
public:
	ActitvityInstMan();
	~ActitvityInstMan();

	// 查询拥有的活动副本类型
	int QueryHasActitvityInstType(UINT32 userId, UINT32 roleId, SQueryHasActivityInstTypeRes *pHasActivityInstType);

	// 进入活动副本
	int EnterActitvityInst(UINT32 userId, UINT32 roleId, UINT32 instId);
};

void InitActitvityInstMan();
ActitvityInstMan* GetActitvityInstMan();
void DestroyActitvityInstMan();

#endif