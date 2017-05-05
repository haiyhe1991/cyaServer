#ifndef __GWS_HELPER_H__
#define __GWS_HELPER_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include <map>
#include "cyaLocker.h"
#include "GWSSdk.h"
#include "cyaRefCount.h"
#include "cyaTcp.h"
#include "cyaCoreTimer.h"

class GWSClientSession;
class GWSHelper
{
public:
	GWSHelper(const char* gwsIp, int gwsPort,
		EServerType serverType,
		fnDataCallback cb, void* context,
		unsigned int connTimeout = 5000,
		unsigned int writeTimeout = 0,
		unsigned int readTimeout = 0);
	~GWSHelper();

	int  Login();
	void Logout();
	void SetEventCallback(fnEventCallback cb, void* context);

	void ReleaseHandle();
	int  SendDataPacket(const void* data, int dataBytes, int targetId, unsigned int userId, ESGSPacketType pktType, int isCrypt, long* seq);
	int  SendDataPacketEx(const SGWSProtocolHead* pHead, const void* data, int dataBytes);

	void NotifySessionBroken();
	void SetLocalSession(GWSClientSession* session);
	void InputData(SGWSProtocolHead* pHeader, const void* payload, int payloadBytes);

private:
	long  OnGetCmdSeq();
	void  OnMakeProtocolHeader(SGWSProtocolHead* pHeader, int pduLen, ESGSPacketType pktType, BOOL isCrypt, int targetId, int userId, long seq);
	int   OnAsyncProcessDataPacket(const void* data, int dataBytes, int targetId, unsigned int userId, ESGSPacketType pktType, int isCrypt, long* seq);

private:
	static void ReconnectTimer(void* param, TTimerID id);
	void OnReconnect();

	static void HeartbeatTimer(void* param, TTimerID id);
	void OnHeartbeat();

private:
	fnDataCallback m_dataCallback;
	void* m_context;
	fnEventCallback m_evtCallback;
	void* m_evtCallbackContext;

	std::string m_gwsIp;
	int m_gwsPort;
	DWORD m_connTimeout;
	DWORD m_heartbeatInterval;
	DWORD m_writeTimeout;
	DWORD m_readTimeout;
	EServerType   m_serverType;
	UINT32 m_sessionId;
	long  m_cmdSeq;
	BOOL  m_isLogin;

	CXTLocker m_locker;
	GWSClientSession* m_localSession;

	DWORD m_recvHeartTick;
	CyaCoreTimer m_heartbeatTimer;
	CyaCoreTimer m_reconnectTimer;
	SGWSProtocolHead m_heartbeatData;
};

#endif