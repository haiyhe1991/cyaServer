#ifndef __GWS_CLIENT_H__
#define __GWS_CLIENT_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
#pragma once
#endif

#include "GWSSDK.h"

class CGWSClient
{
public:
	static CGWSClient* FetchGateWayClientInstance();
	static void ReleaseGateWayClientInstance();

	INT GateWayClientStart();
	void GateWayClientStop();

	INT InputSessionMsg(const void* data, const INT& dataLen);
private:
	CGWSClient();
	~CGWSClient();

	CGWSClient(const CGWSClient&);
	CGWSClient& operator=(const CGWSClient&);
private:
	static void GWDataCallback(SGWSProtocolHead* pHead, const void* payload, int payloadLen, void* context);
	static void GWEventCallback(EServerEvent evt, const void* data, int nDataLen, void* context);

	void OnGWDataCallback(SGWSProtocolHead* pHead, const void* payload, int payloadLen);
	void OnGWEventCallback(EServerEvent evt, const void* data, int nDataLen);
private:
	static CGWSClient* m_this;
	GWSHandle m_gwHandle;
};


#endif