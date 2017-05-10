#include "DelegetMsg.h"
#include "PubFunction.h"
#include "ExtServerMan.h"
#include "ExtServerTcpSession.h"
#include "ServiceErrorCode.h"
#include "GenerateString.h"
#include "AES.h"

#define INVALID_COOKIE 0

DelegetMessage* DelegetMessage::m_this = NULL;

DelegetMessage* DelegetMessage::InstanceDeleteget()
{
	if (NULL == m_this)
	{
		m_this = new DelegetMessage();
	}
	ASSERT(NULL != m_this);
	return m_this;
}

void DelegetMessage::DeleteThis()
{
	if (m_this)
	{
		delete m_this;
		m_this = NULL;
	}
}

DelegetMessage::DelegetMessage()
{

}

DelegetMessage::~DelegetMessage()
{
	UserCookieMap::iterator itr = m_userCookies.begin();
	for (itr; m_userCookies.end() != itr; ++itr)
	{
		delete itr->second;
	}
	m_userCookies.clear();
}

#define VALID_EXTSERVER_COMMAND(cmd) ((0x61A9)<=(cmd))
/// 只作错误处理, 登陆，登出，其它转发
INT DelegetMessage::DelegetMsgHandle(const void* msgDaga, INT msgLen, void* theSession, UINT32 userId = 0)
{
	ASSERT(msgDaga && REQ_HEAD_SIZE <= msgLen && theSession);
	if (NULL == theSession)
	{
		return EXT_OK;
	}

	SGSReqPayload* pLoad = (SGSReqPayload*)msgDaga;
	pLoad->ntoh();
	if (!VALID_EXTSERVER_COMMAND(pLoad->cmdCode))
	{
		return ResponseErrMsg(pLoad->token, pLoad->cmdCode, EXT_COMAND, theSession, DEFAULT_TOKEN_KEY);
	}

	if (MANS_USER_LOGIN == pLoad->cmdCode)
	{
		SExtManLoginReq* pLogin = (SExtManLoginReq*)pLoad->data;
		UINT32 token = 0;
		if (VaildAccount(pLogin->account, token))
		{
			USER_INFO* info = NULL;
			if (EXT_OK == Login(token, theSession, info) && info)
			{
				SExtManLoginRes res;
				res.loginToken = token;
				memcpy(res.key, info->m_key, DEFAULT_KEY_SIZE);
				res.key[DEFAULT_KEY_SIZE] = '\0';
				res.hton();
				return ResponseDataMsg(theSession, &res, sizeof(SExtManLoginRes), MANS_USER_LOGIN, token, DEFAULT_TOKEN_KEY);
			}
		}
		ResponseErrMsg(pLoad->token, MANS_USER_LOGIN, EXT_ACCOUNT, theSession, DEFAULT_TOKEN_KEY);
	}
	else if (MANS_USER_LOGOUT == pLoad->cmdCode)	/// 登出
	{
		char key[DEFAULT_KEY_SIZE + 1] = { 0 };
		INT retCode = Logout(userId, key);
		ResponseErrMsg(userId, MANS_USER_LOGOUT, retCode, theSession, strlen(key)>0 ? key : DEFAULT_TOKEN_KEY);
	}
	else
	{
		ExtServerMan::InstanceServerMan()->PushSessionMsg(pLoad->data, pLoad->rawDataBytes - SGS_REQ_HEAD_LEN, pLoad->cmdCode, userId);
	}
	return EXT_OK;
}

BOOL DelegetMessage::ValidSession(UINT32 cookie)
{
	ASSERT(INVALID_COOKIE != cookie);
	CXTAutoLock locke(m_mapLocker);
	UserCookieMap::iterator itr = m_userCookies.find(cookie);

	return m_userCookies.end() != itr;
}

const char* DelegetMessage::FindEncryptKey(UINT32 cookie)
{
	ASSERT(INVALID_COOKIE != cookie);
	CXTAutoLock locke(m_mapLocker);
	UserCookieMap::iterator itr = m_userCookies.find(cookie);
	if (m_userCookies.end() != itr)
	{
		return itr->second->m_key;
	}
	return NULL;
}

void* DelegetMessage::FindSession(UINT32 cookie)
{
	ASSERT(INVALID_COOKIE != cookie);
	CXTAutoLock locke(m_mapLocker);
	UserCookieMap::iterator itr = m_userCookies.find(cookie);
	if (m_userCookies.end() != itr)
	{
		return itr->second->m_session;
	}
	return NULL;
}

void DelegetMessage::UpdateSessionLot(UINT32 cookie, void* theSession)
{
	ASSERT(INVALID_COOKIE != cookie);
	CXTAutoLock locke(m_mapLocker);
	UserCookieMap::iterator itr = m_userCookies.find(cookie);
	if (m_userCookies.end() != itr)
	{
		itr->second->m_session = theSession;
	}
}

static const UINT32 TokenCryptBase = 0xAB;

BOOL DelegetMessage::VaildAccount(const char* account, UINT32& token)
{
	UINT16 endMarkIndex = 0;
	const char* replica = account;	//// 复制品
	token = 0;
	while (NULL != replica)
	{
		endMarkIndex++;
		if ('\0' == *replica || endMarkIndex > 33)
		{
			break;
		}
		token += (*replica) & TokenCryptBase;
		replica++;
	}
	if (endMarkIndex <= 32 && token > 0)
	{
		return TRUE;
	}

	token = 0;
	return FALSE;
}

