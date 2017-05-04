#include "GMGlobalObjMan.h"
//#include "GMSdkSession.h"


GMGlobalObjMan* g_gmsdkGlobalObjMan = NULL;


GMGlobalObjMan::GMGlobalObjMan()
{
	CyaTcp_MakeServeMan(m_serveMan, &m_gmsdkSessionMaker);
	ASSERT(m_serveMan != NULL);
}

GMGlobalObjMan::~GMGlobalObjMan()
{
	if (m_serveMan != NULL)
	{
		m_serveMan->DeleteThis();
		m_serveMan = NULL;
	}
}


void InitGMGlobalObjMan()
{
	if (g_gmsdkGlobalObjMan != NULL)
	{
		printf("g_gm_sdkGlobalObjMan失败!"); return;
	}
	//创建会话构造;
	g_gmsdkGlobalObjMan = new GMGlobalObjMan();
	ASSERT(g_gmsdkGlobalObjMan != NULL);
}

GMGlobalObjMan* GetGMGlobalObjMan()
{
	return g_gmsdkGlobalObjMan;
}

void DestroyGMGlobalObjMan()
{
	if (g_gmsdkGlobalObjMan == NULL)
		return;

	delete g_gmsdkGlobalObjMan;
	g_gmsdkGlobalObjMan = NULL;

}