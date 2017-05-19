#include "ShoppingMallMan.h"

static ShoppingMallMan* sg_mallMan = NULL;
void InitShoppingMallMan()
{
	ASSERT(sg_mallMan == NULL);
	sg_mallMan = new ShoppingMallMan();
	ASSERT(sg_mallMan != NULL);
}

ShoppingMallMan* GetShoppingMallMan()
{
	return sg_mallMan;
}

void DestroyShoppingMallMan()
{
	ShoppingMallMan* mallMan = sg_mallMan;
	sg_mallMan = NULL;
	if (mallMan)
		delete mallMan;
}

ShoppingMallMan::ShoppingMallMan()
{

}

ShoppingMallMan::~ShoppingMallMan()
{

}