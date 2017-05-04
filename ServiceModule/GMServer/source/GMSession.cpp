#include "GMSession.h"
#include "GMProtocal.h"
#include "AES.h"
#include "OtherServeConnMan.h"
#include "GMServeMan.h"
#include "ServiceCmdCode.h"
#include "ServiceErrorCode.h"
#include "RandWords.h"
#include "GMComm.h"

#define DEFAULT_DATA_SIZE 1024

CGMSessionMaker::CGMSessionMaker()
{

}


CGMSessionMaker::~CGMSessionMaker()
{

}

int CGMSessionMaker::GetSessionObjSize()
{
	return (int)sizeof(CCMSession);
}

void CGMSessionMaker::MakeSessionObj(void* session)
{
	::new(session)CCMSession();
}

CCMSession::CCMSession()
			:m_recvStatus(RECV_HEAD),
			m_userId(0),
			m_isLogin(FALSE)
{
	memset(&m_gmshead, 0, GET_GM_PROTOHEAD_SIZE);
	memset(m_cryptKey, 0, 33);
}

CCMSession::~CCMSession()
{

}

void CCMSession::OnConnected(SOCKET sock, const IPPADDR& peerIPP, int sessionCount, void* param /* = NULL */)
{
	LogInfo(("New socket connected[sock: %d], sessionCount:[%d]\n", sock, sessionCount));
}

void CCMSession::OnConnectedSend()
{

	UINT16 uncrptLen = GET_SGSPAYLOAD_RES_SIZE + GET_ANY_STRUC_SIZE(GMConntedTokenHeart, 0);
	UINT16 crptLen = BytesAlign(uncrptLen);
	int nSize = crptLen + GET_GM_PROTOHEAD_SIZE;
	char key[33] = { 0 };
	BYTE iscrpt = 1;

	std::string strKey;

	BYTE* buf = (BYTE*)CyaTcp_Alloc(nSize + 1);
	ASSERT(buf != NULL);
	memset(buf, 0, nSize + 1);

	GetRandWordsObj()->GetRandENConsistPasswd(32, strKey);
	strcpy(key, DEFAULT_TOKEN_KEY);
	strcpy(m_cryptKey, strKey.c_str());

	GMConntedTokenHeart* p = (GMConntedTokenHeart*)(buf + GET_SGSPAYLOAD_RES_SIZE + GET_GM_PROTOHEAD_SIZE);
	p->heatbeatInterval = GetCfgManObj()->GetHeartbeatInterval();
	memcpy(p->key, m_cryptKey, 32);
	p->hton();

	GMProtocalHead* phead = (GMProtocalHead*)buf;
	GM_PROTO_HEAD_DATA_MAN__(phead, crptLen, DATA_TOKEN_KEY_PACKET, 1, 1, 1);
	SGSResPayload* payload = (SGSResPayload*)(buf + GET_GM_PROTOHEAD_SIZE);
	SGS_PROTO_PAYLOAD_RES_DATA_MAN__(payload, uncrptLen, 0, 0, 0);

	EncryptDatapack(iscrpt, buf, uncrptLen, GET_GM_PROTOHEAD_SIZE, key, &crptLen);

	GetGMServeManObj()->GetGMServeMan()->SendSessionData(this, buf, crptLen);
	CyaTcp_Free(buf);
}


void CCMSession::OnClosed(int cause)
{
	LogInfo(("会话关闭客户端关闭"));
	RemoveSession(m_userId);
}

int CCMSession::RequestReceive(void*& buf, int& bufLen, IoReadType& rt, BOOL& asynNotify)
{
	if (m_recvStatus == RECV_HEAD)
	{
		buf = &m_gmshead;
		bufLen = GET_GM_PROTOHEAD_SIZE;
		rt = ICyaTcpSession::io_mustRead;
		asynNotify = false;
	}
	else if (m_recvStatus == RECV_BODY)
	{
		ASSERT(m_gmshead.pduLen > 0);
		buf = CyaTcp_Alloc(m_gmshead.pduLen + 1);
		ASSERT(buf != NULL);

		bufLen = m_gmshead.pduLen;
		rt = ICyaTcpSession::io_mustRead;
		asynNotify = true;
	}
	else
		ASSERT(false);

	return 0;
}

