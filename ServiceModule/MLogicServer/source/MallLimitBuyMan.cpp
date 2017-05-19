#include "MallLimitBuyMan.h"

static MallLimitBuyMan* sg_pLimitBuyMan = NULL;
void InitMallLimitBuyMan()
{
	ASSERT(sg_pLimitBuyMan == NULL);
	sg_pLimitBuyMan = new MallLimitBuyMan();
	ASSERT(sg_pLimitBuyMan != NULL);
}

MallLimitBuyMan* GetMallLimitBuyMan()
{
	return sg_pLimitBuyMan;
}

void DestroyMallLimitBuyMan()
{
	MallLimitBuyMan* pLimitBuyMan = sg_pLimitBuyMan;
	sg_pLimitBuyMan = NULL;
	if (pLimitBuyMan)
		delete pLimitBuyMan;
}

MallLimitBuyMan::MallLimitBuyMan()
{

}

MallLimitBuyMan::~MallLimitBuyMan()
{

}