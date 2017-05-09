#ifndef __RANK_CLIENT_H__
#define __RANK_CLIENT_H__

#include <vector>
#include "BaseClient.h"

class RankClient : public BaseClient
{
public:
	RankClient();
	~RankClient();

	/// 客户端请求数据入口
	virtual int ClientMsg(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt);
	/// 网关服务器数据回调入口
	virtual void WGSMsg(SGWSProtocolHead* pHead, UINT16 cmdCode, UINT16 retCode, const void* payload, int payloadLen);

private:
	/// 排行信息查询请求
	int OnQueryRoleInfo(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt);
	/// 查询当前玩家的某项排行信息
	int OnQueryRoleRank(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt);

};

#endif