#include "GWSession.h"
#include "RelaySessionMan.h"
#include "cyaLog.h"
#include "ConfigFileMan.h"

GWSessionMaker::GWSessionMaker()
{

}

GWSessionMaker::~GWSessionMaker()
{

}

int GWSessionMaker::GetSessionObjSize()
{
	return sizeof(GWSesssion);
}

void GWSessionMaker::MakeSessionObj(void* session)
{
	::new(session)GWSesssion();
}


GWSesssion::GWSesssion()
: m_recvStatus(recv_header)
, m_linkerId(0)
, m_heartbeatTick(0)
{

}

GWSesssion::~GWSesssion()
{

}

void GWSesssion::OnConnected(SOCKET sock, const IPPADDR& peerIPP, int sessionCount, void* param /*= NULL*/)
{
	m_linkerId = 0;
	m_sessionId = 0;
	m_heartbeatTick = 0;
	m_recvStatus = recv_header;
}

void GWSesssion::OnClosed(int /*cause*/)
{
	if (m_linkerId != 0 && m_sessionId != 0)
	{
		GetRelaySessionMan()->DelRelaySession(m_linkerId, m_sessionId);
		LogInfo(("[%d:%u->%s] Closed!", m_linkerId, m_sessionId, GetServerTypeStr(m_linkerId)));
	}
	m_linkerId = 0;
	m_sessionId = 0;
	m_heartbeatTick = 0;
	m_recvStatus = recv_header;
}


int  GWSesssion::RequestReceive(void*& buf, int& bufLen, IoReadType& rt, BOOL& asynNotify)
{
	if (m_recvStatus == recv_header)
	{
		buf = &m_protoHeader;
		bufLen = sizeof(SGWSProtocolHead);
		rt = ICyaTcpSession::io_mustRead;
		asynNotify = false;
	}
	else if (m_recvStatus == recv_body)
	{
		bufLen = m_protoHeader.pduLen;
		buf = NULL;
		// 		buf = RoseTcp_Alloc(bufLen);
		// 		if(buf == NULL)
		// 		{
		// 			ASSERT(false);
		// 			return -1;
		// 		}

		rt = ICyaTcpSession::io_mustRead;
		asynNotify = true;
	}
	else
		ASSERT(false);
	return 0;
}

int  GWSesssion::OnReceived(void* buf, int bufLen, BOOL callbackByTask, BOOL& continueRecv)
{
	int nRet = 0;
	continueRecv = true;
	if (m_recvStatus == recv_header)
	{
		ASSERT(buf != NULL && bufLen == sizeof(SGWSProtocolHead));
		m_protoHeader.ntoh();

		//ASSERT(IS_VALID_SERVER(m_protoHeader.sourceId) && IS_VALID_SERVER(m_protoHeader.targetId));
		if (!IS_VALID_SERVER(m_protoHeader.sourceId) || !IS_VALID_SERVER(m_protoHeader.targetId))
		{
			LogError(("�����Դ[%d], Ŀ��[%d]", m_protoHeader.sourceId, m_protoHeader.targetId));
			ASSERT(false);
			return GWS_UNKNOWN_SERVERID;
		}

		if (m_protoHeader.pduLen <= 0)
		{
			m_recvStatus = recv_header;
			nRet = OnProcessData(NULL, 0);
		}
		else
			m_recvStatus = recv_body;
	}
	else if (m_recvStatus == recv_body)
	{
		ASSERT(buf != NULL && bufLen == m_protoHeader.pduLen);
		nRet = OnProcessData(buf, bufLen);
		//RoseTcp_Free(buf);
		m_recvStatus = recv_header;
	}
	else
		ASSERT(false);
	return nRet;
}

int GWSesssion::OnProcessData(const void* payloadData, int payLen)
{
	int nRet = GWS_OK;
	int pktType = m_protoHeader.pktType;
	if (pktType == DATA_CONFIRM_PACKET)	//��֤
	{
		ASSERT(IS_CENTER_GW_SERVER(m_protoHeader.targetId));
		nRet = OnReplyConfirmRequest(payloadData, payLen);
	}
	else if (pktType == DATA_HEARTBEAT_PACKET)	//����
	{
		nRet = OnReplyHeartbeatRequest(payloadData, payLen);
	}
	else if (pktType == DATA_PACKET)	//���ݰ�
	{
		nRet = OnReplyDataPacketRequest(payloadData, payLen);
	}
	else if (pktType == DATA_LINKER_GROUP_PACKET)
	{
		nRet = OnSendLinkerGroupDataPacket(payloadData, payLen);
	}
	else
	{
		ASSERT(false);
		nRet = OnReplyUnknownCmdRequest(payloadData, payLen);
	}
	return nRet;
}

int GWSesssion::OnReplyHeartbeatRequest(const void* payloadData, int payLen)
{
	ASSERT(payloadData == NULL && payLen == 0);
	BYTE p[4];
	*(UINT16*)p = DATA_HEARTBEAT_PACKET;	//������
	*(UINT16*)(p + 2) = htons(GWS_OK);	//���ؽ��

	m_protoHeader.pktType = DATA_HEARTBEAT_PACKET;
	m_protoHeader.pduLen = 4;
	m_protoHeader.hton();

	SockBufVec bufs[2];
	bufs[0].buf = &m_protoHeader;
	bufs[0].bufBytes = sizeof(SGWSProtocolHead);
	bufs[1].buf = p;
	bufs[1].bufBytes = 4;
	int r = 0;
	r = GetServeMan()->SendSessionEmergencyDataVec(this, bufs, 2);
	ASSERT(r == 0);

	//��������ʱ��
	UpdateHeartbeatTick();

	//LogDebug(("�յ�������Ϣ[%d:%u->%s]", m_linkerId, m_sessionId, GetServerTypeStr(m_linkerId)));

	return GWS_OK;
}

