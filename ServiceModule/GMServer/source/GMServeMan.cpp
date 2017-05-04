#include "GMServeMan.h"

CGMServeMan* g_cgmserveManObj = NULL;
CGMServeMan::CGMServeMan()
{
	CyaTcp_MakeServeMan(m_gmserveMan, &m_gmsessionMaker);
	ASSERT(m_gmserveMan != NULL);
}


CGMServeMan::~CGMServeMan()
{
	if (m_gmserveMan != NULL)
	{
		m_gmserveMan->DeleteThis();
	}
}


void InitGMServeManObj()
{
	if (g_cgmserveManObj != NULL)
		return;
	g_cgmserveManObj = new CGMServeMan();
	ASSERT(g_cgmserveManObj != NULL);
}


CGMServeMan* GetGMServeManObj()
{
	return g_cgmserveManObj;
}

void DestroyGMServeManObj()
{
	if (g_cgmserveManObj == NULL)
		return;
	delete g_cgmserveManObj;
	g_cgmserveManObj = NULL;
}
