#ifndef __DBS_SESSION_H__
#define __DBS_SESSION_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
#pragma once
#endif	

#include "cyaTcp.h"
#include "DBSprotocol.h"

class DBSHelper;
class DBSSessionMaker:public ICyaTcpSessionMaker
{
public:
	DBSSessionMaker();
	~DBSSessionMaker();

public:
	virtual int GetSessionObjSize();
	virtual void MakeSessionObj(void* session);
};
class DBSSession:public CyaTcpSessionTmpl
{
public:
	DBSSession();
	~DBSSession();

public:
	virtual void OnConnected(SOCKET sock, const IPPADDR& peerIPP, int sessionCount, void* param = NULL);
	virtual void OnClosed(int cause);
	virtual int  RequestReceive(void*& buf, int& bufLen, IoReadType& rt, BOOL& asynNotify);
	virtual int  OnReceived(void* buf, int bufLen, BOOL callbackByTask, BOOL& continueRecv);

private:
	enum RecvStatus
	{
		read_header,
		read_body
	};

	void* m_bodyBuf;
	RecvStatus m_recvStatus;
	DBSHelper* m_dbsHelper;
	SDBSProtocolHead m_header;

};

#endif