#include "MauGWSClient.h"
#include "ConfigFileMan.h"
#include "MauMsgMan.h"
#include "cyaSock.h"
#include "cyaLog.h"

#define MAX_MEMBERS_UNION_CHANNEL 500
#define MAX_CHAT_MSG_LEN 1024

MauGWSClient::MauGWSClient()
{

}

MauGWSClient::~MauGWSClient()
{

}

int  MauGWSClient::Login()
{
	m_gwsHandle = GWS_FetchHandle(GetConfigFileMan()->GetGwsIp(), GetConfigFileMan()->GetGwsPort(), MAU_SERVER, GWSDataCallback, this);
	if (m_gwsHandle == NULL)
		return CHAT_ERROR;

	int ret = GWS_Login(m_gwsHandle);
	if (ret != GWS_OK)
	{
		GWS_ReleaseHandle(m_gwsHandle);
		m_gwsHandle = NULL;
	}
	return ret;
}

void MauGWSClient::Logout()
{
	if (m_gwsHandle != NULL)
	{
		GWS_Logout(m_gwsHandle);
		GWS_ReleaseHandle(m_gwsHandle);
		m_gwsHandle = NULL;
	}
}

void MauGWSClient::GWSDataCallback(SGWSProtocolHead* pHead, const void* payload, int payloadLen, void* context)
{
	MauGWSClient* pThis = (MauGWSClient*)context;
	ASSERT(pThis != NULL);
	pThis->OnGWSDataCallback(pHead, payload, payloadLen);
}

void MauGWSClient::OnGWSDataCallback(SGWSProtocolHead* pHead, const void* payload, int payloadLen)
{
	if (pHead == NULL || payload == NULL || payloadLen <= 0)
		return;

	if (pHead->pktType != DATA_PACKET)
		return;

	const BYTE* p = (const BYTE*)payload;
	UINT16 cmdCode = ntohs(*(UINT16*)p);
	p += sizeof(UINT16);
	int nLen = payloadLen - sizeof(UINT16);

	GetMauMsgMan()->ProcessLogic(this, cmdCode, pHead, p, nLen);
}

int MauGWSClient::SendBasicResponse(SGWSProtocolHead* pHead, UINT16 retCode, UINT16 cmdCode)
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
	pHead->sourceId = MAU_SERVER;
	pHead->hton();

	return GWS_SendDataPacketEx(m_gwsHandle, pHead, buf, nLen);
}

int MauGWSClient::SendResponse(SGWSProtocolHead* pHead, UINT16 retCode, UINT16 cmdCode, void* buf, int nLen, BYTE targetId)
{
	SServerCommonReply* reply = (SServerCommonReply*)buf;
	reply->cmdCode = cmdCode;
	reply->retCode = retCode;
	reply->hton();

	pHead->encrypt = 0;
	pHead->pduLen = nLen;
	pHead->targetId = targetId;	//pHead->sourceId;
	pHead->sourceId = MAU_SERVER;
	pHead->hton();

	return GWS_SendDataPacketEx(m_gwsHandle, pHead, buf, nLen);
}

int MauGWSClient::SendResponse(SGWSProtocolHead* pHead, UINT16 retCode, UINT16 cmdCode, void* buf, int nLen)
{
	SServerCommonReply* reply = (SServerCommonReply*)buf;
	reply->cmdCode = cmdCode;
	reply->retCode = retCode;
	reply->hton();

	pHead->encrypt = 0;
	pHead->pduLen = nLen;
	pHead->targetId = pHead->sourceId;
	pHead->sourceId = MAU_SERVER;
	pHead->hton();

	return GWS_SendDataPacketEx(m_gwsHandle, pHead, buf, nLen);
}

int MauGWSClient::SendResponse(BYTE targetId, UINT32 userId, UINT16 retCode, UINT16 cmdCode, void* buf, int nLen, BYTE pktType/* = DATA_PACKET*/, BYTE sourceId /*= MAU_SERVER*/)
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