int CCMSession::OnReceived(void* buf, int bufLen, BOOL callbackByTask, BOOL& continueRecv)
{
	continueRecv = true;
	if (m_recvStatus == RECV_HEAD)
	{
		m_gmshead.ntoh();
		if (m_gmshead.pktType == DATA_HEARTBEAT_PACKET)
		{
			m_recvStatus = RECV_HEAD;
			OnHeartbeatMan();
			return 0;
		}
		if (m_gmshead.pduLen == 0)
		{
			m_recvStatus = RECV_HEAD;
			ASSERT(false);
			return -1;
		}
		else
			m_recvStatus = RECV_BODY;
	}
	else if (m_recvStatus == RECV_BODY)
	{
		m_recvStatus = RECV_HEAD;
		if (bufLen != m_gmshead.pduLen)
		{
			ASSERT(false);
			return -1;
		}
		ProcessDataMan(buf, bufLen, m_gmshead.req);

	}
	else
		ASSERT(false);
	return 0;
}

void CCMSession::OnSent(const void* buf, int bufLen, BOOL isCopy, void* context, BOOL isSendSuccess)
{

}

int CCMSession::ProcessDataMan(void* data, int dataLen, DWORD req)
{
	if (m_gmshead.isCrypt)
		appDecryptDataWithKey((AES_BYTE*)data, dataLen, (ANSICHAR*)m_cryptKey);

	SGSReqPayload* p = (SGSReqPayload*)data;
	p->ntoh();

	int ret = GM_OK;

	if (p->cmdCode == GM_USER_LOGIN)
		ret = OnLogin(data, dataLen, req);
	else if (m_gmshead.pktType == DATA_PACKET)
		ret = OnMsgRequestMan(p->cmdCode, data, dataLen, m_gmshead.req);
	else
		ErrorMan(0, ret, req);

	if (ret != GM_OK)
		ErrorMan(p->cmdCode, ret, req);

	return 0;
}

void CCMSession::ErrorMan(UINT16 cmdCode, UINT16 retCode, DWORD req)
{
	BYTE* buf = (BYTE*)CyaTcp_Alloc(DEFAULT_DATA_SIZE);
	ASSERT(buf != NULL);
	memset(buf, 0, DEFAULT_DATA_SIZE);

	UINT16 uncrptLen = GET_SGSPAYLOAD_RES_SIZE;
	UINT16 crptLen = BytesAlign(uncrptLen);
	BYTE iscrpt = 1;

	GMProtocalHead* phead = (GMProtocalHead*)buf;
	GM_PROTO_HEAD_DATA_MAN__(phead, crptLen, DATA_PACKET, 1, 1, req);
	SGSResPayload* payload = (SGSResPayload*)(buf + GET_GM_PROTOHEAD_SIZE);
	SGS_PROTO_PAYLOAD_RES_DATA_MAN__(payload, uncrptLen, 0, cmdCode, retCode);

	EncryptDatapack(iscrpt, buf, uncrptLen, GET_GM_PROTOHEAD_SIZE, m_cryptKey, &crptLen);

	GetGMServeManObj()->GetGMServeMan()->SendSessionData(this, buf, crptLen);
	CyaTcp_Free(buf);
}

