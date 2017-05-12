#include "ChatMsgMan.h"
#include "GWSClient.h"
#include "cyaLog.h"

#define MAX_MEMBERS_UNION_CHANNEL 256

#define ADD_CHAT_PROC_HANDLER(cmdCode, fun) \
do \
	{   \
	m_msgProcHandler.insert(std::make_pair(cmdCode, &ChatMsgMan::fun)); \
	} while (0)


ChatMsgMan* sg_chatLgcMan = NULL;
void InitChatMsgMan()
{
	ASSERT(sg_chatLgcMan == NULL);
	sg_chatLgcMan = new ChatMsgMan();
	ASSERT(sg_chatLgcMan != NULL);
}

ChatMsgMan* GetChatMsgMan()
{
	return sg_chatLgcMan;
}

void DestroyChatMsgMan()
{
	ChatMsgMan* chatLgcMan = sg_chatLgcMan;
	sg_chatLgcMan = NULL;
	if (chatLgcMan)
		delete chatLgcMan;
}

ChatMsgMan::ChatMsgMan()
{
	ADD_CHAT_PROC_HANDLER(CHAT_JOIN_CHANNEL, OnJoinChannel);
	ADD_CHAT_PROC_HANDLER(CHAT_LEAVE_CHANNEL, OnLeaveChannel);
	ADD_CHAT_PROC_HANDLER(CHAT_SEND_MESSAGE, OnDispatchMessage);
	ADD_CHAT_PROC_HANDLER(CHAT_USER_EXIT, OnUserExit);
}

ChatMsgMan::~ChatMsgMan()
{

}

int ChatMsgMan::ProcessLogic(GWSClient* gwsSession, UINT16 cmdCode, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	int retCode = CHAT_OK;
	std::map<UINT16, fnMsgProcessHandler>::iterator it = m_msgProcHandler.find(cmdCode);
	if (it != m_msgProcHandler.end())
		retCode = (this->*(it->second))(gwsSession, pHead, pData, nDatalen);
	else
		gwsSession->SendBasicResponse(pHead, CHAT_UNKNOWN_CMD_CODE, cmdCode);
	return retCode;
}

//加入频道
int ChatMsgMan::OnJoinChannel(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerJoinChannelReq) == nDatalen);
	if (sizeof(SLinkerJoinChannelReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, CHAT_INVALID_CMD_PACKET, CHAT_JOIN_CHANNEL);

	SLinkerJoinChannelReq* req = (SLinkerJoinChannelReq*)pData;
	req->ntoh();

	int retCode = GetChannelMan()->JoinChannel(req);
	if (req->channel == CHAT_WORLD_CHANNEL)
		LogDebug(("用户[%u]角色[%u]加入世界聊天频道, 返回码[%d]", req->userId, req->roleId, retCode));
	else if (req->channel == CHAT_UNIONS_CHANNEL)
		LogDebug(("用户[%u]角色[%u]加入公会[%u]聊天频道, 返回码[%d]", req->userId, req->roleId, req->joinId, retCode));

	return gwsSession->SendBasicResponse(pHead, retCode, CHAT_JOIN_CHANNEL);
}

//离开频道
int ChatMsgMan::OnLeaveChannel(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerLeaveChannelReq) == nDatalen);
	if (sizeof(SLinkerLeaveChannelReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, CHAT_INVALID_CMD_PACKET, CHAT_LEAVE_CHANNEL);

	SLinkerLeaveChannelReq* req = (SLinkerLeaveChannelReq*)pData;
	req->ntoh();

	int retCode = GetChannelMan()->LeaveChannel(req);
	if (req->channel == CHAT_WORLD_CHANNEL)
		LogDebug(("用户[%u]角色[%u]离开世界聊天频道, 返回码[%d]", req->userId, req->roleId, retCode));
	else if (req->channel == CHAT_UNIONS_CHANNEL)
		LogDebug(("用户[%u]角色[%u]离开公会[%u]聊天频道, 返回码[%d]", req->userId, req->roleId, req->joinId, retCode));

	return gwsSession->SendBasicResponse(pHead, retCode, CHAT_LEAVE_CHANNEL);
}

//分发消息
int ChatMsgMan::OnDispatchMessage(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	int nChatMsgLen = nDatalen - sizeof(SLinkerChatMessageSend)+sizeof(char);
	if (nChatMsgLen <= 0 || nChatMsgLen + sizeof(SServerCommonReply)-sizeof(char) > 2048)
		return CHAT_OK;

	SLinkerChatMessageSend* pMsgSend = (SLinkerChatMessageSend*)pData;
	pMsgSend->ntoh();

	if (pMsgSend->channel == CHAT_WORLD_CHANNEL)
		OnSendWorldMessage(gwsSession, pHead, pMsgSend);
	else if (pMsgSend->channel == CHAT_UNIONS_CHANNEL)
		OnSendUnionMessage(gwsSession, pHead, pMsgSend);
	else if (pMsgSend->channel == CHAT_PRIVATE_CHANNEL)
		OnSendPrivateMessage(gwsSession, pHead, pMsgSend);
	else
		LogError(("收到未知频道[%d]信息!", pMsgSend->channel));

	return CHAT_OK;
}

