#ifndef __EXT_SERVER_TCP_SESSION_H__
#define __EXT_SERVER_TCP_SESSION_H__

#include "cyaTcp.h"
#include "ServiceProtocol.h"

class ESTcpSession : public CyaTcpSessionTmpl
{
public:
	ESTcpSession();
	~ESTcpSession();

	virtual void OnConnected(SOCKET sock, const IPPADDR& peerIPP, int sessionCount, void* param = NULL);

	virtual void OnConnectedSend();

	virtual void OnClosed(int cause);

	virtual int  RequestReceive(void*& buf, int& bufLen, IoReadType& rt, BOOL& asynNotify);

	virtual int  OnReceived(void* buf, int bufLen, BOOL callbackByTask, BOOL& continueRecv);

	//virtual void  OnSent(const void* buf, int bufLen, BOOL isCopy, void* context);
private:
	typedef enum
	{
		PACKET_HEAD = 0,
		PACKET_BODY = 1
	} SESSION_PACKET_TYPE;

	SESSION_PACKET_TYPE m_recv_type;
	SGSProtocolHead m_proHeader;
};

#endif