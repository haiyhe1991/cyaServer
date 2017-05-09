#ifndef __ML_LOGIC_COMMON_H__
#define __ML_LOGIC_COMMON_H__

/// 客户端只需要命令码 + UserID + RoleID，不需要数据结构

#include "IMLogic.h"

class MLogicCommon : public IMLogicClient
{
public:
	MLogicCommon();
	~MLogicCommon();

	virtual int ProcessMsg(GWSHandle gwsHandle, const void* pMsgData, int nMsgLen, UINT16 nCmdCode, UINT16 nPktType, UINT32 nUserID);
};


#endif