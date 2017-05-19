#ifndef __ONLINE_SINGN_IN_MAN_H__
#define __ONLINE_SINGN_IN_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
����ǩ��
*/

#include <list>
#include <vector>
#include "cyaTypes.h"

class OnlineSingnInMan
{
public:
	OnlineSingnInMan();
	~OnlineSingnInMan();

	//��ѯ��ɫǩ��
	int QueryRoleSignIn(UINT32 roleId, std::vector<LTMSEL>& signVec);
	//ˢ��ǩ����Ϣ
	void RefreshSignInInfo(UINT32 userId, UINT32 roleId);
};

void InitOnlineSingnInMan();
OnlineSingnInMan* GetOnlineSingnInMan();
void DestroyOnlineSingnInMan();

#endif	//_OnlineSingnInMan_h__