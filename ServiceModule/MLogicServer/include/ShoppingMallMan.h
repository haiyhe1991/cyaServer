#ifndef __SHOPPING_MALL_MAN_H__
#define __SHOPPING_MALL_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
商城管理
*/

#include "cyaTypes.h"

class ShoppingMallMan
{
	ShoppingMallMan(const ShoppingMallMan&);
	ShoppingMallMan& operator = (const ShoppingMallMan&);

public:
	ShoppingMallMan();
	~ShoppingMallMan();
};

void InitShoppingMallMan();
ShoppingMallMan* GetShoppingMallMan();
void DestroyShoppingMallMan();

#endif