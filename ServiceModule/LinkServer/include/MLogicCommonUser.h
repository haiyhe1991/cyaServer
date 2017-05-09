#ifndef __MLOGIC_COMMON_USER_H__
#define __MLOGIC_COMMON_USER_H__

/// 在客户端数据前面增加一个UserID

#include "IMLogic.h"

class MLogicCommonUser : public IMLogicClient
{
public:
	MLogicCommonUser();
	~MLogicCommonUser();

	virtual int ProcessMsg(GWSHandle gwsHandle, const void* pMsgData, int nMsgLen, UINT16 nCmdCode, UINT16 nPktType, UINT32 nUserID);
};


#endif