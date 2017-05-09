/*
/// 一个Session对应一个UserID
/// 后面一个登陆会踢掉前一个，具体策略见LogIn
*/
#include "LinkSession.h"
#include "AES.h"
#include "cyaLog.h"

#include "ServiceErrorCode.h"
#include "MsgProcess.h"
#include "RandomString.h"
#include "PubFunction.h"
#include "TokenMan.h"
#include "LinkConfig.h"
#include "LinkGlobalObj.h"

#define VALID_BUF_WORK_RET(bufName) \
if (NULL == bufName) { \
	LogWarning(("内存不足")); \
	return 0; \
}

#define VALID_BUF_WORK(bufName) \
if (NULL == bufName) { \
	LogWarning(("内存不足")); \
	return; \
}

LinkSession::LinkSession()
	: m_ePacketContrl(CYA_HEAD)
	, m_bLongin(false)
	, m_nUserID(0)
{

}

LinkSession::~LinkSession()
{

}

void LinkSession::OnConnected(SOCKET sock, const IPPADDR& peerIPP, int sessionCount, void* param /*= NULL*/)
{
	ASSERT(NULL != MsgProcess::FetchMsgProcess());
	ASSERT(NULL != TokenMan::FetchTokenMan());
	ASSERT(SockValid(sock) && 0 < sessionCount);
	CreateRandomString(m_szKey, DEFAULT_KEY_SIZE);
	m_szKey[DEFAULT_KEY_SIZE] = '\0';
	LogDebug(("New Session Client Arrived !!  Key(%s)", m_szKey));
}

void LinkSession::OnConnectedSend()
{
	CreateRandomString(m_szToken, DEFAULT_TOKEN_SIZE);
	m_szToken[DEFAULT_TOKEN_SIZE] = '\0';
	BYTE* pResponsePacket = NULL;
	UINT16 nPayloadLen = OnCreateKeyPacket(pResponsePacket, m_szToken, m_szKey);
	if (0 < nPayloadLen)
	{
		InputPacket(pResponsePacket, nPayloadLen);
	}
	if (pResponsePacket)
	{
		CyaTcp_Free(pResponsePacket);
	}
}

void LinkSession::OnClosed(int cause)
{
	LogDebug(("Session Client Leaved!! %d", cause));
	m_ePacketContrl = CYA_HEAD;
	if (m_bLongin)
	{
		LogInfo(("User[%d] session closed!", m_nUserID));
		MsgProcess* pMsgPrc = MsgProcess::FetchMsgProcess();
		if (pMsgPrc)   /// 没有登陆的时候，不用通知
		{
			pMsgPrc->MsgProcessSite(NULL, 0, MLS_LEAVE_ROLE, DATA_PACKET, m_nUserID, false);
			pMsgPrc->NotifyLicenseUserExit(m_nUserID);  /// 通知验证服务器，用户退出
			pMsgPrc->NotifyChatUserExit(m_nUserID);     /// 通知聊天服务器，用户退出
			pMsgPrc->UnRegistSession(this, m_nUserID);  /// 注销用户信息
		}
	}
	else
	{
		if (0 != m_nUserID)
		{
			TokenMan::FetchTokenMan()->UserOffline(m_nUserID);
		}
	}
}

int  LinkSession::RequestReceive(void*& buf, int& bufLen, IoReadType& rt, BOOL& asynNotify)
{
	rt = io_mustRead;
	if (CYA_HEAD == m_ePacketContrl)
	{
		memset(&m_proHeader, 0, SGS_PROTO_HEAD_LEN);
		asynNotify = false;
		buf = &m_proHeader;
		bufLen = SGS_PROTO_HEAD_LEN;
	}
	else if (CYA_BODY == m_ePacketContrl)
	{
		asynNotify = true;
		buf = CyaTcp_Alloc(m_proHeader.pduLen + 1);
		bufLen = m_proHeader.pduLen;
	}
	else
		ASSERT(false);
	return 0;
}

int  LinkSession::OnReceived(void* buf, int bufLen, BOOL callbackByTask, BOOL& continueRecv)
{
	ASSERT(NULL != buf && 0 < bufLen);
	if (NULL == buf || 0 >= bufLen)
		return -1;

	continueRecv = true;
	if (CYA_HEAD == m_ePacketContrl)
	{
		ASSERT(SGS_PROTO_HEAD_LEN == bufLen);
		m_proHeader.ntoh();
		m_ePacketContrl = CYA_BODY;
	}
	else if (CYA_BODY == m_ePacketContrl)
	{
		ASSERT(bufLen == m_proHeader.pduLen);
		int nRetCode = 0;
		nRetCode = OnRoughMsgSite(buf, bufLen, m_proHeader.isCrypt, m_szKey, m_proHeader.pktType);
		CyaTcp_Free(buf);
		m_ePacketContrl = CYA_HEAD;
		return nRetCode < 0 ? -1 : 0;
	}
	else
		ASSERT(FALSE);
	return 0;
}

