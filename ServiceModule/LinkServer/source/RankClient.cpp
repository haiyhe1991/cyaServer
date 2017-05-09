#include "LinkGlobalObj.h"
#include "LinkConfig.h"
#include "cyaLog.h"
#include "PubFunction.h"

#include "ServiceErrorCode.h"
#include "RankProtocol.h"
#include "RankClient.h"
#include "MsgProcess.h"

RankClient::RankClient()
{

}

RankClient::~RankClient()
{

}

int RankClient::OnQueryRoleInfo(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt)
{
	UINT32 roleID = MsgProcess::FetchMsgProcess()->FindRoleIDByUserID(userID);
	if (roleID == INVALID_ROLE_ID)
		return LINK_OK;

	if (msgLen < sizeof(SGetRankInfoReq))
	{
		LogDebug(("加入频道数据包异常"));
		return LINK_ERROR;
	}

	SGetRankInfoReq* pClientReq = (SGetRankInfoReq*)msgData;
	pClientReq->ntoh();

	char msgBuff[256] = { 0 };
	UINT16* gwsCmdCode = (UINT16*)msgBuff;
	*gwsCmdCode = htons(cmdCode);
	SLinkerGetRankInfoReq* req = (SLinkerGetRankInfoReq*)(msgBuff + sizeof(UINT16));
	{
		//      req->linkerId = GlobalGetLinkConfig()->GetLinkNumber();
		req->userId = userID;
		req->roleId = roleID;
		req->info.type = pClientReq->type;
		req->info.num = pClientReq->num;
		req->info.start = pClientReq->start;

		req->hton();
	}

	GWS_SendDataPacket(gwsHandle, msgBuff, sizeof(SLinkerGetRankInfoReq)+sizeof(UINT16), OTHER_LOGIC_SERVER, userID, (ESGSPacketType)pktType, encrypt);

	return LINK_OK;
}

int RankClient::OnQueryRoleRank(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt)
{
	UINT32 roleID = MsgProcess::FetchMsgProcess()->FindRoleIDByUserID(userID);
	if (roleID == INVALID_ROLE_ID)
		return LINK_OK;

	if (msgLen < sizeof(SGetRoleRankInfoReq))
	{
		LogDebug(("加入频道数据包异常"));
		return LINK_ERROR;
	}

	SGetRoleRankInfoReq* pClientReq = (SGetRoleRankInfoReq*)msgData;
	pClientReq->ntoh();

	char msgBuff[256] = { 0 };
	UINT16* gwsCmdCode = (UINT16*)msgBuff;
	*gwsCmdCode = htons(cmdCode);
	SLinkerGetRoleRankInfoReq* req = (SLinkerGetRoleRankInfoReq*)(msgBuff + sizeof(UINT16));
	{
		//      req->linkerId = GlobalGetLinkConfig()->GetLinkNumber();
		req->userId = userID;
		req->roleId = roleID;
		req->info.querytype = pClientReq->querytype;
		req->info.roleId = pClientReq->roleId;

		req->hton();
	}

	GWS_SendDataPacket(gwsHandle, msgBuff, sizeof(SLinkerGetRoleRankInfoReq)+sizeof(UINT16), OTHER_LOGIC_SERVER, userID, (ESGSPacketType)pktType, encrypt);

	return LINK_OK;
}

int RankClient::ClientMsg(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt)
{
	if (NULL == gwsHandle)
		return LINK_LOGIN_GWS;

	if (sizeof(UINT16)+msgLen > 256)
	{
		LogDebug(("客户端请求聊天数据包过大"));
		return LINK_PAKCET_SIZE;
	}

	ASSERT(msgLen >= 0);
	int ReturnCode = LINK_CMD_CODE;
	switch (cmdCode)
	{
	case RANK_QUERY_ROLE_INFO:
		ReturnCode = OnQueryRoleInfo(gwsHandle, msgData, msgLen, cmdCode, pktType, userID, encrypt);
		break;

	case RANK_QUERY_ROLE_RANK:
		ReturnCode = OnQueryRoleRank(gwsHandle, msgData, msgLen, cmdCode, pktType, userID, encrypt);
		break;

	default:
		ASSERT(FALSE);
		LogDebug(("收到错误的命令码请求"));
	}

	return ReturnCode;
}


void RankClient::WGSMsg(SGWSProtocolHead* pHead, UINT16 cmdCode, UINT16 retCode, const void* payload, int payloadLen)
{
	ASSERT(NULL != pHead && NULL != payload && 0 <= payloadLen);
	if (!LINK_OTHER_LOGIC_SERVER(cmdCode))
	{
		LogDebug(("收到错误的命令码回复 %d", cmdCode));
		return; ///非法数据
	}

	MsgProcess* msgProc = MsgProcess::FetchMsgProcess();
	ASSERT(NULL != msgProc);
	if (NULL == msgProc)
		return;

	if (retCode != RANK_OK)
	{
		msgProc->InputToSessionError(cmdCode, retCode, pHead->pktType, pHead->userId);
	}
	else
	{
		msgProc->InputToSessionMsg(pHead->userId, pHead->pktType, cmdCode, retCode, payload, payloadLen);
	}
}