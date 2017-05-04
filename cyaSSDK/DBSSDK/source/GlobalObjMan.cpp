#include "cyaLocker.h"
#include "GlobalObjMan.h"

static CXTLocker sg_locker;
static GlobalObjMan* sg_globalObjMan = NULL;

GlobalObjMan::GlobalObjMan()
{
	CyaTcp_MakeServeMan(m_dbsSessionMan, &m_dbsSessionMaker);
	ASSERT(m_dbsSessionMan != NULL);
}

GlobalObjMan::~GlobalObjMan()
{
	if (m_dbsSessionMan != NULL)
	{
		m_dbsSessionMan->DeleteThis();
		m_dbsSessionMan = NULL;
	}
}

void InitGlobalObjMan()
{
	if (sg_globalObjMan != NULL)
		return;

	CXTAutoLock lock(sg_locker);
	if (sg_globalObjMan != NULL)
		return;
	GlobalObjMan* p = new GlobalObjMan();
	ASSERT(p != NULL);
	sg_globalObjMan = p;
}

void DestroyGlobalObjMan()
{
	if (sg_globalObjMan == NULL)
		return;

	CXTAutoLock lock(sg_locker);
	if (sg_globalObjMan == NULL)
		return;
	delete sg_globalObjMan;
	sg_globalObjMan = NULL;
}

GlobalObjMan* GetGlobalObjMan()
{
	return sg_globalObjMan;
}