void LinkSession::InputPacket(bool encrypt, UINT16 nPktType, UINT16 nCmdCode, UINT16 nRetCode, const void* pPktData, UINT16 nPktLen)
{
	if (encrypt)
		OnInputEncryptPakcet(nPktType, nCmdCode, nRetCode, pPktData, nPktLen);
	else
		OnInputUnencryptPakcet(nPktType, nCmdCode, nRetCode, pPktData, nPktLen);
}

void LinkSession::InputPacket(const void* pPktData, UINT16 nPktLen)
{
	ASSERT(NULL != pPktData && 0 < nPktLen);
	GetServeMan()->SendSessionData(this, pPktData, nPktLen);
}

// 收到User消息调用相应的Client回调处理函数
int LinkSession::OnRoughMsgSite(void* buf, int bufLen, bool isCrypt, BYTE* key, UINT16 nPktType)
{
	ASSERT(NULL != buf && REQ_HEAD_SIZE < bufLen);
	int recvLen = bufLen;
	if (isCrypt)
	{
		appDecryptDataWithKey((BYTE*)buf, recvLen, (char*)key);
	}

	SGSReqPayload* pReqHead = (SGSReqPayload*)buf;
	pReqHead->ntoh();

	//  if (CHAT_SEND_MESSAGE != pReqHead->cmdCode &&   /// 聊天信息不更新Token，异步的
	//      !EqualToken(pReqHead->token, m_szToken, DEFAULT_TOKEN_SIZE))
	//  {
	//      LogInfo(("接收到错误的TOKEN, 命令%d", pReqHead->cmdCode));
	//      return -1;
	//  }

	MsgProcess* pMsgPrc = MsgProcess::FetchMsgProcess();
	ASSERT(NULL != pMsgPrc);
	// LinkServer连接LIcenseServer之前GMServer就可能来查询
	if (LINKER_GET_ONLINE_CONNECT_NUM == pReqHead->cmdCode)
	{
		return OnGetCurrentLinkerOnlineNum(pMsgPrc->GetOnlineUserNum());
	}

	int msgLen = pReqHead->rawDataBytes - SGS_REQ_HEAD_LEN;
	if (LINKER_LOGIN == pReqHead->cmdCode)
	{
		return OnLogIn(pReqHead->data, msgLen);
	}

	if (!m_bLongin)
	{
		LogDebug(("未验证的客户请求"));
		return ResReturnCode(isCrypt, nPktType, pReqHead->cmdCode, LINK_ACCOUNT_TOKEN);
	}
	if (LINKER_LOGOUT == pReqHead->cmdCode)
	{
		return OnLogOut();
	}
	if (DATA_HEARTBEAT_PACKET == nPktType)
	{
		ASSERT(LINKER_HEARTBEAT == pReqHead->cmdCode);
		GetServeMan()->UpdateSessionKeepAliveTick(this);
		return OnHeartBeat();
	}
	if (MLS_LEAVE_ROLE == pReqHead->cmdCode)
	{
		pMsgPrc->MsgProcessSite(NULL, 0, MLS_LEAVE_ROLE, DATA_PACKET, m_nUserID, false);
		pMsgPrc->UnRegistRole(m_nUserID);
		return LINK_OK;
	}

	return pMsgPrc->MsgProcessSite(pReqHead->data, msgLen, pReqHead->cmdCode, nPktType, m_nUserID, false);
}

