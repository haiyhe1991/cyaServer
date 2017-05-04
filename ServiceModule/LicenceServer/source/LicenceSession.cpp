#include "AES.h"
#include "cyaLog.h"
#include "LicenceSession.h"
#include "ServiceErrorCode.h"
#include "ConfigFileMan.h"
#include "LicenceLogicProcMan.h"
#include "LicenceCommon.h"
#include "ExtServeLogicMan.h"

//�Ự������
LicenceSessionMaker::LicenceSessionMaker()
{

}

LicenceSessionMaker::~LicenceSessionMaker()
{

}

int LicenceSessionMaker::GetSessionObjSize()
{
	return sizeof(LicenceSession);
}

void LicenceSessionMaker::MakeSessionObj(void* session)
{
	::new(session)LicenceSession();
}

//�Ự
LicenceSession::LicenceSession()
	: m_recvStatus(recv_header)
	, m_communicationToken(0)
	, m_id(0)
	, m_partitionId(0)
	, m_sessionType(other_session)
	, m_sessionTick(0)
{

}

LicenceSession::~LicenceSession()
{

}

void LicenceSession::OnConnected(SOCKET sock, const IPPADDR& peerIPP, int sessionCount, void* param/* = NULL*/)
{
	m_id = 0;
	m_partitionId = 0;
	m_sessionTick = 0;
	m_recvStatus = recv_header;
	m_communicationToken = 0;
	m_sessionType = other_session;
	m_key[DEFAULT_KEY_SIZE] = '\0';
}

void LicenceSession::OnConnectedSend()
{
	BOOL isCrypt = GetConfigFileMan()->IsDataEncrypt();
	int nRawPayloadLen = SGS_RES_HEAD_LEN + DEFAULT_KEY_SIZE;
	int nEncryptedPayloadLen = isCrypt ? ((nRawPayloadLen + 15) / 16) * 16 : nRawPayloadLen;

	BYTE p[MAX_BUF_LEN] = { 0 };
	SGSProtocolHead* pSGSHeader = (SGSProtocolHead*)p;
	pSGSHeader->isCrypt = isCrypt ? 1 : 0;
	pSGSHeader->pktType = DATA_TOKEN_KEY_PACKET;
	pSGSHeader->pduLen = nEncryptedPayloadLen;
	pSGSHeader->hton();

	SGSResPayload* pResPayload = (SGSResPayload*)(p + SGS_PROTO_HEAD_LEN);
	pResPayload->rawDataBytes = nRawPayloadLen;
	pResPayload->cmdCode = DATA_TOKEN_KEY_PACKET;
	pResPayload->retCode = LCS_OK;
	m_communicationToken = GetTickCount();
	pResPayload->token = m_communicationToken;
	GenerateEncryptKey(m_key, DEFAULT_KEY_SIZE);
	memcpy(pResPayload->data, m_key, DEFAULT_KEY_SIZE);
	pResPayload->hton();

	//����
	if (isCrypt)
		appEncryptDataWithKey((AES_BYTE*)(p + SGS_PROTO_HEAD_LEN), nRawPayloadLen, DEFAULT_TOKEN_KEY);

	GetServeMan()->SendSessionData(this, p, SGS_PROTO_HEAD_LEN + nEncryptedPayloadLen, false);
}

void LicenceSession::OnClosed(int cause)
{
	if (CYATCP_KEEPALIVE_EXPIRED == cause)
		LogDebug(("�Ự[0x%x]�������ڽ���!", this));

	if (m_id != 0)
	{
		if (m_sessionType == linker_session)
		{
			GetLicenceLogicProcMan()->LinkerLogout(m_partitionId, (BYTE)m_id);
			LogInfo(("����[%d], LinkServer[%d]�˳�!", m_partitionId, m_id));
		}
		else if (m_sessionType == external_man_session)
		{
			LogInfo(("�ⲿ���������[%d]�˳�!", m_id));
		}
		else if (m_sessionType == user_session)
		{
			GetLicenceLogicProcMan()->UserLogout(m_id);
			LogInfo(("�û�[%u]�˳�!", m_id));
		}
	}

	m_id = 0;
	m_partitionId = 0;
	m_sessionTick = 0;
	m_recvStatus = recv_header;
	m_communicationToken = 0;
	m_sessionType = other_session;
	m_key[DEFAULT_KEY_SIZE] = '\0';
}

