#ifndef __ONE_VS_ONE_MAN_H__
#define __ONE_VS_ONE_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
1v1��������
*/

#include "cyaTypes.h"
#include "ServiceMLogic.h"

class OneVsOneMan
{
public:
	// ÿ������
	void ClearEveryday();

	// ����1v1����
	int EnterOneVsOneInst(UINT32 userId, UINT32 roleId);

	// ��ѯ1v1�����������
	int QueryEnterOneVsOneNum(UINT32 userId, UINT32 roleId, SQuery1V1EnterNumRes *pEnterNum);

	// ����1v1�����������
	int BuyEnterOneVsOneNum(UINT32 userId, UINT32 roleId, int num, SBuy1V1EnterNumRes *pBuyEnterNum);
};

void InitOneVsOneMan();
OneVsOneMan* GetOneVsOneMan();
void DestroyOneVsOneMan();

#endif