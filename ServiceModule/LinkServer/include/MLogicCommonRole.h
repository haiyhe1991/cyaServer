#ifndef __MLOGIC_COMMON_ROLE_H__
#define __MLOGIC_COMMON_ROLE_H__

/// 在客户端请求数据前面增加角色ID

#include "IMLogic.h"

class MLogicCommonRole : public IMLogicClient
{
public:
	MLogicCommonRole();
	~MLogicCommonRole();

	virtual int ProcessMsg(GWSHandle gwsHandle, const void* pMsgData, int nMsgLen, UINT16 nCmdCode, UINT16 nPktType, UINT32 nUserID);
};

#endif