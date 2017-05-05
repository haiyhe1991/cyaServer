#include "GWSHelper.h"
#include "GWGlobalObjMan.h"
#include "cyaSock.h"
#include "cyaIpcvt.h"

#define MAX_NO_HEARTBEAT_TIMES 3


GWSHelper::GWSHelper(const char* gwsIp, int gwsPort,
	EServerType serverType,
	fnDataCallback cb, void* context,
	unsigned int connTimeout/* = 5000*/,
	unsigned int writeTimeout/* = 0*/,
	unsigned int readTimeout/* = 0*/)
	: m_gwsIp(gwsIp)
	, m_gwsPort(gwsPort)
	, m_serverType(serverType)
	, m_connTimeout(connTimeout)
	, m_writeTimeout(writeTimeout)
	, m_readTimeout(readTimeout)
	, m_heartbeatInterval(0)
	, m_dataCallback(cb)
	, m_context(context)
	, m_evtCallback(NULL)
	, m_evtCallbackContext(NULL)
	, m_cmdSeq(0)
	, m_isLogin(false)
	, m_localSession(NULL)
	, m_recvHeartTick(0)
	, m_sessionId(0)
	, m_reconnectTimer(ReconnectTimer, this, 5000)
	, m_heartbeatTimer(HeartbeatTimer, this, 30 * 1000)
{
	memset(&m_heartbeatData, 0, sizeof(SGWSProtocolHead));
	m_heartbeatData.pktType = DATA_HEARTBEAT_PACKET;
	m_heartbeatData.sourceId = m_serverType;
	m_heartbeatData.targetId = CENTER_GW_SERVER;
	m_heartbeatData.userId = m_serverType;
	m_heartbeatData.hton();
}

GWSHelper::~GWSHelper()
{

}

void GWSHelper::SetEventCallback(fnEventCallback cb, void* context)
{
	CXTAutoLock lock(m_locker);
	m_evtCallback = cb;
	m_evtCallbackContext = context;
}

int  GWSHelper::Login()
{
	if (m_isLogin)
		return GWS_OK;

	CXTAutoLock lock(m_locker);
	if (m_isLogin)
		return GWS_OK;
	SOCKET sock = TCPNewClientByTime(GetDWordIP(m_gwsIp.c_str()), m_gwsPort, m_connTimeout);
	if (!SockValid(sock))
		return GWC_CONNECT_TIMEOUT;

	m_sessionId = sock;
	AutoSock autoSock(sock);
	BYTE req[sizeof(SGWSProtocolHead)+sizeof(SGWSConfirmReq)] = { 0 };	//2字节命令码+1字节服务器类型+33字节验证码
	SGWSConfirmReq* payload = (SGWSConfirmReq*)(req + sizeof(SGWSProtocolHead));
	payload->cmdCode = DATA_CONFIRM_PACKET;
	payload->serverType = (BYTE)m_serverType;

	SGWSProtocolHead header;
	OnMakeProtocolHeader((SGWSProtocolHead*)req, sizeof(SGWSConfirmReq), DATA_CONFIRM_PACKET, false, CENTER_GW_SERVER, (int)sock, OnGetCmdSeq());

	int nWrite = SockWriteAllByTime(sock, req, sizeof(req), 5000);
	if (nWrite != sizeof(req))
		return GWC_SEND_TIMEOUT;

	BYTE res[sizeof(SGWSProtocolHead)+sizeof(SGWSConfirmRes)] = { 0 };	//2字节命令码+2字节结果+1字节验证结果+2字节服务器编号
	int nRead = SockReadAllByTime(sock, res, sizeof(res), 10 * 1000);
	if (nRead != sizeof(res))
		return GWC_RECV_TIMEOUT;

	SGWSConfirmRes* pConfirm = (SGWSConfirmRes*)(res + sizeof(SGWSProtocolHead));
	pConfirm->ntoh();
	if (pConfirm->cmdRet != GWS_OK)
		return pConfirm->cmdRet;

	SGWSProtocolHead* pReplyHeader = (SGWSProtocolHead*)res;
	pReplyHeader->ntoh();
	m_heartbeatInterval = pReplyHeader->userId;	//登陆时,将该字段作为心跳时间间隔

	autoSock.Drop();
	GetGWGlobalObjMan()->GetGWSSessionMan()->TrustSession(sock, this, true);

	m_isLogin = true;

	if (m_evtCallback != NULL)
		m_evtCallback(GWS_CONNECTED, NULL, 0, m_evtCallbackContext);

	if (m_heartbeatInterval > 0)
	{
		m_heartbeatTimer.SetPeriod(m_heartbeatInterval);
		m_heartbeatTimer.Start();
		m_recvHeartTick = GetTickCount();
	}
	return GWS_OK;
}