int LinkSession::ResReturnCode(bool bEncrypt, UINT16 nPktType, UINT16 nCmdCode, UINT16 nRetCode)
{
	BYTE gPacketBuf[128] = { 0 };
	UINT16 bytePayloadLen = GetAlignedLen(bEncrypt, SGS_RES_HEAD_LEN, 16);
	UINT16 allPayloadLen = SGS_PROTO_HEAD_LEN + bytePayloadLen;
	if (CHAT_SEND_MESSAGE != nCmdCode)  /// 聊天信息不更新Token，异步的
	{
		CreateRandomString(m_szToken, DEFAULT_TOKEN_SIZE);
		m_szToken[DEFAULT_TOKEN_SIZE] = '\0';
	}
	memset(gPacketBuf, 0, allPayloadLen + sizeof(BYTE));

	SGSProtocolHead* pProHead = (SGSProtocolHead*)gPacketBuf;
	pProHead->isCrypt = bEncrypt;
	pProHead->pktType = nPktType;
	pProHead->pduLen = bytePayloadLen;
	pProHead->hton();

	SGSResPayload* pResPayload = (SGSResPayload*)(gPacketBuf + SGS_PROTO_HEAD_LEN);
	pResPayload->cmdCode = nCmdCode;
	pResPayload->retCode = nRetCode;
	pResPayload->rawDataBytes = SGS_RES_HEAD_LEN;
	memcpy(&(pResPayload->token), m_szToken, DEFAULT_TOKEN_SIZE);
	pResPayload->hton();
	appEncryptDataWithKey((AES_BYTE*)(gPacketBuf + SGS_PROTO_HEAD_LEN), SGS_RES_HEAD_LEN, (char*)m_szKey);

	InputPacket(gPacketBuf, allPayloadLen);
	LogDebug(("回复命令(%d)错误码(%d)", nCmdCode, nRetCode));
	return 0;
}

int LinkSession::OnLogIn(const void* pLoginData, UINT16 nDataLen)
{
	if (m_bLongin)
	{
		return ResReturnCode(true, DATA_PACKET, LINKER_LOGIN, LINK_OK);
	}

	ASSERT(NULL != pLoginData && nDataLen == sizeof(SLoginLinkerReq));
	SLoginLinkerReq* ReqPayload = (SLoginLinkerReq*)pLoginData;
	ReqPayload->ntoh();

	MsgProcess* pMsgProc = MsgProcess::FetchMsgProcess();
	ASSERT(NULL != pMsgProc);
	if (NULL == pMsgProc)
	{
		return ResReturnCode(true, DATA_PACKET, LINKER_LOGIN, LINK_ERROR);
	}
	LogDebug(("User[%d] 登陆", ReqPayload->userId));
	TokenMan* pTokenMgr = TokenMan::FetchTokenMan();
	ASSERT(NULL != pTokenMgr);
	if (!pTokenMgr->UserOnline(ReqPayload->userId, ReqPayload->token))
	{
		LogInfo(("User[%d] login invalid token[%s]", ReqPayload->userId, ReqPayload->token));
		return ResReturnCode(true, DATA_PACKET, LINKER_LOGIN, LINK_TOKEN_INVALID);
	}

	m_nUserID = ReqPayload->userId;
	bool bRepeat = false;
	m_bLongin = pMsgProc->RegistSession(this, m_nUserID, bRepeat); /// 用用户ID注册, 重复则踢掉前一个
	if (!bRepeat)
		pMsgProc->NotifyLicenseUserLogin(m_nUserID);    /// 通知验证服务器,告知其它Linker踢人

	return ResReturnCode(true, DATA_PACKET, LINKER_LOGIN, m_bLongin ? LINK_OK : LINK_TOKEN_INVALID);
}

int LinkSession::OnLogOut()
{
	TokenMan* pTokenMgr = TokenMan::FetchTokenMan();
	ASSERT(NULL != pTokenMgr);
	if (NULL != pTokenMgr)
		pTokenMgr->UserOffline(m_nUserID);

	MsgProcess* pMsgProc = MsgProcess::FetchMsgProcess();
	ASSERT(NULL != pMsgProc);
	if (NULL != pMsgProc)
	{
		pMsgProc->MsgProcessSite(NULL, 0, MLS_LEAVE_ROLE, DATA_PACKET, m_nUserID, false);
		pMsgProc->NotifyLicenseUserExit(m_nUserID);  /// 通知验证服务器，用户退出
		pMsgProc->NotifyChatUserExit(m_nUserID);     /// 通知聊天服务器，用户退出
		pMsgProc->UnRegistSession(this, m_nUserID);
	}

	m_bLongin = false;
	m_nUserID = 0;
	return ResReturnCode(true, DATA_PACKET, LINKER_LOGOUT, LINK_OK);
}

