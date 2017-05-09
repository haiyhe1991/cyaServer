#include "MLogicQueryRole.h"

MLogicQueryRole::MLogicQueryRole()
{

}

MLogicQueryRole::~MLogicQueryRole()
{

}

int MLogicQueryRole::ProcessMsg(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID)
{
	ASSERT(MLS_QUERY_ROLES == cmdCode && NULL != gwsHandle);
	ASSERT(NULL != msgData && sizeof(SQueryRolesReq) <= msgLen);

	SQueryRolesReq* pClientReq = (SQueryRolesReq*)msgData;
	pClientReq->ntoh();
	if (pClientReq->userId != userID)
		return LINK_ACCOUNT_TOKEN;
	pClientReq->hton();

	char reqBuf[256] = { 0 };
	memset(reqBuf, 0, sizeof(reqBuf));

	UINT16* reqCmd = (UINT16*)reqBuf;
	*reqCmd = htons(cmdCode);
	memcpy(reqBuf + sizeof(UINT16), msgData, msgLen);
	GWS_SendDataPacket(gwsHandle, reqBuf, msgLen + sizeof(UINT16), MAIN_LOGIC_SERVER, userID, (ESGSPacketType)pktType, true);

	return LINK_OK;
}