int CCMSession::PasswordVerify(const char* name, const char* passwd)
{
	/*char sql[1024] = { 0 };

	sprintf(sql, "select id, passwd from gm_account where name='%s'", name);
	GetOtherServeManObj()->GetExtSqlObj(DBS_MSG_QUERY, DB_SGSINFO, sql);

	std::vector<DBS_RESULT>::iterator it = GetOtherServeManObj()->GetDBSResult().begin();
	if(GetOtherServeManObj()->GetDBSResult().size() == 0)
	return GM_ACCOUNT_INVALID;

	DBS_RESULT result = *it;
	DBS_ROW row = NULL;
	if(result != NULL)
	{
	row = DBS_FetchRow(result);
	if(row == NULL)
	return GM_USER_LOGIN_FAILED;
	if(strcmp(row[1], passwd) != 0)
	return GM_USER_LOGIN_FAILED;
	}else
	return GM_USER_LOGIN_FAILED;

	m_userId = atoi(row[0]);
	CXTAutoLock locked(m_locker);
	m_sessionMan.insert(std::make_pair(m_userId, this));*/
	return GM_OK;
}

int CCMSession::OnLogin(void* data, int dataLen, DWORD req)
{
	ASSERT(data != NULL);
	if (data == NULL)
		return GM_USER_LOGIN_FAILED;

	//用户身份验证
	GMUserLoginReq* plgReq = (GMUserLoginReq*)((BYTE*)data + member_offset(SGSReqPayload, rawDataBytes, data));
	ASSERT(plgReq != NULL);
	int ret = PasswordVerify(plgReq->name, plgReq->passwd);
	//GetOtherServeManObj()->ClearDBSResult();

	UINT16 uncrptLen = GET_SGSPAYLOAD_RES_SIZE;
	UINT16 cryptLen = BytesAlign(uncrptLen);
	BYTE isCrypt = 1;

	int nSize = cryptLen + GET_GM_PROTOHEAD_SIZE;
	BYTE* buf = (BYTE*)CyaTcp_Alloc(nSize + 1);
	ASSERT(buf != NULL);
	GMProtocalHead* phead = (GMProtocalHead*)buf;
	GM_PROTO_HEAD_DATA_MAN__(phead, cryptLen, DATA_PACKET, isCrypt, req, 1);
	SGSResPayload* payload = (SGSResPayload*)(buf + GET_GM_PROTOHEAD_SIZE);
	SGS_PROTO_PAYLOAD_RES_DATA_MAN__(payload, uncrptLen, 0, GM_USER_LOGIN, GM_OK);

	EncryptDatapack(isCrypt, buf, uncrptLen, GET_GM_PROTOHEAD_SIZE, m_cryptKey, &cryptLen);

	GetGMServeManObj()->GetGMServeMan()->SendSessionData(this, buf, cryptLen);
	CyaTcp_Free(buf);

	return ret;
}

void CCMSession::RemoveSession(UINT32 userId)
{
	CXTAutoLock locked(m_locker);
	std::map<UINT32, CCMSession*>::iterator it = m_sessionMan.find(userId);
	if (it != m_sessionMan.end())
		m_sessionMan.erase(userId);
}

int CCMSession::OnMsgRequestMan(UINT16 cmdCode, void* data, int dataLen, DWORD req)
{
	return m_processDataMan.OnMsgProcessDataMan(cmdCode, data, dataLen, this, req);
}

void CCMSession::EncryptDatapack(BYTE isCrypt, BYTE*& buf, UINT16 uncryptLen, UINT16 sourceHead, char* key, UINT16* cryptLen)
{
	if (isCrypt)
		appEncryptDataWithKey((AES_BYTE*)buf + sourceHead, uncryptLen, key);

	if (isCrypt)
		*cryptLen += sourceHead;
	else
		*cryptLen = uncryptLen + sourceHead;
}

void CCMSession::OnHeartbeatMan()
{
	UINT16 uncrptLen = GET_GM_PROTOHEAD_SIZE;
	BYTE* buf = (BYTE*)CyaTcp_Alloc(uncrptLen + 1);
	ASSERT(buf != NULL);

	GMProtocalHead* p = (GMProtocalHead*)buf;
	GM_PROTO_HEAD_DATA_MAN__(p, GET_GM_PROTOHEAD_SIZE, DATA_HEARTBEAT_PACKET, 0, 0, 1);

	GetGMServeManObj()->GetGMServeMan()->SendSessionData(this, buf, uncrptLen);
	LogInfo(("Server心跳检测\n"));
}
