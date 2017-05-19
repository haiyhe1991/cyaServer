#ifndef __ONLINE_SINGN_IN_MAN_H__
#define __ONLINE_SINGN_IN_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
上线签到
*/

#include <list>
#include <vector>
#include "cyaTypes.h"

class OnlineSingnInMan
{
public:
	OnlineSingnInMan();
	~OnlineSingnInMan();

	//查询角色签到
	int QueryRoleSignIn(UINT32 roleId, std::vector<LTMSEL>& signVec);
	//刷新签到信息
	void RefreshSignInInfo(UINT32 userId, UINT32 roleId);
};

void InitOnlineSingnInMan();
OnlineSingnInMan* GetOnlineSingnInMan();
void DestroyOnlineSingnInMan();

#endif	//_OnlineSingnInMan_h__