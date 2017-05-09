#include "MLogicCommonRole.h"
#include "cyaTcp.h"
#include "cyaLog.h"
#include "MsgProcess.h"
#include "MLogicProtocol.h"

MLogicCommonRole::MLogicCommonRole()
{

}

MLogicCommonRole::~MLogicCommonRole()
{

}

int MLogicCommonRole::ProcessMsg(GWSHandle gwsHandle, const void* pMsgData, int nMsgLen, UINT16 nCmdCode, UINT16 nPktType, UINT32 nUserID)
{
	ASSERT(NULL != pMsgData && 0 < nMsgLen);

	MsgProcess* pMsgProc = MsgProcess::FetchMsgProcess();
	ASSERT(NULL != pMsgProc);
	if (NULL == pMsgProc)
		return LINK_OK;
	UINT32 RoleID = pMsgProc->FindRoleIDByUserID(nUserID);
	if (RoleID <= 0)	/// 未进入角色
	{
		LogDebug(("CommonRole::User(%d) Cmd(%d), 未进入角色!", nUserID, nCmdCode));
		pMsgProc->InputToSessionError(nCmdCode, LINK_ROLE_ID, nPktType, nUserID);
		return LINK_OK;
	}

	UINT16 addLen = sizeof(UINT16)+sizeof(UINT32);

	char* reqBuf = (char*)CyaTcp_Alloc(nMsgLen + addLen + sizeof(char));
	ASSERT(NULL != reqBuf);
	if (NULL == reqBuf)
	{
		return LINK_MEMERY;
	}

	UINT16* cmdReq = (UINT16*)reqBuf;
	*cmdReq = htons(nCmdCode);
	UINT32* rID = (UINT32*)(reqBuf + sizeof(UINT16));
	*rID = htonl(RoleID);
	if (nMsgLen > 0)
		memcpy(reqBuf + addLen, pMsgData, nMsgLen);

	GWS_SendDataPacket(gwsHandle, reqBuf, nMsgLen + addLen, MAIN_LOGIC_SERVER, nUserID, (ESGSPacketType)nPktType, true);
	CyaTcp_Free(reqBuf);

	return LINK_OK;
}