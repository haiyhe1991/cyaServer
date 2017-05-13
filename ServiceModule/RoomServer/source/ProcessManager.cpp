#include "ProcessManager.h"
#include "ServiceRoomDaemon.h"
#include "DaemonSession.h"

CProcessManager* CProcessManager::m_this = NULL;

CProcessManager* CProcessManager::FetchProcessManagerInstance()
{
	if (NULL == m_this)
	{
		m_this = new CProcessManager();
	}

	return m_this;
}

void CProcessManager::ReleaseProcessManagerInstance()
{
	if (NULL != m_this)
	{
		delete m_this;
		m_this = NULL;
	}
}

CProcessManager::CProcessManager()
{

}

CProcessManager::~CProcessManager()
{
	DaemonProcessReg::iterator itr = m_procReg.begin();
	for (itr; itr != m_procReg.end();)
	{
		m_procReg.erase(itr++);
	}
}

INT  CProcessManager::ProcessRegist(OSProcessID id, void* session)
{
	DaemonProcessReg::iterator itr = m_procReg.find(id);
	if (m_procReg.end() == itr)
	{
		m_procReg.insert(std::make_pair(id, session));
	}
	return DAEMON_OK;
}

void CProcessManager::ProcessUnregist(OSProcessID id)
{
	DaemonProcessReg::iterator itr = m_procReg.find(id);
	if (m_procReg.end() != itr)
	{
		m_procReg.erase(itr);
	}
}

void* CProcessManager::GetProcessSession(OSProcessID id) const
{
	DaemonProcessReg::const_iterator itr = m_procReg.find(id);
	if (m_procReg.end() != itr)
		return itr->second;
	return NULL;
}

INT CProcessManager::SendGWData(const void* data, const INT& dataLen, UINT16 cmdCode) const
{
	DaemonProcessReg::const_iterator itr = m_procReg.begin();
	for (itr; m_procReg.end() != itr; ++itr)
	{
		CDaemonSession* pSession = (CDaemonSession*)(itr->second);
		if (NULL != pSession)
		{
			pSession->ResponseSessionMsg(data, dataLen, cmdCode);
		}
	}
	return DAEMON_OK;
}