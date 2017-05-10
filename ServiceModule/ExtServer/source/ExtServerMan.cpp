#include "ExtServerMan.h"
#include "ConfigReader.h"
#include "cyaIpCvt.h"
#include "DelegetMsg.h"
#include "PubFunction.h"
#include "AES.h"

ExtServerMan* ExtServerMan::m_instance = NULL;

ExtServerMan::ExtServerMan()
: m_tcpMan(NULL)
, m_maker(NULL)
, m_gwsHandle(NULL)
, m_gwsConnected(FALSE)
{
	PreInitialize();
}

ExtServerMan::~ExtServerMan()
{
	if (m_tcpMan)
	{
		m_tcpMan->StopServe();
		m_tcpMan->DeleteThis();
		m_tcpMan = NULL;
	}
	if (m_maker)
	{
		delete m_maker;
		m_maker = NULL;
	}
	if (m_gwsHandle)
	{
		GWS_Logout(m_gwsHandle);
		GWS_ReleaseHandle(m_gwsHandle);
		m_gwsHandle = NULL;
		GWS_Cleanup();
	}
	DelegetMessage* pDeleget = NULL;
	pDeleget = DelegetMessage::InstanceDeleteget();
	if (pDeleget)
	{
		pDeleget->DeleteThis();
	}
}

INT ExtServerMan::PreInitialize()
{
	GWS_Init();
	if (NULL == m_maker)
	{
		m_maker = new ESTcpSessionMaker();
		ASSERT(m_maker);
	}
	if (NULL == m_tcpMan && m_maker)
	{
		CyaTcp_MakeServeMan(m_tcpMan, m_maker);
		ASSERT(m_tcpMan);
		if (m_tcpMan)
		{
			ConfigReader* pReader = new ConfigReader();
			ASSERT(pReader);
			if (pReader)
			{
				pReader->Read();
				IPPADDR extaddr(GetDWordIP(pReader->SelfAddress()), pReader->SelfPort());
				m_tcpMan->StartServe(extaddr);
				delete pReader;
			}
		}
	}
	DelegetMessage* pDelegetMsg = DelegetMessage::InstanceDeleteget();
	ASSERT(pDelegetMsg);

	return NULL == m_tcpMan;
}

INT ExtServerMan::Initialize()
{
	if (NULL == m_tcpMan)
	{
		return EXT_ERROR;
	}
	if (NULL == m_gwsHandle)
	{
		ConfigReader* pReader = new ConfigReader();
		ASSERT(pReader);
		if (NULL == pReader)
			return EXT_ERROR;

		pReader->Read();
		m_gwsHandle = GWS_FetchHandle(pReader->GWSAddress(),
			pReader->GWSPort(),
			MAN_INRERFACE_SERVER,
			GWSDataCallback,
			this,
			pReader->GWSConnectTimeout(),
			pReader->GWSWriteTimeOut(),
			pReader->GWSReadTimeOut());
		if (m_gwsHandle)
		{
			GWS_SetEventCallback(m_gwsHandle, GWSEventCallback, this);
		}
		if (m_gwsHandle && 0 != GWS_Login(m_gwsHandle))
		{
			GWS_ReleaseHandle(m_gwsHandle);
			m_gwsHandle = NULL;
		}
		delete pReader;
		m_gwsConnected = (m_gwsHandle != NULL);
	}
	return NULL == m_gwsHandle;
}

ExtServerMan* ExtServerMan::InstanceServerMan()
{
	if (NULL == m_instance)
	{
		m_instance = new ExtServerMan();
	}
	ASSERT(m_instance);
	return m_instance;
}

void ExtServerMan::DeleteThis()
{
	if (m_instance)
	{
		delete m_instance;
		m_instance = NULL;
	}
}

void ExtServerMan::GWSDataCallback(SGWSProtocolHead* pHead, const void* payload, int payloadLen, void* context)
{
	ExtServerMan* pThis = (ExtServerMan*)context;
	if (pThis)
	{
		pThis->OnGWSData(pHead, payload, payloadLen);
	}
}

void ExtServerMan::GWSEventCallback(EServerEvent evt, const void* data, int nDataLen, void* context)
{
	ExtServerMan* pThis = (ExtServerMan*)context;
	if (pThis)
	{
		pThis->OnGWSEvent(evt, data, nDataLen);
	}
}

