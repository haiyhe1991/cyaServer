#include "cyaLog.h"
#include "RankGWClientMan.h"
#include "RankConfigFileMan.h"

static RankGWClientMan* sg_gwsRankClientMan = NULL;
int InitRankGWClientMan()
{
	ASSERT(sg_gwsRankClientMan == NULL);
	sg_gwsRankClientMan = new RankGWClientMan();
	ASSERT(sg_gwsRankClientMan != NULL);
	return sg_gwsRankClientMan->Init();
}

void DestroyRankGWClientMan()
{
	RankGWClientMan* man = sg_gwsRankClientMan;
	sg_gwsRankClientMan = NULL;
	if (man != NULL)
	{
		man->Cleanup();
		delete man;
	}
}

RankGWClientMan::RankGWClientMan()
{
	GWS_Init();
}

RankGWClientMan::~RankGWClientMan()
{
	GWS_Cleanup();
}

int RankGWClientMan::Init()
{
	int conns = GetRankConfigFileMan()->GetInitGWConnections();
	for (int i = 0; i < conns; ++i)
	{
		RankGWClient* client = new RankGWClient();
		int r = client->Login();
		if (r != GWS_OK)
		{
			delete client;
			return r;
		}
		m_gwsClientVec.push_back(client);
	}
	LogDebug(("登陆网关服务器[%s:%d]成功!", GetRankConfigFileMan()->GetGwsIp(), GetRankConfigFileMan()->GetGwsPort()));
	return MLS_OK;
}

void RankGWClientMan::Cleanup()
{
	int conns = (int)m_gwsClientVec.size();
	for (int i = 0; i < conns; ++i)
	{
		RankGWClient* client = m_gwsClientVec[i];
		if (client != NULL)
		{
			client->Logout();
			delete client;
		}
	}
	m_gwsClientVec.clear();
}
