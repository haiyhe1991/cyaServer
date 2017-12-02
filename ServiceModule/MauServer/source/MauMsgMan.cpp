#include "MauMsgMan.h"
#include "MauGWSClient.h"
#include "cyaLog.h"
#include "ServiceMau.h"

#define MAX_MEMBERS_TABLE_CHANNEL 256

#define ADD_MAU_PROC_HANDLER(cmdCode, fun) \
do \
	{   \
	m_msgProcHandler.insert(std::make_pair(cmdCode, &MauMsgMan::fun)); \
	} while (0)


static MauMsgMan* sg_mauLgcMan = NULL;
void InitMauMsgMan()
{
	ASSERT(sg_mauLgcMan == NULL);
	sg_mauLgcMan = new MauMsgMan();
	ASSERT(sg_mauLgcMan != NULL);
}

MauMsgMan* GetMauMsgMan()
{
	return sg_mauLgcMan;
}

void DestroyMauMsgMan()
{
	MauMsgMan* mauLgcMan = sg_mauLgcMan;
	sg_mauLgcMan = NULL;
	if (mauLgcMan)
		delete mauLgcMan;
}

MauMsgMan::MauMsgMan()
{
	ADD_MAU_PROC_HANDLER(MAU_CREATE_TABLE, OnCreateMauTable);
	ADD_MAU_PROC_HANDLER(MAU_JION_TABLE, OnJoinMauTable);
	ADD_MAU_PROC_HANDLER(MAU_PLAYER_READY, OnReadyTable);
	ADD_MAU_PROC_HANDLER(MAU_LEAVE_TABLE, OnLeaveMauTable);
	//ADD_MAU_PROC_HANDLER(MAU_PLAYER_READY, OnUserExit);
}

MauMsgMan::~MauMsgMan()
{

}

int MauMsgMan::ProcessLogic(MauGWSClient* gwsSession, UINT16 cmdCode, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	int retCode = MAU_OK;
	std::map<UINT16, fnMsgProcessHandler>::iterator it = m_msgProcHandler.find(cmdCode);
	if (it != m_msgProcHandler.end())
		retCode = (this->*(it->second))(gwsSession, pHead, pData, nDatalen);
	else
		gwsSession->SendBasicResponse(pHead, MAU_UNKNOWN_CMD_CODE, cmdCode);
	return retCode;
}

//创建牌桌
int MauMsgMan::OnCreateMauTable(MauGWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerCreateMauTabelReq) == nDatalen);
	if (sizeof(SLinkerCreateMauTabelReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MAU_INVALID_CMD_PACKET, MAU_CREATE_TABLE);

	SLinkerCreateMauTabelReq* req = (SLinkerCreateMauTabelReq*)pData;
	req->ntoh();

	UINT32 unionId = 999999;
	BYTE tableId = 0;
	int retCode = GetMauTableMan()->CreateMauTabel(req, unionId, tableId);
	LogDebug(("用户[%u]角色[%u]创建牌桌[%u], 返回码[%d]", req->userId, req->roleId, unionId, retCode));
	if (retCode != MAU_OK)
	{
		return gwsSession->SendBasicResponse(pHead, retCode, MAU_CREATE_TABLE);
	}
	else
	{
		char buf[MAX_BUF_LEN] = { 0 };
		int nLen = GWS_COMMON_REPLY_LEN;
		SJoinTableRes* pMsg = (SJoinTableRes*)(buf + nLen);
		pMsg->rolesCount = 1;
		strcpy(pMsg->roles[0].nick, req->nick);
		pMsg->roles[0].roleId = req->roleId;
		pMsg->roles[0].joinId = unionId;
		pMsg->roles[0].userId = req->userId;
		pMsg->roles[0].tableId = tableId;
		pMsg->roles[0].hton();
		nLen += sizeof(SJoinTableRes);
		return gwsSession->SendResponse(pHead, retCode, MAU_CREATE_TABLE, buf, nLen);
	}
}

