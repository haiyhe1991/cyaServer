#ifndef __GWS_CLIENT_SESSION_H__
#define __GWS_CLIENT_SESSION_H__

#include "cyaTcp.h"
#include "GWSProtocol.h"

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

class GWSClientSessionMaker : public ICyaTcpSessionMaker
{
public:
	GWSClientSessionMaker();
	~GWSClientSessionMaker();

	virtual int GetSessionObjSize();
	virtual void MakeSessionObj(void* session);
};

class GWSHelper;
class GWSClientSession : public CyaTcpSessionTmpl
{
public:
	GWSClientSession();
	~GWSClientSession();

	virtual void OnConnected(SOCKET sock, const IPPADDR& peerIPP, int sessionCount, void* param = NULL);
	virtual void OnClosed(int cause);
	virtual int  RequestReceive(void*& buf, int& bufLen, IoReadType& rt, BOOL& asynNotify);
	virtual int  OnReceived(void* buf, int bufLen, BOOL callbackByTask, BOOL& continueRecv);

public:
	void SetGWSHelper(GWSHelper* gwsHelper)
	{
		m_gwsHelper = gwsHelper;
	}

private:
	SGWSProtocolHead m_protoHeader;
	enum GWSClientRecvStatus
	{
		recv_data_header,
		recv_data_body
	};
	GWSClientRecvStatus m_status;
	GWSHelper* m_gwsHelper;
};


#endif