void GWSHelper::Logout()
{
	if (m_reconnectTimer.IsStarted())
		m_reconnectTimer.Stop();

	if (m_heartbeatTimer.IsStarted())
		m_heartbeatTimer.Stop();

	CXTAutoLock lock(m_locker);
	if (!m_isLogin || m_localSession == NULL)
		return;

	m_localSession->SetGWSHelper(NULL);
	GetGWGlobalObjMan()->GetGWSSessionMan()->CloseSession(m_localSession, true);
	m_localSession = NULL;
	m_isLogin = false;
}

int GWSHelper::SendDataPacket(const void* data, int dataBytes,
	int targetId, unsigned int userId,
	ESGSPacketType pktType, int isCrypt, long* seq)
{
	if (data == NULL || dataBytes <= 0 || dataBytes >= 4080)
		return GWC_INVALID_PARAM;

	return OnAsyncProcessDataPacket(data, dataBytes, targetId, userId, pktType, isCrypt, seq);
}

int  GWSHelper::SendDataPacketEx(const SGWSProtocolHead* pHead, const void* data, int dataBytes)
{
	if (pHead == NULL || data == NULL || dataBytes <= 0 || dataBytes >= 4080)
		return GWC_INVALID_PARAM;

	CXTAutoLock lock(m_locker);
	if (m_localSession == NULL)
		return GWC_SERVER_BROKEN;

	SockBufVec bufs[2];
	bufs[0].buf = (void*)pHead;
	bufs[0].bufBytes = sizeof(SGWSProtocolHead);
	bufs[1].buf = (void*)data;
	bufs[1].bufBytes = dataBytes;
	return GetGWGlobalObjMan()->GetGWSSessionMan()->SendSessionDataVec(m_localSession, bufs, 2) == 0 ? GWS_OK : GWC_SERVER_BROKEN;
}

void GWSHelper::ReleaseHandle()
{
	Logout();
	delete this;
}

void GWSHelper::SetLocalSession(GWSClientSession* session)
{
	m_localSession = session;
}

long GWSHelper::OnGetCmdSeq()
{
	if (InterlockedIncrement(&m_cmdSeq) <= 0)
		m_cmdSeq = 1;
	return m_cmdSeq;
}

void  GWSHelper::OnMakeProtocolHeader(SGWSProtocolHead* pHeader, int pduLen, ESGSPacketType pktType, BOOL isCrypt, int targetId, int userId, long seq)
{
	ASSERT(pHeader != NULL);
	pHeader->encrypt = isCrypt;
	pHeader->extField = 0;
	pHeader->pduLen = pduLen;
	pHeader->pktSeq = seq;
	pHeader->linkerSessId = m_sessionId;
	pHeader->pktType = pktType;
	pHeader->sourceId = m_serverType;
	pHeader->targetId = targetId;
	pHeader->userId = userId;
	pHeader->hton();
}

void GWSHelper::NotifySessionBroken()
{
	m_recvHeartTick = 0;

	{
		CXTAutoLock lock(m_locker);
		m_localSession = NULL;
		if (m_evtCallback != NULL)
			m_evtCallback(GWS_BROKEN, NULL, 0, m_evtCallbackContext);
	}

	if (!m_reconnectTimer.IsStarted())
		m_reconnectTimer.Start();
}

int GWSHelper::OnAsyncProcessDataPacket(const void* data, int dataBytes, int targetId, unsigned int userId, ESGSPacketType pktType, int isCrypt, long* seq)
{
	if (m_localSession == NULL)
		return GWC_SERVER_BROKEN;

	CXTAutoLock lock(m_locker);
	if (m_localSession == NULL)
		return GWC_SERVER_BROKEN;

	long pktSeq = OnGetCmdSeq();
	if (seq != NULL)
		*seq = pktSeq;

	SGWSProtocolHead header;
	OnMakeProtocolHeader(&header, dataBytes, pktType, isCrypt, targetId, userId, pktSeq);
	SockBufVec bufs[2];
	bufs[0].buf = &header;
	bufs[0].bufBytes = sizeof(SGWSProtocolHead);
	bufs[1].buf = (void*)data;
	bufs[1].bufBytes = dataBytes;
	return GetGWGlobalObjMan()->GetGWSSessionMan()->SendSessionDataVec(m_localSession, bufs, 2) == 0 ? GWS_OK : GWC_SERVER_BROKEN;
}

