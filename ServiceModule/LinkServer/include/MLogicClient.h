#ifndef __MLOGIC_CLIENT_H__
#define __MLOGIC_CLIENT_H__

#include "BaseClient.h"
#include "GWSSDK.h"

class MLogicClient : public BaseClient
{
public:
	MLogicClient();
	~MLogicClient();

	virtual int ClientMsg(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt);
	virtual void WGSMsg(SGWSProtocolHead* pHead, UINT16 cmdCode, UINT16 retCode, const void* payload, int payloadLen);
};

#endif