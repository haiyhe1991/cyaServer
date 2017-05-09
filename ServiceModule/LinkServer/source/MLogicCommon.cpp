#include "MLogicCommon.h"
#include "MLogicProtocol.h"
#include "MsgProcess.h"
#include "cyaLog.h"

MLogicCommon::MLogicCommon()
{

}

MLogicCommon::~MLogicCommon()
{

}

int MLogicCommon::ProcessMsg(GWSHandle gwsHandle, const void* pMsgData, int nMsgLen, UINT16 nCmdCode, UINT16 nPktType, UINT32 nUserID)
{
	MsgProcess* pMsgProc = MsgProcess::FetchMsgProcess();
	ASSERT(NULL != pMsgProc);
	if (NULL == pMsgProc)
		return LINK_OK;
	UINT32 RoleID = pMsgProc->FindRoleIDByUserID(nUserID);
	if (RoleID <= 0)	/// 未进入角色
	{
		LogDebug(("Common::User(%d) Cmd(%d), 未进入角色!", nUserID, nCmdCode));
		pMsgProc->InputToSessionError(nCmdCode, LINK_ROLE_ID, nPktType, nUserID);
		return LINK_OK;
	}

	char reqBuf[256] = { 0 };
	memset(reqBuf, 0, sizeof(reqBuf));

	UINT16* pCmd = (UINT16*)reqBuf;
	*pCmd = htons(nCmdCode);
	UINT32*pUser = (UINT32*)(reqBuf + sizeof(INT16));
	*pUser = htonl(nUserID);
	*(pUser + 1) = htonl(RoleID);
	GWS_SendDataPacket(gwsHandle, reqBuf, sizeof(UINT16)+2 * sizeof(UINT32), MAIN_LOGIC_SERVER, nUserID, (ESGSPacketType)nPktType, true);

	return LINK_OK;
}
