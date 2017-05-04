#ifndef __TCP_SESSION_H__
#define __TCP_SESSION_H__

#include <map>

#include "cyaTcp.h"
#include "DBSProtocol.h"

class DBTcpSession : public CyaTcpSessionTmpl
{
public:
	DBTcpSession();
	~DBTcpSession();

	virtual void  OnConnected(SOCKET sock, const IPPADDR& peerIPP, int sessionCount, void* param = NULL);

	virtual void OnClosed(int cause);

	virtual int  RequestReceive(void*& buf, int& bufLen, IoReadType& rt, BOOL& asynNotify);

	virtual int  OnReceived(void* buf, int bufLen, BOOL callbackByTask, BOOL& continueRecv);

	int OnSubmitCmdPacket(const void* data, int bufLen, bool lastPacket);

private:
	typedef enum
	{
		RecvHead = 0,
		RecvBody = 1
	} ERECV_TYPE;

	ERECV_TYPE		m_eRecvType;
	SDBSProtocolHead	m_sProHead;

	char* m_msgData;
	unsigned long m_msgOffet;
};


#endif