#ifndef __GWS_CLIENT_H__
#define __GWS_CLIENT_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
网关服务器数据操作管理类
*/

#include "GWSSDK.h"

class GWSClient
{
public:
	GWSClient();
	~GWSClient();

	int  Login();
	void Logout();

private:
	static void GWSDataCallback(SGWSProtocolHead* pHead, const void* payload, int payloadLen, void* context);
	void OnGWSDataCallback(SGWSProtocolHead* pHead, const void* payload, int payloadLen);

public:
	//发送基本回复
	int SendBasicResponse(SGWSProtocolHead* pHead, UINT16 retCode, UINT16 cmdCode);
	//发送基本回复
	int SendBasicResponse(BYTE targetId, UINT32 userId, UINT16 retCode, UINT16 cmdCode);
	//发送回复
	int SendResponse(SGWSProtocolHead* pHead, UINT16 retCode, UINT16 cmdCode, void* buf, int nLen);
	//发送回复
	int SendResponse(BYTE targetId, UINT32 userId, UINT16 retCode, UINT16 cmdCode, void* buf, int nLen, BYTE pktType = DATA_PACKET, BYTE sourceId = MAIN_LOGIC_SERVER);

private:
	GWSHandle m_gwsHandle;
};

#endif	//_MLogicServerGWSClient_h__