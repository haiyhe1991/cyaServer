#ifndef __MLOGIC_COMMON_EX_PART_H__
#define __MLOGIC_COMMON_EX_PART_H__

/// 在客户端请求前面增加UserID、RoleID和PartId

#include "IMLogic.h"

class MLogicCommonExPart : public IMLogicClient
{
public:
	MLogicCommonExPart();
	~MLogicCommonExPart();

	virtual int ProcessMsg(GWSHandle gwsHandle, const void* pMsgData, int nMsgLen, UINT16 nCmdCode, UINT16 nPktType, UINT32 nUserID);
};

#endif