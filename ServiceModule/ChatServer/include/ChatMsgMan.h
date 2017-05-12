#ifndef __CHAT_MSG_MAN_H__
#define __CHAT_MSG_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
�����߼�����
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

	//����Ƶ��
	int OnJoinChannel(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//�뿪Ƶ��
	int OnLeaveChannel(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//�ַ���Ϣ
	int OnDispatchMessage(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//�û��˳�
	int OnUserExit(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

private:
	//������Ϣ
	void OnSendPrivateMessage(GWSClient* gwsSession, SGWSProtocolHead* pHead, SLinkerChatMessageSend* pMsgSend);
	//������Ϣ
	void OnSendUnionMessage(GWSClient* gwsSession, SGWSProtocolHead* pHead, SLinkerChatMessageSend* pMsgSend);
	//������Ϣ
	void OnSendWorldMessage(GWSClient* gwsSession, SGWSProtocolHead* pHead, SLinkerChatMessageSend* pMsgSend);
	//֪ͨ����������Ŀ�겻����
	void OnNotifySenderTargetOffline(GWSClient* gwsSession, const ChannelMan::MemberAttr& sender, SLinkerChatMessageSend* pMsgSend);
	//Ⱥ����Ϣ
	void SendChatMsgEx(GWSClient* gwsSession, EChatChannelType enType, char *nick, BYTE linkerId, SLinkerChatMessageSend* pMsgSend, std::vector<ChannelMan::SChatRecvParam>& vec);
};

void InitChatMsgMan();
ChatMsgMan* GetChatMsgMan();
void DestroyChatMsgMan();

#endif