int LinkSession::OnHeartBeat()
{
	UINT16 nAlignedLen = GetAlignedLen(true, SGS_RES_HEAD_LEN);
	UINT16 nPayloadLen = SGS_PROTO_HEAD_LEN + nAlignedLen;
	BYTE* pResponsePacketBuff = (BYTE*)CyaTcp_Alloc(nPayloadLen);
	VALID_BUF_WORK_RET(pResponsePacketBuff)

		CreateRandomString(m_szToken, DEFAULT_TOKEN_SIZE);
	m_szToken[DEFAULT_TOKEN_SIZE] = '\0';

	memset(pResponsePacketBuff, 0, nPayloadLen);
	SGSProtocolHead* pProHead = (SGSProtocolHead*)pResponsePacketBuff;
	pProHead->isCrypt = true;
	pProHead->pktType = DATA_HEARTBEAT_PACKET;
	pProHead->pduLen = nAlignedLen;
	pProHead->hton();

	SGSResPayload* pResPayload = (SGSResPayload*)(pResponsePacketBuff + SGS_PROTO_HEAD_LEN);
	pResPayload->cmdCode = LINKER_HEARTBEAT;
	pResPayload->retCode = 0;
	pResPayload->rawDataBytes = SGS_RES_HEAD_LEN;
	pResPayload->token = *(UINT32*)m_szToken;
	pResPayload->hton();

	appEncryptDataWithKey(pResponsePacketBuff + SGS_PROTO_HEAD_LEN, SGS_RES_HEAD_LEN, (ANSICHAR*)m_szKey);

	InputPacket(pResponsePacketBuff, SGS_PROTO_HEAD_LEN + nAlignedLen);

	CyaTcp_Free(pResponsePacketBuff);

	return 0;
}

int LinkSession::OnGetCurrentLinkerOnlineNum(UINT32 onlineCount)
{
	UINT16 nAlignedLen = GetAlignedLen(true, SGS_RES_HEAD_LEN);
	UINT16 nPayloadLen = SGS_PROTO_HEAD_LEN + nAlignedLen;
	BYTE* pResponsePacketBuff = (BYTE*)CyaTcp_Alloc(nPayloadLen);
	VALID_BUF_WORK_RET(pResponsePacketBuff)

		memset(pResponsePacketBuff, 0, nPayloadLen);
	SGSProtocolHead* pProHead = (SGSProtocolHead*)pResponsePacketBuff;
	pProHead->isCrypt = true;
	pProHead->pktType = DATA_PACKET;
	pProHead->pduLen = nAlignedLen;
	pProHead->hton();

	SGSResPayload* pResPayload = (SGSResPayload*)(pResponsePacketBuff + SGS_PROTO_HEAD_LEN);
	pResPayload->cmdCode = LINKER_GET_ONLINE_CONNECT_NUM;
	pResPayload->retCode = 0;
	pResPayload->rawDataBytes = SGS_RES_HEAD_LEN;
	pResPayload->token = onlineCount;
	pResPayload->hton();

	appEncryptDataWithKey(pResponsePacketBuff + SGS_PROTO_HEAD_LEN, SGS_RES_HEAD_LEN, (ANSICHAR*)m_szKey);

	InputPacket(pResponsePacketBuff, SGS_PROTO_HEAD_LEN + nAlignedLen);

	CyaTcp_Free(pResponsePacketBuff);

	return 0;
}

UINT16 LinkSession::OnCreateKeyPacket(BYTE*& buf, const BYTE* token, const BYTE* data)
{
	UINT16 nAlignedLen = GetAlignedLen(true, SGS_RES_HEAD_LEN + sizeof(SConnectLinkerRes));
	UINT16 nPayloadLen = SGS_PROTO_HEAD_LEN + nAlignedLen;
	BYTE* pResponsePacketBuff = (BYTE*)CyaTcp_Alloc(nPayloadLen);
	VALID_BUF_WORK_RET(pResponsePacketBuff)

		memset(pResponsePacketBuff, 0, nPayloadLen);
	SGSProtocolHead* pProHead = (SGSProtocolHead*)pResponsePacketBuff;
	pProHead->isCrypt = true;
	pProHead->pktType = DATA_TOKEN_KEY_PACKET;
	pProHead->pduLen = nAlignedLen;
	pProHead->hton();

	SGSResPayload* pResPayload = (SGSResPayload*)(pResponsePacketBuff + SGS_PROTO_HEAD_LEN);
	pResPayload->cmdCode = DATA_TOKEN_KEY_PACKET;
	pResPayload->retCode = 0;
	pResPayload->rawDataBytes = SGS_RES_HEAD_LEN + sizeof(SConnectLinkerRes);
	pResPayload->token = *(UINT32*)token;
	pResPayload->hton();

	SConnectLinkerRes* pConRes = (SConnectLinkerRes*)(pResponsePacketBuff + SGS_PROTO_HEAD_LEN + SGS_RES_HEAD_LEN);
	pConRes->heartSec = GlobalGetLinkConfig()->GetClientHeartBeatTimeout();
	memcpy(pConRes->key, data, DEFAULT_KEY_SIZE);
	pConRes->hton();
	appEncryptDataWithKey(pResponsePacketBuff + SGS_PROTO_HEAD_LEN, SGS_RES_HEAD_LEN + sizeof(SConnectLinkerRes), DEFAULT_TOKEN_KEY);
	buf = pResponsePacketBuff;

	return nPayloadLen;
}

