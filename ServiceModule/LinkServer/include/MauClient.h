#ifndef __MAU_CLIENT_H__
#define __MAU_CLIENT_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include <vector>
#include "BaseClient.h"

class MauClient : public BaseClient
{
public:
	MauClient();
	~MauClient();

	/// 客户端请求数据入口
	virtual int ClientMsg(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt);
	/// 网关服务器数据回调入口
	virtual void WGSMsg(SGWSProtocolHead* pHead, UINT16 cmdCode, UINT16 retCode, const void* payload, int payloadLen);

private:
	/// 请求创建牌桌
	int OnMauCreateTable(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt);
	/// 请求加入牌桌
	int OnMauJoinTable(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt);
	/// 请求准备
	int OnMauReadyTable(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt);
	/// 请求离开牌桌
	int OnMauLeaveTable(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt);
	/// 用户退出
	int OnMauUserExit(GWSHandle gwsHandle, UINT16 pktType, UINT32 userID, bool encrypt);

private:
	/// 网关返回聊天数据
	void OnRecvMauMsg(UINT16 pktType, UINT16 cmdCode, UINT16 retCode, const void* payload, int payloadLen);
	/// 获取聊天目标，聊天内容在聊天目标之后，返回值为聊天内容需要的偏移量
	UINT16 GetMauTarget(const void* payload, int payloadLen, std::vector<UINT32>&);
};

#endif