#include "LinkServer.h"
#include "cyaIpCvt.h"
#include "cyaLog.h"
#include "LinkGlobalObj.h"
#include "LinkConfig.h"

#define LINK_SERVER_NAME    "LinkServer"

LinkServer::LinkServer()
	: m_maker(NULL)
	, m_man(NULL)
	, m_started(false)
	, m_proc(NULL)
	, m_tokenman(NULL)
{

}

LinkServer::~LinkServer()
{
	UnitLinkServer();
	m_started = false;
}

int  LinkServer::InitLinkServer()
{
	if (!ValidMaker())
		m_maker = new SessionMaker();
	if (ValidMaker() && !ValidMan())
	{
		int heartBeatClock = GlobalGetLinkConfig()->GetClientHeartBeatTimeout() * 3;
		CyaTcp_MakeServeMan(m_man, m_maker, MAX_LINKER_CLIENT_NUM, heartBeatClock);
	}
	if (ValidMaker() && ValidMan())
	{
		m_proc = MsgProcess::FetchMsgProcess();
		ASSERT(NULL != m_proc);
		m_tokenman = TokenMan::FetchTokenMan();
		ASSERT(NULL != m_tokenman);
		LogInfo(("初始化%s成功", LINK_SERVER_NAME));
		return LINK_OK;
	}
	LogInfo(("初始化%s失败", LINK_SERVER_NAME));
	return LINK_ERROR;
}

void LinkServer::UnitLinkServer()
{
	if (ValidMan())
	{
		m_man->DeleteThis();
		m_man = NULL;
	}
	if (ValidMaker())
	{
		delete m_maker;
		m_maker = NULL;
	}
	if (m_proc)
	{
		m_proc->DeleteThis();
		m_proc = NULL;
	}
	if (m_tokenman)
	{
		m_tokenman->DeleteThis();
		m_tokenman = NULL;
	}
	LogInfo(("反始化%s", LINK_SERVER_NAME));
}

int  LinkServer::StartLinkServer()
{
	if (ValidMan() && !m_started)
	{
		DWORD ip = ADDR_ANY;
		if (!(GlobalGetLinkConfig()->GetLinkInvalidIP()))
			ip = GetDWordIP(GlobalGetLinkConfig()->GetLinkAddr());

		IPPADDR adr(ip, GlobalGetLinkConfig()->GetLinkListenPort());
		int ret = m_man->StartServe(adr, true);
		LogInfo(("%s( %s:%d )启动%s",
			LINK_SERVER_NAME,
			GlobalGetLinkConfig()->GetLinkAddr(),
			GlobalGetLinkConfig()->GetLinkListenPort(),
			0 == ret ? "成功" : "失败"
			));
		m_started = (ret == 0 ? true : false);
		return ret == 0 ? 0 : LINK_START_SERVICE;
	}
	LogInfo(("%s( %s:%d )启动失败。《SessionMan无效或者已经启动》",
		LINK_SERVER_NAME,
		GlobalGetLinkConfig()->GetLinkAddr(),
		GlobalGetLinkConfig()->GetLinkListenPort()
		));
	return LINK_ERROR;
}

void LinkServer::StopLinkServer()
{
	if (ValidMan() && m_started)
	{
		m_man->StopServe();
		m_started = false;
		LogInfo(("停止%s成功", LINK_SERVER_NAME));
	}
	LogInfo(("停止%s失败", LINK_SERVER_NAME));
}

bool LinkServer::ServerStarted() const
{
	return m_started;
}

int LinkServer::GetSessionsCount()
{
	if (ValidMan())
		return m_man->GetSessionsCount();
	return 0;
}

bool LinkServer::ValidMaker()
{
	return NULL != m_maker;
}

bool LinkServer::ValidMan()
{
	return NULL != m_man;
}
