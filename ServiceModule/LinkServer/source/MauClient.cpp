#include "MauClient.h"
#include "MauProtocol.h"
#include "ServiceErrorCode.h"
#include "MsgProcess.h"
#include "LinkGlobalObj.h"
#include "LinkConfig.h"
#include "cyaLog.h"
#include "ServiceMau.h"
#include "PubFunction.h"

#define MAX_MAU_MSG_LEN 256

MauClient::MauClient()
{

}

MauClient::~MauClient()
{

}

int MauClient::OnMauCreateTable(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt)
{
	UINT32 roleID = MsgProcess::FetchMsgProcess()->FindRoleIDByUserID(userID);
	if (roleID == INVALID_ROLE_ID)
		return LINK_OK;

	if (msgLen < sizeof(SCreateTabelReq))
	{
		LogDebug(("创建牌桌数据包异常"));
		return LINK_ERROR;
	}

	SCreateTabelReq* pClientReq = (SCreateTabelReq*)msgData;
	char msgBuff[MAX_MAU_MSG_LEN] = { 0 };
	UINT16* gwsCmdCode = (UINT16*)msgBuff;
	*gwsCmdCode = htons(cmdCode);

	SLinkerCreateMauTabelReq* req = (SLinkerCreateMauTabelReq*)(msgBuff + sizeof(UINT16));
	{
		req->linkerId = GlobalGetLinkConfig()->GetLinkNumber();
		req->userId = userID;
		req->roleId = roleID;
		memcpy(req->nick, pClientReq->nick, 32);
		req->nick[32] = '\0';
		req->hton();
	}

	GWS_SendDataPacket(gwsHandle, msgBuff, sizeof(SLinkerCreateMauTabelReq) + sizeof(UINT16), MAU_SERVER, userID, (ESGSPacketType)pktType, encrypt);

	return LINK_OK;
}

int MauClient::OnMauJoinTable(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt)
{
	UINT32 roleID = MsgProcess::FetchMsgProcess()->FindRoleIDByUserID(userID);
	if (roleID == INVALID_ROLE_ID)
		return LINK_OK;

	if (msgLen < sizeof(SJoinTableReq))
	{
		LogDebug(("加入牌桌数据包异常"));
		return LINK_ERROR;
	}

	SJoinTableReq* pClientReq = (SJoinTableReq*)msgData;
	pClientReq->ntoh();

	char msgBuff[MAX_MAU_MSG_LEN] = { 0 };
	UINT16* gwsCmdCode = (UINT16*)msgBuff;
	*gwsCmdCode = htons(cmdCode);
	SLinkerJoinMauTabelReq* req = (SLinkerJoinMauTabelReq*)(msgBuff + sizeof(UINT16));
	{
		req->linkerId = GlobalGetLinkConfig()->GetLinkNumber();
		req->userId = userID;
		req->roleId = roleID;
		req->joinId = pClientReq->joinId;
		memcpy(req->nick, pClientReq->nick, 32);
		req->nick[32] = '\0';
		req->hton();
	}

	GWS_SendDataPacket(gwsHandle, msgBuff, sizeof(SLinkerJoinMauTabelReq) + sizeof(UINT16), MAU_SERVER, userID, (ESGSPacketType)pktType, encrypt);

	return LINK_OK;
}

int MauClient::OnMauReadyTable(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt)
{
	UINT32 roleID = MsgProcess::FetchMsgProcess()->FindRoleIDByUserID(userID);
	if (roleID == INVALID_ROLE_ID)
		return LINK_OK;

	if (sizeof(SReadyTableReq) > msgLen)
	{
		LogDebug(("准备牌桌数据异常"));
		return LINK_ERROR;
	}
	SReadyTableReq* pClientReq = (SReadyTableReq*)msgData;
	pClientReq->ntoh();

	char msgBuff[MAX_MAU_MSG_LEN] = { 0 };
	UINT16* gwsCmdCode = (UINT16*)msgBuff;
	*gwsCmdCode = htons(cmdCode);
	SLinkerReadyTabelReq* pLinkReq = (SLinkerReadyTabelReq*)(msgBuff + sizeof(UINT16));
	pLinkReq->userId = userID;
	pLinkReq->roleId = roleID;
	pLinkReq->joinId = pClientReq->joinId;
	pLinkReq->ready = pClientReq->ready;
	pLinkReq->hton();

	GWS_SendDataPacket(gwsHandle, msgBuff, sizeof(SLinkerReadyTabelReq) + sizeof(UINT16), MAU_SERVER, userID, (ESGSPacketType)pktType, encrypt);

	return LINK_OK;
}

int MauClient::OnMauLeaveTable(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt)
{
	UINT32 roleID = MsgProcess::FetchMsgProcess()->FindRoleIDByUserID(userID);
	if (roleID == INVALID_ROLE_ID)
		return LINK_OK;

	if (sizeof(SLeaveTableReq) > msgLen)
	{
		LogDebug(("离开牌桌数据异常"));
		return LINK_ERROR;
	}
	SLeaveTableReq* pClientReq = (SLeaveTableReq*)msgData;
	pClientReq->ntoh();

	char msgBuff[MAX_MAU_MSG_LEN] = { 0 };
	UINT16* gwsCmdCode = (UINT16*)msgBuff;
	*gwsCmdCode = htons(cmdCode);
	SLinkerLeaveMauTabelReq* pLinkReq = (SLinkerLeaveMauTabelReq*)(msgBuff + sizeof(UINT16));
	pLinkReq->userId = userID;
	pLinkReq->roleId = roleID;
	pLinkReq->joinId = pClientReq->joinId;
	pLinkReq->hton();

	GWS_SendDataPacket(gwsHandle, msgBuff, sizeof(SLinkerLeaveMauTabelReq) + sizeof(UINT16), MAU_SERVER, userID, (ESGSPacketType)pktType, encrypt);

	return LINK_OK;
}

