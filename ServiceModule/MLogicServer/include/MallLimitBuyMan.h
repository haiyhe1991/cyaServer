#ifndef __MALL_LIMIT_BUY_MAN_H__
#define __MALL_LIMIT_BUY_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
商城限购管理
*/

#include "cyaCoreTimer.h"

class MallLimitBuyMan
{
	MallLimitBuyMan(const MallLimitBuyMan&);
	MallLimitBuyMan& operator = (const MallLimitBuyMan&);

public:
	MallLimitBuyMan();
	~MallLimitBuyMan();
};

void InitMallLimitBuyMan();
MallLimitBuyMan* GetMallLimitBuyMan();
void DestroyMallLimitBuyMan();

#endif	//_MallLimitBuyMan_h__