#define INVALID_PTR(buffName) \
	ASSERT(buffName); \
if (NULL == buffName){ return; }

#define INVALID_PTR_FREE(buffName, freeBuff) \
	ASSERT(freeBuff);			\
	ASSERT(buffName);			\
if (NULL == buffName)		\
{							\
if (freeBuff) CyaTcp_Free(freeBuff);	\
	return;				\
}

void ExtServerMan::OnGWSData(SGWSProtocolHead* pHead, const void* payload, int payloadLen)
{
	ASSERT(pHead && payload && 0<payloadLen);
	ASSERT(payloadLen >= sizeof(SServerCommonReply)-sizeof(char));
	if (payloadLen < sizeof(SServerCommonReply)-sizeof(char))
	{
		return;	///		¸ñÊ½´íÎó
	}

	UINT16 originalLen = SGS_RES_HEAD_LEN + payloadLen - sizeof(SServerCommonReply)+sizeof(char);
	UINT16 encryptLen = GetAlignedLen(true, originalLen, 16);
	BYTE* resBuf = (BYTE*)CyaTcp_Alloc(encryptLen + SGS_PROTO_HEAD_LEN + 1);
	INVALID_PTR(resBuf);
	DelegetMessage* pDelete = DelegetMessage::InstanceDeleteget();
	INVALID_PTR_FREE(pDelete, resBuf);
	void* session = pDelete->FindSession(pHead->userId);
	INVALID_PTR_FREE(session, resBuf);

	SServerCommonReply* pComHead = (SServerCommonReply*)payload;
	pComHead->ntoh();
	if (MANS_USER_LOGIN == pComHead->cmdCode && EXT_OK != pComHead->retCode)
	{
		pDelete->LoginFailedHandle(pHead->userId);
	}

	const char* key = DelegetMessage::InstanceDeleteget()->FindEncryptKey(pHead->userId);
	ASSERT(key);

	SGSProtocolHead* pProHead = (SGSProtocolHead*)resBuf;
	pProHead->isCrypt = key ? 1 : 0;
	pProHead->pktType = DATA_PACKET;
	pProHead->pduLen = encryptLen;
	pProHead->hton();

	SGSResPayload* pLoad = (SGSResPayload*)(resBuf + SGS_PROTO_HEAD_LEN);
	pLoad->cmdCode = pComHead->cmdCode;
	pLoad->retCode = pComHead->retCode;
	pLoad->rawDataBytes = originalLen;
	pLoad->token = pHead->userId;
	if (payloadLen - sizeof(SServerCommonReply)+sizeof(char) > 0)
		memcpy(pLoad->data, pComHead->data, payloadLen - sizeof(SServerCommonReply)+sizeof(char));
	pLoad->hton();

	if (key)
	{
		appEncryptDataWithKey((AES_BYTE*)(resBuf + SGS_PROTO_HEAD_LEN), encryptLen, (ANSICHAR*)key);
	}

	ResponseSessionMsg(resBuf, encryptLen + SGS_PROTO_HEAD_LEN, session);
	CyaTcp_Free(resBuf);
}

void ExtServerMan::OnGWSEvent(EServerEvent evt, const void* data, int nDataLen)
{
	if (GWS_CONNECTED == evt)
	{
		m_gwsConnected = TRUE;
	}
	else if (GWS_BROKEN == evt)
	{
		m_gwsConnected = FALSE;
	}
}

void ExtServerMan::PushSessionMsg(const void* msgData, INT msgLen, UINT16 cmdCode, UINT32 userId)
{
	BYTE* reqBuf = (BYTE*)CyaTcp_Alloc(sizeof(UINT16)+msgLen + 1);
	INVALID_PTR(reqBuf);
	UINT16* gwsCmdCode = (UINT16*)reqBuf;
	*gwsCmdCode = htons(cmdCode);
	memcpy(reqBuf + sizeof(UINT16), msgData, msgLen);
	GWS_SendDataPacket(m_gwsHandle, reqBuf, msgLen + sizeof(UINT16), MAIN_LOGIC_SERVER, userId, DATA_PACKET, false);
	CyaTcp_Free(reqBuf);
}

void ExtServerMan::ResponseSessionMsg(const void* msgData, INT msgLen, void* theSession)
{
	if (m_tcpMan)
	{
		m_tcpMan->SendSessionData((ESTcpSession*)theSession, msgData, msgLen);
	}
}
