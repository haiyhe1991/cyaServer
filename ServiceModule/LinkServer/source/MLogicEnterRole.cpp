#include "MLogicEnterRole.h"

#include "MLogicProtocol.h"

MLogicEnterRole::MLogicEnterRole()
{

}

MLogicEnterRole::~MLogicEnterRole()
{

}

int MLogicEnterRole::ProcessMsg(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID)
{
	ASSERT(MLS_ENTER_ROLE == cmdCode);
	ASSERT(NULL != msgData && sizeof(SEnterRoleReq) <= msgLen);
	char reqBuf[256] = { 0 };
	memset(reqBuf, 0, sizeof(reqBuf));

	SEnterRoleReq* pEnterReq = (SEnterRoleReq*)msgData;
	pEnterReq->ntoh();

	SLinkerEnterRoleReq* pLinkReq = (SLinkerEnterRoleReq*)(reqBuf + sizeof(UINT16));
	pLinkReq->roleId = pEnterReq->roleId;
	pLinkReq->userId = userID;
	pLinkReq->hton();

	UINT16* reqCmd = (UINT16*)reqBuf;
	*reqCmd = htons(cmdCode);
	GWS_SendDataPacket(gwsHandle, reqBuf, sizeof(SLinkerEnterRoleReq)+sizeof(UINT16), MAIN_LOGIC_SERVER, userID, (ESGSPacketType)pktType, true);

	return LINK_OK;
}

