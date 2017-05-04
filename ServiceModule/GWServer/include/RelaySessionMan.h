#ifndef __RELAY_SESSION_MAN_H__
#define __RELAY_SESSION_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
转发会话管理
*/

#include <map>
#include "cyaLocker.h"
#include "cyaSock.h"
#include "cyaCoreTimer.h"

class GWSesssion;
class RelaySessionMan
{
	class RelaySessionSite
	{
	public:
		RelaySessionSite();
		~RelaySessionSite();

		int AddSession(UINT32 sessionId, GWSesssion* session);
		void DelSession(UINT32 sessionId);

		int  SendSessionData(UINT32 sessionId, const void* data, int dataLen);
		int  SendSessionDataVec(UINT32 sessionId, const SockBufVec* bufs, int bufsCount);
		int  SendLinkerGroupDataPacket(const SockBufVec* bufs, int bufsCount);

		void HeartbeatCheck();

	private:
		CXTLocker m_locker;
		typedef std::map<UINT32/*session id*/, GWSesssion*> RelaySessionMap;
		RelaySessionMap m_relaySessionMap;
		UINT32 m_nextCallSessionId;
	};

public:
	RelaySessionMan();
	~RelaySessionMan();

	int AddRelaySession(BYTE serverId, UINT32 sessionId, GWSesssion* session);
	void DelRelaySession(BYTE serverId, UINT32 sessionId);

	int  SendRelaySessionData(BYTE serverId, UINT32 sessionId, const void* data, int dataLen);
	int  SendRelaySessionDataVec(BYTE serverId, UINT32 sessionId, const SockBufVec* bufs, int bufsCount);
	int  SendLinkerGroupDataPacket(const SockBufVec* bufs, int bufsCount);

private:
	RelaySessionSite* GetRelaySessionSite(BYTE serverId);
	static void HeartbeatCheckTimer(void* param, TTimerID id);
	void OnHeartbeatCheck();

private:
	typedef std::map<BYTE/*serverId*/, RelaySessionSite*> RelaySessionSiteMap;
	RelaySessionSiteMap m_relaySessionsSiteMap;
	CyaCoreTimer m_heartbeatTimer;
};

void InitRelaySessionMan();
RelaySessionMan* GetRelaySessionMan();
void DestroyRelaySessionMan();

#endif