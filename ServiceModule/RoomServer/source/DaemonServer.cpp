#include "cyaIpCvt.h"

#include "DaemonServer.h"
#include "RoomClient.h"
#include "RoomConfig.h"

#define DAEMONSERVER_MAXCLIENT 5000

CDaemonServer* CDaemonServer::m_this = NULL;

CDaemonServer* CDaemonServer::FetchDaemonServerInstance()
{
	if (NULL == m_this)
		m_this = new CDaemonServer();
	return m_this;
}

void CDaemonServer::ReleaseDaemonServerInstance()
{
	if (NULL != m_this)
	{
		delete m_this;
		m_this = NULL;
	}
}

CDaemonServer::CDaemonServer()
: m_pSessionMaker(NULL)
, m_pSessionServ(NULL)
, m_bListenStatus(FALSE)
{

}

CDaemonServer::~CDaemonServer()
{
	DaemonServerStop();
	if (NULL != m_pSessionServ)
	{
		m_pSessionServ->DeleteThis();
		m_pSessionServ = NULL;
	}

	if (NULL != m_pSessionMaker)
	{
		delete m_pSessionMaker;
		m_pSessionMaker = NULL;
	}
	m_bListenStatus = FALSE;
}

INT CDaemonServer::DaemonServerStart()
{
	if (NULL == m_pSessionMaker)
		m_pSessionMaker = new CDaemonSessionMaker();
	ASSERT(NULL != m_pSessionMaker);
	if (NULL == m_pSessionServ && NULL != m_pSessionMaker)
		CyaTcp_MakeServeMan(m_pSessionServ, m_pSessionMaker, DAEMONSERVER_MAXCLIENT);
	ASSERT(NULL != m_pSessionServ);
	if (NULL == m_pSessionMaker || NULL == m_pSessionServ)
		return DAEMON_ERROR;

	CRoomClient* pRoomClient = CRoomClient::FetchRoomClientInstance();
	ASSERT(NULL != pRoomClient);
	if (NULL != pRoomClient)
	{
		pRoomClient->SetRoomClientConnectStatusCallBack(OnRoomClientConnectStatusCallBack, this);
		pRoomClient->SetRoomClientDataCallBack(OnRoomClientDataCallBack, this);
	}
	if (!m_bListenStatus)
	{
		IPPADDR IPA(INADDR_ANY, CRoomConfig::FetchConfigInstance()->GetLocalListenPort());
		if (0 == m_pSessionServ->StartServe(IPA))
			m_bListenStatus = TRUE;
	}

	return DAEMON_OK;
}

void CDaemonServer::DaemonServerStop()
{
	if (m_bListenStatus)
	{
		m_pSessionServ->StopServe(TRUE);
		m_bListenStatus = FALSE;
	}
}

BOOL CDaemonServer::DaemonServerIsListening() const
{
	return m_bListenStatus;
}

void CDaemonServer::OnRoomClientConnectStatusCallBack(ERoomConnectStatus eStatus, void* pContent)
{
	CDaemonServer* pThis = (CDaemonServer*)pContent;
	if (NULL != pThis)
	{
		pThis->OnRoomClientConnectStatusCallBack(eStatus);
	}
}

void CDaemonServer::OnRoomClientDataCallBack(const void* pData, const INT nDataLen, void* pContent)
{
	CDaemonServer* pThis = (CDaemonServer*)pContent;
	if (NULL != pThis)
	{
		pThis->OnRoomClientDataCallBack(pData, nDataLen);
	}
}

void CDaemonServer::OnRoomClientConnectStatusCallBack(ERoomConnectStatus eStatus)
{
	if (eStatus == RoomConnected && !m_bListenStatus)
	{
		//IPPADDR IPA(GetDWordIP("127.0.0.1"), CDaemonConfig::FetchConfigInstance()->GetLocalListenPort());
		IPPADDR IPA(INADDR_ANY, CRoomConfig::FetchConfigInstance()->GetLocalListenPort());
		if (0 == m_pSessionServ->StartServe(IPA))
			m_bListenStatus = TRUE;
	}
	if (eStatus == RoomDisconnected && m_bListenStatus)
	{
		/// 停止向RoomServer提交数据
	}
}

void CDaemonServer::OnRoomClientDataCallBack(const void* pData, const INT nDataLen)
{
	ASSERT(NULL != pData && 0 < nDataLen);
}