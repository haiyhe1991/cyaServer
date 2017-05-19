#ifndef __ACTIVITY_INST_MAN_H__
#define __ACTIVITY_INST_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
���������
*/

#include <vector>
#include "cyaTypes.h"
#include "ServiceMLogic.h"

class ActitvityInstMan
{
public:
	ActitvityInstMan();
	~ActitvityInstMan();

	// ��ѯӵ�еĻ��������
	int QueryHasActitvityInstType(UINT32 userId, UINT32 roleId, SQueryHasActivityInstTypeRes *pHasActivityInstType);

	// ��������
	int EnterActitvityInst(UINT32 userId, UINT32 roleId, UINT32 instId);
};

void InitActitvityInstMan();
ActitvityInstMan* GetActitvityInstMan();
void DestroyActitvityInstMan();

#endif