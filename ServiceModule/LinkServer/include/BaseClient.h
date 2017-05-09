/*
������Ϣ�������
��ͬ�ķ�������Ϣ�ò�ͬ�Ļ���
ÿ�������Ӧһ����������
*/
#ifndef __BASE_CLIENT_H__
#define __BASE_CLIENT_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "GWSSDK.h"
#include "GWSProtocol.h"

struct BaseClient
{
	virtual ~BaseClient() {}
	/// ������д����Ϣ��GWS��Ϣ�쳣д�룬�����ʱ
	virtual int ClientMsg(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt) = 0;
	/// ������Ϣ�ص�
	virtual void WGSMsg(SGWSProtocolHead* pHead, UINT16 cmdCode, UINT16 retCode, const void* payload, int payloadLen) = 0;
};

#endif ///_SGS_LINKSERVER_BASECLIENT_H_