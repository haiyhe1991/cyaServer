#ifndef __DAEMON_SESSION_H__
#define __DAEMON_SESSION_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaTypes.h"
#include "cyaTcp.h"
#include "ServiceProtocol.h"
#include "ProcessManager.h"
#include "GWSClient.h"

class CDaemonSession : public CyaTcpSessionTmpl
{
public:
	CDaemonSession();
	~CDaemonSession();

	virtual void OnConnected(SOCKET sock, const IPPADDR& peerIPP, int sessionCount, void* param = NULL);
	virtual void OnConnectedSend();
	virtual void OnClosed(int cause);

	virtual int  RequestReceive(void*& buf, int& bufLen, IoReadType& rt, BOOL& asynNotify);
	virtual int  OnReceived(void* buf, int bufLen, BOOL callbackByTask, BOOL& continueRecv);

	int ResponseSessionMsg(const void* data, int dataLen, UINT16 cmdCode);
private:
	typedef enum
	{
		SGS_HEAD = 0,
		SGS_BODY
	} SGS_DATA_TYPE;
	SGS_DATA_TYPE m_dataType;
	CProcessManager* m_msgProcMgr;
	CGWSClient* m_gwClient;
	UINT32 m_procId;
	SGSProtocolHead m_sgsHead;
};


#endif