int  LicenceSession::RequestReceive(void*& buf, int& bufLen, IoReadType& rt, BOOL& asynNotify)
{
	if (m_recvStatus == recv_header)
	{
		buf = &m_sgsProHeader;
		bufLen = SGS_PROTO_HEAD_LEN;    //sizeof(SGSProtocolHead)
		rt = ICyaTcpSession::io_mustRead;
		asynNotify = false;
	}
	else if (m_recvStatus == recv_payload)
	{
		if (m_sgsProHeader.pduLen <= 0)
		{
			ASSERT(false);
			return LCS_INVALID_SESSION_PACKET;
		}

		buf = NULL;
		bufLen = m_sgsProHeader.pduLen;
		rt = ICyaTcpSession::io_mustRead;
		asynNotify = true;
	}
	else
		ASSERT(false);
	return CYATCP_OK;
}

int  LicenceSession::OnReceived(void* buf, int bufLen, BOOL callbackByTask, BOOL& continueRecv)
{
	if (m_recvStatus == recv_header)
	{
		m_sgsProHeader.ntoh();
		m_recvStatus = recv_payload;
	}
	else if (m_recvStatus == recv_payload)
	{
		OnProcessPacket(buf, bufLen);
		m_recvStatus = recv_header;
	}
	else
		ASSERT(false);
	return CYATCP_OK;
}

//�������ݰ�
int LicenceSession::OnProcessPacket(const void* payload, int nPayloadLen)
{
	if (payload == NULL || nPayloadLen <= 0)
		return LCS_OK;

	if (m_sgsProHeader.isCrypt)
		appDecryptDataWithKey((AES_BYTE*)payload, nPayloadLen, m_key);  //����

	int ret = LCS_OK;
	if (m_sgsProHeader.pktType == DATA_CTRL_PACKET)
		ret = OnProcessCtrlPacket(payload);
	else if (m_sgsProHeader.pktType == DATA_PACKET)
		ret = OnProcessDataPacket(payload);
	else
		ASSERT(false);

	return ret;
}

int LicenceSession::OnProcessDataPacket(const void* payload)
{
	SGSReqPayload* pReq = (SGSReqPayload*)payload;
	pReq->ntoh();
	//  if(!IsValidCommunicationToken(pReq->token)) //��֤token
	//      return LCS_INVALID_TOKEN;

	UINT16 rawDataBytes = pReq->rawDataBytes;   //ԭʼ����(δ����)����
	UINT16 cmdCode = pReq->cmdCode;
	BYTE* p = (BYTE*)payload + SGS_REQ_HEAD_LEN;            //sizeof(SGSReqPayload) - sizeof(BYTE); //���帺������
	int nRawPayloadLen = rawDataBytes - SGS_REQ_HEAD_LEN;   //sizeof(SGSReqPayload) + sizeof(BYTE); //���帺�����ݳ���

	LogDebug(("OnProcessDataPacket cmdCode[%d], len[%d]", cmdCode, nRawPayloadLen));

	int retCode = LCS_OK;
	//  if(cmdCode >= LICENCE_LINKER_LOGIN && cmdCode <= LICENCE_QUERY_UNLOCK_JOBS)
	//      retCode = GetLicenceLogicProcMan()->ProcessLogic(this, cmdCode, p, nRawPayloadLen, DATA_PACKET);
	//  else
	//      retCode = GetExtServeLogicMan()->ProcessLogic(this, cmdCode, p, nRawPayloadLen, DATA_PACKET);
	retCode = GetLicenceLogicProcMan()->ProcessLogic(this, cmdCode, p, nRawPayloadLen, DATA_PACKET);

	return retCode;
}

int LicenceSession::OnProcessCtrlPacket(const void* payload)
{
	SGSResPayload* pRes = (SGSResPayload*)payload;
	pRes->ntoh();
	UINT16 rawDataBytes = pRes->rawDataBytes;
	UINT16 cmdCode = pRes->cmdCode;
	BYTE* p = (BYTE*)payload;
	int nRawPayloadLen = rawDataBytes;

	LogDebug(("OnProcessCtrlPacket cmdCode[%d], len[%d]", cmdCode, nRawPayloadLen));

	int retCode = LCS_OK;
	//  if(cmdCode >= LICENCE_LINKER_LOGIN && cmdCode <= LICENCE_QUERY_UNLOCK_JOBS)
	//      retCode = GetLicenceLogicProcMan()->ProcessLogic(this, cmdCode, p, nRawPayloadLen, DATA_CTRL_PACKET);
	//  else
	//      retCode = GetExtServeLogicMan()->ProcessLogic(this, cmdCode, p, nRawPayloadLen, DATA_CTRL_PACKET);

	retCode = GetLicenceLogicProcMan()->ProcessLogic(this, cmdCode, p, nRawPayloadLen, DATA_CTRL_PACKET);

	return retCode;
}