INT  DelegetMessage::Login(UINT32 token, void* theSession, USER_INFO*& info)
{
	info = new USER_INFO();
	info->m_cookie = token;
	info->m_session = theSession;
	GenerateRandomString((BYTE*)(info->m_key), DEFAULT_KEY_SIZE);
	return AddUserInfo(token, info) ? EXT_OK : EXT_LOGIN_REPEATED;
}

INT  DelegetMessage::Logout(UINT32 cookie, char* key)
{
	ASSERT(INVALID_COOKIE != cookie);
	CXTAutoLock locke(m_mapLocker);
	UserCookieMap::iterator itr = m_userCookies.find(cookie);
	if (m_userCookies.end() != itr)
	{
		memcpy(key, itr->second->m_key, DEFAULT_KEY_SIZE);
		delete itr->second;
		m_userCookies.erase(itr);
		return EXT_OK;
	}
	return EXT_NOT_LOGIN;
}

UINT32 DelegetMessage::GetSessionCookie(const void* msgData, INT msgLen)
{
	ASSERT(msgData && sizeof(UINT32) < msgLen);
	UINT32* p = (UINT32*)msgData;
	return *p;
}

#define CHECK_BUFF_VALID(buffName) \
	ASSERT(NULL != buffName); \
if (NULL == buffName) return 0;

#define PROTOCOL_ASSIGN_DEFUALT(pHead, payloadLen) \
	pHead->isCrypt = 1; \
	pHead->pktType = DATA_PACKET; \
	pHead->pduLen = payloadLen; \
	pHead->hton();
static const UINT16 BaseDuLen = 16;
INT DelegetMessage::ResponseErrMsg(UINT32 token, UINT16 cmdCode, UINT16 retCode, void* theSession, const char* key)
{
	CHECK_BUFF_VALID(theSession);
	CHECK_BUFF_VALID(key);
	BYTE* resBuffer = NULL;
	resBuffer = (BYTE*)CyaTcp_Alloc(SGS_PROTO_HEAD_LEN + RES_HEAD_SIZE);
	CHECK_BUFF_VALID(resBuffer);

	SGSProtocolHead* pHead = (SGSProtocolHead*)resBuffer;
	pHead->isCrypt = true;
	pHead->pktType = DATA_PACKET;
	pHead->pduLen = BaseDuLen;
	pHead->hton();

	SGSResPayload* pPayload = (SGSResPayload*)(resBuffer + SGS_PROTO_HEAD_LEN);
	pPayload->cmdCode = cmdCode;
	pPayload->retCode = retCode;
	pPayload->token = token;
	pPayload->rawDataBytes = SGS_RES_HEAD_LEN;
	pPayload->hton();

	appEncryptDataWithKey((AES_BYTE*)(resBuffer + SGS_PROTO_HEAD_LEN), BaseDuLen, (ANSICHAR*)key);

	ExtServerMan::InstanceServerMan()->ResponseSessionMsg((void*)resBuffer, BaseDuLen + SGS_PROTO_HEAD_LEN, theSession);
	return 0;
}

INT DelegetMessage::ResponseDataMsg(void* theSession, const void*msgData, INT msgLen, UINT16 cmdCode, UINT32 token, const char* key)
{
	UINT16 originalLen = SGS_RES_HEAD_LEN + msgLen;
	UINT16 encryptLen = GetAlignedLen(true, originalLen);

	BYTE* resBuffer = NULL;
	resBuffer = (BYTE*)CyaTcp_Alloc(SGS_PROTO_HEAD_LEN + encryptLen + sizeof(char));
	CHECK_BUFF_VALID(resBuffer);

	SGSProtocolHead* pHead = (SGSProtocolHead*)resBuffer;
	pHead->isCrypt = true;
	pHead->pktType = DATA_PACKET;
	pHead->pduLen = encryptLen;
	pHead->hton();

	SGSResPayload* pPayload = (SGSResPayload*)(resBuffer + SGS_PROTO_HEAD_LEN);
	pPayload->cmdCode = cmdCode;
	pPayload->retCode = EXT_OK;
	pPayload->token = token;
	pPayload->rawDataBytes = originalLen;
	memcpy(pPayload->data, msgData, msgLen);
	pPayload->hton();

	appEncryptDataWithKey((AES_BYTE*)(resBuffer + SGS_PROTO_HEAD_LEN), originalLen, (ANSICHAR*)key);

	ExtServerMan::InstanceServerMan()->ResponseSessionMsg((void*)resBuffer, SGS_PROTO_HEAD_LEN + encryptLen, theSession);
	return 0;
}

void DelegetMessage::LoginFailedHandle(UINT32 cookie)
{
	ASSERT(INVALID_COOKIE != cookie);
	CXTAutoLock locke(m_mapLocker);
	UserCookieMap::iterator itr = m_userCookies.find(cookie);
	if (m_userCookies.end() != itr)
	{
		delete itr->second;
		m_userCookies.erase(itr);
	}
}

BOOL DelegetMessage::AddUserInfo(UINT32 token, USER_INFO* info)
{
	CXTAutoLock locke(m_mapLocker);
	return (m_userCookies.insert(std::make_pair(token, info)).second);
}
