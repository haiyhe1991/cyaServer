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

	/// 客户端请求数据入口
	virtual int ClientMsg(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt);
	/// 网关服务器数据回调入口
	virtual void WGSMsg(SGWSProtocolHead* pHead, UINT16 cmdCode, UINT16 retCode, const void* payload, int payloadLen);

private:
	/// 请求加入频道
	int OnChatJoinChannel(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt);
	/// 请求离开频道
	int OnChatLeaveChannel(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt);
	/// 请求聊天内容
	int OnChatSendMsg(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt);
	/// 用户退出
	int OnChatUserExit(GWSHandle gwsHandle, UINT16 pktType, UINT32 userID, bool encrypt);

private:
	/// 网关返回聊天数据
	void OnRecvChatMsg(UINT16 pktType, UINT16 cmdCode, UINT16 retCode, const void* payload, int payloadLen);
	/// 获取聊天目标，聊天内容在聊天目标之后，返回值为聊天内容需要的偏移量
	UINT16 GetChatTarget(const void* payload, int payloadLen, std::vector<UINT32>&);
};


#endif