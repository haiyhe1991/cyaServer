#include "cyaLocker.h"
#include "DBGlobalObjMan.h"

static CXTLocker sg_locker;
static DBGlobalObjMan* sg_globalObjMan = NULL;

DBGlobalObjMan::DBGlobalObjMan()
{
	CyaTcp_MakeServeMan(m_dbsSessionMan, &m_dbsSessionMaker);
	ASSERT(m_dbsSessionMan != NULL);
}

DBGlobalObjMan::~DBGlobalObjMan()
{
	if (m_dbsSessionMan != NULL)
	{
		m_dbsSessionMan->DeleteThis();
		m_dbsSessionMan = NULL;
	}
}

void InitDBGlobalObjMan()
{
	if (sg_globalObjMan != NULL)
		return;

	CXTAutoLock lock(sg_locker);
	if (sg_globalObjMan != NULL)
		return;
	DBGlobalObjMan* p = new DBGlobalObjMan();
	ASSERT(p != NULL);
	sg_globalObjMan = p;
}

void DestroyDBGlobalObjMan()
{
	if (sg_globalObjMan == NULL)
		return;

	CXTAutoLock lock(sg_locker);
	if (sg_globalObjMan == NULL)
		return;
	delete sg_globalObjMan;
	sg_globalObjMan = NULL;
}

DBGlobalObjMan* GetDBGlobalObjMan()
{
	return sg_globalObjMan;
}
