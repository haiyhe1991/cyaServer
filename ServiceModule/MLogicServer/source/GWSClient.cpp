#include "cyaTypes.h"
#include "GWSClient.h"
#include "ConfigFileMan.h"
#include "GMLogicProcMan.h"
#include "MLogicProcessMan.h"

GWSClient::GWSClient()
: m_gwsHandle(NULL)
{

}

GWSClient::~GWSClient()
{

}

int  GWSClient::Login()
{
	m_gwsHandle = GWS_FetchHandle(GetConfigFileMan()->GetGwsIp(), GetConfigFileMan()->GetGwsPort(), MAIN_LOGIC_SERVER, GWSDataCallback, this);
	if (NULL == m_gwsHandle)
		return MLS_RESOURCE_NOT_ENOUGH;

	int ret = GWS_Login(m_gwsHandle);
	if (ret != GWS_OK)
	{
		GWS_ReleaseHandle(m_gwsHandle);
		m_gwsHandle = NULL;
	}
	return ret;
}

void GWSClient::Logout()
{
	if (m_gwsHandle != NULL)
	{
		GWS_Logout(m_gwsHandle);
		GWS_ReleaseHandle(m_gwsHandle);
		m_gwsHandle = NULL;
	}
}

void GWSClient::GWSDataCallback(SGWSProtocolHead* pHead, const void* payload, int payloadLen, void* context)
{
	GWSClient* pThis = (GWSClient*)context;
	ASSERT(pThis != NULL);
	pThis->OnGWSDataCallback(pHead, payload, payloadLen);
}

void GWSClient::OnGWSDataCallback(SGWSProtocolHead* pHead, const void* payload, int payloadLen)
{
	if (pHead->pktType != DATA_PACKET)
		return;

	const BYTE* p = (const BYTE*)payload;
	UINT16 cmdCode = ntohs(*(UINT16*)p);
	p += sizeof(UINT16);
	int nLen = payloadLen - sizeof(UINT16);

	//´¦ÀíÂß¼­
	// 	if(cmdCode >= GMS_LOGIN && cmdCode <= GMS_LOGIN + 200)
	// 		GetGMLogicProcMan()->ProcessLogic(this, cmdCode, pHead, p, nLen);
	// 	else
	// 		GetMLogicProcessMan()->ProcessLogic(this, cmdCode, pHead, p, nLen);
	GetMLogicProcessMan()->ProcessLogic(this, cmdCode, pHead, p, nLen);
}

int GWSClient::SendBasicResponse(SGWSProtocolHead* pHead, UINT16 retCode, UINT16 cmdCode)
{
	char buf[GWS_COMMON_REPLY_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SServerCommonReply* reply = (SServerCommonReply*)buf;
	reply->cmdCode = cmdCode;
	reply->retCode = retCode;
	reply->hton();

	pHead->encrypt = 0;
	pHead->pduLen = nLen;
	pHead->targetId = pHead->sourceId;
	pHead->sourceId = MAIN_LOGIC_SERVER;
	pHead->hton();

	GWS_SendDataPacketEx(m_gwsHandle, pHead, buf, nLen);
	return MLS_OK;
}

int GWSClient::SendBasicResponse(BYTE targetId, UINT32 userId, UINT16 retCode, UINT16 cmdCode)
{
	char buf[GWS_COMMON_REPLY_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SServerCommonReply* reply = (SServerCommonReply*)buf;
	reply->cmdCode = cmdCode;
	reply->retCode = retCode;
	reply->hton();

	SGWSProtocolHead header;
	memset(&header, 0, sizeof(SGWSProtocolHead));
	header.pduLen = nLen;
	header.pktType = DATA_PACKET;
	header.encrypt = 0;
	header.targetId = targetId;
	header.sourceId = MAIN_LOGIC_SERVER;
	header.userId = userId;
	header.hton();

	GWS_SendDataPacketEx(m_gwsHandle, &header, buf, nLen);
	return MLS_OK;
}

int GWSClient::SendResponse(SGWSProtocolHead* pHead, UINT16 retCode, UINT16 cmdCode, void* buf, int nLen)
{
	SServerCommonReply* reply = (SServerCommonReply*)buf;
	reply->cmdCode = cmdCode;
	reply->retCode = retCode;
	reply->hton();

	pHead->encrypt = 0;
	pHead->pduLen = nLen;
	pHead->targetId = pHead->sourceId;
	pHead->sourceId = MAIN_LOGIC_SERVER;
	pHead->hton();

	GWS_SendDataPacketEx(m_gwsHandle, pHead, buf, nLen);
	return MLS_OK;
}

int GWSClient::SendResponse(BYTE targetId, UINT32 userId, UINT16 retCode, UINT16 cmdCode, void* buf, int nLen, BYTE pktType/* = DATA_PACKET*/, BYTE sourceId /*= MAIN_LOGIC_SERVER*/)
{
	SServerCommonReply* reply = (SServerCommonReply*)buf;
	reply->cmdCode = cmdCode;
	reply->retCode = retCode;
	reply->hton();

	SGWSProtocolHead header;
	memset(&header, 0, sizeof(SGWSProtocolHead));
	header.pduLen = nLen;
	header.pktType = pktType;
	header.encrypt = 0;
	header.targetId = targetId;
	header.sourceId = sourceId;
	header.userId = userId;
	header.hton();

	GWS_SendDataPacketEx(m_gwsHandle, &header, buf, nLen);
	return MLS_OK;
}
