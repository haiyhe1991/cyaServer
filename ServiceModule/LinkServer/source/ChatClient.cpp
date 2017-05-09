#include "ChatClient.h"
#include "ChatProtocol.h"
#include "ServiceErrorCode.h"
#include "MsgProcess.h"
#include "LinkGlobalObj.h"
#include "LinkConfig.h"
#include "cyaLog.h"
#include "ServiceChat.h"
#include "PubFunction.h"

#define MAX_CHAT_MSG_LEN 1024

ChatClient::ChatClient()
{

}

ChatClient::~ChatClient()
{

}

int ChatClient::OnChatJoinChannel(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt)
{
	UINT32 roleID = MsgProcess::FetchMsgProcess()->FindRoleIDByUserID(userID);
	if (roleID == INVALID_ROLE_ID)
		return LINK_OK;

	if (msgLen < sizeof(SJoinChannelReq))
	{
		LogDebug(("����Ƶ�����ݰ��쳣"));
		return LINK_ERROR;
	}

	SJoinChannelReq* pClientReq = (SJoinChannelReq*)msgData;
	pClientReq->ntoh();

	char msgBuff[MAX_CHAT_MSG_LEN] = { 0 };
	UINT16* gwsCmdCode = (UINT16*)msgBuff;
	*gwsCmdCode = htons(cmdCode);
	SLinkerJoinChannelReq* req = (SLinkerJoinChannelReq*)(msgBuff + sizeof(UINT16));
	{
		req->channel = pClientReq->channel;
		req->linkerId = GlobalGetLinkConfig()->GetLinkNumber();
		req->userId = userID;
		req->roleId = roleID;
		req->joinId = pClientReq->joinId;
		memcpy(req->nick, pClientReq->nick, 32);
		req->nick[32] = '\0';
		req->hton();
	}

	GWS_SendDataPacket(gwsHandle, msgBuff, sizeof(SLinkerJoinChannelReq)+sizeof(UINT16), CHAT_SERVER, userID, (ESGSPacketType)pktType, encrypt);

	return LINK_OK;
}

int ChatClient::OnChatLeaveChannel(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt)
{
	UINT32 roleID = MsgProcess::FetchMsgProcess()->FindRoleIDByUserID(userID);
	if (roleID == INVALID_ROLE_ID)
		return LINK_OK;

	if (sizeof(SLeaveChannelReq) > msgLen)
	{
		LogDebug(("�뿪Ƶ�������쳣"));
		return LINK_ERROR;
	}
	SLeaveChannelReq* pClientReq = (SLeaveChannelReq*)msgData;
	pClientReq->ntoh();

	char msgBuff[MAX_CHAT_MSG_LEN] = { 0 };
	UINT16* gwsCmdCode = (UINT16*)msgBuff;
	*gwsCmdCode = htons(cmdCode);
	SLinkerLeaveChannelReq* pLinkReq = (SLinkerLeaveChannelReq*)(msgBuff + sizeof(UINT16));
	pLinkReq->userId = userID;
	pLinkReq->roleId = roleID;
	pLinkReq->channel = pClientReq->channel;
	pLinkReq->joinId = pClientReq->joinId;
	pLinkReq->hton();

	GWS_SendDataPacket(gwsHandle, msgBuff, sizeof(SLinkerLeaveChannelReq)+sizeof(UINT16), CHAT_SERVER, userID, (ESGSPacketType)pktType, encrypt);

	return LINK_OK;
}

int ChatClient::OnChatSendMsg(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt)
{
	if (sizeof(SChatMessageSend)-sizeof(char) >= msgLen)
	{
		LogDebug(("������Ϣ���ݰ��쳣"));
		return LINK_ERROR;
	}

	UINT32 roleID = MsgProcess::FetchMsgProcess()->FindRoleIDByUserID(userID);
	if (roleID == INVALID_ROLE_ID)
		return LINK_OK;

	char msgBuff[MAX_CHAT_MSG_LEN] = { 0 };
	UINT16* gwsCmdCode = (UINT16*)msgBuff;
	*gwsCmdCode = htons(cmdCode);
	UINT32* user = (UINT32*)(msgBuff + sizeof(UINT16));
	*user = htonl(userID);
	UINT32* role = (UINT32*)(user + 1);
	*role = htonl(roleID);
	if (NULL != msgData && 0 < msgLen)
		memcpy(role + 1, msgData, msgLen);

	GWS_SendDataPacket(gwsHandle, msgBuff, msgLen + sizeof(UINT16)+sizeof(UINT32)* 2, CHAT_SERVER, userID, (ESGSPacketType)pktType, encrypt);

	return LINK_OK;
}