//加入牌桌
int MauMsgMan::OnJoinMauTable(MauGWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerJoinMauTabelReq) == nDatalen);
	if (sizeof(SLinkerJoinMauTabelReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MAU_INVALID_CMD_PACKET, MAU_JION_TABLE);

	SLinkerJoinMauTabelReq* req = (SLinkerJoinMauTabelReq*)pData;
	req->ntoh();

	BYTE tableId = 0;
	int retCode = GetMauTableMan()->JoinMauTabel(req, tableId);
	LogDebug(("用户[%u]角色[%u]加入牌桌[%u], 返回码[%d]", req->userId, req->roleId, req->joinId, retCode));
	if (retCode != MAU_OK)
	{
		return gwsSession->SendBasicResponse(pHead, retCode, MAU_JION_TABLE);
	}
	else
	{
		char buf[MAX_BUF_LEN] = { 0 };
		SJoinTableRes* res = (SJoinTableRes*)buf;
		BYTE count = 0;
		//获取发送者所在公会所有成员
		std::map<BYTE, std::vector<MauTableMan::SMauRecvParam> > membersMap;
		retCode = GetMauTableMan()->GetUnionMauTabelAllMembers(req->roleId, req->joinId, membersMap);
		if (retCode != MAU_OK)
			return retCode;
		std::map<BYTE, std::vector<MauTableMan::SMauRecvParam> >::iterator it = membersMap.begin();
		for (; it != membersMap.end(); ++it)
		{
			std::vector<MauTableMan::SMauRecvParam>& vec = it->second;
			ASSERT(vec.size() > 0);
			std::vector<MauTableMan::SMauRecvParam>::iterator itRecv = vec.begin();
			while (itRecv != vec.end())
			{
				strcpy(res->roles[count].nick, itRecv->nick);
				res->roles[count].roleId = itRecv->recvRoleId;
				res->roles[count].userId = itRecv->recvUserId;
				res->roles[count].joinId = req->joinId;
				res->roles[count].tableId = itRecv->tableId;
				res->roles[count].ready = itRecv->ready;
				res->roles[count].hton();
				count++;
				itRecv++;
			}
		}
		res->rolesCount = count;
		int nLen = sizeof(SJoinTableRes) + (res->rolesCount - 1) * sizeof(SJoinTableInfo);
		for (it = membersMap.begin(); it != membersMap.end(); ++it)
		{
			std::vector<MauTableMan::SMauRecvParam>& vec = it->second;
			ASSERT(vec.size() > 0);
			SendMauMsgEx(gwsSession, it->first, MAU_JION_TABLE, vec, buf, nLen);
		}
	}
	return retCode;
}

//玩家准备
int MauMsgMan::OnReadyTable(MauGWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerReadyTabelReq) == nDatalen);
	if (sizeof(SLinkerReadyTabelReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MAU_INVALID_CMD_PACKET, MAU_LEAVE_TABLE);

	SLinkerReadyTabelReq* req = (SLinkerReadyTabelReq*)pData;
	req->ntoh();
	BYTE tableId = 0;
	int retCode = GetMauTableMan()->ReadyTabel(req, tableId);
	LogDebug(("用户[%u]角色[%u]准备牌桌[%u],牌桌id[%u], 返回码[%d]", req->userId, req->roleId, req->joinId, tableId, retCode));
	if (retCode != MAU_OK)
	{
		return gwsSession->SendBasicResponse(pHead, retCode, MAU_PLAYER_READY);
	}
	else
	{
		char buf[MAX_BUF_LEN] = { 0 };
		SReadyTableRes* res = (SReadyTableRes*)buf;
		//获取发送者所在公会所有成员
		std::map<BYTE, std::vector<MauTableMan::SMauRecvParam> > membersMap;
		retCode = GetMauTableMan()->GetUnionMauTabelAllMembers(req->roleId, req->joinId, membersMap);
		if (retCode != MAU_OK)
			return retCode;
		std::map<BYTE, std::vector<MauTableMan::SMauRecvParam> >::iterator it = membersMap.begin();
		res->ready = req->ready;
		res->tableId = tableId;
		int nLen = sizeof(SReadyTableRes);
		for (it = membersMap.begin(); it != membersMap.end(); ++it)
		{
			std::vector<MauTableMan::SMauRecvParam>& vec = it->second;
			ASSERT(vec.size() > 0);
			SendMauMsgEx(gwsSession, it->first, MAU_PLAYER_READY, vec, buf, nLen);
		}
	}
	return retCode;
}