#if SEPARATOR("LinkSession Interface")
int LicenceSession::ControlLinkerServe(BOOL startORstop)
{
	BYTE buf[MAX_BUF_LEN] = { 0 };
	BOOL isCrypt = GetConfigFileMan()->IsDataEncrypt();
	int nRawPayloadLen = SGS_REQ_HEAD_LEN;
	int nEncryptedPayloadLen = isCrypt ? ((nRawPayloadLen + 15) / 16) * 16 : nRawPayloadLen;

	SGSProtocolHead* pSGSHeader = (SGSProtocolHead*)buf;
	pSGSHeader->isCrypt = isCrypt ? 1 : 0;
	pSGSHeader->pktType = DATA_CTRL_PACKET;
	pSGSHeader->pduLen = nEncryptedPayloadLen;
	pSGSHeader->hton();

	SGSReqPayload* pReqpayload = (SGSReqPayload*)(buf + SGS_PROTO_HEAD_LEN);
	pReqpayload->cmdCode = startORstop ? LCS_START_LINKER_SERVE : LCS_STOP_LINKER_SERVE;
	pReqpayload->rawDataBytes = nRawPayloadLen;
	pReqpayload->token = m_communicationToken;  //������token
	pReqpayload->hton();

	if (isCrypt)
		appEncryptDataWithKey((AES_BYTE*)(buf + SGS_PROTO_HEAD_LEN), nRawPayloadLen, m_key);

	//��������
	return GetServeMan()->SendSessionData(this, buf, SGS_PROTO_HEAD_LEN + nEncryptedPayloadLen) == CYATCP_OK ? LCS_OK : LCS_LINKER_NOT_LOGIN;
}

int LicenceSession::UserEnterLinkerGetToken(UINT32 userId, const char* username)
{
	BYTE buf[MAX_BUF_LEN] = { 0 };
	BOOL isCrypt = GetConfigFileMan()->IsDataEncrypt();
	int nRawPayloadLen = SGS_REQ_HEAD_LEN + sizeof(SLinkerGetUserTokenReq);//sizeof(SGSReqPayload) - sizeof(BYTE) + sizeof(SLinkerGetUserTokenReq);
	int nEncryptedPayloadLen = isCrypt ? ((nRawPayloadLen + 15) / 16) * 16 : nRawPayloadLen;

	SGSProtocolHead* pSGSHeader = (SGSProtocolHead*)buf;
	pSGSHeader->isCrypt = isCrypt ? 1 : 0;
	pSGSHeader->pktType = DATA_CTRL_PACKET;
	pSGSHeader->pduLen = nEncryptedPayloadLen;
	pSGSHeader->hton();

	SGSReqPayload* pReqpayload = (SGSReqPayload*)(buf + SGS_PROTO_HEAD_LEN);
	pReqpayload->cmdCode = LICENCE_LINKER_GET_TOKEN;
	pReqpayload->rawDataBytes = nRawPayloadLen;
	pReqpayload->token = m_communicationToken;  //������token
	pReqpayload->hton();

	SLinkerGetUserTokenReq* pGetToken = (SLinkerGetUserTokenReq*)((char*)pReqpayload + SGS_REQ_HEAD_LEN/*sizeof(SGSReqPayload) - sizeof(BYTE)*/);
	pGetToken->userId = userId;
	strcpy(pGetToken->username, username);
	pGetToken->hton();

	if (isCrypt)
		appEncryptDataWithKey((AES_BYTE*)(buf + SGS_PROTO_HEAD_LEN), nRawPayloadLen, m_key);

	//��������
	return GetServeMan()->SendSessionData(this, buf, SGS_PROTO_HEAD_LEN + nEncryptedPayloadLen) == CYATCP_OK ? LCS_OK : LCS_LINKER_NOT_LOGIN;
}

