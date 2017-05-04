#include "RelaySessionMan.h"
#include "GWSession.h"
#include "cyaLog.h"
#include "ConfigFileMan.h"

#define MAX_NO_HEARTBEAT_TIMES 3

static RelaySessionMan* sg_relaySessionMan = NULL;
static ConfigFileMan* sg_cfgMan = NULL;

void InitRelaySessionMan()
{
	ASSERT(sg_relaySessionMan == NULL);
	sg_relaySessionMan = new RelaySessionMan();
	ASSERT(sg_relaySessionMan != NULL);
}

RelaySessionMan* GetRelaySessionMan()
{
	return sg_relaySessionMan;
}

void DestroyRelaySessionMan()
{
	RelaySessionMan* relaySessionMan = sg_relaySessionMan;
	sg_relaySessionMan = NULL;
	if (relaySessionMan != NULL)
		delete relaySessionMan;

}

RelaySessionMan::RelaySessionMan()
{
	for (BYTE i = LINK_SERVER_1; i <= MAN_INRERFACE_SERVER; ++i)
	{
		RelaySessionSite* site = new RelaySessionSite();
		m_relaySessionsSiteMap.insert(std::make_pair(i, site));
	}
	if (GetConfigFileMan()->IsEnableHeartbeat())
		m_heartbeatTimer.Start(HeartbeatCheckTimer, this, GetConfigFileMan()->GetHeartbeatInterval());
}

RelaySessionMan::~RelaySessionMan()
{
	if (m_heartbeatTimer.IsStarted())
		m_heartbeatTimer.Stop();

	RelaySessionSiteMap::iterator it = m_relaySessionsSiteMap.begin();
	for (; it != m_relaySessionsSiteMap.end(); ++it)
	{
		RelaySessionSite* site = it->second;
		delete site;
	}
	m_relaySessionsSiteMap.clear();
}

int RelaySessionMan::AddRelaySession(BYTE serverId, UINT32 sessionId, GWSesssion* session)
{
	RelaySessionSite* pSite = GetRelaySessionSite(serverId);
	if (pSite == NULL)
		return GWS_UNKNOWN_SERVERID;
	return pSite->AddSession(sessionId, session);
}

void RelaySessionMan::DelRelaySession(BYTE serverId, UINT32 sessionId)
{
	RelaySessionSite* pSite = GetRelaySessionSite(serverId);
	if (pSite == NULL)
		return;
	return pSite->DelSession(sessionId);
}

int  RelaySessionMan::SendRelaySessionData(BYTE serverId, UINT32 sessionId, const void* data, int dataLen)
{
	RelaySessionSite* pSite = GetRelaySessionSite(serverId);
	if (pSite == NULL)
		return GWS_UNKNOWN_SERVERID;
	return pSite->SendSessionData(sessionId, data, dataLen);
}

int  RelaySessionMan::SendRelaySessionDataVec(BYTE serverId, UINT32 sessionId, const SockBufVec* bufs, int bufsCount)
{
	RelaySessionSite* pSite = GetRelaySessionSite(serverId);
	if (pSite == NULL)
		return GWS_UNKNOWN_SERVERID;
	return pSite->SendSessionDataVec(sessionId, bufs, bufsCount);
}

int  RelaySessionMan::SendLinkerGroupDataPacket(const SockBufVec* bufs, int bufsCount)
{
	RelaySessionSiteMap::iterator it = m_relaySessionsSiteMap.begin();
	for (; it != m_relaySessionsSiteMap.end(); ++it)
	{
		BYTE serverId = it->first;
		if (!IS_LINKER_SERVER(serverId))
			continue;

		if (it->second == NULL)
			continue;

		it->second->SendLinkerGroupDataPacket(bufs, bufsCount);
	}
	return GWS_OK;
}

void RelaySessionMan::HeartbeatCheckTimer(void* param, TTimerID id)
{
	RelaySessionMan* pThis = (RelaySessionMan*)param;
	ASSERT(pThis != NULL);
	pThis->OnHeartbeatCheck();
}

void RelaySessionMan::OnHeartbeatCheck()
{
	for (RelaySessionSiteMap::iterator it = m_relaySessionsSiteMap.begin(); it != m_relaySessionsSiteMap.end(); ++it)
	{
		RelaySessionSite* pSite = it->second;
		pSite->HeartbeatCheck();
	}
}

RelaySessionMan::RelaySessionSite::RelaySessionSite()
: m_nextCallSessionId(0)
{

}

RelaySessionMan::RelaySessionSite::~RelaySessionSite()
{

}

RelaySessionMan::RelaySessionSite* RelaySessionMan::GetRelaySessionSite(BYTE serverId)
{
	RelaySessionSite* pSite = NULL;
	RelaySessionSiteMap::iterator it = m_relaySessionsSiteMap.find(serverId);
	if (it != m_relaySessionsSiteMap.end())
		pSite = it->second;
	return pSite;
}

