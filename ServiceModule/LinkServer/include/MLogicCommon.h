#ifndef __ML_LOGIC_COMMON_H__
#define __ML_LOGIC_COMMON_H__

/// �ͻ���ֻ��Ҫ������ + UserID + RoleID������Ҫ���ݽṹ

#include "IMLogic.h"

class MLogicCommon : public IMLogicClient
{
public:
	MLogicCommon();
	~MLogicCommon();

	virtual int ProcessMsg(GWSHandle gwsHandle, const void* pMsgData, int nMsgLen, UINT16 nCmdCode, UINT16 nPktType, UINT32 nUserID);
};


#endif