int  LicenceSession::NotifyUserOffline(UINT32 userId)
{
	BYTE buf[MAX_BUF_LEN] = { 0 };
	BOOL isCrypt = GetConfigFileMan()->IsDataEncrypt();
	int nRawPayloadLen = SGS_REQ_HEAD_LEN + sizeof(SLinkerNotifyUserOffline);
	int nEncryptedPayloadLen = isCrypt ? ((nRawPayloadLen + 15) / 16) * 16 : nRawPayloadLen;

	SGSProtocolHead* pSGSHeader = (SGSProtocolHead*)buf;
	pSGSHeader->isCrypt = isCrypt ? 1 : 0;
	pSGSHeader->pktType = DATA_CTRL_PACKET;
	pSGSHeader->pduLen = nEncryptedPayloadLen;
	pSGSHeader->hton();

	SGSReqPayload* pReqpayload = (SGSReqPayload*)(buf + SGS_PROTO_HEAD_LEN);
	pReqpayload->cmdCode = LICENCE_NOTIFY_USER_OFFLINE;
	pReqpayload->rawDataBytes = nRawPayloadLen;
	pReqpayload->token = m_communicationToken;  //������token
	pReqpayload->hton();

	SLinkerNotifyUserOffline* pOffline = (SLinkerNotifyUserOffline*)((char*)pReqpayload + SGS_REQ_HEAD_LEN);
	pOffline->linkerId = m_id;
	pOffline->userId = userId;
	pOffline->hton();

	if (isCrypt)
		appEncryptDataWithKey((AES_BYTE*)(buf + SGS_PROTO_HEAD_LEN), nRawPayloadLen, m_key);

	//��������
	return GetServeMan()->SendSessionData(this, buf, SGS_PROTO_HEAD_LEN + nEncryptedPayloadLen) == CYATCP_OK ? LCS_OK : LCS_LINKER_NOT_LOGIN;
}

int LicenceSession::GetUnlockJobs(UINT32 userId)
{
	BYTE buf[MAX_BUF_LEN] = { 0 };
	BOOL isCrypt = GetConfigFileMan()->IsDataEncrypt();
	int nRawPayloadLen = SGS_REQ_HEAD_LEN + sizeof(SLinkerQueryUnlockJobsReq);
	int nEncryptedPayloadLen = isCrypt ? ((nRawPayloadLen + 15) / 16) * 16 : nRawPayloadLen;

	SGSProtocolHead* pSGSHeader = (SGSProtocolHead*)buf;
	pSGSHeader->isCrypt = isCrypt ? 1 : 0;
	pSGSHeader->pktType = DATA_CTRL_PACKET;
	pSGSHeader->pduLen = nEncryptedPayloadLen;
	pSGSHeader->hton();

	SGSReqPayload* pReqpayload = (SGSReqPayload*)(buf + SGS_PROTO_HEAD_LEN);
	pReqpayload->cmdCode = LICENCE_QUERY_UNLOCK_JOBS;
	pReqpayload->rawDataBytes = nRawPayloadLen;
	pReqpayload->token = m_communicationToken;  //������token
	pReqpayload->hton();

	SLinkerQueryUnlockJobsReq* pUnlockJobsReq = (SLinkerQueryUnlockJobsReq*)((char*)pReqpayload + SGS_REQ_HEAD_LEN);
	pUnlockJobsReq->userId = userId;
	pUnlockJobsReq->roleId = 0;
	pUnlockJobsReq->type = ESGS_UNLOCK_ROLE_JOB;
	pUnlockJobsReq->hton();

	if (isCrypt)
		appEncryptDataWithKey((AES_BYTE*)(buf + SGS_PROTO_HEAD_LEN), nRawPayloadLen, m_key);

	//��������
	return GetServeMan()->SendSessionData(this, buf, SGS_PROTO_HEAD_LEN + nEncryptedPayloadLen) == CYATCP_OK ? LCS_OK : LCS_LINKER_NOT_LOGIN;
}
#endif

