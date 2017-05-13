#include "GWSClient.h"
#include "RoomConfig.h"
#include "ServiceErrorCode.h"
#include "ServiceRoom.h"
#include "ProcessManager.h"

CGWSClient* CGWSClient::m_this = NULL;

CGWSClient* CGWSClient::FetchGateWayClientInstance()
{
	if (NULL == m_this)
	{
		m_this = new CGWSClient();
	}
	return m_this;
}

void CGWSClient::ReleaseGateWayClientInstance()
{

}

CGWSClient::CGWSClient()
: m_gwHandle(NULL)
{
	GWS_Init();
}

CGWSClient::~CGWSClient()
{
	GWS_Cleanup();
	m_gwHandle = NULL;
}

INT CGWSClient::GateWayClientStart()
{
	CRoomConfig* pConfig = CRoomConfig::FetchConfigInstance();
	ASSERT(NULL != pConfig);
	m_gwHandle = GWS_FetchHandle(pConfig->GetGWServerAddr(), pConfig->GetGWServerPort(), ROOM_SERVER_1, CGWSClient::GWDataCallback, this);
	ASSERT(NULL != m_gwHandle);
	if (NULL != m_gwHandle && GWS_OK == GWS_Login(m_gwHandle))
	{
		GWS_SetEventCallback(m_gwHandle, GWEventCallback, this);
		return DAEMON_OK;
	}
	return DAEMON_ERROR;
}

void CGWSClient::GateWayClientStop()
{
	if (NULL != m_gwHandle)
	{
		GWS_Logout(m_gwHandle);
		GWS_ReleaseHandle(m_gwHandle);
		m_gwHandle = NULL;
	}
}

INT CGWSClient::InputSessionMsg(const void* data, const INT& dataLen)
{
	return GWS_SendDataPacket(m_gwHandle, data, dataLen, 0, 0, DATA_PACKET, 0);
}

void CGWSClient::GWDataCallback(SGWSProtocolHead* pHead, const void* payload, int payloadLen, void* context)
{
	CGWSClient* pThis = (CGWSClient*)context;
	if (pThis)
	{
		pThis->OnGWDataCallback(pHead, payload, payloadLen);
	}
}

void CGWSClient::GWEventCallback(EServerEvent evt, const void* data, int nDataLen, void* context)
{
	CGWSClient* pThis = (CGWSClient*)context;
	if (pThis)
	{
		pThis->OnGWEventCallback(evt, data, nDataLen);
	}
}

void CGWSClient::OnGWDataCallback(SGWSProtocolHead* pHead, const void* payload, int payloadLen)
{
	ASSERT(NULL != pHead && NULL != payload && 0 < payloadLen);
	SServerCommonReply* pComHead = (SServerCommonReply*)payload;
	pComHead->hton();

	CProcessManager* pProcMgr = CProcessManager::FetchProcessManagerInstance();
	if (NULL != pProcMgr)
	{
		pProcMgr->SendGWData(pComHead->data, payloadLen - sizeof(SServerCommonReply)+sizeof(char), pComHead->cmdCode);
	}
}

void CGWSClient::OnGWEventCallback(EServerEvent evt, const void* data, int nDataLen)
{

}