int RelaySessionMan::RelaySessionSite::AddSession(UINT32 sessionId, GWSesssion* session)
{
	CXTAutoLock lock(m_locker);
	RelaySessionMap::iterator it = m_relaySessionMap.find(sessionId);
	if (it != m_relaySessionMap.end())
	{
		//ASSERT(false);
		//return GWS_SESSIONID_IS_EXIST;
		GWSesssion* pSession = it->second;
		if (pSession == session)
			return GWS_OK;

		pSession->SetLinkerId(0);
		pSession->SetSessionId(0);
		it->second = session;
		pSession->GetServeMan()->CloseSession(pSession);
		return GWS_OK;
	}
	m_nextCallSessionId = sessionId;
	m_relaySessionMap.insert(std::make_pair(sessionId, session));
	return GWS_OK;
}

void RelaySessionMan::RelaySessionSite::DelSession(UINT32 sessionId)
{
	CXTAutoLock lock(m_locker);
	RelaySessionMap::iterator it = m_relaySessionMap.find(sessionId);
	if (it != m_relaySessionMap.end())
		m_relaySessionMap.erase(it);
}

int  RelaySessionMan::RelaySessionSite::SendSessionData(UINT32 sessionId, const void* data, int dataLen)
{
	if (m_relaySessionMap.empty())
		return GWS_SERVER_NOT_OPEN;

	GWSesssion* session = NULL;

	CXTAutoLock lock(m_locker);
	if (m_relaySessionMap.empty())
		return GWS_SERVER_NOT_OPEN;
	RelaySessionMap::iterator it = m_relaySessionMap.find(sessionId);
	if (it != m_relaySessionMap.end())
		session = it->second;
	else
	{
		it = m_relaySessionMap.find(m_nextCallSessionId);
		if (it != m_relaySessionMap.end())
		{
			session = it->second;
			++it;
		}
		else
		{
			it = m_relaySessionMap.begin();
			session = it->second;
			++it;
		}

		if (it != m_relaySessionMap.end())
			m_nextCallSessionId = it->first;
		else
			m_nextCallSessionId = m_relaySessionMap.begin()->first;
	}
	ASSERT(session != NULL);
	return session->GetServeMan()->SendSessionData(session, data, dataLen);
}

int  RelaySessionMan::RelaySessionSite::SendSessionDataVec(UINT32 sessionId, const SockBufVec* bufs, int bufsCount)
{
	if (m_relaySessionMap.empty())
		return GWS_SERVER_NOT_OPEN;

	GWSesssion* session = NULL;

	CXTAutoLock lock(m_locker);
	if (m_relaySessionMap.empty())
		return GWS_SERVER_NOT_OPEN;
	RelaySessionMap::iterator it = m_relaySessionMap.find(sessionId);
	if (it != m_relaySessionMap.end())
		session = it->second;
	else
	{
		it = m_relaySessionMap.find(m_nextCallSessionId);
		if (it != m_relaySessionMap.end())
		{
			session = it->second;
			++it;
		}
		else
		{
			it = m_relaySessionMap.begin();
			session = it->second;
			++it;
		}

		if (it != m_relaySessionMap.end())
			m_nextCallSessionId = it->first;
		else
		{
			it = m_relaySessionMap.begin();
			m_nextCallSessionId = it->first;
		}
	}
	ASSERT(session != NULL);
	return session->GetServeMan()->SendSessionDataVec(session, bufs, bufsCount);
}

int  RelaySessionMan::RelaySessionSite::SendLinkerGroupDataPacket(const SockBufVec* bufs, int bufsCount)
{
	for (RelaySessionMap::iterator it = m_relaySessionMap.begin(); it != m_relaySessionMap.end(); ++it)
	{
		GWSesssion* session = it->second;
		if (session == NULL)
			continue;

		int r = session->GetServeMan()->SendSessionDataVec(session, bufs, bufsCount);
		if (r == 0)
			break;
	}

	return GWS_OK;
}


void RelaySessionMan::RelaySessionSite::HeartbeatCheck()
{
	if (m_relaySessionMap.empty())
		return;

	DWORD timerPeriod = GetConfigFileMan()->GetHeartbeatInterval() * MAX_NO_HEARTBEAT_TIMES;
	DWORD tickNow = GetTickCount();

	CXTAutoLock lock(m_locker);
	if (m_relaySessionMap.empty())
		return;

	for (RelaySessionMap::iterator it = m_relaySessionMap.begin(); it != m_relaySessionMap.end();)
	{
		GWSesssion* session = it->second;
		ASSERT(session != NULL);
		DWORD lastTick = session->GetLastHeartbeatTick();
		int interval = tickNow - lastTick;
		if (lastTick > 0 && interval > timerPeriod)
		{
			BYTE linkerId = session->GetLinkerId();
			LogInfo(("%u秒内未收到[%d:%u->%s]心跳信息,将其连接断开", timerPeriod / 1000, linkerId, it->first, GetServerTypeStr(linkerId)));
			session->SetLinkerId(0);
			session->SetSessionId(0);
			m_relaySessionMap.erase(it++);
			session->GetServeMan()->CloseSession(session);
		}
		else
			++it;
	}
}