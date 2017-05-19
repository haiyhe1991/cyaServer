#ifndef __PROP_SELL_MAN_H__
#define __PROP_SELL_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
��Ʒ���۹���
*/

#include <vector>
#include "RoleInfos.h"

class PropSellMan
{
	PropSellMan(const PropSellMan&);
	PropSellMan& operator = (const PropSellMan&);

public:
	PropSellMan();
	~PropSellMan();

public:
	//��¼������Ϣ
	void InputPropSellRecord(UINT32 roleId, SPropSellInfo& sellInfo);
	//��ѯ������Ϣ
	int QueryPropSellRecord(UINT32 roleId, UINT32 from, BYTE num, SQueryRoleSellPropRcdRes* pSellRcdRes, BYTE maxNum);
	//��ȡ������Ϣ
	int GetPropSellInfo(UINT32 roleId, UINT32 sellId, SPropertyAttr& propAttr);
	//ɾ�����ۼ�¼
	int DelPropSellInfo(UINT32 roleId, UINT32 sellId);

	//��ȡ������Ϣ
	int GetPropSellInfo(UINT32 roleId, UINT32 sellId, SPropertyAttr& propAttr, std::vector<SPropSellInfo>& sellVec);
	//ɾ�����ۼ�¼
	int DelPropSellInfo(UINT32 roleId, UINT32 sellId, BYTE num, std::vector<SPropSellInfo>& sellVec);
};

void InitPropSellMan();
PropSellMan* GetPropSellMan();
void DestroyPropSellMan();

#endif	//_PropSellMan_h__