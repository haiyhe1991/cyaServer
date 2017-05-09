#ifndef __CHAT_CLIENT_H__
#define __CHAT_CLIENT_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include <vector>
#include "BaseClient.h"

class ChatClient : public BaseClient
{
public:
	ChatClient();
	~ChatClient();

	/// �ͻ��������������
	virtual int ClientMsg(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt);
	/// ���ط��������ݻص����
	virtual void WGSMsg(SGWSProtocolHead* pHead, UINT16 cmdCode, UINT16 retCode, const void* payload, int payloadLen);

private:
	/// �������Ƶ��
	int OnChatJoinChannel(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt);
	/// �����뿪Ƶ��
	int OnChatLeaveChannel(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt);
	/// ������������
	int OnChatSendMsg(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt);
	/// �û��˳�
	int OnChatUserExit(GWSHandle gwsHandle, UINT16 pktType, UINT32 userID, bool encrypt);

private:
	/// ���ط�����������
	void OnRecvChatMsg(UINT16 pktType, UINT16 cmdCode, UINT16 retCode, const void* payload, int payloadLen);
	/// ��ȡ����Ŀ�꣬��������������Ŀ��֮�󣬷���ֵΪ����������Ҫ��ƫ����
	UINT16 GetChatTarget(const void* payload, int payloadLen, std::vector<UINT32>&);
};


#endif