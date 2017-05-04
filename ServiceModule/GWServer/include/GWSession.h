#ifndef __GW_SESSION_H__
#define __GW_SESSION_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
网关转发会话
*/

#include "cyaTcp.h"
#include "GWSProtocol.h"

class GWSessionMaker : public ICyaTcpSessionMaker
{
public:
	GWSessionMaker();
	~GWSessionMaker();

	virtual int GetSessionObjSize();
	virtual void MakeSessionObj(void* session);
};

class GWSesssion : public CyaTcpSessionTmpl
{
public:
	GWSesssion();
	~GWSesssion();

	virtual void OnConnected(SOCKET sock, const IPPADDR& peerIPP, int sessionCount, void* param = NULL);
	virtual void OnClosed(int cause);
	virtual int  RequestReceive(void*& buf, int& bufLen, IoReadType& rt, BOOL& asynNotify);
	virtual int  OnReceived(void* buf, int bufLen, BOOL callbackByTask, BOOL& continueRecv);

	void UpdateHeartbeatTick()
	{
		m_heartbeatTick = GetTickCount();
	}

	DWORD GetLastHeartbeatTick() const
	{
		return m_heartbeatTick;
	}

	UINT32 GetSessionId() const
	{
		return m_sessionId;
	}

	BYTE GetLinkerId() const
	{
		return m_linkerId;
	}

	void SetLinkerId(BYTE linkerId)
	{
		m_linkerId = linkerId;
	}

	void SetSessionId(UINT32 sessionId)
	{
		m_sessionId = sessionId;
	}

private:
	int OnProcessData(const void* payloadData, int payLen);

	int OnReplyConfirmRequest(const void* payloadData, int payLen);
	int OnReplyHeartbeatRequest(const void* payloadData, int payLen);
	int OnReplyDataPacketRequest(const void* payloadData, int payLen);
	int OnSendLinkerGroupDataPacket(const void* payloadData, int payLen);
	int OnReplyUnknownCmdRequest(const void* payloadData, int payLen);

private:
	SGWSProtocolHead m_protoHeader;
	enum EnumRecvStatus
	{
		recv_header,
		recv_body
	};
	EnumRecvStatus m_recvStatus;
	BYTE m_linkerId;
	UINT32 m_sessionId;
	DWORD m_heartbeatTick;
};

#endif