void GWSHelper::InputData(SGWSProtocolHead* pHeader, const void* payload, int payloadBytes)
{
	if (pHeader->pktType == DATA_HEARTBEAT_PACKET)
	{
		m_recvHeartTick = GetTickCount();
		return;
	}

	if (m_dataCallback)
		m_dataCallback(pHeader, payload, payloadBytes, m_context);
}

void GWSHelper::ReconnectTimer(void* param, TTimerID id)
{
	GWSHelper* pThis = (GWSHelper*)param;
	ASSERT(pThis != NULL);
	pThis->OnReconnect();
}

void GWSHelper::OnReconnect()
{
	SOCKET sock = TCPNewClientByTime(GetDWordIP(m_gwsIp.c_str()), m_gwsPort, m_connTimeout);
	if (!SockValid(sock))
		return;

	m_sessionId = sock;
	AutoSock autoSock(sock);
	BYTE req[sizeof(SGWSProtocolHead)+sizeof(SGWSConfirmReq)] = { 0 };	//2字节命令码+1字节服务器类型+33字节验证码
	SGWSConfirmReq* payload = (SGWSConfirmReq*)(req + sizeof(SGWSProtocolHead));
	payload->cmdCode = DATA_CONFIRM_PACKET;
	payload->serverType = (BYTE)m_serverType;

	SGWSProtocolHead header;
	OnMakeProtocolHeader((SGWSProtocolHead*)req, sizeof(SGWSConfirmReq), DATA_CONFIRM_PACKET, false, CENTER_GW_SERVER, (int)sock, OnGetCmdSeq());

	int nWrite = SockWriteAllByTime(sock, req, sizeof(req), 5000);
	if (nWrite != sizeof(req))
		return;

	BYTE res[sizeof(SGWSProtocolHead)+sizeof(SGWSConfirmRes)] = { 0 };	//2字节命令码+2字节结果+1字节验证结果+2字节服务器编号
	int nRead = SockReadAllByTime(sock, res, sizeof(res), 10 * 1000);
	if (nRead != sizeof(res))
		return;

	SGWSConfirmRes* pConfirm = (SGWSConfirmRes*)(res + sizeof(SGWSProtocolHead));
	pConfirm->ntoh();
	if (pConfirm->cmdRet != GWS_OK)
		return;

	SGWSProtocolHead* pReplyHeader = (SGWSProtocolHead*)res;
	pReplyHeader->ntoh();
	m_heartbeatInterval = pReplyHeader->userId;	//登陆时,将该字段作为心跳时间间隔

	autoSock.Drop();
	GetGWGlobalObjMan()->GetGWSSessionMan()->TrustSession(sock, this, true);

	{
		if (m_evtCallback != NULL)
		{
			CXTAutoLock lock(m_locker);
			if (m_evtCallback != NULL)
				m_evtCallback(GWS_CONNECTED, NULL, 0, m_evtCallbackContext);
		}
	}

	if (m_heartbeatInterval > 0)
	{
		m_heartbeatTimer.SetPeriod(m_heartbeatInterval);
		m_heartbeatTimer.Start();
		m_recvHeartTick = GetTickCount();
	}
	else
		m_heartbeatTimer.Stop(false);

	m_reconnectTimer.Stop(false);
}

void GWSHelper::HeartbeatTimer(void* param, TTimerID id)
{
	GWSHelper* pThis = (GWSHelper*)param;
	ASSERT(pThis != NULL);
	pThis->OnHeartbeat();
}

void GWSHelper::OnHeartbeat()
{
	if (m_localSession == NULL)
	{
		m_heartbeatTimer.Stop(false);
		return;
	}

	DWORD tickNow = GetTickCount();
	int tickInterval = tickNow - m_recvHeartTick;
	if (tickInterval < 0)
	{
		m_recvHeartTick = tickNow;
		return;
	}

	if (m_recvHeartTick > 0 && tickInterval > m_heartbeatInterval*MAX_NO_HEARTBEAT_TIMES)	//broken ???
	{
		{
			CXTAutoLock lock(m_locker);
			if (m_localSession != NULL)
			{
				GWSClientSession * session = m_localSession;
				m_localSession = NULL;
				GetGWGlobalObjMan()->GetGWSSessionMan()->CloseSession(session, true);
			}
		}

		m_heartbeatTimer.Stop(false);
		return;
	}

	CXTAutoLock lock(m_locker);
	if (m_localSession != NULL)
		GetGWGlobalObjMan()->GetGWSSessionMan()->SendSessionEmergencyData(m_localSession, &m_heartbeatData, sizeof(SGWSProtocolHead), false);
}