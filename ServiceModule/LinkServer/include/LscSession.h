#ifndef __LSC_SESSION_H__
#define __LSC_SESSION_H__

#include "cyaTcp.h"
#include "ServiceProtocol.h"

typedef int(*LscClientMsgCallBack)(bool encrypt, UINT16 pktType, void* msgData, int msgLen, void* context);

class LSCSession : public CyaTcpSessionTmpl
{
public:
	LSCSession();
	~LSCSession();

	virtual void OnConnected(SOCKET sock, const IPPADDR& peerIPP, int sessionCount, void* param = NULL);
	virtual void OnClosed(int cause);
	virtual int  RequestReceive(void*& buf, int& bufLen, IoReadType& rt, BOOL& asynNotify);
	virtual int  OnReceived(void* buf, int bufLen, BOOL callbackByTask, BOOL& continueRecv);

	void SetLscClientCallBack(LscClientMsgCallBack lscFunc, void* lscContext);
private:
	typedef enum {
		RECV_HEAD = 0,
		RECV_BODY
	} RECV_STATUS;
	RECV_STATUS m_recv_status;
	SGSProtocolHead m_pro_head;
	LscClientMsgCallBack m_LscCallBack;
	void* m_LscContext;
};

#endif