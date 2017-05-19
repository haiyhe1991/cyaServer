#ifndef __GWS_CLIENT_H__
#define __GWS_CLIENT_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
���ط��������ݲ���������
*/

#include "GWSSDK.h"

class GWSClient
{
public:
	GWSClient();
	~GWSClient();

	int  Login();
	void Logout();

private:
	static void GWSDataCallback(SGWSProtocolHead* pHead, const void* payload, int payloadLen, void* context);
	void OnGWSDataCallback(SGWSProtocolHead* pHead, const void* payload, int payloadLen);

public:
	//���ͻ����ظ�
	int SendBasicResponse(SGWSProtocolHead* pHead, UINT16 retCode, UINT16 cmdCode);
	//���ͻ����ظ�
	int SendBasicResponse(BYTE targetId, UINT32 userId, UINT16 retCode, UINT16 cmdCode);
	//���ͻظ�
	int SendResponse(SGWSProtocolHead* pHead, UINT16 retCode, UINT16 cmdCode, void* buf, int nLen);
	//���ͻظ�
	int SendResponse(BYTE targetId, UINT32 userId, UINT16 retCode, UINT16 cmdCode, void* buf, int nLen, BYTE pktType = DATA_PACKET, BYTE sourceId = MAIN_LOGIC_SERVER);

private:
	GWSHandle m_gwsHandle;
};

#endif	//_MLogicServerGWSClient_h__