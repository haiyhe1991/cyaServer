#include "MauGWSClientMan.h"
#include "ConfigFileMan.h"
#include "cyaLog.h"

static MauGWSClientMan* sg_gwsClientMan = NULL;
int InitGWSClientMan()
{
	ASSERT(sg_gwsClientMan == NULL);
	sg_gwsClientMan = new MauGWSClientMan();
	ASSERT(sg_gwsClientMan != NULL);
	return sg_gwsClientMan->Init();
}

void DestroyGWSClientMan()
{
	MauGWSClientMan* man = sg_gwsClientMan;
	sg_gwsClientMan = NULL;
	if (man != NULL)
	{
		man->Cleanup();
		delete man;
	}
}

MauGWSClientMan::MauGWSClientMan()
{
	GWS_Init();
}

MauGWSClientMan::~MauGWSClientMan()
{
	GWS_Cleanup();
}

int MauGWSClientMan::Init()
{
	int conns = GetConfigFileMan()->GetInitGWConnections();
	for (int i = 0; i < conns; ++i)
	{
		MauGWSClient* client = new MauGWSClient();
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

void MauGWSClientMan::Cleanup()
{
	int conns = (int)m_gwsClientVec.size();
	for (int i = 0; i < conns; ++i)
	{
		MauGWSClient* client = m_gwsClientVec[i];
		if (client != NULL)
		{
			client->Logout();
			delete client;
		}
	}
	m_gwsClientVec.clear();
}