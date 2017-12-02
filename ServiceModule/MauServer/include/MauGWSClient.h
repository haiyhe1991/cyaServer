#ifndef __MAU_GWS_CLIENT_H__
#define __MAU_GWS_CLIENT_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
#pragma once
#endif

#include "GWSSDK.h"

class MauGWSClient
{
public:
	MauGWSClient();
	~MauGWSClient();

	int  Login();
	void Logout();

	int SendBasicResponse(SGWSProtocolHead* pHead, UINT16 retCode, UINT16 cmdCode);
	int SendResponse(SGWSProtocolHead* pHead, UINT16 retCode, UINT16 cmdCode, void* buf, int nLen, BYTE targetId);
	int SendResponse(SGWSProtocolHead* pHead, UINT16 retCode, UINT16 cmdCode, void* buf, int nLen);
	int SendResponse(BYTE targetId, UINT32 userId, UINT16 retCode, UINT16 cmdCode, void* buf, int nLen, BYTE pktType = DATA_PACKET, BYTE sourceId = MAU_SERVER);

private:
	static void GWSDataCallback(SGWSProtocolHead* pHead, const void* payload, int payloadLen, void* context);
	void OnGWSDataCallback(SGWSProtocolHead* pHead, const void* payload, int payloadLen);

private:
	GWSHandle m_gwsHandle;
};


#endif