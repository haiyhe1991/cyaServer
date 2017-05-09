#include "MLogicRoleExit.h"
#include "MsgProcess.h"
#include "MLogicProtocol.h"
#include "cyaTcp.h"
#include "cyaLog.h"

MLogicRoleExit::MLogicRoleExit()
{

}

MLogicRoleExit::~MLogicRoleExit()
{

}

int MLogicRoleExit::ProcessMsg(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID)
{
	UINT32 RoleID = MsgProcess::FetchMsgProcess()->FindRoleIDByUserID(userID);
	if (RoleID <= 0)	/// 未进入角色
	{
		LogDebug(("MLogicRoleExit::User(%d) 退出角色!", userID));
		return LINK_OK;
	}

	UINT16 addLen = sizeof(UINT32)* 2 + sizeof(UINT16);

	char* reqBuf = (char*)CyaTcp_Alloc(msgLen + addLen + sizeof(char));
	ASSERT(NULL != reqBuf);
	if (NULL == reqBuf)
	{
		return LINK_MEMERY;
	}

	UINT16* cmdReq = (UINT16*)reqBuf;
	*cmdReq = htons(cmdCode);
	UINT32* uID = (UINT32*)(reqBuf + sizeof(UINT16));
	*uID = htonl(userID);
	UINT32* rID = (UINT32*)(reqBuf + sizeof(UINT16)+sizeof(UINT32));
	*rID = htonl(RoleID);
	if (msgLen > 0)
		memcpy(reqBuf + addLen, msgData, msgLen);

	GWS_SendDataPacket(gwsHandle, reqBuf, msgLen + addLen, MAIN_LOGIC_SERVER, userID, (ESGSPacketType)pktType, true);
	CyaTcp_Free(reqBuf);

	return LINK_OK;
}