#if SEPARATOR("UserSession Interface")
int LicenceSession::SendUserEnterLinkerReply(UINT16 partitionId, UINT32 userId, const char* token, UINT32 linkerIp, UINT16 linkerPort, int retCode)
{
	if (retCode != LCS_OK)
		return SendCommonReply(LICENCE_USER_ENTER_PARTITION, retCode);

	BYTE buf[MAX_BUF_LEN] = { 0 };
	BOOL isCrypt = GetConfigFileMan()->IsDataEncrypt();
	int nRawPayloadLen = SGS_RES_HEAD_LEN + sizeof(SUserEnterPartitionRes);
	SUserEnterPartitionRes* pGetTokenRes = (SUserEnterPartitionRes*)(buf + SGS_BASIC_RES_LEN);
	pGetTokenRes->partitionId = partitionId;
	pGetTokenRes->linkerIp = linkerIp;
	pGetTokenRes->linkerPort = linkerPort;
	pGetTokenRes->userId = userId;
	strcpy(pGetTokenRes->token, token);
	pGetTokenRes->hton();

	return SendDataReply(buf, nRawPayloadLen, LICENCE_USER_ENTER_PARTITION, retCode, isCrypt);
}
#endif

#if SEPARATOR("Session Common Interface")
int LicenceSession::SendCommonReply(UINT16 cmdCode, UINT16 retCode, BOOL upToken/* = true*/, ESGSPacketType pktType /*= DATA_PACKET*/)
{
	BOOL isCrypt = GetConfigFileMan()->IsDataEncrypt();
	int nRawPayloadLen = SGS_RES_HEAD_LEN;
	int nEncryptedPayloadLen = ((nRawPayloadLen + 15) / 16) * 16;

	char buf[MAX_BUF_LEN] = { 0 };
	SGSProtocolHead* pSGSHeader = (SGSProtocolHead*)buf;
	pSGSHeader->isCrypt = isCrypt;
	pSGSHeader->pktType = pktType;
	pSGSHeader->pduLen = nEncryptedPayloadLen;
	pSGSHeader->hton();

	SGSResPayload* pResPayload = (SGSResPayload*)(buf + SGS_PROTO_HEAD_LEN);
	pResPayload->cmdCode = cmdCode;
	pResPayload->rawDataBytes = nRawPayloadLen;
	pResPayload->retCode = retCode;
	if (upToken) //����token
	{
		UINT32 u32Token = GetTickCount();
		pResPayload->token = u32Token;
		m_communicationToken = u32Token;
	}
	else
		pResPayload->token = m_communicationToken;
	pResPayload->hton();

	//����
	if (isCrypt)
		appEncryptDataWithKey((AES_BYTE*)(buf + SGS_PROTO_HEAD_LEN), nRawPayloadLen, m_key);

	//������Ӧ
	return GetServeMan()->SendSessionData(this, buf, SGS_PROTO_HEAD_LEN + nEncryptedPayloadLen);
}

int LicenceSession::SendDataReply(BYTE* buf, int nRawPayloadLen, UINT16 cmdCode, UINT16 retCode, BOOL upToken /*= true*/, BYTE pktType/* = DATA_PACKET*/)
{
	BOOL isCrypt = GetConfigFileMan()->IsDataEncrypt();
	int nEncryptedPayloadLen = ((nRawPayloadLen + 15) / 16) * 16;
	ASSERT(SGS_PROTO_HEAD_LEN + nEncryptedPayloadLen < MAX_BUF_LEN);

	SGSProtocolHead* pSGSHeader = (SGSProtocolHead*)buf;
	pSGSHeader->isCrypt = isCrypt ? 1 : 0;
	pSGSHeader->pktType = pktType;
	pSGSHeader->pduLen = nEncryptedPayloadLen;
	pSGSHeader->hton();

	SGSResPayload* pResPayload = (SGSResPayload*)(buf + SGS_PROTO_HEAD_LEN);
	pResPayload->cmdCode = cmdCode;
	pResPayload->rawDataBytes = nRawPayloadLen;
	pResPayload->retCode = (UINT16)retCode;

	if (upToken) //����token
	{
		UINT32 u32Token = GetTickCount();
		pResPayload->token = u32Token;
		m_communicationToken = u32Token;
	}
	else
		pResPayload->token = m_communicationToken;
	pResPayload->hton();

	//����
	if (isCrypt)
		appEncryptDataWithKey((AES_BYTE*)(buf + SGS_PROTO_HEAD_LEN), nRawPayloadLen, m_key);

	//������Ӧ
	return GetServeMan()->SendSessionData(this, buf, SGS_PROTO_HEAD_LEN + nEncryptedPayloadLen);
}
#endif
