#ifndef __CHAT_MSG_MAN_H__
#define __CHAT_MSG_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
聊天逻辑处理
*/

#include <map>
#include "GWSProtocol.h"
#include "ChatProtocol.h"
#include "ChannelMan.h"

class GWSClient;
class ChatMsgMan
{
public:
	ChatMsgMan();
	~ChatMsgMan();

	int ProcessLogic(GWSClient* gwsSession, UINT16 cmdCode, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

private:
	typedef int (ChatMsgMan::*fnMsgProcessHandler)(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	std::map<UINT16, fnMsgProcessHandler> m_msgProcHandler;

	//加入频道
	int OnJoinChannel(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//离开频道
	int OnLeaveChannel(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//分发消息
	int OnDispatchMessage(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//用户退出
	int OnUserExit(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

private:
	//好友信息
	void OnSendPrivateMessage(GWSClient* gwsSession, SGWSProtocolHead* pHead, SLinkerChatMessageSend* pMsgSend);
	//公会消息
	void OnSendUnionMessage(GWSClient* gwsSession, SGWSProtocolHead* pHead, SLinkerChatMessageSend* pMsgSend);
	//世界消息
	void OnSendWorldMessage(GWSClient* gwsSession, SGWSProtocolHead* pHead, SLinkerChatMessageSend* pMsgSend);
	//通知发送者聊天目标不在线
	void OnNotifySenderTargetOffline(GWSClient* gwsSession, const ChannelMan::MemberAttr& sender, SLinkerChatMessageSend* pMsgSend);
	//群发消息
	void SendChatMsgEx(GWSClient* gwsSession, EChatChannelType enType, char *nick, BYTE linkerId, SLinkerChatMessageSend* pMsgSend, std::vector<ChannelMan::SChatRecvParam>& vec);
};

void InitChatMsgMan();
ChatMsgMan* GetChatMsgMan();
void DestroyChatMsgMan();

#endif