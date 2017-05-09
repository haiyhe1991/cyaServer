/*
各种消息处理基类
不同的服务器消息用不同的基类
每个基类对应一个网关连接
*/
#ifndef __BASE_CLIENT_H__
#define __BASE_CLIENT_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "GWSSDK.h"
#include "GWSProtocol.h"

struct BaseClient
{
	virtual ~BaseClient() {}
	/// 向网关写入消息，GWS消息异常写入，不会耗时
	virtual int ClientMsg(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt) = 0;
	/// 网关消息回调
	virtual void WGSMsg(SGWSProtocolHead* pHead, UINT16 cmdCode, UINT16 retCode, const void* payload, int payloadLen) = 0;
};

#endif ///_SGS_LINKSERVER_BASECLIENT_H_