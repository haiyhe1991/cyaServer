#ifndef __ML_LOGIC_COMMON_CMD_H__
#define __ML_LOGIC_COMMON_CMD_H__

/// ֻ��Ҫ�ڿͻ�������һ��������
#include "IMLogic.h"

class MLogicCommonCmd : public IMLogicClient
{
public:
	MLogicCommonCmd();
	~MLogicCommonCmd();

	virtual int ProcessMsg(GWSHandle gwsHandle, const void* pMsgData, int nMsgLen, UINT16 nCmdCode, UINT16 nPktType, UINT32 nUserID);
};

#endif