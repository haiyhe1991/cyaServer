#include "MLogicQueryFinishHelp.h"

#include "cyaLog.h"

#include "MsgProcess.h"
#include "MLogicProtocol.h"


MLogicQueryFinishHelp::MLogicQueryFinishHelp()
{

}

MLogicQueryFinishHelp::~MLogicQueryFinishHelp()
{

}

int MLogicQueryFinishHelp::ProcessMsg(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID)
{
	ASSERT(MLS_QUERY_FINISHED_HELP == cmdCode);
	MsgProcess* pMsgProc = MsgProcess::FetchMsgProcess();
	ASSERT(NULL != pMsgProc);
	if (NULL == pMsgProc)
		return LINK_OK;
	UINT32 RoleID = pMsgProc->FindRoleIDByUserID(userID);
	if (RoleID <= 0)	/// 未进入角色
	{
		LogDebug(("QueryFinishHelp::User(%d) Cmd(%d), 未进入角色!", userID, cmdCode));
		pMsgProc->InputToSessionError(cmdCode, LINK_ROLE_ID, pktType, userID);
		return LINK_OK;
	}

	char reqBuf[128] = { 0 };
	memset(reqBuf, 0, sizeof(reqBuf));
	SLinkerQueryFinishedHelpReq* pLinkReq = (SLinkerQueryFinishedHelpReq*)(reqBuf + sizeof(UINT16));
	pLinkReq->roleId = RoleID;
	pLinkReq->hton();

	UINT16* pCmd = (UINT16*)reqBuf;
	*pCmd = htons(cmdCode);
	GWS_SendDataPacket(gwsHandle, reqBuf, sizeof(SLinkerQueryFinishedHelpReq)+sizeof(UINT16),
		MAIN_LOGIC_SERVER, userID, (ESGSPacketType)pktType, true);

	return LINK_OK;
}
