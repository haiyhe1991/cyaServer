#ifndef __ML_LOGIC_COMMON_EX_H__
#define __ML_LOGIC_COMMON_EX_H__

/// 在客户端请求前面增加UserID和RoleID

#include "IMLogic.h"

class MLogicCommonEx : public IMLogicClient
{
public:
	MLogicCommonEx();
	~MLogicCommonEx();

	virtual int ProcessMsg(GWSHandle gwsHandle, const void* pMsgData, int nMsgLen, UINT16 nCmdCode, UINT16 nPktType, UINT32 nUserID);
};

#endif