#include "LscSession.h"
#include "AES.h"
#include "LinkGlobalObj.h"
#include "LicenceClient.h"
#include "cyaLog.h"

LSCSession::LSCSession()
	: m_recv_status(RECV_HEAD)
	, m_LscCallBack(NULL)
	, m_LscContext(NULL)
{

}

LSCSession::~LSCSession()
{

}

void LSCSession::OnConnected(SOCKET sock, const IPPADDR& peerIPP, int sessionCount, void* param)
{
	ASSERT(SockValid(sock) && 0<sessionCount);
	GlobalGetLicenceClient()->SetLocalLSCSession(this);
}

void LSCSession::OnClosed(int cause)
{
	m_recv_status = RECV_HEAD;
	GlobalGetLicenceClient()->LscSessionBroken();
	LogDebug(("LSCSession::OnClosed(%d)", cause));
}

int  LSCSession::RequestReceive(void*& buf, int& bufLen, IoReadType& rt, BOOL& asynNotify)
{
	if (RECV_HEAD == m_recv_status)
	{
		memset(&m_pro_head, 0, sizeof(SGSProtocolHead));
		buf = &m_pro_head;
		bufLen = sizeof(SGSProtocolHead);
		rt = io_mustRead;
		asynNotify = false;
	}
	else if (RECV_BODY == m_recv_status)
	{
		buf = CyaTcp_Alloc(m_pro_head.pduLen + 1);
		bufLen = m_pro_head.pduLen;
		rt = io_mustRead;
		asynNotify = false;
	}
	else
		ASSERT(false);
	return 0;
}

int  LSCSession::OnReceived(void* buf, int bufLen, BOOL callbackByTask, BOOL& continueRecv)
{
	if (RECV_HEAD == m_recv_status)
	{
		ASSERT(sizeof(SGSProtocolHead) == bufLen);
		m_pro_head.ntoh();
		m_recv_status = RECV_BODY;
		continueRecv = true;
	}
	else if (RECV_BODY == m_recv_status)
	{
		ASSERT(bufLen == m_pro_head.pduLen);
		continueRecv = true;
		m_recv_status = RECV_HEAD;
		if (NULL != m_LscCallBack)
			return m_LscCallBack(m_pro_head.isCrypt, m_pro_head.pktType, buf, bufLen, m_LscContext);
	}
	else
		ASSERT(false);

	return 0;
}

void LSCSession::SetLscClientCallBack(LscClientMsgCallBack lscFunc, void* lscContext)
{
	m_LscCallBack = lscFunc;
	m_LscContext = lscContext;
}