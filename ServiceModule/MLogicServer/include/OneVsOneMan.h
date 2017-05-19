#ifndef __ONE_VS_ONE_MAN_H__
#define __ONE_VS_ONE_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
1v1副本管理
*/

#include "cyaTypes.h"
#include "ServiceMLogic.h"

class OneVsOneMan
{
public:
	// 每日清理
	void ClearEveryday();

	// 进入1v1副本
	int EnterOneVsOneInst(UINT32 userId, UINT32 roleId);

	// 查询1v1副本进入次数
	int QueryEnterOneVsOneNum(UINT32 userId, UINT32 roleId, SQuery1V1EnterNumRes *pEnterNum);

	// 购买1v1副本进入次数
	int BuyEnterOneVsOneNum(UINT32 userId, UINT32 roleId, int num, SBuy1V1EnterNumRes *pBuyEnterNum);
};

void InitOneVsOneMan();
OneVsOneMan* GetOneVsOneMan();
void DestroyOneVsOneMan();

#endif