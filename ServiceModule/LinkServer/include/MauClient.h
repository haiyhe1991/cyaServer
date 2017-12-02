#ifndef __MAU_CLIENT_H__
#define __MAU_CLIENT_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include <vector>
#include "BaseClient.h"

class MauClient : public BaseClient
{
public:
	MauClient();
	~MauClient();

	/// �ͻ��������������
	virtual int ClientMsg(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt);
	/// ���ط��������ݻص����
	virtual void WGSMsg(SGWSProtocolHead* pHead, UINT16 cmdCode, UINT16 retCode, const void* payload, int payloadLen);

private:
	/// ���󴴽�����
	int OnMauCreateTable(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt);
	/// �����������
	int OnMauJoinTable(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt);
	/// ����׼��
	int OnMauReadyTable(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt);
	/// �����뿪����
	int OnMauLeaveTable(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt);
	/// �û��˳�
	int OnMauUserExit(GWSHandle gwsHandle, UINT16 pktType, UINT32 userID, bool encrypt);

private:
	/// ���ط�����������
	void OnRecvMauMsg(UINT16 pktType, UINT16 cmdCode, UINT16 retCode, const void* payload, int payloadLen);
	/// ��ȡ����Ŀ�꣬��������������Ŀ��֮�󣬷���ֵΪ����������Ҫ��ƫ����
	UINT16 GetMauTarget(const void* payload, int payloadLen, std::vector<UINT32>&);
};

#endif