#include "SqlMan.h"
#include "cyaLog.h"

#define DEFUALT_WAIT_TIMEOUT 28800	/// Ä¬ÈÏ³¬Ê±Ãë

DBSession::DBSession(UINT16 MinKeep/* =5 */, UINT16 MaxLink/* =12 */)
	: m_MinKeep(MinKeep)
	, m_MaxLink(MaxLink)
	, m_insertSession(NULL)
	, m_pingTimer(PingTimerCallBack, this, DEFUALT_WAIT_TIMEOUT * 1000 / 3)
{

}

DBSession::~DBSession()
{
	if (m_pingTimer.IsStarted())
	{
		m_pingTimer.Stop();
	}
	DBS_Close();
}

UINT16 DBSession::DBS_Connect(const char* dbHost, const char* dbUser, const char* dbPwd, const char* dbName, UINT dbPort/* = 3306*/)
{
	CXTAutoLock locke(m_handleLocker);
	int wait_timeout = DEFUALT_WAIT_TIMEOUT;
	if (NULL == m_insertSession)
	{
		m_insertSession = new SqlSingle();
	}
	if (m_insertSession)
	{
		m_insertSession->SqlConnect(dbHost, dbUser, dbPwd, dbName, dbPort);
		wait_timeout = m_insertSession->GetWaitTimeout();
		if (wait_timeout<DEFUALT_WAIT_TIMEOUT)
		{
			m_insertSession->SetWaitTimeout();
			m_pingTimer.SetPeriod(DEFUALT_WAIT_TIMEOUT * 1000 / 3);
		}
		else
		{
			m_pingTimer.SetPeriod(wait_timeout / 3);
		}
	}

	while (m_MinKeep>m_sessions.size())
	{
		SqlSingle* sit = new SqlSingle();
		if (sit)
		{
			if (0 == sit->SqlConnect(dbHost, dbUser, dbPwd, dbName, dbPort))
			{
				if (wait_timeout<DEFUALT_WAIT_TIMEOUT)
					sit->SetWaitTimeout();
				m_sessions.push_back(sit);
			}
		}
	}

	m_pingTimer.Start();

	return m_sessions.size();
}

void DBSession::DBS_Close()
{
	CXTAutoLock locke(m_handleLocker);
	if (NULL != m_insertSession)
	{
		delete m_insertSession;
		m_insertSession = NULL;
	}
	SESSION_VEC::iterator itr = m_sessions.begin();
	for (itr; m_sessions.end() != itr; ++itr)
	{
		(*itr)->SqlClose();
		delete (*itr);
	}
	m_sessions.clear();
}

DBSessionHandle DBSession::FetchDBSessionHandle()
{
	CXTAutoLock locke(m_handleLocker);
	srand((unsigned)time(NULL));
	int flag = rand() % m_MinKeep;

	return static_cast<SqlSingle*>(m_sessions[flag]);
}

DBSessionHandle DBSession::FetchDBSessionInsertHandle()
{
	CXTAutoLock locke(m_handleLocker);
	return static_cast<SqlSingle*>(m_insertSession);
}

void DBSession::PingTimerCallBack(void* param, TTimerID id)
{
	DBSession* pThis = (DBSession*)param;
	if (pThis)
		pThis->OnTimer();
}

void DBSession::OnTimer()
{
	SESSION_VEC::iterator itr = m_sessions.begin();
	for (itr; m_sessions.end() != itr; ++itr)
	{
		if (0 != (*itr)->SessionPing())
		{
			LogDebug(("Ping Error"));
		}
	}
}