void LinkSession::OnInputUnencryptPakcet(UINT16 nPktType, UINT16 nCmdCode, UINT16 nRetCode, const void* pPktData, UINT16 nPktLen)
{
	UINT16 srcPayloadLen = SGS_RES_HEAD_LEN + nPktLen;
	BYTE* gPacketBuf = (BYTE*)CyaTcp_Alloc(srcPayloadLen + SGS_PROTO_HEAD_LEN + sizeof(BYTE));
	ASSERT(NULL != gPacketBuf);
	VALID_BUF_WORK(gPacketBuf)

	if (CHAT_SEND_MESSAGE != nCmdCode)  /// 聊天信息不更新Token，异步的
	{
		CreateRandomString(m_szToken, DEFAULT_TOKEN_SIZE);
		m_szToken[DEFAULT_TOKEN_SIZE] = '\0';
	}

	memset(gPacketBuf, 0, srcPayloadLen + SGS_PROTO_HEAD_LEN + sizeof(BYTE));
	SGSProtocolHead* pProHead = (SGSProtocolHead*)gPacketBuf;
	pProHead->isCrypt = false;
	pProHead->pktType = nPktType;
	pProHead->pduLen = srcPayloadLen;
	pProHead->hton();

	SGSResPayload* pResPayload = (SGSResPayload*)(gPacketBuf + SGS_PROTO_HEAD_LEN);
	pResPayload->cmdCode = nCmdCode;
	pResPayload->retCode = nRetCode;
	pResPayload->rawDataBytes = srcPayloadLen;
	memcpy(&(pResPayload->token), m_szToken, DEFAULT_TOKEN_SIZE);
	if (NULL != pPktData && 0 < nPktLen)
	{
		memcpy(pResPayload->data, pPktData, nPktLen);
	}
	pResPayload->hton();
	InputPacket(gPacketBuf, srcPayloadLen + SGS_PROTO_HEAD_LEN);
	CyaTcp_Free(gPacketBuf);
}

void LinkSession::OnInputEncryptPakcet(UINT16 nPktType, UINT16 nCmdCode, UINT16 nRetCode, const void* pPktData, UINT16 nPktLen)
{
	UINT16 srcPayloadLen = SGS_RES_HEAD_LEN + nPktLen;
	UINT16 bytePayloadLen = GetAlignedLen(true, srcPayloadLen, 16);/// 16字节对齐
	BYTE* gPacketBuf = (BYTE*)CyaTcp_Alloc(bytePayloadLen + SGS_PROTO_HEAD_LEN + sizeof(BYTE));
	ASSERT(NULL != gPacketBuf);
	VALID_BUF_WORK(gPacketBuf)

	if (CHAT_SEND_MESSAGE != nCmdCode)  /// 聊天信息不更新Token，异步的
	{
		CreateRandomString(m_szToken, DEFAULT_TOKEN_SIZE);
		m_szToken[DEFAULT_TOKEN_SIZE] = '\0';
	}

	memset(gPacketBuf, 0, bytePayloadLen + SGS_PROTO_HEAD_LEN + sizeof(BYTE));
	SGSProtocolHead* pProHead = (SGSProtocolHead*)gPacketBuf;
	pProHead->isCrypt = true;
	pProHead->pktType = nPktType;
	pProHead->pduLen = bytePayloadLen;
	pProHead->hton();

	SGSResPayload* pResPayload = (SGSResPayload*)(gPacketBuf + SGS_PROTO_HEAD_LEN);
	pResPayload->cmdCode = nCmdCode;
	pResPayload->retCode = nRetCode;
	pResPayload->rawDataBytes = srcPayloadLen;
	memcpy(&(pResPayload->token), m_szToken, DEFAULT_TOKEN_SIZE);
	if (NULL != pPktData && 0 < nPktLen)
	{
		memcpy(pResPayload->data, pPktData, nPktLen);
	}
	pResPayload->hton();
	appEncryptDataWithKey(gPacketBuf + SGS_PROTO_HEAD_LEN, srcPayloadLen, (char*)m_szKey);
	InputPacket(gPacketBuf, bytePayloadLen + SGS_PROTO_HEAD_LEN);
	CyaTcp_Free(gPacketBuf);
}