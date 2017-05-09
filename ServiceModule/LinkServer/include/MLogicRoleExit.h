#ifndef __MLOGIC_ROLE_EXIT_H__
#define __MLOGIC_ROLE_EXIT_H__

/// 客户端只需要命令码 + UserID + RoleID，不需要数据结构

#include "IMLogic.h"

class MLogicRoleExit : public IMLogicClient
{
public:
	MLogicRoleExit();
	~MLogicRoleExit();

	virtual int ProcessMsg(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID);
};

#endif