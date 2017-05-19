#ifndef __PROP_SELL_MAN_H__
#define __PROP_SELL_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
物品出售管理
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
	//记录出售信息
	void InputPropSellRecord(UINT32 roleId, SPropSellInfo& sellInfo);
	//查询出售信息
	int QueryPropSellRecord(UINT32 roleId, UINT32 from, BYTE num, SQueryRoleSellPropRcdRes* pSellRcdRes, BYTE maxNum);
	//获取出售信息
	int GetPropSellInfo(UINT32 roleId, UINT32 sellId, SPropertyAttr& propAttr);
	//删除出售记录
	int DelPropSellInfo(UINT32 roleId, UINT32 sellId);

	//获取出售信息
	int GetPropSellInfo(UINT32 roleId, UINT32 sellId, SPropertyAttr& propAttr, std::vector<SPropSellInfo>& sellVec);
	//删除出售记录
	int DelPropSellInfo(UINT32 roleId, UINT32 sellId, BYTE num, std::vector<SPropSellInfo>& sellVec);
};

void InitPropSellMan();
PropSellMan* GetPropSellMan();
void DestroyPropSellMan();

#endif	//_PropSellMan_h__