#include "cyaLog.h"

#include "RoomClient.h"
#include "RoomConfig.h"

CRoomClient* CRoomClient::m_this = NULL;

CRoomClient* CRoomClient::FetchRoomClientInstance()
{
	if (NULL == m_this)
		m_this = new CRoomClient();
	return m_this;
}

void CRoomClient::ReleaseRoomClientInstance()
{
	if (NULL != m_this)
	{
		delete m_this;
		m_this = NULL;
	}
}

CRoomClient::CRoomClient()
: m_pSessionMaker(NULL)
, m_pSessionServe(NULL)
, m_eRoomConStatus(RoomDisconnected)
, m_pfnStatusCallBcak(NULL)
, m_pStatusUserContent(NULL)
, m_pfnDataCallBack(NULL)
, m_pDataUserContent(NULL)
, m_iConnectTimer(ConnectTimerCallBack, this, 5000)
{

}

CRoomClient::~CRoomClient()
{
	RoomClientStop();

	m_eRoomConStatus = RoomDisconnected;
	m_pfnStatusCallBcak = NULL;
	m_pStatusUserContent = NULL;
	m_pfnDataCallBack = NULL;
	m_pDataUserContent = NULL;
}

INT CRoomClient::RoomClientStart()
{
	if (NULL == m_pSessionMaker)
		m_pSessionMaker = new CRoomSessionMaker();
	ASSERT(NULL != m_pSessionMaker);

	if (NULL == m_pSessionServe && NULL != m_pSessionMaker)
		CyaTcp_MakeServeMan(m_pSessionServe, m_pSessionMaker, 32);
	ASSERT(NULL != m_pSessionServe);

	if (NULL == m_pSessionMaker || NULL == m_pSessionServe)
		return DAEMON_ERROR;

	m_iConnectTimer.Start();

	return DAEMON_OK;
}

void CRoomClient::RoomClientStop()
{
	if (m_iConnectTimer.IsStarted())
	{
		m_iConnectTimer.Stop(TRUE);
	}
	if (m_pSessionServe != NULL)
	{
		//m_pSessionServe->StopServe(TRUE);
		m_pSessionServe->DeleteThis();
		m_pSessionServe = NULL;
	}
	if (m_pSessionMaker != NULL)
	{
		delete m_pSessionMaker;
		m_pSessionMaker = NULL;
	}
	if (m_pfnStatusCallBcak != NULL)
	{
		m_pfnStatusCallBcak(RoomDisconnected, m_pStatusUserContent);
	}
}

void CRoomClient::SetRoomClientConnectStatusCallBack(pfnRoomClientConnectStatusCallBack* pfnCallBack, void* pUserContent)
{
	ASSERT(NULL != pfnCallBack);
	m_pfnStatusCallBcak = pfnCallBack;
	m_pStatusUserContent = pUserContent;
}

void CRoomClient::SetRoomClientDataCallBack(pfnRoomClientDataCallBack* pfnCallBack, void* pUserContent)
{
	ASSERT(NULL != pfnCallBack);
	m_pfnDataCallBack = pfnCallBack;
	m_pDataUserContent = pUserContent;
}

void CRoomClient::ConnectRoomServer(DWORD dwConnectTimeout/* = 15000*/)
{
	CRoomConfig* pDaemonConfig = CRoomConfig::FetchConfigInstance();
	ASSERT(NULL != pDaemonConfig);

	SOCKET sockRoom = TCPNewClientByTime(pDaemonConfig->GetRoomServerAddr(), pDaemonConfig->GetRoomServerPort(), dwConnectTimeout);
	if (SockValid(sockRoom))
	{
		LogInfo(("连接RoomServer[IP:%s(%d),Port:%d]失败",
			pDaemonConfig->GetRoomServerStrAddr(),
			pDaemonConfig->GetRoomServerAddr(),
			pDaemonConfig->GetRoomServerPort()));
		return;
	}
	LogInfo(("连接RoomServer[IP:%s(%d),Port:%d]成功",
		pDaemonConfig->GetRoomServerStrAddr(),
		pDaemonConfig->GetRoomServerAddr(),
		pDaemonConfig->GetRoomServerPort()));
	/// 收取加密token
	/// 发送登陆信息
	/// 收取登陆回复
	/// 连接成功
	/// 回调

	pDaemonConfig->ReleaseConfigInstance();
}

void CRoomClient::ConnectTimerCallBack(void* pParam, TTimerID id)
{
	CRoomClient* pThis = (CRoomClient*)pParam;
	if (NULL != pThis)
	{
		pThis->OnCconnectWork();
	}
}

void CRoomClient::OnCconnectWork()
{
	if (NULL != m_pfnStatusCallBcak)
	{
		m_pfnStatusCallBcak(RoomConnected, m_pDataUserContent);
	}

	m_iConnectTimer.Stop(FALSE);
}
