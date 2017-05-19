#include "GWSClientMan.h"
#include "ConfigFileMan.h"
#include "cyaLog.h"

static GWSClientMan* sg_gwsClientMan = NULL;
int InitGWSClientMan()
{
	ASSERT(sg_gwsClientMan == NULL);
	sg_gwsClientMan = new GWSClientMan();
	ASSERT(sg_gwsClientMan != NULL);
	return sg_gwsClientMan->Init();
}

GWSClientMan* GetGWSClientMan()
{
	return sg_gwsClientMan;
}

void DestroyGWSClientMan()
{
	GWSClientMan* gwsMan = sg_gwsClientMan;
	sg_gwsClientMan = NULL;
	if (gwsMan != NULL)
	{
		gwsMan->Cleanup();
		delete gwsMan;
	}
}

GWSClientMan::GWSClientMan()
{
	GWS_Init();
}

GWSClientMan::~GWSClientMan()
{
	GWS_Cleanup();
}

int GWSClientMan::Init()
{
	int conns = GetConfigFileMan()->GetInitGWConnections();
	for (int i = 0; i < conns; ++i)
	{
		GWSClient* client = new GWSClient();
		int r = client->Login();
		if (r != GWS_OK)
		{
			delete client;
			return r;
		}
		m_gwsClientVec.push_back(client);
	}
	LogInfo(("登陆网关服务器[%s:%d]成功!", GetConfigFileMan()->GetGwsIp(), GetConfigFileMan()->GetGwsPort()));
	return MLS_OK;
}

void GWSClientMan::Cleanup()
{
	int conns = (int)m_gwsClientVec.size();
	for (int i = 0; i < conns; ++i)
	{
		GWSClient* client = m_gwsClientVec[i];
		if (client != NULL)
		{
			client->Logout();
			delete client;
		}
	}
	m_gwsClientVec.clear();
}

GWSClient* GWSClientMan::RandGWSClient()
{
	int conns = (int)m_gwsClientVec.size();
	if (conns <= 0)
		return NULL;

	int n = rand() % conns;
	return m_gwsClientVec[n];
}

GWSClient* GWSClientMan::GetFristGWSClient()
{
	return m_gwsClientVec.empty() ? NULL : m_gwsClientVec[0];
}