int ChatMsgMan::OnUserExit(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerChatUserExitReq) == nDatalen);
	if (sizeof(SLinkerChatUserExitReq) != nDatalen)
		return CHAT_OK;

	SLinkerChatUserExitReq* req = (SLinkerChatUserExitReq*)pData;
	req->ntoh();

	GetChannelMan()->UserExit(req->userId, req->roleId);
	LogDebug(("用户[%u]角色[%u]退出聊天频道", req->userId, req->roleId));
	return CHAT_OK;
}

void ChatMsgMan::OnSendPrivateMessage(GWSClient* gwsSession, SGWSProtocolHead* pHead, SLinkerChatMessageSend* pMsgSend)
{
	int nMsgLen = pMsgSend->msgBytes;
	if (nMsgLen > MAX_CHAT_MSG_BYTES)
	{
		LogError(("收到大于%d字节的聊天信息,数据将被丢弃!", MAX_CHAT_MSG_BYTES));
		return;
	}

	//获取发送者昵称
	ChannelMan::MemberAttr senderInfo;
	int retCode = GetChannelMan()->GetWorldChannelMember(pMsgSend->roleId, senderInfo);
	if (retCode != CHAT_OK)
		return;

	//获取接收者信息
	ChannelMan::MemberAttr friendInfo;
	retCode = GetChannelMan()->GetWorldChannelMember(pMsgSend->targetId, friendInfo);
	if (retCode != CHAT_OK)
	{
		OnNotifySenderTargetOffline(gwsSession, senderInfo, pMsgSend);
		return;
	}

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;

	*(UINT16*)(buf + nLen) = htons(1);
	nLen += sizeof(UINT16);

	SLinkerChatRecvObj* pRecvObj = (SLinkerChatRecvObj*)(buf + nLen);
	pRecvObj->recvUserId = friendInfo.userId;
	pRecvObj->hton();
	nLen += sizeof(SLinkerChatRecvObj);

	SChatMessageRecv* pRecvMsg = (SChatMessageRecv*)(buf + nLen);
	pRecvMsg->channel = CHAT_PRIVATE_CHANNEL;
	pRecvMsg->sendSourceId = pMsgSend->roleId;
	strcpy(pRecvMsg->nick, senderInfo.nick);
	pRecvMsg->sendTargetId = pMsgSend->targetId;
	pRecvMsg->msgBytes = nMsgLen;
	pRecvMsg->hton();
	nLen += sizeof(SChatMessageRecv)-sizeof(char);

	memcpy(buf + nLen, pMsgSend->msg, nMsgLen);
	nLen += nMsgLen;
	gwsSession->SendResponse(friendInfo.linkerId, 0, CHAT_OK, CHAT_RECV_MESSAGE, buf, nLen);
}

void ChatMsgMan::OnSendUnionMessage(GWSClient* gwsSession, SGWSProtocolHead* pHead, SLinkerChatMessageSend* pMsgSend)
{
	int nMsgLen = pMsgSend->msgBytes;
	if (nMsgLen > MAX_CHAT_MSG_BYTES)
	{
		LogError(("收到大于%d字节的公会频道聊天信息,数据将被丢弃!", MAX_CHAT_MSG_BYTES));
		return;
	}

	//获取发送者昵称
	ChannelMan::MemberAttr sender;
	int retCode = GetChannelMan()->GetUnionChannelMember(pMsgSend->targetId, pMsgSend->roleId, sender);
	if (retCode != CHAT_OK)
		return;

	//获取发送者所在公会所有成员
	std::map<BYTE, std::vector<ChannelMan::SChatRecvParam> > membersMap;
	retCode = GetChannelMan()->GetUnionChannelAllMembers(pMsgSend->roleId, pMsgSend->targetId, membersMap);
	if (retCode != CHAT_OK)
		return;

	std::map<BYTE, std::vector<ChannelMan::SChatRecvParam> >::iterator it = membersMap.begin();
	for (; it != membersMap.end(); ++it)
	{
		std::vector<ChannelMan::SChatRecvParam>& vec = it->second;
		ASSERT(vec.size() > 0);

		SendChatMsgEx(gwsSession, CHAT_WORLD_CHANNEL, sender.nick, it->first, pMsgSend, vec);
	}
}

