#include "GWServeMan.h"
#include "ConfigFileMan.h"

static GWServeMan* sg_gwServeMan = NULL;
void StartGWServe()
{
	ASSERT(sg_gwServeMan == NULL);
	sg_gwServeMan = new GWServeMan();
	ASSERT(sg_gwServeMan != NULL);
	sg_gwServeMan->Start();
}

void StopGWServe()
{
	GWServeMan* gwServeMan = sg_gwServeMan;
	sg_gwServeMan = NULL;
	if (gwServeMan != NULL)
	{
		gwServeMan->Stop();
		delete gwServeMan;
	}
}

GWServeMan::GWServeMan()
: m_serveMan(NULL)
{

}

GWServeMan::~GWServeMan()
{

}

void GWServeMan::Start()
{
	CyaTcp_MakeServeMan(m_serveMan, &m_sessionMaker);
	ASSERT(m_serveMan != NULL);
	m_serveMan->StartServe(IPPADDR(INADDR_ANY, GetConfigFileMan()->GetServePort()));
}

void GWServeMan::Stop()
{
	if (m_serveMan != NULL)
	{
		m_serveMan->StopServe();
		m_serveMan->DeleteThis();
		m_serveMan = NULL;
	}
}