int ChatClient::OnChatUserExit(GWSHandle gwsHandle, UINT16 pktType, UINT32 userID, bool encrypt)
{
	UINT32 roleID = MsgProcess::FetchMsgProcess()->FindRoleIDByUserID(userID);
	if (roleID == INVALID_ROLE_ID)
		return LINK_OK;

	char msgBuff[MAX_CHAT_MSG_LEN] = { 0 };
	UINT16* gwsCmdCode = (UINT16*)msgBuff;
	*gwsCmdCode = htons(CHAT_USER_EXIT);
	UINT32* user = (UINT32*)(msgBuff + sizeof(UINT16));
	*user = htonl(userID);
	UINT32* role = (UINT32*)(user + 1);
	*role = htonl(roleID);

	GWS_SendDataPacket(gwsHandle, msgBuff, sizeof(UINT16)+sizeof(UINT32)* 2, CHAT_SERVER, userID, (ESGSPacketType)pktType, encrypt);

	return LINK_OK;
}

int ChatClient::ClientMsg(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt)
{
	if (NULL == gwsHandle)
		return LINK_LOGIN_GWS;

	if (sizeof(UINT16)+msgLen > MAX_CHAT_MSG_LEN)
	{
		LogDebug(("�ͻ��������������ݰ�����"));
		return LINK_PAKCET_SIZE;
	}

	ASSERT(msgLen >= 0);
	int ReturnCode = LINK_CMD_CODE;
	switch (cmdCode)
	{
	case CHAT_JOIN_CHANNEL:
		ReturnCode = OnChatJoinChannel(gwsHandle, msgData, msgLen, cmdCode, pktType, userID, encrypt);
		break;
	case CHAT_SEND_MESSAGE:
		ReturnCode = OnChatSendMsg(gwsHandle, msgData, msgLen, cmdCode, pktType, userID, encrypt);
		break;
	case CHAT_LEAVE_CHANNEL:
		ReturnCode = OnChatLeaveChannel(gwsHandle, msgData, msgLen, cmdCode, pktType, userID, encrypt);
		break;
	case CHAT_USER_EXIT:
		ReturnCode = OnChatUserExit(gwsHandle, pktType, userID, encrypt);
		break;
	default:
		ASSERT(FALSE);
		LogDebug(("�յ����������������"));
	}

	return ReturnCode;
}

void ChatClient::WGSMsg(SGWSProtocolHead* pHead, UINT16 cmdCode, UINT16 retCode, const void* payload, int payloadLen)
{
	ASSERT(NULL != pHead && NULL != payload && 0 <= payloadLen);
	if (!LINK_CHAT_SERVER(cmdCode))
	{
		LogDebug(("�յ������������ظ� %d", cmdCode));
		return;	///�Ƿ�����
	}

	MsgProcess* msgProc = MsgProcess::FetchMsgProcess();
	ASSERT(NULL != msgProc);
	if (NULL == msgProc)
		return;

	if (retCode != CHAT_OK)
	{
		msgProc->InputToSessionError(cmdCode, retCode, pHead->pktType, pHead->userId);
	}
	else
	{
		if (cmdCode == CHAT_RECV_MESSAGE)	/// �������ݴ���
			OnRecvChatMsg(pHead->pktType, cmdCode, retCode, payload, payloadLen);
		else
			msgProc->InputToSessionMsg(pHead->userId, pHead->pktType, cmdCode, retCode, payload, payloadLen);
	}
}

void ChatClient::OnRecvChatMsg(UINT16 pktType, UINT16 cmdCode, UINT16 retCode, const void* payload, int payloadLen)
{
	ASSERT(NULL != payload && 0 < payloadLen);

	std::vector<UINT32> targetArr;
	UINT16 msgOffset = GetChatTarget(payload, payloadLen, targetArr);
	ASSERT(payloadLen >= msgOffset + sizeof(SChatMessageRecv));

	MsgProcess* msgProc = MsgProcess::FetchMsgProcess();
	ASSERT(NULL != msgProc);

	for (int i = 0; i < targetArr.size(); ++i)
	{
		msgProc->InputToSessionMsg(targetArr[i], pktType, cmdCode, retCode, (char*)payload + msgOffset, payloadLen - msgOffset);
	}
}

/// ��ȡ����Ŀ�꣬��������������Ŀ��֮�󣬷���ֵΪ����������Ҫ��ƫ����
UINT16 ChatClient::GetChatTarget(const void* payload, int payloadLen, std::vector<UINT32>& targetArr)
{
	UINT16 target_num = htons(*(UINT16*)payload);	///	�ж��ٸ�����
	UINT32* target_arr = (UINT32*)((char*)payload + sizeof(UINT16));		///	��Ա�б�

	for (UINT16 i = 0; i < target_num; ++i)
	{
		targetArr.push_back(ntohl(target_arr[i]));
	}

	return sizeof(UINT16)+sizeof(UINT32)* target_num;
}