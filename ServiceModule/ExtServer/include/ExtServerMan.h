#ifndef __EXT_SERVER_MAN_H__
#define __EXT_SERVER_MAN_H__

#include "ExtServerSessionMaker.h"
#include "GWSSDK.h"


class ExtServerMan
{
public:
	ExtServerMan();
	~ExtServerMan();

	INT Initialize();

	static ExtServerMan* InstanceServerMan();
	static void DeleteThis();
	void PushSessionMsg(const void* msgData, INT msgLen, UINT16 cmdCode, UINT32 userId);
	void ResponseSessionMsg(const void* msgData, INT msgLen, void* theSession);
private:
	INT PreInitialize();

	static void GWSDataCallback(SGWSProtocolHead* pHead, const void* payload, int payloadLen, void* context);
	static void GWSEventCallback(EServerEvent evt, const void* data, int nDataLen, void* context);

	void OnGWSData(SGWSProtocolHead* pHead, const void* payload, int payloadLen);
	void OnGWSEvent(EServerEvent evt, const void* data, int nDataLen);
private:
	ICyaTcpServeMan* m_tcpMan;
	ESTcpSessionMaker* m_maker;
	GWSHandle m_gwsHandle;
	BOOL m_gwsConnected;
	static ExtServerMan* m_instance;
};

#endif