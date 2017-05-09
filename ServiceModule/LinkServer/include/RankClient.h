#ifndef __RANK_CLIENT_H__
#define __RANK_CLIENT_H__

#include <vector>
#include "BaseClient.h"

class RankClient : public BaseClient
{
public:
	RankClient();
	~RankClient();

	/// �ͻ��������������
	virtual int ClientMsg(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt);
	/// ���ط��������ݻص����
	virtual void WGSMsg(SGWSProtocolHead* pHead, UINT16 cmdCode, UINT16 retCode, const void* payload, int payloadLen);

private:
	/// ������Ϣ��ѯ����
	int OnQueryRoleInfo(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt);
	/// ��ѯ��ǰ��ҵ�ĳ��������Ϣ
	int OnQueryRoleRank(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt);

};

#endif