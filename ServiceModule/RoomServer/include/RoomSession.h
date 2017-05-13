#ifndef __ROOM_SESSION_H__
#define __ROOM_SESSION_H__

#include "cyaTcp.h"

class CRoomSession : public CyaTcpSessionTmpl
{
public:
	CRoomSession();
	~CRoomSession();

	virtual void OnConnected(SOCKET sock, const IPPADDR& peerIPP, int sessionCount, void* param = NULL);
	virtual void OnConnectedSend();
	virtual void OnClosed(int cause);

	virtual int  RequestReceive(void*& buf, int& bufLen, IoReadType& rt, BOOL& asynNotify);
	virtual int  OnReceived(void* buf, int bufLen, BOOL callbackByTask, BOOL& continueRecv);
private:
};

#endif