void ChatMsgMan::OnSendWorldMessage(GWSClient* gwsSession, SGWSProtocolHead* pHead, SLinkerChatMessageSend* pMsgSend)
{
	int nMsgLen = pMsgSend->msgBytes;
	if (nMsgLen > MAX_CHAT_MSG_BYTES)
	{
		LogError(("收到大于%d字节世界频道的聊天信息,数据将被丢弃!", MAX_CHAT_MSG_BYTES));
		return;
	}

	//获取发送者昵称
	ChannelMan::MemberAttr sender;
	int retCode = GetChannelMan()->GetWorldChannelMember(pMsgSend->roleId, sender);
	if (retCode != CHAT_OK)
		return;

	//获取公共频道内所有成员
	std::map<BYTE/*linkerId*/, std::vector<ChannelMan::SChatRecvParam> > membersMap;
	retCode = GetChannelMan()->GetWorldChannelAllMembers(pMsgSend->roleId, membersMap);
	if (retCode != CHAT_OK)
		return;

	std::map<BYTE/*linkerId*/, std::vector<ChannelMan::SChatRecvParam> >::iterator it = membersMap.begin();
	for (; it != membersMap.end(); ++it)
	{
		std::vector<ChannelMan::SChatRecvParam>& vec = it->second;
		ASSERT(vec.size() > 0);

		SendChatMsgEx(gwsSession, CHAT_WORLD_CHANNEL, sender.nick, it->first, pMsgSend, vec);
	}
}

void ChatMsgMan::OnNotifySenderTargetOffline(GWSClient* gwsSession, const ChannelMan::MemberAttr& sender, SLinkerChatMessageSend* pMsgSend)
{
	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	UINT16 nMsgLen = pMsgSend->msgBytes;

	SChatTargetOfflineNotify* pOfflineNotify = (SChatTargetOfflineNotify*)(buf + nLen);
	pOfflineNotify->channel = CHAT_PRIVATE_CHANNEL;
	pOfflineNotify->targetId = pMsgSend->targetId;
	pOfflineNotify->msgBytes = nMsgLen;
	pOfflineNotify->hton();
	nLen += sizeof(SChatTargetOfflineNotify)-sizeof(char);

	memcpy(buf + nLen, pMsgSend->msg, nMsgLen);
	nLen += nMsgLen;
	gwsSession->SendResponse(sender.linkerId, sender.userId, CHAT_OK, CHAT_USER_OFFLINE_NOTIFY, buf, nLen);
}

// 消息构成: SServerCommonReply + userid_num + userids + SChatMessageRecv + MsgData
void ChatMsgMan::SendChatMsgEx(GWSClient* gwsSession, EChatChannelType enType, char *nick, BYTE linkerId, SLinkerChatMessageSend* pMsgSend, std::vector<ChannelMan::SChatRecvParam>& vec)
{
	int index = 0, count = 0;
	int totalMembers = (int)vec.size();
	while (totalMembers > 0)
	{
		BOOL bFindSelf = false;
		char buf[MAX_BUF_LEN] = { 0 };
		int  nLen = GWS_COMMON_REPLY_LEN;
		UINT16 num = totalMembers > MAX_MEMBERS_UNION_CHANNEL ? MAX_MEMBERS_UNION_CHANNEL : (UINT16)totalMembers;

		totalMembers -= num;
		count += num;
		nLen += sizeof(UINT16);
		for (; index < count; ++index)
		{
			SLinkerChatRecvObj* pRecvObj = (SLinkerChatRecvObj*)(buf + nLen);
			pRecvObj->recvUserId = vec[index].recvUserId;
			if (pMsgSend->userId == pRecvObj->recvUserId)
			{
				// 不能发给自己
				bFindSelf = true;
				continue;
			}
			pRecvObj->hton();
			nLen += sizeof(SLinkerChatRecvObj);
		}

		if (bFindSelf)
			*(UINT16*)(buf + GWS_COMMON_REPLY_LEN) = htons(num - 1);
		else
			*(UINT16*)(buf + GWS_COMMON_REPLY_LEN) = htons(num);

		SChatMessageRecv* pMsgRecv = (SChatMessageRecv*)(buf + nLen);
		strcpy(pMsgRecv->nick, nick);
		pMsgRecv->channel = (BYTE)enType;
		pMsgRecv->sendSourceId = pMsgSend->roleId;
		pMsgRecv->sendTargetId = pMsgSend->targetId;
		pMsgRecv->msgBytes = pMsgSend->msgBytes;
		pMsgRecv->hton();
		nLen += sizeof(SChatMessageRecv)-sizeof(char);

		memcpy(buf + nLen, pMsgSend->msg, pMsgSend->msgBytes);
		nLen += pMsgSend->msgBytes;
		gwsSession->SendResponse(linkerId, 0, CHAT_OK, CHAT_RECV_MESSAGE, buf, nLen);
	}
}