int GWSesssion::OnReplyDataPacketRequest(const void* payloadData, int payLen)
{
	ASSERT(payLen >= sizeof(UINT16));
	if (payloadData == NULL || payLen <= 0)
		return GWS_ERROR;

	BYTE targetId = m_protoHeader.targetId;
	UINT32 sessionId = m_protoHeader.linkerSessId;
	SockBufVec bufs[2];
	m_protoHeader.hton();
	bufs[0].buf = &m_protoHeader;
	bufs[0].bufBytes = sizeof(SGWSProtocolHead);
	bufs[1].buf = (void*)payloadData;
	bufs[1].bufBytes = payLen;
	int r = GetRelaySessionMan()->SendRelaySessionDataVec(targetId, sessionId, bufs, 2);
	if (r == 0)	//ת���ɹ�
		return GWS_OK;

	//ת��ʧ�ܣ������ظ���ǰsession
	BYTE p[4];	//SServerCommonReply
	*(UINT16*)p = *(UINT16*)payloadData;	//ָ��
	*(UINT16*)(p + 2) = htons((UINT16)r);	//���ؽ��

	m_protoHeader.ntoh();
	m_protoHeader.pktType = DATA_RELAY_FAILED_PACKET;
	m_protoHeader.pduLen = 4;
	m_protoHeader.hton();
	bufs[0].buf = &m_protoHeader;
	bufs[0].bufBytes = sizeof(SGWSProtocolHead);
	bufs[1].buf = p;
	bufs[1].bufBytes = 4;
	GetServeMan()->SendSessionDataVec(this, bufs, 2);

	return GWS_OK;
}

int GWSesssion::OnReplyConfirmRequest(const void* payloadData, int payLen)
{
	ASSERT(payLen == sizeof(SGWSConfirmReq));
	if (payloadData == NULL || payLen != sizeof(SGWSConfirmReq))
		return GWS_ERROR;
	/*
	������֤��
	*/

	int ret = GetRelaySessionMan()->AddRelaySession(m_protoHeader.sourceId, m_protoHeader.linkerSessId, this);
	if (ret == GWS_OK)
	{
		m_linkerId = m_protoHeader.sourceId;
		m_sessionId = m_protoHeader.linkerSessId;
		LogInfo(("[%d:%u->%s] Login Ok!", m_linkerId, m_sessionId, GetServerTypeStr(m_linkerId)));
	}
	else
		LogInfo(("[%d:%u->%s] Login Failed!(errno:%d)", m_protoHeader.sourceId, m_protoHeader.linkerSessId, GetServerTypeStr(m_protoHeader.sourceId), ret));

	SGWSConfirmReq* req = (SGWSConfirmReq*)payloadData;
	req->ntoh();

	SGWSConfirmRes res;	//2�ֽ�ָ��+2�ֽڷ��ؽ��+1�ֽ���֤���+2�ֽڷ��������
	res.cmdCode = req->cmdCode;
	res.cmdRet = (UINT16)ret;
	res.confirmRet = ret == GWS_OK ? 0x00 : 0x01;
	res.serverCode = CENTER_GW_SERVER;
	res.hton();

	m_protoHeader.pduLen = sizeof(SGWSConfirmRes);
	m_protoHeader.userId = GetConfigFileMan()->GetHeartbeatInterval();
	m_protoHeader.hton();

	SockBufVec bufs[2];
	bufs[0].buf = &m_protoHeader;
	bufs[0].bufBytes = sizeof(SGWSProtocolHead);
	bufs[1].buf = &res;
	bufs[1].bufBytes = sizeof(res);
	GetServeMan()->SendSessionDataVec(this, bufs, 2, NULL);

	//��������ʱ��
	UpdateHeartbeatTick();

	return GWS_OK;
}

int GWSesssion::OnSendLinkerGroupDataPacket(const void* payloadData, int payLen)
{
	SockBufVec bufs[2];
	m_protoHeader.hton();
	bufs[0].buf = &m_protoHeader;
	bufs[0].bufBytes = sizeof(SGWSProtocolHead);
	bufs[1].buf = (void*)payloadData;
	bufs[1].bufBytes = payLen;
	return GetRelaySessionMan()->SendLinkerGroupDataPacket(bufs, 2);
}

int GWSesssion::OnReplyUnknownCmdRequest(const void* payloadData, int payLen)
{
	if (payloadData == NULL || payLen <= 0)
		return GWS_ERROR;

	BYTE p[4];
	*(UINT16*)p = payLen >= sizeof(UINT16) ? *(UINT16*)payloadData : 0;	//������
	*(UINT16*)(p + 2) = htons((UINT16)GWS_UNKNOWN_PACKET);	//���ؽ��

	m_protoHeader.ntoh();
	m_protoHeader.pktType = DATA_RELAY_FAILED_PACKET;
	m_protoHeader.pduLen = 4;
	m_protoHeader.hton();

	SockBufVec bufs[2];
	bufs[0].buf = &m_protoHeader;
	bufs[0].bufBytes = sizeof(SGWSProtocolHead);
	bufs[1].buf = p;
	bufs[1].bufBytes = 4;
	GetServeMan()->SendSessionDataVec(this, bufs, 2);

	return GWS_OK;
}