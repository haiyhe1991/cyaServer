#include "cyaLocker.h"
#include "GWGlobalObjMan.h"

static CXTLocker sg_locker;
static GWGlobalObjMan* sg_globalObjMan = NULL;

GWGlobalObjMan::GWGlobalObjMan()
{
	CyaTcp_MakeServeMan(m_gwsSessionMan, &m_gwsSessionMaker);
	ASSERT(m_gwsSessionMan != NULL);
}

GWGlobalObjMan::~GWGlobalObjMan()
{
	if (m_gwsSessionMan != NULL)
	{
		m_gwsSessionMan->DeleteThis();
		m_gwsSessionMan = NULL;
	}
}

void InitGWGlobalObjMan()
{
	if (sg_globalObjMan != NULL)
		return;

	CXTAutoLock lock(sg_locker);
	if (sg_globalObjMan != NULL)
		return;
	GWGlobalObjMan* p = new GWGlobalObjMan();
	ASSERT(p != NULL);
	sg_globalObjMan = p;
}

void DestroyGWGlobalObjMan()
{
	if (sg_globalObjMan == NULL)
		return;

	CXTAutoLock lock(sg_locker);
	if (sg_globalObjMan == NULL)
		return;
	delete sg_globalObjMan;
	sg_globalObjMan = NULL;
}

GWGlobalObjMan* GetGWGlobalObjMan()
{
	return sg_globalObjMan;
}