int MauClient::OnMauUserExit(GWSHandle gwsHandle, UINT16 pktType, UINT32 userID, bool encrypt)
{
	UINT32 roleID = MsgProcess::FetchMsgProcess()->FindRoleIDByUserID(userID);
	if (roleID == INVALID_ROLE_ID)
		return LINK_OK;

	char msgBuff[MAX_MAU_MSG_LEN] = { 0 };
	UINT16* gwsCmdCode = (UINT16*)msgBuff;
	*gwsCmdCode = htons(MAU_USER_EXIT);
	UINT32* user = (UINT32*)(msgBuff + sizeof(UINT16));
	*user = htonl(userID);
	UINT32* role = (UINT32*)(user + 1);
	*role = htonl(roleID);

	GWS_SendDataPacket(gwsHandle, msgBuff, sizeof(UINT16) + sizeof(UINT32) * 2, MAU_SERVER, userID, (ESGSPacketType)pktType, encrypt);

	return LINK_OK;
}

int MauClient::ClientMsg(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt)
{
	if (NULL == gwsHandle)
		return LINK_LOGIN_GWS;

	if (sizeof(UINT16) + msgLen > MAX_MAU_MSG_LEN)
	{
		LogDebug(("客户端请求变色龙数据包过大"));
		return LINK_PAKCET_SIZE;
	}

	ASSERT(msgLen >= 0);
	int ReturnCode = LINK_CMD_CODE;
	switch (cmdCode)
	{
	case MAU_CREATE_TABLE:
		ReturnCode = OnMauCreateTable(gwsHandle, msgData, msgLen, cmdCode, pktType, userID, encrypt);
		break;
	case MAU_JION_TABLE:
		ReturnCode = OnMauJoinTable(gwsHandle, msgData, msgLen, cmdCode, pktType, userID, encrypt);
		break;
	case MAU_PLAYER_READY:
		ReturnCode = OnMauReadyTable(gwsHandle, msgData, msgLen, cmdCode, pktType, userID, encrypt);
		break;
	case MAU_LEAVE_TABLE:
		ReturnCode = OnMauLeaveTable(gwsHandle, msgData, msgLen, cmdCode, pktType, userID, encrypt);
		break;
	case MAU_USER_EXIT:
		ReturnCode = OnMauUserExit(gwsHandle, pktType, userID, encrypt);
		break;
	default:
		ASSERT(FALSE);
		LogDebug(("收到错误的命令码请求"));
	}

	return ReturnCode;
}

void MauClient::WGSMsg(SGWSProtocolHead* pHead, UINT16 cmdCode, UINT16 retCode, const void* payload, int payloadLen)
{
	ASSERT(NULL != pHead && NULL != payload && 0 <= payloadLen);
	if (!LINK_MAU_SERVER(cmdCode))
	{
		LogDebug(("收到错误的命令码回复 %d", cmdCode));
		return;	///非法数据
	}

	MsgProcess* msgProc = MsgProcess::FetchMsgProcess();
	ASSERT(NULL != msgProc);
	if (NULL == msgProc)
		return;

	if (retCode != LINK_OK)
	{
		msgProc->InputToSessionError(cmdCode, retCode, pHead->pktType, pHead->userId);
	}
	else
	{
		if (cmdCode == MAU_CREATE_TABLE)	/// 创建牌桌
			msgProc->InputToSessionMsg(pHead->userId, pHead->pktType, cmdCode, retCode, payload, payloadLen);
		else 
		OnRecvMauMsg(pHead->pktType, cmdCode, retCode, payload, payloadLen);
		
	}
}

void MauClient::OnRecvMauMsg(UINT16 pktType, UINT16 cmdCode, UINT16 retCode, const void* payload, int payloadLen)
{
	ASSERT(NULL != payload && 0 < payloadLen);

	std::vector<UINT32> targetArr;
	UINT16 msgOffset = GetMauTarget(payload, payloadLen, targetArr);
	//ASSERT(payloadLen >= msgOffset + sizeof(SJoinTableRes));

	MsgProcess* msgProc = MsgProcess::FetchMsgProcess();
	ASSERT(NULL != msgProc);

	for (int i = 0; i < targetArr.size(); ++i)
	{
		msgProc->InputToSessionMsg(targetArr[i], pktType, cmdCode, retCode, (char*)payload + msgOffset, payloadLen - msgOffset);
	}
}

/// 获取聊天目标，聊天内容在聊天目标之后，返回值为聊天内容需要的偏移量
UINT16 MauClient::GetMauTarget(const void* payload, int payloadLen, std::vector<UINT32>& targetArr)
{
	UINT16 target_num = htons(*(UINT16*)payload);	///	有多少个对象
	UINT32* target_arr = (UINT32*)((char*)payload + sizeof(UINT16));		///	成员列表

	for (UINT16 i = 0; i < target_num; ++i)
	{
		targetArr.push_back(ntohl(target_arr[i]));
	}

	return sizeof(UINT16) + sizeof(UINT32)* target_num;
}