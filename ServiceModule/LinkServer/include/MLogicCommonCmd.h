#ifndef __ML_LOGIC_COMMON_CMD_H__
#define __ML_LOGIC_COMMON_CMD_H__

/// 只需要在客户端数据一个命令码
#include "IMLogic.h"

class MLogicCommonCmd : public IMLogicClient
{
public:
	MLogicCommonCmd();
	~MLogicCommonCmd();

	virtual int ProcessMsg(GWSHandle gwsHandle, const void* pMsgData, int nMsgLen, UINT16 nCmdCode, UINT16 nPktType, UINT32 nUserID);
};

#endif