#include "LicenceServeMan.h"
#include "ConfigFileMan.h"

static LicenceServeMan* sg_lcServeMan = NULL;
void StartLicenceServe()
{
	ASSERT(sg_lcServeMan == NULL);
	sg_lcServeMan = new LicenceServeMan();
	ASSERT(sg_lcServeMan != NULL);
	sg_lcServeMan->Start();
}

void StopLicenceServe()
{
	LicenceServeMan* lcServeMan = sg_lcServeMan;
	sg_lcServeMan = NULL;
	if (lcServeMan != NULL)
	{
		lcServeMan->Stop();
		delete lcServeMan;
	}
}

void DestroyLicenceServe()
{

}

LicenceServeMan* GetLicenceServeMan()
{
	return sg_lcServeMan;
}

LicenceServeMan::LicenceServeMan()
	: m_serveMan(NULL)
{

}

LicenceServeMan::~LicenceServeMan()
{

}

void LicenceServeMan::Start()
{
	CyaTcp_MakeServeMan(m_serveMan, &m_sessionMaker, 64 * 1024, GetConfigFileMan()->GetMaxUserOnlineTime() / 1000);
	ASSERT(m_serveMan != NULL);
	m_serveMan->SetMaxSessionsPerIoThread(2048);
	m_serveMan->StartServe(IPPADDR(INADDR_ANY, GetConfigFileMan()->GetServePort()));
}

void LicenceServeMan::Stop()
{
	if (m_serveMan != NULL)
	{
		m_serveMan->StopServe();
		m_serveMan->DeleteThis();
		m_serveMan = NULL;
	}
}
