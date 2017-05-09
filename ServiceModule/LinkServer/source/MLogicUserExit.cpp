#include "MLogicUserExit.h"
#include "MsgProcess.h"
#include "MLogicProtocol.h"

MLogicUserExit::MLogicUserExit()
{

}

MLogicUserExit::~MLogicUserExit()
{

}

int MLogicUserExit::ProcessMsg(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID)
{
	ASSERT(MLS_USER_EXIT == cmdCode);

	BYTE reqBuf[256] = { 0 };
	SLinkerUserExitReq* pLinkReq = (SLinkerUserExitReq*)(reqBuf + sizeof(UINT16));
	pLinkReq->userId = userID;
	pLinkReq->hton();
	UINT16* pCmd = (UINT16*)reqBuf;
	*pCmd = htons(cmdCode);

	GWS_SendDataPacket(gwsHandle, reqBuf, sizeof(SLinkerUserExitReq)+sizeof(UINT16), MAIN_LOGIC_SERVER, userID, (ESGSPacketType)pktType, true);

	return LINK_OK;
}

