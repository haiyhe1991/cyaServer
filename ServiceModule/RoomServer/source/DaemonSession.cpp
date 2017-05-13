#include "DaemonSession.h"
#include "ServiceRoom.h"
#include "ServiceRoomDaemon.h"

CDaemonSession::CDaemonSession()
: m_dataType(SGS_HEAD)
, m_msgProcMgr(NULL)
, m_gwClient(NULL)
, m_procId(INVALID_OSPROCESSID)
{

}

CDaemonSession::~CDaemonSession()
{
	m_msgProcMgr->ProcessUnregist(m_procId);
	m_procId = INVALID_OSPROCESSID;
}

void CDaemonSession::OnConnected(SOCKET sock, const IPPADDR& peerIPP, int sessionCount, void* param /*= NULL*/)
{
	ASSERT(sessionCount >= 1);
	m_msgProcMgr = CProcessManager::FetchProcessManagerInstance();
	m_gwClient = CGWSClient::FetchGateWayClientInstance();
	ASSERT(NULL != m_msgProcMgr);
	ASSERT(NULL != m_gwClient);
}

void CDaemonSession::OnConnectedSend()
{
	char pResponsePacketBuff[512] = { 0 };
	memset(pResponsePacketBuff, 0, 512);
	SGSProtocolHead* pProHead = (SGSProtocolHead*)pResponsePacketBuff;
	pProHead->isCrypt = 0;
	pProHead->pktType = DATA_PACKET;
	pProHead->pduLen = RES_RAW_SIZE;
	pProHead->hton();

	SGSResPayload* pResPayload = (SGSResPayload*)(pResponsePacketBuff + PRO_HEAD_SIZE);
	pResPayload->cmdCode = DATA_TOKEN_KEY_PACKET;
	pResPayload->retCode = 0;
	pResPayload->rawDataBytes = RES_RAW_SIZE;
	pResPayload->token = 0;
	pResPayload->hton();

	GetServeMan()->SendSessionData(this, pResponsePacketBuff, PRO_HEAD_SIZE + RES_RAW_SIZE);
}

void CDaemonSession::OnClosed(int cause)
{

}

int  CDaemonSession::RequestReceive(void*& buf, int& bufLen, IoReadType& rt, BOOL& asynNotify)
{
	buf = NULL;
	bufLen = 0;
	rt = IoReadType::io_mustRead;
	asynNotify = TRUE;

	if (SGS_HEAD == m_dataType)
	{
		asynNotify = TRUE;
		buf = &m_sgsHead;
		bufLen = PRO_HEAD_SIZE;
		memset(&m_sgsHead, 0, PRO_HEAD_SIZE);
	}
	else if (SGS_BODY == m_dataType)
	{
		asynNotify = FALSE;
		bufLen = m_sgsHead.pduLen;
		buf = CyaTcp_Alloc(bufLen + 1);
	}
	else
		ASSERT(FALSE);

	return 0;
}

int  CDaemonSession::OnReceived(void* buf, int bufLen, BOOL callbackByTask, BOOL& continueRecv)
{
	ASSERT(NULL != buf && 0 < bufLen);
	if (NULL == buf || 0 >= bufLen)
	{
		continueRecv = FALSE;
		return -1;
	}

	continueRecv = TRUE;
	if (SGS_HEAD == m_dataType)
	{
		ASSERT(PRO_HEAD_SIZE == bufLen);
		m_sgsHead.ntoh();
		m_dataType = SGS_BODY;
	}
	else if (SGS_BODY == m_dataType)
	{
		ASSERT(m_sgsHead.pduLen == bufLen);
		m_dataType = SGS_HEAD;
		SGSReqPayload* pReqHead = (SGSReqPayload*)buf;
		pReqHead->ntoh();
		if (pReqHead->cmdCode == DAEMON_LOGIN)
		{
			SSceneLoginDaemon* pLogin = (SSceneLoginDaemon*)pReqHead->data;
			pLogin->ntoh();
			if (m_msgProcMgr && pLogin->id > 0)
			{
				m_procId = pLogin->id;
				m_msgProcMgr->ProcessRegist(pLogin->id, this);
				ResponseSessionMsg(NULL, 0, DAEMON_LOGIN);
			}
		}
		else
		{
			m_gwClient->InputSessionMsg(buf, bufLen);
		}

		CyaTcp_Free(buf);
	}
	else
		ASSERT(FALSE);
	return 0;
}

int CDaemonSession::ResponseSessionMsg(const void* data, int dataLen, UINT16 cmdCode)
{
	char* ResponseBuff = (char*)CyaTcp_Alloc(PRO_HEAD_SIZE + RES_RAW_SIZE + dataLen + 1);
	ASSERT(NULL != ResponseBuff);
	if (NULL == ResponseBuff)
		return -1;
	memset(ResponseBuff, 0, PRO_HEAD_SIZE + RES_RAW_SIZE + dataLen + 1);

	SGSProtocolHead* pProHead = (SGSProtocolHead*)ResponseBuff;
	pProHead->isCrypt = 0;
	pProHead->pktType = DATA_PACKET;
	pProHead->pduLen = RES_RAW_SIZE + dataLen;
	pProHead->hton();

	SGSResPayload* pResHead = (SGSResPayload*)(ResponseBuff + PRO_HEAD_SIZE);
	pResHead->cmdCode = cmdCode;
	pResHead->rawDataBytes = RES_RAW_SIZE + dataLen;
	pResHead->retCode = 0;
	pResHead->token = 0;
	pResHead->hton();

	if (NULL != data && 0 < dataLen)
		memcpy(pResHead->data, data, dataLen);

	int n = GetServeMan()->SendSessionData(this, ResponseBuff, PRO_HEAD_SIZE + RES_RAW_SIZE + dataLen);
	CyaTcp_Free(ResponseBuff);
	return n;
}