//离开频道
int MauMsgMan::OnLeaveMauTable(MauGWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerLeaveMauTabelReq) == nDatalen);
	if (sizeof(SLinkerLeaveMauTabelReq) != nDatalen)
		return gwsSession->SendBasicResponse(pHead, MAU_INVALID_CMD_PACKET, MAU_LEAVE_TABLE);

	SLinkerLeaveMauTabelReq* req = (SLinkerLeaveMauTabelReq*)pData;
	req->ntoh();

	BYTE tableId = 0;
	int retCode = GetMauTableMan()->LeaveMauTabel(req, tableId);
	LogDebug(("用户[%u]角色[%u]离开牌桌[%u],牌桌id[%u], 返回码[%d]", req->userId, req->roleId, req->joinId, tableId,retCode));

	return gwsSession->SendBasicResponse(pHead, retCode, MAU_LEAVE_TABLE);
}

int MauMsgMan::OnUserExit(MauGWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen)
{
	ASSERT(sizeof(SLinkerChatUserExitReq) == nDatalen);
	if (sizeof(SLinkerChatUserExitReq) != nDatalen)
		return CHAT_OK;

	SLinkerChatUserExitReq* req = (SLinkerChatUserExitReq*)pData;
	req->ntoh();
	BYTE tableId = 0;
	int retCode = GetMauTableMan()->UserExit(req->userId, req->roleId, tableId);
	LogDebug(("用户[%u]角色[%u]退出牌桌[%u], 返回码[%d]", req->userId, req->roleId, tableId, retCode));
	return CHAT_OK;
}

//void MauMsgMan::OnSendUnionMessage(MauGWSClient* gwsSession, SGWSProtocolHead* pHead, SLinkerMauBasicSend* pMsgSend)
//{
//	//获取发送者昵称
//	MauTableMan::MemberAttr sender;
//	int retCode = GetMauTableMan()->GetUnionMauTabelMember(pMsgSend->targetId, pMsgSend->roleId, sender);
//	if (retCode != CHAT_OK)
//		return;
//
//	//获取发送者所在公会所有成员
//	std::map<BYTE, std::vector<MauTableMan::SMauRecvParam> > membersMap;
//	retCode = GetMauTableMan()->GetUnionMauTabelAllMembers(pMsgSend->roleId, pMsgSend->targetId, membersMap);
//	if (retCode != CHAT_OK)
//		return;
//
//	std::map<BYTE, std::vector<MauTableMan::SMauRecvParam> >::iterator it = membersMap.begin();
//	for (; it != membersMap.end(); ++it)
//	{
//		std::vector<MauTableMan::SMauRecvParam>& vec = it->second;
//		ASSERT(vec.size() > 0);
//
//		SendMauMsgEx(gwsSession, sender.nick, it->first, pMsgSend, vec);
//	}
//}

// 消息构成: SServerCommonReply + userid_num + userids + SChatMessageRecv + MsgData
void MauMsgMan::SendMauMsgEx(MauGWSClient* gwsSession, BYTE linkerId,UINT16 cmdCode, std::vector<MauTableMan::SMauRecvParam>& vec, const char* pData, int nDatalen)
{
	int index = 0, count = 0;
	int totalMembers = (int)vec.size();
	while (totalMembers > 0)
	{
		char buf[MAX_BUF_LEN] = { 0 };
		int  nLen = GWS_COMMON_REPLY_LEN;
		UINT16 num = totalMembers > MAX_MEMBERS_TABLE_CHANNEL ? MAX_MEMBERS_TABLE_CHANNEL : (UINT16)totalMembers;

		totalMembers -= num;
		count += num;
		nLen += sizeof(UINT16);
		for (; index < count; ++index)
		{
			SLinkerMauRecvObj* pRecvObj = (SLinkerMauRecvObj*)(buf + nLen);
			pRecvObj->recvUserId = vec[index].recvUserId;
			pRecvObj->hton();
			nLen += sizeof(SLinkerMauRecvObj);
		}

		*(UINT16*)(buf + GWS_COMMON_REPLY_LEN) = htons(num);

		//发送的信息
		ASSERT(nLen + nDatalen < MAX_BUF_LEN);
		memcpy(buf + nLen, (const char *)pData, nDatalen);
		nLen += nDatalen;

		gwsSession->SendResponse(linkerId, 0, MAU_OK, cmdCode, buf, nLen);
	}
}
